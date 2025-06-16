#include "common.h"
#include "usuario.h"
#include "tablero.h"
#include "interfaz.h"
#include "juego.h"

/**
 * Crea un nuevo usuario con valores por defecto inicializados
 * @param nombre - Nombre del nuevo usuario
 * @param usuario - Puntero a la estructura donde guardar los datos del usuario
 */
void crearNuevoUsuario(const char* nombre, sUsuario* usuario)
{
    if (!usuario || !nombre)
        return;

    strcpy(usuario->nombre, nombre);
    usuario->fechaCreacion = time(NULL);
    usuario->ultimoAcceso = time(NULL);

    inicializarEstadisticas(&usuario->estadisticas);

    // Inicializar partidas guardadas - ciclo para limpiar todos los slots
    for (int i = 0; i < MAX_PARTIDAS_GUARDADAS; i++)
    {
        usuario->partidas[i].esValida = 0;
        strcpy(usuario->partidas[i].nombre, "");
    }
}

/**
 * Carga los datos de un usuario desde archivo o crea uno nuevo si no existe
 * @param nombre - Nombre del usuario a cargar
 * @param usuario - Puntero donde almacenar los datos del usuario
 * @return 0 si es un usuario nuevo, 1 si se cargó un usuario existente, -1 si hay error
 */
int cargarUsuario(const char* nombre, sUsuario* usuario)
{
    if (!usuario || !nombre)
        return -1;

    FILE* archivo = fopen(ARCHIVO_USUARIOS, "rb");
    if (!archivo)
    {
        // Si no existe el archivo, crear usuario nuevo
        crearNuevoUsuario(nombre, usuario);
        return 0; // sUsuario nuevo creado
    }

    sUsuario temp;
    int encontrado = 0;

    // Ciclo para buscar el usuario en el archivo
    while (fread(&temp, sizeof(sUsuario), 1, archivo) == 1)
    {
        if (strcmp(temp.nombre, nombre) == 0)
        {
            *usuario = temp;
            usuario->ultimoAcceso = time(NULL);
            encontrado = 1;
            break;
        }
    }

    fclose(archivo);

    if (!encontrado)
    {
        // sUsuario no encontrado, crear nuevo
        crearNuevoUsuario(nombre, usuario);
        return 0;
    }

    return 1; // sUsuario existente cargado
}

/**
 * Guarda los datos de un usuario en el archivo de usuarios
 * @param usuario - Puntero al usuario a guardar
 * @return 0 si es exitoso, -1 si hay error
 */
int guardarUsuario(sUsuario* usuario)
{
    if (!usuario)
        return -1;

    FILE* archivo = fopen(ARCHIVO_USUARIOS, "rb");
    sUsuario* usuarios = NULL;
    int numsUsuarios = 0;
    int usuarioExistente = -1;

    // Leer todos los usuarios existentes
    if (archivo)
    {
        fseek(archivo, 0, SEEK_END);
        long tam = ftell(archivo);
        numsUsuarios = tam / sizeof(sUsuario);
        fseek(archivo, 0, SEEK_SET);

        if (numsUsuarios > 0)
        {
            usuarios = malloc(sizeof(sUsuario) * numsUsuarios);
            if (usuarios)
            {
                fread(usuarios, sizeof(sUsuario), numsUsuarios, archivo);

                // Buscar si el usuario ya existe - ciclo para verificar usuarios existentes
                for (int i = 0; i < numsUsuarios; i++)
                {
                    if (strcmp(usuarios[i].nombre, usuario->nombre) == 0)
                    {
                        usuarioExistente = i;
                        break;
                    }
                }
            }
        }
        fclose(archivo);
    }

    // Escribir archivo actualizado
    archivo = fopen(ARCHIVO_USUARIOS, "wb");
    if (!archivo)
    {
        if (usuarios) free(usuarios);
        return -1;
    }

    if (usuarioExistente >= 0)
    {
        // Actualizar usuario existente
        usuarios[usuarioExistente] = *usuario;
        fwrite(usuarios, sizeof(sUsuario), numsUsuarios, archivo);
    }
    else
    {
        // Escribir usuarios existentes y agregar el nuevo
        if (usuarios && numsUsuarios > 0)
            fwrite(usuarios, sizeof(sUsuario), numsUsuarios, archivo);
        fwrite(usuario, sizeof(sUsuario), 1, archivo);
    }

    fclose(archivo);
    if (usuarios) free(usuarios);
    return 0;
}

