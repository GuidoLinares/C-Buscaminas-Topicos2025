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

void manejarInmunidadPrimerClic(sCelda **matriz, int fila, int columna, int dimensiones)
{
    printf("*** PRIMER CLIC EN MINA - INMUNIDAD ACTIVADA ***\n");
    printf("Reubicando mina de posicion (%d, %d)\n", fila, columna);

    // Quitar la mina de la celda actual
    (*(matriz+fila)+columna)->tieneMina = 0;

    // Buscar una posición vacía para reubicar la mina
    int nuevaMinaColocada = 0;
    for (int r = 0; r < dimensiones && !nuevaMinaColocada; r++)
    {
        for (int c = 0; c < dimensiones && !nuevaMinaColocada; c++)
        {
            // Encontrar una celda que no tenga mina y no sea la celda clickeada
            if (!(*(matriz+r)+c)->tieneMina && !(r == fila && c == columna))
            {
                (*(matriz+r)+c)->tieneMina = 1;
                nuevaMinaColocada = 1;
                printf("Mina reubicada en posicion (%d, %d)\n", r, c);
            }
        }
    }
}

void recalcularMinasAdyacentes(sCelda **matriz, int dimensiones)
{
    for (int r = 0; r < dimensiones; r++)
    {
        for (int c = 0; c < dimensiones; c++)
        {
            if (!(*(matriz+r)+c)->tieneMina)
            {
                int minasAdyacentes = 0;
                // Contar minas en las 8 direcciones
                for (int dr = -1; dr <= 1; dr++)
                {
                    for (int dc = -1; dc <= 1; dc++)
                    {
                        int nr = r + dr;
                        int nc = c + dc;
                        if (nr >= 0 && nr < dimensiones &&
                            nc >= 0 && nc < dimensiones &&
                            (*(matriz+nr)+nc)->tieneMina)
                        {
                            minasAdyacentes++;
                        }
                    }
                }
                (*(matriz+r)+c)->minasAdyacentes = minasAdyacentes;
            }
        }
    }
}

void procesarVictoria(SDL_Renderer *renderer, TTF_Font *fuente, TTF_Font *fuenteGrande, sCelda **matriz, int dimensiones, int *minasRestantes, int ventana_ancho)
{
    printf("¡Felicidades Ganaste!\n");
    logFinPartida("VICTORIA");

    // Renderizado final de victoria
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    dibujarHeader(renderer, fuente, *minasRestantes, ventana_ancho);
    dibujarCeldas(renderer, matriz, dimensiones, fuente);
    dibujarTablero(renderer, dimensiones);

    mostrarVictoria(renderer, fuenteGrande, dimensiones);
    SDL_RenderPresent(renderer);

    SDL_Delay(4000); // Muestra la victoria por un tiempo
}

void procesarGameOver(SDL_Renderer *renderer, TTF_Font *fuente, TTF_Font *fuenteGrande,sCelda **matriz, int dimensiones, int *minasRestantes, int ventana_ancho,int fila, int columna)
{
    printf("\n*** BOOM! HAS ENCONTRADO UNA MINA ***\n");
    printf("GAME OVER - Mina en posicion (%d, %d)\n", fila, columna);
    logFinPartida("DERROTA - MINA ENCONTRADA");

    // Revelar todas las minas para mostrar el tablero final
    for (int r = 0; r < dimensiones; r++)
    {
        for (int c = 0; c < dimensiones; c++)
        {
            (*(matriz+r)+c)->esRevelada = 1;
        }
    }

    // Renderizar una vez más para mostrar todas las minas y el GAME OVER
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    dibujarHeader(renderer, fuente, *minasRestantes, ventana_ancho);
    dibujarCeldas(renderer, matriz, dimensiones, fuente);
    dibujarTablero(renderer, dimensiones);

    mostrarGameOver(renderer, fuenteGrande, dimensiones);
    SDL_RenderPresent(renderer);

    // Esperar 3 segundos para que el usuario vea el resultado
    SDL_Delay(3000);
}

