#include <audio/MusicStream.h>
#include <string.h>

MusicStream::MusicStream()
{
    decoder = NULL;
    playing = false;
    looping = false;
}

MusicStream::~MusicStream()
{
    stop();
}

bool MusicStream::open(IAudioDecoder* dec, bool loop)
{
    decoder = dec;
    looping = loop;
    playing = false;

    return (decoder != NULL);
}

void MusicStream::play()
{
    if (!decoder)
        return;

    playing = true;
}

void MusicStream::stop()
{
    playing = false;
}

int MusicStream::read(u8* out, int len)
{
    if (!playing || !decoder)
    {
        memset(out, 0, len);
        return len;
    }

    int total = 0;

    while (total < len)
    {
        int decoded = decoder->decode(out + total, len - total);

        if (decoded <= 0)
        {
            if (looping)
            {
                decoder->rewind();
                continue;
            }
            else
            {
                // llenar con silencio lo restante
                memset(out + total, 0, len - total);
                playing = false;
                break;
            }
        }

        total += decoded;
    }

    return len;
}