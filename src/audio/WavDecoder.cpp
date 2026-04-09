#include <audio/WavDecoder.h>
#include <string.h>
#include <stdio.h>

WavDecoder::WavDecoder()
{
	fp = NULL;
	dataStart = 0;
	dataSize = 0;
	dataPos = 0;

}


WavDecoder::~WavDecoder()
{
}

bool WavDecoder::open(FS_FILE * file)
{
	if (!file)
		return false;

	fp = file;

	if (!parseWav())
		return false;

	fs_seek(fp, dataStart, FS_SEEK_SET);
	dataPos = 0;

	return true;
}

static u32 read_u32_le(FS_FILE * f)
{
	u8 b[4];
	if (fs_read(b, 1, 4, f) != 4)
		return 0;

	return (u32) b[0] | ((u32) b[1] << 8) | ((u32) b[2] << 16) | ((u32) b[3] << 24);
}

bool WavDecoder::parseWav()
{
	char id[4];

	fs_read(id, 1, 4, fp);
	if (strncmp(id, "RIFF", 4) != 0)
		return false;

	fs_seek(fp, 4, FS_SEEK_CUR);

	fs_read(id, 1, 4, fp);
	if (strncmp(id, "WAVE", 4) != 0)
		return false;

	char chunkId[4];
	u32 chunkSize = 0;

	while (1)
	{
		if (fs_read(chunkId, 1, 4, fp) != 4)
			return false;

		chunkSize = read_u32_le(fp);

		if (strncmp(chunkId, "fmt ", 4) == 0)
		{
			u16 audioFormat;
			fs_read(&this->audioFormat, sizeof(u16), 1, fp);
			fs_read(&channels, sizeof(u16), 1, fp);
			fs_read(&sampleRate, sizeof(u32), 1, fp);

			fs_seek(fp, 6, FS_SEEK_CUR);	// byteRate + blockAlign

			fs_read(&bitsPerSample, sizeof(u16), 1, fp);

			if (chunkSize > 16)
				fs_seek(fp, chunkSize - 16, FS_SEEK_CUR);
			

		}
		else if (strncmp(chunkId, "data", 4) == 0)
		{
			dataSize = chunkSize;
			dataStart = fs_tell(fp);
			return true;
		}
		else
		{
			fs_seek(fp, chunkSize, FS_SEEK_CUR);
		}
	}

	return false;
}

int WavDecoder::decode(u8 * buffer, int len)
{
	if (!fp)
		return 0;

	u32 remaining = dataSize - dataPos;
	if (remaining == 0)
		return 0;

	if ((u32) len > remaining)
		len = remaining;

	int read = fs_read(buffer, 1, len, fp);

	dataPos += read;

	return read;
}

void WavDecoder::rewind()
{
	if (!fp)
		return;

	fs_seek(fp, dataStart, FS_SEEK_SET);
	dataPos = 0;
}

void WavDecoder::close()
{
	if (fp)
	{
		fs_close(fp);
		fp = NULL;
	}
}
