#include <string.h>
#include <stdio.h>
#include <math.h>
#include <audio/mixer.h>
#include <audio/WavDecoder.h>
// #include <audio/MP3Decoder.h>

Cmixer::Cmixer()
{
    masterVolume = MAX_VOLUME;
    musicVolume = MAX_VOLUME;
    decoder = NULL;
    mutex = createMutex();
    
    musicPaused = false;
    isFadingOut = false;
    fadeTotalSamples = 0;
    fadeCurrentSample = 0;
    volumeBeforeFade = MAX_VOLUME;

    // Inicialización del Ducking Global
    currentDuckingMultiplier = 1.0f;
    targetDuckingMultiplier = 1.0f;
    duckingSpeed = 0.05f;
    autoRestoreSpeed = 0.04f;
    
    // Inicialización del canal de Notificaciones
    notificationChannelActive = false;
    notificationChannelLoop = false;
    notificationChannelVolume = MAX_VOLUME;
    notificationChannelPos = 0;
}

Cmixer::~Cmixer()
{
    stopMusic();

    if (decoder)
    {
        delete decoder;
        decoder = NULL;
    }

    if (mutex)
    {
        delete mutex;
        mutex = NULL;
    }
}

bool Cmixer::init(int freq, int channels_count, int bufferSize)
{
    spec.freq = freq;
    spec.channels = channels_count;
    spec.samples = bufferSize;
    spec.callback = audioCallback;
    spec.userdata = this;

#if defined(GC_BUILD)
    spec.format = AUDIO_S16MSB;
#else
    spec.format = AUDIO_S16LSB;
#endif

    if (SDL_OpenAudio(&spec, NULL) < 0)
    {
        printf("Audio init error: %s\n", SDL_GetError());
        return false;
    }

    SDL_PauseAudio(0);
    return true;
}

/* ======================================================================== */
/* MUSIC (Protegidas con Mutex)                                             */
/* ======================================================================== */

bool Cmixer::playMusic(const char *filename, bool loop)
{
    if (mutex) mutex->lock();

    stopMusic();
    musicPaused = false;
    isFadingOut = false;

    FS_FILE *fp = fs_open(filename, "rb");
    if (!fp)
    {
        if (mutex) mutex->unlock();
        printf("Error opening file\n");
        return false;
    }

    if (isWav(filename))
    {
        decoder = new WavDecoder();
    }
    else if (isMp3(filename))
    {
        printf("mp3 no implementado\n");
        fs_close(fp);
        if (mutex) mutex->unlock();
        return false;
    }
    else
    {
        printf("Unsupported format\n");
        fs_close(fp);
        if (mutex) mutex->unlock();
        return false;
    }

    if (!decoder)
    {
        fs_close(fp);
        if (mutex) mutex->unlock();
        return false;
    }

    if (!decoder->open(fp))
    {
        printf("Decoder open failed\n");
        fs_close(fp);
        delete decoder;
        decoder = NULL;
        if (mutex) mutex->unlock();
        return false;
    }

    if (!musicStream.open(decoder, loop))
    {
        delete decoder;
        decoder = NULL;
        if (mutex) mutex->unlock();
        return false;
    }

    musicStream.play();
    
    if (mutex) mutex->unlock();
    return true;
}

void Cmixer::stopMusic()
{
    musicStream.stop();

    if (decoder)
    {
        delete decoder;
        decoder = NULL;
    }
}

void Cmixer::pauseMusic()
{
    if (mutex) mutex->lock();
    musicPaused = true;
    if (mutex) mutex->unlock();
}

void Cmixer::resumeMusic()
{
    if (mutex) mutex->lock();
    musicPaused = false;
    if (mutex) mutex->unlock();
}

void Cmixer::setMusicVolume(int vol)
{
    if (mutex) mutex->lock();
    if (vol < 0) vol = 0;
    if (vol > MAX_VOLUME) vol = MAX_VOLUME;
    musicVolume = vol;
    if (mutex) mutex->unlock();
}

