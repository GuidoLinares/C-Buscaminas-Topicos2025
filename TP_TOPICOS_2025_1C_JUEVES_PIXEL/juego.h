#ifndef JUEGO_H_INCLUDED
#define JUEGO_H_INCLUDED

#include "common.h"
#include "usuario.h"

void manejarInmunidadPrimerClic(sCelda **matriz, int fila, int columna, int dimensiones);
void recalcularMinasAdyacentes(sCelda **matriz, int dimensiones);
void procesarVictoria(SDL_Renderer *renderer, TTF_Font *fuente, TTF_Font *fuenteGrande, sCelda **matriz, int dimensiones, int *minasRestantes, int ventana_ancho);
void procesarGameOver(SDL_Renderer *renderer, TTF_Font *fuente, TTF_Font *fuenteGrande,sCelda **matriz, int dimensiones, int *minasRestantes, int ventana_ancho,int fila, int columna);
void ejecutarPartida(SDL_Window* ventana,SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuarioActual,sArchivo_conf config);
int manejarClicIzquierdo(SDL_Event *e, SDL_Renderer *renderer, TTF_Font *fuente, TTF_Font *fuenteGrande,sCelda **matriz, sArchivo_conf configuracion, int *minasRestantes, int ventana_ancho, int *primerClic);
void manejarClicDerecho(SDL_Event *e, sCelda **matriz, sArchivo_conf configuracion, int *minasRestantes);
void jugarConGuardado(SDL_Window *ventana, SDL_Renderer *renderer, TTF_Font *fuente, TTF_Font *fuenteGrande, sCelda **matriz, sArchivo_conf configuracion, int *minasRestantes, int ventana_ancho, sUsuario* usuario, time_t tiempoInicioPartida, int* primerClic, sEstadoCheat* estadoCheat);
void limpiarTodosLosRecursos(SDL_Window *, SDL_Renderer *,TTF_Font *,TTF_Font *, sCelda **, sArchivo_conf );
int activarCheatXray(sEstadoCheat* estadoCheat);
void actualizarCheatXray(sEstadoCheat* estadoCheat);
void reiniciarCheatXray(sEstadoCheat* estadoCheat);
void marcarCheatXrayComoUsado(sEstadoCheat* estadoCheat);
int mostrarMenuPausaFijo(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuario, sCelda** matriz, sArchivo_conf configuracion, int minasRestantes, int tiempoTranscurrido, int primerClic, sEstadoCheat* estadoCheat);


#endif
