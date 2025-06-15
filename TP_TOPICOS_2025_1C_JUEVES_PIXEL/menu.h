#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include "common.h"
#include "usuario.h"

int pantallaIngreso(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuario);

int mostrarMenuSDL(SDL_Window* ventana, SDL_Renderer* renderer,TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuario);

void mostrarEstadisticasSDL(SDL_Window* ventana, SDL_Renderer* renderer,TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuario);

int cargarPartidaSDL(SDL_Window* ventana, SDL_Renderer* renderer,TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuario, sArchivo_conf configuracion);

void dibujarTextoCentrado(SDL_Renderer* renderer, TTF_Font* fuente, const char* texto, int x, int y, int ancho, SDL_Color color);

void dibujarBotonSDL(SDL_Renderer* renderer, TTF_Font* fuente, const char* texto, SDL_Rect rect, int seleccionado);

int clicEnRect(int x, int y, SDL_Rect rect);

int manejarEntradaTextoSDL(SDL_Event* evento, char* buffer, int maxLen);

void renderizarTextoCentrado(SDL_Renderer* , TTF_Font* , const char* , int , SDL_Color , SDL_Color );

// FUNCIÓN ORIGINAL MANTENIDA
int cargarYEjecutarPartida(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuario, int indicePartida, sArchivo_conf configuracion);

// NUEVA FUNCIÓN PARA CARGAR PARTIDAS COMPLETAS
int cargarYEjecutarPartidaCompleta(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuario, int indicePartida, sArchivo_conf configuracion);

int validarNombreUsuario(const char *nombre);

#endif // MENU_H_INCLUDED
