#include "common.h"
#include "menu.h"
#include "usuario.h"
#include "juego.h"


void renderizarTextoConSombra(SDL_Renderer* renderer, TTF_Font* fuente, const char* texto, int x, int y, SDL_Color colorTexto, SDL_Color colorSombra)
{
    if (!fuente || !texto || !renderer)
        return;

    SDL_Surface* superficieSombra = TTF_RenderText_Solid(fuente, texto, colorSombra);
    SDL_Surface* superficieTexto = TTF_RenderText_Solid(fuente, texto, colorTexto);

    if (!superficieSombra || !superficieTexto)
    {
        if (superficieSombra)
            SDL_FreeSurface(superficieSombra);
        if (superficieTexto)
            SDL_FreeSurface(superficieTexto);
        return;
    }

    SDL_Texture* texturaSombra = SDL_CreateTextureFromSurface(renderer, superficieSombra);
    SDL_Texture* texturaTexto = SDL_CreateTextureFromSurface(renderer, superficieTexto);

    if (texturaSombra && texturaTexto)
    {
        SDL_Rect rectSombra = {x + 2, y + 2, superficieTexto->w, superficieTexto->h};
        SDL_Rect rectTexto = {x, y, superficieTexto->w, superficieTexto->h};

        SDL_RenderCopy(renderer, texturaSombra, NULL, &rectSombra);
        SDL_RenderCopy(renderer, texturaTexto, NULL, &rectTexto);
    }

    SDL_FreeSurface(superficieSombra);
    SDL_FreeSurface(superficieTexto);
    if (texturaSombra) SDL_DestroyTexture(texturaSombra);
    if (texturaTexto) SDL_DestroyTexture(texturaTexto);
}

int pantallaIngreso(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, Usuario* usuario)
{
    SDL_SetWindowSize(ventana, 600, 600);
    SDL_SetWindowPosition(ventana, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    char nombreBuffer[50] = "";
    int posicionCursor = 0;
    int activo = 1;

    SDL_Color blanco = {255, 255, 255, 255};
    SDL_Color gris = {128, 128, 128, 255};
    SDL_Color negro = {0, 0, 0, 255};
    SDL_Color azul = {30, 42, 75, 255};

    SDL_StartTextInput();

    while (activo)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                SDL_StopTextInput();
                return -1;
            }

            if (e.type == SDL_TEXTINPUT)
            {
                if (strlen(nombreBuffer) < 20)
                {
                    strcat(nombreBuffer, e.text.text);
                    posicionCursor = strlen(nombreBuffer);
                }
            }

            if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_BACKSPACE && posicionCursor > 0)
                {
                    nombreBuffer[posicionCursor - 1] = '\0';
                    posicionCursor--;
                }

                if (e.key.keysym.sym == SDLK_RETURN && strlen(nombreBuffer) >= 3)
                {
                    if (validarNombreUsuario(nombreBuffer))
                    {
                        int resultado = cargarUsuario(nombreBuffer, usuario);
                        if (resultado >= 0)
                        {
                            SDL_StopTextInput();
                            return 0; // Éxito
                        }
                    }
                }

                if (e.key.keysym.sym == SDLK_ESCAPE)
                {
                    SDL_StopTextInput();
                    return -1;
                }
            }
        }

        // Renderizado
        SDL_SetRenderDrawColor(renderer, azul.r, azul.g, azul.b, 255);
        SDL_RenderClear(renderer);

        // Título
        if (fuenteGrande)
renderizarTextoCentrado(renderer, fuenteGrande, "BUSCAMINAS PIXEL", 100, blanco, negro);

        // Instrucciones
        if (fuente)
        {
renderizarTextoCentrado(renderer, fuente, "Ingresa tu nombre de usuario:", 200, blanco, negro);
renderizarTextoCentrado(renderer, fuente, "(3-20 caracteres, presiona ENTER)", 220, gris, negro);
        }

        // Campo de entrada (centrado horizontalmente)
        int anchoVentana = 0;
        SDL_GetRendererOutputSize(renderer, &anchoVentana, NULL);
        SDL_Rect rectCampo = {anchoVentana / 2 - 100, 260, 200, 30}; // centrado

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rectCampo);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rectCampo);

        // Texto ingresado (centrado dentro del campo)
        if (strlen(nombreBuffer) > 0 && fuente)
        {
            SDL_Surface* superficieNombre = TTF_RenderText_Solid(fuente, nombreBuffer, negro);
            if (superficieNombre)
            {
                SDL_Texture* texturaNombre = SDL_CreateTextureFromSurface(renderer, superficieNombre);
                if (texturaNombre)
                {
                    int xTexto = rectCampo.x + (rectCampo.w - superficieNombre->w) / 2;
                    SDL_Rect rectNombre = {xTexto, 265, superficieNombre->w, superficieNombre->h};
                    SDL_RenderCopy(renderer, texturaNombre, NULL, &rectNombre);
                    SDL_DestroyTexture(texturaNombre);
                }
                SDL_FreeSurface(superficieNombre);
            }
        }

        // Cursor parpadeante
        static Uint32 tiempoAnterior = 0;
        Uint32 tiempoActual = SDL_GetTicks();
        if (tiempoActual - tiempoAnterior > 500)
            tiempoAnterior = tiempoActual;

        if ((tiempoActual - tiempoAnterior) < 250)
        {
            int anchoTexto = 0;
            if (fuente && strlen(nombreBuffer) > 0)
            {
                int w, h;
                TTF_SizeText(fuente, nombreBuffer, &w, &h);
                anchoTexto = w;
            }
            int xCursor = rectCampo.x + (rectCampo.w - anchoTexto) / 2 + anchoTexto;
            SDL_RenderDrawLine(renderer, xCursor, 265, xCursor, 285);
        }

        // Instrucción inferior
        if (fuente)
