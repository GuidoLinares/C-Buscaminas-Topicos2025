#include "common.h"
#include "usuario.h"

// Función para crear un nuevo usuario
void crearNuevoUsuario(const char* nombre, Usuario* usuario)
{
    if (!usuario || !nombre)
        return;

    strcpy(usuario->nombre, nombre);
    usuario->fechaCreacion = time(NULL);
    usuario->ultimoAcceso = time(NULL);

    inicializarEstadisticas(&usuario->estadisticas);

    // Inicializar partidas guardadas
    for (int i = 0; i < MAX_PARTIDAS_GUARDADAS; i++)
    {
        usuario->partidas[i].esValida = 0;
        strcpy(usuario->partidas[i].nombre, "");
    }
}

int cargarUsuario(const char* nombre, Usuario* usuario)
{
    if (!usuario || !nombre)
        return -1;

    FILE* archivo = fopen(ARCHIVO_USUARIOS, "rb");
    if (!archivo)
    {
        // Si no existe el archivo, crear usuario nuevo
        crearNuevoUsuario(nombre, usuario);
        return 0; // Usuario nuevo creado
    }

    Usuario temp;
    int encontrado = 0;

    while (fread(&temp, sizeof(Usuario), 1, archivo) == 1)
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
        // Usuario no encontrado, crear nuevo
        crearNuevoUsuario(nombre, usuario);
        return 0;
    }

    return 1; // Usuario existente cargado
}

int guardarUsuario(Usuario* usuario)
{
    if (!usuario)
        return -1;

    FILE* archivo = fopen(ARCHIVO_USUARIOS, "rb");
    Usuario* usuarios = NULL;
    int numUsuarios = 0;
    int usuarioExistente = -1;

    // Leer todos los usuarios existentes
    if (archivo)
    {
        fseek(archivo, 0, SEEK_END);
        long tam = ftell(archivo);
        numUsuarios = tam / sizeof(Usuario);
        fseek(archivo, 0, SEEK_SET);

        if (numUsuarios > 0)
        {
            usuarios = malloc(sizeof(Usuario) * numUsuarios);
            if (usuarios)
            {
                fread(usuarios, sizeof(Usuario), numUsuarios, archivo);

                // Buscar si el usuario ya existe
                for (int i = 0; i < numUsuarios; i++)
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
        fwrite(usuarios, sizeof(Usuario), numUsuarios, archivo);
    }
    else
    {
        // Escribir usuarios existentes y agregar el nuevo
        if (usuarios && numUsuarios > 0)
            fwrite(usuarios, sizeof(Usuario), numUsuarios, archivo);
        fwrite(usuario, sizeof(Usuario), 1, archivo);
    }

    fclose(archivo);
    if (usuarios) free(usuarios);
    return 0;
}

int usuarioExiste(const char* nombre)
{
    if (!nombre)
        return 0;

    FILE* archivo = fopen(ARCHIVO_USUARIOS, "rb");
    if (!archivo)
        return 0;

    Usuario temp;
    while (fread(&temp, sizeof(Usuario), 1, archivo) == 1)
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

void actualizarEstadisticas(Usuario* usuario, int gano, int tiempoSegundos)
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

double calcularPorcentajeVictorias(Usuario* usuario)
{
    if (!usuario || usuario->estadisticas.partidasJugadas == 0)
        return 0.0;

    return ((double)usuario->estadisticas.partidasGanadas / usuario->estadisticas.partidasJugadas) * 100.0;
}

int guardarPartida(Usuario* usuario, const char* nombrePartida, sCelda** matriz, Archivo_conf config, int minasRestantes, int tiempoTranscurrido)
{
    if (!usuario || !nombrePartida)
        return -1;

    int slot = buscarSlotLibre(usuario);
    if (slot == -1)
        return -1; // No hay slots disponibles

    PartidaGuardada* partida = &usuario->partidas[slot];

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

int cargarPartida(Usuario* usuario, int indicePartida, sCelda*** matriz, Archivo_conf* config, int* minasRestantes, int* tiempoTranscurrido)
{
    if (!usuario || indicePartida < 0 || indicePartida >= MAX_PARTIDAS_GUARDADAS)
        return -1;

    PartidaGuardada* partida = &usuario->partidas[indicePartida];
    if (!partida->esValida)
        return -1;

    // Restaurar configuración
    config->dimensiones = partida->dimensiones;
    config->cantMinas = partida->cantMinas;
    *minasRestantes = partida->minasRestantes;
    *tiempoTranscurrido = partida->tiempoTranscurrido;

    // Aquí deberías restaurar también el estado de la matriz
    // Por ahora solo restauramos la configuración básica

    return 0;
}

void eliminarPartida(Usuario* usuario, int indicePartida)
{
    if (!usuario || indicePartida < 0 || indicePartida >= MAX_PARTIDAS_GUARDADAS)
        return;

    usuario->partidas[indicePartida].esValida = 0;
    strcpy(usuario->partidas[indicePartida].nombre, "");
}

int buscarSlotLibre(Usuario* usuario)
{
    if (!usuario)
        return -1;

    for (int i = 0; i < MAX_PARTIDAS_GUARDADAS; i++)
    {
        if (!usuario->partidas[i].esValida)
            return i;
    }

    return -1; // No hay slots libres
}

int validarNombreUsuario(const char* nombre)
{
    if (!nombre)
        return 0;

    int longitud = strlen(nombre);

    // Verificar longitud válida
    if (longitud < 3 || longitud >= MAX_NOMBRE_USUARIO)
        return 0;

    // Verificar que todos los caracteres sean alfanuméricos o guion bajo
    for (int i = 0; i < longitud; i++)
    {
        char c = nombre[i];
        if (!isalnum(c) && c != '_')
            return 0;
    }

    return 1; // Nombre válido
}

void formatearTiempo(int segundos, char* buffer)
{
    if (!buffer)
        return;

    int horas = segundos / 3600;
    int minutos = (segundos % 3600) / 60;
    int seg = segundos % 60;

    sprintf(buffer, "%02d:%02d:%02d", horas, minutos, seg);
}

void inicializarEstadisticas(EstadisticasUsuario* stats)
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






