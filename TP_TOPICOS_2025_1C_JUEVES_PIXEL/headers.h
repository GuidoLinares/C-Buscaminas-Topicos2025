#ifndef HEADERS_H_INCLUDED
#define HEADERS_H_INCLUDED

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define FORMATO "CANTIDAD DE MINAS = %d| DIMENSION DEL TABLERO = %d"
#define SIN_MEM "NO SE PUDO ASIGNAR MEMORIA"
#define ARCH_CONFIG "buscaminas.conf"

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
Archivo_conf leerArchivo();


//FUNCIONES SDL

#endif // HEADERS_H_INCLUDED
