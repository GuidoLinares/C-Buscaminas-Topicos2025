#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

// Variables globales
#define MAX_LINEA 100
#define FORMATO "CANTIDAD DE MINAS = %[^|]| DIMENSION DEL TABLERO = %d"
#define SIN_MEM "NO SE PUDO ASIGNAR MEMORIA"
#define ARCH_CONFIG "buscaminas.conf"
#define TAM_PIXEL 8
#define PIXEL_CELDA 32
#define ALTURA_HEADER 64

// Estructuras compartidas
typedef struct
{
    int cantMinas;
    int dimensiones;
}sArchivo_conf;

typedef struct
{
    int esRevelada;
    int tieneMina;
    int tieneBandera;
    int minasAdyacentes;
}sCelda;

typedef struct {
    FILE* archivo;
    char* nombreArchivo;
    time_t tiempoInicio;
}sSistemaLog;

typedef struct {
    SDL_Window* ventana;
    SDL_Renderer* renderer;
    TTF_Font* fuente;
    TTF_Font* fuenteGrande;
}sRecursosSDL;

#endif // COMMON_H_INCLUDED
