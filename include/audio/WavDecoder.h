#ifndef WAVDECODER_H_
#define WAVDECODER_H_

#include <audio/IAudioDecoder.h>

class WavDecoder : public IAudioDecoder
{
public:
    WavDecoder();
    ~WavDecoder();

    bool open(FS_FILE* file);
    int decode(u8* buffer, int len);
    void rewind();
    void close();

private:
    FS_FILE* fp;

    u32 dataStart;
    u32 dataSize;
    u32 dataPos;

    u16 channels;
    u32 sampleRate;
    u16 bitsPerSample;
    u16 audioFormat;

    bool parseWav();
};

#endif