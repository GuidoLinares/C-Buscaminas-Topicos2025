#include "common.h"
#include "tablero.h"

/**
 * Cuenta el número de minas adyacentes a una celda específica
 * @param matriz - Matriz de celdas del tablero
 * @param dimension - Dimensiones del tablero (NxN)
 * @param fila - Fila de la celda a analizar
 * @param columna - Columna de la celda a analizar
 */
void contarMinasAdyacentes(sCelda** matriz, int dimension, int fila, int columna)
{
    sCelda* celdaActual = *(matriz + fila) + columna;

    if (celdaActual->tieneMina)
        return;

    int contador = 0;
    // Ciclo para revisar las 8 direcciones alrededor de la celda actual
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
                sCelda* celdaVecina = *(matriz + nuevaFila) + nuevaColumna;

                if (celdaVecina->tieneMina)
                    contador++;
            }
        }
    }
    celdaActual->minasAdyacentes = contador;
}

/**
 * Crea y reserva memoria para una matriz de celdas del tablero
 * @param dimension - Dimensiones del tablero (NxN)
 * @return Puntero a la matriz creada o NULL si hay error de memoria
 */
sCelda** crearMatriz (int dimension)
{
    sCelda** matriz = (sCelda**)malloc(sizeof(sCelda*) * dimension);
    if (!matriz) {
        return NULL;
    }

    sCelda** punteroFilaActual = matriz;
    sCelda** punteroFilaFin = matriz + dimension;

    // Ciclo para crear cada fila de la matriz
    for( ; punteroFilaActual < punteroFilaFin ; punteroFilaActual++)
    {
        *punteroFilaActual = (sCelda*)malloc(sizeof(sCelda) * dimension);
        if(!*punteroFilaActual)
        {
            // Si falla, liberar toda la memoria ya asignada
            for (sCelda** punteroLimpieza = matriz ; punteroLimpieza < punteroFilaActual; punteroLimpieza++)
                free(*punteroLimpieza);

            free(matriz);
            return NULL;
        }
    }
    return matriz;
}

/**
 * Libera toda la memoria ocupada por la matriz del tablero
 * @param matriz - Matriz a destruir
 * @param tamano - Número de filas de la matriz
 */
void destruirMatriz(sCelda** matriz, int tamano)
{
    if (!matriz) return;

    sCelda** punteroFilaActual = matriz;
    sCelda** punteroFilaFin = matriz + tamano;

    // Ciclo para liberar cada fila de la matriz
    for( ; punteroFilaActual < punteroFilaFin ; punteroFilaActual++) {
        free(*punteroFilaActual);
    }
    free(matriz);
}

/**
 * Inicializa todas las celdas de la matriz con valores por defecto
 * @param matriz - Matriz de celdas a inicializar
 * @param dimension - Dimensiones del tablero
 */
void inicializarMatriz(sCelda** matriz, int dimension)
{
    sCelda** punteroFila = matriz;
    sCelda** punteroFilaFin = matriz + dimension;

    // Ciclo para recorrer todas las filas
    for ( ; punteroFila < punteroFilaFin; punteroFila++) {
        sCelda* punteroColumna = *punteroFila;
        sCelda* punteroColumnaFin = *punteroFila + dimension;

        // Ciclo para recorrer todas las columnas de la fila actual
        for ( ; punteroColumna < punteroColumnaFin; punteroColumna++) {
            punteroColumna->esRevelada = 0;
            punteroColumna->tieneMina = 0;
            punteroColumna->tieneBandera = 0;
            punteroColumna->minasAdyacentes = 0;
        }
    }
}

/**
 * Genera un número aleatorio dentro de un rango específico
 * @param minimo - Valor mínimo del rango (inclusivo)
 * @param maximo - Valor máximo del rango (inclusivo)
 * @return Número aleatorio dentro del rango especificado
 */
int generarAleatorio(int minimo, int maximo)
{
    return rand() % (maximo - minimo + 1) + minimo;
}

/**
 * Llena la matriz con minas en posiciones aleatorias y calcula las minas adyacentes
 * @param matriz - Matriz de celdas del tablero
 * @param configuracion - Configuración con dimensiones y cantidad de minas
 */
