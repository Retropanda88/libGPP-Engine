
#include <string.h>
#include <audio/mixer.h>
#include <engine/fs.h>

#define MUSIC_STREAM_SIZE 32768

u8 buffer[MUSIC_STREAM_SIZE];
int buffered;


typedef struct
{
	u16 audioFormat;
	u16 channels;
	u32 sampleRate;
	u32 byteRate;
	u16 blockAlign;
	u16 bitsPerSample;
} WAV_FMT;



Cmixer::Cmixer()
{

	music.fp = NULL;
	music.position = 0;
	music.active = false;
	music.loop = false;
	music.len = 0;

	masterVolume = 0;
}

Cmixer::~Cmixer()
{


}

bool Cmixer::init(int freq, int outChannel, int bufferSize, int volume)
{

	spec.freq = freq;
	spec.format = AUDIO_S16LSB;
	spec.channels = outChannel;
	spec.samples = bufferSize;
	spec.callback = audioCallback;
	spec.userdata = this;

	if (SDL_OpenAudio(&spec, NULL) < 0)
	{
		printf("error: sound init fail %s\n", SDL_GetError());
		return false;
	}

	masterVolume = volume;

	SDL_PauseAudio(0);

	printf("saund init\nfreq %d\nchannels %d\nbuffersize %d\n", spec.freq, spec.channels,
		   spec.samples);
	return true;
}



int Cmixer::playChannel(CSample * s, int id, int loop, int volume)
{
	// validar el sample
	if (!s)
		return -1;

	// si el id es -1, buscar un canal libre
	if (id == -1)
	{
		id = -1;				// lo vuelvo a usar como indicador
		for (int i = 0; i < MAX_CHANNELS; i++)
		{
			// si un canal no está activo, está libre
			if (!channels[i].getActive())
			{
				id = i;
				break;
			}
		}

		// si no encontró canal libre
		if (id == -1)
			return -1;
	}

	// validar ID
	if (id < 0 || id >= MAX_CHANNELS)
		return -1;

	// asignar el sample al canal
	channels[id] = *s;

	if (!channels[id].getData())
		return -1;

	channels[id].setLoop(loop);
	channels[id].setActive(true);
	channels[id].setVolume(volume);

	return id;					// regresar el canal asignado (útil para
	// debug)
}

void Cmixer::stopChannel(int id)
{

	// si el canal es valido
	if (id < 0 || id > MAX_CHANNELS)
		return;
	if (channels[id].getActive())
		channels[id].reset();
}

void Cmixer::pauseChannel(int id)
{
	// si el canal es valido
	if (id < 0 || id > MAX_CHANNELS)
		return;
	if (channels[id].getActive())
	{
		channels[id].setActive(false);
	}
	else
	{
		channels[id].setActive(true);
	}
}

void Cmixer::stopAllChsnnels()
{
	// stop all channels
	for (int i = 0; i < MAX_CHANNELS; i++)
		channels[i].reset();
}

void Cmixer::setChannelVolume(int id, int volume)
{
	// si el canal es valido
	if (id < 0 || id > MAX_CHANNELS)
		return;

	channels[id].setVolume(volume);
}


// /////////music/////////////
/* bool Cmixer::loadMusic(const char *filename, bool loop) { // Si ya había
   música cargada, cerrarla if (music.fp) { fclose(music.fp); music.fp =
   NULL; }

   // Abrir archivo music.fp = fopen(filename, "rb"); if (!music.fp) {
   printf("error: music file not found %s\n", filename); return false; }

   // ----------------------------- // VALIDAR QUE SEA RIFF/WAVE //
   ----------------------------- char riff[4]; fread(riff, 1, 4, music.fp); if 
   (memcmp(riff, "RIFF", 4) != 0) { printf("error: music is not RIFF\n");
   fclose(music.fp); music.fp = NULL; return false; }

   // Saltar tamaño RIFF fseek(music.fp, 4, SEEK_CUR);

   char wave[4]; fread(wave, 1, 4, music.fp); if (memcmp(wave, "WAVE", 4) !=
   0) { printf("error: music is not WAVE\n"); fclose(music.fp); music.fp =
   NULL; return false; }

   // ---------------------------------- // Buscar el chunk "data" //
   ---------------------------------- // WAV no siempre tiene data enseguida
   (puede tener LIST, FACT, etc.) char chunkId[4]; u32 chunkSize;

   while (1) { if (fread(chunkId, 1, 4, music.fp) != 4) { printf("error:
   invalid wav header\n"); fclose(music.fp); music.fp = NULL; return false; }

   if (fread(&chunkSize, 4, 1, music.fp) != 1) { printf("error: invalid wav
   chunk size\n"); fclose(music.fp); music.fp = NULL; return false; }

   // Si encontramos el chunk de data → salir if (memcmp(chunkId, "data", 4) 
   == 0) { break; }

   // si no es data, saltar el chunk fseek(music.fp, chunkSize, SEEK_CUR); }

   // ---------------------------------- // Ahora estamos justo al inicio de
   los datos PCM // ----------------------------------

   music.len = chunkSize; // tamaño total del PCM music.position = 0;
   music.loop = loop; music.active = false; music.volume = 128; // volumen
   medio

   printf("music loaded: %s\n", filename); printf("pcm size: %u bytes\n",
   music.len);

   return true; } */


