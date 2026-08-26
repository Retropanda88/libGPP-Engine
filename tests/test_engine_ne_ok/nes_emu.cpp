#include <SDL/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <engine/engine.h>
#include <nes.h>
#include "nes_sound_mgr.h"

// === CONTROL DE AUDIO E INPUT NATIVO DE TU MOTOR ===
#include <audio/mixer.h>
#include <input/Input.h>

extern Cmixer mixer;        
extern int global_volume;  

// Traemos la superficie global y la función de setup de tu motor
extern SDL_Surface *logic;
// ==========================================
// CONFIGURACIÓN CONSTANTES DE PRODUCCIÓN
// ==========================================
#define SCREEN_WIDTH      320
#define SCREEN_HEIGHT     240
#define SCREEN_BPP        16

// Limitador inteligente por consola sin alterar la PS2
#if defined(GC_BUILD)
  #define TARGET_FPS      60                  // Clavado para Nintendo GameCube
  #define DELAY_FRAME     (1000 / TARGET_FPS) // ~16.6 ms por frame
#else
  #define TARGET_FPS      90                  // Mantiene tus 90 FPS intactos para PS2 y PSP
  #define DELAY_FRAME     (1000 / TARGET_FPS) // 11 ms teóricos
#endif

#define NES_WIDTH         256 // Ancho real de la imagen NES
#define SAVE_STATE_SLOT   "save_slot0.sav"

// OPTIMIZACIÓN CRÍTICA DE MEMORIA: Buffer alineado a 16 bytes
static uint16_t VideoBuffer[SCREEN_HEIGHT][SCREEN_WIDTH] __attribute__((aligned(16)));

// Instancias globales del sistema
static NES *emu               = NULL;
static ezx_sound_mgr *snd_mgr = NULL;
static SDL_Surface *nes_screen = NULL; 

// Requerido por la interfaz del núcleo del emulador
uint16 get_nesscreen_pixel_color(int x, int y)
{
    return 0;
}

// ==========================================
// FUNCIÓN DE ESCALADO DE VIDEO OPTIMIZADA
// ==========================================
void render_scaled_frame_local(SDL_Surface *screen)
{
    if (!screen) return;
    SDL_LockSurface(screen);

    // Escalado mediante punto fijo (16.16) sin divisiones
    uint32_t x_step = ((uint32_t)NES_WIDTH << 16) / (uint32_t)SCREEN_WIDTH;

    uint8_t* dest_pixels = (uint8_t*)screen->pixels;
    int dest_pitch = screen->pitch;

    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        uint16_t* dest_row = (uint16_t*)(dest_pixels + (y * dest_pitch));
        uint16_t* src_row = VideoBuffer[y];

        // Punto fijo: empezamos en el píxel 16 del buffer de la NES
        uint32_t fp_nes_x = 16 << 16; 

        // Desenrollado x4 para el pipeline MIPS de la PS2
        int x = 0;
        for (; x < SCREEN_WIDTH - 3; x += 4)
        {
            dest_row[x]     = src_row[fp_nes_x >> 16]; fp_nes_x += x_step;
            dest_row[x + 1] = src_row[fp_nes_x >> 16]; fp_nes_x += x_step;
            dest_row[x + 2] = src_row[fp_nes_x >> 16]; fp_nes_x += x_step;
            dest_row[x + 3] = src_row[fp_nes_x >> 16]; fp_nes_x += x_step;
        }

        for (; x < SCREEN_WIDTH; x++)
        {
            dest_row[x] = src_row[fp_nes_x >> 16];
            fp_nes_x += x_step;
        }
    }

    SDL_UnlockSurface(screen);
}

