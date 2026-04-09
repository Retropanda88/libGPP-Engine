#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <audio/sample.h>
#include <engine/types.h>

CSample::CSample()
{
	Data = NULL;
	len = 0;
	position = 0;
	active = false;
	loop = false;
	volume = 0;

}

CSample::~CSample()
{
	if (Data)
	{
		free(Data);
		Data = NULL;
	}

	len = 0;
	position = 0;
	active = false;
	loop = false;
	volume = 0;

}


static bool mem_read(const u8 * &ptr, const u8 * end, void *dst, u32 size)
{
	if (ptr + size > end)
		return false;

	memcpy(dst, ptr, size);
	ptr += size;
	return true;
}

static bool mem_skip(const u8 * &ptr, const u8 * end, u32 size)
{
	if (ptr + size > end)
		return false;

	ptr += size;
	return true;
}


bool CSample::Load(const char *filename)
{
	// ----------------------------
	// 1. Verificar la extensión del archivo
	// ----------------------------

	// Buscamos el último punto para obtener la extensión
	const char *ext = strrchr(filename, '.');

	// Si no hay extensión, error
	if (!ext)
	{
		printf("Error: archivo sin extensión válida\n");
		return false;
	}

	ext++;						// avanzamos para saltar el punto '.'

	// Comparamos ignorando mayúsculas/minúsculas
	if (strcasecmp(ext, "wav") != 0)
	{
		printf("Error: solo se admiten archivos WAV por ahora (%s no soportado)\n", ext);
		return false;
	}

	// ----------------------------
	// 2. Abrir archivo en modo binario
	// ----------------------------

	FILE *fp = fopen(filename, "rb");	// abrir archivo

	if (!fp)					// si falla la apertura
	{
		printf("Error: fichero no encontrado %s\n", filename);
		return false;
	}

	// ----------------------------
	// 3. Leer la cabecera RIFF/WAV
	// ----------------------------

	char riff[4];
	fread(riff, 1, 4, fp);		// leemos "RIFF"

	if (strncmp(riff, "RIFF", 4) != 0)	// verificar identificador
	{
		printf("Error: no es un archivo RIFF válido\n");
		fclose(fp);
		return false;
	}

	fseek(fp, 4, SEEK_CUR);		// saltar tamaño total del archivo (4 bytes)

	char wave[4];
	fread(wave, 1, 4, fp);		// leemos "WAVE"

	if (strncmp(wave, "WAVE", 4) != 0)	// verificar formato
	{
		printf("Error: archivo no es WAV\n");
		fclose(fp);
		return false;
	}

	// ----------------------------
	// 4. Buscar el chunk "data"
	// ----------------------------

	char chunkId[4];			// nombre del chunk
	u32 chunkSize = 0;			// tamaño del chunk

	// Leemos chunk por chunk hasta encontrar "data"
	while (fread(chunkId, 1, 4, fp) == 4)
	{
		fread(&chunkSize, 4, 1, fp);	// leemos tamaño del chunk

		// Si encontramos "data", salimos del ciclo
		if (strncmp(chunkId, "data", 4) == 0)
		{
			break;
		}
		else
		{
			// Saltamos este chunk desconocido
			fseek(fp, chunkSize, SEEK_CUR);
		}
	}

	// Si no encontramos "data", error
	if (strncmp(chunkId, "data", 4) != 0)
	{
		printf("Error: no se encontró la sección de datos PCM\n");
		fclose(fp);
		return false;
	}

	// ----------------------------
	// 5. Reservar memoria para los datos PCM
	// ----------------------------

	Data = (u8 *) malloc(chunkSize);	// reservamos memoria exacta para PCM

	if (!Data)					// si malloc falla
	{
		printf("Error: memoria insuficiente\n");
		fclose(fp);
		return false;
	}

	// ----------------------------
	// 6. Leer los datos PCM (solo audio)
	// ----------------------------

	u32 read = fread(Data, 1, chunkSize, fp);	// cargamos el PCM en memoria
	fclose(fp);					// cerramos archivo

	// verificar lectura correcta
	if (read != chunkSize)
	{
		printf("Error: no se pudo leer toda la data PCM\n");
		free(Data);
		Data = NULL;
		return false;
	}

	// ----------------------------
	// 7. Inicializar datos del sample
	// ----------------------------

	len = chunkSize;			// tamaño total del PCM
	position = 0;				// reiniciamos posición para reproducción
	active = false;				// aún no se está reproduciendo

	printf("Sample WAV cargado correctamente (%u bytes)\n", chunkSize);

	return true;				// éxito
}


