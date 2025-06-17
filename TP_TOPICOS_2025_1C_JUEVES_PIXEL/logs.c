#include "common.h"
#include "logs.h"

static sSistemaLog* g_sistemaLog = NULL;

/**
 * Obtiene el timestamp actual en formato "YYYY-MM-DD HH:MM:SS"
 * @return Puntero a string con el timestamp formateado
 */
char* obtenerTimestamp()
{
    static char buffer[80];  // Buffer estatico - persiste entre llamadas
    time_t tiempoActual;
    struct tm* infoTiempo;

    time(&tiempoActual);
    infoTiempo = localtime(&tiempoActual);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", infoTiempo);

    return buffer;  // Seguro porque es estatico
}

/**
 * Escribe un evento al archivo de log con timestamp
 * @param evento - Mensaje del evento a escribir
 */
void escribirEvento(const char* evento)
{
    if (!g_sistemaLog || !g_sistemaLog->archivo)
        return;

    char* timestamp = obtenerTimestamp();
    fprintf(g_sistemaLog->archivo, "[%s] %s\n", timestamp, evento);
    fflush(g_sistemaLog->archivo); // Asegurar escritura inmediata
}

/**
 * Inicializa el sistema de logging, creando/abriendo el archivo de log
 * @param nombreArchivo - Nombre del archivo donde guardar los logs
 * @return Puntero al sistema de log inicializado o NULL si hay error
 */
sSistemaLog* inicializarLog(const char* nombreArchivo)
{
    // Asignar memoria para el archivo
    g_sistemaLog = (sSistemaLog*)malloc(sizeof(sSistemaLog));
    if (!g_sistemaLog)
        return NULL;

    // Asignar memoria para nombre del archivo
    size_t longitud = strlen(nombreArchivo) + 1;
    g_sistemaLog->nombreArchivo = (char*)malloc(longitud * sizeof(char));
    if (!g_sistemaLog->nombreArchivo)
    {
        free(g_sistemaLog);
        return NULL;
    }
    strcpy(g_sistemaLog->nombreArchivo, nombreArchivo);

    // Abrir archivo en modo append (agregar sobre el final)
    g_sistemaLog->archivo = fopen(nombreArchivo, "a");
    if (!g_sistemaLog->archivo)
    {
        free(g_sistemaLog->nombreArchivo);
        free(g_sistemaLog);
        return NULL;
    }

    // Guardar tiempo de inicio
    time(&g_sistemaLog->tiempoInicio);

    // Escribir header inicial
    escribirEvento("=== BUSCAMINAS - NUEVA SESION ===");

    return g_sistemaLog;
}

/**
 * Destruye el sistema de logging y libera todos los recursos
 */
void destruirLog()
{
    if (!g_sistemaLog)
        return;

    escribirEvento("=== FIN DE SESION ===");

    if (g_sistemaLog->archivo)
        fclose(g_sistemaLog->archivo);

    if (g_sistemaLog->nombreArchivo)
        free(g_sistemaLog->nombreArchivo);

    free(g_sistemaLog);
    g_sistemaLog = NULL;
}

/**
 * Registra el inicio de una nueva partida con la configuracion
 * @param configuracion - Configuracion de la partida (dimensiones, minas)
 */
void logInicioPartida(sArchivo_conf configuracion)
{
    if (!g_sistemaLog)
        return;

    char* mensaje = (char*)malloc(200 * sizeof(char));

    if (!mensaje)
        return;

    snprintf(mensaje, 200,
             "INICIO PARTIDA - Dimensiones: %dx%d, Minas: %d, Total celdas: %d",
             configuracion.dimensiones, configuracion.dimensiones,
             configuracion.cantMinas, configuracion.dimensiones * configuracion.dimensiones);

    escribirEvento(mensaje);
    free(mensaje);
}

/**
 * Registra un clic en una celda del tablero
 * @param matriz - Matriz de celdas del tablero
 * @param fila - Fila de la celda clickeada
 * @param columna - Columna de la celda clickeada
 * @param dimensiones - Dimensiones del tablero
 * @param tipoClick - Tipo de clic ("IZQUIERDO" o "DERECHO")
 */
void logClickCelda(sCelda** matriz, int fila, int columna, int dimensiones, const char* tipoClick)
{
    if (!g_sistemaLog || fila < 0 || fila >= dimensiones || columna < 0 || columna >= dimensiones)
        return;

    sCelda* celdaClickeada = *(matriz + fila) + columna;

    char* mensaje = (char*)malloc(250 * sizeof(char));
    if (!mensaje) return;

    snprintf(mensaje, 250,
             "CLICK %s en (%d,%d) - Mina:%s, Revelada:%s, Bandera:%s, Adyacentes:%d",
             tipoClick, fila, columna,
             celdaClickeada->tieneMina ? "SI" : "NO",
             celdaClickeada->esRevelada ? "SI" : "NO",
             celdaClickeada->tieneBandera ? "SI" : "NO",
             celdaClickeada->minasAdyacentes);

    escribirEvento(mensaje);
    free(mensaje);
}

