#include "common.h"
#include "menu.h"
#include "usuario.h"
#include "juego.h"
#include "tablero.h"

/**
 * Renderiza texto con efecto de sombra en una posición específica
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente a utilizar
 * @param texto - Texto a renderizar
 * @param x - Coordenada X donde dibujar
 * @param y - Coordenada Y donde dibujar
 * @param colorTexto - Color del texto principal
 * @param colorSombra - Color de la sombra
 */
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

/**
 * Valida si un nombre de usuario cumple con los criterios requeridos
 * @param nombre - Nombre a validar
 * @return 1 si es válido, 0 si no lo es
 */
int validarNombreUsuario(const char *nombre)
{
    if (nombre == NULL)
        return 0;

    size_t longitud = strlen(nombre);

    if (longitud < 3)
        return 0;

    // Ciclo para verificar que todos los caracteres sean alfanuméricos o guión bajo
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
 * @param fuente - Fuente pequeña
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

        // Instruccion inferior
        if (fuente)
            renderizarTextoCentrado(renderer, fuente, "ESC para salir", 400, gris, negro);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_StopTextInput();
    return -1;
}

/**
 * Muestra el menú principal y maneja la selección de opciones
 * @param ventana - Ventana SDL
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente pequeña
 * @param fuenteGrande - Fuente grande
 * @param usuario - Datos del usuario actual
 * @return Número de la opción seleccionada (1-5)
 */
