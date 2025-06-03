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
    s_celdas **matriz;

    Archivo_conf configuracion;
    configuracion = leerArchivo();

    inicializarLog("Session_Buscaminas.log");
    logConfiguracion(configuracion);
    logInicioPartida(configuracion);

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
    int ventana_alto = configuracion.dimensiones * PIXEL_CELDA;

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

    SDL_Event e;
    int corriendo = 1;

    printf("Entrando al bucle principal...\n");

    while (corriendo)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                corriendo = 0;
                printf("Usuario cerró la ventana\n");
            }

            // revela celdas con el clic:
            if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    int mouse_x = e.button.x;
                    int mouse_y = e.button.y;

                    int columna_cliqueada = mouse_x / PIXEL_CELDA;
                    int fila_cliqueada = mouse_y / PIXEL_CELDA;

                    if (fila_cliqueada >= 0 && fila_cliqueada < configuracion.dimensiones &&
                        columna_cliqueada >= 0 && columna_cliqueada < configuracion.dimensiones)
                    {
                        logClickCelda(matriz, fila_cliqueada, columna_cliqueada, configuracion.dimensiones, "IZQUIERDO");

                        // Revela la celda si no está ya revelada o marcada con bandera
                        if (!(*(matriz+fila_cliqueada)+columna_cliqueada)->esRevelada &&
                            !(*(matriz+fila_cliqueada)+columna_cliqueada)->tieneBandera)
                        {
                            (*(matriz+fila_cliqueada)+columna_cliqueada)->esRevelada = 1;
                            logRevelarCelda(matriz, fila_cliqueada, columna_cliqueada, configuracion.dimensiones);

                            // VERIFICAR SI HAY MINA - GAME OVER
                            if ((*(matriz+fila_cliqueada)+columna_cliqueada)->tieneMina)
                            {
                                printf("\n*** BOOM! HAS ENCONTRADO UNA MINA ***\n");
                                printf("GAME OVER - Mina en posición (%d, %d)\n", fila_cliqueada, columna_cliqueada);
                                logFinPartida("DERROTA - MINA ENCONTRADA");

                                // Revelar todas las minas para mostrar el tablero final
                                for (int r = 0; r < configuracion.dimensiones; r++) {
                                    for (int c = 0; c < configuracion.dimensiones; c++) {
                                        if ((*(matriz+r)+c)->esRevelada) {
                                            (*(matriz+r)+c)->esRevelada = 1;
                                        }
                                    }
                                }

                                // Renderizar una vez más para mostrar todas las minas
                                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                                SDL_RenderClear(renderer);
                                dibujarCeldas(renderer, matriz, configuracion.dimensiones, fuente);
                                dibujarTablero(renderer, configuracion.dimensiones);
                                SDL_RenderPresent(renderer);

                                // Esperar 3 segundos para que el usuario vea el resultado
                                SDL_Delay(3000);

                                // Terminar el programa
                                corriendo = 0;
                            }
                        }

                        printf("Clic Izquierdo en celda: (%d, %d)\n", fila_cliqueada, columna_cliqueada);
                    }
                }
                else if (e.button.button == SDL_BUTTON_RIGHT)
                {
                    int mouse_x = e.button.x;
                    int mouse_y = e.button.y;

                    int columna_cliqueada = mouse_x / PIXEL_CELDA;
                    int fila_cliqueada = mouse_y / PIXEL_CELDA;

                    if (fila_cliqueada >= 0 && fila_cliqueada < configuracion.dimensiones &&
                        columna_cliqueada >= 0 && columna_cliqueada < configuracion.dimensiones)
                    {
                        // Pone/quita bandera si la celda no está revelada
                        if (!(*(matriz+fila_cliqueada)+columna_cliqueada)->esRevelada)
                        {
                            (*(matriz+fila_cliqueada)+columna_cliqueada)->tieneBandera =
                                !(*(matriz+fila_cliqueada)+columna_cliqueada)->tieneBandera;

                            logBandera(matriz, fila_cliqueada, columna_cliqueada,configuracion.dimensiones,matriz[fila_cliqueada][columna_cliqueada].tieneBandera);

                            printf("Clic Derecho (Bandera) en celda: (%d, %d)\n",
                                   fila_cliqueada, columna_cliqueada);
                        }
                    }
                }
            }
        }

        // Renderizado
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Fondo negro
        SDL_RenderClear(renderer);

        // Dibuja el contenido de las celdas
        dibujarCeldas(renderer, matriz, configuracion.dimensiones, fuente);

        // Dibuja las líneas de la cuadrícula
        dibujarTablero(renderer, configuracion.dimensiones);

        SDL_RenderPresent(renderer);

        // Pequeña pausa para no saturar la CPU
        SDL_Delay(16); // ~60 FPS
    }

    printf("Limpiando recursos...\n");

    // Limpieza de recursos en el orden correcto
    if (fuente) {
        TTF_CloseFont(fuente);
    }
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(ventana);
    SDL_Quit();

    destruirMatriz(matriz, configuracion.dimensiones);
    destruirLog();

    printf("Programa terminado correctamente\n");
    return 0;
}
