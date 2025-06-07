#ifndef TABLERO_H_INCLUDED
#define TABLERO_H_INCLUDED

#include "common.h"

// Funciones de manejo de matriz
sCelda** crearMatriz(int dimension);
void destruirMatriz(sCelda** matriz, int tamano);
void inicializarMatriz(sCelda** matriz, int dimension);
void llenarMatriz(sCelda** matriz, Archivo_conf configuracion);
void mostrarMatriz(sCelda** matriz, int dimension);

// Funciones auxiliares
int generarAleatorio(int minimo, int maximo);
void trim(char* cadena);
Archivo_conf leerArchivo();
void revelarEspaciosVacios(sCelda** matriz, int dimensiones, int fila, int col);
void contarMinasAdyacentes(sCelda** matriz, int dimension, int fila, int columna);


#endif // TABLERO_H_INCLUDED
