#include "common.h"
#include "interfaz.h"

/**
 * Dibuja las lineas del tablero (cuadricula) en el renderizador SDL
 * @param renderizador - Puntero al renderizador SDL
 * @param dimensiones - Numero de filas y columnas del tablero
 */
void dibujarTablero(SDL_Renderer *renderizador, int dimensiones)
{
    SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255);

    // Ciclo para dibujar todas las lineas verticales y horizontales del tablero
    for (int i = 0; i <= dimensiones; i++)
    {
        // Dibujar lineas verticales
        SDL_RenderDrawLine(renderizador, i * PIXEL_CELDA, ALTURA_HEADER, i * PIXEL_CELDA, dimensiones * PIXEL_CELDA + ALTURA_HEADER);
        // Dibujar lineas horizontales
        SDL_RenderDrawLine(renderizador, 0, i * PIXEL_CELDA + ALTURA_HEADER, dimensiones * PIXEL_CELDA, i * PIXEL_CELDA + ALTURA_HEADER);
    }
}

/**
 * Renderiza una celda individual con sus contenidos (bandera, mina, numero)
 * @param renderizador - Puntero al renderizador SDL
 * @param rectCelda - Rectangulo donde dibujar la celda
 * @param celda - Datos de la celda a renderizar
 * @param fuente - Fuente para renderizar texto
 * @param aplicarXray - 1 si aplicar efecto X-Ray, 0 para renderizado normal
 */
