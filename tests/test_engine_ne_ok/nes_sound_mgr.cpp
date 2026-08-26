/*#include <SDL/SDL.h>
#include <string.h>
#include <stdio.h>
#include "nes/types.h"
#include "nes_sound_mgr.h"
#include "nes/debug.h"
#include <stdint.h>

// Redefinimos el búfer circular para que mida exactamente en base a muestras de 16-bit
// Usamos 735 muestras por bloque (SOUND_BUF_LEN / 2) con un multiplicador amplio de amortiguación
#define SAMPLES_PER_BLOCK   735
#define RING_SAMPLES_SIZE   (SAMPLES_PER_BLOCK * 16)

// El búfer intermedio que usa el emulador para depositar las muestras
static uint8_t FakeAudioBuffer[SOUND_BUF_LEN * 4]; 
static uint32_t write_offset = 0;

// SOLUCIÓN AL "GRRR": Forzamos a la cola circular a almacenar datos nativos de 16-bit
static int16_t RingBuffer[RING_SAMPLES_SIZE];
static volatile uint32_t ring_head = 0;
static volatile uint32_t ring_tail = 0;

// Búfer de respaldo unificado a 16-bit para evitar chasquidos en los vacíos
static int16_t LastValidBuffer[SAMPLES_PER_BLOCK];
static bool has_valid_backup = false;

// ====================================================
// CALLBACK SDL 1.2: El hardware extrae sonido alineado
// ====================================================
void sdl_audio_callback(void *userdata, uint8_t *stream, int len)
{
    // SDL pide los datos en bytes (len), pero nuestro hardware trabaja a 16 bits (2 bytes por muestra)
    int samples_requested = len / sizeof(int16_t);
    int16_t *dest_stream = (int16_t*)stream;

    // Calculamos el espacio ocupado medido estrictamente en muestras de 16-bit
    uint32_t head = ring_head;
    uint32_t tail = ring_tail;
    uint32_t available_samples = (head >= tail) ? (head - tail) : (RING_SAMPLES_SIZE - tail + head);

    // Si la CPU del emulador se retrasa y no hay suficientes muestras listas
    if (available_samples < (uint32_t)samples_requested) {
        if (has_valid_backup) {
            // Repetimos de forma limpia la última onda generada en lugar de meter silencio o estática
            int to_copy = (samples_requested > SAMPLES_PER_BLOCK) ? SAMPLES_PER_BLOCK : samples_requested;
            memcpy(dest_stream, LastValidBuffer, to_copy * sizeof(int16_t));
            
            // Si el canal pide más datos de los que tiene el respaldo, el resto va a cero suave
            if (samples_requested > to_copy) {
                memset(dest_stream + to_copy, 0, (samples_requested - to_copy) * sizeof(int16_t));
            }
        } else {
            memset(stream, 0, len);
        }
        return;
    }

    // Extracción atómica de 16 bits: Se respeta el signo y la forma de la onda de la NES
    for (int i = 0; i < samples_requested; i++) {
        dest_stream[i] = RingBuffer[ring_tail];
        ring_tail = (ring_tail + 1) % RING_SAMPLES_SIZE;
    }
}

// ====================================================
// CONSTRUCTOR: Inicialización limpia del bus de audio
// ====================================================
ezx_sound_mgr::ezx_sound_mgr(bool dummy)
{
    buffer_locked = false;
    dspfd = -1; 
    memset(FakeAudioBuffer, 0, sizeof(FakeAudioBuffer));
    memset(RingBuffer, 0, sizeof(RingBuffer));
    memset(LastValidBuffer, 0, sizeof(LastValidBuffer));
    has_valid_backup = false;
    ring_head = 0;
    ring_tail = 0;

    SDL_AudioSpec wanted;
    wanted.freq     = SAMPLE_RATE;          // 22050 Hz
    wanted.format   = AUDIO_S16SYS;         // 16 bits nativos con signo
    wanted.channels = 2;                    // Mono
    wanted.samples  = 512;                  // Búfer bajo para máxima respuesta táctil y sónica
    wanted.callback = sdl_audio_callback;   
    wanted.userdata = NULL;

    if (SDL_OpenAudio(&wanted, NULL) < 0) {
        fprintf(stderr, "[AUDIO ERROR] Imposible abrir mezclador: %s\n", SDL_GetError());
    } else {
        SDL_PauseAudio(0); // Abrir compuertas del mezclador Android
    }
}

ezx_sound_mgr::~ezx_sound_mgr()
{
    SDL_CloseAudio(); 
    dspfd = -1;
}

// ====================================================
// MÉTODOS DE INTERCAMBIO CON EL EMULADOR
// ====================================================
boolean ezx_sound_mgr::lock(sound_buf_pos pos, void** buf, uint32* buf_len)
{
    if (buffer_locked) return false;
    buffer_locked = true;
    
    if (pos == SOUND_BUF_LOW) {
        write_offset = 0;
    } else {
        write_offset = SOUND_BUF_LEN;
    }
    
    *buf = (void*)(FakeAudioBuffer + write_offset);
    *buf_len = SOUND_BUF_LEN;
    
    return true;
}

void ezx_sound_mgr::unlock()
{
    if (!buffer_locked) return;
    buffer_locked = false;

    // Convertimos el búfer plano de la NES a un puntero real de 16-bit
    int16_t *src_samples = (int16_t*)(FakeAudioBuffer + write_offset);

    // Guardamos la ráfaga de respaldo en su formato correcto de 16-bit
    memcpy(LastValidBuffer, src_samples, SAMPLES_PER_BLOCK * sizeof(int16_t));
    has_valid_backup = true;

    uint32_t head = ring_head;
    uint32_t tail = ring_tail;
    uint32_t ocupado = (head >= tail) ? (head - tail) : (RING_SAMPLES_SIZE - tail + head);
    uint32_t espacio_libre = RING_SAMPLES_SIZE - ocupado - 1;

    // Si el búfer circular se encuentra al límite, adelantamos el puntero de salida (Soft Drop)
    // para liberar espacio sin romper el bloque de 16 bits
    if (espacio_libre < SAMPLES_PER_BLOCK) {
        SDL_LockAudio();
        ring_tail = (ring_tail + SAMPLES_PER_BLOCK) % RING_SAMPLES_SIZE;
        SDL_UnlockAudio();
    }

    // Inyectamos las muestras completas de 16-bit en la cola circular de SDL
    SDL_LockAudio();
    for (uint32_t i = 0; i < SAMPLES_PER_BLOCK; i++) {
        RingBuffer[ring_head] = src_samples[i];
        ring_head = (ring_head + 1) % RING_SAMPLES_SIZE;
    }
    SDL_UnlockAudio();
}

sound_mgr::sound_buf_pos ezx_sound_mgr::get_currently_playing_half()
{
    static sound_mgr::sound_buf_pos last = SOUND_BUF_HIGH;
    last = (last == SOUND_BUF_HIGH) ? SOUND_BUF_LOW : SOUND_BUF_HIGH;
    return last;
}

void ezx_sound_mgr::ezx_pause(bool pause, bool dummy)
{
    SDL_PauseAudio(pause ? 1 : 0);
}

int ezx_sound_mgr::get_sample_rate() { return SAMPLE_RATE; }
int ezx_sound_mgr::get_sample_bits() { return SAMPLE_BITS; }
boolean ezx_sound_mgr::IsNull() { return FALSE; }
*/

