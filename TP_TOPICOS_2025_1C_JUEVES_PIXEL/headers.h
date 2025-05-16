#ifndef HEADERS_H_INCLUDED
#define HEADERS_H_INCLUDED

#include <SDL2/SDL.h>
#define TAM_GRILLA 32
#define CANT_MINAS 10
#define SIN_MEM "NO SE PUDO ASIGNAR MEMORIA"

//FUNCIONES DE CONSOLA
int** crearMatriz ();
void destruirMatriz(int** m);
void llenarMatriz(int**);
void mostrarMatriz(int**);

//FUNCIONES SDL

#endif // HEADERS_H_INCLUDED
