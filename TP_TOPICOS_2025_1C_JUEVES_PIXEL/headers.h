#ifndef HEADERS_H_INCLUDED
#define HEADERS_H_INCLUDED

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#define MAX_LINEA 100

#define FORMATO "CANTIDAD DE MINAS = %[^|]| DIMENSION DEL TABLERO = %d"
#define SIN_MEM "NO SE PUDO ASIGNAR MEMORIA"
#define ARCH_CONFIG "buscaminas.conf"
#define TAM_PIXEL 8
#define PIXEL_CELDA 1

typedef struct
{
    int minas ;
    int dimensiones;
}Archivo_conf;

//FUNCIONES DE CONSOLA
int** crearMatriz (int);
void destruirMatriz(int**, size_t);
void llenarMatriz(int**, Archivo_conf);
void mostrarMatriz(int**, int);
int generarAleatorio(int ,int );
int validar2 (int , int );
void inicializarMatriz(int**, int);
void trim(char*);
Archivo_conf leerArchivo();
void dibujarTablero(SDL_Renderer *, int);

//FUNCIONES SDL
int verificarSDL();

#endif // HEADERS_H_INCLUDED