#include <SDL/SDL.h>
#include <string.h>
#include <stdio.h>
#include <types.h>
#include "nes_sound_mgr.h"
#include <debug.h>
#include <stdint.h>

// Si estás compilando para GameCube, incluimos las funciones nativas de byteswap de devkitPPC
#if defined(GC_BUILD)
#include <gctypes.h>
#endif

// Muestras por bloque (735 para el estándar de NES a esta frecuencia)
#define SAMPLES_PER_BLOCK   735

// OPTIMIZACIÓN CRÍTICA: Forzamos tamaño Potencia de 2 (2^14 = 16384)
#define RING_SAMPLES_SIZE   16384  
#define RING_MASK           (RING_SAMPLES_SIZE - 1)

// El búfer intermedio que usa el emulador para depositar las muestras
static uint8_t FakeAudioBuffer[SOUND_BUF_LEN * 4]; 
static uint32_t write_offset = 0;

// OPTIMIZACIÓN DE ALINEACIÓN: Forzamos alineación a 16 bytes (Quadwords)
static int16_t RingBuffer[RING_SAMPLES_SIZE] __attribute__((aligned(16)));
static int16_t LastValidBuffer[SAMPLES_PER_BLOCK] __attribute__((aligned(16)));
static bool has_valid_backup = false;

static volatile uint32_t ring_head = 0;
static volatile uint32_t ring_tail = 0;

