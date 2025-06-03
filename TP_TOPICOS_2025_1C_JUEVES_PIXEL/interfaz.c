#include "headers.h"

void dibujarTablero(SDL_Renderer *renderizador, int dimensiones)
{
    SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255);

    for (int i = 0; i <= dimensiones; i++)
    {
        SDL_RenderDrawLine(renderizador, i * PIXEL_CELDA, 0, i * PIXEL_CELDA, dimensiones * PIXEL_CELDA);
        SDL_RenderDrawLine(renderizador, 0, i * PIXEL_CELDA, dimensiones * PIXEL_CELDA, i * PIXEL_CELDA);
    }
}

void dibujarCeldas(SDL_Renderer *renderizador, s_celdas** matriz, int dimensiones, TTF_Font* fuente)
{
    s_celdas** punteroFila = matriz;
    s_celdas** punteroFilaFin = matriz + dimensiones;

    for (int r = 0; punteroFila < punteroFilaFin; punteroFila++, r++)
    {
        s_celdas* punteroColumna = *punteroFila;
        s_celdas* punteroColumnaFin = *punteroFila + dimensiones;

        for (int c = 0; punteroColumna < punteroColumnaFin; punteroColumna++, c++)
        {
            SDL_Rect rectCelda;
            rectCelda.x = c * PIXEL_CELDA;
            rectCelda.y = r * PIXEL_CELDA;
            rectCelda.w = PIXEL_CELDA;
            rectCelda.h = PIXEL_CELDA;

            if (!(punteroColumna->esRevelada))
            {
                if (punteroColumna->tieneBandera)
                    SDL_SetRenderDrawColor(renderizador, 0, 0, 0, 255); // Negro para banderas
                else
                    SDL_SetRenderDrawColor(renderizador, 100, 100, 100, 255); // Gris para celdas no reveladas

                SDL_RenderFillRect(renderizador, &rectCelda);
            }
            else
            {
                if (punteroColumna->tieneMina)
                {
                    SDL_SetRenderDrawColor(renderizador, 180, 50, 60, 255); // Rojo para minas
                    SDL_RenderFillRect(renderizador, &rectCelda);
                }
                else
                {
                    // Nuevos colores basados en cantidad de minas adyacentes
                    switch (punteroColumna->minasAdyacentes)
                    {
                        case 0:
                            SDL_SetRenderDrawColor(renderizador, 175, 215, 235, 255); // Azul muy claro
                            break;
                        case 1:
                            SDL_SetRenderDrawColor(renderizador, 145, 195, 220, 255); // Azul suave
                            break;
                        case 2:
                            SDL_SetRenderDrawColor(renderizador, 115, 170, 200, 255); // Azul celeste
                            break;
                        case 3:
                            SDL_SetRenderDrawColor(renderizador, 90, 145, 180, 255);  // Azul claro
                            break;
                        case 4:
                            SDL_SetRenderDrawColor(renderizador, 70, 120, 160, 255);  // Azul brillante
                            break;
                        case 5:
                            SDL_SetRenderDrawColor(renderizador, 55, 95, 135, 255);   // Azul mediterráneo
                            break;
                        case 6:
                            SDL_SetRenderDrawColor(renderizador, 45, 75, 110, 255);   // Azul marino
                            break;
                        case 7:
                            SDL_SetRenderDrawColor(renderizador, 35, 55, 85, 255);    // Azul oscuro
                            break;
                        case 8:
                            SDL_SetRenderDrawColor(renderizador, 25, 35, 55, 255);    // Azul noche
                            break;
                        default:
                            SDL_SetRenderDrawColor(renderizador, 115, 170, 200, 255);
                            break;
                    }
                    SDL_RenderFillRect(renderizador, &rectCelda);
                }

                // Solo dibuja números si la celda está REVELADA
                if (punteroColumna->minasAdyacentes > 0 && fuente != NULL && !punteroColumna->tieneMina)
                {
                    char texto[12];
                    sprintf(texto,"%d", punteroColumna->minasAdyacentes);
                    SDL_Color colorTexto = {255, 255, 255}; // Cambié a negro para mejor contraste
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
