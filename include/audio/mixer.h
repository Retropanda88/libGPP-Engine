#ifndef MIXER_H_
#define MIXER_H_

#include <engine/types.h>
#include <audio/sample.h>
#include <engine/fs.h>
#include <SDL/SDL.h>

#define MAX_VOLUME  128;
#define MIN_VOLUME 0
#define MAX_CHANNELS 16


/*struct Music
{
	FILE *fp;
	int position;
	bool active;
	bool loop;
	int volume;
	int len;
	
};
*/
struct Music {
    FS_FILE* fp;          // archivo WAV
    bool active;       // ¿se está reproduciendo?
    bool loop;         // loop sí/no
    int volume;        // volumen de la música (0-128)
    u32 len;           // tamaño total del PCM
    u32 position;      // SOLO si usas lectura por position (aquí no se usa)

    long dataStart;    // <--- NUEVO: posición donde inicia el chunk "data"
    u8 buffer[4096];   // <--- NUEVO: buffer para streaming
    u32 buffered;
};
class Cmixer
{
  public:
	Cmixer();
	~Cmixer();


	bool init(int freq, int outChannel, int bufferSize, int volume);


	int playChannel(CSample * s, int id, int loop, int volume);
	void stopChannel(int ID);
	void pauseChannel(int id);
	void stopAllChsnnels();

	void setChannelVolume(int id, int volume);
	void streamMusic();

	bool loadMusic(const char *filename, bool loop);
	void playMusic();
	void stopMusic();
	void pauseMusic();

	void setVolumeMusic(int volume);

	// all

	void setVolumeMaster(int volume);

	void mix(u8 * outputBuffer, int len);

  private:
	  CSample channels[MAX_CHANNELS];
	struct Music music;
	int masterVolume;
	SDL_AudioSpec spec;
	static void audioCallback(void *userdata, u8 * stream, int len);
	
};

#endif