void renderizarCeldaIndividual(SDL_Renderer* renderizador, SDL_Rect rectCelda, sCelda* celda, TTF_Font* fuente, int aplicarXray)
{
    if (!celda->esRevelada) // Celda no revelada
    {
        // Aplicar efecto X-Ray si está activo
        if (aplicarXray) {
            if (celda->tieneMina) {
                // Fondo rojo semi-transparente para minas
                SDL_SetRenderDrawColor(renderizador, 255, 100, 100, 255);
                SDL_RenderFillRect(renderizador, &rectCelda);

                // Bomba con efecto fantasma
                int cx = rectCelda.x + PIXEL_CELDA / 2;
                int cy = rectCelda.y + PIXEL_CELDA / 2;

                SDL_SetRenderDrawColor(renderizador, 100, 0, 0, 255);
                int radio = 4;
                for (int y = -radio; y <= radio; y++) {
                    for (int x = -radio; x <= radio; x++) {
                        if (x*x + y*y <= radio*radio) {
                            SDL_RenderDrawPoint(renderizador, cx + x, cy + y);
                        }
                    }
                }

                // Texto "M" para mina
                SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255);
                SDL_RenderDrawPoint(renderizador, cx - 2, cy - 2);
                SDL_RenderDrawPoint(renderizador, cx + 2, cy - 2);
                SDL_RenderDrawPoint(renderizador, cx, cy);

            } else {
                // Fondo verde semi-transparente para celdas seguras
                SDL_SetRenderDrawColor(renderizador, 100, 255, 100, 255);
                SDL_RenderFillRect(renderizador, &rectCelda);

                // Mostrar numero de minas adyacentes si existe
                if (celda->minasAdyacentes > 0 && fuente) {
                    char numero[2];
                    sprintf(numero, "%d", celda->minasAdyacentes);

                    SDL_Color colorNum = {50, 50, 50, 255};
                    SDL_Surface* superficie = TTF_RenderText_Solid(fuente, numero, colorNum);
                    if (superficie) {
                        SDL_Texture* textura = SDL_CreateTextureFromSurface(renderizador, superficie);
                        if (textura) {
                            int x = rectCelda.x + (PIXEL_CELDA - superficie->w) / 2;
                            int y = rectCelda.y + (PIXEL_CELDA - superficie->h) / 2;
                            SDL_Rect destRect = {x, y, superficie->w, superficie->h};
                            SDL_RenderCopy(renderizador, textura, NULL, &destRect);
                            SDL_DestroyTexture(textura);
                        }
                        SDL_FreeSurface(superficie);
                    }
                }
            }

            // Borde brillante para indicar efecto X-Ray
            SDL_SetRenderDrawColor(renderizador, 255, 255, 0, 255);
            SDL_RenderDrawRect(renderizador, &rectCelda);
        }
        else {
            // Renderizado normal de celda no revelada
            SDL_SetRenderDrawColor(renderizador, 30, 42, 75, 255);
            SDL_RenderFillRect(renderizador, &rectCelda);

            if (celda->tieneBandera)
            {
                int cx = rectCelda.x + PIXEL_CELDA / 2;
                int cy = rectCelda.y + PIXEL_CELDA / 2;

                //DIBUJAR BANDERA
                SDL_SetRenderDrawColor(renderizador, 139, 69, 19, 255);
                SDL_RenderDrawLine(renderizador, cx, cy - 6, cx, cy + 6);
                SDL_Rect banderaRect;
                banderaRect.x = cx - 7;
                banderaRect.y = cy - 6;
                banderaRect.w = 6;
                banderaRect.h = 6;
                SDL_SetRenderDrawColor(renderizador, 255, 0, 0, 255);
                SDL_RenderFillRect(renderizador, &banderaRect);
            }
        }
    }
    else // Celda revelada
    {
        if (celda->tieneMina)
        {
            // Fondo rojo brillante
            SDL_SetRenderDrawColor(renderizador, 220, 20, 20, 255);
            SDL_RenderFillRect(renderizador, &rectCelda);

            int cx = rectCelda.x + PIXEL_CELDA / 2;
            int cy = rectCelda.y + PIXEL_CELDA / 2;

            // Cuerpo principal (negro)
            SDL_SetRenderDrawColor(renderizador, 20, 20, 20, 255);
            int radio = 6;
            for (int y = -radio; y <= radio; y++) {
                for (int x = -radio; x <= radio; x++) {
                    if (x*x + y*y <= radio*radio) {
                        SDL_RenderDrawPoint(renderizador, cx + x, cy + y);
                    }
                }
            }

            // Brillo cartoon
            SDL_SetRenderDrawColor(renderizador, 200, 200, 200, 255);
            SDL_RenderDrawPoint(renderizador, cx - 3, cy - 3);
            SDL_RenderDrawPoint(renderizador, cx - 2, cy - 3);
            SDL_RenderDrawPoint(renderizador, cx - 3, cy - 2);

            // Mecha gruesa
            SDL_SetRenderDrawColor(renderizador, 101, 67, 33, 255); // Marron
            SDL_RenderDrawLine(renderizador, cx - 1, cy - 6, cx - 2, cy - 10);
            SDL_RenderDrawLine(renderizador, cx, cy - 6, cx - 1, cy - 10);

            // Explosion en la punta
            SDL_SetRenderDrawColor(renderizador, 255, 0, 0, 255);
            SDL_RenderDrawPoint(renderizador, cx - 2, cy - 11);
            SDL_RenderDrawPoint(renderizador, cx - 1, cy - 12);
            SDL_RenderDrawPoint(renderizador, cx - 3, cy - 10);
            SDL_SetRenderDrawColor(renderizador, 255, 255, 0, 255);
            SDL_RenderDrawPoint(renderizador, cx - 1, cy - 11);
            SDL_RenderDrawPoint(renderizador, cx - 2, cy - 10);
        }
        else // Celda revelada y sin mina
        {
            // Fondo de color segun la cantidad de minas adyacentes
            switch (celda->minasAdyacentes)
            {
                case 0:
                    SDL_SetRenderDrawColor(renderizador, 190, 225, 245, 255);
                    break;
                case 1:
                    SDL_SetRenderDrawColor(renderizador, 150, 195, 220, 255);
                    break;
                case 2:
                    SDL_SetRenderDrawColor(renderizador, 115, 165, 195, 255);
                    break;
                case 3:
                    SDL_SetRenderDrawColor(renderizador, 85, 135, 170, 255);
                    break;
                case 4:
                    SDL_SetRenderDrawColor(renderizador, 60, 105, 140, 255);
                    break;
                case 5:
                    SDL_SetRenderDrawColor(renderizador, 40, 75, 110, 255);
                    break;
                case 6:
                    SDL_SetRenderDrawColor(renderizador, 25, 50, 80, 255);
                    break;
                case 7:
                    SDL_SetRenderDrawColor(renderizador, 15, 30, 55, 255);
                    break;
                case 8:
                    SDL_SetRenderDrawColor(renderizador, 8, 15, 35, 255);
                    break;
                default:
                    SDL_SetRenderDrawColor(renderizador, 115, 165, 195, 255);
                    break;
            }
            SDL_RenderFillRect(renderizador, &rectCelda);

            if (celda->minasAdyacentes > 0 && fuente != NULL)
            {
                char texto[12];
                sprintf(texto,"%d", celda->minasAdyacentes);
                SDL_Color colorTexto = {255, 255, 255};

                SDL_Surface* superficieTexto = TTF_RenderText_Solid(fuente, texto, colorTexto);
                if (superficieTexto == NULL) {
                    fprintf(stderr, "Error al crear superficie de texto: %s\n", TTF_GetError());
                } else {
                    SDL_Texture* texturaTexto = SDL_CreateTextureFromSurface(renderizador, superficieTexto);
                    if (texturaTexto == NULL) {
                        fprintf(stderr, "Error al crear textura de texto: %s\n", SDL_GetError());
                        SDL_FreeSurface(superficieTexto);
                    } else {
                        SDL_Rect destinoTexto;
                        destinoTexto.w = superficieTexto->w;
                        destinoTexto.h = superficieTexto->h;
                        destinoTexto.x = rectCelda.x + (PIXEL_CELDA - destinoTexto.w) / 2;
                        destinoTexto.y = rectCelda.y + (PIXEL_CELDA - destinoTexto.h) / 2;

                        SDL_RenderCopy(renderizador, texturaTexto, NULL, &destinoTexto);

                        SDL_FreeSurface(superficieTexto);
                        SDL_DestroyTexture(texturaTexto);
                    }
                }
            }
        }
    }
}