/**
 * Registra cuando una celda es revelada
 * @param matriz - Matriz de celdas del tablero
 * @param fila - Fila de la celda revelada
 * @param columna - Columna de la celda revelada
 * @param dimensiones - Dimensiones del tablero
 */
void logRevelarCelda(sCelda** matriz, int fila, int columna, int dimensiones)
{
    if (!g_sistemaLog)
        return;

    sCelda* celda = *(matriz + fila) + columna;

    char* mensaje = (char*)malloc(150 * sizeof(char));
    if (!mensaje)
        return;

    if (celda->tieneMina)
        snprintf(mensaje, 150, "BOOM! Mina detonada en (%d,%d)", fila, columna);

    else
        snprintf(mensaje, 150, "Celda revelada (%d,%d) - %d minas adyacentes",
                fila, columna, celda->minasAdyacentes);

    escribirEvento(mensaje);
    free(mensaje);
}

/**
 * Registra cuando se coloca o quita una bandera
 * @param matriz - Matriz de celdas del tablero
 * @param fila - Fila donde se modifico la bandera
 * @param columna - Columna donde se modifico la bandera
 * @param dimensiones - Dimensiones del tablero
 * @param colocada - 1 si se coloco la bandera, 0 si se quito
 */
void logBandera(sCelda** matriz, int fila, int columna, int dimensiones, int colocada)
{
    if (!g_sistemaLog)
        return;

    char* mensaje = (char*)malloc(100 * sizeof(char));

    if (!mensaje)
        return;

    snprintf(mensaje, 100, "BANDERA %s en (%d,%d)",
             colocada ? "COLOCADA" : "REMOVIDA", fila, columna);

    escribirEvento(mensaje);
    free(mensaje);
}

/**
 * Registra el final de una partida con el resultado
 * @param resultado - String describiendo el resultado ("VICTORIA", "DERROTA", etc.)
 */
void logFinPartida(const char* resultado)
{
    if (!g_sistemaLog)
        return;

    time_t tiempoActual;
    time(&tiempoActual);
    int tiempoTranscurrido = (int)difftime(tiempoActual, g_sistemaLog->tiempoInicio);

    char* mensaje = (char*)malloc(150 * sizeof(char));

    if (!mensaje)
        return;

    snprintf(mensaje, 150, "FIN PARTIDA - Resultado: %s, Tiempo: %d segundos",
             resultado, tiempoTranscurrido);

    escribirEvento(mensaje);
    free(mensaje);
}

/**
 * Registra la configuracion leida del archivo
 * @param config - Estructura con la configuracion cargada
 */
void logConfiguracion(sArchivo_conf config)
{
    if (!g_sistemaLog)
        return;

    char* mensaje = (char*)malloc(150 * sizeof(char));
    if (!mensaje)
        return;

    snprintf(mensaje, 150, "CONFIGURACION CARGADA - Archivo: %s, Dim: %d, Minas: %d",
             ARCH_CONFIG, config.dimensiones, config.cantMinas);

    escribirEvento(mensaje);
    free(mensaje);
}

/**
 * Registra el estado actual del tablero para debugging
 * @param matriz - Matriz de celdas del tablero
 * @param dimensiones - Dimensiones del tablero
 */
void logEstadoTablero(sCelda** matriz, int dimensiones)
{
    if (!g_sistemaLog)
        return;

    int celdasReveladas = 0;
    int banderasColocadas = 0;
    int minasReveladas = 0;

    sCelda** punteroFila = matriz;
    sCelda** punteroFilaFin = matriz + dimensiones;

    // Ciclo para recorrer todas las filas del tablero
    for (; punteroFila < punteroFilaFin; punteroFila++)
    {
        sCelda* punteroColumna = *punteroFila;
        sCelda* punteroColumnaFin = *punteroFila + dimensiones;

        // Ciclo para recorrer todas las columnas de la fila actual
        for (; punteroColumna < punteroColumnaFin; punteroColumna++)
        {
            if (punteroColumna->esRevelada)
                celdasReveladas++;
            if (punteroColumna->tieneBandera)
                banderasColocadas++;
            if (punteroColumna->esRevelada && punteroColumna->tieneMina)
                minasReveladas++;
        }
    }

    char* mensaje = (char*)malloc(200 * sizeof(char));

    if (!mensaje)
        return;

    snprintf(mensaje, 200,
             "ESTADO TABLERO - Reveladas: %d, Banderas: %d, Minas reveladas: %d",
             celdasReveladas, banderasColocadas, minasReveladas);

    escribirEvento(mensaje);
    free(mensaje);
}
