#include "headers.h"
/////////////////// FUNCIONES CONSOLA ///////////////////////////////

void destruirMatriz(int** m)
{
    int**ult = m + TAM_GRILLA - 1;

    for(int**i=m; i<= ult ; i++)
    {
        free(*i);
    }
    free(m);
}

int** crearMatriz ()
{
    int** matriz = malloc(sizeof(void*)*TAM_GRILLA);

    int**ult = matriz + TAM_GRILLA - 1;

    for(int**i = matriz ; i<= ult ; i++)
    {
        *i = (int*)malloc(sizeof(int)*TAM_GRILLA); // reservo memoria para cada elemento
        if(!*i)
        {
            destruirMatriz(matriz, i - matriz);
            return NULL;
        }

    }
    return matriz;

}

void llenarMatriz(int**m)
{


}

void mostrarMatriz(int**m)
{
    int i=0,j;
    for(i=0; i<TAM_GRILLA ; i++)
    {
        for(j=0; j<TAM_GRILLA ; j++ )
        {
            printf("[%d]\t",mat[i][j]);
        }
        printf("\n");
    }

}
/////////////////// FUNCIONES SDL ///////////////////////////////
void borrarPantalla(SDL_Window *ventana, SDL_Renderer *renderer)
{
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0,0,0,0);
    SDL_Rect pixel = {0, 0, 0, 0};
    SDL_RenderFillRect(renderer, &pixel);
    SDL_RenderPresent(renderer);
}
