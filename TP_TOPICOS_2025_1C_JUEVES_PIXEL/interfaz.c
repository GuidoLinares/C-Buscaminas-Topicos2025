#include "headers.h"

void dibujarTablero(SDL_Renderer *renderizador, int dimensiones)
{
    SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255);

<<<<<<< Updated upstream
<<<<<<< Updated upstream
    for (int i = 0; i <= dimensiones; i++) {
        SDL_RenderDrawLine(renderizador, i * PIXEL_CELDA, 0, i * PIXEL_CELDA, dimensiones * PIXEL_CELDA);
        SDL_RenderDrawLine(renderizador, 0, i * PIXEL_CELDA, dimensiones * PIXEL_CELDA, i * PIXEL_CELDA);
    }
}

void dibujarCeldas(SDL_Renderer *renderizador, s_celdas** matrizLogica, int dimensiones)
=======
    for (int i = 0; i <= dimensiones; i++)
    {
        // Dibujar líneas verticales
        SDL_RenderDrawLine(renderizador, i * PIXEL_CELDA, ALTURA_HEADER, i * PIXEL_CELDA, dimensiones * PIXEL_CELDA + ALTURA_HEADER);
        // Dibujar líneas horizontales
        SDL_RenderDrawLine(renderizador, 0, i * PIXEL_CELDA + ALTURA_HEADER, dimensiones * PIXEL_CELDA, i * PIXEL_CELDA + ALTURA_HEADER);
    }
}