void llenarMatriz(sCelda** matriz, sArchivo_conf configuracion)
{
    srand(time(NULL));

    size_t minasColocadas = 0;
    // Ciclo para colocar todas las minas en posiciones aleatorias
    while (minasColocadas < configuracion.cantMinas) {
        int fila = generarAleatorio(0, configuracion.dimensiones - 1);
        int columna = generarAleatorio(0, configuracion.dimensiones - 1);

        sCelda* celdaObjetivo = *(matriz + fila) + columna;

        if (celdaObjetivo->tieneMina == 0) {
            celdaObjetivo->tieneMina = 1;
            minasColocadas++;
        }
    }

    // Calcular minas adyacentes para todas las celdas
    // Ciclo para recorrer todas las filas
    for (int r = 0; r < configuracion.dimensiones; r++) {
        // Ciclo para recorrer todas las columnas
        for (int c = 0; c < configuracion.dimensiones; c++) {
            contarMinasAdyacentes(matriz, configuracion.dimensiones, r, c);
        }
    }
}

/**
 * Muestra el estado actual de la matriz en la consola (para debug)
 * @param matriz - Matriz de celdas a mostrar
 * @param dimension - Dimensiones del tablero
 */
void mostrarMatriz(sCelda** matriz, int dimension)
{
    printf("\n--- Estado de la Matriz (Consola) ---\n");
    sCelda** punteroFila = matriz;
    sCelda** punteroFilaFin = matriz + dimension;

    // Ciclo para recorrer todas las filas
    for( ; punteroFila < punteroFilaFin ; punteroFila++) {
        sCelda* punteroColumna = *punteroFila;
        sCelda* punteroColumnaFin = *punteroFila + dimension;

        // Ciclo para recorrer todas las columnas de la fila actual
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

/**
 * Elimina espacios en blanco al inicio y final de una cadena
 * @param cadena - Cadena de caracteres a limpiar
 */
void trim(char* cadena)
{
    char* fin;
    // Eliminar espacios al inicio
    while (isspace((unsigned char)*cadena))
        cadena++;

    if (*cadena == 0)
        return;

    // Eliminar espacios al final
    fin = cadena + strlen(cadena) - 1;
    while (fin > cadena && isspace((unsigned char)*fin))
        fin--;

    *(fin + 1) = '\0';
}

/**
 * Lee la configuración del juego desde un archivo de texto
 * @return Estructura con la configuración leída (dimensiones y cantidad de minas)
 */
sArchivo_conf leerArchivo()
{
    sArchivo_conf configuracion;
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
        configuracion.cantMinas = (porcentaje * totalCasillas) / 100;
    } else {
        configuracion.cantMinas = atoi(minasCadena);
    }

    if (configuracion.cantMinas < 0 || configuracion.cantMinas > configuracion.dimensiones * configuracion.dimensiones) {
        fprintf(stderr, "ERROR: Cantidad de minas inválida: %d (Fuera de rango para %dx%d).\n", configuracion.cantMinas, configuracion.dimensiones, configuracion.dimensiones);
        exit(-5);
    }

     if (configuracion.dimensiones > MAX_DIMENSION) {
        fprintf(stderr, "ERROR: Dimensiones del tablero (%d) exceden el máximo permitido (%d).\n",
                configuracion.dimensiones, MAX_DIMENSION);
        exit(-6);
    }


    printf("Configuracion leida: Dimensiones=%d, Minas=%d\n", configuracion.dimensiones, configuracion.cantMinas);
    return configuracion;
}

/**
 * Revela espacios vacíos adyacentes recursivamente cuando se hace clic en una celda vacía
 * @param matriz - Matriz de celdas del tablero
 * @param dimensiones - Dimensiones del tablero
 * @param fila - Fila de la celda inicial
 * @param col - Columna de la celda inicial
 */
void revelarEspaciosVacios(sCelda **matriz, int dimensiones, int fila, int col)
{
    // Verificar límites
    if (fila < 0 || fila >= dimensiones || col < 0 || col >= dimensiones)
        return;

    // Si ya está revelada, no hacer nada
    if ((*(matriz + fila) + col)->esRevelada)
        return;

    // Si tiene mina, no revelar
    if ((*(matriz + fila) + col)->tieneMina)
        return;

    // Si tiene bandera, no revelar
    if ((*(matriz + fila) + col)->tieneBandera)
        return;

    // Revelar la celda actual
    (*(matriz + fila) + col)->esRevelada = 1;

    // Si la celda tiene número de minas adyacentes > 0, no expandir más
    if ((*(matriz + fila) + col)->minasAdyacentes > 0)
        return;

    // Si es espacio vacío (minasAdyacentes == 0), revelar celdas adyacentes
    if ((*(matriz + fila) + col)->minasAdyacentes == 0)
    {
        // Revelar las 8 celdas adyacentes recursivamente
        // Ciclo para recorrer las 8 direcciones alrededor de la celda actual
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                if (i != 0 || j != 0)  // No procesar la celda actual
                    revelarEspaciosVacios(matriz, dimensiones, fila + i, col + j);
            }
        }
    }
}