int manejarClicIzquierdo(SDL_Event *e, SDL_Renderer *renderer, TTF_Font *fuente, TTF_Font *fuenteGrande,sCelda **matriz, Archivo_conf configuracion, int *minasRestantes, int ventana_ancho, int *primerClic)
{
    int mouse_x = e->button.x;
    int mouse_y = e->button.y;

    // AJUSTAR COORDENADAS RESTANDO EL HEADER
    if (mouse_y >= ALTURA_HEADER)
    {
        int columna_cliqueada = mouse_x / PIXEL_CELDA;
        int fila_cliqueada = (mouse_y - ALTURA_HEADER) / PIXEL_CELDA;

        if (fila_cliqueada >= 0 && fila_cliqueada < configuracion.dimensiones &&
            columna_cliqueada >= 0 && columna_cliqueada < configuracion.dimensiones)
        {
            logClickCelda(matriz, fila_cliqueada, columna_cliqueada, configuracion.dimensiones, "IZQUIERDO");

            // Revela la celda si no está ya revelada o marcada con bandera
            if (!(*(matriz+fila_cliqueada)+columna_cliqueada)->esRevelada &&
                !(*(matriz+fila_cliqueada)+columna_cliqueada)->tieneBandera)
            {
                // VERIFICAR INMUNIDAD EN PRIMER CLIC
                if (*primerClic && (*(matriz+fila_cliqueada)+columna_cliqueada)->tieneMina)
                {
                    manejarInmunidadPrimerClic(matriz, fila_cliqueada, columna_cliqueada, configuracion.dimensiones);
                    recalcularMinasAdyacentes(matriz, configuracion.dimensiones);
                }

                // Marcar que ya no es el primer clic
                *primerClic = 0;

                revelarEspaciosVacios(matriz, configuracion.dimensiones, fila_cliqueada, columna_cliqueada);
                logRevelarCelda(matriz, fila_cliqueada, columna_cliqueada, configuracion.dimensiones);

                // Verificar victoria
                if (verificarVictoria(matriz, configuracion.dimensiones, configuracion.cantMinas))
                {
                    procesarVictoria(renderer, fuente, fuenteGrande, matriz, configuracion.dimensiones,
                                   minasRestantes, ventana_ancho);
                    return 0; // Terminar juego
                }

                // VERIFICAR SI HAY MINA - GAME OVER (solo después del primer clic)
                if ((*(matriz+fila_cliqueada)+columna_cliqueada)->tieneMina)
                {
                    procesarGameOver(renderer, fuente, fuenteGrande, matriz, configuracion.dimensiones,
                                   minasRestantes, ventana_ancho, fila_cliqueada, columna_cliqueada);
                    return 0; // Terminar juego
                }
            }
        }
        printf("Clic Izquierdo en celda: (%d, %d)\n", fila_cliqueada, columna_cliqueada);
    }
    return 1; // Continuar juego
}

void manejarClicDerecho(SDL_Event *e, sCelda **matriz, Archivo_conf configuracion, int *minasRestantes)
{
    int mouse_x = e->button.x;
    int mouse_y = e->button.y;

    // AJUSTAR COORDENADAS RESTANDO EL HEADER
    if (mouse_y >= ALTURA_HEADER)
    {
        int columna_cliqueada = mouse_x / PIXEL_CELDA;
        int fila_cliqueada = (mouse_y - ALTURA_HEADER) / PIXEL_CELDA;

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

void renderizarJuego(SDL_Renderer *renderer, TTF_Font *fuente, sCelda **matriz, int dimensiones, int *minasRestantes, int ventana_ancho)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Fondo negro
    SDL_RenderClear(renderer);
    dibujarHeader(renderer, fuente, *minasRestantes, ventana_ancho);
    dibujarCeldas(renderer, matriz, dimensiones, fuente);
    dibujarTablero(renderer, dimensiones);
    SDL_RenderPresent(renderer);
}

void jugar(SDL_Window *ventana, SDL_Renderer *renderer, TTF_Font *fuente, TTF_Font *fuenteGrande, sCelda **matriz, Archivo_conf configuracion, int *minasRestantes, int ventana_ancho)
{
    int corriendo = 1;
    int primerClic = 1;
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
                    corriendo = manejarClicIzquierdo(&e, renderer, fuente, fuenteGrande, matriz,
                                                   configuracion, minasRestantes, ventana_ancho, &primerClic);
                }
                else if (e.button.button == SDL_BUTTON_RIGHT)
                {
                    manejarClicDerecho(&e, matriz, configuracion, minasRestantes);
                }
            }
        }

        // Renderizado
        renderizarJuego(renderer, fuente, matriz, configuracion.dimensiones, minasRestantes, ventana_ancho);

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
