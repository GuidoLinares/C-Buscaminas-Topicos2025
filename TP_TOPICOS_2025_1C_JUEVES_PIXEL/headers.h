#ifndef HEADERS_H_INCLUDED
#define HEADERS_H_INCLUDED

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define TAM_GRILLA 32
#define CANT_MINAS 10
#define SIN_MEM "NO SE PUDO ASIGNAR MEMORIA"

//FUNCIONES DE CONSOLA
int** crearMatriz ();
void destruirMatriz(int**, size_t);
void llenarMatriz(int**, size_t);
void mostrarMatriz(int**);
int generarAleatorio(int ,int );
int validar2 (int , int );
void inicializarMatriz(int**);

//FUNCIONES SDL

#endif // HEADERS_H_INCLUDED
