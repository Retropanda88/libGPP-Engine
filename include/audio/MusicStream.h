#ifndef MUSICSTREAM_H_
#define MUSICSTREAM_H_

#include <engine/types.h>
#include <audio/IAudioDecoder.h>

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
    bool playing;
    bool looping;
};

#endif