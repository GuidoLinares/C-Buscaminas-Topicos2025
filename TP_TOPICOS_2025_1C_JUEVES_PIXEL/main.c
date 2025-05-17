
#include "headers.h"
/*
Apellido(s), nombre(s): Linares, Guido Hernan
DNI: 43170056
Entrega: Sí

Apellido(s), nombre(s): Goldring, Facundo
DNI:   (pongan su DNI)
Entrega: Sí

Apellido(s), nombre(s): Calvet, Lucas
DNI:  (pongan su DNI)
Entrega: Sí
*/

int main(int argc, char* argv[])
{
    int op,porcentaje;
    int** matriz;

    matriz  = crearMatriz();
    if(!matriz)
       printf("%s",SIN_MEM);
    else
    {
        inicializarMatriz(matriz);
        puts("ARRANCA SETEO DEL JUEGO");
        puts("Opcion 1- generar 10 bombas aleatorias en el tablero. \nOpcion 2- Generar bombas equivalentes al 10% del tablero");
        op = validar2(1,2);
        if(op == 1)
            llenarMatriz(matriz, 10);
        else
        {
            porcentaje = (TAM_GRILLA*TAM_GRILLA)/10;
            llenarMatriz(matriz,porcentaje);
        }
    }


    /*
    SDL_Init(SDL_INIT_VIDEO);
    char nombreVentana[100];
    sprintf(nombreVentana, "Tablero %dx%d",TAM_GRILLA,TAM_GRILLA);
    SDL_Window *ventana = SDL_CreateWindow(nombreVentana,
                                           SDL_WINDOWPOS_CENTERED,
                                           SDL_WINDOWPOS_CENTERED,
                                           TAM_GRILLA * TAM_PIXEL * PIXELES_X_LADO  + TAM_GRILLA * PX_PADDING, TAM_GRILLA * TAM_PIXEL * PIXELES_X_LADO  + TAM_GRILLA * PX_PADDING,
                                           2);

    SDL_Renderer *renderer = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    borrarPantalla(ventana, renderer);
    srand(time(0));
    */

    mostrarMatriz(matriz);
    free(matriz);
    return 0;

}
