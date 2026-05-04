#include <string.h>
#include <stdio.h>

#if defined(GC_BUILD)
#include <gcsound.h>
#endif

#include <audio/mixer.h>
#include <audio/WavDecoder.h>
// #include <audio/MP3Decoder.h>

Cmixer::Cmixer()
{
#if defined(GC_BUILD)
    masterVolume = MAX_VOLUME;
#else
    masterVolume = MAX_VOLUME;
    decoder = NULL;
#endif
}

Cmixer::~Cmixer()
{
#if defined(GC_BUILD)
    stopMusic();
#else
    stopMusic();

    if (decoder)
    {
        delete decoder;
        decoder = NULL;
    }
#endif
}

bool Cmixer::init(int freq, int channels, int bufferSize)
{
#if defined(GC_BUILD)
    GCSound_Init(); //INICIALIZA GC 
    return true;
#else
    //INICILIZA SDL 
    spec.freq = freq;
    spec.format = AUDIO_S16LSB;
    spec.channels = channels;
    spec.samples = bufferSize;
    spec.callback = audioCallback;
    spec.userdata = this;

    if (SDL_OpenAudio(&spec, NULL) < 0)
    {
        printf("Audio init error: %s\n", SDL_GetError());
        return false;
    }

    SDL_PauseAudio(0);
    return true;
#endif
}

/* ========================= */
/* UTILS */
/* ========================= */

bool Cmixer::isWav(const char *filename)
{
    const char *ext = strrchr(filename, '.');
    if (!ext)
        return false;
    return (strcasecmp(ext, ".wav") == 0);
}

bool Cmixer::isMp3(const char *filename)
{
    const char *ext = strrchr(filename, '.');
    if (!ext)
        return false;
    return (strcasecmp(ext, ".mp3") == 0);
}

/* ========================= */
/* MUSIC */
/* ========================= */

bool Cmixer::playMusic(const char *filename, bool loop)
{

#if defined(GC_BUILD)

    GCSound_PlayMusic(filename,0); //en 0 detecta la frecuencia 
    return true;
#else

    stopMusic();

    FS_FILE *fp = fs_open(filename, "rb");
    if (!fp)
    {
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
        return false;
    }
    else
    {
        printf("Unsupported format\n");
        fs_close(fp);
        return false;
    }

    if (!decoder)
    {
        fs_close(fp);
        return false;
    }

    if (!decoder->open(fp))
    {
        printf("Decoder open failed\n");
        fs_close(fp);
        delete decoder;
        decoder = NULL;
        return false;
    }

    if (!musicStream.open(decoder, loop))
    {
        delete decoder;
        decoder = NULL;
        return false;
    }

    musicStream.play();
    return true;
#endif
}

///////////////////////////////////////////////////////////////////////

void Cmixer::stopMusic()
{
#if defined(GC_BUILD)
    GCSound_StopMusic();
#else
    musicStream.stop();

    if (decoder)
    {
        delete decoder;
        decoder = NULL;
    }
#endif
}

/* ========================= */
/* SFX */
/* ========================= */

int Cmixer::playChannel(CSample *s, bool loop, int volume, int channel)
{

 #if defined(GC_BUILD)
    // Ajuste de volumen para GCSound (0-127)
    int vol_gc = (volume > 127) ? 127 : volume;
    GCSound_PlaySample(s->sample, vol_gc, vol_gc);
    return 0;
#else   

    if (!s || !s->getData())
        return -1;

    if (volume < 0) volume = 0;
    if (volume > MAX_VOLUME) volume = MAX_VOLUME;

    // canal específico
    if (channel >= 0 && channel < MAX_CHANNELS)
    {
        channels[channel] = *s;
        channels[channel].setLoop(loop);
        channels[channel].setVolume(volume);
        channels[channel].setActive(true);
        channels[channel].setPosition(0);
        return channel;
    }

    // automático
    for (int i = 0; i < MAX_CHANNELS; i++)
    {
        if (!channels[i].getActive())
        {
            channels[i] = *s;
            channels[i].setLoop(loop);
            channels[i].setVolume(volume);
            channels[i].setActive(true);
            channels[i].setPosition(0);
            return i;
        }
    }

    return -1;
#endif
}

void Cmixer::stopChannel(int id)
{
    if (id < 0 || id >= MAX_CHANNELS)
        return;

    channels[id].setActive(false);
}

void Cmixer::stopAll()
{
    for (int i = 0; i < MAX_CHANNELS; i++)
        channels[i].setActive(false);
}

void Cmixer::setMasterVolume(int vol)
{
if (vol < 0) vol = 0;
    if (vol > MAX_VOLUME) vol = MAX_VOLUME;
    masterVolume = vol;

#if defined(GC_BUILD)
    // Esto actualizará la música que está sonando actualmente en la SD
    GCSound_SetMusicVolume(vol, vol); 
#endif
}

/* ========================= */
/* MIX */
/* ========================= */

void Cmixer::mix(u8 *output, int len)
{
    Sint16 *out = (Sint16 *)output;
    int totalSamples = len / sizeof(Sint16);

    memset(out, 0, len);

    /* ===== SFX ===== */
    for (int ch = 0; ch < MAX_CHANNELS; ch++)
    {
        if (!channels[ch].getActive())
            continue;

        Sint16 *data = (Sint16 *)channels[ch].getData();
        int pos = channels[ch].getPosition();
        int size = channels[ch].getSize() / sizeof(Sint16);

        int vol = (channels[ch].getVolume() * masterVolume) / MAX_VOLUME;

        for (int i = 0; i < totalSamples; i++)
        {
            if (pos >= size)
            {
                if (channels[ch].getLoop())
                    pos = 0;
                else
                {
                    channels[ch].setActive(false);
                    break;
                }
            }

            int val = data[pos++];
            val = (val * vol) / MAX_VOLUME;

            int mixed = out[i] + val;

            if (mixed > 32767) mixed = 32767;
            if (mixed < -32768) mixed = -32768;

            out[i] = (Sint16)mixed;
        }

        channels[ch].setPosition(pos);
    }

    /* ===== MUSIC ===== */
    static u8 temp[16384]; // 🔥 seguro

    if (len > (int)sizeof(temp))
        len = sizeof(temp);

    int read = musicStream.read(temp, len);

    Sint16 *mdata = (Sint16 *)temp;
    int musicSamples = read / sizeof(Sint16);

    for (int i = 0; i < totalSamples && i < musicSamples; i++)
    {
        int val = mdata[i];
        val = (val * masterVolume) / MAX_VOLUME;

        int mixed = out[i] + val;

        if (mixed > 32767) mixed = 32767;
        if (mixed < -32768) mixed = -32768;

        out[i] = (Sint16)mixed;
    }
}

/* ========================= */
/* CALLBACK */
/* ========================= */

void Cmixer::audioCallback(void *userdata, u8 *stream, int len)
{
    Cmixer *m = (Cmixer *)userdata;
    m->mix(stream, len);
}