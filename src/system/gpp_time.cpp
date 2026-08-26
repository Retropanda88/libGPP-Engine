#include <system/gpp_time.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

// Si estás en GameCube o Wii, incluimos sus cabeceras nativas
#if defined(GC_BUILD)
#include <ogc/lwp_watchdog.h>
//#include <ogc/rtc.h>
#endif

// Si estás en PSP
#if defined(PSP_BUILD)
#include <psprtc.h>
#endif

/* ======================================================================== */
/* 🛠️ PARCHE EXCLUSIVO PARA PLAYSTATION 2 (MÉTODO SYSCALL SEGURO)            */
/* ======================================================================== */
#if defined(PS2_BUILD)
// 1. Definimos la estructura del reloj interno de la PS2
typedef struct {
    unsigned char stat;
    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    unsigned char pad;
    unsigned char day;
    unsigned char month;
    unsigned char year;
} sceCdCLOCK;

// 2. Invocación segura mediante la Syscall 53 del Kernel de PS2
// Esto no causará crasheos ni pantallas negras bajo ninguna circunstancia.
static int PS2_ReadClockSafe(sceCdCLOCK *clock) {
    int result;
    __asm__ volatile (
        "li $3, 53\n"      // Syscall 53 representa _sceCdReadClock en el kernel de PS2
        "move $4, %1\n"    // Mueve el puntero de la estructura al registro de argumento $4 (a0)
        "syscall\n"        // Llama a la interrupción de software del sistema operativo
        "move %0, $2\n"    // El valor de retorno se recupera del registro $2 (v0)
        : "=r" (result) 
        : "r" (clock) 
        : "$3", "$4", "$2"
    );
    return result;
}

// 3. Implementamos el localtime que el PS2SDK dejó vacío
extern "C" struct tm *localtime(const time_t *timep) {
    static struct tm t;
    sceCdCLOCK ps2_clock;

    // Llamamos a nuestro lector seguro por Syscall
    if (PS2_ReadClockSafe(&ps2_clock) != 0 && ps2_clock.stat == 0) {
        // Convertimos el formato BCD del chip a números enteros normales
        t.tm_sec  = ((ps2_clock.second >> 4) * 10) + (ps2_clock.second & 0x0F);
        t.tm_min  = ((ps2_clock.minute >> 4) * 10) + (ps2_clock.minute & 0x0F);
        t.tm_hour = ((ps2_clock.hour >> 4) * 10) + (ps2_clock.hour & 0x0F);
        t.tm_mday = ((ps2_clock.day >> 4) * 10) + (ps2_clock.day & 0x0F);
        
        // En C los meses van de 0 (enero) a 11 (diciembre)
        t.tm_mon  = (((ps2_clock.month >> 4) * 10) + (ps2_clock.month & 0x0F)) - 1; 
        
        // La PS2 da el año en 2 dígitos. struct tm requiere años transcurridos desde 1900.
        int anio_corto = ((ps2_clock.year >> 4) * 10) + (ps2_clock.year & 0x0F);
        t.tm_year = (2000 + anio_corto) - 1900; 
    } else {
        // Fallback seguro si la consola no tiene hora válida o falla el chip
        t.tm_sec = t.tm_min = t.tm_hour = 0;
        t.tm_mday = 29; t.tm_mon = 4; t.tm_year = 126; // Mayo de 2026
    }

    t.tm_wday = 0;
    t.tm_yday = 0;
    t.tm_isdst = -1;

    return &t;
}
#endif
/* ======================================================================== */


void GPP_GetSystemDateTime(GPP_DateTime *outDateTime)
{
    if (!outDateTime) return;

    // Inicializar estructura por seguridad
    memset(outDateTime, 0, sizeof(GPP_DateTime));

    /* ======================================================================== */
    /* 🎮 CONFIGURACIÓN ESPECÍFICA POR PLATAFORMA                               */
    /* ======================================================================== */

#if defined(GC_BUILD)
    outDateTime->platform_name = "Nintendo GameCube";
    
    time_t rawTime = time(NULL);
    struct tm *timeInfo = localtime(&rawTime);

#elif defined(PSP_BUILD)
    outDateTime->platform_name = "Sony PSP";
    
    time_t rawTime;
    time(&rawTime);
    struct tm *timeInfo = localtime(&rawTime);

#elif defined(PS2_BUILD)
    outDateTime->platform_name = "Sony PlayStation 2";
    
    time_t rawTime = time(NULL);
    struct tm *timeInfo = localtime(&rawTime);

#else
    // Por descarte: Android (C4droid) / Linux / PC
    outDateTime->platform_name = "Android / OS Host";
    time_t rawTime;
    time(&rawTime);
    struct tm *timeInfo = localtime(&rawTime);
#endif

    /* ======================================================================== */
    /* 📦 PROCESAMIENTO Y FORMATEO UNIFICADO                                    */
    /* ======================================================================== */
    if (timeInfo)
    {
        outDateTime->hour   = (u8)timeInfo->tm_hour;
        outDateTime->minute = (u8)timeInfo->tm_min;
        outDateTime->second = (u8)timeInfo->tm_sec;
        outDateTime->day    = (u8)timeInfo->tm_mday;
        outDateTime->month  = (u8)(timeInfo->tm_mon + 1);
        outDateTime->year   = (u16)(timeInfo->tm_year + 1900);
    }
    else
    {
        // Fallback en caso de colapso de pila de reloj en hardware viejo
        outDateTime->hour = 12; outDateTime->minute = 0; outDateTime->second = 0;
        outDateTime->day = 1;   outDateTime->month = 1;  outDateTime->year = 2026;
    }

    // Dejar los strings listos para que tu renderizador de fuentes los use directamente
    snprintf(outDateTime->text, sizeof(outDateTime->text), "%02d:%02d:%02d", 
             outDateTime->hour, outDateTime->minute, outDateTime->second);

    snprintf(outDateTime->date_text, sizeof(outDateTime->date_text), "%02d/%02d/%04d", 
             outDateTime->day, outDateTime->month, outDateTime->year);
}