// ====================================================
// CALLBACK CON BYTESWAP POR HARDWARE (Ultra Rápido)
// ====================================================
void sdl_audio_callback(void *userdata, uint8_t *stream, int len)
{
    int samples_requested = len / sizeof(int16_t);
    int16_t *dest_stream = (int16_t*)stream;

    uint32_t head = ring_head;
    uint32_t tail = ring_tail;
    
    uint32_t available_samples = (head - tail) & RING_MASK;

    // Si la CPU del emulador se retrasa (Búfer vacío)
    if (available_samples < (uint32_t)samples_requested) {
        if (has_valid_backup) {
            int to_copy = (samples_requested > SAMPLES_PER_BLOCK) ? SAMPLES_PER_BLOCK : samples_requested;
            
#if defined(GC_BUILD)
            // GameCube: Inversión usando operaciones optimizadas por registro
            for (int i = 0; i < to_copy; i++) {
                uint16_t val = (uint16_t)LastValidBuffer[i];
                dest_stream[i] = (int16_t)((val << 8) | (val >> 8));
            }
            if (samples_requested > to_copy) {
                int16_t last_sample = (to_copy > 0) ? dest_stream[to_copy - 1] : 0;
                for (int i = to_copy; i < samples_requested; i++) {
                    dest_stream[i] = last_sample;
                }
            }
#else
            // PS2 / PSP: Copiar el último buffer válido para evitar "clics" de estática y rellenar con silencio
            memcpy(dest_stream, LastValidBuffer, to_copy * sizeof(int16_t));
            if (samples_requested > to_copy) {
                memset(dest_stream + to_copy, 0, (samples_requested - to_copy) * sizeof(int16_t));
            }
#endif
        } else {
            memset(stream, 0, len);
        }
        return;
    }

    uint32_t samples_to_end = RING_SAMPLES_SIZE - tail;
    
#if defined(GC_BUILD)
    // --- MODO GAMECUBE: Byteswap por Hardware (Evita los cortes de audio) ---
    if ((uint32_t)samples_requested <= samples_to_end) {
        int i = 0;
        for (; i <= samples_requested - 4; i += 4) {
            uint16_t v0 = RingBuffer[tail + i];
            uint16_t v1 = RingBuffer[tail + i + 1];
            uint16_t v2 = RingBuffer[tail + i + 2];
            uint16_t v3 = RingBuffer[tail + i + 3];
            dest_stream[i]     = (int16_t)((v0 << 8) | (v0 >> 8));
            dest_stream[i + 1] = (int16_t)((v1 << 8) | (v1 >> 8));
            dest_stream[i + 2] = (int16_t)((v2 << 8) | (v2 >> 8));
            dest_stream[i + 3] = (int16_t)((v3 << 8) | (v3 >> 8));
        }
        for (; i < samples_requested; i++) {
            uint16_t val = RingBuffer[tail + i];
            dest_stream[i] = (int16_t)((val << 8) | (val >> 8));
        }
    } else {
        int idx = 0;
        uint32_t i = 0;
        for (; i < samples_to_end; i++) {
            uint16_t val = RingBuffer[tail + i];
            dest_stream[idx++] = (int16_t)((val << 8) | (val >> 8));
        }
        uint32_t rem = samples_requested - samples_to_end;
        for (i = 0; i < rem; i++) {
            uint16_t val = RingBuffer[i];
            dest_stream[idx++] = (int16_t)((val << 8) | (val >> 8));
        }
    }
#else
    // --- MODO PS2 / PSP: Pipeline original intacto por memcpy (Formato nativo) ---
    if ((uint32_t)samples_requested <= samples_to_end) {
        memcpy(dest_stream, &RingBuffer[tail], samples_requested * sizeof(int16_t));
    } else {
        memcpy(dest_stream, &RingBuffer[tail], samples_to_end * sizeof(int16_t));
        memcpy(dest_stream + samples_to_end, &RingBuffer[0], (samples_requested - samples_to_end) * sizeof(int16_t));
    }
#endif

    ring_tail = (tail + samples_requested) & RING_MASK;
}

