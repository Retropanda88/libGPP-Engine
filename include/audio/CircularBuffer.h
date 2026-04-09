#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <engine/types.h>

class CircularBuffer
{
  public:
	CircularBuffer();
	~CircularBuffer();

	bool init(u32 size);
	void release();
	void reset();

	u32 write(const u8 * data, u32 len);
	u32 read(u8 * out, u32 len);

	u32 available();			// datos listos para leer
	u32 freeSpace();			// espacio libre

	void clear();

  private:
	  u8 * buffer;
	u32 capacity;
	u32 size;
	u32 readPos;
	u32 writePos;
};

#endif
