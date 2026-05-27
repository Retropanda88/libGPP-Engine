#include <string.h>
#include <stdio.h>
#include <audio/mixer.h>
#include <audio/WavDecoder.h>
// #include <audio/MP3Decoder.h>

Cmixer::Cmixer()
{
    masterVolume = MAX_VOLUME;
    decoder = NULL;
}

Cmixer::~Cmixer()
{
    stopMusic();

    if (decoder)
    {
        delete decoder;
        decoder = NULL;
    }
}

bool Cmixer::init(int freq, int channels, int bufferSize)
{
    // Configuración base de la especificación de audio de SDL
    spec.freq = freq;
    spec.channels = channels;
    spec.samples = bufferSize;
    spec.callback = audioCallback;
    spec.userdata = this;

    // FLAGS DE ARQUITECTURA: Asignar formato nativo según la consola
#if defined(GC_BUILD)
    spec.format = AUDIO_S16MSB; // GameCube (Big-Endian Nativo)
#else
    spec.format = AUDIO_S16LSB; // Android, PSP, PS2 (Little-Endian)
#endif

    if (SDL_OpenAudio(&spec, NULL) < 0)
    {
        printf("Audio init error: %s\n", SDL_GetError());
        return false;
    }

    SDL_PauseAudio(0); // Despierta tu driver de audio asíncrono
    return true;
}

/* ======================================================================== */
/* UTILS                                                                    */
/* ======================================================================== */

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

/* ======================================================================== */
/* MUSIC (Streaming desde almacenamiento masivo)                            */
/* ======================================================================== */

bool Cmixer::playMusic(const char *filename, bool loop)
{
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

/* ======================================================================== */
/* SFX (Canales de Efectos de Sonido en RAM)                                */
/* ======================================================================== */

int Cmixer::playChannel(CSample *s, bool loop, int volume, int channel)
{
    if (!s || !s->getData())
        return -1;

    if (volume < 0) volume = 0;
    if (volume > MAX_VOLUME) volume = MAX_VOLUME;

    // Canal específico requerido
    if (channel >= 0 && channel < MAX_CHANNELS)
    {
        channels[channel] = *s;
        channels[channel].setLoop(loop);
        channels[channel].setVolume(volume);
        channels[channel].setActive(true);
        channels[channel].setPosition(0);
        return channel;
    }

    // Búsqueda de canal libre automático
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
}

/* ======================================================================== */
/* MIX (Mezclador por Software Unificado Multiplataforma)                    */
/* ======================================================================== */

void Cmixer::mix(u8 *output, int len)
{
    Sint16 *out = (Sint16 *)output;
    int totalSamples = len / sizeof(Sint16);

    // Inicializar el búfer de salida con silencio absoluto (ceros)
    memset(out, 0, len);

    /* ========================================== */
    /* 1. PROCESAMIENTO DE EFECTOS DE SONIDO (SFX)*/
    /* ========================================== */
    for (int ch = 0; ch < MAX_CHANNELS; ch++)
    {
        if (!channels[ch].getActive())
            continue;

        Sint16 *data = (Sint16 *)channels[ch].getData();
        int pos = channels[ch].getPosition();
        int size = channels[ch].getSize() / sizeof(Sint16);

        int vol = (channels[ch].getVolume() * masterVolume) / MAX_VOLUME;

        // 🔥 DEBUG EXCLUSIVO PARA GAMECUBE (Rastrear por qué no suena)
#if defined(GC_BUILD)
        static int debugTicks = 0;
        if (debugTicks++ % 100 == 0) { // No saturar la consola, imprime cada 100 llamadas
            printf("[libGPP DEBUG] Ch: %d | DataPtr: %p | TotalSamples: %d | Pos: %d | Vol: %d\n", 
                   ch, data, size, pos, vol);
            if (data) {
                printf("[libGPP DATA] Primeros bytes raw: %04X %04X\n", data[0], data[1]);
            }
        }
#endif

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

            // FLAGS GAMECUBE: Desempaquetar Little-Endian del WAV a entero plano
#if defined(GC_BUILD)
            val = (Sint16)(((val & 0xFF) << 8) | ((val >> 8) & 0xFF));
#endif

            val = (val * vol) / MAX_VOLUME;

            // Obtener de forma segura lo acumulado por canales anteriores
#if defined(GC_BUILD)
            Sint16 current_out = (Sint16)(((out[i] & 0xFF) << 8) | ((out[i] >> 8) & 0xFF));
            int mixed = current_out + val;
#else
            int mixed = out[i] + val;
#endif

            // Protección de clipping duro (Saturación digital)
            if (mixed > 32767) mixed = 32767;
            if (mixed < -32768) mixed = -32768;

            // Almacenar el acumulado intermedio en el formato correcto
#if defined(GC_BUILD)
            out[i] = (Sint16)(((mixed & 0xFF) << 8) | ((mixed >> 8) & 0xFF));
#else
            out[i] = (Sint16)mixed;
#endif
        }

        channels[ch].setPosition(pos);
    }

    /* ========================================== */
    /* 2. PROCESAMIENTO DE MÚSICA DE FONDO (BGM)  */
    /* ========================================== */
    static u8 temp[16384]; // Búfer estático alineado

    if (len > (int)sizeof(temp))
        len = sizeof(temp);

    // Lee el streaming decodificado lineal
    int read = musicStream.read(temp, len);

    Sint16 *mdata = (Sint16 *)temp;
    int musicSamples = read / sizeof(Sint16);

    for (int i = 0; i < totalSamples && i < musicSamples; i++)
    {
        int val = mdata[i];

        // FLAGS GAMECUBE: Desempaquetar Little-Endian del stream de música
#if defined(GC_BUILD)
        val = (Sint16)(((val & 0xFF) << 8) | ((val >> 8) & 0xFF));
#endif

        val = (val * masterVolume) / MAX_VOLUME;

        // Mezclar música sobre la mezcla previa de efectos de sonido
#if defined(GC_BUILD)
        Sint16 current_out = (Sint16)(((out[i] & 0xFF) << 8) | ((out[i] >> 8) & 0xFF));
        int mixed = current_out + val;
#else
        int mixed = out[i] + val;
#endif

        if (mixed > 32767) mixed = 32767;
        if (mixed < -32768) mixed = -32768;

        // Volcar el resultado final empaquetado para el hardware destino
#if defined(GC_BUILD)
        out[i] = (Sint16)(((mixed & 0xFF) << 8) | ((mixed >> 8) & 0xFF));
#else
        out[i] = (Sint16)mixed;
#endif
    }
}

/* ======================================================================== */
/* CALLBACK                                                                 */
/* ======================================================================== */

void Cmixer::audioCallback(void *userdata, u8 *stream, int len)
{
    Cmixer *m = (Cmixer *)userdata;
    m->mix(stream, len);
}