/**
 * Verifica si un usuario existe en el archivo de usuarios
 * @param nombre - Nombre del usuario a verificar
 * @return 1 si existe, 0 si no existe
 */
int usuarioExiste(const char* nombre)
{
    if (!nombre)
        return 0;

    FILE* archivo = fopen(ARCHIVO_USUARIOS, "rb");
    if (!archivo)
        return 0;

    sUsuario temp;
    // Ciclo para buscar el usuario en todo el archivo
    while (fread(&temp, sizeof(sUsuario), 1, archivo) == 1)
    {
        if (strcmp(temp.nombre, nombre) == 0)
        {
            fclose(archivo);
            return 1;
        }
    }

    fclose(archivo);
    return 0;
}

/**
 * Actualiza las estadísticas del usuario después de una partida
 * @param usuario - Puntero al usuario a actualizar
 * @param gano - 1 si ganó la partida, 0 si perdió
 * @param tiempoSegundos - Tiempo que duró la partida en segundos
 */
void actualizarEstadisticas(sUsuario* usuario, int gano, int tiempoSegundos)
{
    if (!usuario)
        return;

    usuario->estadisticas.partidasJugadas++;
    usuario->estadisticas.tiempoTotalJugado += tiempoSegundos;

    if (gano)
    {
        usuario->estadisticas.partidasGanadas++;
        usuario->estadisticas.rachaActual++;

        if (usuario->estadisticas.rachaActual > usuario->estadisticas.mejorRacha)
            usuario->estadisticas.mejorRacha = usuario->estadisticas.rachaActual;

        // Actualizar mejor tiempo si es la primera victoria o si es mejor que el anterior
        if (usuario->estadisticas.mejorTiempo == 0 || tiempoSegundos < usuario->estadisticas.mejorTiempo)
            usuario->estadisticas.mejorTiempo = tiempoSegundos;
    }
    else
    {
        usuario->estadisticas.partidasPerdidas++;
        usuario->estadisticas.rachaActual = 0;
    }
}

/**
 * Calcula el porcentaje de victorias del usuario
 * @param usuario - Puntero al usuario
 * @return Porcentaje de victorias (0.0 a 100.0)
 */
double calcularPorcentajeVictorias(sUsuario* usuario)
{
    if (!usuario || usuario->estadisticas.partidasJugadas == 0)
        return 0.0;

    return ((double)usuario->estadisticas.partidasGanadas / usuario->estadisticas.partidasJugadas) * 100.0;
}

/**
 * Guarda una partida en progreso en un slot del usuario
 * @param usuario - Puntero al usuario
 * @param nombrePartida - Nombre descriptivo para la partida
 * @param matriz - Matriz del estado actual del juego
 * @param config - Configuración del juego
 * @param minasRestantes - Minas restantes por marcar
 * @param tiempoTranscurrido - Tiempo jugado hasta ahora
 * @return Índice del slot donde se guardó, o -1 si no hay espacio
 */
int guardarPartida(sUsuario* usuario, const char* nombrePartida, sCelda** matriz, sArchivo_conf config, int minasRestantes, int tiempoTranscurrido)
{
    if (!usuario || !nombrePartida)
        return -1;

    int slot = buscarSlotLibre(usuario);
    if (slot == -1)
        return -1; // No hay slots disponibles

    sPartidaGuardada* partida = &usuario->partidas[slot];

    strncpy(partida->nombre, nombrePartida, sizeof(partida->nombre) - 1);
    partida->nombre[sizeof(partida->nombre) - 1] = '\0';
    partida->fechaGuardado = time(NULL);
    partida->dimensiones = config.dimensiones;
    partida->cantMinas = config.cantMinas;
    partida->minasRestantes = minasRestantes;
    partida->tiempoTranscurrido = tiempoTranscurrido;
    partida->esValida = 1;

    // Aquí podrías guardar también el estado de la matriz si es necesario
    // Por ahora solo guardamos la configuración básica

    return slot;
}

/**
 * Carga una partida guardada desde un slot del usuario (VERSIÓN ACTUALIZADA)
 * @param usuario - Puntero al usuario
 * @param indicePartida - Índice del slot de la partida a cargar
 * @param matriz - Puntero donde restaurar la matriz del juego
 * @param config - Puntero donde restaurar la configuración
 * @param minasRestantes - Puntero donde restaurar las minas restantes
 * @param tiempoTranscurrido - Puntero donde restaurar el tiempo transcurrido
 * @return 0 si es exitoso, -1 si hay error
 */
