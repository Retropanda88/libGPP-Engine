#ifndef SAMPLE_H_
#define SAMPLE_H_

#include <engine/types.h>

class CSample{
	public:
	      CSample();
	      ~CSample();
	      
	      //tetodos
	      bool Load(const char *filename);
	      bool LoadFromMemory(const u8 *mem, u32 len);
	      void close();
	      void reset();
	      
	      //geters
	      u8* getData(){return Data;}
	      int getVolume(){return volume;}
	      bool getActive(){return active;}
	      bool getLoop(){return loop;}
	      int getSize(){return len;}
	      int getPosition(){return position;}
	      //sters
	      void setVolume(int volume){ this->volume = volume;}
	      void setLoop(bool loop){this->loop = loop;}
	      void setActive(bool active){this->active = active;}
	      void setPosition(int position){this->position = position;}
	      
	      
	      
	  private:
	       u8 *Data;
	       int len;
	       int position;
	       int active;
	       bool loop;
	       int volume;
};


#endif /*the end*/