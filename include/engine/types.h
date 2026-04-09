#ifndef GPP_TYPES_H_
#define GPP_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================
//  Si estamos en PS2 o PSP, usar los tipos del SDK
// ============================================================
#if defined(PSP_BUILD)
    #include <psptypes.h>   // Para PSP
#elif defined(_EE)
    #include <tamtypes.h>   // Para PS2
#else   // ======= Otros sistemas (PC, GC, etc.) =======

// ============================================================
//  Tipos enteros sin signo
// ============================================================
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

// Alias opcionales
typedef u8   U8;
typedef u16  U16;
typedef u32  U32;
typedef u64  U64;

// ============================================================
//  Tipos enteros con signo
// ============================================================
typedef signed char        s8;
typedef signed short       s16;
typedef signed int         s32;
typedef signed long long   s64;

#endif  // Fin de la condición (PS2/PSP vs otros)

// ============================================================
//  Tipos flotantes
// ============================================================
typedef float              f32;
typedef double             f64;

typedef enum {
    GPP_FALSE = 0,
    GPP_TRUE  = 1
} gpp_bool;

#ifndef NULL
#define NULL 0
#endif

#define GPP_MAX_U8   255
#define GPP_MAX_U16  65535
#define GPP_MAX_U32  4294967295U

#define GPP_PI       3.14159265358979323846f

#ifdef __cplusplus
}
#endif

#endif // GPP_TYPES_H_