int cargarPartida(sUsuario* usuario, int indicePartida, sCelda*** matriz, sArchivo_conf* config, int* minasRestantes, int* tiempoTranscurrido)
{
    // Llamar a la función completa con una variable temporal para primerClic
    int primerClic;
    return cargarPartidaCompleta(usuario, indicePartida, matriz, config, minasRestantes, tiempoTranscurrido, &primerClic);
}


/**
 * Elimina una partida guardada de un slot específico
 * @param usuario - Puntero al usuario
 * @param indicePartida - Índice del slot a eliminar
 */
void eliminarPartida(sUsuario* usuario, int indicePartida)
{
    if (!usuario || indicePartida < 0 || indicePartida >= MAX_PARTIDAS_GUARDADAS)
        return;

    usuario->partidas[indicePartida].esValida = 0;
    strcpy(usuario->partidas[indicePartida].nombre, "");
}

/**
 * Busca el primer slot libre para guardar una partida
 * @param usuario - Puntero al usuario
 * @return Índice del primer slot libre, o -1 si no hay slots disponibles
 */
int buscarSlotLibre(sUsuario* usuario)
{
    if (!usuario)
        return -1;

    // Ciclo para buscar el primer slot libre
    for (int i = 0; i < MAX_PARTIDAS_GUARDADAS; i++)
    {
        if (!usuario->partidas[i].esValida)
            return i;
    }

    return -1; // No hay slots libres
}

/**
 * Valida si un nombre de usuario cumple con los criterios establecidos
 * @param nombre - Nombre a validar
 * @return 1 si es válido, 0 si no cumple los criterios
 */
int validarNombresUsuario(const char* nombre)
{
    if (!nombre)
        return 0;

    int longitud = strlen(nombre);

    // Verificar longitud válida
    if (longitud < 3 || longitud >= MAX_NOMBRE_USUARIO)
        return 0;

    // Verificar que todos los caracteres sean alfanuméricos o guion bajo
    // Ciclo para validar cada caracter del nombre
    for (int i = 0; i < longitud; i++)
    {
        char c = nombre[i];
        if (!isalnum(c) && c != '_')
            return 0;
    }

    return 1; // Nombre válido
}

/**
 * Formatea un tiempo en segundos al formato HH:MM:SS
 * @param segundos - Tiempo en segundos a formatear
 * @param buffer - Buffer donde escribir el tiempo formateado
 */
void formatearTiempo(int segundos, char* buffer)
{
    if (!buffer)
        return;

    int horas = segundos / 3600;
    int minutos = (segundos % 3600) / 60;
    int seg = segundos % 60;

    sprintf(buffer, "%02d:%02d:%02d", horas, minutos, seg);
}

/**
 * Inicializa una estructura de estadísticas con valores por defecto
 * @param stats - Puntero a la estructura de estadísticas a inicializar
 */
void inicializarEstadisticas(sEstadisticasUsuario* stats)
{
    if (!stats)
        return;

    stats->partidasJugadas = 0;
    stats->partidasGanadas = 0;
    stats->partidasPerdidas = 0;
    stats->mejorTiempo = 0;
    stats->tiempoTotalJugado = 0;
    stats->rachaActual = 0;
    stats->mejorRacha = 0;
}

/**
 * Carga una partida completa restaurando todo el estado del tablero
 * @param usuario - Puntero al usuario
 * @param indicePartida - Índice del slot de la partida a cargar
 * @param matriz - Puntero donde restaurar la matriz del juego
 * @param config - Puntero donde restaurar la configuración
 * @param minasRestantes - Puntero donde restaurar las minas restantes
 * @param tiempoTranscurrido - Puntero donde restaurar el tiempo transcurrido
 * @param primerClic - Puntero donde restaurar el estado del primer clic
 * @return 0 si es exitoso, -1 si hay error
 */
