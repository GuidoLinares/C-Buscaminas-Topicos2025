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
}EstadisticasUsuario;

// Estructura de partida guardada
typedef struct {
    char nombre[30];
    time_t fechaGuardado;
    int dimensiones;
    int cantMinas;
    int minasRestantes;
    int tiempoTranscurrido;
    int esValida;
}PartidaGuardada;

// Estructura principal del usuario
typedef struct {
    char nombre[MAX_NOMBRE_USUARIO];
    time_t fechaCreacion;
    time_t ultimoAcceso;
    EstadisticasUsuario estadisticas;
    PartidaGuardada partidas[MAX_PARTIDAS_GUARDADAS];
}Usuario;

//USUARIOS
void crearNuevoUsuario(const char* nombre, Usuario* usuario);
int cargarUsuario(const char* nombre, Usuario* usuario);
int guardarUsuario(Usuario* usuario);
int usuarioExiste(const char* nombre);

//ESTADISTICAS
void actualizarEstadisticas(Usuario* usuario, int gano, int tiempoSegundos);
double calcularPorcentajeVictorias(Usuario* usuario);
int guardarPartida(Usuario* usuario, const char* nombrePartida, sCelda** matriz, Archivo_conf config, int minasRestantes, int tiempoTranscurrido);
void ejecutarPartida(SDL_Window* ventana,SDL_Renderer* renderer, TTF_Font* fuente, TTF_Font* fuenteGrande, Usuario* usuarioActual,Archivo_conf config);


//PARTIDAS
int cargarPartida(Usuario* usuario, int indicePartida, sCelda*** matriz, Archivo_conf* config,int* minasRestantes, int* tiempoTranscurrido);
void eliminarPartida(Usuario* usuario, int indicePartida);
int buscarSlotLibre(Usuario* usuario);

//AUX
int validarNombreUsuario(const char* nombre);
void formatearTiempo(int segundos, char* buffer);
void inicializarEstadisticas(EstadisticasUsuario* stats);


#endif // USUARIO_H_INCLUDED
