#include "headers.h"

void destruirMatriz(int** m, size_t tam)
{
    int**ult = m + tam - 1;

    for(int**i=m; i<= ult ; i++)
    {
        free(*i);
    }
    free(m);
}

int** crearMatriz (int dimension)
{
    int** matriz = malloc(sizeof(void*)*dimension);

    int**ult = matriz + dimension - 1;

    for(int**i = matriz ; i<= ult ; i++)
    {
        *i = (int*)malloc(sizeof(int)*dimension); // reservo memoria para cada elemento
        if(!*i)
        {
            destruirMatriz(matriz, i - matriz);
            return NULL;
        }
    }

    return matriz;
}

void inicializarMatriz(int** m, int dimension)
{
    // Inicializar con ceros
    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            *(*(m + i) + j) = 0;
        }
    }
}

int generarAleatorio(int minimo, int maximo)
{
    return rand() % (maximo - minimo + 1) + minimo;
}

void llenarMatriz(int** m, Archivo_conf config)
{
    // Semilla aleatoria
    srand(time(NULL));

    // Colocar minas (unos) en posiciones aleatorias no repetidas
    size_t minas_colocadas = 0;
    while (minas_colocadas < config.minas)
    {
        int fila = generarAleatorio(0, config.dimensiones-1);
        int col = generarAleatorio(0, config.dimensiones-1);

        if (*(*(m + fila) + col) == 0)
        {
            *(*(m + fila) + col) = 1;
            minas_colocadas++;
        }
    }
}

void mostrarMatriz(int**m, int dimension)
{
    int i=0,j;
    for(i=0; i<dimension ; i++)
    {
        for(j=0; j<dimension ; j++ )
        {
            printf("[%d]",m[i][j]);
        }
        printf("\n");
    }

}

void trim(char* str)
{
    char* end;
    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0)
        return;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    *(end + 1) = '\0';
}

Archivo_conf leerArchivo()
{
    Archivo_conf config;
    char linea[MAX_LINEA];
    char minasStr[20];
    int dimensiones;

    FILE*arch;
    arch = fopen(ARCH_CONFIG, "r");
    if(!arch)
    {
        fclose(arch);
        printf("NO SE PUDO ABRIR EL ARCHIVO %s",ARCH_CONFIG);
        exit(-1);
    }

    if (!fgets(linea, MAX_LINEA, arch)) {
        fprintf(stderr, "Error al leer la línea del archivo.\n");
        fclose(arch);
        exit(-2);
    }

    fclose(arch);

    if (sscanf(linea, "CANTIDAD DE MINAS = %[^|]| DIMENSION DEL TABLERO = %d", minasStr, &dimensiones) != 2) {
        fprintf(stderr, "Formato incorrecto en el archivo de configuración.\n");
        exit(-3);
    }

    trim(minasStr); // Limpiar espacios
    config.dimensiones = dimensiones;

    int len = strlen(minasStr);
    if (minasStr[len - 1] == '%')
    {
        minasStr[len - 1] = '\0'; // Quitar %
        trim(minasStr);
        int porcentaje = atoi(minasStr); //atoi se usa para convertir una cadena en entero
        if (porcentaje < 0 || porcentaje > 100) {
            fprintf(stderr, "Porcentaje de minas inválido: %d%%\n", porcentaje);
            exit(-4);
        }
        int totalCasillas = dimensiones * dimensiones;
        config.minas = (porcentaje * totalCasillas) / 100;
    } else {
        config.minas = atoi(minasStr);
    }

    if (config.minas < 0 || config.minas > config.dimensiones * config.dimensiones) {
        fprintf(stderr, "Cantidad de minas inválida: %d\n", config.minas);
        exit(-5);
    }

    return config;
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

int verificarSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL no se pudo inicializar: %s\n", SDL_GetError());
        return 0; // Indica que SDL no está bien instalado o no se pudo inicializar
    } else {
        printf("SDL se inicializo correctamente.\n");
    }

    SDL_Quit(); // Limpia los recursos de SDL
    return 1; // Indica que SDL parece estar bien instalado
}
