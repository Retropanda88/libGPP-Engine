#ifndef GPP_TIME_H
#define GPP_TIME_H

#include <engine/types.h>

// Estructura unificada para contener toda la info temporal
typedef struct {
    u8 hour;
    u8 minute;
    u8 second;
    u8 day;
    u8 month;
    u16 year;
    char text[32];      // "HH:MM:SS" pre-formateado
    char date_text[32]; // "DD/MM/YYYY" pre-formateado
    const char* platform_name; // Identificador de la consola actual
} GPP_DateTime;

#ifdef __cplusplus
extern "C" {
#endif

// Única función que llamará tu Suit Test / App
void GPP_GetSystemDateTime(GPP_DateTime *outDateTime);

#ifdef __cplusplus
}
#endif

#endif // GPP_TIME_H