renderizarTextoCentrado(renderer, fuente, "ESC para salir", 400, gris, negro);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_StopTextInput();
    return -1;
}

int mostrarMenuSDL(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, Usuario* usuario)
{
    SDL_SetWindowSize(ventana, 700, 700);
    SDL_SetWindowPosition(ventana, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    int opcionSeleccionada = 0;
    const int numOpciones = 5;
    const char* opciones[] = {
        "1. Nueva Partida",
        "2. Ver Estadísticas",
        "3. Cargar Partida",
        "4. Recargar Configuración",
        "5. Salir"
    };

    SDL_Color blanco = {255, 255, 255, 255};
    SDL_Color amarillo = {255, 255, 0, 255};
    SDL_Color negro = {0, 0, 0, 255};
    SDL_Color azul = {30, 42, 75, 255};
    SDL_Color gris = {128, 128, 128, 255};

    while (1)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT) {
                return 5; // Salir
            }

            if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                    case SDLK_UP:
                        opcionSeleccionada = (opcionSeleccionada - 1 + numOpciones) % numOpciones;
                        break;
                    case SDLK_DOWN:
                        opcionSeleccionada = (opcionSeleccionada + 1) % numOpciones;
                        break;
                    case SDLK_RETURN:
                        return opcionSeleccionada + 1;
                    case SDLK_1: return 1;
                    case SDLK_2: return 2;
                    case SDLK_3: return 3;
                    case SDLK_4: return 4;
                    case SDLK_5: return 5;
                    case SDLK_ESCAPE: return 5;
                }
            }
        }

        // Renderizado
        SDL_SetRenderDrawColor(renderer, azul.r, azul.g, azul.b, 255);
        SDL_RenderClear(renderer);

        // Título
        if (fuenteGrande)
        renderizarTextoCentrado(renderer, fuenteGrande, "MENÚ PRINCIPAL", 50, blanco, negro);

        // Información del usuario
        if (fuente)
        {
            char infoUsuario[100];
            snprintf(infoUsuario, sizeof(infoUsuario), "Usuario: %s", usuario->nombre);
            renderizarTextoCentrado(renderer, fuente, infoUsuario, 120, gris, negro);

            snprintf(infoUsuario, sizeof(infoUsuario), "Partidas: %d | Ganadas: %d",
            usuario->estadisticas.partidasJugadas, usuario->estadisticas.partidasGanadas);
            renderizarTextoCentrado(renderer, fuente, infoUsuario, 140, gris, negro);
        }

        // Opciones del menú
        for (int i = 0; i < numOpciones; i++)
        {
            SDL_Color colorOpcion;
            if (i == opcionSeleccionada)
                colorOpcion = amarillo;
            else
                colorOpcion = blanco;

            int y = 200 + i * 40; // Calcula la posición 'y' para la opción

            int anchoOpcionTexto = 0;
            if (fuente)
                TTF_SizeText(fuente, opciones[i], &anchoOpcionTexto, NULL);


            int anchoVentana;
            SDL_GetRendererOutputSize(renderer, &anchoVentana, NULL);
            int xInicioOpcion = (anchoVentana - anchoOpcionTexto) / 2;

            if (fuente)
                renderizarTextoConSombra(renderer, fuente, opciones[i], xInicioOpcion, y, colorOpcion, negro);

            if (i == opcionSeleccionada)
            {
                int anchoFlecha = 0;
                if (fuente)
                    TTF_SizeText(fuente, ">>", &anchoFlecha, NULL);

                renderizarTextoConSombra(renderer, fuente, ">>", xInicioOpcion - anchoFlecha - 15, y, amarillo, negro);
            }
        }

            // Instrucciones
            if (fuente)
                renderizarTextoCentrado(renderer, fuente, "Usa las flechas y ENTER, o presiona el número", 420, gris, negro);

            SDL_RenderPresent(renderer);
            SDL_Delay(16);
    }
}