// ========================================================================
// CALLBACK EJECUTADO POR LA SUITE REPLOID
// ========================================================================
void run_nes_emulator() {

    printf("[NES] Deteniendo la música de la Suite...\n");
    mixer.stopMusic();
    mixer.stopAll();
    
    printf("[NES] Deteniendo subsistema de audio previo...\n");
    SDL_QuitSubSystem(SDL_INIT_AUDIO); 

    // --- PARCHE DE ENCENDIDO CRÍTICO PARA GAMECUBE ---
#if defined(GC_BUILD)
    printf("[NES] Re-inicializando subsistema de audio para GameCube...\n");
    SDL_InitSubSystem(SDL_INIT_AUDIO);
#endif
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    printf("[NES] Cambiando modo de video a 16 bits para el emulador...\n");
    if (logic) { logic = NULL; }

    nes_screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_HWSURFACE);
    if (!nes_screen) {
        printf("[NES ERROR] No se pudo setear el modo de video a 16 bits\n");
        Set_Video(); 
        SDL_InitSubSystem(SDL_INIT_AUDIO); // Asegurar encendido al volver
        mixer.init(44100, 2, 2048);
        mixer.setMasterVolume(global_volume);
        mixer.playMusic("music/music.wav", true);
        return;
    }

    // Inicializar Audio del emu
    snd_mgr = new ezx_sound_mgr(false);
    if (!snd_mgr) {
        printf("[NES ERROR] Error en snd_mgr\n");
        if (nes_screen) { SDL_FreeSurface(nes_screen); nes_screen = NULL; }
        Set_Video();
        SDL_InitSubSystem(SDL_INIT_AUDIO);
        mixer.init(44100, 2, 2048);
        mixer.setMasterVolume(global_volume);
        mixer.playMusic("music/music.wav", true);
        return;
    }

    // Inicializar CPU NES
    emu = new NES(snd_mgr);
    if (!emu) {
        printf("[NES ERROR] Error al crear CPU NES\n");
        delete snd_mgr; snd_mgr = NULL;
        if (nes_screen) { SDL_FreeSurface(nes_screen); nes_screen = NULL; }
        Set_Video();
        SDL_InitSubSystem(SDL_INIT_AUDIO);
        mixer.init(44100, 2, 2048);
        mixer.setMasterVolume(global_volume);
        mixer.playMusic("music/music.wav", true);
        return;
    }

    // Cargar ROM
    if (!emu->initialize("game.nes")) {
        printf("[NES ERROR] Rom 'game.nes' no encontrada\n");
        delete emu; emu = NULL;
        delete snd_mgr; snd_mgr = NULL;
        if (nes_screen) { SDL_FreeSurface(nes_screen); nes_screen = NULL; }
        Set_Video();
        SDL_InitSubSystem(SDL_INIT_AUDIO);
        mixer.init(44100, 2, 2048);
        mixer.setMasterVolume(global_volume);
        mixer.playMusic("music/music.wav", true);
        return;
    }

    emu->set_exsound_enable(true);
    memset(VideoBuffer, 0, sizeof(VideoBuffer));
    
    uint16_t *fb = (uint16_t*)VideoBuffer;
    
    // Limpieza inicial de mandos en la memoria interna del emu
    if (emu) {
        for(int i = 0; i < 4; i++){
            NES_pad *temp_pad = emu->get_pad(i);
            if (temp_pad) {
                temp_pad->nes_UP     = false;
                temp_pad->nes_DOWN   = false; 
                temp_pad->nes_LEFT   = false;
                temp_pad->nes_RIGHT  = false;
                temp_pad->nes_A      = false; 
                temp_pad->nes_B      = false; 
                temp_pad->nes_START  = false;
                temp_pad->nes_SELECT = false;
            }
        }
    }

    bool running = true;
    uint32_t tiempo_siguiente_frame = SDL_GetTicks();
    const uint32_t MAX_FRAMESKIP = 2; 

    while (running) {
        uint32_t tiempo_actual = SDL_GetTicks();
        uint32_t loops = 0;
        bool frame_emulated = false;

        // OBLIGATORIO PARA PS2: Vaciamos la cola de eventos de SDL de forma pasiva.
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // No hacemos nada, solo dejamos pasar los eventos del sistema.
        }

        // Actualizamos tu fachada nativa que ya maneja los mandos abiertos de la Suite
        Input::update();

        // --- BUCLE DE EMULACIÓN (CON FRAMESKIP) ---
        while (tiempo_actual > tiempo_siguiente_frame && loops < MAX_FRAMESKIP) {
            if (emu && fb) {
                NES_pad *pad_j0 = emu->get_pad(0); 
                
                if (pad_j0) {
                    // Mapeo directo usando Input::isDown (mantenido continuo)
                    pad_j0->nes_UP     = Input::isDown(0, BUTTON_UP);
                    pad_j0->nes_DOWN   = Input::isDown(0, BUTTON_DOWN); 
                    pad_j0->nes_LEFT   = Input::isDown(0, BUTTON_LEFT);
                    pad_j0->nes_RIGHT  = Input::isDown(0, BUTTON_RIGHT);
                    pad_j0->nes_A      = Input::isDown(0, BUTTON_A); 
                    pad_j0->nes_B      = Input::isDown(0, BUTTON_B); 
                    pad_j0->nes_START  = Input::isDown(0, BUTTON_START);
                    pad_j0->nes_SELECT = Input::isDown(0, BUTTON_SELECT);
                }

                emu->emulate_frame(fb);
                frame_emulated = true;
            }
            tiempo_siguiente_frame += DELAY_FRAME;
            loops++;
        }

        // Salida limpia al presionar START + SELECT al mismo tiempo
        if (Input::isDown(0, BUTTON_X) && Input::isDown(0, BUTTON_SELECT)) {
            running = false;
            break;
        }

        // Atajos de eventos adicionales (Usan isPressed porque son desencadenadores únicos)
        if (Input::isPressed(0, BUTTON_L1)) emu->saveState(SAVE_STATE_SLOT);
        if (Input::isPressed(0, BUTTON_R1)) emu->loadState(SAVE_STATE_SLOT);
        if (Input::isPressed(0, BUTTON_X))  emu->reset();

        // --- RENDERIZADO Y FRAME REFORZADO ---
        if (tiempo_actual >= tiempo_siguiente_frame - DELAY_FRAME) {
            if (!frame_emulated && emu && fb) {
                NES_pad *pad_j0 = emu->get_pad(0);
                if (pad_j0) {
                    pad_j0->nes_UP     = Input::isDown(0, BUTTON_UP);
                    pad_j0->nes_DOWN   = Input::isDown(0, BUTTON_DOWN); 
                    pad_j0->nes_LEFT   = Input::isDown(0, BUTTON_LEFT);
                    pad_j0->nes_RIGHT  = Input::isDown(0, BUTTON_RIGHT);
                    pad_j0->nes_A      = Input::isDown(0, BUTTON_A); 
                    pad_j0->nes_B      = Input::isDown(0, BUTTON_B); 
                    pad_j0->nes_START  = Input::isDown(0, BUTTON_START);
                    pad_j0->nes_SELECT = Input::isDown(0, BUTTON_SELECT);
                }
                emu->emulate_frame(fb);
            }
            
            render_scaled_frame_local(nes_screen);
            SDL_Flip(nes_screen);
            
            tiempo_siguiente_frame += DELAY_FRAME;
        }
        
        if (tiempo_actual > tiempo_siguiente_frame + 100) {
            tiempo_siguiente_frame = tiempo_actual;
        }
    }

    // --- CLEANUP Y RETORNO SEGURO A TU SUITE (CORREGIDO) ---
    printf("[NES] Liberando componentes del emulador...\n");
    SDL_PauseAudio(1); // Pausar antes de destruir

    if (emu)     { delete emu; emu = NULL; }
    if (snd_mgr) { delete snd_mgr; snd_mgr = NULL; }

    // ¡SOLUCIÓN DE FUGA DE MEMORIA CRÍTICA! 
    // Liberamos la superficie de video intermedia antes de restaurar el motor
    if (nes_screen) {
        SDL_FreeSurface(nes_screen);
        nes_screen = NULL;
    }

    printf("[NES] Restaurando modo de video de la Suite a 32 bits...\n");
    Set_Video(); 

    printf("[NES] Levantando de nuevo el driver de audio de SDL...\n");
#if !defined(GC_BUILD)
    // En las Play se había quedado apagado, lo volvemos a iniciar
    SDL_InitSubSystem(SDL_INIT_AUDIO); 
#endif
    
    mixer.init(44100, 2, 2048);
    mixer.setMasterVolume(global_volume);
    mixer.playMusic("music/music.wav", true);
}