#ifndef _EZX_SOUND_MGR_H_
#define _EZX_SOUND_MGR_H_

// === CORRECCIÓN DE RUTAS CRÍTICAS ===
// Cambiamos < > por " " y apuntamos a los archivos reales de la NES en src/
#include "../../nes/src/types.h"
#include "../../nes/src/sound_mgr.h"

// Frecuencia de muestreo a calidad de CD
#define SAMPLE_RATE   44100

// CORRECCIÓN PARA 44100 Hz: 
#define SOUND_BUF_LEN ((SAMPLE_RATE / 60) * sizeof(int16_t))
#define SAMPLE_BITS   16

// Ahora el compilador ya sabe qué es "sound_mgr" gracias al include de arriba
class ezx_sound_mgr : public sound_mgr
{
public:
  ezx_sound_mgr(bool);
  ~ezx_sound_mgr();
  
  void reset() {}
  
  // lock down for a period of inactivity
  void freeze() {}
  void thaw() {}
  
  void clear_buffer() {};
  
  // Estos tipos (boolean, sound_buf_pos, uint32) ya se reconocerán perfectamente
  boolean lock(sound_buf_pos, void** buf, uint32* buf_len);
  void unlock();
  
  int get_sample_rate();
  int get_sample_bits();
  
  // returns SOUND_BUF_LOW or SOUND_BUF_HIGH
  sound_buf_pos get_currently_playing_half();
  
  boolean IsNull();
  
  void ezx_pause(bool, bool);
  
private:
  int dspfd;
  int buffer_locked;
//  uint8 buffer[8840];
};

#endif // _EZX_SOUND_MGR_H_