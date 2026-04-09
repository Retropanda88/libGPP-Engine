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
}

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

bool Cmixer::playMusic(const char *filename, bool loop)
{
    stopMusic();

    FS_FILE *fp = fs_open(filename, "rb");
    if (!fp)
    {
        printf("Error opening file");
        return false;
    }

    /* elegir decoder correctamente */
    if (isWav(filename))
    {
        //Write_Log("load wav");
        decoder = new WavDecoder();
    }
    else if (isMp3(filename))
    {
        printf("mp3 no implementado");
        fs_close(fp);
        return false;
    }
    else
    {
        printf("Unsupported format");
        fs_close(fp);
        return false;
    }

    if (!decoder)
    {
        printf("ERROR: decoder NULL");
        fs_close(fp);
        return false;
    }

    

    /* abrir decoder */
    
    if (!decoder->open(fp))
    {
        printf("ERROR: decoder open failed");
        fs_close(fp);
        delete decoder;
        decoder = NULL;
        return false;
    }


    /* iniciar stream */
    if (!musicStream.open(decoder, loop))
    {
        printf("ERROR: stream open failed");
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

int Cmixer::playChannel(CSample * s, bool loop, int volume)
{
	if (!s || !s->getData())
		return -1;

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

void Cmixer::mix(u8 * output, int len)
{
	Sint16 *out = (Sint16 *) output;
	int totalSamples = len / sizeof(Sint16);

	memset(out, 0, len);

	/* ===== SFX ===== */
	for (int ch = 0; ch < MAX_CHANNELS; ch++)
	{
		if (!channels[ch].getActive())
			continue;

		Sint16 *data = (Sint16 *) channels[ch].getData();
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
					break;
				}
			}

			int val = data[pos++];
			val = (val * channels[ch].getVolume()) / MAX_VOLUME;
			val = (val * masterVolume) / MAX_VOLUME;

			int mixed = out[i] + val;

			if (mixed > 32767)
				mixed = 32767;
			if (mixed < -32768)
				mixed = -32768;

			out[i] = (Sint16) mixed;
		}

		channels[ch].setPosition(pos);
	}

	/* ===== MUSIC ===== */
	u8 temp[4096];
	int read = musicStream.read(temp, len);

	Sint16 *mdata = (Sint16 *) temp;
	int musicSamples = read / sizeof(Sint16);

	for (int i = 0; i < totalSamples && i < musicSamples; i++)
	{
		int val = mdata[i];
		val = (val * masterVolume) / MAX_VOLUME;

		int mixed = out[i] + val;

		if (mixed > 32767)
			mixed = 32767;
		if (mixed < -32768)
			mixed = -32768;

		out[i] = (Sint16) mixed;
	}
}

void Cmixer::audioCallback(void *userdata, u8 * stream, int len)
{
	Cmixer *m = (Cmixer *) userdata;
	m->mix(stream, len);
}
