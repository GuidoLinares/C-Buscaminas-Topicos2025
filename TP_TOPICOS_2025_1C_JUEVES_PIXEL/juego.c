#include "common.h"
#include "juego.h"
#include "logs.h"
#include "tablero.h"
#include "interfaz.h"
#include "usuario.h"
#include "menu.h"

void ejecutarPartida(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, Usuario* usuarios, Archivo_conf configuracion)
{
    sCelda **matriz;
    time_t tiempoInicio = time(NULL);

    // === REDIMENSIONAR VENTANA PARA EL JUEGO ===
    int ventana_ancho = configuracion.dimensiones * PIXEL_CELDA;
    int ventana_alto = configuracion.dimensiones * PIXEL_CELDA + ALTURA_HEADER;

    SDL_SetWindowSize(ventana, ventana_ancho, ventana_alto);
    SDL_SetWindowPosition(ventana, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    // === TU CÓDIGO ORIGINAL DE JUEGO ===
    inicializarLog("Session_Buscaminas.log");
    logConfiguracion(configuracion);
    logInicioPartida(configuracion);
    int minasRestantes = configuracion.cantMinas;

    matriz = crearMatriz(configuracion.dimensiones);
    if(!matriz) {
        printf("%s\n",SIN_MEM);
        return;
    }

    inicializarMatriz(matriz, configuracion.dimensiones);
    puts("ARRANCA SETEO DEL JUEGO");
    llenarMatriz(matriz, configuracion);
    mostrarMatriz(matriz,configuracion.dimensiones);

    printf("Entrando al bucle principal...\n");

    // JUGAR (tu función original, sin cambios)
    jugar(ventana, renderer, fuente, fuenteGrande, matriz, configuracion, &minasRestantes, ventana_ancho);

    // Al terminar la partida, actualizar estadísticas del usuario
    time_t tiempoFin = time(NULL);
    int tiempoTranscurrido = (int)difftime(tiempoFin, tiempoInicio);

    // Determinar si ganó o perdió (esto ya lo tienes en tu lógica)
    int gano = verificarVictoria(matriz, configuracion.dimensiones, configuracion.cantMinas);
    actualizarEstadisticas(usuarios, gano, tiempoTranscurrido);

    // Limpieza de la matriz y logs
    destruirMatriz(matriz, configuracion.dimensiones);
    destruirLog();

    printf("Partida terminada\n");
}

void jugar(SDL_Window *ventana, SDL_Renderer *renderer, TTF_Font *fuente,TTF_Font *fuenteGrande, sCelda **matriz, Archivo_conf configuracion, int *minasRestantes, int ventana_ancho)
{
    int corriendo = 1;
    SDL_Event e;


    while (corriendo)
    {
            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT)
                {
                    corriendo = 0;
                    printf("Usuario cerró la ventana\n");
                }

                if (e.type == SDL_MOUSEBUTTONDOWN)
                {
                    if (e.button.button == SDL_BUTTON_LEFT)
                    {
                        int mouse_x = e.button.x;
                        int mouse_y = e.button.y;

                        // AJUSTAR COORDENADAS RESTANDO EL HEADER
                        if (mouse_y >= ALTURA_HEADER)
                        { // Solo procesar clics debajo del header

                            int columna_cliqueada = mouse_x / PIXEL_CELDA;
                            int fila_cliqueada = (mouse_y - ALTURA_HEADER) / PIXEL_CELDA; // ← RESTAR HEADER

                            if (fila_cliqueada >= 0 && fila_cliqueada < configuracion.dimensiones &&
                                columna_cliqueada >= 0 && columna_cliqueada < configuracion.dimensiones)
                            {
                                logClickCelda(matriz, fila_cliqueada, columna_cliqueada, configuracion.dimensiones, "IZQUIERDO");

                                // Revela la celda si no está ya revelada o marcada con bandera
                                if (!(*(matriz+fila_cliqueada)+columna_cliqueada)->esRevelada &&
                                    !(*(matriz+fila_cliqueada)+columna_cliqueada)->tieneBandera)
                                {
                                    revelarEspaciosVacios(matriz, configuracion.dimensiones, fila_cliqueada, columna_cliqueada);
                                    logRevelarCelda(matriz, fila_cliqueada, columna_cliqueada, configuracion.dimensiones);

                                    if (verificarVictoria(matriz, configuracion.dimensiones, configuracion.cantMinas))
                                    {
                                        printf("¡Felicidades Ganaste!\n");
                                        logFinPartida("VICTORIA");

                                        // Renderizado final de victoria
                                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                                        SDL_RenderClear(renderer);
                                        dibujarHeader(renderer, fuente, *minasRestantes, ventana_ancho);
                                        dibujarCeldas(renderer, matriz, configuracion.dimensiones, fuente);
                                        dibujarTablero(renderer, configuracion.dimensiones);

                                        mostrarVictoria(renderer, fuenteGrande, configuracion.dimensiones);
                                        SDL_RenderPresent(renderer);

                                        SDL_Delay(4000); // Muestra la victoria por un tiempo
                                        corriendo = 0; // Termina el juego
                                    }


                                    // VERIFICAR SI HAY MINA - GAME OVER
                                    if ((*(matriz+fila_cliqueada)+columna_cliqueada)->tieneMina)
                                    {
                                        printf("\n*** BOOM! HAS ENCONTRADO UNA MINA ***\n");
                                        printf("GAME OVER - Mina en posicion (%d, %d)\n", fila_cliqueada, columna_cliqueada);
                                        logFinPartida("DERROTA - MINA ENCONTRADA");

                                        // Revelar todas las minas para mostrar el tablero final
                                        for (int r = 0; r < configuracion.dimensiones; r++)
                                        {
                                            for (int c = 0; c < configuracion.dimensiones; c++)
                                            {
                                                (*(matriz+r)+c)->esRevelada = 1;
                                            }
                                        }

                                        // Renderizar una vez más para mostrar todas las minas y el GAME OVER
                                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                                        SDL_RenderClear(renderer);
                                        dibujarHeader(renderer, fuente, *minasRestantes, ventana_ancho);
                                        dibujarCeldas(renderer, matriz, configuracion.dimensiones, fuente);
                                        dibujarTablero(renderer, configuracion.dimensiones);

                                        mostrarGameOver(renderer, fuenteGrande, configuracion.dimensiones);
                                        SDL_RenderPresent(renderer);

                                        // Esperar 3 segundos para que el usuario vea el resultado
                                        SDL_Delay(3000);

                                        // Terminar el programa
                                        corriendo = 0;
                                    }
                                }
                            }
                            printf("Clic Izquierdo en celda: (%d, %d)\n", fila_cliqueada, columna_cliqueada);
                        }
                    }
                    else if (e.button.button == SDL_BUTTON_RIGHT)
                    {
                        int mouse_x = e.button.x;
                        int mouse_y = e.button.y;

                        // AJUSTAR COORDENADAS RESTANDO EL HEADER
                        if (mouse_y >= ALTURA_HEADER)
                        { // Solo procesar clics debajo del header

                            int columna_cliqueada = mouse_x / PIXEL_CELDA;
                            int fila_cliqueada = (mouse_y - ALTURA_HEADER) / PIXEL_CELDA; // ← RESTAR HEADER

                            if (fila_cliqueada >= 0 && fila_cliqueada < configuracion.dimensiones &&
                                columna_cliqueada >= 0 && columna_cliqueada < configuracion.dimensiones)
                            {
                                // Pone/quita bandera si la celda no está revelada
                                if (!(*(matriz+fila_cliqueada)+columna_cliqueada)->esRevelada)
                                {
                                    // Verificar si se está poniendo o quitando bandera
                                    int teniaBandera = (*(matriz+fila_cliqueada)+columna_cliqueada)->tieneBandera;

                                    // Cambiar estado de la bandera
                                    (*(matriz+fila_cliqueada)+columna_cliqueada)->tieneBandera = !teniaBandera;

                                    // Actualizar contador de minas restantes
                                    if (teniaBandera)
                                        (*minasRestantes)++; // Se quitó bandera
                                    else
                                        (*minasRestantes)--; // Se puso bandera

                                    logBandera(matriz, fila_cliqueada, columna_cliqueada, configuracion.dimensiones,
                                               (*(matriz+fila_cliqueada)+columna_cliqueada)->tieneBandera);

                                    printf("Clic Derecho (Bandera) en celda: (%d, %d) - Minas restantes: %d\n",
                                                   fila_cliqueada, columna_cliqueada, *minasRestantes);
                                }
                            }
                        }
                    }
                }
            }


            // Renderizado
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Fondo negro
            SDL_RenderClear(renderer);
            dibujarHeader(renderer, fuente, *minasRestantes, ventana_ancho);
            // Dibuja el contenido de las celdas
            dibujarCeldas(renderer, matriz, configuracion.dimensiones, fuente);

            // Dibuja las líneas de la cuadrícula
            dibujarTablero(renderer, configuracion.dimensiones);

            SDL_RenderPresent(renderer);

            // Pequeña pausa para no saturar la CPU
            SDL_Delay(16); // ~60 FPS
    }

}

void limpiarTodosLosRecursos(SDL_Window *ventana, SDL_Renderer *renderer, TTF_Font *fuente,TTF_Font *fuenteGrande, sCelda **matriz, Archivo_conf configuracion)
{

    printf("Limpiando recursos...\n");

    if (fuenteGrande && fuenteGrande != fuente)
        TTF_CloseFont(fuenteGrande);

    if (fuente)
        TTF_CloseFont(fuente);

    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(ventana);
    SDL_Quit();

    destruirMatriz(matriz, configuracion.dimensiones);
    destruirLog();

}
