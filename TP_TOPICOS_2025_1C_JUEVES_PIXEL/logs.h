#ifndef LOGS_H_INCLUDED
#define LOGS_H_INCLUDED

#include "common.h"

// Funciones de logging
sSistemaLog* inicializarLog(const char* nombreArchivo);
void destruirLog();
void logInicioPartida(sArchivo_conf configuracion);
void logClickCelda(sCelda** matriz, int fila, int columna, int dimensiones, const char* tipoClick);
void logRevelarCelda(sCelda** matriz, int fila, int columna, int dimensiones);
void logBandera(sCelda** matriz, int fila, int columna, int dimensiones, int colocada);
void logFinPartida(const char* resultado);
void logConfiguracion(sArchivo_conf config);
void logEstadoTablero(sCelda** matriz, int dimensiones);

// Funciones auxiliares internas
char* obtenerTimestamp();
void escribirEvento(const char*);


#endif // LOGS_H_INCLUDED
