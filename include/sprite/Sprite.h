#ifndef SPRITE_H
#define SPRITE_H

#include <cstdlib>
#include <string>
#include <math.h>
#include <SDL/SDL.h>
#include <ENGINE/types.h>

/**
 * @file Sprite.h
 * @brief Definición de la clase Sprite y SpriteEffects para manejo de animaciones y efectos en 2D usando SDL.
 */

#define SPRITE_MAX(a,b) ((a > b) ? a : b) ///< Devuelve el valor máximo.
#define SPRITE_MIN(a,b) ((a < b) ? a : b) ///< Devuelve el valor mínimo.

#define FLIP_HORIZONTAL 1 ///< Indica volteo horizontal.
#define FLIP_VERTICAL   2 ///< Indica volteo vertical.

class SpriteEffects;

/**
 * @class Sprite
 * @brief Representa un sprite animado con soporte para múltiples frames y efectos gráficos.
 *
 * Permite:
 * - Cargar sprites desde archivo o superficie.
 * - Controlar animaciones y velocidades.
 * - Rotar, voltear y escalar.
 * - Detectar colisiones por rectángulo o píxel.
 * - Acceder y modificar píxeles individuales.
 */
class Sprite {
public:
    /** @brief Constructor por defecto. */
    Sprite();

    /** @brief Destructor. Libera recursos asociados al sprite. */
    virtual ~Sprite();

    /**
     * @brief Constructor que carga un sprite desde un archivo.
     * @param file Ruta del archivo de imagen.
     * @param frames Número de frames que contiene.
     * @param speed Velocidad de animación (ms por frame).
     */
    Sprite(const char *file, int frames, int speed);
    
    Sprite(u8* buffer, int len, int frames, int speed);

    /**
     * @brief Constructor que inicializa desde una superficie SDL.
     * @param surface Superficie SDL ya cargada.
     * @param maxFrames Número máximo de frames.
     * @param animationSpeed Velocidad de animación (ms por frame).
     */
    Sprite(SDL_Surface* surface, int maxFrames, int animationSpeed);

    /**
     * @brief Carga un sprite desde un archivo.
     * @param file Ruta del archivo de imagen.
     * @param frames Número de frames.
     * @param speed Velocidad de animación (ms por frame).
     */
    void load(const char *file, int frames, int speed);

    /**
     * @brief Obtiene la superficie del frame actual.
     * @return Puntero a la superficie SDL correspondiente al frame actual.
     */
    SDL_Surface* getCurrentFrame();

    /**
     * @brief Dibuja el sprite en un buffer destino.
     * @param buffer Superficie destino.
     * @param x Posición X en pantalla.
     * @param y Posición Y en pantalla.
     * @return Puntero al propio sprite (para encadenar llamadas).
     */
    Sprite* draw(SDL_Surface* buffer, int x, int y);

    /**
     * @brief Define un color como transparente.
     * @param color Color en formato Uint32.
     * @return Puntero al propio sprite.
     */
    Sprite* setTransparency(Uint32 color);

    /**
     * @brief Define un color como transparente usando componentes RGB.
     * @param r Componente roja.
     * @param g Componente verde.
     * @param b Componente azul.
     * @return Puntero al propio sprite.
     */
    Sprite* setTransparency(Uint8 r, Uint8 g, Uint8 b);

    /** @brief Cambia la velocidad de animación (ms por frame). */
    Sprite* setSpeed(int animationSpeed);

    /** @brief Obtiene la velocidad de animación actual. */
    int getSpeed();

    /** @brief Inicia la animación desde el frame actual. */
    Sprite* start();

    /** @brief Reinicia la animación desde el primer frame. */
    Sprite* restart();

    /** @brief Avanza la animación al siguiente frame según la velocidad configurada. */
    Sprite* animate();

    /**
     * @brief Define si la animación debe volver al inicio al terminar.
     * @param loop true para reiniciar desde el principio, false para invertir la dirección de animación.
     */
    Sprite* setLoopToBegin(bool loop);

    /** @brief Indica si la animación está en ejecución. */
    bool running();

    /** @brief Detiene la animación. */
    Sprite* stop();

    /** @brief Indica si la instancia es un sprite válido. */
    bool isSprite();

    /** @brief Obtiene el índice del frame actual. */
    int getFrame();

    /** @brief Obtiene el iterador interno de frames. */
    int getFrameIterator();

    /** @brief Obtiene el número máximo de frames. */
    int getMaxFrames();

    /** @brief Obtiene el ancho de un frame. */
    int getWidth();

    /** @brief Obtiene el alto de un frame. */
    int getHeight();

    /** @brief Define el ancho de un frame. */
    Sprite* setWidth(int width);

    /** @brief Define el alto de un frame. */
    Sprite* setHeight(int height);

    /** @brief Obtiene el ancho total de la superficie del sprite. */
    int getSpriteWidth();

    /** @brief Obtiene el alto total de la superficie del sprite. */
    int getSpriteHeight();

    /** @brief Compara si dos sprites son iguales. */
    bool equals(Sprite compare);

    /** @brief Obtiene la superficie completa del sprite. */
    SDL_Surface* getSurface();

