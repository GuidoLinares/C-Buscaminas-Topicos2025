#include "headers.h"


void contarMinasAdyacentes(s_celdas** matriz, int dimension, int fila, int columna)
{
    s_celdas* celdaActual = *(matriz + fila) + columna;

    if (celdaActual->tieneMina)
        return;

    int contador = 0;
    for (int deltaFila = -1; deltaFila <= 1; deltaFila++)
    {
        for (int deltaColumna = -1; deltaColumna <= 1; deltaColumna++)
        {
            if (deltaFila == 0 && deltaColumna == 0)
                continue;

            int nuevaFila = fila + deltaFila;
            int nuevaColumna = columna + deltaColumna;

            if (nuevaFila >= 0 && nuevaFila < dimension && nuevaColumna >= 0 && nuevaColumna < dimension)
            {
                s_celdas* celdaVecina = *(matriz + nuevaFila) + nuevaColumna;

                if (celdaVecina->tieneMina)
                    contador++;

            }
        }
    }
    celdaActual->minasAdyacentes = contador;
}

s_celdas** crearMatriz (int dimension)
{
    s_celdas** matriz = (s_celdas**)malloc(sizeof(s_celdas*) * dimension);
    if (!matriz) {
        return NULL;
    }

    s_celdas** punteroFilaActual = matriz;
    s_celdas** punteroFilaFin = matriz + dimension;

    for( ; punteroFilaActual < punteroFilaFin ; punteroFilaActual++)
    {
        *punteroFilaActual = (s_celdas*)malloc(sizeof(s_celdas) * dimension);
        if(!*punteroFilaActual)
        {

            for (s_celdas** punteroLimpieza = matriz ; punteroLimpieza < punteroFilaActual; punteroLimpieza++)
                free(*punteroLimpieza);

            free(matriz);
            return NULL;
        }
    }
    return matriz;
}

void destruirMatriz(s_celdas** matriz, int tamano)
{
    if (!matriz) return;

    s_celdas** punteroFilaActual = matriz;
    s_celdas** punteroFilaFin = matriz + tamano;

    for( ; punteroFilaActual < punteroFilaFin ; punteroFilaActual++) {
        free(*punteroFilaActual);
    }
    free(matriz);
}

void inicializarMatriz(s_celdas** matriz, int dimension)
{
    s_celdas** punteroFila = matriz;
    s_celdas** punteroFilaFin = matriz + dimension;

    for ( ; punteroFila < punteroFilaFin; punteroFila++) {
        s_celdas* punteroColumna = *punteroFila;
        s_celdas* punteroColumnaFin = *punteroFila + dimension;

        for ( ; punteroColumna < punteroColumnaFin; punteroColumna++) {
            punteroColumna->esRevelada = 0;
            punteroColumna->tieneMina = 0;
            punteroColumna->tieneBandera = 0;
            punteroColumna->minasAdyacentes = 0;
        }
    }
}

int generarAleatorio(int minimo, int maximo)
{
    return rand() % (maximo - minimo + 1) + minimo;
}

void llenarMatriz(s_celdas** matriz, Archivo_conf configuracion)
{
    srand(time(NULL));

    size_t minasColocadas = 0;
    while (minasColocadas < configuracion.minas) {
        int fila = generarAleatorio(0, configuracion.dimensiones - 1);
        int columna = generarAleatorio(0, configuracion.dimensiones - 1);

        s_celdas* celdaObjetivo = *(matriz + fila) + columna;

        if (celdaObjetivo->tieneMina == 0) {
            celdaObjetivo->tieneMina = 1;
            minasColocadas++;
        }
    }

    for (int r = 0; r < configuracion.dimensiones; r++) {
        for (int c = 0; c < configuracion.dimensiones; c++) {
            contarMinasAdyacentes(matriz, configuracion.dimensiones, r, c);
        }
    }
}

void mostrarMatriz(s_celdas** matriz, int dimension)
{
    printf("\n--- Estado de la Matriz (Consola) ---\n");
    s_celdas** punteroFila = matriz;
    s_celdas** punteroFilaFin = matriz + dimension;

    for( ; punteroFila < punteroFilaFin ; punteroFila++) {
        s_celdas* punteroColumna = *punteroFila;
        s_celdas* punteroColumnaFin = *punteroFila + dimension;

        for( ; punteroColumna < punteroColumnaFin ; punteroColumna++ ) {
            if (punteroColumna->tieneMina) {
                printf("[M]");
            } else {
                printf("[%d]", punteroColumna->minasAdyacentes);
            }
        }
        printf("\n");
    }
    printf("---------------------------------------------\n");
}