/* esto va bien no mover

   bool Cmixer::loadMusic(const char *filename, bool loop) { // Cerrar archivo 
   anterior si existía if (music.fp) { fclose(music.fp); music.fp = NULL; }

   // Abrir archivo WAV music.fp = fopen(filename, "rb"); if (!music.fp) {
   printf("error: file not found %s\n", filename); return false; }

   // Leer cabecera WAV mínima para saltar chunks no deseados char riff[4];
   fread(riff, 1, 4, music.fp); // "RIFF" fseek(music.fp, 4, SEEK_CUR); //
   tamaño total fread(riff, 1, 4, music.fp); // "WAVE"

   // Buscar chunk "data" char chunkId[4]; u32 chunkSize; while (true) { if
   (fread(chunkId, 1, 4, music.fp) != 4) break; fread(&chunkSize, 4, 1,
   music.fp);

   if (strncmp(chunkId, "data", 4) == 0) { music.len = chunkSize;
   music.dataStart = ftell(music.fp); break; } else { // Saltar chunk
   fseek(music.fp, chunkSize, SEEK_CUR); } }

   // Inicializar variables music.loop = loop; music.active = false;
   music.volume = 128; memset(music.buffer, 0, sizeof(music.buffer));
   music.position = 0;

   printf("Music loaded: %s, size: %u bytes\n", filename, music.len); return
   true; } */

static u32 read_u32_le(FS_FILE * f)
{
	u8 b[4];
	if (fs_read(b, 1, 4, f) != 4)
		return 0;

	return (u32) b[0] | ((u32) b[1] << 8) | ((u32) b[2] << 16) | ((u32) b[3] << 24);
}

