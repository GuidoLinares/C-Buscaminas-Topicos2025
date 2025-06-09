#ifndef JUEGO_H_INCLUDED
#define JUEGO_H_INCLUDED

#include "common.h"

void manejarInmunidadPrimerClic(sCelda **matriz, int fila, int columna, int dimensiones);
void recalcularMinasAdyacentes(sCelda **matriz, int dimensiones);
void procesarVictoria(SDL_Renderer *renderer, TTF_Font *fuente, TTF_Font *fuenteGrande, sCelda **matriz, int dimensiones, int *minasRestantes, int ventana_ancho);
void procesarGameOver(SDL_Renderer *renderer, TTF_Font *fuente, TTF_Font *fuenteGrande,sCelda **matriz, int dimensiones, int *minasRestantes, int ventana_ancho,int fila, int columna);
int manejarClicIzquierdo(SDL_Event *e, SDL_Renderer *renderer, TTF_Font *fuente, TTF_Font *fuenteGrande,sCelda **matriz, Archivo_conf configuracion, int *minasRestantes, int ventana_ancho, int *primerClic);
void manejarClicDerecho(SDL_Event *e, sCelda **matriz, Archivo_conf configuracion, int *minasRestantes);
void renderizarJuego(SDL_Renderer *renderer, TTF_Font *fuente, sCelda **matriz, int dimensiones, int *minasRestantes, int ventana_ancho);
void jugar(SDL_Window *ventana, SDL_Renderer *renderer, TTF_Font *fuente, TTF_Font *fuenteGrande, sCelda **matriz, Archivo_conf configuracion, int *minasRestantes, int ventana_ancho);
void limpiarTodosLosRecursos(SDL_Window *, SDL_Renderer *,TTF_Font *,TTF_Font *, sCelda **, Archivo_conf );

#endif // JUEGO_H_INCLUDED