    /** @brief Asigna una superficie SDL como sprite. */
    Sprite* setSurface(SDL_Surface* surface);

    /** @brief Libera la memoria asociada al sprite. */
    Sprite* destroy();

    /** @brief Verifica si un píxel específico es transparente. */
    bool isTransparentPixel(int x, int y);

    /** @brief Marca un píxel específico como transparente. */
    Sprite* setTransparentPixel(int x, int y);

    /** @brief Obtiene el valor de un píxel (32 bits). */
    Uint32 getPixel(int x, int y);

    /** @brief Establece el valor de un píxel (32 bits). */
    int setPixel(int x, int y, Uint32 color);

    /** @brief Establece el valor de un píxel usando RGB. */
    int setPixel(int x, int y, Uint8 red, Uint8 green, Uint8 blue);

    /** @brief Obtiene el valor de un píxel (8 bits). */
    Uint8 getPixel8(int x, int y);

    /** @brief Establece el valor de un píxel (8 bits). */
    int setPixel8(int x, int y, Uint8 color);

    /** @brief Obtiene el valor de un píxel (16 bits). */
    Uint16 getPixel16(int x, int y);

    /** @brief Establece el valor de un píxel (16 bits). */
    int setPixel16(int x, int y, Uint16 color);

    /** @brief Obtiene el valor de un píxel (32 bits). */
    Uint32 getPixel32(int x, int y);

    /** @brief Establece el valor de un píxel (32 bits). */
    int setPixel32(int x, int y, Uint32 color);

    /**
     * @brief Obtiene una sub-superficie rectangular del sprite.
     * @param x Coordenada X inicial.
     * @param y Coordenada Y inicial.
     * @param width Ancho del rectángulo.
     * @param height Alto del rectángulo.
     * @return Superficie SDL recortada.
     */
    SDL_Surface* getRect(int x, int y, int width, int height);

    /** @brief Detecta colisión rectangular entre este sprite y otro. */
    bool rectCollide(int x1, int y1, Sprite &spriteB, int x2, int y2);

    /** @brief Detecta colisión por píxeles entre este sprite y otro. */
    bool pixelCollide(int x1, int y1, Sprite &spriteB, int x2, int y2);

    /** @brief Rota el sprite 90 grados. */
    Sprite* rotate90();

    /** @brief Rota el sprite 180 grados. */
    Sprite* rotate180();

    /** @brief Rota el sprite 270 grados. */
    Sprite* rotate270();

    /** @brief Voltea horizontalmente el sprite. */
    Sprite* flipHorizontal();

    /** @brief Voltea verticalmente el sprite. */
    Sprite* flipVertical();

    /** @brief Aplica un efecto de desvanecimiento. */
    Sprite* fade(float fade);

    /** @brief Invierte el orden de los frames en la animación. */
    Sprite* reverseAnimation();

    /** @brief Escala el sprite proporcionalmente. */
    Sprite* zoom(float z);

    /** @brief Estira el sprite horizontalmente. */
    Sprite* stretchX(float x);

    /** @brief Estira el sprite verticalmente. */
    Sprite* stretchY(float y);

    /** @brief Estira el sprite en ambos ejes. */
    Sprite* stretch(float x, float y);

private:
    bool loaded;               ///< Indica si el sprite está cargado.
    bool run;                  ///< Indica si la animación está activa.
    Uint32 speed;               ///< Velocidad de animación (ms por frame).
    Uint32 width;               ///< Ancho de un frame.
    Uint32 height;              ///< Alto de un frame.
    Uint32 index;               ///< Frame actual.
    Uint32 indexIterator;       ///< Iterador interno de frames.
    bool loopToBeginning;       ///< Control de bucle de animación.
    Uint32 maxFrames;           ///< Número máximo de frames.
    Uint32 lastAnimated;        ///< Marca de tiempo de la última animación.
    SDL_Surface* sprite;        ///< Superficie SDL del sprite.
};

/**
 * @class SpriteEffects
 * @brief Clase auxiliar para aplicar efectos gráficos sobre sprites.
 */
class SpriteEffects {
public:
    static SDL_Surface* getRect(Sprite &sprite, int x, int y, int width, int height);
    static bool rectCollide(Sprite &spriteA, int x1, int y1, Sprite &spriteB, int x2, int y2);
    static bool pixelCollide(Sprite &spriteA, int x1, int y1, Sprite &spriteB, int x2, int y2);
    static void rotate90(Sprite &sprite);
    static void rotate180(Sprite &sprite);
    static void rotate270(Sprite &sprite);
    static void flipHorizontal(Sprite &sprite);
    static void flipVertical(Sprite &sprite);
    static void rotate(Sprite &sprite, int dir);
    static void flip(Sprite &sprite, int dir);
    static void fade(Sprite &sprite, float fade);
    static void reverseAnimation(Sprite &sprite);
    static void zoom(Sprite &sprite, float x);
    static void stretchX(Sprite &sprite, float x);
    static void stretchY(Sprite &sprite, float y);
    static void stretch(Sprite &sprite, float x,float y);
    static Uint32 getPixel(SDL_Surface* sprite, int x, int y);
    static int setPixel(SDL_Surface* sprite, int x, int y, Uint32 color);
};

#endif // SPRITE_H
