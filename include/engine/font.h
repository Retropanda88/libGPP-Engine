/*
 * libGPP-Engine - A lightweight static game engine for retro consoles.
 * Copyright (c) 2025 Andrés Ruiz Pérez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 or version 3.
 * https://www.gnu.org/licenses/
 */


#ifndef FONT_H_
#define FONT_H_


#ifdef __cplusplus

extern "C" {

#endif


/**
 * @brief Dibuja un solo carácter en una posición específica.
 *
 * Esta función dibuja un único carácter en las coordenadas (x, y) con un color especificado
 * en la pantalla o en una superficie SDL. El carácter se dibuja utilizando una fuente de tamaño fijo.
 *
 * @param x Coordenada X en la que se dibujará el carácter.
 * @param y Coordenada Y en la que se dibujará el carácter.
 * @param ascii Carácter ASCII que se desea dibujar.
 * @param color Color del carácter en formato ARGB.
 */
void caracter(int x, int y, const char ascii, unsigned int color);


/**
 * @brief Establece el tamaño de fuente para la impresión de texto.
 *
 * Esta función ajusta el tamaño de los caracteres utilizados por las funciones de impresión,
 * permitiendo definir el ancho (`w`) y la altura (`h`) de los caracteres. Esto puede cambiar
 * el tamaño de los textos impresos en la pantalla.
 *
 * @param w Ancho de cada carácter en píxeles.
 * @param h Alto de cada carácter en píxeles.
 */
void fontsize(int w, int h);


/**
 * @brief Imprime un texto en una posición específica con un color determinado.
 *
 * Esta función imprime una cadena de texto en las coordenadas (x, y) con un color especificado.
 * El color se utiliza para renderizar todo el texto.
 *
 * @param x Coordenada X donde se imprimirá el texto.
 * @param y Coordenada Y donde se imprimirá el texto.
 * @param text Cadena de caracteres que se desea imprimir.
 * @param color_t Color del texto en formato ARGB.
 */
void print(int x, int y, const char *text, unsigned int color_t);


/**
 * @brief Imprime un texto en una posición específica con un color determinado, soportando formato.
 *
 * Esta función es similar a `print`, pero admite un formato de texto que se pasa como una cadena
 * de caracteres y los parámetros adicionales necesarios para el formato (por ejemplo, números, texto variable).
 * Funciona de manera similar a la función `printf` en C, permitiendo imprimir texto formateado.
 *
 * @param x Coordenada X donde se imprimirá el texto.
 * @param y Coordenada Y donde se imprimirá el texto.
 * @param color_t Color del texto en formato ARGB.
 * @param s Cadena de formato similar a `printf`, con parámetros adicionales.
 */
void print_f(int x, int y, unsigned int color_t, const char *s, ...);



#ifdef __cplusplus
}
#endif



#endif /*end*/

