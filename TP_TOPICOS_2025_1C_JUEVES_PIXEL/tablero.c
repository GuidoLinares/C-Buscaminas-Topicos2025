#include "headers.h"

/////////////////// FUNCIONES SDL ///////////////////////////////
void borrarPantalla(SDL_Window *ventana, SDL_Renderer *renderer)
{
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0,0,0,0);
    SDL_Rect pixel = {0, 0, 0, 0};
    SDL_RenderFillRect(renderer, &pixel);
    SDL_RenderPresent(renderer);
}

int verificarSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL no se pudo inicializar: %s\n", SDL_GetError());
        return 0; // Indica que SDL no está bien instalado o no se pudo inicializar
    } else {
        printf("SDL se inicializó correctamente.\n");
    }

    SDL_Quit(); // Limpia los recursos de SDL
    return 1; // Indica que SDL parece estar bien instalado
}

int inicializarSDL(const char *titulo, int ancho, int alto, SDL_Window **ventana, SDL_Renderer **renderer)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Error al inicializar SDL: %s\n", SDL_GetError());
        return -1;
    }

    *ventana = SDL_CreateWindow(titulo, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ancho, alto, SDL_WINDOW_SHOWN);
    if (!(*ventana))
    {
        printf("Error al crear la ventana: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    *renderer = SDL_CreateRenderer(*ventana, -1, SDL_RENDERER_ACCELERATED);

    if (!(*renderer))
    {
        printf("Error al crear el renderizador: %s\n", SDL_GetError());
        SDL_DestroyWindow(*ventana);
        SDL_Quit();
        return -1;
    }

    SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);
    return 0; // Retorna 0 si todo salió bien
}
