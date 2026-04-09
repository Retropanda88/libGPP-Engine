#ifndef MIXER_H_
#define MIXER_H_

#include <engine/types.h>
#include <audio/sample.h>
#include <audio/MusicStream.h>
#include <audio/IAudioDecoder.h>
#include <SDL/SDL.h>

#define MAX_CHANNELS 16
#define MAX_VOLUME 128

class Cmixer
{
  public:
	Cmixer();
	~Cmixer();

	bool init(int freq, int channels, int bufferSize);

	/* SFX */
	int playChannel(CSample * s, bool loop, int volume);
	void stopChannel(int id);
	void stopAll();

	/* MUSIC */
	bool playMusic(const char *filename, bool loop);
	void stopMusic();

	void setMasterVolume(int vol);

	/* MIX */
	void mix(u8 * stream, int len);

  private:
	  CSample channels[MAX_CHANNELS];
	int masterVolume;

	/* sistema de música */
	MusicStream musicStream;
	IAudioDecoder *decoder;

	SDL_AudioSpec spec;

	/* utils */
	bool isWav(const char *filename);
	bool isMp3(const char *filename);

	static void audioCallback(void *userdata, u8 * stream, int len);
};


#endif
