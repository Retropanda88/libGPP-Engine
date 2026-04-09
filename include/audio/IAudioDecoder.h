#ifndef IAUDIODECODER_H_
#define IAUDIODECODER_H_

#include <engine/types.h>
#include <filesystem/fs.h>

class IAudioDecoder
{
public:
    virtual ~IAudioDecoder() {}

    /* abrir archivo YA abierto */
    virtual bool open(FS_FILE* file) = 0;

    /* decodificar a PCM (16-bit) */
    virtual int decode(u8* buffer, int len) = 0;

    /* volver al inicio (loop) */
    virtual void rewind() = 0;

    /* cerrar */
    virtual void close() = 0;
};

#endif