#include "common.h"
#include "menu.h"
#include "usuario.h"
#include "tablero.h"
#include "juego.h"

/**
 * Función unificada para renderizar texto con opciones de centrado y sombra
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente a utilizar
 * @param texto - Texto a renderizar
 * @param x - Coordenada X (ignorada si centrado=1)
 * @param y - Coordenada Y donde dibujar
 * @param colorTexto - Color del texto principal
 * @param colorSombra - Color de la sombra
 * @param centrado - 1 para centrar horizontalmente, 0 para usar coordenada X
 * @param conSombra - 1 para incluir sombra, 0 para no incluir
 */
void renderizarTexto(SDL_Renderer* renderer, TTF_Font* fuente, const char* texto,int x, int y, SDL_Color colorTexto, SDL_Color colorSombra,int centrado, int conSombra)
{
    if (!fuente || !texto || !renderer) return;

    SDL_Surface* superficie = TTF_RenderText_Solid(fuente, texto, colorTexto);
    if (!superficie) return;

    int xFinal = x;
    if (centrado)
    {
        int anchoVentana;
        SDL_GetRendererOutputSize(renderer, &anchoVentana, NULL);
        xFinal = (anchoVentana - superficie->w) / 2;
    }

    SDL_Texture* textura = SDL_CreateTextureFromSurface(renderer, superficie);
    if (!textura)
    {
        SDL_FreeSurface(superficie);
        return;
    }

    if (conSombra)
    {
        SDL_Surface* sombra = TTF_RenderText_Solid(fuente, texto, colorSombra);
        if (sombra)
        {
            SDL_Texture* texturaSombra = SDL_CreateTextureFromSurface(renderer, sombra);
            if (texturaSombra)
            {
                SDL_Rect rectSombra = {xFinal + 2, y + 2, superficie->w, superficie->h};
                SDL_RenderCopy(renderer, texturaSombra, NULL, &rectSombra);
                SDL_DestroyTexture(texturaSombra);
            }
            SDL_FreeSurface(sombra);
        }
    }

    SDL_Rect rect = {xFinal, y, superficie->w, superficie->h};
    SDL_RenderCopy(renderer, textura, NULL, &rect);

    SDL_FreeSurface(superficie);
    SDL_DestroyTexture(textura);
}

/**
 * Valida si un nombre de usuario cumple con los criterios requeridos
 * @param nombre - Nombre a validar
 * @return 1 si es valido, 0 si no lo es
 */
int validarNombreUsuario(const char *nombre)
{
    if (nombre == NULL)
        return 0;

    size_t longitud = strlen(nombre);

    if (longitud < 3)
        return 0;

    // Ciclo para verificar que todos los caracteres sean alfanumericos o guion bajo
    for (size_t i = 0; i < longitud; i++)
    {
        if (!isalnum(nombre[i]) && nombre[i] != '_')
        {
            return 0;
        }
    }
    return 1;
}

/**
 * Muestra la pantalla de ingreso de usuario y maneja la entrada de texto
 * @param ventana - Ventana SDL
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente pequena
 * @param fuenteGrande - Fuente grande
 * @param usuario - Puntero donde guardar los datos del usuario
 * @return 0 si es exitoso, -1 si hay error o se cancela
 */
int pantallaIngreso(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuario)
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

    // Bucle principal de la pantalla de ingreso
    while (activo)
    {
        SDL_Event e;
        // Ciclo para procesar todos los eventos pendientes
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
                            return 0; // Exito
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

        // Renderizado de la pantalla de ingreso
        SDL_SetRenderDrawColor(renderer, azul.r, azul.g, azul.b, 255);
        SDL_RenderClear(renderer);

        // Titulo
        if (fuenteGrande)
            renderizarTexto(renderer, fuenteGrande, "BUSCAMINAS PIXEL", 0, 100, blanco, negro, 1, 1);

        // Instrucciones
        if (fuente)
        {
            renderizarTexto(renderer, fuente, "Ingresa tu nombre de usuario:", 0, 200, blanco, negro, 1, 1);
            renderizarTexto(renderer, fuente, "(3-20 caracteres, presiona ENTER)", 0, 220, gris, negro, 1, 1);
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

        // Instruccion inferior
        if (fuente)
            renderizarTexto(renderer, fuente, "ESC para salir", 0, 400, gris, negro, 1, 1);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_StopTextInput();
    return -1;
}


/**
 * Muestra el menu principal y maneja la seleccion de opciones (VERSION FINAL)
 * @param ventana - Ventana SDL
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente pequena
 * @param fuenteGrande - Fuente grande
 * @param usuario - Datos del usuario actual
 * @return Numero de la opcion seleccionada (1-5)
 */
int mostrarMenuSDL(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuario)
{
    SDL_SetWindowSize(ventana, 700, 700);
    SDL_SetWindowPosition(ventana, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    int opcionSeleccionada = 0;
    const int numOpciones = 6;
    const char* opciones[] = {
        "1. Nueva Partida",
        "2. Ver Estadisticas",
        "3. Cargar Partida",
        "4. Configurar Juego",
        "5. Instrucciones",
        "6. Salir"
    };

    SDL_Color blanco = {255, 255, 255, 255};
    SDL_Color amarillo = {255, 255, 0, 255};
    SDL_Color negro = {0, 0, 0, 255};
    SDL_Color azul = {30, 42, 75, 255};
    SDL_Color gris = {128, 128, 128, 255};

    // Bucle principal del menu
    while (1)
    {
        SDL_Event e;
        // Ciclo para procesar eventos del menu
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                return 5; // Salir

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
                    case SDLK_6: return 6;
                    case SDLK_ESCAPE: return 6;
                }
            }
        }

        // Renderizado del menu principal
        SDL_SetRenderDrawColor(renderer, azul.r, azul.g, azul.b, 255);
        SDL_RenderClear(renderer);

        // Titulo
        if (fuenteGrande)
            renderizarTexto(renderer, fuenteGrande, "MENU PRINCIPAL", 0, 50, blanco, negro, 1, 1);

        // Informacion del usuario
        if (fuente)
        {
            char infosUsuario[100];
            snprintf(infosUsuario, sizeof(infosUsuario), "Usuario: %s", usuario->nombre);
            renderizarTexto(renderer, fuente, infosUsuario, 0, 120, gris, negro, 1, 1);

            snprintf(infosUsuario, sizeof(infosUsuario), "Partidas: %d | Ganadas: %d",
            usuario->estadisticas.partidasJugadas, usuario->estadisticas.partidasGanadas);
            renderizarTexto(renderer, fuente, infosUsuario, 0, 140, gris, negro, 1, 1);
        }

        // Opciones del menu
        for (int i = 0; i < numOpciones; i++)
        {
            SDL_Color colorOpcion;
            if (i == opcionSeleccionada)
                colorOpcion = amarillo;
            else
                colorOpcion = blanco;

            int y = 200 + i * 40;

            int anchoOpcionTexto = 0;
            if (fuente)
                TTF_SizeText(fuente, opciones[i], &anchoOpcionTexto, NULL);

            int anchoVentana;
            SDL_GetRendererOutputSize(renderer, &anchoVentana, NULL);
            int xInicioOpcion = (anchoVentana - anchoOpcionTexto) / 2;

            if (fuente)
                renderizarTexto(renderer, fuente, opciones[i], xInicioOpcion, y, colorOpcion, negro, 0, 1);

            if (i == opcionSeleccionada)
            {
                int anchoFlecha = 0;
                if (fuente)
                    TTF_SizeText(fuente, ">>", &anchoFlecha, NULL);

                renderizarTexto(renderer, fuente, ">>", xInicioOpcion - anchoFlecha - 15, y, amarillo, negro, 0, 1);
            }
        }

        // Instrucciones
        if (fuente)
            renderizarTexto(renderer, fuente, "Usa las flechas y ENTER, o presiona el numero", 0, 500, gris, negro, 1, 1);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}