int mostrarMenuSDL(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuario)
{
    SDL_SetWindowSize(ventana, 700, 700);
    SDL_SetWindowPosition(ventana, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    int opcionSeleccionada = 0;
    const int numOpciones = 5;
    const char* opciones[] = {
        "1. Nueva Partida",
        "2. Ver Estadisticas",
        "3. Cargar Partida",
        "4. Recargar Configuracion",
        "5. Salir"
    };

    SDL_Color blanco = {255, 255, 255, 255};
    SDL_Color amarillo = {255, 255, 0, 255};
    SDL_Color negro = {0, 0, 0, 255};
    SDL_Color azul = {30, 42, 75, 255};
    SDL_Color gris = {128, 128, 128, 255};

    // Bucle principal del menú
    while (1)
    {
        SDL_Event e;
        // Ciclo para procesar eventos del menú
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

        // Renderizado del menú principal
        SDL_SetRenderDrawColor(renderer, azul.r, azul.g, azul.b, 255);
        SDL_RenderClear(renderer);

        // Titulo
        if (fuenteGrande)
            renderizarTextoCentrado(renderer, fuenteGrande, "MENU PRINCIPAL", 50, blanco, negro);

        // Informacion del usuario
        if (fuente)
        {
            char infosUsuario[100];
            snprintf(infosUsuario, sizeof(infosUsuario), "sUsuario: %s", usuario->nombre);
            renderizarTextoCentrado(renderer, fuente, infosUsuario, 120, gris, negro);

            snprintf(infosUsuario, sizeof(infosUsuario), "Partidas: %d | Ganadas: %d",
            usuario->estadisticas.partidasJugadas, usuario->estadisticas.partidasGanadas);
            renderizarTextoCentrado(renderer, fuente, infosUsuario, 140, gris, negro);
        }

        // Opciones del menu - ciclo para dibujar todas las opciones
        for (int i = 0; i < numOpciones; i++)
        {
            SDL_Color colorOpcion;
            if (i == opcionSeleccionada)
                colorOpcion = amarillo;
            else
                colorOpcion = blanco;

            int y = 200 + i * 40; // Calcula la posicion 'y' para la opcion

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
            renderizarTextoCentrado(renderer, fuente, "Usa las flechas y ENTER, o presiona el numero", 420, gris, negro);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

/**
 * Muestra las estadísticas del usuario en pantalla completa
 * @param ventana - Ventana SDL
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente pequeña
 * @param fuenteGrande - Fuente grande
 * @param usuario - Datos del usuario para mostrar estadísticas
 */
void mostrarEstadisticasSDL(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuario)
{
    SDL_Color blanco = {255, 255, 255, 255};
    SDL_Color negro = {0, 0, 0, 255};
    SDL_Color azul = {30, 42, 75, 255};
    SDL_Color gris = {128, 128, 128, 255};

    // Bucle para mostrar estadísticas hasta que el usuario presione una tecla
    while (1)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN)
                return; // Regresar al menu
        }

        // Renderizado de la pantalla de estadísticas
        SDL_SetRenderDrawColor(renderer, azul.r, azul.g, azul.b, 255);
        SDL_RenderClear(renderer);

        // Titulo
        if (fuenteGrande)
            renderizarTextoConSombra(renderer, fuenteGrande, "ESTADISTICAS", 200, 50, blanco, negro);

        if (fuente)
        {
            char buffer[100];
            int y = 120;
            int espaciado = 30;

            // Nombre del usuario
            snprintf(buffer, sizeof(buffer), "sUsuario: %s", usuario->nombre);
            renderizarTextoConSombra(renderer, fuente, buffer, 100, y, blanco, negro);
            y += espaciado;

            // Estadisticas
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
            renderizarTextoConSombra(renderer, fuente, "Presiona cualquier tecla para volver", 250, y + espaciado, gris, negro);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

/**
 * Interfaz responsive para cargar partidas guardadas del usuario
 * Se adapta automáticamente al tamaño de la ventana
 */
int cargarPartidaSDL(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuario, sArchivo_conf configuracion)
{
    debugMostrarPartidasGuardadas(usuario);

    // Obtener dimensiones actuales de la ventana
    int anchoVentana, altoVentana;
    SDL_GetWindowSize(ventana, &anchoVentana, &altoVentana);

    // Configurar ventana con tamaño mínimo adecuado
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

    // ===== CREAR LISTA DE PARTIDAS VÁLIDAS =====
    int partidasValidas[MAX_PARTIDAS_GUARDADAS];
    int partidasDisponibles = 0;

    for (int i = 0; i < MAX_PARTIDAS_GUARDADAS; i++) {
        if (usuario->partidas[i].esValida) {
            partidasValidas[partidasDisponibles] = i;
            partidasDisponibles++;
        }
    }

    int opcionSeleccionada = 0;

    // Si no hay partidas guardadas
    if (partidasDisponibles == 0) {
        Uint32 tiempoInicio = SDL_GetTicks();

        while (SDL_GetTicks() - tiempoInicio < 3000) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN)
                    return -1;
            }

            SDL_SetRenderDrawColor(renderer, azul.r, azul.g, azul.b, 255);
            SDL_RenderClear(renderer);

            if (fuenteGrande)
                renderizarTextoCentrado(renderer, fuenteGrande, "CARGAR PARTIDA", altoVentana/2 - 100, amarillo, negro);

            if (fuente) {
                renderizarTextoCentrado(renderer, fuente, "No tienes partidas guardadas", altoVentana/2 - 50, rojo, negro);
                renderizarTextoCentrado(renderer, fuente, "Juega una partida y presiona ESC para guardarla", altoVentana/2 - 20, gris, negro);
                renderizarTextoCentrado(renderer, fuente, "Cerrando automáticamente...", altoVentana/2 + 20, gris, negro);
            }

            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }
        return -1;
    }

    // ===== BUCLE PRINCIPAL =====
    while (1) {
        // Actualizar dimensiones de ventana en cada frame (por si el usuario la redimensiona)
        SDL_GetWindowSize(ventana, &anchoVentana, &altoVentana);

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                return -1;
            }

            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        opcionSeleccionada = (opcionSeleccionada - 1 + partidasDisponibles) % partidasDisponibles;
                        break;

                    case SDLK_DOWN:
                        opcionSeleccionada = (opcionSeleccionada + 1) % partidasDisponibles;
                        break;

                    case SDLK_RETURN:
                        return cargarYEjecutarPartidaCompleta(ventana, renderer, fuente, fuenteGrande,
                                                            usuario, partidasValidas[opcionSeleccionada], configuracion);

                    case SDLK_DELETE:
                        eliminarPartida(usuario, partidasValidas[opcionSeleccionada]);
                        guardarUsuario(usuario);

                        // Reconstruir lista
                        partidasDisponibles = 0;
                        for (int i = 0; i < MAX_PARTIDAS_GUARDADAS; i++) {
                            if (usuario->partidas[i].esValida) {
                                partidasValidas[partidasDisponibles] = i;
                                partidasDisponibles++;
                            }
                        }

                        if (partidasDisponibles == 0) {
                            return -1;
                        }

                        if (opcionSeleccionada >= partidasDisponibles) {
                            opcionSeleccionada = partidasDisponibles - 1;
                        }
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

        // Título
        if (fuenteGrande)
            renderizarTextoCentrado(renderer, fuenteGrande, "CARGAR PARTIDA GUARDADA", 30, amarillo, negro);

        // Info usuario
        if (fuente) {
            char infoUsuario[100];
            snprintf(infoUsuario, sizeof(infoUsuario), "Usuario: %s | Partidas: %d/%d",
                    usuario->nombre, partidasDisponibles, MAX_PARTIDAS_GUARDADAS);
            renderizarTextoCentrado(renderer, fuente, infoUsuario, 70, gris, negro);
        }

        // ===== SLOTS RESPONSIVOS =====
        int yInicial = 110;
        int altoSlot = (altoVentana - yInicial - 80) / (partidasDisponibles + 1); // Distribuir espacio disponible
        if (altoSlot > 100) altoSlot = 100; // Máximo 100px de alto
        if (altoSlot < 60) altoSlot = 60;   // Mínimo 60px de alto

        for (int i = 0; i < partidasDisponibles; i++) {
            int indiceReal = partidasValidas[i];
            sPartidaGuardada* partida = &usuario->partidas[indiceReal];

            int ySlot = yInicial + (i * (altoSlot + 5));

            // Colores según selección
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
            if (i == opcionSeleccionada) {
                SDL_Rect rectBorde2 = {xSlot-2, ySlot-2, anchoSlot+4, altoSlot+4};
                SDL_RenderDrawRect(renderer, &rectBorde2);
            }

            if (fuente) {
                // Calcular posiciones del texto dentro del slot
                int margenTexto = 15;
                int xTexto = xSlot + margenTexto;
                int espaciadoLinea = altoSlot / 4;

                // Línea 1: Slot y nombre (truncar si es muy largo)
                char linea1[150];
                char nombreTruncado[50];
                if (strlen(partida->nombre) > 30) {
                    strncpy(nombreTruncado, partida->nombre, 27);
                    nombreTruncado[27] = '\0';
                    strcat(nombreTruncado, "...");
                } else {
                    strcpy(nombreTruncado, partida->nombre);
                }
                snprintf(linea1, sizeof(linea1), "SLOT %d: %s", indiceReal + 1, nombreTruncado);
                renderizarTextoConSombra(renderer, fuente, linea1, xTexto, ySlot + espaciadoLinea/2, colorNombre, negro);

                // Línea 2: Configuración
                char linea2[100];
                snprintf(linea2, sizeof(linea2), "Tablero: %dx%d | Minas: %d | Restantes: %d",
                        partida->dimensiones, partida->dimensiones, partida->cantMinas, partida->minasRestantes);
                renderizarTextoConSombra(renderer, fuente, linea2, xTexto, ySlot + espaciadoLinea * 2, colorTexto, negro);

                // Línea 3: Tiempo y estado
                char tiempoFormateado[20];
                formatearTiempo(partida->tiempoTranscurrido, tiempoFormateado);
                char linea3[100];
                char estadoTexto[30];
                if (partida->primerClic) {
                    strcpy(estadoTexto, "Primer clic pendiente");
                } else {
                    strcpy(estadoTexto, "En progreso");
                }
                snprintf(linea3, sizeof(linea3), "Tiempo: %s | Estado: %s", tiempoFormateado, estadoTexto);
                renderizarTextoConSombra(renderer, fuente, linea3, xTexto, ySlot + espaciadoLinea * 3, colorTexto, negro);
            }

            // Indicador de selección
            if (i == opcionSeleccionada && fuente) {
                renderizarTextoConSombra(renderer, fuente, "►", xSlot - 25, ySlot + altoSlot/2 - 8, amarillo, negro);
            }
        }

        // Instrucciones en la parte inferior
        if (fuente) {
            int yInstrucciones = altoVentana - 50;
            renderizarTextoCentrado(renderer, fuente, "↑↓ Navegar | ENTER Cargar | DELETE Eliminar | ESC Volver",
                                  yInstrucciones, amarillo, negro);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

/**
 * Carga y ejecuta una partida específica del usuario
 * @param ventana - Ventana SDL
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente pequeña
 * @param fuenteGrande - Fuente grande
 * @param usuario - Datos del usuario
 * @param indicePartida - Índice de la partida a cargar
 * @param configuracion - Configuración base del juego
 * @return 0 si es exitoso, -1 si hay error
 */
int cargarYEjecutarPartida(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuario, int indicePartida, sArchivo_conf configuracion)
{
    if (indicePartida < 0 || indicePartida >= MAX_PARTIDAS_GUARDADAS || !usuario->partidas[indicePartida].esValida) {
        return -1;
    }

    sPartidaGuardada* partida = &usuario->partidas[indicePartida];

    sArchivo_conf configPartida;
    configPartida.dimensiones = partida->dimensiones;
    configPartida.cantMinas = partida->cantMinas;

    SDL_Color blanco = {255, 255, 255, 255};
    SDL_Color negro = {0, 0, 0, 255};
    SDL_Color azul = {30, 42, 75, 255};

    SDL_SetRenderDrawColor(renderer, azul.r, azul.g, azul.b, 255);
    SDL_RenderClear(renderer);

    if (fuenteGrande)
        renderizarTextoCentrado(renderer, fuenteGrande, "CARGANDO PARTIDA...", 200, blanco, negro);

    char nombrePartida[100];
    snprintf(nombrePartida, sizeof(nombrePartida), "Cargando: %s", partida->nombre);
    if (fuente)
        renderizarTextoCentrado(renderer, fuente, nombrePartida, 280, blanco, negro);

    SDL_RenderPresent(renderer);
    SDL_Delay(1000);

    ejecutarPartida(ventana, renderer, fuente, fuenteGrande, usuario, configPartida);

    return 0;
}

/**
 * Dibuja texto centrado en un rectángulo específico
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente a utilizar
 * @param texto - Texto a dibujar
 * @param x - Coordenada X del rectángulo
 * @param y - Coordenada Y del rectángulo
 * @param ancho - Ancho del rectángulo para centrar
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
 * Dibuja un botón con texto centrado y efectos visuales
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente para el texto
 * @param texto - Texto del botón
 * @param rect - Rectángulo donde dibujar el botón
 * @param seleccionado - 1 si está seleccionado, 0 si no
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
 * Verifica si un clic del mouse está dentro de un rectángulo
 * @param x - Coordenada X del clic
 * @param y - Coordenada Y del clic
 * @param rect - Rectángulo a verificar
 * @return 1 si el clic está dentro, 0 si no
 */
int clicEnRect(int x, int y, SDL_Rect rect)
{
    return (x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h);
}

/**
 * Maneja la entrada de texto del usuario (escribir y borrar)
 * @param evento - Evento SDL a procesar
 * @param buffer - Buffer donde almacenar el texto
 * @param maxLen - Longitud máxima del texto
 * @return 1 si el buffer cambió, 0 si no
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
 * Renderiza texto centrado horizontalmente en la ventana con sombra
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente a utilizar
 * @param texto - Texto a renderizar
 * @param y - Coordenada Y donde dibujar
 * @param colorTexto - Color del texto principal
 * @param colorSombra - Color de la sombra
 */
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

/**
 * Carga y ejecuta una partida específica del usuario (versión completa)
 * @param ventana - Ventana SDL
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente pequeña
 * @param fuenteGrande - Fuente grande
 * @param usuario - Datos del usuario
 * @param indicePartida - Índice de la partida a cargar
 * @param configuracion - Configuración base del juego (no se usa, se carga de la partida)
 * @return 0 si es exitoso, -1 si hay error
 */
int cargarYEjecutarPartidaCompleta(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente,TTF_Font* fuenteGrande, sUsuario* usuario, int indicePartida,sArchivo_conf configuracion)
{
    if (indicePartida < 0 || indicePartida >= MAX_PARTIDAS_GUARDADAS || !usuario->partidas[indicePartida].esValida) {
        return -1;
    }

    sPartidaGuardada* partida = &usuario->partidas[indicePartida];

    SDL_Color blanco = {255, 255, 255, 255};
    SDL_Color negro = {0, 0, 0, 255};
    SDL_Color azul = {30, 42, 75, 255};

    // Mostrar pantalla de carga
    SDL_SetRenderDrawColor(renderer, azul.r, azul.g, azul.b, 255);
    SDL_RenderClear(renderer);

    if (fuenteGrande)
        renderizarTextoCentrado(renderer, fuenteGrande, "CARGANDO PARTIDA...", 200, blanco, negro);

    char nombrePartida[150];
    snprintf(nombrePartida, sizeof(nombrePartida), "Cargando: %s", partida->nombre);
    if (fuente)
        renderizarTextoCentrado(renderer, fuente, nombrePartida, 280, blanco, negro);

    char detallesPartida[150];
    snprintf(detallesPartida, sizeof(detallesPartida), "Tablero: %dx%d | Minas: %d",
             partida->dimensiones, partida->dimensiones, partida->cantMinas);
    if (fuente)
        renderizarTextoCentrado(renderer, fuente, detallesPartida, 320, blanco, negro);

    SDL_RenderPresent(renderer);
    SDL_Delay(1500);

    // Variables para cargar la partida
    sCelda** matriz = NULL;
    sArchivo_conf configCargada;
    int minasRestantes;
    int tiempoTranscurrido;
    int primerClic;

    // Cargar la partida completa
    int resultado = cargarPartidaCompleta(usuario, indicePartida, &matriz, &configCargada,
                                         &minasRestantes, &tiempoTranscurrido, &primerClic);

    if (resultado != 0 || !matriz) {
        printf("Error al cargar la partida\n");

        // Mostrar mensaje de error
        SDL_SetRenderDrawColor(renderer, azul.r, azul.g, azul.b, 255);
        SDL_RenderClear(renderer);

        if (fuenteGrande)
            renderizarTextoCentrado(renderer, fuenteGrande, "ERROR AL CARGAR", 250, blanco, negro);
        if (fuente)
            renderizarTextoCentrado(renderer, fuente, "No se pudo cargar la partida", 300, blanco, negro);

        SDL_RenderPresent(renderer);
        SDL_Delay(2000);

        if (matriz) {
            destruirMatriz(matriz, configCargada.dimensiones);
        }
        return -1;
    }

    // Ejecutar la partida cargada
    ejecutarPartidaCargada(ventana, renderer, fuente, fuenteGrande, usuario, matriz,
                          configCargada, minasRestantes, tiempoTranscurrido, primerClic);

    // Limpiar recursos
    destruirMatriz(matriz, configCargada.dimensiones);

    return 0;
}
