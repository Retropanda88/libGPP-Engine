
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <engine/font.h>

#define MAX_LINES 20
#define MAX_LINE_LENGTH 256
#define LINE_SPACING 12
#define ADD_LINE_INTERVAL 500  // milisegundos entre líneas nuevas

char lines[MAX_LINES][MAX_LINE_LENGTH];
int line_count = 0;
int lines_to_show = 0;
Uint32 last_add_time = 0;

void Init_Log()
{
	int i;
    for (i = 0; i < MAX_LINES; i++) {
        lines[i][0] = '\0';
    }
    line_count = 0;
    lines_to_show = 0;
 }


void Write_Log(const char *Log, ...)
{
    if (!Log) return;

    int i;

    char buffer[MAX_LINE_LENGTH];
    va_list args;
    va_start(args, Log);
    vsnprintf(buffer, sizeof(buffer), Log, args);
    va_end(args);

    if (line_count >= MAX_LINES) {
        for (i = 1; i < MAX_LINES; i++) {
            strncpy(lines[i - 1], lines[i], MAX_LINE_LENGTH);
        }
        strncpy(lines[MAX_LINES - 1], buffer, MAX_LINE_LENGTH - 1);
        lines[MAX_LINES - 1][MAX_LINE_LENGTH - 1] = '\0';
    } else {
        strncpy(lines[line_count], buffer, MAX_LINE_LENGTH - 1);
        lines[line_count][MAX_LINE_LENGTH - 1] = '\0';
        line_count++;
    }
}


void save_Log()
{
    FILE *fd = fopen("log.txt", "w");
    if (!fd) return;
    int i;
    for (i = 0; i < line_count; i++) {
        fprintf(fd, "%s\n", lines[i]);
    }
    fclose(fd);
}


void print_log()
{
    //SDL_FillRect(screen, NULL, 0);

    int max_visible = 240 / LINE_SPACING;
    int start = (lines_to_show > max_visible) ? (lines_to_show - max_visible) : 0;

    int y = 10;
    int i;
    for (i = start; i < lines_to_show; i++) {
        print_f(10, y, 0xFFFFFFFF, "%s", lines[i]);
        y += LINE_SPACING;
    }
}

// Agrega automáticamente nuevas líneas al log visual (controla la velocidad)
void update_log_scroll()
{
    Uint32 now = SDL_GetTicks();
    if (lines_to_show < line_count && now - last_add_time > ADD_LINE_INTERVAL) {
        lines_to_show++;
        last_add_time = now;
    }
}
