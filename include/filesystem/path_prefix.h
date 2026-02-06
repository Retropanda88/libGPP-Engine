#ifndef PATH_PREFIX_H
#define PATH_PREFIX_H

#if defined(PS2_BUILD)

#define PREFIX       "mass:/"
#define PREFIX_SAVE  "mc0:/"

#elif defined(PSP_BUILD)

#define PREFIX       "ms0:/PSP/GAME/"
#define PREFIX_SAVE  "ms0:/PSP/GAME/"

#elif defined(GC_BUILD)

#define PREFIX       "sd:/"
#define PREFIX_SAVE  "sd:/"

#elif defined(ANDROID_BUILD)

/*#define PREFIX       "/sdcard/"
#define PREFIX_SAVE  "/sdcard/"*/

#define PREFIX       ""
#define PREFIX_SAVE  ""

#elif defined(PC_BUILD)

#define PREFIX       "./"
#define PREFIX_SAVE  "./"

#else
#error Plataforma no soportada
#endif

#endif /* PATH_PREFIX_H */