/**
 * Dibuja el contenido de cada celda del tablero (FUNCIÓN UNIFICADA)
 * @param renderizador - Puntero al renderizador SDL
 * @param matriz - Matriz de celdas del tablero
 * @param dimensiones - Dimensiones del tablero
 * @param fuente - Fuente para renderizar texto
 * @param estadoCheat - Estado actual del cheat X-Ray (NULL para renderizado normal)
 */
void dibujarCeldas(SDL_Renderer* renderizador, sCelda** matriz, int dimensiones, TTF_Font* fuente, sEstadoCheat* estadoCheat)
{
    sCelda** punteroFila = matriz;
    sCelda** punteroFilaFin = matriz + dimensiones;

    // Ciclo principal para recorrer todas las filas de la matriz
    for (int r = 0; punteroFila < punteroFilaFin; punteroFila++, r++)
    {
        sCelda* punteroColumna = *punteroFila;
        sCelda* punteroColumnaFin = *punteroFila + dimensiones;

        // Ciclo para recorrer todas las columnas de la fila actual
        for (int c = 0; punteroColumna < punteroColumnaFin; punteroColumna++, c++)
        {
            SDL_Rect rectCelda;
            rectCelda.x = c * PIXEL_CELDA;
            rectCelda.y = r * PIXEL_CELDA + ALTURA_HEADER;
            rectCelda.w = PIXEL_CELDA;
            rectCelda.h = PIXEL_CELDA;

            // Determinar si aplicar X-Ray
            int aplicarXray = (estadoCheat && estadoCheat->xrayActivo);

            // Renderizar la celda usando la función unificada
            renderizarCeldaIndividual(renderizador, rectCelda, punteroColumna, fuente, aplicarXray);
        }
    }
}

