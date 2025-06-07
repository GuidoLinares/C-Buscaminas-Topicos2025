#include "common.h"
#include "logs.h"

static SistemaLog* g_sistemaLog = NULL;

char* obtenerTimestamp()
{
    static char buffer[80];  // Buffer estático - persiste entre llamadas
    time_t tiempoActual;
    struct tm* infoTiempo;

    time(&tiempoActual);
    infoTiempo = localtime(&tiempoActual);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", infoTiempo);

    return buffer;  // Seguro porque es estático
}

void escribirEvento(const char* evento)
{
    if (!g_sistemaLog || !g_sistemaLog->archivo)
        return;

    char* timestamp = obtenerTimestamp();
    fprintf(g_sistemaLog->archivo, "[%s] %s\n", timestamp, evento);
    fflush(g_sistemaLog->archivo); // Asegurar escritura inmediata
}

SistemaLog* inicializarLog(const char* nombreArchivo)
{

    g_sistemaLog = (SistemaLog*)malloc(sizeof(SistemaLog));
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

    // Abrir archivo en modo append
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

void destruirLog()
{

    if (!g_sistemaLog)
        return;

    escribirEvento("=== FIN DE SESION ===");

    if (g_sistemaLog->archivo) {
        fclose(g_sistemaLog->archivo);
    }

    if (g_sistemaLog->nombreArchivo) {
        free(g_sistemaLog->nombreArchivo);
    }

    free(g_sistemaLog);
    g_sistemaLog = NULL;
}

void logInicioPartida(Archivo_conf configuracion)
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

void logRevelarCelda(sCelda** matriz, int fila, int columna, int dimensiones)
{
    if (!g_sistemaLog)
        return;

    sCelda* celda = *(matriz + fila) + columna;

    char* mensaje = (char*)malloc(150 * sizeof(char));
    if (!mensaje)
        return;

    if (celda->tieneMina)
    {
        snprintf(mensaje, 150, "BOOM! Mina detonada en (%d,%d)", fila, columna);
    } else {
        snprintf(mensaje, 150, "Celda revelada (%d,%d) - %d minas adyacentes",
                fila, columna, celda->minasAdyacentes);
    }

    escribirEvento(mensaje);
    free(mensaje);
}

// Log para banderas
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

// Log de fin de partida
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

// Log de configuración leída
void logConfiguracion(Archivo_conf config)
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

// Log de estado del tablero (para debugging)
void logEstadoTablero(sCelda** matriz, int dimensiones)
{
    if (!g_sistemaLog)
        return;

    int celdasReveladas = 0;
    int banderasColocadas = 0;
    int minasReveladas = 0;

    sCelda** punteroFila = matriz;
    sCelda** punteroFilaFin = matriz + dimensiones;

    for (; punteroFila < punteroFilaFin; punteroFila++) {
        sCelda* punteroColumna = *punteroFila;
        sCelda* punteroColumnaFin = *punteroFila + dimensiones;

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




