#include <SDL2/SDL.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error inicializando SDL: %s\n", SDL_GetError());
        return 1;
    }
    printf("SDL se inicializó correctamente.\n");
    SDL_Quit();
    return 0;
}