/**
 * Dibuja la seccion superior de la ventana con titulo, contador de minas y estado del cheat
 * @param renderer - Puntero al renderizador SDL
 * @param fuente - Fuente para renderizar texto
 * @param minasRestantes - Numero de minas sin marcar
 * @param anchoVentana - Ancho de la ventana para centrar texto
 * @param estadoCheat - Estado actual del cheat X-Ray
 */
void dibujarHeader(SDL_Renderer *renderer, TTF_Font *fuente, int minasRestantes, int anchoVentana, sEstadoCheat* estadoCheat)
{
    // Dibujar fondo del header (azul oscuro) - UN POCO MAS ALTO
    SDL_SetRenderDrawColor(renderer, 30, 42, 75, 255);
    SDL_Rect rectHeader = {0, 0, anchoVentana, ALTURA_HEADER};
    SDL_RenderFillRect(renderer, &rectHeader);

    if (fuente != NULL)
    {
        SDL_Color sombra = {0, 0, 0, 255};                // Negro
        SDL_Color colorTexto = {173, 216, 230, 255};      // Celeste claro
        SDL_Color amarillo = {255, 255, 0, 255};          // Amarillo para X-RAY

        // ========== DIBUJAR TITULO "BUSCAMINAS PIXEL" CON SOMBRA ==========
        SDL_Surface *superficieSombraTitulo = TTF_RenderText_Solid(fuente, "BUSCAMINAS PIXEL", sombra);
        SDL_Surface *superficieTitulo = TTF_RenderText_Solid(fuente, "BUSCAMINAS PIXEL", colorTexto);
        if (superficieTitulo != NULL && superficieSombraTitulo != NULL)
        {
            SDL_Texture *texturaTitulo = SDL_CreateTextureFromSurface(renderer, superficieTitulo);
            SDL_Texture *texturaSombraTitulo = SDL_CreateTextureFromSurface(renderer, superficieSombraTitulo);
            int anchoTitulo = superficieTitulo->w;
            int altoTitulo = superficieTitulo->h;
            int xTitulo = (anchoVentana - anchoTitulo) / 2;
            int yTitulo = 30;
            SDL_Rect rectTitulo = {xTitulo, yTitulo, anchoTitulo, altoTitulo};
            SDL_Rect rectSombra = {xTitulo + 1, yTitulo + 1, anchoTitulo, altoTitulo};
            SDL_RenderCopy(renderer, texturaSombraTitulo, NULL, &rectSombra);
            SDL_RenderCopy(renderer, texturaTitulo, NULL, &rectTitulo);
            SDL_DestroyTexture(texturaTitulo);
            SDL_DestroyTexture(texturaSombraTitulo);
            SDL_FreeSurface(superficieTitulo);
            SDL_FreeSurface(superficieSombraTitulo);
        }

        // ========== DIBUJAR CONTADOR DE MINAS CON SOMBRA ==========
        char texto[50];
        snprintf(texto, sizeof(texto), "Minas restantes: %d", minasRestantes);
        SDL_Surface *superficieSombraTexto = TTF_RenderText_Solid(fuente, texto, sombra);
        SDL_Surface *superficieTexto = TTF_RenderText_Solid(fuente, texto, colorTexto);
        if (superficieTexto != NULL && superficieSombraTexto != NULL)
        {
            SDL_Texture *texturaTexto = SDL_CreateTextureFromSurface(renderer, superficieTexto);
            SDL_Texture *texturaSombraTexto = SDL_CreateTextureFromSurface(renderer, superficieSombraTexto);
            int anchoTexto = superficieTexto->w;
            int altoTexto = superficieTexto->h;
            int xTexto = (anchoVentana - anchoTexto) / 2;
            int yTexto = ALTURA_HEADER - altoTexto - 10;
            SDL_Rect rectTexto = {xTexto, yTexto, anchoTexto, altoTexto};
            SDL_Rect rectSombraTexto = {xTexto + 1, yTexto + 1, anchoTexto, altoTexto};
            SDL_RenderCopy(renderer, texturaSombraTexto, NULL, &rectSombraTexto);
            SDL_RenderCopy(renderer, texturaTexto, NULL, &rectTexto);
            SDL_DestroyTexture(texturaTexto);
            SDL_DestroyTexture(texturaSombraTexto);
            SDL_FreeSurface(superficieTexto);
            SDL_FreeSurface(superficieSombraTexto);
        }

        // ========== DIBUJAR MENSAJE X-RAY CON SOMBRA (ESQUINA SUPERIOR DERECHA) ==========
        if (estadoCheat && estadoCheat->xrayActivo)
        {
            Uint32 tiempoTranscurrido = SDL_GetTicks() - estadoCheat->tiempoInicioXray;
            if ((tiempoTranscurrido / 200) % 2 == 0) // Parpadea cada 200ms
            {
                SDL_Surface *superficieSombraXray = TTF_RenderText_Solid(fuente, "X-RAY ACTIVO", sombra);
                SDL_Surface *superficieXray = TTF_RenderText_Solid(fuente, "X-RAY ACTIVO", amarillo);
                if (superficieXray != NULL && superficieSombraXray != NULL)
                {
                    SDL_Texture *texturaXray = SDL_CreateTextureFromSurface(renderer, superficieXray);
                    SDL_Texture *texturaSombraXray = SDL_CreateTextureFromSurface(renderer, superficieSombraXray);
                    int anchoXray = superficieXray->w;
                    int altoXray = superficieXray->h;
                    int xXray = anchoVentana - anchoXray - 10;  // 10px del borde derecho
                    int yXray = 10;                             // Misma altura que el titulo
                    SDL_Rect rectXray = {xXray, yXray, anchoXray, altoXray};
                    SDL_Rect rectSombraXray = {xXray + 1, yXray + 1, anchoXray, altoXray};
                    SDL_RenderCopy(renderer, texturaSombraXray, NULL, &rectSombraXray);
                    SDL_RenderCopy(renderer, texturaXray, NULL, &rectXray);
                    SDL_DestroyTexture(texturaXray);
                    SDL_DestroyTexture(texturaSombraXray);
                    SDL_FreeSurface(superficieXray);
                    SDL_FreeSurface(superficieSombraXray);
                }
            }
        }
    }
}

