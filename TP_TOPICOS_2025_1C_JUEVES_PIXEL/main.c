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

    if (verificarSDL())
        printf("SDL2 esta instalado y funcionando correctamente (inicializacion basica exitosa).\n");
    else
        printf("Se encontraron problemas con la instalacion de SDL2.\n");

    mostrarMatriz(matriz,configuracion.dimensiones);

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *ventana = SDL_CreateWindow("BUSCAMINAS_PIXEL",SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,640, 640, SDL_WINDOW_SHOWN);

    if (!ventana)
    {
        printf("Error al crear la ventana: %s\n", SDL_GetError());
        return -1;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer)
    {
        printf("Error al crear el renderizador: %s\n", SDL_GetError());
        return -1;
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    dibujarTablero(renderer, configuracion.dimensiones);

    SDL_Event e;
    int corriendo = 1;

    while (corriendo)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                corriendo = 0;  // Cierra la ventana cuando el usuario la cierre
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
