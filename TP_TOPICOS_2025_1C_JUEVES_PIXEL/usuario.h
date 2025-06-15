#ifndef USUARIO_H_INCLUDED
#define USUARIO_H_INCLUDED
#include "common.h"

#define MAX_NOMBRE_USUARIO 30
#define MAX_PARTIDAS_GUARDADAS 5
#define ARCHIVO_USUARIOS "usuarios.dat"

typedef struct {
    int partidasJugadas;
    int partidasGanadas;
    int partidasPerdidas;
    int mejorTiempo;
    int tiempoTotalJugado;
    int rachaActual;
    int mejorRacha;
}sEstadisticasUsuario;

// Estructura de partida guardada MODIFICADA
typedef struct {
    char nombre[100];                    // AUMENTADO: más espacio para el nombre
    time_t fechaGuardado;
    int dimensiones;
    int cantMinas;
    int minasRestantes;
    int tiempoTranscurrido;
    int primerClic;                      // NUEVO: 1 si NO se hizo primer clic, 0 si ya se hizo
    int esValida;
    sCelda estadoTablero[MAX_DIMENSION * MAX_DIMENSION];
}sPartidaGuardada;

// Estructura principal del usuario
typedef struct {
    char nombre[MAX_NOMBRE_USUARIO];
    time_t fechaCreacion;
    time_t ultimoAcceso;
    sEstadisticasUsuario estadisticas;
    sPartidaGuardada partidas[MAX_PARTIDAS_GUARDADAS];
}sUsuario;

//USUARIOS
void crearNuevoUsuario(const char* nombre, sUsuario* usuario);
int cargarUsuario(const char* nombre, sUsuario* usuario);
int guardarUsuario(sUsuario* usuario);
int usuarioExiste(const char* nombre);

//ESTADISTICAS
void actualizarEstadisticas(sUsuario* usuario, int gano, int tiempoSegundos);
double calcularPorcentajeVictorias(sUsuario* usuario);

int guardarPartidaCompleta(sUsuario* usuario, const char* nombrePartida, sCelda** matriz,
                          sArchivo_conf config, int minasRestantes, int tiempoTranscurrido, int primerClic);

void ejecutarPartida(SDL_Window* ventana,SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, sUsuario* usuarioActual,sArchivo_conf config);


//PARTIDAS (funciones antiguas mantenidas para compatibilidad)
int cargarPartida(sUsuario* usuario, int indicePartida, sCelda*** matriz, sArchivo_conf* config,int* minasRestantes, int* tiempoTranscurrido);
void eliminarPartida(sUsuario* usuario, int indicePartida);
int buscarSlotLibre(sUsuario* usuario);
void debugMostrarPartidasGuardadas(sUsuario* usuario);
int guardarPartidaCompleta(sUsuario* usuario, const char* nombrePartida, sCelda** matriz,
                          sArchivo_conf config, int minasRestantes, int tiempoTranscurrido, int primerClic);
int cargarPartidaCompleta(sUsuario* usuario, int indicePartida, sCelda*** matriz,
                         sArchivo_conf* config, int* minasRestantes, int* tiempoTranscurrido, int* primerClic);
void ejecutarPartidaCargada(SDL_Window* ventana, SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande,
                           sUsuario* usuario, sCelda** matriz, sArchivo_conf config, int minasRestantes,
                           int tiempoTranscurrido, int primerClic);
void debugMostrarPartidasGuardadas(sUsuario* usuario);

//AUX
int validarNombreUsuario(const char* nombre);
void formatearTiempo(int segundos, char* buffer);
void inicializarEstadisticas(sEstadisticasUsuario* stats);

#endif // USUARIO_H_INCLUDED
