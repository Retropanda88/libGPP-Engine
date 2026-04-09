#include <audio/MusicStream.h>
#include <string.h>
#include <engine/log.h>

MusicStream::MusicStream()
{
	decoder = NULL;
	thread = NULL;
	mutex = SDL_CreateMutex();

	running = false;
	looping = false;

	 buffer.init(65536); // 64 KB buffer
	
}

MusicStream::~MusicStream()
{
	stop();

	if (mutex)
		SDL_DestroyMutex(mutex);
}

bool MusicStream::open(IAudioDecoder * dec, bool loop)
{
	decoder = dec;
	looping = loop;

	buffer.clear();

	return (decoder != NULL);
}

void MusicStream::play()
{
	if (!decoder)
		return;

	running = true;

	thread = SDL_CreateThread(threadFunc, this);
}

void MusicStream::stop()
{
	running = false;

	if (thread)
	{
		SDL_WaitThread(thread, NULL);
		thread = NULL;
	}
}

int MusicStream::threadFunc(void *data)
{
	MusicStream *ms = (MusicStream *) data;
	ms->streamLoop();
	return 0;
}

void MusicStream::streamLoop()
{
	u8 temp[4096];

	while (running)
	{
		SDL_LockMutex(mutex);

		int free = buffer.freeSpace();

		if (free >= (int)sizeof(temp))
		{

			int decoded = decoder->decode(temp, sizeof(temp));


			if (decoded <= 0)
			{
				if (looping)
				{
					decoder->rewind();
				}
				else
				{
					running = false;
					SDL_UnlockMutex(mutex);
					break;
				}
			}
			else
			{
				buffer.write(temp, decoded);
			}
		}

		SDL_UnlockMutex(mutex);

		SDL_Delay(5);
	}
}

int MusicStream::read(u8 * out, int len)
{
	SDL_LockMutex(mutex);

	int read = buffer.read(out, len);

	SDL_UnlockMutex(mutex);

	return read;
}