bool Cmixer::loadMusic(const char *filename, bool loop)
{
	FILE *f = fopen("debug.txt", "w");
	if (!f)
		return false;

	/* cerrar archivo anterior */
	if (music.fp)
	{
		fs_close(music.fp);
		music.fp = NULL;
	}

	/* abrir archivo WAV */
	music.fp = fs_open(filename, "rb");
	if (!music.fp)
	{
		fprintf(f, "ERROR: file not found %s\n", filename);
		fclose(f);
		return false;
	}

	fprintf(f, "File loaded from: %s\n", filename);

	/* ===== RIFF ===== */
	char id[4];

	fs_read(id, 1, 4, music.fp);
	if (strncmp(id, "RIFF", 4) != 0)
	{
		fprintf(f, "ERROR: not RIFF\n");
		fclose(f);
		return false;
	}

	fs_seek(music.fp, 4, FS_SEEK_CUR);	/* tamaño total */

	fs_read(id, 1, 4, music.fp);
	if (strncmp(id, "WAVE", 4) != 0)
	{
		fprintf(f, "ERROR: not WAVE\n");
		fclose(f);
		return false;
	}

	fprintf(f, "RIFF/WAVE OK\n");

	/* ===== buscar chunks ===== */
	char chunkId[4];
	u32 chunkSize = 0;

	music.len = 0;
	music.dataStart = 0;

	WAV_FMT fmt;
	memset(&fmt, 0, sizeof(WAV_FMT));

	while (1)
	{
		if (fs_read(chunkId, 1, 4, music.fp) != 4)
			break;

		chunkSize = read_u32_le(music.fp);

		/* ---- CHUNK fmt ---- */
		if (strncmp(chunkId, "fmt ", 4) == 0)
		{
			fs_read(&fmt, sizeof(WAV_FMT), 1, music.fp);

			fprintf(f,
					"FMT: format=%u channels=%u rate=%u bits=%u\n",
					fmt.audioFormat, fmt.channels, fmt.sampleRate, fmt.bitsPerSample);

			/* saltar bytes extra si fmt > 16 */
			if (chunkSize > sizeof(WAV_FMT))
				fs_seek(music.fp, chunkSize - sizeof(WAV_FMT), FS_SEEK_CUR);
		}
		/* ---- CHUNK data ---- */
		else if (strncmp(chunkId, "data", 4) == 0)
		{
			music.len = chunkSize;
			music.dataStart = fs_tell(music.fp);

			fprintf(f, "DATA: start=%u size=%u\n", (u32) music.dataStart, music.len);

			break;				/* ya no necesitamos más chunks */
		}
		/* ---- CHUNK desconocido ---- */
		else
		{
			fs_seek(music.fp, chunkSize, FS_SEEK_CUR);

			/* padding RIFF */
			if (chunkSize & 1)
				fs_seek(music.fp, 1, FS_SEEK_CUR);
		}
	}

	fprintf(f, "dataStart=%u len=%u\n", music.dataStart, music.len);

	/* ===== prueba de lectura real ===== */
	if (music.len > 0)
	{
		u8 test[16];

		fs_seek(music.fp, music.dataStart, FS_SEEK_SET);
		fs_read(test, 1, 16, music.fp);

		fprintf(f, "audio bytes: ");
		for (int i = 0; i < 16; i++)
			fprintf(f, "%02X ", test[i]);
		fprintf(f, "\n");
	}
	else
	{
		fprintf(f, "ERROR: data chunk not found\n");
	}

	/* ===== estado inicial ===== */
	music.loop = loop;
	music.active = false;
	music.volume = 128;
	music.position = 0;
	memset(music.buffer, 0, sizeof(music.buffer));

	fprintf(f, "loadMusic OK\n");
	fclose(f);
	return true;
}


void Cmixer::playMusic()
{
	music.active = true;
}

/* void Cmixer::mix(u8 * outputBuffer, int len) { Sint16 *out = (Sint16 *)
   outputBuffer; int samples = len / sizeof(Sint16); // número de muestras
   (L+R si // estéreo)

   memset(out, 0, len); // silencio en 0

   for (int ch = 0; ch < MAX_CHANNELS; ch++) { if (!channels[ch].getActive()
   || !channels[ch].getData()) continue;

   Sint16 *data = (Sint16 *) channels[ch].getData();

   for (int i = 0; i < samples; i += spec.channels) { if
   (channels[ch].getPosition() >= channels[ch].getSize() / 2) // dividir //
   entre // 2 // (bytes // → // samples) { if (channels[ch].getLoop())
   channels[ch].setPosition(0); else { channels[ch].setActive(false); break; }
   }

   // tomar muestra directamente (WAV 16-bit) Sint16 valSound =
   data[channels[ch].getPosition()];

   // aplicar volúmenes int channelVol = channels[ch].getVolume(); valSound =
   valSound * channelVol / MAX_VOLUME; valSound = valSound * masterVolume /
   MAX_VOLUME;

   // mezclar en todos los canales de salida for (int c = 0; c < spec.channels; 
   c++) { int mixed = out[i + c] + valSound;

   if (mixed > 32767) mixed = 32767; if (mixed < -32768) mixed = -32768;

   out[i + c] = (Sint16) mixed; }

   // avanzar una muestra channels[ch].setPosition(channels[ch].getPosition() + 
   1); } } } */


