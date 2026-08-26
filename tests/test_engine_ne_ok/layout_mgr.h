#ifndef LAYOUT_MGR_H
#define LAYOUT_MGR_H

#include <engine/engine.h>
#include <system/gpp_time.h>

// Forzamos la inclusión para heredar los tipos de datos y macros en orden secuencial
#include "interfaces.h" 

inline void draw_advanced_panel(SDL_Surface* s, int x, int y, int w, int h, u32 color_borde, u32 color_fondo, const char* title) {
    // 1. Renderizar geometría interna de fondo oscuro
    fill_rect(s, x + 4, y, w - 4, h, color_fondo);
    fill_triangle_fast(s, x + 4, y, x, y + 4, x + 4, y + 4, color_fondo);
    fill_triangle_fast(s, x, y + 4, x, y + h, x + 4, y + h, color_fondo);

    // 2. Líneas perimetrales cerradas al 100% (Sello total de esquinas)
    fill_rect(s, x + 4, y, w - 4, 1, color_borde);          // Arriba
    fill_rect(s, x, y + 4, 1, h - 4, color_borde);          // Izquierda
    fill_rect(s, x + w - 1, y, 1, h, color_borde);          // Derecha
    fill_rect(s, x, y + h - 1, w, 1, color_borde);          // Abajo

    // 3. CORRECCIÓN MATEMÁTICA PARA LA FUENTE DEL SISTEMA
    if (title != NULL) {
        int len = strlen(title);
        int text_w = (len * 6); // 6 píxeles de ancho por carácter estándar
        
        // Creamos una máscara de fondo negro exacta de 8 píxeles de alto, 
        // centrada verticalmente sobre la línea superior (y - 4) para cortarla limpiamente.
        fill_rect(s, x + 10, y - 4, text_w + 4, 8, color_rgb(5, 8, 12));
        
        // CALIBRACIÓN CRÍTICA: Cambiamos a 'y - 4'. Al restarle píxeles, compensamos el comportamiento
        // de dibujo de tu motor, forzando a que la base de las letras se alinee perfectamente 
        // en el centro vertical de la pestaña.
        sysfontDrawString(x + 12, y - 4, title, color_rgb(0, 255, 255));
    }
}



inline void render_unified_top_bar(SDL_Surface* dest, GPP_DateTime& systemClock, float& marqueeX, const char* marqueeText) {
    sysfontDrawString(10, HUD_SYSTEM_TOP, "TEST SUITE PLATFORM", color_rgb(0, 255, 255));
    sysfontDrawString(245, HUD_SYSTEM_TOP, systemClock.text, 0x00B4B4FF);

    u32 cyan_hud = color_rgb(0, 255, 255);
    u32 magenta_hud = color_rgb(220, 0, 255);
    
    fill_rect(dest, 8, HUD_LINE_Y, 140, 2, cyan_hud);
    fill_rect(dest, 148, HUD_LINE_Y, 164, 2, magenta_hud);
}

inline void render_unified_marquee(SDL_Surface* dest, float& marqueeX, const char* marqueeText) {
    int marquee_base_y = 212;

    fill_rect(dest, 0, marquee_base_y, 320, 28, color_rgb(4, 6, 10)); 
    
    fill_rect(dest, 8, marquee_base_y + 4, 304, 16, color_rgb(12, 16, 24));
    fill_rect(dest, 8, marquee_base_y + 4, 304, 1, color_rgb(35, 45, 60));
    fill_rect(dest, 8, marquee_base_y + 19, 304, 1, color_rgb(35, 45, 60));

    sysfontDrawString((int)marqueeX, marquee_base_y + 8, marqueeText, color_rgb(0, 255, 255));

    marqueeX -= 1.2f;
    if (marqueeX < -580.0f) {
        marqueeX = 320.0f;
    }
}

#endif
