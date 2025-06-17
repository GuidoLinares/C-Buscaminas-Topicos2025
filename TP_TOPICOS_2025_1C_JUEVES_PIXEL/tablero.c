#include "common.h"
#include "tablero.h"

/**
 * Cuenta el numero de minas adyacentes a una celda especifica
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
 * @param tamano - Numero de filas de la matriz
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
 * Genera un numero aleatorio dentro de un rango especifico
 * @param minimo - Valor minimo del rango (inclusivo)
 * @param maximo - Valor maximo del rango (inclusivo)
 * @return Numero aleatorio dentro del rango especificado
 */
int generarAleatorio(int minimo, int maximo)
{
    return rand() % (maximo - minimo + 1) + minimo;
}

/**
 * Llena la matriz con minas en posiciones aleatorias y calcula las minas adyacentes
 * @param matriz - Matriz de celdas del tablero
 * @param configuracion - Configuracion con dimensiones y cantidad de minas
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
 * Lee la configuracion del juego desde un archivo de texto
 * Formato segun especificaciones del TP:
 * - dimensiones=16 (minimo 8, maximo 32)
 * - cantidad_minas=10 o cantidad_minas=15%
 * @return Estructura con la configuracion leida
 */
sArchivo_conf leerArchivo()
{
    sArchivo_conf configuracion;
    FILE* archivo;
    char linea[MAX_LINEA];
    int dimensionesLeidas = 0;
    int minasLeidas = 0;

    // Valores por defecto segun especificaciones del TP
    configuracion.dimensiones = 16;  // Por defecto 16x16
    configuracion.cantMinas = 40;    // Por defecto 40 minas

    archivo = fopen(ARCH_CONFIG, "r");
    if (!archivo) {
        fprintf(stderr, "AVISO: No se pudo abrir %s, usando configuracion por defecto\n", ARCH_CONFIG);
        fprintf(stderr, "Creando archivo de configuracion por defecto...\n");

        // Crear archivo por defecto
        archivo = fopen(ARCH_CONFIG, "w");
        if (archivo) {
            fprintf(archivo, "dimensiones=16\n");
            fprintf(archivo, "cantidad_minas=40\n");
            fclose(archivo);
            printf("Archivo %s creado con configuracion por defecto\n", ARCH_CONFIG);
        }
        return configuracion;
    }

    // Leer linea por linea
    while (fgets(linea, MAX_LINEA, archivo)) {
        trim(linea);

        // Ignorar lineas vacias y comentarios
        if (strlen(linea) == 0 || linea[0] == '#') {
            continue;
        }

        // Buscar dimensiones
        if (strncmp(linea, "dimensiones=", 12) == 0) {
            int dim = atoi(linea + 12);
            if (dim >= 8 && dim <= MAX_DIMENSION) {
                configuracion.dimensiones = dim;
                dimensionesLeidas = 1;
            } else {
                fprintf(stderr, "ERROR: Dimensiones %d fuera del rango valido (8-%d)\n", dim, MAX_DIMENSION);
                exit(-1);
            }
        }
        // Buscar cantidad de minas
        else if (strncmp(linea, "cantidad_minas=", 15) == 0) {
            char* valorMinas = linea + 15;
            trim(valorMinas);

            int longitud = strlen(valorMinas);
            if (longitud > 0 && valorMinas[longitud - 1] == '%') {
                // Formato porcentaje: cantidad_minas=15%
                valorMinas[longitud - 1] = '\0';
                int porcentaje = atoi(valorMinas);

                if (porcentaje < 1 || porcentaje > 90) {
                    fprintf(stderr, "ERROR: Porcentaje de minas debe estar entre 1%% y 90%%\n");
                    exit(-2);
                }

                int totalCasillas = configuracion.dimensiones * configuracion.dimensiones;
                configuracion.cantMinas = (porcentaje * totalCasillas) / 100;

                // Asegurar al menos 1 mina
                if (configuracion.cantMinas < 1) {
                    configuracion.cantMinas = 1;
                }

                printf("Minas calculadas por porcentaje: %d%% de %d = %d minas\n",
                       porcentaje, totalCasillas, configuracion.cantMinas);
            } else {
                // Formato numero: cantidad_minas=40
                configuracion.cantMinas = atoi(valorMinas);
            }
            minasLeidas = 1;
        }
    }

    fclose(archivo);

    // Validar que se leyeron ambos valores
    if (!dimensionesLeidas) {
        fprintf(stderr, "AVISO: No se encontro 'dimensiones=' en %s, usando %d por defecto\n",
                ARCH_CONFIG, configuracion.dimensiones);
    }

    if (!minasLeidas) {
        fprintf(stderr, "AVISO: No se encontro 'cantidad_minas=' en %s, usando %d por defecto\n",
                ARCH_CONFIG, configuracion.cantMinas);
    }

    // Validacion final
    int maxMinasPosibles = (configuracion.dimensiones * configuracion.dimensiones) - 1;
    if (configuracion.cantMinas < 1 || configuracion.cantMinas > maxMinasPosibles) {
        fprintf(stderr, "ERROR: Cantidad de minas %d fuera del rango valido (1-%d) para tablero %dx%d\n",
                configuracion.cantMinas, maxMinasPosibles, configuracion.dimensiones, configuracion.dimensiones);
        exit(-3);
    }

    printf("Configuracion cargada exitosamente:\n");
    printf("- Dimensiones: %dx%d\n", configuracion.dimensiones, configuracion.dimensiones);
    printf("- Minas: %d\n", configuracion.cantMinas);
    printf("- Porcentaje de minas: %.1f%%\n",
           ((double)configuracion.cantMinas / (configuracion.dimensiones * configuracion.dimensiones)) * 100.0);

    return configuracion;
}

/**
 * Revela espacios vacios adyacentes recursivamente cuando se hace clic en una celda vacia
 * @param matriz - Matriz de celdas del tablero
 * @param dimensiones - Dimensiones del tablero
 * @param fila - Fila de la celda inicial
 * @param col - Columna de la celda inicial
 */
void revelarEspaciosVacios(sCelda **matriz, int dimensiones, int fila, int col)
{
    // Verificar limites
    if (fila < 0 || fila >= dimensiones || col < 0 || col >= dimensiones)
        return;

    // Si ya esta revelada, no hacer nada
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

    // Si la celda tiene numero de minas adyacentes > 0, no expandir mas
    if ((*(matriz + fila) + col)->minasAdyacentes > 0)
        return;

    // Si es espacio vacio (minasAdyacentes == 0), revelar celdas adyacentes
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
