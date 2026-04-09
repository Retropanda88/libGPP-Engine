#ifndef MUSICSTREAM_H_
#define MUSICSTREAM_H_

#include <engine/types.h>
#include <audio/IAudioDecoder.h>
#include <audio/CircularBuffer.h>
#include <SDL/SDL.h>

class MusicStream
{
public:
    MusicStream();
    ~MusicStream();

    bool open(IAudioDecoder* dec, bool loop);
    void play();
    void stop();

    int read(u8* out, int len);

private:
    IAudioDecoder* decoder;
    CircularBuffer buffer;

    SDL_Thread* thread;
    SDL_mutex* mutex;

    bool running;
    bool looping;

    static int threadFunc(void* data);
    void streamLoop();
};

#endif