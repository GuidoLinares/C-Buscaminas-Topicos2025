#include "headers.h"

void dibujarTablero(SDL_Renderer *renderizador, int dimensiones)
{
    SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255);

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

    sCelda** punteroFila = matriz;
    sCelda** punteroFilaFin = matriz + dimensiones;

    for (int r = 0; punteroFila < punteroFilaFin; punteroFila++, r++)
    {
        sCelda* punteroColumna = *punteroFila;
        sCelda* punteroColumnaFin = *punteroFila + dimensiones;

        for (int c = 0; punteroColumna < punteroColumnaFin; punteroColumna++, c++)
        {
            SDL_Rect rectCelda;
            rectCelda.x = c * PIXEL_CELDA;
            rectCelda.y = r * PIXEL_CELDA + ALTURA_HEADER; // ← AGREGAR ALTURA_HEADER
            rectCelda.w = PIXEL_CELDA;
            rectCelda.h = PIXEL_CELDA;

            if (!(punteroColumna->esRevelada)) // Celda no revelada (oculta)
            {
                // Fondo gris para celda no revelada
                SDL_SetRenderDrawColor(renderizador, 100, 100, 100, 255);
                SDL_RenderFillRect(renderizador, &rectCelda);

                if (punteroColumna->tieneBandera)
                {
                    int cx = rectCelda.x + PIXEL_CELDA / 2;
                    int cy = rectCelda.y + PIXEL_CELDA / 2;

                    // Mástil (línea vertical marrón)
                    SDL_SetRenderDrawColor(renderizador, 139, 69, 19, 255); // Marrón
                    SDL_RenderDrawLine(renderizador, cx, cy - 6, cx, cy + 6);

                    // Bandera (triángulo rojo a la izquierda del mástil)
                    SDL_SetRenderDrawColor(renderizador, 255, 0, 0, 255); // Rojo
                    // Dibuja un triángulo sólido o un contorno
                    // Para un triángulo sólido, puedes usar SDL_RenderGeometry si SDL2 lo soporta,
                    // o dibujar varias líneas. Aquí simulo un triángulo simple con líneas.
                    SDL_RenderDrawLine(renderizador, cx, cy - 6, cx - 6, cy - 3);
                    SDL_RenderDrawLine(renderizador, cx - 6, cy - 3, cx, cy);
                    SDL_RenderDrawLine(renderizador, cx, cy, cx, cy - 6);
                }
            }
            else // Celda revelada
            {
                if (punteroColumna->tieneMina)
                {
                    // Fondo negro para celda con mina
                    SDL_SetRenderDrawColor(renderizador, 0, 0, 0, 255);
                    SDL_RenderFillRect(renderizador, &rectCelda);

                    int cx = rectCelda.x + PIXEL_CELDA / 2;
                    int cy = rectCelda.y + PIXEL_CELDA / 2;
                    int brazo = 5; // longitud de cada brazo del asterisco

                    // Asterisco rojo en forma de bomba
                    SDL_SetRenderDrawColor(renderizador, 255, 0, 0, 255); // Rojo

                    // Línea vertical
                    SDL_RenderDrawLine(renderizador, cx, cy - brazo, cx, cy + brazo);
                    // Línea horizontal
                    SDL_RenderDrawLine(renderizador, cx - brazo, cy, cx + brazo, cy);
                    // Diagonal 1
                    SDL_RenderDrawLine(renderizador, cx - brazo, cy - brazo, cx + brazo, cy + brazo);
                    // Diagonal 2
                    SDL_RenderDrawLine(renderizador, cx - brazo, cy + brazo, cx + brazo, cy - brazo);

                    // Mecha blanca hacia arriba
                    SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255); // Blanco
                    SDL_RenderDrawLine(renderizador, cx, cy - brazo - 1, cx, cy - brazo - 6);

                    // Chispa amarilla
                    SDL_SetRenderDrawColor(renderizador, 255, 255, 0, 255); // Amarillo
                    SDL_RenderDrawPoint(renderizador, cx, cy - brazo - 7);
                    SDL_RenderDrawPoint(renderizador, cx - 1, cy - brazo - 6);
                    SDL_RenderDrawPoint(renderizador, cx + 1, cy - brazo - 6);
                }
                else // Celda revelada y sin mina
                {
                    // Fondo de color según la cantidad de minas adyacentes
                    switch (punteroColumna->minasAdyacentes)
                    {
                        case 0:
                            SDL_SetRenderDrawColor(renderizador, 190, 225, 245, 255); // Espuma marina
                            break;
                        case 1:
                            SDL_SetRenderDrawColor(renderizador, 150, 195, 220, 255); // Azul cielo
                            break;
                        case 2:
                            SDL_SetRenderDrawColor(renderizador, 115, 165, 195, 255); // Azul claro
                            break;
                        case 3:
                            SDL_SetRenderDrawColor(renderizador, 85, 135, 170, 255);  // Azul medio
                            break;
                        case 4:
                            SDL_SetRenderDrawColor(renderizador, 60, 105, 140, 255);  // Azul profundo
                            break;
                        case 5:
                            SDL_SetRenderDrawColor(renderizador, 40, 75, 110, 255);   // Azul océano
                            break;
                        case 6:
                            SDL_SetRenderDrawColor(renderizador, 25, 50, 80, 255);   // Azul marino
                            break;
                        case 7:
                            SDL_SetRenderDrawColor(renderizador, 15, 30, 55, 255);   // Azul abisal
                            break;
                        case 8:
                            SDL_SetRenderDrawColor(renderizador, 8, 15, 35, 255);    // Abismo profundo
                            break;
                        default: // Para cualquier otro valor (seguridad)
                            SDL_SetRenderDrawColor(renderizador, 115, 165, 195, 255);
                            break;
                    }
                    SDL_RenderFillRect(renderizador, &rectCelda);


                    if (punteroColumna->minasAdyacentes > 0 && fuente != NULL)
                    {
                        char texto[12]; // Suficiente para números de hasta 8 dígitos + null terminator
                        sprintf(texto,"%d", punteroColumna->minasAdyacentes);
                        SDL_Color colorTexto = {255, 255, 255}; // Color del texto (blanco)

                        // Renderiza el texto a una superficie
                        SDL_Surface* superficieTexto = TTF_RenderText_Solid(fuente, texto, colorTexto);
                        if (superficieTexto == NULL) {
                            fprintf(stderr, "Error al crear superficie de texto: %s\n", TTF_GetError());
                            // Continúa para evitar un crash, pero el número no se dibujará
                        } else {
                            // Crea una textura a partir de la superficie
                            SDL_Texture* texturaTexto = SDL_CreateTextureFromSurface(renderizador, superficieTexto);
                            if (texturaTexto == NULL) {
                                fprintf(stderr, "Error al crear textura de texto: %s\n", SDL_GetError());
                                SDL_FreeSurface(superficieTexto); // Libera la superficie si la textura falla
                                // Continúa para evitar un crash
                            } else {
                                SDL_Rect destinoTexto;
                                destinoTexto.w = superficieTexto->w;
                                destinoTexto.h = superficieTexto->h;
                                destinoTexto.x = rectCelda.x + (PIXEL_CELDA - destinoTexto.w) / 2; // Centra horizontalmente
                                destinoTexto.y = rectCelda.y + (PIXEL_CELDA - destinoTexto.h) / 2; // Centra verticalmente

                                // Copia la textura a la posición deseada en el renderizador
                                SDL_RenderCopy(renderizador, texturaTexto, NULL, &destinoTexto);

                                // Libera los recursos de la superficie y la textura
                                SDL_FreeSurface(superficieTexto);
                                SDL_DestroyTexture(texturaTexto);
                            }
                        }
                    }
                }
            }
        }
    }
}



