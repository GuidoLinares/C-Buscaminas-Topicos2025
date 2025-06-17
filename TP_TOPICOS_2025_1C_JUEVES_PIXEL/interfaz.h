#ifndef INTERFAZ_H_INCLUDED
#define INTERFAZ_H_INCLUDED

#include "common.h"

// Funciones de renderizado SDL
void dibujarTablero(SDL_Renderer *renderizador, int dimensiones);

// FUNCIÓN UNIFICADA (NUEVA FIRMA)
void dibujarCeldas(SDL_Renderer* renderizador, sCelda** matriz, int dimensiones, TTF_Font* fuente, sEstadoCheat* estadoCheat);

// FUNCIÓN AUXILIAR NUEVA
void renderizarCeldaIndividual(SDL_Renderer* renderizador, SDL_Rect rectCelda, sCelda* celda, TTF_Font* fuente, int aplicarXray);

void dibujarHeader(SDL_Renderer* renderer, TTF_Font* fuente, int minasRestantes, int anchoVentana, sEstadoCheat* estadoCheat);

int verificarVictoria(sCelda**, int, int);
void mostrarVictoria(SDL_Renderer*, TTF_Font*, int);
void mostrarGameOver(SDL_Renderer*, TTF_Font*, int);


#endif // INTERFAZ_H_INCLUDED
