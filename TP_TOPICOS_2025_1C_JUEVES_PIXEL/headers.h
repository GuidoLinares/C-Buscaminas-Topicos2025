#ifndef HEADERS_H_INCLUDED
#define HEADERS_H_INCLUDED

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>


#define MAX_LINEA 100
#define FORMATO "CANTIDAD DE MINAS = %[^|]| DIMENSION DEL TABLERO = %d"
#define SIN_MEM "NO SE PUDO ASIGNAR MEMORIA"
#define ARCH_CONFIG "buscaminas.conf"
#define TAM_PIXEL 8
#define PIXEL_CELDA 32
#define ALTURA_HEADER 64

typedef struct
{
    int cantMinas ;
    int dimensiones;
}Archivo_conf;

//estructura de interfaz de celdas
typedef struct
{
    int esRevelada;
    int tieneMina;
    int tieneBandera;
    int minasAdyacentes;
}sCelda;

//estructura del log
typedef struct {
    FILE* archivo;
    char* nombreArchivo;
    time_t tiempoInicio;
}SistemaLog;



//FUNCIONES DE CONSOLA
sCelda** crearMatriz (int);
void destruirMatriz(sCelda**, int);
void llenarMatriz(sCelda**, Archivo_conf);
void inicializarMatriz(sCelda**, int);
void mostrarMatriz(sCelda**, int);
int generarAleatorio(int ,int );
void trim(char*);
Archivo_conf leerArchivo();
void revelarEspaciosVacios(sCelda**, int, int, int);


//FUNCIONES SDL
void dibujarTablero(SDL_Renderer *, int);
void dibujarCeldas(SDL_Renderer*, sCelda**, int, TTF_Font*);
void dibujarContadorMinas(SDL_Renderer*, TTF_Font*, int);
void dibujarHeader(SDL_Renderer*, TTF_Font*, int, int);
void mostrarGameOver(SDL_Renderer*, TTF_Font*, int);
void mostrarVictoria(SDL_Renderer*, TTF_Font*, int);
int verificarVictoria(sCelda**, int, int);

//FUNCIONES LOG
SistemaLog* inicializarLog(const char*);
void destruirLog();
void logInicioPartida(Archivo_conf);
void logClickCelda(sCelda**, int, int, int, const char*);
void logRevelarCelda(sCelda**, int, int, int);
void logBandera(sCelda**, int, int, int, int);
void logFinPartida(const char*);
void logConfiguracion(Archivo_conf);

//FUNCIONES JUEGO
void jugar(SDL_Window *ventana, SDL_Renderer *, TTF_Font *,TTF_Font *, sCelda **, Archivo_conf , int *, int );
void limpiarTodosLosRecursos(SDL_Window *, SDL_Renderer *,TTF_Font *,TTF_Font *, sCelda **, Archivo_conf );


#endif // HEADERS_H_INCLUDED