void dibujarHeader(SDL_Renderer *renderer, TTF_Font *fuente, int minasRestantes, int anchoVentana)
{
    // Dibujar fondo del header (azul oscuro)
    SDL_SetRenderDrawColor(renderer, 30, 42, 75, 255);
    SDL_Rect rectHeader = {0, 0, anchoVentana, ALTURA_HEADER};
    SDL_RenderFillRect(renderer, &rectHeader);

    if (fuente != NULL)
    {
        SDL_Color sombra = {0, 0, 0, 255};                // Negro
        SDL_Color colorTexto = {173, 216, 230, 255};      // Celeste claro

        // ========== DIBUJAR TÍTULO "BUSCAMINAS PIXEL" CON SOMBRA ==========
        SDL_Surface *superficieSombraTitulo = TTF_RenderText_Solid(fuente, "BUSCAMINAS PIXEL", sombra);
        SDL_Surface *superficieTitulo = TTF_RenderText_Solid(fuente, "BUSCAMINAS PIXEL", colorTexto);

        if (superficieTitulo != NULL && superficieSombraTitulo != NULL)
        {
            SDL_Texture *texturaTitulo = SDL_CreateTextureFromSurface(renderer, superficieTitulo);
            SDL_Texture *texturaSombraTitulo = SDL_CreateTextureFromSurface(renderer, superficieSombraTitulo);

            int anchoTitulo = superficieTitulo->w;
            int altoTitulo = superficieTitulo->h;
            int xTitulo = (anchoVentana - anchoTitulo) / 2;
            int yTitulo = 10;

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
    }
}

