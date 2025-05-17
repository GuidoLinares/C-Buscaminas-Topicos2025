#include "headers.h"
/*
Apellido(s), nombre(s): Linares, Guido Hernan
DNI: 43170056
Entrega: Sí

Apellido(s), nombre(s): Goldring, Facundo
DNI: 44595085
Entrega: Sí

Apellido(s), nombre(s): Calvet, Lucas
DNI:  (pongan su DNI)
Entrega: Sí
*/

int main(int argc, char *argv[])
{
    int** matriz;
    Archivo_conf configuracion;
    configuracion = leerArchivo();

    matriz = crearMatriz(configuracion.dimensiones);
    if(!matriz)
        printf("%s",SIN_MEM);
    else
    {
        inicializarMatriz(matriz, configuracion.dimensiones);
        puts("ARRANCA SETEO DEL JUEGO");
        llenarMatriz(matriz, configuracion);
    }

    mostrarMatriz(matriz,configuracion.dimensiones);

    SDL_Window *ventana;
    SDL_Renderer *renderer;

    if (inicializarSDL("BUSCAMINAS_PIXEL", 640, 640, &ventana, &renderer) != 0)
        return -1;

    dibujarTablero(renderer, configuracion.dimensiones);

    SDL_Event evento;
    int ejecutando = 1;

    while (ejecutando)
    {
        while (SDL_PollEvent(&evento))
        {
            if (evento.type == SDL_QUIT)
                ejecutando = 0;  // Cierra la ventana cuando el usuario la cierre
        }

        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(100);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(ventana);
    SDL_Quit();

    free(matriz);

    return 0;
}
