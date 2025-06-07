#ifndef JUEGO_H_INCLUDED
#define JUEGO_H_INCLUDED

#include "common.h"

// Funciones de control del juego
void jugar(SDL_Window *ventana, SDL_Renderer *, TTF_Font *,TTF_Font *, sCelda **, Archivo_conf , int *, int );
void limpiarTodosLosRecursos(SDL_Window *, SDL_Renderer *,TTF_Font *,TTF_Font *, sCelda **, Archivo_conf );

#endif // JUEGO_H_INCLUDED
