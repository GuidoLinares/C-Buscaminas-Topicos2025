#include "headers.h"

void dibujarTablero(SDL_Renderer *renderer, int dimensiones)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Color blanco

    for (int i = 0; i <= dimensiones; i++) {
        SDL_RenderDrawLine(renderer, i * PIXEL_CELDA, 0, i * PIXEL_CELDA, dimensiones * PIXEL_CELDA);
        SDL_RenderDrawLine(renderer, 0, i * PIXEL_CELDA, dimensiones * PIXEL_CELDA, i * PIXEL_CELDA);
    }

    SDL_RenderPresent(renderer);
}
