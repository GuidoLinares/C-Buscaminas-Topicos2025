#include "headers.h"
/////////////////// FUNCIONES CONSOLA ///////////////////////////////
/*
bool crearArchivo (char nombreArchivo[MAX_NOMBRE_ARCHIVO],int cantidad )
{
    int i=0;
    s_random Srandom;

    FILE*arch;
    arch = fopen(nombreArchivo,"wb");
    if(arch == NULL)
    {
        printf("no se pudo crear el archivo %s",nombreArchivo);
        return false;
    }

    for(i=0; i<cantidad; i++)
    {
        Srandom.NumAleat = generarAleatorio(COTA_INF,COTA_SUP); //creo archivo y guardo numeros aleatorios generados por la funcion
        fwrite(&Srandom, sizeof(s_random),1,arch);
    }

    fclose(arch);
    return true;
}
*/
void destruirMatriz(int** m, size_t tam)
{
    int**ult = m + tam - 1;

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

int validar2 (int li, int ls)
{
    int num;
    do{
        scanf("%d",&num);
        if(num<li || num>ls)
            puts("Numero invalido, ingrese nuevamente: ");
    }while(num<li || num>ls);
    return num;
}

void inicializarMatriz(int** m)
{
    // Inicializar con ceros
    for (int i = 0; i < TAM_GRILLA; i++) {
        for (int j = 0; j < TAM_GRILLA; j++) {
            *(*(m + i) + j) = 0;
        }
    }
}

int generarAleatorio(int minimo, int maximo)
{
    return rand() % (maximo - minimo + 1) + minimo;
}

void llenarMatriz(int** m, size_t cant_minas)
{

    // Semilla aleatoria
    srand(time(NULL));

    // Colocar minas (unos) en posiciones aleatorias no repetidas
    size_t minas_colocadas = 0;
    while (minas_colocadas < cant_minas) {
        int fila = generarAleatorio(0, TAM_GRILLA-1);
        int col = generarAleatorio(0, TAM_GRILLA-1);

        if (*(*(m + fila) + col) == 0)
        {
            *(*(m + fila) + col) = 1;
            minas_colocadas++;
        }
    }
}

void mostrarMatriz(int**m)
{
    int i=0,j;
    for(i=0; i<TAM_GRILLA ; i++)
    {
        for(j=0; j<TAM_GRILLA ; j++ )
        {
            printf("[%d]",m[i][j]);
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
