#include "common.h"
#include "juego.h"
#include "logs.h"
#include "tablero.h"
#include "interfaz.h"
#include "usuario.h"
#include "menu.h"

/**
 * Activa el cheat X-Ray si aun no se ha usado en la partida actual
 * @param estadoCheat - Puntero al estado actual del cheat
 * @return 1 si se activo exitosamente, 0 si ya se uso
 */
int activarCheatXray(sEstadoCheat* estadoCheat) {
    if (estadoCheat->xrayUsado) {
        printf("CHEAT X-RAY: Ya fue usado en esta partida\n");
        return 0;
    }
    estadoCheat->xrayActivo = 1;
    estadoCheat->xrayUsado = 1;
    estadoCheat->tiempoInicioXray = SDL_GetTicks();
    printf("¡CHEAT X-RAY ACTIVADO! (2 segundos)\n");
    return 1;
}

/**
 * Actualiza el estado del cheat X-Ray verificando si debe desactivarse
 * @param estadoCheat - Puntero al estado actual del cheat
 */
void actualizarCheatXray(sEstadoCheat* estadoCheat) {
    if (estadoCheat->xrayActivo && SDL_GetTicks() - estadoCheat->tiempoInicioXray > DURACION_XRAY) {
        estadoCheat->xrayActivo = 0;
        printf("Cheat X-Ray desactivado\n");
    }
}

/**
 * Reinicia el estado del cheat para una nueva partida
 * @param estadoCheat - Puntero al estado del cheat a reiniciar
 */
void reiniciarCheatXray(sEstadoCheat* estadoCheat) {
    estadoCheat->xrayActivo = 0;
    estadoCheat->xrayUsado = 0;
    estadoCheat->tiempoInicioXray = 0;
}

/**
 * Marca el cheat como usado (para partidas cargadas)
 * @param estadoCheat - Puntero al estado del cheat a modificar
 */
void marcarCheatXrayComoUsado(sEstadoCheat* estadoCheat) {
    estadoCheat->xrayUsado = 1;
}

/**
 * Funcion principal que ejecuta una partida completa del buscaminas
 * @param ventana - Ventana SDL donde se dibuja el juego
 * @param renderer - Renderizador SDL para dibujar graficos
 * @param fuente - Fuente pequena para texto
 * @param fuenteGrande - Fuente grande para titulos
 * @param usuarios - Puntero al usuario actual
 * @param configuracion - Configuracion del juego (dimensiones, minas)
 */
