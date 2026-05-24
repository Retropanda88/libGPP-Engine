#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <video/video.h>
#include <font/sysfont.h>
#include <video/pixel.h>

#include <SDL/SDL.h>

static char *font[] = {
"           .      . .                    .                ..       .      .                                                     ",
"          .#.    .#.#.    . .     ...   .#. .     .      .##.     .#.    .#.     . .       .                                .   ",
"          .#.    .#.#.   .#.#.   .###.  .#..#.   .#.     .#.     .#.      .#.   .#.#.     .#.                              .#.  ",
"          .#.    .#.#.  .#####. .#.#.    ..#.   .#.#.   .#.      .#.      .#.    .#.     ..#..           ....             .#.   ",
"          .#.     . .    .#.#.   .###.   .#..    .#.     .       .#.      .#.   .###.   .#####.   ..    .####.    ..     .#.    ",
"           .            .#####.   .#.#. .#..#.  .#.#.            .#.      .#.    .#.     ..#..   .##.    ....    .##.   .#.     ",
"          .#.            .#.#.   .###.   . .#.   .#.#.            .#.    .#.    .#.#.     .#.    .#.             .##.    .      ",
"           .              . .     ...       .     . .              .      .      . .       .    .#.               ..            ",
"                                                                                                 .                              ",
"  .       .       ..     ....      .     ....     ..     ....     ..      ..                                              .     ",
" .#.     .#.     .##.   .####.    .#.   .####.   .##.   .####.   .##.    .##.     ..      ..       .             .       .#.    ",
".#.#.   .##.    .#..#.   ...#.   .##.   .#...   .#..     ...#.  .#..#.  .#..#.   .##.    .##.     .#.    ....   .#.     .#.#.   ",
".#.#.    .#.     . .#.   .##.   .#.#.   .###.   .###.     .#.    .##.   .#..#.   .##.    .##.    .#.    .####.   .#.     ..#.   ",
".#.#.    .#.      .#.    ...#.  .####.   ...#.  .#..#.    .#.   .#..#.   .###.    ..      ..    .#.      ....     .#.    .#.    ",
".#.#.    .#.     .#..   .#..#.   ..#.   .#..#.  .#..#.   .#.    .#..#.    ..#.   .##.    .##.    .#.    .####.   .#.      .     ",
" .#.    .###.   .####.   .##.     .#.    .##.    .##.    .#.     .##.    .##.    .##.    .#.      .#.    ....   .#.      .#.    ",
"  .      ...     ....     ..       .      ..      ..      .       ..      ..      ..    .#.        .             .        .     ",
"                                                                                         .                                      ",
"  ..      ..     ...      ..     ...     ....    ....     ..     .  .    ...        .    .  .    .       .   .   .   .    ..    ",
" .##.    .##.   .###.    .##.   .###.   .####.  .####.   .##.   .#..#.  .###.      .#.  .#..#.  .#.     .#. .#. .#. .#.  .##.   ",
".#..#.  .#..#.  .#..#.  .#..#.  .#..#.  .#...   .#...   .#..#.  .#..#.   .#.       .#.  .#.#.   .#.     .##.##. .##..#. .#..#.  ",
".#.##.  .#..#.  .###.   .#. .   .#..#.  .###.   .###.   .#...   .####.   .#.       .#.  .##.    .#.     .#.#.#. .#.#.#. .#..#.  ",
".#.##.  .####.  .#..#.  .#. .   .#..#.  .#..    .#..    .#.##.  .#..#.   .#.     . .#.  .##.    .#.     .#...#. .#.#.#. .#..#.  ",
".#...   .#..#.  .#..#.  .#..#.  .#..#.  .#...   .#.     .#..#.  .#..#.   .#.    .#..#.  .#.#.   .#...   .#. .#. .#..##. .#..#.  ",
" .##.   .#..#.  .###.    .##.   .###.   .####.  .#.      .###.  .#..#.  .###.    .##.   .#..#.  .####.  .#. .#. .#. .#.  .##.   ",
"  ..     .  .    ...      ..     ...     ....    .        ...    .  .    ...      ..     .  .    ....    .   .   .   .    ..    ",
"                                                                                                                                ",
" ...      ..     ...      ..     ...     .   .   .   .   .   .   .  .    . .     ....    ...             ...      .             ",
".###.    .##.   .###.    .##.   .###.   .#. .#. .#. .#. .#. .#. .#..#.  .#.#.   .####.  .###.    .      .###.    .#.            ",
".#..#.  .#..#.  .#..#.  .#..#.   .#.    .#. .#. .#. .#. .#...#. .#..#.  .#.#.    ...#.  .#..    .#.      ..#.   .#.#.           ",
".#..#.  .#..#.  .#..#.   .#..    .#.    .#. .#. .#. .#. .#.#.#.  .##.   .#.#.     .#.   .#.      .#.      .#.    . .            ",
".###.   .#..#.  .###.    ..#.    .#.    .#. .#. .#. .#. .#.#.#. .#..#.   .#.     .#.    .#.       .#.     .#.                   ",
".#..    .##.#.  .#.#.   .#..#.   .#.    .#...#.  .#.#.  .##.##. .#..#.   .#.    .#...   .#..       .#.   ..#.            ....   ",
".#.      .##.   .#..#.   .##.    .#.     .###.    .#.   .#. .#. .#..#.   .#.    .####.  .###.       .   .###.           .####.  ",
" .        ..#.   .  .     ..      .       ...      .     .   .   .  .     .      ....    ...             ...             ....   ",
"            .                                                                                                                   ",
" ..              .                  .              .             .        .        .     .       ..                             ",
".##.            .#.                .#.            .#.           .#.      .#.      .#.   .#.     .##.                            ",
" .#.      ...   .#..      ..      ..#.    ..     .#.#.    ...   .#..     ..        .    .#..     .#.     .. ..   ...      ..    ",
"  .#.    .###.  .###.    .##.    .###.   .##.    .#..    .###.  .###.   .##.      .#.   .#.#.    .#.    .##.##. .###.    .##.   ",
"   .    .#..#.  .#..#.  .#..    .#..#.  .#.##.  .###.   .#..#.  .#..#.   .#.      .#.   .##.     .#.    .#.#.#. .#..#.  .#..#.  ",
"        .#.##.  .#..#.  .#..    .#..#.  .##..    .#.     .##.   .#..#.   .#.     ..#.   .#.#.    .#.    .#...#. .#..#.  .#..#.  ",
"         .#.#.  .###.    .##.    .###.   .##.    .#.    .#...   .#..#.  .###.   .#.#.   .#..#.  .###.   .#. .#. .#..#.   .##.   ",
"          . .    ...      ..      ...     ..      .      .###.   .  .    ...     .#.     .  .    ...     .   .   .  .     ..    ",
"                                                          ...                     .                                             ",
"                                  .                                                        .      .      .        . .           ",
"                                 .#.                                                      .#.    .#.    .#.      .#.#.          ",
" ...      ...    ...      ...    .#.     .  .    . .     .   .   .  .    .  .    ....    .#.     .#.     .#.    .#.#.           ",
".###.    .###.  .###.    .###.  .###.   .#..#.  .#.#.   .#...#. .#..#.  .#..#.  .####.  .##.     .#.     .##.    . .            ",
".#..#.  .#..#.  .#..#.  .##..    .#.    .#..#.  .#.#.   .#.#.#.  .##.   .#..#.   ..#.    .#.     .#.     .#.                    ",
".#..#.  .#..#.  .#. .    ..##.   .#..   .#..#.  .#.#.   .#.#.#.  .##.    .#.#.   .#..    .#.     .#.     .#.                    ",
".###.    .###.  .#.     .###.     .##.   .###.   .#.     .#.#.  .#..#.    .#.   .####.    .#.    .#.    .#.                     ",
".#..      ..#.   .       ...       ..     ...     .       . .    .  .    .#.     ....      .      .      .                      ",
" .          .                                                             .                                                     ",
};


