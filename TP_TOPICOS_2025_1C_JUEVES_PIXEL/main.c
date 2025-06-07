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
Entrega: NO
*/

int main(int argc, char *argv[])
{
    sCelda **matriz;

    Archivo_conf configuracion;
    configuracion = leerArchivo();

    inicializarLog("Session_Buscaminas.log");
    logConfiguracion(configuracion);
    logInicioPartida(configuracion);
    int minasRestantes = configuracion.cantMinas;

    matriz = crearMatriz(configuracion.dimensiones);
    if(!matriz)
    {
        printf("%s\n",SIN_MEM);
        return -1;
    }
    else
    {
        inicializarMatriz(matriz, configuracion.dimensiones);
        puts("ARRANCA SETEO DEL JUEGO");
        llenarMatriz(matriz, configuracion);
    }

    mostrarMatriz(matriz,configuracion.dimensiones);


    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Error al inicializar SDL: %s\n", SDL_GetError());
        destruirMatriz(matriz, configuracion.dimensiones);
        destruirLog();
        return -1;
    }

    // Inicializar TTF
    if (TTF_Init() == -1) {
        printf("Error al inicializar TTF: %s\n", TTF_GetError());
        SDL_Quit();
        destruirMatriz(matriz, configuracion.dimensiones);
        destruirLog();
        return -1;
    }

    // Definir el tamaño de la ventana según la configuración y PIXEL_CELDA
    int ventana_ancho = configuracion.dimensiones * PIXEL_CELDA;
    int ventana_alto = configuracion.dimensiones * PIXEL_CELDA + ALTURA_HEADER;

    SDL_Window *ventana = SDL_CreateWindow("BUSCAMINAS_PIXEL",
                                           SDL_WINDOWPOS_CENTERED,
                                           SDL_WINDOWPOS_CENTERED,
                                           ventana_ancho,
                                           ventana_alto,
                                           SDL_WINDOW_SHOWN);

    if (!ventana)
    {
        printf("Error al crear la ventana: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        destruirMatriz(matriz, configuracion.dimensiones);
        destruirLog();
        return -1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(ventana, -1,
                                                 SDL_RENDERER_ACCELERATED |
                                                 SDL_RENDERER_PRESENTVSYNC);

    if (!renderer)
    {
        printf("Error al crear el renderizador: %s\n", SDL_GetError());
        SDL_DestroyWindow(ventana);
        TTF_Quit();
        SDL_Quit();
        destruirMatriz(matriz, configuracion.dimensiones);
        destruirLog();
        return -1;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);


    // Intentar cargar múltiples fuentes como fallback
    TTF_Font* fuente = NULL;
    TTF_Font* fuenteGrande = NULL;
    const char* fuentes[] = {
        "arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "/System/Library/Fonts/Arial.ttf",  // macOS
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",  // Linux
        NULL
    };

    for (int i = 0; fuentes[i] != NULL; i++) {
        fuente = TTF_OpenFont(fuentes[i], 16);
        if (fuente) {
            printf("Fuente cargada: %s\n", fuentes[i]);
            break;
        }
    }

    if (!fuente) {
        printf("Advertencia: No se pudo cargar ninguna fuente. Los números no se mostrarán.\n");
        printf("Error TTF: %s\n", TTF_GetError());
        // No salimos del programa, solo continuamos sin fuente
    }

    for (int i = 0; fuentes[i] != NULL; i++)
    {
        fuenteGrande = TTF_OpenFont(fuentes[i], 40);
        if (fuenteGrande)
        {
            printf("Fuente grande cargada: %s\n", fuentes[i]);
            break;
        }
    }

    if (!fuenteGrande) {
        printf("⚠️ No se pudo cargar fuente grande, se usará la normal.\n");
        fuenteGrande = fuente;
    }

    printf("Entrando al bucle principal...\n");



    jugar(ventana, renderer, fuente,fuenteGrande, matriz, configuracion, &minasRestantes, ventana_ancho);

    limpiarTodosLosRecursos(ventana, renderer, fuente, fuenteGrande, matriz, configuracion);
    printf("Programa terminado correctamente\n");

    return 0;
}