void dibujarCeldas(SDL_Renderer* renderizador, sCelda** matriz, int dimensiones, TTF_Font* fuente)
>>>>>>> Stashed changes
{
    s_celdas** punteroFila = matrizLogica;
    s_celdas** punteroFilaFin = matrizLogica + dimensiones;

<<<<<<< Updated upstream
    for (int r = 0; punteroFila < punteroFilaFin; punteroFila++, r++) {
        s_celdas* punteroColumna = *punteroFila;
        s_celdas* punteroColumnaFin = *punteroFila + dimensiones;
=======
=======
    for (int i = 0; i <= dimensiones; i++)
    {
        // Dibujar líneas verticales
        SDL_RenderDrawLine(renderizador, i * PIXEL_CELDA, ALTURA_HEADER, i * PIXEL_CELDA, dimensiones * PIXEL_CELDA + ALTURA_HEADER);
        // Dibujar líneas horizontales
        SDL_RenderDrawLine(renderizador, 0, i * PIXEL_CELDA + ALTURA_HEADER, dimensiones * PIXEL_CELDA, i * PIXEL_CELDA + ALTURA_HEADER);
    }
}

void dibujarCeldas(SDL_Renderer* renderizador, sCelda** matriz, int dimensiones, TTF_Font* fuente)
{

>>>>>>> Stashed changes
    sCelda** punteroFila = matriz;
    sCelda** punteroFilaFin = matriz + dimensiones;

    for (int r = 0; punteroFila < punteroFilaFin; punteroFila++, r++)
    {
        sCelda* punteroColumna = *punteroFila;
        sCelda* punteroColumnaFin = *punteroFila + dimensiones;
<<<<<<< Updated upstream
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes

        for (int c = 0; punteroColumna < punteroColumnaFin; punteroColumna++, c++) {
            SDL_Rect rectCelda;
            rectCelda.x = c * PIXEL_CELDA;
            rectCelda.y = r * PIXEL_CELDA + ALTURA_HEADER; // ← AGREGAR ALTURA_HEADER
            rectCelda.w = PIXEL_CELDA;
            rectCelda.h = PIXEL_CELDA;

            if (!(punteroColumna->esRevelada)) {
                if (punteroColumna->tieneBandera) {
                    SDL_SetRenderDrawColor(renderizador, 255, 165, 0, 255);
                    SDL_RenderFillRect(renderizador, &rectCelda);
                } else {
                    SDL_SetRenderDrawColor(renderizador, 100, 100, 100, 255);
                    SDL_RenderFillRect(renderizador, &rectCelda);
                }
            } else {
                if (punteroColumna->tieneMina) {
                    SDL_SetRenderDrawColor(renderizador, 255, 0, 0, 255);
                    SDL_RenderFillRect(renderizador, &rectCelda);
                } else {
                    switch (punteroColumna->minasAdyacentes) { //COLOR DEPENDIENDO CANT DE MINAS ADYACENTES
                        case 0:
                            SDL_SetRenderDrawColor(renderizador, 200, 200, 200, 255);
                            break;
                        case 1:
                            SDL_SetRenderDrawColor(renderizador, 0, 0, 200, 255);
                            break;
                        case 2:
                            SDL_SetRenderDrawColor(renderizador, 0, 150, 0, 255);
                            break;
                        case 3:
                            SDL_SetRenderDrawColor(renderizador, 200, 0, 0, 255);
                            break;
                        case 4:
                            SDL_SetRenderDrawColor(renderizador, 0, 0, 100, 255);
                            break;
                        case 5:
                            SDL_SetRenderDrawColor(renderizador, 150, 0, 0, 255);
                            break;
                        case 6:
                            SDL_SetRenderDrawColor(renderizador, 0, 100, 100, 255);
                            break;
                        case 7:
                            SDL_SetRenderDrawColor(renderizador, 50, 50, 50, 255);
                            break;
                        case 8:
                            SDL_SetRenderDrawColor(renderizador, 100, 0, 100, 255);
                            break;
                        default:
                            SDL_SetRenderDrawColor(renderizador, 255, 255, 0, 255);
                            break;
                    }
                    SDL_RenderFillRect(renderizador, &rectCelda);
                }
            }
            SDL_SetRenderDrawColor(renderizador, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderizador, &rectCelda);
        }
    }
}

void dibujarHeader(SDL_Renderer *renderer, TTF_Font *fuente, int minasRestantes, int anchoVentana)
{
    // Dibujar fondo del header (gris oscuro)
    SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
    SDL_Rect rectHeader = {0, 0, anchoVentana, ALTURA_HEADER};
    SDL_RenderFillRect(renderer, &rectHeader);

    if (fuente != NULL)
    {
        // Color del texto (blanco)
        SDL_Color colorTexto = {255, 255, 255, 255};

        // ========== DIBUJAR TÍTULO "BUSCAMINAS PIXEL" ==========
        SDL_Surface *superficieTitulo = TTF_RenderText_Solid(fuente, "BUSCAMINAS PIXEL", colorTexto);
        if (superficieTitulo != NULL)
        {
            SDL_Texture *texturaTitulo = SDL_CreateTextureFromSurface(renderer, superficieTitulo);
            if (texturaTitulo != NULL)
            {
                // Posicionar el título en la parte superior del header
                int anchoTitulo = superficieTitulo->w;
                int altoTitulo = superficieTitulo->h;
                int xTitulo = (anchoVentana - anchoTitulo) / 2;
                int yTitulo = 10; // Margen superior

                SDL_Rect rectTitulo = {xTitulo, yTitulo, anchoTitulo, altoTitulo};
                SDL_RenderCopy(renderer, texturaTitulo, NULL, &rectTitulo);
                SDL_DestroyTexture(texturaTitulo);
            }
            SDL_FreeSurface(superficieTitulo);
        }

        // ========== DIBUJAR CONTADOR DE MINAS ==========
        char texto[50];
        snprintf(texto, sizeof(texto), "Minas restantes: %d", minasRestantes);

        SDL_Surface *superficieTexto = TTF_RenderText_Solid(fuente, texto, colorTexto);
        if (superficieTexto != NULL)
        {
            SDL_Texture *texturaTexto = SDL_CreateTextureFromSurface(renderer, superficieTexto);
            if (texturaTexto != NULL)
            {
                // Posicionar el contador debajo del título
                int anchoTexto = superficieTexto->w;
                int altoTexto = superficieTexto->h;
                int xTexto = (anchoVentana - anchoTexto) / 2;
                int yTexto = ALTURA_HEADER - altoTexto - 10; // Margen inferior

                SDL_Rect rectTexto = {xTexto, yTexto, anchoTexto, altoTexto};
                SDL_RenderCopy(renderer, texturaTexto, NULL, &rectTexto);
                SDL_DestroyTexture(texturaTexto);
            }
            SDL_FreeSurface(superficieTexto);
        }
    }
}

void dibujarHeader(SDL_Renderer *renderer, TTF_Font *fuente, int minasRestantes, int anchoVentana)
{
    // Dibujar fondo del header (gris oscuro)
    SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
    SDL_Rect rectHeader = {0, 0, anchoVentana, ALTURA_HEADER};
    SDL_RenderFillRect(renderer, &rectHeader);

    if (fuente != NULL)
    {
        // Color del texto (blanco)
        SDL_Color colorTexto = {255, 255, 255, 255};

        // ========== DIBUJAR TÍTULO "BUSCAMINAS PIXEL" ==========
        SDL_Surface *superficieTitulo = TTF_RenderText_Solid(fuente, "BUSCAMINAS PIXEL", colorTexto);
        if (superficieTitulo != NULL)
        {
            SDL_Texture *texturaTitulo = SDL_CreateTextureFromSurface(renderer, superficieTitulo);
            if (texturaTitulo != NULL)
            {
                // Posicionar el título en la parte superior del header
                int anchoTitulo = superficieTitulo->w;
                int altoTitulo = superficieTitulo->h;
                int xTitulo = (anchoVentana - anchoTitulo) / 2;
                int yTitulo = 10; // Margen superior

                SDL_Rect rectTitulo = {xTitulo, yTitulo, anchoTitulo, altoTitulo};
                SDL_RenderCopy(renderer, texturaTitulo, NULL, &rectTitulo);
                SDL_DestroyTexture(texturaTitulo);
            }
            SDL_FreeSurface(superficieTitulo);
        }

        // ========== DIBUJAR CONTADOR DE MINAS ==========
        char texto[50];
        snprintf(texto, sizeof(texto), "Minas restantes: %d", minasRestantes);

        SDL_Surface *superficieTexto = TTF_RenderText_Solid(fuente, texto, colorTexto);
        if (superficieTexto != NULL)
        {
            SDL_Texture *texturaTexto = SDL_CreateTextureFromSurface(renderer, superficieTexto);
            if (texturaTexto != NULL)
            {
                // Posicionar el contador debajo del título
                int anchoTexto = superficieTexto->w;
                int altoTexto = superficieTexto->h;
                int xTexto = (anchoVentana - anchoTexto) / 2;
                int yTexto = ALTURA_HEADER - altoTexto - 10; // Margen inferior

                SDL_Rect rectTexto = {xTexto, yTexto, anchoTexto, altoTexto};
                SDL_RenderCopy(renderer, texturaTexto, NULL, &rectTexto);
                SDL_DestroyTexture(texturaTexto);
            }
            SDL_FreeSurface(superficieTexto);
        }
    }
}