int cargarPartidaCompleta(sUsuario* usuario, int indicePartida, sCelda*** matriz,sArchivo_conf* config, int* minasRestantes, int* tiempoTranscurrido, int* primerClic)
{
    printf("\n=== CARGANDO PARTIDA ===\n");

    if (!usuario || indicePartida < 0 || indicePartida >= MAX_PARTIDAS_GUARDADAS) {
        printf("ERROR: Parámetros inválidos\n");
        return -1;
    }

    sPartidaGuardada* partida = &usuario->partidas[indicePartida];
    if (!partida->esValida) {
        printf("ERROR: Partida en slot %d no es válida\n", indicePartida);
        return -1;
    }

    printf("Cargando: '%s'\n", partida->nombre);
    printf("Dimensiones: %dx%d\n", partida->dimensiones, partida->dimensiones);
    printf("Minas totales: %d\n", partida->cantMinas);
    printf("Minas restantes: %d\n", partida->minasRestantes);
    printf("Tiempo transcurrido: %d seg\n", partida->tiempoTranscurrido);

    // Restaurar configuración básica
    config->dimensiones = partida->dimensiones;
    config->cantMinas = partida->cantMinas;
    *minasRestantes = partida->minasRestantes;
    *tiempoTranscurrido = partida->tiempoTranscurrido;
    *primerClic = partida->primerClic;

    // Destruir matriz anterior si existe
    if (*matriz) {
        printf("Destruyendo matriz anterior...\n");
        destruirMatriz(*matriz, config->dimensiones);
    }

    // Crear nueva matriz con las dimensiones correctas
    printf("Creando nueva matriz %dx%d...\n", config->dimensiones, config->dimensiones);
    *matriz = crearMatriz(config->dimensiones);
    if (!*matriz) {
        printf("ERROR: No se pudo crear la matriz\n");
        return -1;
    }

    // Restaurar estado completo del tablero
    int indice = 0;
    int minasRestauradas = 0;
    int celdasReveladas = 0;
    int banderasRestauradas = 0;

    for (int r = 0; r < config->dimensiones; r++) {
        for (int c = 0; c < config->dimensiones; c++) {
            sCelda* celdaDestino = *(*matriz + r) + c;
            *celdaDestino = partida->estadoTablero[indice];

            // Contar para verificación
            if (celdaDestino->tieneMina) minasRestauradas++;
            if (celdaDestino->esRevelada) celdasReveladas++;
            if (celdaDestino->tieneBandera) banderasRestauradas++;

            indice++;
        }
    }

    printf("Estado restaurado:\n");
    printf("- Minas en tablero: %d\n", minasRestauradas);
    printf("- Celdas reveladas: %d\n", celdasReveladas);
    printf("- Banderas colocadas: %d\n", banderasRestauradas);
    printf("========================\n");

    return 0;
}

/**
 * Ejecuta una partida cargada desde un estado guardado
 * @param ventana - Ventana SDL
 * @param renderer - Renderizador SDL
 * @param fuente - Fuente pequeña
 * @param fuenteGrande - Fuente grande
 * @param usuario - Usuario actual
 * @param matriz - Matriz con el estado cargado
 * @param config - Configuración de la partida
 * @param minasRestantes - Minas restantes
 * @param tiempoTranscurrido - Tiempo ya transcurrido
 * @param primerClic - Estado del primer clic
 */