void trim(char* cadena)
{
    char* fin;
    while (isspace((unsigned char)*cadena))
        cadena++;

    if (*cadena == 0)
        return;

    fin = cadena + strlen(cadena) - 1;
    while (fin > cadena && isspace((unsigned char)*fin))
        fin--;

    *(fin + 1) = '\0';
}

Archivo_conf leerArchivo()
{

    Archivo_conf configuracion;
    char linea[MAX_LINEA];
    char minasCadena[20];
    int dimensionesLeidas;

    FILE* archivo;
    archivo = fopen(ARCH_CONFIG, "r");
    if(!archivo) {
        fprintf(stderr, "ERROR: NO SE PUDO ABRIR EL ARCHIVO DE CONFIGURACION: %s\n", ARCH_CONFIG);
        exit(-1);
    }

    if (!fgets(linea, MAX_LINEA, archivo)) {
        fprintf(stderr, "ERROR: No se pudo leer la línea del archivo de configuracion.\n");
        fclose(archivo);
        exit(-2);
    }

    fclose(archivo);

    if (sscanf(linea, FORMATO, minasCadena, &dimensionesLeidas) != 2) {
        fprintf(stderr, "ERROR: Formato incorrecto en el archivo de configuración.\n");
        exit(-3);
    }

    trim(minasCadena);
    configuracion.dimensiones = dimensionesLeidas;

    int longitud = strlen(minasCadena);
    if (longitud > 0 && minasCadena[longitud - 1] == '%') {
        minasCadena[longitud - 1] = '\0';
        trim(minasCadena);
        int porcentaje = atoi(minasCadena);
        if (porcentaje < 0 || porcentaje > 100) {
            fprintf(stderr, "ERROR: Porcentaje de minas inválido: %d%%\n", porcentaje);
            exit(-4);
        }
        int totalCasillas = configuracion.dimensiones * configuracion.dimensiones;
        configuracion.minas = (porcentaje * totalCasillas) / 100;
    } else {
        configuracion.minas = atoi(minasCadena);
    }

    if (configuracion.minas < 0 || configuracion.minas > configuracion.dimensiones * configuracion.dimensiones) {
        fprintf(stderr, "ERROR: Cantidad de minas inválida: %d (Fuera de rango para %dx%d).\n", configuracion.minas, configuracion.dimensiones, configuracion.dimensiones);
        exit(-5);
    }
    printf("Configuracion leida: Dimensiones=%d, Minas=%d\n", configuracion.dimensiones, configuracion.minas);
    return configuracion;
}


//FUNCIONES LOG
char* obtenerTimestamp()
{
    char buffer[80];
    time_t tiempoActual;
    struct tm* infoTiempo;

    time(&tiempoActual);
    infoTiempo = localtime(&tiempoActual);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", infoTiempo);

    return buffer;
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
             configuracion.minas, configuracion.dimensiones * configuracion.dimensiones);

    escribirEvento(mensaje);
    free(mensaje);
}

void logClickCelda(s_celdas** matriz, int fila, int columna, int dimensiones, const char* tipoClick)
{
    if (!g_sistemaLog || fila < 0 || fila >= dimensiones || columna < 0 || columna >= dimensiones)
        return;

    s_celdas* celdaClickeada = *(matriz + fila) + columna;

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

void logRevelarCelda(s_celdas** matriz, int fila, int columna, int dimensiones)
{
    if (!g_sistemaLog)
        return;

    s_celdas* celda = *(matriz + fila) + columna;

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
void logBandera(s_celdas** matriz, int fila, int columna, int dimensiones, int colocada)
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
             ARCH_CONFIG, config.dimensiones, config.minas);

    escribirEvento(mensaje);
    free(mensaje);
}

// Log de estado del tablero (para debugging)
void logEstadoTablero(s_celdas** matriz, int dimensiones)
{
    if (!g_sistemaLog)
        return;

    int celdasReveladas = 0;
    int banderasColocadas = 0;
    int minasReveladas = 0;

    s_celdas** punteroFila = matriz;
    s_celdas** punteroFilaFin = matriz + dimensiones;

    for (; punteroFila < punteroFilaFin; punteroFila++) {
        s_celdas* punteroColumna = *punteroFila;
        s_celdas* punteroColumnaFin = *punteroFila + dimensiones;

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
