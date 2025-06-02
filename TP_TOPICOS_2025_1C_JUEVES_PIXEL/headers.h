#ifndef HEADERS_H_INCLUDED
#define HEADERS_H_INCLUDED

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <SDL2/SDL_ttf.h>


#define MAX_LINEA 100
#define FORMATO "CANTIDAD DE MINAS = %[^|]| DIMENSION DEL TABLERO = %d"
#define SIN_MEM "NO SE PUDO ASIGNAR MEMORIA"
#define ARCH_CONFIG "buscaminas.conf"
#define TAM_PIXEL 8
#define PIXEL_CELDA 32

typedef struct
{
    int minas ;
    int dimensiones;
}Archivo_conf;

typedef struct
{
    int esRevelada;
    int tieneMina;
    int tieneBandera;
    int minasAdyacentes;
}s_celdas;


//FUNCIONES DE CONSOLA
s_celdas** crearMatriz (int);
void destruirMatriz(s_celdas**, int);
void llenarMatriz(s_celdas**, Archivo_conf);
void inicializarMatriz(s_celdas**, int);
void mostrarMatriz(s_celdas**, int);
int generarAleatorio(int ,int );
void trim(char*);
Archivo_conf leerArchivo();


//FUNCIONES SDL
void dibujarTablero(SDL_Renderer *, int);
void dibujarCeldas(SDL_Renderer *, s_celdas**, int, TTF_Font*);

#endif // HEADERS_H_INCLUDED
