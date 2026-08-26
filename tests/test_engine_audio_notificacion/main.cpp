#include <engine/engine.h>
#include <input/Input.h>
#include <string.h>
#include <stdio.h>
#include <font/gpp_font.h> // Sistema de fuentes POO

static int isDone = 0;

// --- Sistema de Audio ---
Cmixer mixer;
CSample sndMove;
CSample sndSelect;

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (Init_Sistem("libGPP-Engine Audio Test") < 0)
        return -1;

    if (Set_Video(640, 480) < 0)
        return -1;

    Input::init();

    // Inicializamos el mezclador a 44.1kHz, estéreo
    if (!mixer.init(44100, 2, 2048))
    {
        return -1;
    }

    // Carga de recursos
    sndSelect.Load("mass:/menu/sfx/select.wav");
    sndMove.Load("mass:/menu/sfx/move.wav");
    mixer.playMusic("menu/music/background.wav", true);

    // --- Instanciamos nuestra clase de fuentes GPP_Font ---
    // Usamos el font_normal_24 por defecto a tamaño 16 y color blanco
    GPP_Font font(FONT_NORMAL_24, 16, 0xFFFFFFFF);

    // Variables de flanco para los botones de prueba
    int down_l = 0, up_l = 0, left_l = 0, right_l = 0;
    int a_l = 0, b_l = 0, x_l = 0, y_l = 0;
    int l1_l = 0, r1_l = 0, select_l = 0;

    int currentMasterVol = MAX_VOLUME;
    int currentMusicVol = MAX_VOLUME;
    bool isPausedState = false;

    // --- Variables para el sistema de Notificación Automática (Cada 10 Segundos) ---
    int notificationTimer = 0;
    const int NOTIFICATION_INTERVAL = 600; // 10 segundos * 60 FPS

    // Buffers para estado y color del texto dinámico
    char statusMessage[128] = "ESTADO: Sistema Iniciado Correctamente";
    unsigned int statusColor = 0xFF00FF00; // Verde brillante por defecto

    while (!isDone)
    {
        Input::update();

        if (Input::isDown(0, BUTTON_START))
        {
            isDone = 1;
        }

        // Lectura de botones
        int downPressed    = Input::isDown(0, BUTTON_DOWN);
        int upPressed      = Input::isDown(0, BUTTON_UP);
        int leftPressed    = Input::isDown(0, BUTTON_LEFT);
        int rightPressed   = Input::isDown(0, BUTTON_RIGHT);
        int aPressed       = Input::isDown(0, BUTTON_A);
        int bPressed       = Input::isDown(0, BUTTON_B);
        int xPressed       = Input::isDown(0, BUTTON_X);
        int yPressed       = Input::isDown(0, BUTTON_Y);
        int l1Pressed      = Input::isDown(0, BUTTON_L1);
        int r1Pressed      = Input::isDown(0, BUTTON_R1);
        int selectPressed  = Input::isDown(0, BUTTON_SELECT);

        // Generación de flancos de bajada (edge detection)
        int press_down   = (downPressed && !down_l);
        int press_up     = (upPressed && !up_l);
        int press_left   = (leftPressed && !left_l);
        int press_right  = (rightPressed && !right_l);
        int press_a      = (aPressed && !a_l);
        int press_b      = (bPressed && !b_l);
        int press_x      = (xPressed && !x_l);
        int press_y      = (yPressed && !y_l);
        int press_l1     = (l1Pressed && !l1_l);
        int press_r1     = (r1Pressed && !r1_l);
        int press_select = (selectPressed && !select_l);

        down_l   = downPressed;
        up_l     = upPressed;
        left_l   = leftPressed;
        right_l  = rightPressed;
        a_l      = aPressed;
        b_l      = bPressed;
        x_l      = xPressed;
        y_l      = yPressed;
        l1_l     = l1Pressed;
        r1_l     = r1Pressed;
        select_l = selectPressed;

        // --- SISTEMA DE NOTIFICACIÓN AUTOMÁTICA (Cada 10 Segundos) ---
        notificationTimer++;
        if (notificationTimer >= NOTIFICATION_INTERVAL)
        {
            notificationTimer = 0; // Reiniciamos el temporizador
            
            if (sndSelect.getData() != NULL) {
                mixer.playNotification(&sndSelect, false, 120, 10, 0.4f, 0.12f);
                snprintf(statusMessage, sizeof(statusMessage), ">>> NOTIFICACION EXCLUSIVA DISPARADA <<<");
                statusColor = 0xFF00FF00; // Verde brillante
            }
        }

        // 1. SFX Move
        if (press_down)
        {
            if (sndMove.getData() != NULL) {
                mixer.playChannel(&sndMove, false, 100, -1);
                snprintf(statusMessage, sizeof(statusMessage), "SFX: Move disparado (Canal Dinamico)");
                statusColor = 0xFFFFFFFF;
            }
        }

        // 2. SFX Select (Forzado en Canal 0)
        if (press_up)
        {
            if (sndSelect.getData() != NULL) {
                mixer.playChannel(&sndSelect, false, 120, 0);
                snprintf(statusMessage, sizeof(statusMessage), "SFX: Select disparado (Canal Fijo 0)");
                statusColor = 0xFFFFFFFF;
            }
        }

        // 3. FadeOut All SFX (100ms) con D-PAD IZQUIERDO
        if (press_left)
        {
            mixer.fadeOutAllChannels(100);
            snprintf(statusMessage, sizeof(statusMessage), "SFX: FadeOut All (100ms)");
            statusColor = 0xFFFFFFFF;
        }

        // 4. FadeOut All SFX (300ms) con D-PAD DERECHO
        if (press_right)
        {
            mixer.fadeOutAllChannels(300);
            snprintf(statusMessage, sizeof(statusMessage), "SFX: FadeOut All (300ms)");
            statusColor = 0xFFFFFFFF;
        }

        // 5. Pausar / Reanudar todo (PauseAll / ResumeAll)
        if (press_x)
        {
            isPausedState = !isPausedState;
            if (isPausedState)
            {
                mixer.pauseAll();
                snprintf(statusMessage, sizeof(statusMessage), "AUDIO: Pausado Globalmente");
            }
            else
            {
                mixer.resumeAll();
                snprintf(statusMessage, sizeof(statusMessage), "AUDIO: Reanudado Globalmente");
            }
            statusColor = 0xFFFFFFFF;
        }

        // 6. FadeOut de Música (3 segundos)
        if (press_y)
        {
            mixer.fadeOutMusic(3000);
            snprintf(statusMessage, sizeof(statusMessage), "MUSICA: Ejecutando FadeOut (3000ms)");
            statusColor = 0xFFFFFFFF;
        }

        // 7. Reiniciar / Reproducir Música tras FadeOut (Botón SELECT)
        if (press_select)
        {
            mixer.playMusic("menu/music/background.wav", true);
            snprintf(statusMessage, sizeof(statusMessage), "MUSICA: Reiniciada con PlayMusic");
            statusColor = 0xFFFFFFFF;
        }

        // 8. Control de Volumen Maestro (+)
        if (press_a)
        {
            currentMasterVol = (currentMasterVol + 16 > MAX_VOLUME) ? MAX_VOLUME : currentMasterVol + 16;
            mixer.setMasterVolume(currentMasterVol);
            snprintf(statusMessage, sizeof(statusMessage), "VOLUMEN MAESTRO: Subido (%d)", currentMasterVol);
            statusColor = 0xFFFFFFFF;
        }

        // 9. Control de Volumen Maestro (-)
        if (press_b)
        {
            currentMasterVol = (currentMasterVol - 16 < 0) ? 0 : currentMasterVol - 16;
            mixer.setMasterVolume(currentMasterVol);
            snprintf(statusMessage, sizeof(statusMessage), "VOLUMEN MAESTRO: Bajado (%d)", currentMasterVol);
            statusColor = 0xFFFFFFFF;
        }

        // 10. Volumen de Música (-)
        if (press_l1)
        {
            currentMusicVol = (currentMusicVol - 16 < 0) ? 0 : currentMusicVol - 16;
            mixer.setMusicVolume(currentMusicVol);
            snprintf(statusMessage, sizeof(statusMessage), "VOLUMEN MUSICA: Bajado (%d)", currentMusicVol);
            statusColor = 0xFFFFFFFF;
        }

        // 11. Volumen de Música (+)
        if (press_r1)
        {
            currentMusicVol = (currentMusicVol + 16 > MAX_VOLUME) ? MAX_VOLUME : currentMusicVol + 16;
            mixer.setMusicVolume(currentMusicVol);
            snprintf(statusMessage, sizeof(statusMessage), "VOLUMEN MUSICA: Subido (%d)", currentMusicVol);
            statusColor = 0xFFFFFFFF;
        }

        // --- RENDERIZADO VISUAL EN PANTALLA ---
        cls_rgb(20, 20, 40);

        // Título principal con tamaño mayor
        font.setSize(22);
        font.setColor(0xFFFFFFFF);
        font.drawText(40, 20, "=== BANCO DE PRUEBAS AUDIO LIBGPP ===");

        // Opciones del menú con tamaño compacto
        font.setSize(16);
        font.setColor(0xFF00FFFF);
        font.drawText(40, 55,  "[D-PAD ABAJO]    -> SFX Move (Canal Dinamico)");
        font.drawText(40, 80,  "[D-PAD ARRIBA]   -> SFX Select (Canal Fijo 0)");
        font.drawText(40, 105, "[D-PAD IZQ]      -> FadeOut All SFX (100ms)");
        font.drawText(40, 130, "[D-PAD DER]      -> FadeOut All SFX (300ms)");
        font.drawText(40, 155, "[BOTON X]        -> Pausar / Reanudar Todo");
        font.drawText(40, 180, "[BOTON Y]        -> FadeOut de Musica (3s)");
        font.drawText(40, 205, "[BOTON SELECT]   -> Reproducir / Reiniciar Musica");
        font.drawText(40, 230, "[BOTON A / B]    -> Subir / Bajar Volumen Maestro");
        font.drawText(40, 255, "[L1 / R1]        -> Bajar / Subir Volumen Musica");
        font.drawText(40, 280, "[START]          -> Salir");

        // Sección de notificaciones y estados
        font.setColor(0xFFFF9900);
        font.drawText(40, 320, "CANAL DE NOTIFICACION EXCLUSIVO (Cada 10s):");
        
        font.setSize(14);
        font.setColor(0xFF888888);
        font.drawText(40, 345, "Sidechain Ducking global sobre fondo | Sonido limpio");

        font.setSize(16);
        font.setColor(0xFF888888);
        font.drawText(40, 380, "ULTIMA ACCION:");

        // Última acción usando el método printf de la clase
        font.setColor(statusColor);
        font.printf(40, 410, "%s", statusMessage);

        Render();
        Fps_sincronizar(60);
    }

    // Limpieza de recursos
    sndMove.close();
    sndSelect.close();
    mixer.stopMusic();

    off_video();
    shoutdown_sistem();
    return 0;
}