void Cmixer::fadeOutMusic(int ms)
{
    if (mutex) mutex->lock();

    if (ms <= 0)
    {
        stopMusic();
        isFadingOut = false;
        if (mutex) mutex->unlock();
        return;
    }

    long samplesPerSec = spec.freq;
    fadeTotalSamples = (samplesPerSec * ms) / 1000;
    fadeCurrentSample = 0;
    
    volumeBeforeFade = musicVolume;
    isFadingOut = true;
    musicPaused = false;

    if (mutex) mutex->unlock();
}

/* ======================================================================== */
/* SFX (Canales protegidos con Mutex)                                       */
/* ======================================================================== */

int Cmixer::playChannel(CSample *s, bool loop, int volume, int channel)
{
    if (!s || !s->getData())
        return -1;

    if (volume < 0) volume = 0;
    if (volume > MAX_VOLUME) volume = MAX_VOLUME;

    if (mutex) mutex->lock();

    int target = -1;

    if (channel >= 0 && channel < MAX_CHANNELS)
    {
        target = channel;
    }
    else
    {
        for (int i = 0; i < MAX_CHANNELS; i++)
        {
            if (!channels[i].getActive())
            {
                target = i;
                break;
            }
        }
    }

    if (target == -1)
    {
        if (mutex) mutex->unlock();
        return -1;
    }

    channels[target].setData(s->getData());
    channels[target].setSize(s->getSize());
    channels[target].setLoop(loop);
    channels[target].setVolume(volume);
    channels[target].setPosition(0);
    channels[target].setActive(true);
    channels[target].setPaused(false);
    
    channelFading[target] = false;
    channelFadeCurrentSample[target] = 0;
    channelFadeTotalSamples[target] = 0;
    channelVolumeBeforeFade[target] = volume;

    if (mutex) mutex->unlock();
    return target;
}

void Cmixer::stopChannel(int id)
{
    if (id < 0 || id >= MAX_CHANNELS)
        return;

    if (mutex) mutex->lock();
    channels[id].setActive(false);
    channelFading[id] = false;
    if (mutex) mutex->unlock();
}

void Cmixer::stopAll()
{
    if (mutex) mutex->lock();
    for (int i = 0; i < MAX_CHANNELS; i++)
    {
        channels[i].setActive(false);
        channelFading[i] = false;
    }
    musicStream.stop();
    if (decoder) { delete decoder; decoder = NULL; }
    if (mutex) mutex->unlock();
}

void Cmixer::pauseChannel(int id)
{
    if (id < 0 || id >= MAX_CHANNELS) return;
    if (mutex) mutex->lock();
    channels[id].setPaused(true);
    if (mutex) mutex->unlock();
}

void Cmixer::resumeChannel(int id)
{
    if (id < 0 || id >= MAX_CHANNELS) return;
    if (mutex) mutex->lock();
    channels[id].setPaused(false);
    if (mutex) mutex->unlock();
}

void Cmixer::pauseAllChannels()
{
    if (mutex) mutex->lock();
    for (int i = 0; i < MAX_CHANNELS; i++) channels[i].setPaused(true);
    if (mutex) mutex->unlock();
}

void Cmixer::resumeAllChannels()
{
    if (mutex) mutex->lock();
    for (int i = 0; i < MAX_CHANNELS; i++) channels[i].setPaused(false);
    if (mutex) mutex->unlock();
}

void Cmixer::setChannelVolume(int id, int vol)
{
    if (id < 0 || id >= MAX_CHANNELS) return;
    if (vol < 0) vol = 0;
    if (vol > MAX_VOLUME) vol = MAX_VOLUME;
    if (mutex) mutex->lock();
    channels[id].setVolume(vol);
    if (mutex) mutex->unlock();
}