void mostrarEstadisticasSDL(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, Usuario* usuario)
{
    SDL_Color blanco = {255, 255, 255, 255};
    SDL_Color negro = {0, 0, 0, 255};
    SDL_Color azul = {30, 42, 75, 255};
    SDL_Color gris = {128, 128, 128, 255};

    while (1)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN)
                return; // Regresar al menú
        }

        // Renderizado
        SDL_SetRenderDrawColor(renderer, azul.r, azul.g, azul.b, 255);
        SDL_RenderClear(renderer);

        // Título
        if (fuenteGrande)
            renderizarTextoConSombra(renderer, fuenteGrande, "ESTADÍSTICAS", 200, 50, blanco, negro);

        if (fuente)
        {
            char buffer[100];
            int y = 120;
            int espaciado = 30;

            // Nombre del usuario
            snprintf(buffer, sizeof(buffer), "Usuario: %s", usuario->nombre);
            renderizarTextoConSombra(renderer, fuente, buffer, 100, y, blanco, negro);
            y += espaciado;

            // Estadísticas
            snprintf(buffer, sizeof(buffer), "Partidas jugadas: %d", usuario->estadisticas.partidasJugadas);
            renderizarTextoConSombra(renderer, fuente, buffer, 100, y, blanco, negro);
            y += espaciado;

            snprintf(buffer, sizeof(buffer), "Partidas ganadas: %d", usuario->estadisticas.partidasGanadas);
            renderizarTextoConSombra(renderer, fuente, buffer, 100, y, blanco, negro);
            y += espaciado;

            snprintf(buffer, sizeof(buffer), "Partidas perdidas: %d", usuario->estadisticas.partidasPerdidas);
            renderizarTextoConSombra(renderer, fuente, buffer, 100, y, blanco, negro);
            y += espaciado;

            snprintf(buffer, sizeof(buffer), "Tiempo total: %d segundos", usuario->estadisticas.tiempoTotalJugado);
            renderizarTextoConSombra(renderer, fuente, buffer, 100, y, blanco, negro);
            y += espaciado;

            if (usuario->estadisticas.mejorTiempo > 0)
            {
                snprintf(buffer, sizeof(buffer), "Mejor tiempo: %d segundos", usuario->estadisticas.mejorTiempo);
                renderizarTextoConSombra(renderer, fuente, buffer, 100, y, blanco, negro);
                y += espaciado;
            }

            snprintf(buffer, sizeof(buffer), "Racha actual: %d", usuario->estadisticas.rachaActual);
            renderizarTextoConSombra(renderer, fuente, buffer, 100, y, blanco, negro);
            y += espaciado;

            snprintf(buffer, sizeof(buffer), "Mejor racha: %d", usuario->estadisticas.mejorRacha);
            renderizarTextoConSombra(renderer, fuente, buffer, 100, y, blanco, negro);
            y += espaciado;

            if (usuario->estadisticas.partidasJugadas > 0)
            {
                double porcentaje = calcularPorcentajeVictorias(usuario);
                snprintf(buffer, sizeof(buffer), "Porcentaje de victorias: %.1f%%", porcentaje);
                renderizarTextoConSombra(renderer, fuente, buffer, 100, y, blanco, negro);
                y += espaciado;

                float tiempoPromedio = (float)usuario->estadisticas.tiempoTotalJugado / usuario->estadisticas.partidasJugadas;
                snprintf(buffer, sizeof(buffer), "Tiempo promedio: %.1f segundos", tiempoPromedio);
                renderizarTextoConSombra(renderer, fuente, buffer, 100, y, blanco, negro);
            }

            // Instrucciones
            renderizarTextoConSombra(renderer, fuente, "Presiona cualquier tecla para volver", 250, 400, gris, negro);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

int cargarPartidaSDL(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, Usuario* usuario, Archivo_conf configuracion)
{
    SDL_Color blanco = {255, 255, 255, 255};
    SDL_Color negro = {0, 0, 0, 255};
    SDL_Color azul = {30, 42, 75, 255};

    Uint32 tiempoInicio = SDL_GetTicks();

    while (SDL_GetTicks() - tiempoInicio < 2000)
    { // Mostrar por 2 segundos
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN)
                return -1;
        }

        SDL_SetRenderDrawColor(renderer, azul.r, azul.g, azul.b, 255);
        SDL_RenderClear(renderer);

        if (fuenteGrande)
renderizarTextoCentrado(renderer, fuenteGrande, "FUNCIÓN NO IMPLEMENTADA", 200, blanco, negro);

        if (fuente)
renderizarTextoCentrado(renderer, fuente, "Esta función estará disponible en futuras versiones", 280, blanco, negro);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    return 0;
}