/**
 * Muestra las estadisticas del usuario en pantalla completa
 * @param ventana - Ventana SDL
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente pequena
 * @param fuenteGrande - Fuente grande
 * @param usuario - Datos del usuario para mostrar estadisticas
 */
void mostrarEstadisticasSDL(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuario)
{
    SDL_Color blanco = {255, 255, 255, 255};
    SDL_Color negro = {0, 0, 0, 255};
    SDL_Color azul = {30, 42, 75, 255};
    SDL_Color gris = {128, 128, 128, 255};

    // Bucle para mostrar estadisticas hasta que el usuario presione una tecla
    while (1)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN)
                return; // Regresar al menu
        }

        // Renderizado de la pantalla de estadisticas
        SDL_SetRenderDrawColor(renderer, azul.r, azul.g, azul.b, 255);
        SDL_RenderClear(renderer);

        // Titulo
        if (fuenteGrande)
            renderizarTexto(renderer, fuenteGrande, "ESTADISTICAS", 0, 50, blanco, negro, 1, 1);

        if (fuente)
        {
            char buffer[100];
            int y = 120;
            int espaciado = 30;

            // Nombre del usuario
            snprintf(buffer, sizeof(buffer), "Usuario: %s", usuario->nombre);
            renderizarTexto(renderer, fuente, buffer, 0, y, blanco, negro, 1, 1);
            y += espaciado;

            // Estadisticas
            snprintf(buffer, sizeof(buffer), "Partidas jugadas: %d", usuario->estadisticas.partidasJugadas);
            renderizarTexto(renderer, fuente, buffer, 0, y, blanco, negro, 1, 1);
            y += espaciado;

            snprintf(buffer, sizeof(buffer), "Partidas ganadas: %d", usuario->estadisticas.partidasGanadas);
            renderizarTexto(renderer, fuente, buffer, 0, y, blanco, negro, 1, 1);
            y += espaciado;

            snprintf(buffer, sizeof(buffer), "Partidas perdidas: %d", usuario->estadisticas.partidasPerdidas);
            renderizarTexto(renderer, fuente, buffer, 0, y, blanco, negro, 1, 1);
            y += espaciado;

            snprintf(buffer, sizeof(buffer), "Tiempo total: %d segundos", usuario->estadisticas.tiempoTotalJugado);
            renderizarTexto(renderer, fuente, buffer, 0, y, blanco, negro, 1, 1);
            y += espaciado;

            if (usuario->estadisticas.mejorTiempo > 0)
            {
                snprintf(buffer, sizeof(buffer), "Mejor tiempo: %d segundos", usuario->estadisticas.mejorTiempo);
                renderizarTexto(renderer, fuente, buffer, 0, y, blanco, negro, 1, 1);
                y += espaciado;
            }

            snprintf(buffer, sizeof(buffer), "Racha actual: %d", usuario->estadisticas.rachaActual);
            renderizarTexto(renderer, fuente, buffer, 0, y, blanco, negro, 1, 1);
            y += espaciado;

            snprintf(buffer, sizeof(buffer), "Mejor racha: %d", usuario->estadisticas.mejorRacha);
            renderizarTexto(renderer, fuente, buffer, 0, y, blanco, negro, 1, 1);
            y += espaciado;

            if (usuario->estadisticas.partidasJugadas > 0)
            {
                double porcentaje = calcularPorcentajeVictorias(usuario);
                snprintf(buffer, sizeof(buffer), "Porcentaje de victorias: %.1f%%", porcentaje);
                renderizarTexto(renderer, fuente, buffer, 0, y, blanco, negro, 1, 1);
                y += espaciado;

                float tiempoPromedio = (float)usuario->estadisticas.tiempoTotalJugado / usuario->estadisticas.partidasJugadas;
                snprintf(buffer, sizeof(buffer), "Tiempo promedio: %.1f segundos", tiempoPromedio);
                renderizarTexto(renderer, fuente, buffer, 0, y, blanco, negro, 1, 1);
            }

            // Instrucciones
            renderizarTexto(renderer, fuente, "Presiona cualquier tecla para volver", 0, y + espaciado + 20, gris, negro, 1, 1);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

/**
 * Interfaz responsive para cargar partidas guardadas del usuario
 * Se adapta automaticamente al tamano de la ventana
 */
int cargarPartidaSDL(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuario, sArchivo_conf configuracion)
{

    // Obtener dimensiones actuales de la ventana
    int anchoVentana, altoVentana;
    SDL_GetWindowSize(ventana, &anchoVentana, &altoVentana);

    // Configurar ventana con tamano minimo adecuado
    if (anchoVentana < 800) anchoVentana = 800;
    if (altoVentana < 600) altoVentana = 600;
    SDL_SetWindowSize(ventana, anchoVentana, altoVentana);
    SDL_SetWindowPosition(ventana, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    SDL_Color blanco = {255, 255, 255, 255};
    SDL_Color negro = {0, 0, 0, 255};
    SDL_Color azul = {30, 42, 75, 255};
    SDL_Color azulClaro = {40, 60, 100, 255};
    SDL_Color amarillo = {255, 255, 0, 255};
    SDL_Color gris = {128, 128, 128, 255};
    SDL_Color rojo = {255, 100, 100, 255};

    // ===== CREAR LISTA DE PARTIDAS VALIDAS =====
    int partidasValidas[MAX_PARTIDAS_GUARDADAS];
    int partidasDisponibles = 0;

    for (int i = 0; i < MAX_PARTIDAS_GUARDADAS; i++)
    {
        if (usuario->partidas[i].esValida)
        {
            partidasValidas[partidasDisponibles] = i;
            partidasDisponibles++;
        }
    }

    int opcionSeleccionada = 0;

    // Si no hay partidas guardadas
    if (partidasDisponibles == 0)
    {
        Uint32 tiempoInicio = SDL_GetTicks();

        while (SDL_GetTicks() - tiempoInicio < 3000)
        {
            SDL_Event e;
            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN)
                    return -1;
            }

            SDL_SetRenderDrawColor(renderer, azul.r, azul.g, azul.b, 255);
            SDL_RenderClear(renderer);

            if (fuenteGrande)
                renderizarTexto(renderer, fuenteGrande, "CARGAR PARTIDA", 0, altoVentana/2 - 100, amarillo, negro, 1, 1);

            if (fuente)
            {
                renderizarTexto(renderer, fuente, "No tienes partidas guardadas", 0, altoVentana/2 - 50, rojo, negro, 1, 1);
                renderizarTexto(renderer, fuente, "Juega una partida y presiona ESC para guardarla", 0, altoVentana/2 - 20, gris, negro, 1, 1);
                renderizarTexto(renderer, fuente, "Cerrando automaticamente...", 0, altoVentana/2 + 20, gris, negro, 1, 1);
            }

            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }
        return -1;
    }

    // ===== BUCLE PRINCIPAL =====
    while (1)
    {
        // Actualizar dimensiones de ventana en cada frame (por si el usuario la redimensiona)
        SDL_GetWindowSize(ventana, &anchoVentana, &altoVentana);

        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                return -1;

            if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                    case SDLK_UP:
                        opcionSeleccionada = (opcionSeleccionada - 1 + partidasDisponibles) % partidasDisponibles;
                        break;

                    case SDLK_DOWN:
                        opcionSeleccionada = (opcionSeleccionada + 1) % partidasDisponibles;
                        break;

                    case SDLK_RETURN:
                        return cargarYEjecutarPartida(ventana, renderer, fuente, fuenteGrande,usuario, partidasValidas[opcionSeleccionada], configuracion);

                    case SDLK_DELETE:
                        eliminarPartida(usuario, partidasValidas[opcionSeleccionada]);
                        guardarUsuario(usuario);

                        // Reconstruir lista
                        partidasDisponibles = 0;
                        for (int i = 0; i < MAX_PARTIDAS_GUARDADAS; i++)
                        {
                            if (usuario->partidas[i].esValida)
                            {
                                partidasValidas[partidasDisponibles] = i;
                                partidasDisponibles++;
                            }
                        }

                        if (partidasDisponibles == 0)
                            return -1;


                        if (opcionSeleccionada >= partidasDisponibles)
                            opcionSeleccionada = partidasDisponibles - 1;

                        break;

                    case SDLK_ESCAPE:
                        return -1;
                }
            }
        }

        // ===== RENDERIZADO RESPONSIVE =====
        SDL_SetRenderDrawColor(renderer, azul.r, azul.g, azul.b, 255);
        SDL_RenderClear(renderer);

        // Calcular posiciones responsivas
        int margen = anchoVentana * 0.05; // 5% de margen a cada lado
        int anchoSlot = anchoVentana - (margen * 2);
        int xSlot = margen;

        // Titulo
        if (fuenteGrande)
            renderizarTexto(renderer, fuenteGrande, "CARGAR PARTIDA GUARDADA", 0, 30, amarillo, negro, 1, 1);

        // Info usuario
        if (fuente)
        {
            char infoUsuario[100];
            snprintf(infoUsuario, sizeof(infoUsuario), "Usuario: %s | Partidas: %d/%d",
                    usuario->nombre, partidasDisponibles, MAX_PARTIDAS_GUARDADAS);
            renderizarTexto(renderer, fuente, infoUsuario, 0, 70, gris, negro, 1, 1);
        }

        // ===== SLOTS RESPONSIVOS =====
        int yInicial = 110;
        int altoSlot = (altoVentana - yInicial - 80) / (partidasDisponibles + 1); // Distribuir espacio disponible
        if (altoSlot > 100) altoSlot = 100; // Maximo 100px de alto
        if (altoSlot < 60) altoSlot = 60;   // Minimo 60px de alto

        for (int i = 0; i < partidasDisponibles; i++)
        {
            int indiceReal = partidasValidas[i];
            sPartidaGuardada* partida = &usuario->partidas[indiceReal];

            int ySlot = yInicial + (i * (altoSlot + 5));

            // Colores segun seleccion
            SDL_Color colorFondo = (i == opcionSeleccionada) ? azulClaro : (SDL_Color){30, 40, 60, 255};
            SDL_Color colorBorde = (i == opcionSeleccionada) ? amarillo : gris;
            SDL_Color colorTexto = (i == opcionSeleccionada) ? blanco : gris;
            SDL_Color colorNombre = (i == opcionSeleccionada) ? amarillo : blanco;

            // Fondo del slot
            SDL_Rect rectSlot = {xSlot, ySlot, anchoSlot, altoSlot};
            SDL_SetRenderDrawColor(renderer, colorFondo.r, colorFondo.g, colorFondo.b, 255);
            SDL_RenderFillRect(renderer, &rectSlot);

            // Borde
            SDL_SetRenderDrawColor(renderer, colorBorde.r, colorBorde.g, colorBorde.b, 255);
            SDL_RenderDrawRect(renderer, &rectSlot);
            if (i == opcionSeleccionada)
            {
                SDL_Rect rectBorde2 = {xSlot-2, ySlot-2, anchoSlot+4, altoSlot+4};
                SDL_RenderDrawRect(renderer, &rectBorde2);
            }

            if (fuente)
            {
                // Calcular posiciones del texto dentro del slot
                int margenTexto = 15;
                int xTexto = xSlot + margenTexto;
                int espaciadoLinea = altoSlot / 4;

                // Linea 1: Slot y nombre (truncar si es muy largo)
                char linea1[150];
                char nombreTruncado[50];
                if (strlen(partida->nombre) > 30)
                {
                    strncpy(nombreTruncado, partida->nombre, 27);
                    nombreTruncado[27] = '\0';
                    strcat(nombreTruncado, "...");
                }
                else
                    strcpy(nombreTruncado, partida->nombre);

                snprintf(linea1, sizeof(linea1), "SLOT %d: %s", indiceReal + 1, nombreTruncado);
                renderizarTexto(renderer, fuente, linea1, xTexto, ySlot + espaciadoLinea/2, colorNombre, negro, 0, 1);

                // Linea 2: Configuracion
                char linea2[100];
                snprintf(linea2, sizeof(linea2), "Tablero: %dx%d | Minas: %d | Restantes: %d", partida->dimensiones, partida->dimensiones, partida->cantMinas, partida->minasRestantes);
                renderizarTexto(renderer, fuente, linea2, xTexto, ySlot + espaciadoLinea * 2, colorTexto, negro, 0, 1);

                // Linea 3: Tiempo y estado
                char tiempoFormateado[20];
                formatearTiempo(partida->tiempoTranscurrido, tiempoFormateado);
                char linea3[100];
                char estadoTexto[30];

                if (partida->primerClic)
                    strcpy(estadoTexto, "Primer clic pendiente");
                else
                    strcpy(estadoTexto, "En progreso");

                snprintf(linea3, sizeof(linea3), "Tiempo: %s | Estado: %s", tiempoFormateado, estadoTexto);
                renderizarTexto(renderer, fuente, linea3, xTexto, ySlot + espaciadoLinea * 3, colorTexto, negro, 0, 1);
            }

            // Indicador de seleccion
            if (i == opcionSeleccionada && fuente)
                renderizarTexto(renderer, fuente, ">>", xSlot - 25, ySlot + altoSlot/2 - 8, amarillo, negro, 0, 1);
        }

        // Instrucciones en la parte inferior
        if (fuente)
        {
            int yInstrucciones = altoVentana - 50;
            renderizarTexto(renderer, fuente, "v^ Navegar | ENTER Cargar | DELETE Eliminar | ESC Volver",0, yInstrucciones, amarillo, negro, 1, 1);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

/**
 * Carga y ejecuta una partida especifica del usuario (version completa)
 * @param ventana - Ventana SDL
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente pequena
 * @param fuenteGrande - Fuente grande
 * @param usuario - Datos del usuario
 * @param indicePartida - Indice de la partida a cargar
 * @param configuracion - Configuracion base del juego (no se usa, se carga de la partida)
 * @return 0 si es exitoso, -1 si hay error
 */
int cargarYEjecutarPartida(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente,TTF_Font* fuenteGrande, sUsuario* usuario, int indicePartida,sArchivo_conf configuracion)
{
    if (indicePartida < 0 || indicePartida >= MAX_PARTIDAS_GUARDADAS || !usuario->partidas[indicePartida].esValida)
        return -1;

    sPartidaGuardada* partida = &usuario->partidas[indicePartida];

    SDL_Color blanco = {255, 255, 255, 255};
    SDL_Color negro = {0, 0, 0, 255};
    SDL_Color azul = {30, 42, 75, 255};

    // Mostrar pantalla de carga
    SDL_SetRenderDrawColor(renderer, azul.r, azul.g, azul.b, 255);
    SDL_RenderClear(renderer);

    if (fuenteGrande)
        renderizarTexto(renderer, fuenteGrande, "CARGANDO PARTIDA...", 0, 200, blanco, negro, 1, 1);

    char nombrePartida[150];
    snprintf(nombrePartida, sizeof(nombrePartida), "Cargando: %s", partida->nombre);
    if (fuente)
        renderizarTexto(renderer, fuente, nombrePartida, 0, 280, blanco, negro, 1, 1);

    char detallesPartida[150];
    snprintf(detallesPartida, sizeof(detallesPartida), "Tablero: %dx%d | Minas: %d",partida->dimensiones, partida->dimensiones, partida->cantMinas);
    if (fuente)
        renderizarTexto(renderer, fuente, detallesPartida, 0, 320, blanco, negro, 1, 1);

    SDL_RenderPresent(renderer);
    SDL_Delay(1500);

    // Variables para cargar la partida
    sCelda** matriz = NULL;
    sArchivo_conf configCargada;
    int minasRestantes;
    int tiempoTranscurrido;
    int primerClic;

    // Cargar la partida completa
    int resultado = cargarPartidaCompleta(usuario, indicePartida, &matriz, &configCargada,&minasRestantes, &tiempoTranscurrido, &primerClic);

    if (resultado != 0 || !matriz)
    {
        printf("Error al cargar la partida\n");

        // Mostrar mensaje de error
        SDL_SetRenderDrawColor(renderer, azul.r, azul.g, azul.b, 255);
        SDL_RenderClear(renderer);

        if (fuenteGrande)
            renderizarTexto(renderer, fuenteGrande, "ERROR AL CARGAR", 0, 250, blanco, negro, 1, 1);
        if (fuente)
            renderizarTexto(renderer, fuente, "No se pudo cargar la partida", 0, 300, blanco, negro, 1, 1);

        SDL_RenderPresent(renderer);
        SDL_Delay(2000);

        if (matriz)
            destruirMatriz(matriz, configCargada.dimensiones);

        return -1;
    }

    // Ejecutar la partida cargada
    ejecutarPartidaCargada(ventana, renderer, fuente, fuenteGrande, usuario, matriz,
                          configCargada, minasRestantes, tiempoTranscurrido, primerClic);

    // Limpiar recursos
    destruirMatriz(matriz, configCargada.dimensiones);

    return 0;
}

/**
 * Dibuja texto centrado en un rectangulo especifico
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente a utilizar
 * @param texto - Texto a dibujar
 * @param x - Coordenada X del rectangulo
 * @param y - Coordenada Y del rectangulo
 * @param ancho - Ancho del rectangulo para centrar
 * @param color - Color del texto
 */
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

/**
 * Dibuja un boton con texto centrado y efectos visuales
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente para el texto
 * @param texto - Texto del boton
 * @param rect - Rectangulo donde dibujar el boton
 * @param seleccionado - 1 si esta seleccionado, 0 si no
 */
void dibujarBotonSDL(SDL_Renderer* renderer, TTF_Font* fuente, const char* texto, SDL_Rect rect, int seleccionado)
{
    if (!renderer || !fuente || !texto)
        return;

    SDL_Color colorFondo = seleccionado ? (SDL_Color){100, 100, 100, 255} : (SDL_Color){50, 50, 50, 255};
    SDL_Color colorTexto = seleccionado ? (SDL_Color){255, 255, 0, 255} : (SDL_Color){255, 255, 255, 255};
    SDL_Color colorBorde = {200, 200, 200, 255};

    // Dibujar fondo del boton
    SDL_SetRenderDrawColor(renderer, colorFondo.r, colorFondo.g, colorFondo.b, colorFondo.a);
    SDL_RenderFillRect(renderer, &rect);

    // Dibujar borde
    SDL_SetRenderDrawColor(renderer, colorBorde.r, colorBorde.g, colorBorde.b, colorBorde.a);
    SDL_RenderDrawRect(renderer, &rect);

    // Dibujar texto centrado
    dibujarTextoCentrado(renderer, fuente, texto, rect.x, rect.y + (rect.h / 4), rect.w, colorTexto);
}

/**
 * Verifica si un clic del mouse esta dentro de un rectangulo
 * @param x - Coordenada X del clic
 * @param y - Coordenada Y del clic
 * @param rect - Rectangulo a verificar
 * @return 1 si el clic esta dentro, 0 si no
 */
int clicEnRect(int x, int y, SDL_Rect rect)
{
    return (x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h);
}

/**
 * Maneja la entrada de texto del usuario (escribir y borrar)
 * @param evento - Evento SDL a procesar
 * @param buffer - Buffer donde almacenar el texto
 * @param maxLen - Longitud maxima del texto
 * @return 1 si el buffer cambio, 0 si no
 */
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

/**
 * Escribe la configuracion actual al archivo buscaminas.conf segun formato del TP
 * @param config - Configuracion a guardar
 * @return 0 si es exitoso, -1 si hay error
 */
int guardarConfiguracion(sArchivo_conf config)
{
    FILE* archivo = fopen(ARCH_CONFIG, "w");
    if (!archivo) {
        return -1;
    }

    // Escribir archivo segun formato del TP
    fprintf(archivo, "# Configuracion del Buscaminas\n");
    fprintf(archivo, "# Dimensiones del tablero (minimo 8, maximo 32)\n");
    fprintf(archivo, "dimensiones=%d\n", config.dimensiones);
    fprintf(archivo, "\n");
    fprintf(archivo, "# Cantidad de minas (por numero o porcentaje, ej: cantidad_minas=40 o cantidad_minas=15%%)\n");
    fprintf(archivo, "cantidad_minas=%d\n", config.cantMinas);

    fclose(archivo);
    return 0;
}

/**
 * Muestra el menu de configuracion segun especificaciones del TP (SIN UNICODE)
 * @param ventana - Ventana SDL
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente pequena
 * @param fuenteGrande - Fuente grande
 * @param config - Puntero a la configuracion actual (se modifica)
 * @return 1 si se guardaron cambios, 0 si se cancelo
 */
int mostrarMenuConfiguracion(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, sArchivo_conf* config)
{
    SDL_SetWindowSize(ventana, 850, 750);
    SDL_SetWindowPosition(ventana, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    int opcionSeleccionada = 0;
    const int numOpciones = 5;
    const char* opciones[] =
    {
        "Modificar Dimensiones (8-32)",
        "Modificar Cantidad de Minas",
        "Configurar por Porcentaje",
        "Guardar y Aplicar",
        "Cancelar (sin guardar)"
    };

    SDL_Color blanco = {255, 255, 255, 255};
    SDL_Color amarillo = {255, 255, 0, 255};
    SDL_Color negro = {0, 0, 0, 255};
    SDL_Color azul = {30, 42, 75, 255};
    SDL_Color gris = {128, 128, 128, 255};
    SDL_Color verde = {0, 255, 0, 255};
    SDL_Color rojo = {255, 100, 100, 255};
    SDL_Color cian = {0, 255, 255, 255};

    // Variables temporales para la nueva configuracion
    sArchivo_conf configTemp = *config;
    int mostrarMensaje = 0;
    char mensaje[250] = "";
    SDL_Color colorMensaje = verde;

    // Variables para entrada de numeros
    int modificandoValor = 0; // 0: no, 1: dimensiones, 2: minas, 3: porcentaje
    char bufferEntrada[10] = "";

    while (1)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                return 0; // Cancelar


            if (modificandoValor > 0)
            {
                // Modo entrada de numeros
                if (e.type == SDL_TEXTINPUT)
                {
                    if (strlen(bufferEntrada) < 8)
                    {
                        // Solo permitir digitos
                        if (e.text.text[0] >= '0' && e.text.text[0] <= '9')
                            strcat(bufferEntrada, e.text.text);
                    }
                }
                else if (e.type == SDL_KEYDOWN)
                {
                    if (e.key.keysym.sym == SDLK_BACKSPACE && strlen(bufferEntrada) > 0)
                        bufferEntrada[strlen(bufferEntrada) - 1] = '\0';

                    else if (e.key.keysym.sym == SDLK_RETURN && strlen(bufferEntrada) > 0)
                    {
                        int valor = atoi(bufferEntrada);

                        if (modificandoValor == 1) { // Dimensiones
                            if (valor >= 8 && valor <= MAX_DIMENSION)
                            {
                                configTemp.dimensiones = valor;
                                // Ajustar minas si excede el maximo posible
                                int maxMinas = (valor * valor) - 1;
                                if (configTemp.cantMinas > maxMinas)
                                    configTemp.cantMinas = maxMinas / 4; // 25% por defecto

                                sprintf(mensaje, "Dimensiones cambiadas a %dx%d (Especificaciones TP: 8-32)", valor, valor);
                                colorMensaje = verde;
                            }
                            else
                            {
                                sprintf(mensaje, "ERROR: Dimensiones deben estar entre 8 y 32 (segun especificaciones del TP)");
                                colorMensaje = rojo;
                            }
                        }
                        else if (modificandoValor == 2)
                        { // Minas por cantidad
                            int maxMinas = (configTemp.dimensiones * configTemp.dimensiones) - 1;
                            if (valor >= 1 && valor <= maxMinas)
                            {
                                configTemp.cantMinas = valor;
                                sprintf(mensaje, "Cantidad de minas cambiada a %d", valor);
                                colorMensaje = verde;
                            }
                            else
                            {
                                sprintf(mensaje, "ERROR: Minas deben estar entre 1 y %d", maxMinas);
                                colorMensaje = rojo;
                            }
                        }
                        else if (modificandoValor == 3)
                        { // Minas por porcentaje
                            if (valor >= 1 && valor <= 90)
                            {
                                int totalCeldas = configTemp.dimensiones * configTemp.dimensiones;
                                configTemp.cantMinas = (valor * totalCeldas) / 100;
                                if (configTemp.cantMinas < 1) configTemp.cantMinas = 1;
                                sprintf(mensaje, "Porcentaje %d%% = %d minas de %d celdas", valor, configTemp.cantMinas, totalCeldas);
                                colorMensaje = verde;
                            }
                            else
                            {
                                sprintf(mensaje, "ERROR: Porcentaje debe estar entre 1%% y 90%%");
                                colorMensaje = rojo;
                            }
                        }

                        mostrarMensaje = 1;
                        modificandoValor = 0;
                        strcpy(bufferEntrada, "");
                        SDL_StopTextInput();
                    }
                    else if (e.key.keysym.sym == SDLK_ESCAPE)
                    {
                        modificandoValor = 0;
                        strcpy(bufferEntrada, "");
                        SDL_StopTextInput();
                    }
                }
            }
            else
            {
                // Modo navegacion normal
                if (e.type == SDL_KEYDOWN)
                {
                    switch (e.key.keysym.sym)
                    {
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
                                case 0: // Modificar dimensiones
                                    modificandoValor = 1;
                                    strcpy(bufferEntrada, "");
                                    SDL_StartTextInput();
                                    mostrarMensaje = 0;
                                    break;
                                case 1: // Modificar minas por cantidad
                                    modificandoValor = 2;
                                    strcpy(bufferEntrada, "");
                                    SDL_StartTextInput();
                                    mostrarMensaje = 0;
                                    break;
                                case 2: // Modificar minas por porcentaje
                                    modificandoValor = 3;
                                    strcpy(bufferEntrada, "");
                                    SDL_StartTextInput();
                                    mostrarMensaje = 0;
                                    break;
                                case 3: // Guardar
                                    if (guardarConfiguracion(configTemp) == 0)
                                    {
                                        *config = configTemp;
                                        return 1; // Guardado exitoso
                                    } else {
                                        sprintf(mensaje, "Error al guardar configuracion");
                                        colorMensaje = rojo;
                                        mostrarMensaje = 1;
                                    }
                                    break;
                                case 4: // Cancelar
                                    return 0;
                            }
                            break;
                        case SDLK_ESCAPE:
                            return 0; // Cancelar
                    }
                }
            }
        }

        // Renderizado
        SDL_SetRenderDrawColor(renderer, azul.r, azul.g, azul.b, 255);
        SDL_RenderClear(renderer);

        int anchoVentana;
        SDL_GetRendererOutputSize(renderer, &anchoVentana, NULL);

        // Titulo
        if (fuenteGrande)
            renderizarTexto(renderer, fuenteGrande, "CONFIGURACION - ESPECIFICACIONES TP", 0, 30, amarillo, negro, 1, 1);

        // Mostrar configuracion actual
        if (fuente)
        {
            char configActual[200];
            sprintf(configActual, "Configuracion actual: %dx%d tablero, %d minas",configTemp.dimensiones, configTemp.dimensiones, configTemp.cantMinas);
            renderizarTexto(renderer, fuente, configActual, 0, 80, blanco, negro, 1, 1);

            int totalCeldas = configTemp.dimensiones * configTemp.dimensiones;
            double porcentaje = ((double)configTemp.cantMinas / totalCeldas) * 100.0;
            sprintf(configActual, "Porcentaje: %.1f%% | Rango TP: 8x8 a 32x32", porcentaje);
            renderizarTexto(renderer, fuente, configActual, 0, 105, cian, negro, 1, 1);

            renderizarTexto(renderer, fuente, "Formato archivo: dimensiones=16 y cantidad_minas=40", 0, 130, gris, negro, 1, 1);
        }

        if (modificandoValor > 0)
        {
            // Modo entrada de numeros
            if (fuente)
            {
                const char* pregunta;
                switch (modificandoValor)
                {
                    case 1: pregunta = "Nuevas dimensiones (8-32):"; break;
                    case 2: pregunta = "Nueva cantidad de minas:"; break;
                    case 3: pregunta = "Nuevo porcentaje (1-90%):"; break;
                    default: pregunta = "Valor:"; break;
                }

                renderizarTexto(renderer, fuente, pregunta, 0, 200, blanco, negro, 1, 1);

                // Campo de entrada
                char textoEntrada[20];
                sprintf(textoEntrada, "%s_", bufferEntrada);
                renderizarTexto(renderer, fuente, textoEntrada, 0, 240, amarillo, negro, 1, 1);

                renderizarTexto(renderer, fuente, "ENTER para confirmar, ESC para cancelar", 0, 280, gris, negro, 1, 1);
            }
        }
        else
        {
            // Menu normal - USANDO EL MISMO SISTEMA QUE EL MENU PRINCIPAL
            for (int i = 0; i < numOpciones; i++)
            {
                SDL_Color color = (i == opcionSeleccionada) ? amarillo : blanco;
                int y = 180 + i * 40;

                if (fuente)
                {
                    // Calcular posicion centrada de la opcion
                    int anchoOpcionTexto = 0;
                    TTF_SizeText(fuente, opciones[i], &anchoOpcionTexto, NULL);
                    int xInicioOpcion = (anchoVentana - anchoOpcionTexto) / 2;

                    // Dibujar texto de la opcion
                    renderizarTexto(renderer, fuente, opciones[i], xInicioOpcion, y, color, negro, 0, 1);

                    // Dibujar indicador de seleccion (igual que el menu principal)
                    if (i == opcionSeleccionada)
                    {
                        int anchoFlecha = 0;
                        TTF_SizeText(fuente, ">>", &anchoFlecha, NULL);
                        renderizarTexto(renderer, fuente, ">>", xInicioOpcion - anchoFlecha - 15, y, amarillo, negro, 0, 1);
                    }
                }
            }


        }

        // Mostrar mensaje si hay uno
        if (mostrarMensaje && fuente)
            renderizarTexto(renderer, fuente, mensaje, 0, 550, colorMensaje, negro, 1, 1);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

/**
 * Muestra las instrucciones del juego en pantalla completa con scroll
 * @param ventana - Ventana SDL
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente pequeña
 * @param fuenteGrande - Fuente grande
 */
void mostrarInstruccionesSDL(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande)
{
    SDL_SetWindowSize(ventana, 900, 700);
    SDL_SetWindowPosition(ventana, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    SDL_Color blanco = {255, 255, 255, 255};
    SDL_Color amarillo = {255, 255, 0, 255};
    SDL_Color negro = {0, 0, 0, 255};
    SDL_Color azul = {30, 42, 75, 255};
    SDL_Color gris = {128, 128, 128, 255};
    SDL_Color verde = {0, 255, 0, 255};
    SDL_Color rojo = {255, 100, 100, 255};
    SDL_Color cian = {0, 255, 255, 255};

    int scrollOffset = 0;
    const int maxScroll = 800;
    const int scrollSpeed = 20;

    // Array con todas las líneas de instrucciones
    const char* instrucciones[] =
    {
        "=== BUSCAMINAS PIXEL - INSTRUCCIONES ===",
        "",
        "OBJETIVO DEL JUEGO:",
        "- Revelar todas las celdas que no contengan minas",
        "- Usar banderas para marcar celdas sospechosas",
        "- Los numeros indican minas en celdas adyacentes",
        "",
        "CONTROLES BASICOS:",
        "- Clic Izquierdo: Revelar celda",
        "- Clic Derecho: Colocar/quitar bandera",
        "- ESC o P: Pausar y acceder al menu de guardado",
        "- Flechas: Navegar en menus",
        "- ENTER: Confirmar seleccion",
        "",
        "FUNCIONES ESPECIALES:",
        "",
        "[ESCUDO] INMUNIDAD DEL PRIMER CLIC:",
        "- Tu primer clic NUNCA sera una mina",
        "- Si clickeas una mina al inicio, se reubica",
        "- Los numeros se recalculan automaticamente",
        "",
        "[VISION] CHEAT X-RAY (Ctrl + H):",
        "- DISPONIBLE: Solo una vez por partida",
        "- DURACION: 2 segundos",
        "- EFECTO: Revela el contenido oculto",
        "  * Minas: Fondo rojo con bomba",
        "  * Seguras: Fondo verde con numeros",
        "  * Bordes amarillos indican efecto activo",
        "- INDICADOR: Mensaje parpadeante arriba-derecha",
        "- RESTRICCION: Una vez usado, no se puede repetir",
        "",
        "SISTEMA DE GUARDADO:",
        "- Presiona ESC durante el juego",
        "- Selecciona 'Guardar y Salir'",
        "- Asigna un nombre descriptivo",
        "- Se guarda TODO el estado actual",
        "- Maximo 5 partidas por usuario",
        "- DELETE para eliminar partidas guardadas",
        "",
        "CONFIGURACION:",
        "- Dimensiones: 8x8 hasta 32x32",
        "- Minas por numero: cantidad_minas=40",
        "- Minas por porcentaje: cantidad_minas=15%",
        "- Archivo: buscaminas.conf",
        "",
        "ESTRATEGIAS Y TIPS:",
        "- Empieza por las esquinas y bordes",
        "- Marca minas obvias con banderas",
        "- Si un numero ya tiene suficientes banderas,",
        "  las celdas restantes son seguras",
        "- Patron 1-2-1: Formacion comun de minas",
        "- Usa el cheat cuando estes atascado",
        "- Experimenta con movimientos arriesgados",
        "  y guarda antes si no estas seguro",
        "",
        "ESTADISTICAS:",
        "- Partidas jugadas y ganadas",
        "- Tiempo total y mejor tiempo personal",
        "- Racha actual y mejor racha historica",
        "- Porcentaje de victorias",
        "",
        "ARCHIVOS DEL JUEGO:",
        "- buscaminas.conf - Configuracion",
        "- usuarios.dat - Base de datos usuarios",
        "- Session_Buscaminas.log - Logs actuales",
        "",
        "SOLUCION DE PROBLEMAS:",
        "- Fuentes: Coloca arial.ttf en la carpeta",
        "- SDL2: Instala las bibliotecas necesarias",
        "- Permisos: Verifica escritura en la carpeta",
        "",
        "DESARROLLADO POR:",
        "- Linares, Guido Hernan (DNI: 43170056)",
        "- Goldring, Facundo (DNI: 44595085)",
        "",
        "Presiona ESC para volver al menu principal",
        "Usa las flechas UP/DOWN para hacer scroll",
        NULL // Marcador de fin
    };

    // Bucle principal de instrucciones
    while (1)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                return;

            if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                    case SDLK_UP:
                        scrollOffset -= scrollSpeed;
                        if (scrollOffset < 0) scrollOffset = 0;
                        break;
                    case SDLK_DOWN:
                        scrollOffset += scrollSpeed;
                        if (scrollOffset > maxScroll) scrollOffset = maxScroll;
                        break;
                    case SDLK_ESCAPE:
                        return; // Volver al menu principal
                    case SDLK_PAGEUP:
                        scrollOffset -= scrollSpeed * 5;
                        if (scrollOffset < 0) scrollOffset = 0;
                        break;
                    case SDLK_PAGEDOWN:
                        scrollOffset += scrollSpeed * 5;
                        if (scrollOffset > maxScroll) scrollOffset = maxScroll;
                        break;
                    case SDLK_HOME:
                        scrollOffset = 0;
                        break;
                    case SDLK_END:
                        scrollOffset = maxScroll;
                        break;
                }
            }
        }

        // Renderizado
        SDL_SetRenderDrawColor(renderer, azul.r, azul.g, azul.b, 255);
        SDL_RenderClear(renderer);

        // Contenido con scroll PRIMERO
        if (fuente)
        {
            int yInicial = 120 - scrollOffset;
            int lineaActual = 0;

            // Renderizar cada línea de instrucciones
            for (int i = 0; instrucciones[i] != NULL; i++)
            {
                int yLinea = yInicial + (lineaActual * 25);

                if (yLinea > 80 && yLinea < 620) // Ajustar límites para evitar superposición
                {
                    SDL_Color color = blanco; // Color por defecto

                    // Colorear según el contenido
                    if (strstr(instrucciones[i], "===") || strstr(instrucciones[i], "OBJETIVO") ||
                        strstr(instrucciones[i], "CONTROLES") || strstr(instrucciones[i], "FUNCIONES") ||
                        strstr(instrucciones[i], "SISTEMA") || strstr(instrucciones[i], "CONFIGURACION") ||
                        strstr(instrucciones[i], "ESTRATEGIAS") || strstr(instrucciones[i], "ESTADISTICAS") ||
                        strstr(instrucciones[i], "ARCHIVOS") || strstr(instrucciones[i], "SOLUCION") ||
                        strstr(instrucciones[i], "DESARROLLADO"))
                        color = amarillo;

                    else if (strstr(instrucciones[i], "[ESCUDO]") || strstr(instrucciones[i], "INMUNIDAD"))
                        color = verde;

                    else if (strstr(instrucciones[i], "[VISION]") || strstr(instrucciones[i], "CHEAT") ||
                             strstr(instrucciones[i], "X-RAY"))
                        color = cian;

                    else if (strstr(instrucciones[i], "-"))
                        color = gris;

                    else if (strstr(instrucciones[i], "Presiona") || strstr(instrucciones[i], "Usa las"))
                        color = rojo;


                    // Renderizar la línea
                    int anchoVentana;
                    SDL_GetRendererOutputSize(renderer, &anchoVentana, NULL);

                    // Calcular posición centrada o con margen
                    int xTexto = 50;
                    if (strstr(instrucciones[i], "===") || strstr(instrucciones[i], "OBJETIVO") || strlen(instrucciones[i]) == 0)
                    {
                        // Centrar títulos y líneas vacías
                        renderizarTexto(renderer, fuente, instrucciones[i], 0, yLinea, color, negro, 1, 1);
                    } else {
                        // Texto con margen izquierdo
                        renderizarTexto(renderer, fuente, instrucciones[i], xTexto, yLinea, color, negro, 0, 1);
                    }
                }
                lineaActual++;
            }
        }

        // Header fijo encima del contenido con fondo
        SDL_SetRenderDrawColor(renderer, azul.r, azul.g, azul.b, 255);
        SDL_Rect rectHeader = {0, 0, 900, 80};
        SDL_RenderFillRect(renderer, &rectHeader);

        // Línea separadora del header
        SDL_SetRenderDrawColor(renderer, amarillo.r, amarillo.g, amarillo.b, 255);
        SDL_RenderDrawLine(renderer, 0, 80, 900, 80);

        // Título fijo en la parte superior
        if (fuenteGrande)
            renderizarTexto(renderer, fuenteGrande, "INSTRUCCIONES", 0, 20, amarillo, negro, 1, 1);

        // Indicador de scroll
        if (fuente)
        {
            char scrollInfo[50];
            sprintf(scrollInfo, "Scroll: %d/%d", scrollOffset, maxScroll);
            renderizarTexto(renderer, fuente, scrollInfo, 0, 50, gris, negro, 1, 1);
        }

        // Instrucciones de navegación en la parte inferior (fijas)
        if (fuente)
        {
            SDL_SetRenderDrawColor(renderer, azul.r - 10, azul.g - 10, azul.b - 10, 230);
            SDL_Rect rectInstrucciones = {0, 620, 900, 80};
            SDL_RenderFillRect(renderer, &rectInstrucciones);

            // Línea separadora del footer
            SDL_SetRenderDrawColor(renderer, amarillo.r, amarillo.g, amarillo.b, 255);
            SDL_RenderDrawLine(renderer, 0, 620, 900, 620);

            renderizarTexto(renderer, fuente, "UP/DOWN Scroll | PgUp/PgDn Scroll rapido", 0, 635, amarillo, negro, 1, 1);
            renderizarTexto(renderer, fuente, "Home/End Inicio/Fin | ESC Volver al menu", 0, 655, amarillo, negro, 1, 1);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}