void Cmixer::fadeOutChannel(int id, int ms)
{
    if (id < 0 || id >= MAX_CHANNELS) return;

    if (mutex) mutex->lock();

    if (!channels[id].getActive())
    {
        if (mutex) mutex->unlock();
        return;
    }

    if (ms <= 0)
    {
        channels[id].setActive(false);
        channelFading[id] = false;
        if (mutex) mutex->unlock();
        return;
    }

    long samplesPerSec = spec.freq;
    channelFadeTotalSamples[id] = (samplesPerSec * ms) / 1000;
    channelFadeCurrentSample[id] = 0;
    channelVolumeBeforeFade[id] = channels[id].getVolume();
    channelFading[id] = true;

    if (mutex) mutex->unlock();
}

void Cmixer::fadeOutAllChannels(int ms)
{
    if (mutex) mutex->lock();
    for (int i = 0; i < MAX_CHANNELS; i++)
    {
        if (channels[i].getActive())
        {
            if (ms <= 0)
            {
                channels[i].setActive(false);
                channelFading[i] = false;
            }
            else
            {
                long samplesPerSec = spec.freq;
                channelFadeTotalSamples[i] = (samplesPerSec * ms) / 1000;
                channelFadeCurrentSample[i] = 0;
                channelVolumeBeforeFade[i] = channels[i].getVolume();
                channelFading[i] = true;
            }
        }
    }
    if (mutex) mutex->unlock();
}

void Cmixer::pauseAll()
{
    pauseAllChannels();
    pauseMusic();
}

void Cmixer::resumeAll()
{
    resumeAllChannels();
    resumeMusic();
}

void Cmixer::setMasterVolume(int vol)
{
    if (vol < 0) vol = 0;
    if (vol > MAX_VOLUME) vol = MAX_VOLUME;
    if (mutex) mutex->lock();
    masterVolume = vol;
    if (mutex) mutex->unlock();
}

void Cmixer::fadeOutAll(int ms)
{
    fadeOutAllChannels(ms);
    fadeOutMusic(ms);
}

int Cmixer::playNotification(CSample *s, bool loop, int volume, int targetVolumePercent, float duckSpeed, float restoreSpeed) {
    if (!s || !s->getData()) return -1;

    if (mutex) mutex->lock();

    // Configuramos el canal exclusivo de notificación
    notificationChannelSample = *s;
    notificationChannelLoop = loop;
    notificationChannelVolume = (volume < 0) ? 0 : (volume > MAX_VOLUME ? MAX_VOLUME : volume);
    notificationChannelPos = 0;
    notificationChannelActive = true;

    // Disparamos el Ducking global (afectando a la mezcla general de fondo)
    targetDuckingMultiplier = (float)targetVolumePercent / 100.0f;
    duckingSpeed = (duckSpeed > 0.0f) ? duckSpeed : 0.04f;
    autoRestoreSpeed = restoreSpeed;

    if (mutex) mutex->unlock();
    return 0;
}


/* ======================================================================== */
/* MIX (Sección crítica principal ejecutada por el hilo de audio)           */
/* ======================================================================== */