void ejecutarPartida(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuarios, sArchivo_conf configuracion)
{
    // Inicializar estado del cheat
    sEstadoCheat estadoCheat = {0, 0, 0};

    sCelda **matriz;
    time_t tiempoInicio = time(NULL);
    int primerClic = 1;

    int ventana_ancho = configuracion.dimensiones * PIXEL_CELDA;
    int ventana_alto = configuracion.dimensiones * PIXEL_CELDA + ALTURA_HEADER;

    SDL_SetWindowSize(ventana, ventana_ancho, ventana_alto);
    SDL_SetWindowPosition(ventana, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

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

    jugarConGuardado(ventana, renderer, fuente, fuenteGrande, matriz, configuracion, &minasRestantes, ventana_ancho, usuarios, tiempoInicio, &primerClic, &estadoCheat);

    time_t tiempoFin = time(NULL);
    int tiempoTranscurrido = (int)difftime(tiempoFin, tiempoInicio);

    int gano = verificarVictoria(matriz, configuracion.dimensiones, configuracion.cantMinas);
    actualizarEstadisticas(usuarios, gano, tiempoTranscurrido);

    destruirMatriz(matriz, configuracion.dimensiones);
    destruirLog();

    printf("Partida terminada\n");
}

/**
 * Maneja la inmunidad del primer clic, reubicando la mina si el primer clic es en una mina
 * @param matriz - Matriz de celdas del tablero
 * @param fila - Fila donde se hizo el primer clic
 * @param columna - Columna donde se hizo el primer clic
 * @param dimensiones - Dimensiones del tablero
 */
void manejarInmunidadPrimerClic(sCelda **matriz, int fila, int columna, int dimensiones)
{
    printf("*** PRIMER CLIC EN MINA - INMUNIDAD ACTIVADA ***\n");
    printf("Reubicando mina de posicion (%d, %d)\n", fila, columna);

    // Quitar la mina de la celda actual
    (*(matriz+fila)+columna)->tieneMina = 0;

    // Buscar una posicion vacia para reubicar la mina
    int nuevaMinaColocada = 0;
    // Ciclo para buscar una posicion libre donde reubicar la mina
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

/**
 * Recalcula el numero de minas adyacentes para todas las celdas despues de reubicar una mina
 * @param matriz - Matriz de celdas del tablero
 * @param dimensiones - Dimensiones del tablero
 */
void recalcularMinasAdyacentes(sCelda **matriz, int dimensiones)
{
    // Ciclo para recorrer todas las celdas del tablero
    for (int r = 0; r < dimensiones; r++)
    {
        for (int c = 0; c < dimensiones; c++)
        {
            if (!(*(matriz+r)+c)->tieneMina)
            {
                int minasAdyacentes = 0;
                // Contar minas en las 8 direcciones alrededor de la celda actual
                for (int dr = -1; dr <= 1; dr++)
                {
                    for (int dc = -1; dc <= 1; dc++)
                    {
                        int nr = r + dr;
                        int nc = c + dc;

                        if (nr >= 0 && nr < dimensiones && nc >= 0 && nc < dimensiones && (*(matriz+nr)+nc)->tieneMina)
                            minasAdyacentes++;
                    }
                }
                (*(matriz+r)+c)->minasAdyacentes = minasAdyacentes;
            }
        }
    }
}

/**
 * Procesa la victoria del jugador, mostrando mensaje y actualizando logs
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente pequena
 * @param fuenteGrande - Fuente grande
 * @param matriz - Matriz de celdas
 * @param dimensiones - Dimensiones del tablero
 * @param minasRestantes - Puntero al contador de minas restantes
 * @param ventana_ancho - Ancho de la ventana
 */
void procesarVictoria(SDL_Renderer *renderer, TTF_Font *fuente, TTF_Font *fuenteGrande, sCelda **matriz, int dimensiones, int *minasRestantes, int ventana_ancho)
{
    printf("!Felicidades Ganaste!\n");
    logFinPartida("VICTORIA");

    // Renderizado final de victoria
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    dibujarHeader(renderer, fuente, *minasRestantes, ventana_ancho, NULL);
    dibujarCeldas(renderer, matriz, dimensiones, fuente, NULL);

    dibujarTablero(renderer, dimensiones);

    mostrarVictoria(renderer, fuenteGrande, dimensiones);
    SDL_RenderPresent(renderer);

    SDL_Delay(4000);
}

/**
 * Procesa el game over, revelando todas las celdas y mostrando mensaje
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente pequena
 * @param fuenteGrande - Fuente grande
 * @param matriz - Matriz de celdas
 * @param dimensiones - Dimensiones del tablero
 * @param minasRestantes - Puntero al contador de minas restantes
 * @param ventana_ancho - Ancho de la ventana
 * @param fila - Fila donde se encontro la mina
 * @param columna - Columna donde se encontro la mina
 */
void procesarGameOver(SDL_Renderer *renderer, TTF_Font *fuente, TTF_Font *fuenteGrande,sCelda **matriz, int dimensiones, int *minasRestantes, int ventana_ancho,int fila, int columna)
{
    printf("\n*** BOOM! HAS ENCONTRADO UNA MINA ***\n");
    printf("GAME OVER - Mina en posicion (%d, %d)\n", fila, columna);
    logFinPartida("DERROTA - MINA ENCONTRADA");

    // Revelar todas las celdas del tablero
    for (int r = 0; r < dimensiones; r++)
    {
        for (int c = 0; c < dimensiones; c++)
        {
            (*(matriz+r)+c)->esRevelada = 1;
        }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    dibujarHeader(renderer, fuente, *minasRestantes, ventana_ancho, NULL);
dibujarCeldas(renderer, matriz, dimensiones, fuente, NULL);

        dibujarTablero(renderer, dimensiones);

    mostrarGameOver(renderer, fuenteGrande, dimensiones);
    SDL_RenderPresent(renderer);

    SDL_Delay(3000);
}

/**
 * Maneja los clics izquierdos del mouse (revelar celdas)
 * @param e - Evento SDL
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente pequena
 * @param fuenteGrande - Fuente grande
 * @param matriz - Matriz de celdas
 * @param configuracion - Configuracion del juego
 * @param minasRestantes - Puntero al contador de minas restantes
 * @param ventana_ancho - Ancho de la ventana
 * @param primerClic - Puntero al flag del primer clic
 * @return 1 para continuar juego, 0 para terminar
 */
int manejarClicIzquierdo(SDL_Event *e, SDL_Renderer *renderer, TTF_Font *fuente, TTF_Font *fuenteGrande,sCelda **matriz, sArchivo_conf configuracion, int *minasRestantes, int ventana_ancho, int *primerClic)
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

            // Revela la celda si no esta ya revelada o marcada con bandera
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

                // VERIFICAR SI HAY MINA - GAME OVER (solo despues del primer clic)
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

/**
 * Maneja los clics derechos del mouse (colocar/quitar banderas)
 * @param e - Evento SDL
 * @param matriz - Matriz de celdas
 * @param configuracion - Configuracion del juego
 * @param minasRestantes - Puntero al contador de minas restantes
 */
void manejarClicDerecho(SDL_Event *e, sCelda **matriz, sArchivo_conf configuracion, int *minasRestantes)
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
            // Pone/quita bandera si la celda no esta revelada
            if (!(*(matriz+fila_cliqueada)+columna_cliqueada)->esRevelada)
            {
                // Verificar si se esta poniendo o quitando bandera
                int teniaBandera = (*(matriz+fila_cliqueada)+columna_cliqueada)->tieneBandera;

                // Cambiar estado de la bandera
                (*(matriz+fila_cliqueada)+columna_cliqueada)->tieneBandera = !teniaBandera;

                // Actualizar contador de minas restantes
                if (teniaBandera)
                    (*minasRestantes)++; // Se quito bandera
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


/**
 * Limpia todos los recursos asignados al finalizar el juego
 * @param ventana - Ventana SDL
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente pequena
 * @param fuenteGrande - Fuente grande
 * @param matriz - Matriz de celdas
 * @param configuracion - Configuracion del juego
 */
void limpiarTodosLosRecursos(SDL_Window *ventana, SDL_Renderer *renderer, TTF_Font *fuente,TTF_Font *fuenteGrande, sCelda **matriz, sArchivo_conf configuracion)
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

/**
 * Muestra el menu de pausa con opciones de guardar y continuar
 * @param ventana - Ventana SDL
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente pequena
 * @param fuenteGrande - Fuente grande
 * @param usuario - Usuario actual
 * @param matriz - Matriz del juego
 * @param configuracion - Configuracion actual
 * @param minasRestantes - Minas restantes
 * @param tiempoTranscurrido - Tiempo transcurrido
 * @param primerClic - Estado del primer clic
 * @param estadoCheat - Estado actual del cheat X-Ray
 * @return 0 para continuar, 1 para guardar y salir, 2 para salir sin guardar
 */
int mostrarMenuPausaFijo(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuario, sCelda** matriz, sArchivo_conf configuracion, int minasRestantes, int tiempoTranscurrido, int primerClic, sEstadoCheat* estadoCheat)
{
    SDL_Color blanco = {255, 255, 255, 255};
    SDL_Color amarillo = {255, 255, 0, 255};
    SDL_Color azulOscuro = {20, 30, 50, 200};
    SDL_Color rojo = {255, 0, 0, 255};

    int opcionSeleccionada = 0;
    const int numOpciones = 4;
    const char* opciones[] = {
        "Continuar",
        "Guardar y Salir",
        "Salir sin Guardar",
        "Cancelar"
    };

    char nombrePartida[100] = "";
    int escribiendoNombre = 0;
    int mostrarMensaje = 0;
    char mensaje[150] = "";

    // Bucle del menu de pausa
    while (1) {
        SDL_Event e;
        // Ciclo para procesar eventos del menu de pausa
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                if (escribiendoNombre)
                    SDL_StopTextInput();
                return 2; // Salir sin guardar
            }

            if (escribiendoNombre)
            {
                if (e.type == SDL_TEXTINPUT && strlen(nombrePartida) < 95)
                {
                    strcat(nombrePartida, e.text.text);
                }
                else if (e.type == SDL_KEYDOWN)
                {
                    if (e.key.keysym.sym == SDLK_BACKSPACE && strlen(nombrePartida) > 0)
                    {
                        nombrePartida[strlen(nombrePartida) - 1] = '\0';
                    }
                    else if (e.key.keysym.sym == SDLK_RETURN && strlen(nombrePartida) > 0)
                    {
                        // Guardar partida incluyendo estado del cheat
                        int slot = guardarPartidaCompleta(usuario, nombrePartida, matriz, configuracion, minasRestantes, tiempoTranscurrido, primerClic, estadoCheat->xrayUsado);
                        if (slot >= 0)
                        {
                            guardarUsuario(usuario);
                            SDL_StopTextInput();
                            return 1; // Guardar y salir
                        }
                        else
                        {
                            strcpy(mensaje, "Error al guardar. Sin slots libres.");
                            mostrarMensaje = 1;
                            escribiendoNombre = 0;
                            SDL_StopTextInput();
                        }
                    } else if (e.key.keysym.sym == SDLK_ESCAPE)
                    {
                        escribiendoNombre = 0;
                        strcpy(nombrePartida, "");
                        SDL_StopTextInput();
                    }
                }
            } else {
                if (e.type == SDL_KEYDOWN) {
                    switch (e.key.keysym.sym) {
                        case SDLK_UP:
                            opcionSeleccionada = (opcionSeleccionada - 1 + numOpciones) % numOpciones;
                            mostrarMensaje = 0;
                            break;
                        case SDLK_DOWN:
                            opcionSeleccionada = (opcionSeleccionada + 1) % numOpciones;
                            mostrarMensaje = 0;
                            break;
                        case SDLK_RETURN:
                            switch (opcionSeleccionada)
                            {
                                case 0: // Continuar
                                case 3: // Cancelar
                                    return 0;
                                case 1: // Guardar y salir
                                    if (buscarSlotLibre(usuario) == -1)
                                    {
                                        strcpy(mensaje, "No hay slots libres para guardar");
                                        mostrarMensaje = 1;
                                    } else {
                                        escribiendoNombre = 1;
                                        SDL_StartTextInput();
                                        strcpy(nombrePartida, "");
                                        mostrarMensaje = 0;
                                    }
                                    break;
                                case 2: // Salir sin guardar
                                    return 2;
                            }
                            break;
                        case SDLK_ESCAPE:
                            return 0; // Continuar
                    }
                }
            }
        }

        // Renderizado del menu de pausa
        SDL_SetRenderDrawColor(renderer, azulOscuro.r, azulOscuro.g, azulOscuro.b, azulOscuro.a);
        SDL_Rect overlay = {0, 0, configuracion.dimensiones * PIXEL_CELDA,
                           configuracion.dimensiones * PIXEL_CELDA + ALTURA_HEADER};
        SDL_RenderFillRect(renderer, &overlay);

        if (fuenteGrande)
        {
            int cx = (configuracion.dimensiones * PIXEL_CELDA) / 2;
            int cy = (configuracion.dimensiones * PIXEL_CELDA + ALTURA_HEADER) / 2;

            // Titulo
            SDL_Surface* superficie = TTF_RenderText_Solid(fuenteGrande, "JUEGO PAUSADO", blanco);
            if (superficie)
            {
                SDL_Texture* textura = SDL_CreateTextureFromSurface(renderer, superficie);
                SDL_Rect rect = {cx - superficie->w/2, cy - 120, superficie->w, superficie->h};
                SDL_RenderCopy(renderer, textura, NULL, &rect);
                SDL_DestroyTexture(textura);
                SDL_FreeSurface(superficie);
            }

            if (escribiendoNombre)
            {
                // Modo escribir nombre
                if (fuente)
                {
                    superficie = TTF_RenderText_Solid(fuente, "Nombre de la partida:", blanco);
                    if (superficie)
                    {
                        SDL_Texture* textura = SDL_CreateTextureFromSurface(renderer, superficie);
                        SDL_Rect rect = {cx - superficie->w/2, cy - 60, superficie->w, superficie->h};
                        SDL_RenderCopy(renderer, textura, NULL, &rect);
                        SDL_DestroyTexture(textura);
                        SDL_FreeSurface(superficie);
                    }

                    // Campo de texto con cursor
                    char textoMostrar[110];
                    sprintf(textoMostrar, "%s_", nombrePartida);
                    superficie = TTF_RenderText_Solid(fuente, textoMostrar, amarillo);
                    if (superficie)
                    {
                        SDL_Texture* textura = SDL_CreateTextureFromSurface(renderer, superficie);
                        SDL_Rect rect = {cx - superficie->w/2, cy - 20, superficie->w, superficie->h};
                        SDL_RenderCopy(renderer, textura, NULL, &rect);
                        SDL_DestroyTexture(textura);
                        SDL_FreeSurface(superficie);
                    }

                    superficie = TTF_RenderText_Solid(fuente, "ENTER para guardar, ESC para cancelar", blanco);
                    if (superficie)
                    {
                        SDL_Texture* textura = SDL_CreateTextureFromSurface(renderer, superficie);
                        SDL_Rect rect = {cx - superficie->w/2, cy + 20, superficie->w, superficie->h};
                        SDL_RenderCopy(renderer, textura, NULL, &rect);
                        SDL_DestroyTexture(textura);
                        SDL_FreeSurface(superficie);
                    }
                }
            } else {
                // Mostrar opciones del menu - ciclo para dibujar todas las opciones
                for (int i = 0; i < numOpciones; i++)
                {
                    SDL_Color color = (i == opcionSeleccionada) ? amarillo : blanco;

                    if (fuente)
                    {
                        superficie = TTF_RenderText_Solid(fuente, opciones[i], color);
                        if (superficie)
                        {
                            SDL_Texture* textura = SDL_CreateTextureFromSurface(renderer, superficie);
                            SDL_Rect rect = {cx - superficie->w/2, cy - 40 + i * 30, superficie->w, superficie->h};
                            SDL_RenderCopy(renderer, textura, NULL, &rect);
                            SDL_DestroyTexture(textura);
                            SDL_FreeSurface(superficie);
                        }
                    }
                }

                if (fuente)
                {
                    superficie = TTF_RenderText_Solid(fuente, "Usa las flechas y ENTER, ESC para continuar", blanco);
                    if (superficie)
                    {
                        SDL_Texture* textura = SDL_CreateTextureFromSurface(renderer, superficie);
                        SDL_Rect rect = {cx - superficie->w/2, cy + 100, superficie->w, superficie->h};
                        SDL_RenderCopy(renderer, textura, NULL, &rect);
                        SDL_DestroyTexture(textura);
                        SDL_FreeSurface(superficie);
                    }
                }

                // Mostrar mensaje de error si existe
                if (mostrarMensaje && fuente)
                {
                    superficie = TTF_RenderText_Solid(fuente, mensaje, rojo);
                    if (superficie)
                    {
                        SDL_Texture* textura = SDL_CreateTextureFromSurface(renderer, superficie);
                        SDL_Rect rect = {cx - superficie->w/2, cy + 130, superficie->w, superficie->h};
                        SDL_RenderCopy(renderer, textura, NULL, &rect);
                        SDL_DestroyTexture(textura);
                        SDL_FreeSurface(superficie);
                    }
                }
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

/**
 * Bucle principal del juego modificado para incluir pausa, guardado y cheat X-Ray
 * @param ventana - Ventana SDL
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente pequena
 * @param fuenteGrande - Fuente grande
 * @param matriz - Matriz de celdas
 * @param configuracion - Configuracion del juego
 * @param minasRestantes - Puntero al contador de minas restantes
 * @param ventana_ancho - Ancho de la ventana
 * @param usuario - Usuario actual (para guardar partidas)
 * @param tiempoInicioPartida - Tiempo cuando comenzo la partida
 * @param primerClic - Puntero al estado del primer clic
 * @param estadoCheat - Puntero al estado del cheat X-Ray
 */
void jugarConGuardado(SDL_Window *ventana, SDL_Renderer *renderer, TTF_Font *fuente, TTF_Font *fuenteGrande, sCelda **matriz, sArchivo_conf configuracion, int *minasRestantes, int ventana_ancho, sUsuario* usuario, time_t tiempoInicioPartida, int* primerClic, sEstadoCheat* estadoCheat)
{
    int corriendo = 1;
    SDL_Event e;

    printf("Juego iniciado. Presiona ESC o P para pausar y guardar.\n");
    printf("Presiona Ctrl+H para activar X-Ray (solo una vez por partida)\n");

    // Bucle principal del juego - maneja eventos y renderizado
    while (corriendo)
    {
        // Ciclo para procesar todos los eventos pendientes
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                corriendo = 0;
                printf("Usuario cerro la ventana\n");
            }

            if (e.type == SDL_KEYDOWN)
            {
                // Cheat X-Ray
                if (e.key.keysym.sym == SDLK_h && (SDL_GetModState() & KMOD_CTRL))
                {
                    activarCheatXray(estadoCheat);
                }

                if (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_p)
                {
                    // Pausar juego y mostrar menu
                    int tiempoTranscurrido = (int)difftime(time(NULL), tiempoInicioPartida);
                    int resultadoPausa = mostrarMenuPausaFijo(ventana, renderer, fuente, fuenteGrande, usuario, matriz, configuracion, *minasRestantes, tiempoTranscurrido, *primerClic, estadoCheat);

                    switch (resultadoPausa)
                    {
                        case 0: // Continuar
                            printf("Continuando juego...\n");
                            break;
                        case 1: // Guardar y salir
                            printf("Partida guardada. Saliendo del juego.\n");
                            corriendo = 0;
                            break;
                        case 2: // Salir sin guardar
                            printf("Saliendo sin guardar.\n");
                            corriendo = 0;
                            break;
                    }
                }
            }

            if (e.type == SDL_MOUSEBUTTONDOWN && corriendo)
            {
                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    corriendo = manejarClicIzquierdo(&e, renderer, fuente, fuenteGrande, matriz, configuracion, minasRestantes, ventana_ancho, primerClic);
                }
                else if (e.button.button == SDL_BUTTON_RIGHT)
                {
                    manejarClicDerecho(&e, matriz, configuracion, minasRestantes);
                }
            }
        }

        // Renderizado solo si el juego sigue corriendo
        if (corriendo) {
            actualizarCheatXray(estadoCheat);
SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
SDL_RenderClear(renderer);
dibujarHeader(renderer, fuente, *minasRestantes, ventana_ancho, estadoCheat);
dibujarCeldas(renderer, matriz, configuracion.dimensiones, fuente, estadoCheat);
dibujarTablero(renderer, configuracion.dimensiones);
SDL_RenderPresent(renderer);        }

        // Pequena pausa para controlar FPS
        SDL_Delay(16);
    }
}











