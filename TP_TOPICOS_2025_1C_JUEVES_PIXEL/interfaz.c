#include "headers.h"

void dibujarTablero(SDL_Renderer *renderizador, int dimensiones)
{
    SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255);

    for (int i = 0; i <= dimensiones; i++) {
        SDL_RenderDrawLine(renderizador, i * PIXEL_CELDA, 0, i * PIXEL_CELDA, dimensiones * PIXEL_CELDA);
        SDL_RenderDrawLine(renderizador, 0, i * PIXEL_CELDA, dimensiones * PIXEL_CELDA, i * PIXEL_CELDA);
    }
}

void dibujarCeldas(SDL_Renderer *renderizador, s_celdas** matrizLogica, int dimensiones)
{
    s_celdas** punteroFila = matrizLogica;
    s_celdas** punteroFilaFin = matrizLogica + dimensiones;

    for (int r = 0; punteroFila < punteroFilaFin; punteroFila++, r++) {
        s_celdas* punteroColumna = *punteroFila;
        s_celdas* punteroColumnaFin = *punteroFila + dimensiones;

        for (int c = 0; punteroColumna < punteroColumnaFin; punteroColumna++, c++) {
            SDL_Rect rectCelda;
            rectCelda.x = c * PIXEL_CELDA;
            rectCelda.y = r * PIXEL_CELDA;
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

                // CORRECCIÓN: Solo dibuja números si la celda está REVELADA
                if (punteroColumna->minasAdyacentes > 0 && fuente != NULL && !punteroColumna->tieneMina)
                {
                    char texto[12];
                    sprintf(texto,"%d", punteroColumna->minasAdyacentes);
                    SDL_Color colorTexto = {255, 255, 255};
                    SDL_Surface* superficieTexto = TTF_RenderText_Solid(fuente, texto, colorTexto);
                    SDL_Texture* texturaTexto = SDL_CreateTextureFromSurface(renderizador, superficieTexto);

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

            SDL_SetRenderDrawColor(renderizador, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderizador, &rectCelda);
        }
    }
}