void dibujarTextoCentrado(SDL_Renderer* renderer, TTF_Font* fuente, const char* texto, int x, int y, int ancho, SDL_Color color)
{
    if (!fuente || !texto || !renderer)
        return;

    SDL_Surface* superficie = TTF_RenderText_Solid(fuente, texto, color);
    if (!superficie)
        return;

    SDL_Texture* textura = SDL_CreateTextureFromSurface(renderer, superficie);
    if (textura)
    {
        int centroX = x + (ancho - superficie->w) / 2;
        SDL_Rect rect = {centroX, y, superficie->w, superficie->h};
        SDL_RenderCopy(renderer, textura, NULL, &rect);
        SDL_DestroyTexture(textura);
    }

    SDL_FreeSurface(superficie);
}

void dibujarBotonSDL(SDL_Renderer* renderer, TTF_Font* fuente, const char* texto, SDL_Rect rect, int seleccionado)
{
    if (!renderer || !fuente || !texto)
        return;

    SDL_Color colorFondo = seleccionado ? (SDL_Color){100, 100, 100, 255} : (SDL_Color){50, 50, 50, 255};
    SDL_Color colorTexto = seleccionado ? (SDL_Color){255, 255, 0, 255} : (SDL_Color){255, 255, 255, 255};
    SDL_Color colorBorde = {200, 200, 200, 255};

    // Dibujar fondo del botón
    SDL_SetRenderDrawColor(renderer, colorFondo.r, colorFondo.g, colorFondo.b, colorFondo.a);
    SDL_RenderFillRect(renderer, &rect);

    // Dibujar borde
    SDL_SetRenderDrawColor(renderer, colorBorde.r, colorBorde.g, colorBorde.b, colorBorde.a);
    SDL_RenderDrawRect(renderer, &rect);

    // Dibujar texto centrado
    dibujarTextoCentrado(renderer, fuente, texto, rect.x, rect.y + (rect.h / 4), rect.w, colorTexto);
}

int clicEnRect(int x, int y, SDL_Rect rect)
{
    return (x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h);
}

int manejarEntradaTextoSDL(SDL_Event* evento, char* buffer, int maxLen)
{
    if (!evento || !buffer)
        return 0;

    if (evento->type == SDL_TEXTINPUT)
    {
        if (strlen(buffer) < maxLen - 1)
        {
            strcat(buffer, evento->text.text);
            return 1;
        }
    }
    else if (evento->type == SDL_KEYDOWN)
    {
        if (evento->key.keysym.sym == SDLK_BACKSPACE && strlen(buffer) > 0)
        {
            buffer[strlen(buffer) - 1] = '\0';
            return 1;
        }
    }

    return 0;
}

void renderizarTextoCentrado(SDL_Renderer* renderer, TTF_Font* fuente, const char* texto, int y, SDL_Color colorTexto, SDL_Color colorSombra)
{
    if (!renderer || !fuente || !texto)
        return;

    SDL_Surface* superficieSombra = TTF_RenderText_Solid(fuente, texto, colorSombra);
    SDL_Surface* superficieTexto = TTF_RenderText_Solid(fuente, texto, colorTexto);

    if (!superficieSombra || !superficieTexto)
    {
        if (superficieSombra) SDL_FreeSurface(superficieSombra);
        if (superficieTexto) SDL_FreeSurface(superficieTexto);
        return;
    }

    SDL_Texture* texturaSombra = SDL_CreateTextureFromSurface(renderer, superficieSombra);
    SDL_Texture* texturaTexto = SDL_CreateTextureFromSurface(renderer, superficieTexto);

    int anchoVentana;
    SDL_GetRendererOutputSize(renderer, &anchoVentana, NULL);
    int x = (anchoVentana - superficieTexto->w) / 2;

    SDL_Rect rectSombra = {x + 2, y + 2, superficieTexto->w, superficieTexto->h};
    SDL_Rect rectTexto = {x, y, superficieTexto->w, superficieTexto->h};

    SDL_RenderCopy(renderer, texturaSombra, NULL, &rectSombra);
    SDL_RenderCopy(renderer, texturaTexto, NULL, &rectTexto);

    SDL_FreeSurface(superficieSombra);
    SDL_FreeSurface(superficieTexto);
    SDL_DestroyTexture(texturaSombra);
    SDL_DestroyTexture(texturaTexto);
}





