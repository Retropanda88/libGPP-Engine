#ifndef SAMPLE_H_
#define SAMPLE_H_

#include <engine/types.h>

class CSample{
    public:
          CSample();
          ~CSample();
          
          // Métodos
          bool Load(const char *filename);
          bool LoadFromMemory(const u8 *mem, u32 len);
          void close();
          void reset();
          
          // Getters
          u8* getData(){return Data;}
          int getVolume(){return volume;}
          bool getActive(){return active;}
          bool getLoop(){return loop;}
          int getSize(){return len;}
          int getPosition(){return position;}
          bool isPaused(){return paused;} // <- Añadido
          
          // Setters
          void setVolume(int volume){ this->volume = volume;}
          void setLoop(bool loop){this->loop = loop;}
          void setActive(bool active){this->active = active;}
          void setPosition(int position){this->position = position;}
          void setPaused(bool paused){this->paused = paused;} // <- Añadido
          
          void setData(u8* data){ this->Data = data; }
          void setSize(int size){ this->len = size; }
        
      private:
           u8 *Data;
           int len;
           int position;
           int active;
           bool loop;
           int volume;
           bool paused; // <- Añadido
};

#endif