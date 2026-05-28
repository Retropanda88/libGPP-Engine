#include <system/gpp_time.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

// Si estás en GameCube o Wii, incluimos sus cabeceras nativas
#if defined(GC_BUILD)
#include <ogc/lwp_watchdog.h>
#include <ogc/rtc.h>
#endif

// Si estás en PSP
#if defined(PSP_BUILD)
#include <psprtc.h>
#endif

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
    
    // Aunque localtime() funciona en libogc, leer directamente el RTC o usar 
    // la hora corregida por hardware asegura precisión total si el chip Unix falla.
    time_t rawTime = time(NULL);
    struct tm *timeInfo = localtime(&rawTime);

#elif defined(PSP_BUILD)
    outDateTime->platform_name = "Sony PSP";
    
    // La PSP tiene un módulo de reloj de alta precisión (pspRtc)
    // Usamos el método estándar que se apoya en la newlib de la PSP
    time_t rawTime;
    time(&rawTime);
    struct tm *timeInfo = localtime(&rawTime);

#elif defined(PS2_BUILD)
    outDateTime->platform_name = "Sony PlayStation 2";
    
    // Nota: Asegúrate de tener cargado 'sysclib' o 'cdvd.irx' en tu init de PS2
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
