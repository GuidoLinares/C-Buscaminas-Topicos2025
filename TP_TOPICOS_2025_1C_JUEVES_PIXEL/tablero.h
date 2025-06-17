#ifndef TABLERO_H_INCLUDED
#define TABLERO_H_INCLUDED

#include "common.h"

// FUNCIONES DE MANEJO DE MATRIZ
sCelda** crearMatriz(int dimension);
void destruirMatriz(sCelda** matriz, int tamano);
void inicializarMatriz(sCelda** matriz, int dimension);
void llenarMatriz(sCelda** matriz, sArchivo_conf configuracion);
void mostrarMatriz(sCelda** matriz, int dimension);

// FUNCIONES AUXILIARES
int generarAleatorio(int minimo, int maximo);
void trim(char* cadena);
sArchivo_conf leerArchivo();
void revelarEspaciosVacios(sCelda** matriz, int dimensiones, int fila, int col);
void contarMinasAdyacentes(sCelda** matriz, int dimension, int fila, int columna);


#endif // TABLERO_H_INCLUDED
