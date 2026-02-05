#ifndef GFXTEXTURE_H
#define GFXTEXTURE_H

#include <SDL/SDL.h>
#include <string>
#include <engine/types.h>				// Definiciones de u8, u32, etc.

/**
 * @class GfxTexture
 * @brief Clase wrapper para manejo de texturas y superficies SDL.
 *
 * Permite:
 *  - Cargar imágenes desde archivo o memoria.
 *  - Rellenar superficies con colores sólidos, patrones y gradientes.
 *  - Aplicar rotación, escalado y transparencia.
 *  - Renderizar texturas en pantalla.
 */
class GfxTexture
{
  public:
	/** @brief Constructor. Inicializa variables y punteros a NULL. */
	GfxTexture();

	/** @brief Destructor. Libera memoria usada por las superficies. */
	~GfxTexture();

	/**
     * @brief Inicializa una superficie vacía con el tamaño dado.
     * @param w Ancho en píxeles.
     * @param h Alto en píxeles.
     * @return true si se creó correctamente, false si hubo error.
     */
	bool init(int w, int h);

	/**
     * @brief Carga una imagen desde un archivo.
     * @param filename Ruta del archivo.
     * @return true si la carga fue exitosa, false si hubo error.
     */
	bool load_image(const char *filename);

	/**
     * @brief Carga una imagen desde un buffer en memoria.
     * @param buffer Puntero al inicio del buffer.
     * @param len Tamaño en bytes del buffer.
     * @return true si la carga fue exitosa, false si hubo error.
     */
	bool load_frommem(u8 * buffer, int len);

	/**
 * @brief Carga una superficie existente como textura.
 * @param surf Puntero a la superficie SDL que se quiere usar.
 * @return true si se cargó correctamente.
 */
	bool load_from_surface(SDL_Surface * surf);

	/**
     * @brief Rellena la superficie con un color sólido.
     * @param r Componente roja.
     * @param g Componente verde.
     * @param b Componente azul.
     */
	void fill(u8 r, u8 g, u8 b);

	/**
     * @brief Rellena con un patrón de tablero de ajedrez usando colores RGB.
     * @param r1 Componente roja del primer color.
     * @param g1 Componente verde del primer color.
     * @param b1 Componente azul del primer color.
     * @param r2 Componente roja del segundo color.
     * @param g2 Componente verde del segundo color.
     * @param b2 Componente azul del segundo color.
     * @param block_size Tamaño de cada bloque.
     */
	void fill_checkerboard(u8 r1, u8 g1, u8 b1, u8 r2, u8 g2, u8 b2, int block_size);

	/**
     * @brief Rellena con patrón de tablero de ajedrez usando colores Uint32.
     * @param c1 Primer color.
     * @param c2 Segundo color.
     * @param block_size Tamaño de cada bloque.
     */
	void fill_checkerboard_colors(u32 c1, u32 c2, int block_size);

	/**
     * @brief Aplica un gradiente horizontal entre dos colores.
     * @param color1 Color inicial.
     * @param color2 Color final.
     */
	void fill_horizontal_gradient(Uint32 color1, Uint32 color2);

	/**
     * @brief Aplica un gradiente vertical entre dos colores.
     * @param color1 Color inicial.
     * @param color2 Color final.
     */
	void fill_vertical_gradient(Uint32 color1, Uint32 color2);

	/**
     * @brief Aplica transparencia a un color clave (color key).
     * @param r Componente roja.
     * @param g Componente verde.
     * @param b Componente azul.
     */
	void applyTransparency(Uint8 r, Uint8 g, Uint8 b);


	void rotozoom();			// /< Aplica rotación y escala a la textura

	/**
     * @brief Renderiza la textura en la superficie destino.
     * @param dst Superficie donde se dibujará.
     */
	void render(SDL_Surface * dst);

	/**
     * @brief Define la posición donde se renderizará.
     * @param x Coordenada X.
     * @param y Coordenada Y.
     */
	void set_position(int x, int y);

	/**
     * @brief Ajusta la transparencia global.
     * @param alpha Valor entre 0 (invisible) y 255 (opaco).
     */
	void set_alpha(u8 alpha);

	/**
     * @brief Ajusta el escalado.
     * @param percent Escala relativa (1.0 = tamaño original).
     */
	void set_scale(float percent);

	/**
     * @brief Ajusta la rotación.
     * @param angleDegrees Ángulo en grados.
     */
	void set_rotation(float angleDegrees);
	

	/**
     * @brief Obtiene el ancho de la superficie base.
     */
	int get_w() const
	{
		return work_surface ? work_surface->w : 0;
	}

	/**
     * @brief Obtiene el alto de la superficie base.
     */
	int get_h() const
	{
		return work_surface ? work_surface->h : 0;
	}

	/**
 * @brief Obtiene la superficie interna de la textura.
 * @return Puntero a la superficie SDL.
 * @note Solo leer/modificar si sabes lo que haces.
 */
	SDL_Surface *get_surface() const
	{
		return work_surface;
	}

	/**
 * @brief Asigna directamente una superficie SDL a la textura.
 * @param src Puntero a la superficie SDL que se quiere usar.
 * @note La clase toma posesión de la superficie y liberará la memoria al destruirse.
 */
	void set_surface(SDL_Surface * src, int x, int y);

  private:
	void free_surface(SDL_Surface * &surf);	// /< Libera memoria de una
	// superficie.
	void update_pixels();		// /< Actualiza puntero rápido a píxeles.

	SDL_Surface *surface;		// /< Superficie procesada (rotada/escalada).
	SDL_Surface *work_surface;	// /< Superficie base editable.
	u32 *pixels;				// /< Puntero rápido a píxeles para
	// edición.
	int x, y;					// /< Posición de renderizado.
	u8 alpha;					// /< Transparencia global.
	float rotation;				// /< Ángulo de rotación.
	float scale;				// /< Escala relativa.
};

#endif // GFXTEXTURE_H
