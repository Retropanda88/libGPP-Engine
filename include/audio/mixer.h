#ifndef CMIXER_H_
#define CMIXER_H_

#include <engine/types.h>
#include <audio/sample.h>
#include <audio/MusicStream.h>
#include <audio/IAudioDecoder.h>
#include <engine/mutex.h>		// <- Incluimos la interfaz del Mutex
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
	int playChannel(CSample * s, bool loop, int volume, int channel = -1);
	void stopChannel(int id);
	void stopAll();
	void pauseChannel(int id);
	void resumeChannel(int id);
	void pauseAllChannels();
	void resumeAllChannels();
	void setChannelVolume(int id, int vol);
	void fadeOutChannel(int id, int ms);
	void fadeOutAllChannels(int ms);

	/* NOTIFICACIÓN EXCLUSIVA CON DUCKING GLOBAL */
	int playNotification(CSample *s, bool loop, int volume, int targetVolumePercent = 30, float duckSpeed = 0.4f, float restoreSpeed = 0.6f);

	/* MUSIC */
	bool playMusic(const char *filename, bool loop);
	void stopMusic();
	void pauseMusic();
	void resumeMusic();
	void setMusicVolume(int vol);
	void fadeOutMusic(int ms);

	/* GLOBAL PAUSE / RESUME & FADE ALL */
	void pauseAll();
	void resumeAll();
	void setMasterVolume(int vol);
	void fadeOutAll(int ms);

	/* MIX */
	void mix(u8 * stream, int len);

  private:
	CSample channels[MAX_CHANNELS];
	int masterVolume;
	int musicVolume;

	bool isFadingOut;
	int fadeTotalSamples;
	int fadeCurrentSample;
	int volumeBeforeFade;

	bool musicPaused;

	// Variables de control de fade out individuales por canal SFX
	bool channelFading[MAX_CHANNELS];
	long channelFadeCurrentSample[MAX_CHANNELS];
	long channelFadeTotalSamples[MAX_CHANNELS];
	int channelVolumeBeforeFade[MAX_CHANNELS];

	// Control de Ducking Global para el fondo (Música + SFX)
	float currentDuckingMultiplier;
	float targetDuckingMultiplier;
	float duckingSpeed;
	float autoRestoreSpeed;

	// Canal exclusivo para Notificaciones independientes
	CSample notificationChannelSample;
	bool notificationChannelActive;
	bool notificationChannelLoop;
	int notificationChannelVolume;
	int notificationChannelPos;

	/* música */
	MusicStream musicStream;
	IAudioDecoder *decoder;

	/* Sincronización multi-hilo */
	Mutex *mutex;				// <- Nuestro mutex protegido

	SDL_AudioSpec spec;

	/* utils */
	bool isWav(const char *filename);
	bool isMp3(const char *filename);

	static void audioCallback(void *userdata, u8 * stream, int len);
};

#endif /* CMIXER_H_ */
