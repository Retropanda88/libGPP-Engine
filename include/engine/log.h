/*
 * libGPP-Engine - A lightweight static game engine for retro consoles.
 * Copyright (c) 2025 Andrés Ruiz Pérez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 or version 3.
 * https://www.gnu.org/licenses/
 */

#ifndef LOG_H_
#define LOG_H_


#ifdef __cplusplus

extern "C" {

#endif


/**
 * @brief Inicializa el sistema de log.
 * 
 * Debe llamarse antes de usar cualquier otra función de log.
 * Puede reservar memoria o inicializar estructuras internas.
 */
void Init_Log();

/**
 * @brief Escribe un mensaje al log.
 * 
 * Esta función acepta una cadena con formato como printf.
 * 
 * @param log Cadena de formato estilo printf.
 * @param ... Argumentos variables según el formato.
 */
void Write_Log(const char *log, ...);



/**
 * @brief Guarda el log actual en un archivo de texto.
 * 
 * Escribe el contenido del log en un fichero predefinido o configurado internamente.
 */
void save_Log();


void print_log();


void update_log_scroll();





#ifdef __cplusplus
}
#endif



#endif