/* void Cmixer::mix(u8 *outputBuffer, int len) { // Convertir el buffer de
   bytes a enteros de 16 bits Sint16 *out = (Sint16 *)outputBuffer;

   // Total de muestras pedidas por SDL (contando canales L+R si es estéreo)
   int samples = len / sizeof(Sint16);

   // Inicializar el buffer en silencio memset(out, 0, len);

   // ================================================ // MEZCLAR TODOS LOS
   SAMPLES (16 canales) // ================================================
   for (int ch = 0; ch < MAX_CHANNELS; ch++) { // Si el canal no está activo
   o no tiene datos, saltar if (!channels[ch].getActive() ||
   !channels[ch].getData()) continue;

   Sint16 *data = (Sint16 *)channels[ch].getData(); // PCM 16-bit

   for (int i = 0; i < samples; i += spec.channels) // spec.channels = 1 o 2 {
   // Si el sample terminó if (channels[ch].getPosition() >=
   channels[ch].getSize() / 2) { if (channels[ch].getLoop()) {
   channels[ch].setPosition(0); } else { channels[ch].setActive(false); break;
   } }

   // Tomar muestra actual Sint16 val = data[channels[ch].getPosition()];

   // Aplicar volumen por canal val = (val * channels[ch].getVolume()) /
   MAX_VOLUME;

   // Aplicar master volume val = (val * masterVolume) / MAX_VOLUME;

   // Mezclar en cada canal de salida (mono o estéreo) for (int c = 0; c <
   spec.channels; c++) { int mixed = out[i + c] + val;

   // Saturación if (mixed > 32767) mixed = 32767; if (mixed < -32768) mixed = 
   -32768;

   out[i + c] = (Sint16)mixed; }

   // Avanzar una muestra channels[ch].setPosition(channels[ch].getPosition() + 
   1); } }

   // ================================================ // MEZCLAR LA MÚSICA // 
   ================================================ if (music.active &&
   music.fp) { int bytesToRead = samples * sizeof(Sint16);

   // Leer bloque desde el archivo WAV int readed = fread(music.Buffer, 1,
   bytesToRead, music.fp);

   // Si llegó al final del archivo if (readed < bytesToRead) { if
   (music.loop) { // Volver a leer desde el inicio del PCM (chunk data)
   fseek(music.fp, music.dataStart, SEEK_SET); int r2 = fread(music.Buffer +
   readed, 1, bytesToRead - readed, music.fp); readed += r2; } else {
   music.active = false; memset(music.Buffer + readed, 0, bytesToRead -
   readed); } }

   // Convertir buffer a muestras de 16-bit Sint16 *mdata = (Sint16
   *)music.Buffer; int musicSamples = readed / 2;

   // Mezclar música en el buffer final for (int i = 0; i < samples; i++) { if 
   (i >= musicSamples) break;

   Sint16 val = mdata[i];

   // Volumen de la música val = (val * music.volume) / MAX_VOLUME;

   // Volumen maestro val = (val * masterVolume) / MAX_VOLUME;

   int mixed = out[i] + val;

   // Saturación if (mixed > 32767) mixed = 32767; if (mixed < -32768) mixed = 
   -32768;

   out[i] = (Sint16)mixed; } } } */

/* void Cmixer::mix(u8 *outputBuffer, int len) { // Convertir buffer a enteros 
   de 16 bits Sint16 *out = (Sint16 *)outputBuffer; int samples = len /
   sizeof(Sint16);

   // Inicializar el buffer final en silencio memset(out, 0, len);

   // ============================ // Mezclar todos los samples //
   ============================ for (int ch = 0; ch < MAX_CHANNELS; ch++) { if 
   (!channels[ch].getActive() || !channels[ch].getData()) continue;

   Sint16 *data = (Sint16 *)channels[ch].getData();

   for (int i = 0; i < samples; i += spec.channels) { if
   (channels[ch].getPosition() >= channels[ch].getSize() / 2) { if
   (channels[ch].getLoop()) channels[ch].setPosition(0); else {
   channels[ch].setActive(false); break; } }

   Sint16 val = data[channels[ch].getPosition()]; val = (val *
   channels[ch].getVolume()) / MAX_VOLUME; val = (val * masterVolume) /
   MAX_VOLUME;

   for (int c = 0; c < spec.channels; c++) { int mixed = out[i + c] + val; if
   (mixed > 32767) mixed = 32767; if (mixed < -32768) mixed = -32768; out[i +
   c] = (Sint16)mixed; }

   channels[ch].setPosition(channels[ch].getPosition() + 1); } }

   // ============================ // Mezclar música en streaming //
   ============================ if (music.active && music.fp) { int
   bytesToRead = samples * sizeof(Sint16); int totalRead = 0;

   // Limpiar el buffer de música memset(music.buffer, 0,
   sizeof(music.buffer));

   while (totalRead < bytesToRead && music.active) { int r =
   fread(music.buffer + totalRead, 1, bytesToRead - totalRead, music.fp);

   if (r == 0) // EOF { if (music.loop) { fseek(music.fp, music.dataStart,
   SEEK_SET); continue; } else { music.active = false; break; } }

   totalRead += r; }

   Sint16 *mdata = (Sint16 *)music.buffer; int musicSamples = totalRead / 2;

   for (int i = 0; i < samples; i++) { if (i >= musicSamples) break;

   Sint16 val = mdata[i]; val = (val * music.volume) / MAX_VOLUME; val = (val
   * masterVolume) / MAX_VOLUME;

   int mixed = out[i] + val; if (mixed > 32767) mixed = 32767; if (mixed <
   -32768) mixed = -32768;

   out[i] = (Sint16)mixed; } } } */