extern SDL_Surface *logic; 

static int font_width = 8;
static int font_height = 9;

// --- DIBUJAR UN CARÁCTER (Optimizado) ---
void sysfontDisplayChar(int x, int y, uint8_t c, unsigned int color) {
    // Evitamos procesar caracteres fuera del rango visible de la matriz
    if (c < 32 || c > 127) return; 

    int line = ((c - 32) >> 4) * font_height;
    int offset = ((c - 32) & 15) * font_width;
    int h, w;

    for (h = 0; h < font_height; h++) {
        // Obtenemos el puntero a la fila de la matriz una sola vez por línea del carácter
        const char *font_row = &font[line + h][offset];
        
        for (w = 0; w < font_width; w++) {
            // Acceso directo a memoria indexada, mucho más rápido
            if (font_row[w] == '#') {
                pixel(logic, x + w, y + h, color);
            }
        }
    }
}

// --- IMPRIMIR CADENAS DE TEXTO (Corrección de Bugs de Salto de Línea) ---
void sysfontDrawString(int x, int y, const char *string, unsigned int color) {
    int cur_x = x;
    int cur_y = y;
    int i = 0;

    // Usamos un bucle while directo sobre el puntero, más eficiente que strlen
    while (string[i] != '\0') {
        char c = string[i];

        // Soporte nativo y limpio para saltos de línea manuales '\n'
        if (c == '\n') {
            cur_x = x;
            cur_y += font_height;
            if (cur_y >= 240) break; // Límite vertical de la pantalla
            i++;
            continue;
        }

        // Control de límite de pantalla dinámico basado en la posición real de X
        // Si la siguiente letra se va a salir de los 320 píxeles, salta de línea automáticamente
        if (cur_x + font_width > 320) {
            cur_x = x;
            cur_y += font_height;
            if (cur_y >= 240) break;
        }

        // Ignora otros caracteres de control menores a espacio, excepto el '\n' ya procesado
        if ((uint8_t)c >= 32) {
            sysfontDisplayChar(cur_x, cur_y, (uint8_t)c, color);
            cur_x += (font_width - 1); // Espaciado horizontal
        }

        i++;
    }
}

// --- IMPRESIÓN CON FORMATO ESTILO PRINTF ---
void sysfontPrintF(int x, int y, unsigned int color, const char *str, ...) {
    char buffer[256]; // 256 bytes es un tamaño estándar seguro para HUD/Menús
    va_list zeiger;
    
    va_start(zeiger, str);
    vsnprintf(buffer, sizeof(buffer), str, zeiger); // vsnprintf es más seguro que vsprintf
    va_end(zeiger);
    
    sysfontDrawString(x, y, buffer, color); 
}

// --- DIBUJAR UN RECTÁNGULO LLENO ---
void sysfontDrawRect(int x, int y, int w, int h, unsigned int color) {
    int ww, hh;
    

    for (hh = 0; hh < h; hh++) {
        for (ww = 0; ww < w; ww++) {
            pixel(logic, x + ww, y + hh, color);
        }
    }
}