// ====================================================
// CONSTRUCTOR: Inicialización del gestor de sonido
// ====================================================
ezx_sound_mgr::ezx_sound_mgr(bool dummy)
{
    buffer_locked = false;
    dspfd = -1; 
    memset(FakeAudioBuffer, 0, sizeof(FakeAudioBuffer));
    memset(RingBuffer, 0, sizeof(RingBuffer));
    memset(LastValidBuffer, 0, sizeof(LastValidBuffer));
    has_valid_backup = false;
    ring_head = 0;
    ring_tail = 0;

    SDL_AudioSpec wanted;
    wanted.freq     = SAMPLE_RATE;          
    
#if defined(GC_BUILD)
    wanted.format   = AUDIO_S16MSB; // Big-Endian para GameCube
#else
    wanted.format   = AUDIO_S16SYS; // Little-Endian nativo para PS2/PSP
#endif
    
    // CORRECCIÓN DEL "GRR": Forzado a Mono (1 canal). 
    // Evita el desfase de frecuencias que generaba el ruido robótico.
    wanted.channels = 1;                     
    
#if defined(GC_BUILD)
    wanted.samples  = 512;  // Tamaño optimizado para GameCube                      
#else
    wanted.samples  = 512;  // CORRECCIÓN DE PAUSAS: 512 muestras evita la inanición en PS2
#endif

    wanted.callback = sdl_audio_callback;   
    wanted.userdata = NULL;

    if (SDL_OpenAudio(&wanted, NULL) < 0) {
        fprintf(stderr, "[AUDIO ERROR] Imposible abrir mezclador: %s\n", SDL_GetError());
    } else {
        SDL_PauseAudio(0);
    }
}

ezx_sound_mgr::~ezx_sound_mgr()
{
    SDL_CloseAudio(); 
    dspfd = -1;
}

// ====================================================
// MÉTODOS DE INTERCAMBIO CON EL CORE DEL EMULADOR
// ====================================================
boolean ezx_sound_mgr::lock(sound_buf_pos pos, void** buf, uint32* buf_len)
{
    if (buffer_locked) return false;
    buffer_locked = true;
    
    if (pos == SOUND_BUF_LOW) {
        write_offset = 0;
    } else {
        write_offset = SOUND_BUF_LEN;
    }
    
    *buf = (void*)(FakeAudioBuffer + write_offset);
    *buf_len = SOUND_BUF_LEN;
    
    return true;
}

void ezx_sound_mgr::unlock()
{
    if (!buffer_locked) return;
    buffer_locked = false;

    int16_t *src_samples = (int16_t*)(FakeAudioBuffer + write_offset);

    // Copia de seguridad en Mono para el LastValidBuffer
    memcpy(LastValidBuffer, src_samples, SAMPLES_PER_BLOCK * sizeof(int16_t));
    has_valid_backup = true;

    uint32_t head = ring_head;
    uint32_t tail = ring_tail;
    uint32_t ocupado = (head - tail) & RING_MASK;
    uint32_t espacio_libre = RING_SAMPLES_SIZE - ocupado - 1;

    // Si el emulador va demasiado rápido, adelantamos el tail de forma segura
    if (espacio_libre < SAMPLES_PER_BLOCK) {
        SDL_LockAudio();
        ring_tail = (ring_tail + SAMPLES_PER_BLOCK) & RING_MASK;
        SDL_UnlockAudio();
    }

    SDL_LockAudio();
    uint32_t samples_to_end = RING_SAMPLES_SIZE - head;
    
    if (SAMPLES_PER_BLOCK <= samples_to_end) {
        memcpy(&RingBuffer[head], src_samples, SAMPLES_PER_BLOCK * sizeof(int16_t));
    } else {
        memcpy(&RingBuffer[head], src_samples, samples_to_end * sizeof(int16_t));
        memcpy(&RingBuffer[0], src_samples + samples_to_end, (SAMPLES_PER_BLOCK - samples_to_end) * sizeof(int16_t));
    }
    
    ring_head = (head + SAMPLES_PER_BLOCK) & RING_MASK;
    SDL_UnlockAudio();
}

sound_mgr::sound_buf_pos ezx_sound_mgr::get_currently_playing_half()
{
    static sound_mgr::sound_buf_pos last = SOUND_BUF_HIGH;
    last = (last == SOUND_BUF_HIGH) ? SOUND_BUF_LOW : SOUND_BUF_HIGH;
    return last;
}

void ezx_sound_mgr::ezx_pause(bool pause, bool dummy)
{
    SDL_PauseAudio(pause ? 1 : 0);
}

int ezx_sound_mgr::get_sample_rate() { return SAMPLE_RATE; }
int ezx_sound_mgr::get_sample_bits() { return SAMPLE_BITS; }
boolean ezx_sound_mgr::IsNull() { return FALSE; }