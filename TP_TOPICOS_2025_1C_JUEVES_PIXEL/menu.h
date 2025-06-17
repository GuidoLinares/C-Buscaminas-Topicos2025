#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include "common.h"
#include "usuario.h"


void renderizarTexto(SDL_Renderer* renderer, TTF_Font* fuente, const char* texto, int x, int y, SDL_Color colorTexto, SDL_Color colorSombra, int centrado, int conSombra);
int pantallaIngreso(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuario);
int mostrarMenuSDL(SDL_Window* ventana, SDL_Renderer* renderer,TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuario);
void mostrarEstadisticasSDL(SDL_Window* ventana, SDL_Renderer* renderer,TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuario);
int cargarPartidaSDL(SDL_Window* ventana, SDL_Renderer* renderer,TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuario, sArchivo_conf configuracion);
int cargarYEjecutarPartida(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuario, int indicePartida, sArchivo_conf configuracion);
void dibujarTextoCentrado(SDL_Renderer* renderer, TTF_Font* fuente, const char* texto, int x, int y, int ancho, SDL_Color color);
void dibujarBotonSDL(SDL_Renderer* renderer, TTF_Font* fuente, const char* texto, SDL_Rect rect, int seleccionado);
int clicEnRect(int x, int y, SDL_Rect rect);
int manejarEntradaTextoSDL(SDL_Event* evento, char* buffer, int maxLen);
int validarNombreUsuario(const char *nombre);
int mostrarMenuConfiguracion(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, sArchivo_conf* config);
int guardarConfiguracion(sArchivo_conf config);
void mostrarInstruccionesSDL(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande);

#endif // MENU_H_INCLUDED