void ejecutarPartidaCargada(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande,sUsuario* usuario, sCelda** matriz, sArchivo_conf config, int minasRestantes,int tiempoTranscurrido, int primerClic)
{
    // Ajustar tiempo de inicio para que el tiempo transcurrido sea correcto
    time_t tiempoInicioAjustado = time(NULL) - tiempoTranscurrido;

    // === REDIMENSIONAR VENTANA PARA EL JUEGO ===
    int ventana_ancho = config.dimensiones * PIXEL_CELDA;
    int ventana_alto = config.dimensiones * PIXEL_CELDA + ALTURA_HEADER;

    SDL_SetWindowSize(ventana, ventana_ancho, ventana_alto);
    SDL_SetWindowPosition(ventana, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    printf("Reanudando partida cargada...\n");
    printf("Tiempo ya transcurrido: %d segundos\n", tiempoTranscurrido);
    printf("Minas restantes: %d\n", minasRestantes);
    printf("Primer clic: %s\n", primerClic ? "No realizado" : "Ya realizado");

    // Usar la nueva función de juego con guardado
    jugarConGuardado(ventana, renderer, fuente, fuenteGrande, matriz, config,
                    &minasRestantes, ventana_ancho, usuario, tiempoInicioAjustado, &primerClic);

    // Al terminar la partida, calcular tiempo total y actualizar estadísticas
    time_t tiempoFin = time(NULL);
    int tiempoTotalPartida = (int)difftime(tiempoFin, tiempoInicioAjustado);

    int gano = verificarVictoria(matriz, config.dimensiones, config.cantMinas);
    actualizarEstadisticas(usuario, gano, tiempoTotalPartida);

    printf("Partida terminada. Tiempo total: %d segundos\n", tiempoTotalPartida);
}

void debugMostrarPartidasGuardadas(sUsuario* usuario) {
    printf("\n=== DEBUG: PARTIDAS GUARDADAS ===\n");
    printf("Usuario: %s\n", usuario->nombre);

    int partidasEncontradas = 0;
    for (int i = 0; i < MAX_PARTIDAS_GUARDADAS; i++) {
        if (usuario->partidas[i].esValida) {
            printf("Slot %d: '%s' - %dx%d - %d minas - %d seg\n", i, usuario->partidas[i].nombre,
               usuario->partidas[i].dimensiones,        // ← PRIMER %d
               usuario->partidas[i].dimensiones,        // ← SEGUNDO %d (AGREGAR)
               usuario->partidas[i].cantMinas,
               usuario->partidas[i].tiempoTranscurrido);
            partidasEncontradas++;
        } else {
            printf("Slot %d: VACIO\n", i);
        }
    }
    printf("Total partidas válidas: %d\n", partidasEncontradas);
    printf("================================\n\n");
}


/**
 * Guarda una partida completa con todo el estado del tablero
 * @param usuario - Puntero al usuario
 * @param nombrePartida - Nombre descriptivo para la partida
 * @param matriz - Matriz del estado actual del juego
 * @param config - Configuración del juego
 * @param minasRestantes - Minas restantes por marcar
 * @param tiempoTranscurrido - Tiempo jugado hasta ahora
 * @param primerClic - Estado del primer clic (1 si no se hizo, 0 si ya se hizo)
 * @return Índice del slot donde se guardó, o -1 si no hay espacio
 */
int guardarPartidaCompleta(sUsuario* usuario, const char* nombrePartida, sCelda** matriz,sArchivo_conf config, int minasRestantes, int tiempoTranscurrido, int primerClic)
{
    printf("\n=== GUARDANDO PARTIDA ===\n");
    printf("Nombre: %s\n", nombrePartida);
    printf("Dimensiones: %dx%d\n", config.dimensiones, config.dimensiones);
    printf("Minas totales: %d\n", config.cantMinas);
    printf("Minas restantes: %d\n", minasRestantes);
    printf("Tiempo: %d seg\n", tiempoTranscurrido);

    if (!usuario || !nombrePartida || !matriz) {
        printf("ERROR: Parámetros nulos\n");
        return -1;
    }

    // Verificar que las dimensiones no excedan el máximo
    if (config.dimensiones > MAX_DIMENSION) {
        printf("ERROR: Dimensiones (%d) exceden máximo (%d)\n",
               config.dimensiones, MAX_DIMENSION);
        return -1;
    }

    int slot = buscarSlotLibre(usuario);
    if (slot == -1) {
        printf("ERROR: No hay slots libres\n");
        return -1;
    }

    printf("Usando slot: %d\n", slot);

    sPartidaGuardada* partida = &usuario->partidas[slot];

    // Guardar información básica
    strncpy(partida->nombre, nombrePartida, sizeof(partida->nombre) - 1);
    partida->nombre[sizeof(partida->nombre) - 1] = '\0';
    partida->fechaGuardado = time(NULL);
    partida->dimensiones = config.dimensiones;
    partida->cantMinas = config.cantMinas;
    partida->minasRestantes = minasRestantes;
    partida->tiempoTranscurrido = tiempoTranscurrido;
    partida->primerClic = primerClic;
    partida->esValida = 1;

    // Limpiar el array de estado antes de llenar
    for (int i = 0; i < MAX_DIMENSION * MAX_DIMENSION; i++) {
        partida->estadoTablero[i].esRevelada = 0;
        partida->estadoTablero[i].tieneMina = 0;
        partida->estadoTablero[i].tieneBandera = 0;
        partida->estadoTablero[i].minasAdyacentes = 0;
    }

    // Guardar estado completo del tablero
    int indice = 0;
    int minasGuardadas = 0;
    int celdasReveladas = 0;
    int banderasGuardadas = 0;

    for (int r = 0; r < config.dimensiones; r++) {
        for (int c = 0; c < config.dimensiones; c++) {
            sCelda* celdaOriginal = *(matriz + r) + c;
            partida->estadoTablero[indice] = *celdaOriginal;

            // Contar para verificación
            if (celdaOriginal->tieneMina) minasGuardadas++;
            if (celdaOriginal->esRevelada) celdasReveladas++;
            if (celdaOriginal->tieneBandera) banderasGuardadas++;

            indice++;
        }
    }

    printf("Estado guardado:\n");
    printf("- Minas en tablero: %d\n", minasGuardadas);
    printf("- Celdas reveladas: %d\n", celdasReveladas);
    printf("- Banderas colocadas: %d\n", banderasGuardadas);
    printf("=========================\n");

    return slot;
}




