/**
 * Muestra el mensaje "GAME OVER" centrado en la pantalla
 * @param renderizador - Puntero al renderizador SDL
 * @param fuente - Fuente para renderizar el texto
 * @param dimensiones - Dimensiones del tablero para centrar el mensaje
 */
void mostrarGameOver(SDL_Renderer* renderizador, TTF_Font* fuente, int dimensiones)
{
    const char* texto = "GAME OVER";

    if (!fuente) {
        printf("No hay fuente disponible para mostrar GAME OVER.\n");
        return;
    }

    SDL_Color blanco = {255, 255, 255, 255};
    SDL_Color sombra = {0, 0, 0, 255};

    SDL_Surface* sombraSurf = TTF_RenderText_Solid(fuente, texto, sombra);
    SDL_Surface* textoSurf = TTF_RenderText_Solid(fuente, texto, blanco);

    if (!sombraSurf || !textoSurf) {
        printf("Error renderizando texto: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* sombraTex = SDL_CreateTextureFromSurface(renderizador, sombraSurf);
    SDL_Texture* textoTex = SDL_CreateTextureFromSurface(renderizador, textoSurf);

    int w = textoSurf->w;
    int h = textoSurf->h;

    SDL_Rect rectTexto = {
        (dimensiones * PIXEL_CELDA - w) / 2,
        (dimensiones * PIXEL_CELDA + ALTURA_HEADER - h) / 2,
        w, h
    };

    SDL_Rect rectSombra = rectTexto;
    rectSombra.x += 2;
    rectSombra.y += 2;

    SDL_RenderCopy(renderizador, sombraTex, NULL, &rectSombra);
    SDL_RenderCopy(renderizador, textoTex, NULL, &rectTexto);

    SDL_FreeSurface(sombraSurf);
    SDL_FreeSurface(textoSurf);
    SDL_DestroyTexture(sombraTex);
    SDL_DestroyTexture(textoTex);
}

/**
 * Verifica si el jugador ha ganado el juego
 * @param matriz - Matriz de celdas del tablero
 * @param dimensiones - Dimensiones del tablero
 * @param totalMinas - Numero total de minas en el tablero
 * @return 1 si gano, 0 si no
 */
int verificarVictoria(sCelda** matriz, int dimensiones, int totalMinas)
{
    int reveladas = 0;

    // Ciclo para contar todas las celdas reveladas
    for (int r = 0; r < dimensiones; r++) {
        for (int c = 0; c < dimensiones; c++) {
            if ((*(matriz + r) + c)->esRevelada)
                reveladas++;
        }
    }

    int totalCeldas = dimensiones * dimensiones;
    return (reveladas == totalCeldas - totalMinas);
}

/**
 * Muestra el mensaje de victoria centrado en la pantalla
 * @param renderizador - Puntero al renderizador SDL
 * @param fuente - Fuente para renderizar el texto
 * @param dimensiones - Dimensiones del tablero para centrar el mensaje
 */
void mostrarVictoria(SDL_Renderer* renderizador, TTF_Font* fuente, int dimensiones) {
    const char* linea1 = "!FELICIDADES!";
    const char* linea2 = "GANASTE";

    if (!fuente) {
        printf("No hay fuente disponible para mostrar VICTORIA.\n");
        return;
    }

    SDL_Color blanco = {255, 255, 255, 255};
    SDL_Color sombra = {0, 0, 0, 255};

    // Renderizar sombras
    SDL_Surface* sombra1 = TTF_RenderText_Solid(fuente, linea1, sombra);
    SDL_Surface* sombra2 = TTF_RenderText_Solid(fuente, linea2, sombra);

    // Renderizar texto blanco
    SDL_Surface* texto1 = TTF_RenderText_Solid(fuente, linea1, blanco);
    SDL_Surface* texto2 = TTF_RenderText_Solid(fuente, linea2, blanco);

    if (!sombra1 || !sombra2 || !texto1 || !texto2) {
        printf("Error al renderizar texto VICTORIA: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* texSombra1 = SDL_CreateTextureFromSurface(renderizador, sombra1);
    SDL_Texture* texSombra2 = SDL_CreateTextureFromSurface(renderizador, sombra2);
    SDL_Texture* texTexto1 = SDL_CreateTextureFromSurface(renderizador, texto1);
    SDL_Texture* texTexto2 = SDL_CreateTextureFromSurface(renderizador, texto2);

    int w1 = texto1->w, h1 = texto1->h;
    int w2 = texto2->w, h2 = texto2->h;

    // Calcula coordenadas centradas
    int cx = (dimensiones * PIXEL_CELDA) / 2;
    int cy = (dimensiones * PIXEL_CELDA + ALTURA_HEADER) / 2;

    SDL_Rect r1 = {cx - w1 / 2, cy - h1 - 5, w1, h1};
    SDL_Rect r2 = {cx - w2 / 2, cy + 5, w2, h2};
    SDL_Rect s1 = r1; s1.x += 2; s1.y += 2;
    SDL_Rect s2 = r2; s2.x += 2; s2.y += 2;

    // Dibujar sombras
    SDL_RenderCopy(renderizador, texSombra1, NULL, &s1);
    SDL_RenderCopy(renderizador, texSombra2, NULL, &s2);

    // Dibujar texto encima
    SDL_RenderCopy(renderizador, texTexto1, NULL, &r1);
    SDL_RenderCopy(renderizador, texTexto2, NULL, &r2);

    // Liberar recursos
    SDL_FreeSurface(sombra1);
    SDL_FreeSurface(sombra2);
    SDL_FreeSurface(texto1);
    SDL_FreeSurface(texto2);
    SDL_DestroyTexture(texSombra1);
    SDL_DestroyTexture(texSombra2);
    SDL_DestroyTexture(texTexto1);
    SDL_DestroyTexture(texTexto2);
}
