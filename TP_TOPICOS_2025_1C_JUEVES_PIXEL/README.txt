===============================================================================
                              BUSCAMINAS - SDL2
===============================================================================
INTEGRANTES:

LINARES, GUIDO - DNI: 43170056
GOLDRING, FACUNDO - DNI: 44595085


DESCRIPCION:
Implementación del clásico juego Buscaminas utilizando la biblioteca gráfica SDL2. El proyecto fue desarrollado como trabajo práctico para la materia Tópicos de Programación - 1er Cuatrimestre 2025.

ESTRUCTURA DEL PROYECTO:
TP_TOPICOS_2025_1C_JUEVES_PIXEL/
├── fnt/                   Archivos de fuentes (.ttf)
├── lib/                   Bibliotecas necesarias (excepto SDL2.dll)
├── common.h               Definiciones y estructuras comunes
├── interfaz.c/.h          Manejo de la interfaz grafica
├── juego.c/.h             Logica principal del juego
├── logs.c/.h              Sistema de logging
├── main.c                 Funcion principal y comentarios de integrantes
├── menu.c/.h              Sistema de menus
├── tablero.c/.h           Logica del tablero de juego
├── usuario.c/.h           Manejo de entrada de usuario
└── README                 Este archivo


INSTALACION Y CONFIGURACION DE ENTORNO EN CODEBLOCKS:

PASO 1) DESCARGAR SDL2 : https://github.com/libsdl-org/SDL/releases/download/release-2.0.22/SDL2-devel-2.0.
22-mingw.zip

PASO 2) EXTRAERLO EN LA RAIZ DEL JUEGO (debería dejar la carpeta con nombre SDL2-2.0.22 en la raíz (esta dentro de la extraída)).

PASO 3) COPIAR Y PEGAR EL ARCHIVO SDL2.dll que esta en la ruta " TP_TOPICOS_2025_1C_JUEVES_PIXEL\SDL2-2.0.22\x86_64-w64-mingw32\bin" en la raíz de la carpeta del juego.

PASO 4) ENTRAR A LA CARPETA LIB DONDE ESTARA LA LIBRERIA TTF NECESARIA PARA LA FUENTE.

PASO 5) COPIAR Y PEGAR EL ARCHIVO SDL2_ttf.dll que esta en la ruta "TP_TOPICOS_2025_1C_JUEVES_PIXEL\lib\SDL2_ttf-2.24.0\x86_64-w64-mingw32\bin" en la raíz de la carpeta del juego.

PASO 6) Configurar el proyecto dentro de codeblocks con los siguientes 
parámetros (en este orden, asegurarse que se copie todo tanto en debug, release como en el proyecto):

Linker settings: SDL2main 
		 SDL2_ttf
		 SDL2

Search directories (agregar estos paths de carpetas(podria tener variaciones)):

Compiler: SDL2-2.0.22\x86_64-w64-mingw32\include
	  SDL2-2.0.22\x86_64-w64-mingw32\include\SDL2
	  lib\SDL2_ttf-2.24.0\x86_64-w64-mingw32\include

Linker: SDL2-2.0.22\x86_64-w64-mingw32\lib
	lib\SDL2_ttf-2.24.0\x86_64-w64-mingw32\lib


PASO 7) A DISFRUTAR DEL JUEGO !!!!



FUNCIONALIDADES IMPLEMENTADAS:
Funcionalidades Minimas:

 Generacion aleatoria de minas
 Configuracion mediante archivo buscaminas.conf
 Dimensiones configurables (8x8 a 32x32)
 Cantidad de minas configurable (por numero o porcentaje)
 Clic izquierdo para revelar casillas
 Deteccion de fin de juego (victoria/derrota)
 Mostrar adyacencia de minas
 Clic derecho para marcar minas
 Sistema de logging con fecha/hora y coordenadas

Funcionalidades Opcionales:

 Contador de minas restantes en pantalla
 Guardado/carga de partida
 Ingreso de usuario desde SDL
 Estadisticas de ultimas 10 partidas
 Sistema de cheats