void Cmixer::streamMusic(void)
{
	if (!music.active || !music.fp)
		return;

	if (music.buffered > MUSIC_STREAM_SIZE / 2)
		return;					/* aún hay datos */

	int r = fs_read(music.buffer + music.buffered,
					1,
					MUSIC_STREAM_SIZE - music.buffered,
					music.fp);

	if (r <= 0)
	{
		if (music.loop)
		{
			fs_seek(music.fp, music.dataStart, FS_SEEK_SET);
			return;
		}
		music.active = false;
		return;
	}

	music.buffered += r;
}



void Cmixer::mix(u8 * outputBuffer, int len)
{
	Sint16 *out = (Sint16 *) outputBuffer;
	int totalSamples = len / sizeof(Sint16);

	/* limpiar buffer final */
	memset(out, 0, len);

	/* ============================ Mezcla de samples (SFX)
	   ============================ */
	for (int ch = 0; ch < MAX_CHANNELS; ch++)
	{
		if (!channels[ch].getActive() || !channels[ch].getData())
			continue;

		Sint16 *data = (Sint16 *) channels[ch].getData();
		int dataSamples = channels[ch].getSize() / sizeof(Sint16);

		for (int i = 0; i < totalSamples; i += spec.channels)
		{
			if (channels[ch].getPosition() >= dataSamples)
			{
				if (channels[ch].getLoop())
					channels[ch].setPosition(0);
				else
				{
					channels[ch].setActive(false);
					break;
				}
			}

			Sint16 val = data[channels[ch].getPosition()];
			val = (val * channels[ch].getVolume()) / MAX_VOLUME;
			val = (val * masterVolume) / MAX_VOLUME;

			for (int c = 0; c < spec.channels; c++)
			{
				int mixed = out[i + c] + val;
				if (mixed > 32767)
					mixed = 32767;
				if (mixed < -32768)
					mixed = -32768;
				out[i + c] = (Sint16) mixed;
			}

			channels[ch].setPosition(channels[ch].getPosition() + 1);
		}
	}

	/* ============================ Mezcla de música (stream)
	   ============================ */
	if (music.active && music.fp)
	{
		int maxRead = sizeof(music.buffer);
		int bytesNeeded = len;
		if (bytesNeeded > maxRead)
			bytesNeeded = maxRead;

		int totalRead = 0;
		memset(music.buffer, 0, sizeof(music.buffer));

		while (totalRead < bytesNeeded && music.active)
		{
			int r = fs_read(music.buffer + totalRead,
							1,
							bytesNeeded - totalRead,
							music.fp);

			if (r <= 0)			/* EOF */
			{
				if (music.loop)
				{
					fs_seek(music.fp, music.dataStart, FS_SEEK_SET);
					continue;
				}
				else
				{
					music.active = false;
					break;
				}
			}

			totalRead += r;
		}

		Sint16 *mdata = (Sint16 *) music.buffer;
		int musicSamples = totalRead / sizeof(Sint16);

		for (int i = 0; i < totalSamples && i < musicSamples; i++)
		{
			Sint16 val = mdata[i];
			val = (val * music.volume) / MAX_VOLUME;
			val = (val * masterVolume) / MAX_VOLUME;

			int mixed = out[i] + val;
			if (mixed > 32767)
				mixed = 32767;
			if (mixed < -32768)
				mixed = -32768;
			out[i] = (Sint16) mixed;
		}
	}
}



void Cmixer::audioCallback(void *userdata, u8 * stream, int len)
{
	Cmixer *m = (Cmixer *) userdata;
	m->mix(stream, len);
}
