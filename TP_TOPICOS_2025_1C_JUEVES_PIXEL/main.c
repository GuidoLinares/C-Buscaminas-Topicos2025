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
        printf("%s",SIN_MEM);
    else
    {
        inicializarMatriz(matriz, configuracion.dimensiones);
        puts("ARRANCA SETEO DEL JUEGO");
        llenarMatriz(matriz, configuracion);

    /*
       for (int r = 0; r < configuracion.dimensiones; r++)
        {
            for (int c = 0; c < configuracion.dimensiones; c++)
            {
                matriz[r][c].esRevelada = 1;
            }
        }
    }
*/
    mostrarMatriz(matriz,configuracion.dimensiones);

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Error al inicializar SDL: %s\n", SDL_GetError());
        destruirMatriz(matriz, configuracion.dimensiones); // Libera memoria antes de salir
        return -1;
    }

    // Definir el tamaño de la ventana según la configuración y PIXEL_CELDA
    int ventana_ancho = configuracion.dimensiones * PIXEL_CELDA;
    int ventana_alto = configuracion.dimensiones * PIXEL_CELDA;

    SDL_Window *ventana = SDL_CreateWindow("BUSCAMINAS_PIXEL",SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ventana_ancho, ventana_alto, SDL_WINDOW_SHOWN);

    if (!ventana)
    {
        printf("Error al crear la ventana: %s\n", SDL_GetError());
        SDL_Quit(); // Limpiar SDL si la ventana falla
        destruirMatriz(matriz, configuracion.dimensiones);
        return -1;
    }

    // Usar SDL_RENDERER_PRESENTVSYNC para una mejor sincronización de fotogramas
    SDL_Renderer *renderer = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer)
    {
        printf("Error al crear el renderizador: %s\n", SDL_GetError());
        SDL_DestroyWindow(ventana); // Limpiar ventana si el renderizador falla
        SDL_Quit();
        destruirMatriz(matriz, configuracion.dimensiones);
        return -1;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    TTF_Init();
    TTF_Font* fuente = TTF_OpenFont("arial.ttf", 16);

    SDL_Event e;
    int corriendo = 1;

    while (corriendo)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                corriendo = 0; // Cierra la ventana cuando el usuario la cierre

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
                        if (!matriz[fila_cliqueada][columna_cliqueada].esRevelada &&
                            !matriz[fila_cliqueada][columna_cliqueada].tieneBandera)
                        {
                            matriz[fila_cliqueada][columna_cliqueada].esRevelada = 1;
                            logRevelarCelda(matriz, fila_cliqueada, columna_cliqueada, configuracion.dimensiones);
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
                        if (!matriz[fila_cliqueada][columna_cliqueada].esRevelada)
                        {
                            logBandera(matriz, fila_cliqueada, columna_cliqueada, configuracion.dimensiones,
                                      matriz[fila_cliqueada][columna_cliqueada].tieneBandera);
                            matriz[fila_cliqueada][columna_cliqueada].tieneBandera =
                                !matriz[fila_cliqueada][columna_cliqueada].tieneBandera;
                        }

                        printf("Clic Derecho (Bandera) en celda: (%d, %d)\n", fila_cliqueada, columna_cliqueada);
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Fondo negro para limpiar
        SDL_RenderClear(renderer); // Limpia toda la pantalla

        // Dibuja el contenido de las celdas (basado en el estado de matriz)
        dibujarCeldas(renderer, matriz, configuracion.dimensiones, fuente);

        // Dibuja las líneas de la cuadrícula (para que se vean encima de las celdas)
        dibujarTablero(renderer, configuracion.dimensiones);

        SDL_RenderPresent(renderer); // Muestra lo que se ha dibujado
    }

    // Limpieza de recursos en el orden correcto
    TTF_CloseFont(fuente);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(ventana);
    SDL_Quit();

    destruirMatriz(matriz, configuracion.dimensiones);
    destruirLog();

    return 0;
}
