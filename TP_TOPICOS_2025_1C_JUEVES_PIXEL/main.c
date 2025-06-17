#include "common.h"
#include "tablero.h"
#include "juego.h"
#include "logs.h"
#include "menu.h"
#include "usuario.h"

/*
Apellido(s), nombre(s): Linares, Guido Hernan
DNI: 43170056
Entrega: Si

Apellido(s), nombre(s): Goldring, Facundo
DNI: 44595085
Entrega: Si

Apellido(s), nombre(s): Calvet, Lucas
DNI: N/A
Entrega: NO
*/


int main(int argc, char *argv[])
{
    // === CONFIGURACION DESDE ARCHIVO ===
    sArchivo_conf configuracion = leerArchivo();

    // === INICIALIZACION SDL ===
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Error al inicializar SDL: %s\n", SDL_GetError());
        return -1;
    }

    if (TTF_Init() == -1)
    {
        printf("Error al inicializar TTF: %s\n", TTF_GetError());
        SDL_Quit();
        return -1;
    }

    // Tamano inicial para menus, se ajustara dinamicamente para el juego
    int ventana_ancho = 900;
    int ventana_alto = 800;

    SDL_Window *ventana = SDL_CreateWindow("BUSCAMINAS AVANZADO",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,ventana_ancho,ventana_alto,SDL_WINDOW_SHOWN);

    if (!ventana)
    {
        printf("Error al crear la ventana: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(ventana, -1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer)
    {
        printf("Error al crear el renderizador: %s\n", SDL_GetError());
        SDL_DestroyWindow(ventana);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Cargar fuentes - busca en multiples ubicaciones posibles
    TTF_Font* fuente = NULL;
    TTF_Font* fuenteGrande = NULL;
    const char* fuentes[] =
    {
        "ARIAL.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "/System/Library/Fonts/Arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        NULL
    };

    // Ciclo para intentar cargar fuente pequena desde diferentes ubicaciones
    for (int i = 0; fuentes[i] != NULL; i++)
    {
        fuente = TTF_OpenFont(fuentes[i], 18);
        if (fuente)
            break;
    }

    // Ciclo para intentar cargar fuente grande desde diferentes ubicaciones
    for (int i = 0; fuentes[i] != NULL; i++)
    {
        fuenteGrande = TTF_OpenFont(fuentes[i], 32);
        if (fuenteGrande)
            break;
    }

    if (!fuente)
        printf("Advertencia: No se pudo cargar fuente\n");

    if (!fuenteGrande)
        fuenteGrande = fuente;

    // === GESTION DE USUARIO EN SDL ===
    sUsuario usuarioActual = {0};
    if (pantallaIngreso(ventana, renderer, fuente, fuenteGrande, &usuarioActual) != 0)
    {
        printf("Error en el sistema de usuarios\n");

        if (fuenteGrande && fuenteGrande != fuente)
            TTF_CloseFont(fuenteGrande);

        if (fuente)
            TTF_CloseFont(fuente);

        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(ventana);
        SDL_Quit();
        return -1;
    }

    // === BUCLE DE MENU PRINCIPAL EN SDL ===
    int continuar = 1;
    // Bucle principal del programa - mantiene el menu activo hasta que el usuario salga
    while (continuar)
    {
        int opcion = mostrarMenuSDL(ventana, renderer, fuente, fuenteGrande, &usuarioActual);

        switch (opcion)
        {
            case 1: // Nueva partida
                ejecutarPartida(ventana, renderer, fuente, fuenteGrande, &usuarioActual, configuracion);
                break;
            case 2: // Estadisticas
                mostrarEstadisticasSDL(ventana, renderer, fuente, fuenteGrande, &usuarioActual);
                break;
            case 3: // Cargar partida
                cargarPartidaSDL(ventana, renderer, fuente, fuenteGrande, &usuarioActual, configuracion);
                break;
            case 4: // Configurar juego
                {
                    int resultado = mostrarMenuConfiguracion(ventana, renderer, fuente, fuenteGrande, &configuracion);
                    if (resultado == 1)
                    {
                        // Configuracion guardada exitosamente
                        printf("Configuracion actualizada: Dimensiones=%dx%d, Minas=%d\n",configuracion.dimensiones, configuracion.dimensiones, configuracion.cantMinas);

                        // Mostrar mensaje de confirmacion por 2 segundos
                        SDL_SetRenderDrawColor(renderer, 30, 42, 75, 255);
                        SDL_RenderClear(renderer);

                        if (fuenteGrande)
                        renderizarTexto(renderer, fuenteGrande, "CONFIGURACION GUARDADA", 0, 250, (SDL_Color){0, 255, 0, 255}, (SDL_Color){0, 0, 0, 255}, 1, 1);

                        char detalles[150];
                        sprintf(detalles, "Nuevo tablero: %dx%d con %d minas",configuracion.dimensiones, configuracion.dimensiones, configuracion.cantMinas);
                        if (fuente)
                        renderizarTexto(renderer, fuente, detalles, 0, 300, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255}, 1, 1);

                        SDL_RenderPresent(renderer);
                        SDL_Delay(2000);
                    }
                }
                break;
            case 5: // Instrucciones
                mostrarInstruccionesSDL(ventana, renderer, fuente, fuenteGrande);
                break;
            case 6: // Salir
                continuar = 0;
                break;
        }
    }

    // Guardar datos del usuario al salir
    guardarUsuario(&usuarioActual);

    // Limpieza final de todos los recursos
    if (fuenteGrande && fuenteGrande != fuente)
        TTF_CloseFont(fuenteGrande);
    if (fuente) TTF_CloseFont(fuente);
        TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(ventana);
    SDL_Quit();

    return 0;
}
