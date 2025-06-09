
#include "common.h"
#include "tablero.h"
#include "juego.h"
#include "logs.h"
#include "menu.h"
#include "usuario.h"


/*
Apellido(s), nombre(s): Linares, Guido Hernan
DNI: 43170056
Entrega: Sí

Apellido(s), nombre(s): Goldring, Facundo
DNI: 44595085
Entrega: Sí

Apellido(s), nombre(s): Calvet, Lucas
DNI: (pongan su DNI)
Entrega: NO
*/




int main(int argc, char *argv[])
{

    // === CONFIGURACIÓN DESDE ARCHIVOg===
    Archivo_conf configuracion = leerArchivo();

    // === INICIALIZACIÓN SDL ===
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error al inicializar SDL: %s\n", SDL_GetError());
        return -1;
    }

    if (TTF_Init() == -1) {
        printf("Error al inicializar TTF: %s\n", TTF_GetError());
        SDL_Quit();
        return -1;
    }

    // Tamaño inicial para menús, se ajustará dinámicamente para el juego
    int ventana_ancho = 900;
    int ventana_alto = 800;

    SDL_Window *ventana = SDL_CreateWindow("BUSCAMINAS AVANZADO",
                                           SDL_WINDOWPOS_CENTERED,
                                           SDL_WINDOWPOS_CENTERED,
                                           ventana_ancho,
                                           ventana_alto,
                                           SDL_WINDOW_SHOWN);

    if (!ventana) {
        printf("Error al crear la ventana: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(ventana, -1,
                                                 SDL_RENDERER_ACCELERATED |
                                                 SDL_RENDERER_PRESENTVSYNC);

    if (!renderer) {
        printf("Error al crear el renderizador: %s\n", SDL_GetError());
        SDL_DestroyWindow(ventana);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Cargar fuentes (tu código original)
    TTF_Font* fuente = NULL;
    TTF_Font* fuenteGrande = NULL;
    const char* fuentes[] = {
        "arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "/System/Library/Fonts/Arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        NULL
    };

    for (int i = 0; fuentes[i] != NULL; i++) {
        fuente = TTF_OpenFont(fuentes[i], 18);
        if (fuente) break;
    }

    for (int i = 0; fuentes[i] != NULL; i++) {
        fuenteGrande = TTF_OpenFont(fuentes[i], 32);
        if (fuenteGrande) break;
    }

    if (!fuente) {
        printf("Advertencia: No se pudo cargar fuente\n");
    }
    if (!fuenteGrande) {
        fuenteGrande = fuente;
    }

    // === GESTIÓN DE USUARIO EN SDL ===
    Usuario usuarioActual = {0};
    if (pantallaIngreso(ventana, renderer, fuente, fuenteGrande, &usuarioActual) != 0) {
        printf("Error en el sistema de usuarios\n");
        // Limpieza y salida
        if (fuenteGrande && fuenteGrande != fuente) TTF_CloseFont(fuenteGrande);
        if (fuente) TTF_CloseFont(fuente);
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(ventana);
        SDL_Quit();
        return -1;
    }

    // === BUCLE DE MENÚ PRINCIPAL EN SDL ===
    int continuar = 1;
    while (continuar) {
        int opcion = mostrarMenuSDL(ventana, renderer, fuente, fuenteGrande, &usuarioActual);

        switch (opcion) {
            case 1: // Nueva partida
                ejecutarPartida(ventana, renderer, fuente, fuenteGrande, &usuarioActual, configuracion);
                break;
            case 2: // Estadísticas
                mostrarEstadisticasSDL(ventana, renderer, fuente, fuenteGrande, &usuarioActual);
                break;
            case 3: // Cargar partida
                cargarPartidaSDL(ventana, renderer, fuente, fuenteGrande, &usuarioActual, configuracion);
                break;
            case 4: // Recargar configuración
                configuracion = leerArchivo();
                printf("Configuracion recargada: Dimensiones=%d, Minas=%d\n",
                       configuracion.dimensiones, configuracion.cantMinas);
                break;
            case 5: // Salir
                continuar = 0;
                break;
        }
    }

    // Guardar datos del usuario al salir
    guardarUsuario(&usuarioActual);

    // Limpieza final
    if (fuenteGrande && fuenteGrande != fuente) TTF_CloseFont(fuenteGrande);
    if (fuente) TTF_CloseFont(fuente);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(ventana);
    SDL_Quit();

    return 0;
}
