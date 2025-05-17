
#include "headers.h"
/*
Apellido(s), nombre(s): Linares, Guido Hernan
DNI: 43170056
Entrega: Sí

Apellido(s), nombre(s): Goldring, Facundo
DNI: 44595085
Entrega: Sí

Apellido(s), nombre(s): Calvet, Lucas
DNI:  (pongan su DNI)
Entrega: Sí
*/

#include "headers.h"


int main(int argc, char *argv[])
{
    int** matriz;
    Archivo_conf configuracion;
    configuracion = leerArchivo();

    matriz = crearMatriz(configuracion.dimensiones);
    if(!matriz)
        printf("%s",SIN_MEM);
    else
    {
        inicializarMatriz(matriz, configuracion.dimensiones);
        puts("ARRANCA SETEO DEL JUEGO");
        llenarMatriz(matriz, configuracion);
    }

    if (verificarSDL()) {
        printf("SDL2 está instalado y funcionando correctamente (inicialización básica exitosa).\n");
    } else {
        printf("Se encontraron problemas con la instalación de SDL2.\n");
    }

    mostrarMatriz(matriz,configuracion.dimensiones);
    free(matriz);
    return 0;
}