bool CSample::LoadFromMemory(const u8 * mem, u32 len)
{
	if (!mem || len < 12)
	{
		printf("Error: buffer WAV inválido\n");
		return false;
	}

	const u8 *ptr = mem;
	const u8 *end = mem + len;

	/* ---------------------------- 1. RIFF ---------------------------- */
	char id[4];
	u32 size;

	if (!mem_read(ptr, end, id, 4) || strncmp(id, "RIFF", 4) != 0)
	{
		printf("Error: no es RIFF válido\n");
		return false;
	}

	mem_skip(ptr, end, 4);		/* tamaño RIFF */

	if (!mem_read(ptr, end, id, 4) || strncmp(id, "WAVE", 4) != 0)
	{
		printf("Error: no es WAV válido\n");
		return false;
	}

	/* ---------------------------- 2. Leer chunks
	   ---------------------------- */
	bool foundFmt = false;
	bool foundData = false;

	u16 audioFormat = 0;
	u16 channels = 0;
	u32 sampleRate = 0;
	u16 bitsPerSample = 0;

	const u8 *dataPtr = NULL;
	u32 dataSize = 0;

	while (ptr + 8 <= end)
	{
		mem_read(ptr, end, id, 4);
		mem_read(ptr, end, &size, 4);

		if (strncmp(id, "fmt ", 4) == 0)
		{
			if (size < 16 || ptr + size > end)
			{
				printf("Error: chunk fmt corrupto\n");
				return false;
			}

			mem_read(ptr, end, &audioFormat, 2);
			mem_read(ptr, end, &channels, 2);
			mem_read(ptr, end, &sampleRate, 4);
			mem_skip(ptr, end, 6);	/* byteRate + blockAlign */
			mem_read(ptr, end, &bitsPerSample, 2);

			/* Saltar extra si existe */
			if (size > 16)
				mem_skip(ptr, end, size - 16);

			foundFmt = true;
		}
		else if (strncmp(id, "data", 4) == 0)
		{
			if (ptr + size > end)
			{
				printf("Error: chunk data corrupto\n");
				return false;
			}

			dataPtr = ptr;
			dataSize = size;
			foundData = true;

			mem_skip(ptr, end, size);
		}
		else
		{
			/* Saltar chunk desconocido */
			mem_skip(ptr, end, size);
		}

		/* Alineación a 2 bytes */
		if (size & 1)
			mem_skip(ptr, end, 1);
	}

	if (!foundFmt || !foundData)
	{
		printf("Error: WAV incompleto (fmt/data)\n");
		return false;
	}

	if (audioFormat != 1)
	{
		printf("Error: WAV no es PCM\n");
		return false;
	}

	/* ---------------------------- 3. Reservar y copiar PCM
	   ---------------------------- */
	if (Data)
	{
		free(Data);
		Data = NULL;
	}

	Data = (u8 *) malloc(dataSize);
	if (!Data)
	{
		printf("Error: memoria insuficiente\n");
		return false;
	}

	memcpy(Data, dataPtr, dataSize);

	/* ---------------------------- 4. Inicializar sample
	   ---------------------------- */
	this->len = dataSize;		/* ajusta si tu miembro se llama distinto */
	position = 0;
	active = false;

	printf("WAV PCM cargado: %u Hz, %u ch, %u bits (%u bytes)\n",
		   sampleRate, channels, bitsPerSample, dataSize);

	return true;
}


void CSample::reset()
{
	if (Data)
		free(Data);
	Data = NULL;
	len = 0;
	active = false;
	loop = false;
	position = 0;
	volume = 0;

}