void Cmixer::mix(u8 *output, int len)
{
    if (mutex) mutex->lock();

    Sint16 *out = (Sint16 *)output;
    int totalSamples = len / sizeof(Sint16);

    memset(out, 0, len);

    // --- 1. VIGILANCIA DE FIN DE NOTIFICACIÓN Y RESTAURACIÓN DE DUCKING ---
    if (notificationChannelActive) {
        int nSize = notificationChannelSample.getSize() / sizeof(Sint16);
        // Si la posición actual ya rebasó o alcanzó el tamaño, la notificación terminó
        if (notificationChannelPos >= nSize) {
            if (!notificationChannelLoop) {
                notificationChannelActive = false;
                // Ordenamos restaurar el volumen general de música y SFX suavemente
                targetDuckingMultiplier = 1.0f;
                duckingSpeed = autoRestoreSpeed;
            } else {
                notificationChannelPos = 0;
            }
        }
    }

    /* 2. INTERPOLACIÓN Y CONTROL DE DUCKING */
    if (currentDuckingMultiplier != targetDuckingMultiplier) {
        currentDuckingMultiplier += (targetDuckingMultiplier - currentDuckingMultiplier) * duckingSpeed;
        
        if (fabsf(currentDuckingMultiplier - targetDuckingMultiplier) < 0.001f) {
            currentDuckingMultiplier = targetDuckingMultiplier;
        }
    }

    /* 3. SFX (Canales regulares 0 a 15) - Afectados por el Ducking Global */
    for (int ch = 0; ch < MAX_CHANNELS; ch++)
    {
        if (!channels[ch].getActive() || channels[ch].isPaused())
            continue;

        Sint16 *data = (Sint16 *)channels[ch].getData();
        int pos = channels[ch].getPosition();
        int size = channels[ch].getSize() / sizeof(Sint16);

        for (int i = 0; i < totalSamples; i++)
        {
            if (pos >= size)
            {
                if (channels[ch].getLoop())
                    pos = 0;
                else
                {
                    channels[ch].setActive(false);
                    channelFading[ch] = false;
                    break;
                }
            }

            if (!data || pos < 0 || pos >= size)
            {
                channels[ch].setActive(false);
                channelFading[ch] = false;
                break;
            }

            int currentVol = channels[ch].getVolume();

            if (channelFading[ch])
            {
                if (channelFadeCurrentSample[ch] >= channelFadeTotalSamples[ch])
                {
                    channels[ch].setActive(false);
                    channelFading[ch] = false;
                    break;
                }

                float progress = (float)channelFadeCurrentSample[ch] / (float)channelFadeTotalSamples[ch];
                currentVol = (int)(channelVolumeBeforeFade[ch] * (1.0f - progress));
                if (currentVol < 0) currentVol = 0;

                channelFadeCurrentSample[ch]++;
            }

            int vol = (int)(((currentVol * masterVolume) / MAX_VOLUME) * currentDuckingMultiplier);
            int val = data[pos++];

#if defined(GC_BUILD)
            val = (Sint16)(((val & 0xFF) << 8) | ((val >> 8) & 0xFF));
#endif

            val = (val * vol) / MAX_VOLUME;

#if defined(GC_BUILD)
            Sint16 current_out = (Sint16)(((out[i] & 0xFF) << 8) | ((out[i] >> 8) & 0xFF));
            int mixed = current_out + val;
#else
            int mixed = out[i] + val;
#endif

            if (mixed > 32767) mixed = 32767;
            if (mixed < -32768) mixed = -32768;

#if defined(GC_BUILD)
            out[i] = (Sint16)(((mixed & 0xFF) << 8) | ((mixed >> 8) & 0xFF));
#else
            out[i] = (Sint16)mixed;
#endif
        }

        channels[ch].setPosition(pos);
    }

    /* 4. MUSIC - Afectada por el Ducking Global de forma limpia */
    static u8 temp[16384] __attribute__((aligned(32)));
    int readLen = (len > (int)sizeof(temp)) ? sizeof(temp) : len;

    if (musicPaused)
    {
        // Música pausada, no se mezcla nada
    }
    else if (isFadingOut)
    {
        int read = musicStream.read(temp, readLen);
        Sint16 *mdata = (Sint16 *)temp;
        int musicSamples = read / sizeof(Sint16);

        for (int i = 0; i < totalSamples && i < musicSamples; i++)
        {
            if (fadeCurrentSample >= fadeTotalSamples)
            {
                stopMusic();
                isFadingOut = false;
                break;
            }

            float progress = (float)fadeCurrentSample / (float)fadeTotalSamples;
            int fadedMusicVol = (int)(volumeBeforeFade * (1.0f - progress));
            if (fadedMusicVol < 0) fadedMusicVol = 0;

            fadeCurrentSample++;

            int val = mdata[i];

#if defined(GC_BUILD)
            val = (Sint16)(((val & 0xFF) << 8) | ((val >> 8) & 0xFF));
#endif

            int effectiveMusicVol = (int)(fadedMusicVol * currentDuckingMultiplier);
            val = (val * effectiveMusicVol * masterVolume) / (MAX_VOLUME * MAX_VOLUME);

#if defined(GC_BUILD)
            Sint16 current_out = (Sint16)(((out[i] & 0xFF) << 8) | ((out[i] >> 8) & 0xFF));
            int mixed = current_out + val;
#else
            int mixed = out[i] + val;
#endif

            if (mixed > 32767) mixed = 32767;
            if (mixed < -32768) mixed = -32768;

#if defined(GC_BUILD)
            out[i] = (Sint16)(((mixed & 0xFF) << 8) | ((mixed >> 8) & 0xFF));
#else
            out[i] = (Sint16)mixed;
#endif
        }
    }
    else
    {
        int read = musicStream.read(temp, readLen);
        Sint16 *mdata = (Sint16 *)temp;
        int musicSamples = read / sizeof(Sint16);

        for (int i = 0; i < totalSamples && i < musicSamples; i++)
        {
            int val = mdata[i];

#if defined(GC_BUILD)
            val = (Sint16)(((val & 0xFF) << 8) | ((val >> 8) & 0xFF));
#endif

            int effectiveMusicVol = (int)(musicVolume * currentDuckingMultiplier);
            val = (val * effectiveMusicVol * masterVolume) / (MAX_VOLUME * MAX_VOLUME);

#if defined(GC_BUILD)
            Sint16 current_out = (Sint16)(((out[i] & 0xFF) << 8) | ((out[i] >> 8) & 0xFF));
            int mixed = current_out + val;
#else
            int mixed = out[i] + val;
#endif

            if (mixed > 32767) mixed = 32767;
            if (mixed < -32768) mixed = -32768;

#if defined(GC_BUILD)
            out[i] = (Sint16)(((mixed & 0xFF) << 8) | ((mixed >> 8) & 0xFF));
#else
            out[i] = (Sint16)mixed;
#endif
        }
    }

    /* 5. NOTIFICACIÓN EXCLUSIVA (Libre de atenuación, suena por encima de todo) */
    if (notificationChannelActive && notificationChannelSample.getData() != NULL) {
        Sint16 *nData = (Sint16 *)notificationChannelSample.getData();
        int nSize = notificationChannelSample.getSize() / sizeof(Sint16);

        for (int i = 0; i < totalSamples; i++) {
            if (notificationChannelPos >= nSize) {
                if (notificationChannelLoop) {
                    notificationChannelPos = 0;
                } else {
                    break;
                }
            }

            int nVol = (notificationChannelVolume * masterVolume) / MAX_VOLUME;
            int nVal = nData[notificationChannelPos++];

#if defined(GC_BUILD)
            nVal = (Sint16)(((nVal & 0xFF) << 8) | ((nVal >> 8) & 0xFF));
#endif
            nVal = (nVal * nVol) / MAX_VOLUME;

#if defined(GC_BUILD)
            Sint16 current_out = (Sint16)(((out[i] & 0xFF) << 8) | ((out[i] >> 8) & 0xFF));
            int mixed = current_out + nVal;
#else
            int mixed = out[i] + nVal;
#endif

            if (mixed > 32767) mixed = 32767;
            if (mixed < -32768) mixed = -32768;

#if defined(GC_BUILD)
            out[i] = (Sint16)(((mixed & 0xFF) << 8) | ((mixed >> 8) & 0xFF));
#else
            out[i] = (Sint16)mixed;
#endif
        }
    }

    if (mutex) mutex->unlock();
}
/* ======================================================================== */
/* UTILS & CALLBACK                                                         */
/* ======================================================================== */

bool Cmixer::isWav(const char *filename)
{
    int len = strlen(filename);
    if (len < 4) return false;
    if (strcasecmp(filename + len - 4, ".wav") == 0)
        return true;
    return false;
}

bool Cmixer::isMp3(const char *filename)
{
    int len = strlen(filename);
    if (len < 4) return false;
    if (strcasecmp(filename + len - 4, ".mp3") == 0)
        return true;
    return false;
}

void Cmixer::audioCallback(void *userdata, u8 *stream, int len)
{
    Cmixer *mixer = (Cmixer *)userdata;
    if (mixer)
    {
        mixer->mix(stream, len);
    }
}
