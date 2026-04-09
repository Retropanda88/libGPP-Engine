#include <audio/CircularBuffer.h>
#include <stdlib.h>
#include <string.h>

CircularBuffer::CircularBuffer()
{
	buffer = NULL;
	size = 0;
	readPos = writePos = 0;
}

CircularBuffer::~CircularBuffer()
{
	release();
}

void CircularBuffer::clear()
{
	readPos = 0;
	writePos = 0;
	size = 0;
}

bool CircularBuffer::init(u32 size)
{
	buffer = (u8 *) malloc(size);
	if (!buffer)
		return false;

	capacity = size;
	this->size = 0;
	readPos = 0;
	writePos = 0;

	return true;
}

void CircularBuffer::release()
{
	if (buffer)
	{
		free(buffer);
		buffer = NULL;
	}
}

void CircularBuffer::reset()
{
    readPos = writePos = 0;
    size = 0; // 👈 IMPORTANTE
}

u32 CircularBuffer::write(const u8 * data, u32 len)
{
    u32 free = freeSpace();
    if (len > free)
        len = free;

    for (u32 i = 0; i < len; i++)
    {
        buffer[writePos++] = data[i];

        if (writePos >= capacity)   // ✅ CORRECTO
            writePos = 0;
    }

    size += len; // 👈 FALTABA ESTO

    return len;
}


u32 CircularBuffer::read(u8 * out, u32 len)
{
    u32 avail = available();
    if (len > avail)
        len = avail;

    for (u32 i = 0; i < len; i++)
    {
        out[i] = buffer[readPos++];

        if (readPos >= capacity)   // ✅ CORRECTO
            readPos = 0;
    }

    size -= len; // 👈 FALTABA ESTO

    return len;
}

u32 CircularBuffer::available()
{
	return size;
}

u32 CircularBuffer::freeSpace()
{
	return capacity - size;
}
