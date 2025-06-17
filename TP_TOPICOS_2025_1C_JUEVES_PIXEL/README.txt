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

COMPILACION Y EJECUCION:
Requisitos:

COPIAR Y PEGAR EL ARCHIVO SDL2.dll de la ruta " TP_TOPICOS_2025_1C_JUEVES_PIXEL\SDL2-2.0.22\x86_64-w64-mingw32\bin" en la raíz de la carpeta.

COPIAR Y PEGAR EL ARCHIVO SDL2_ttf.dll de la ruta "TP_TOPICOS_2025_1C_JUEVES_PIXEL\lib\SDL2_ttf-2.24.0\x86_64-w64-mingw32\bin" en la raíz de la carpeta.


CONFIGURACION:
El archivo buscaminas.conf debe contener:
dimensiones=16
cantidad_minas=40
O alternativamente:
cantidad_minas=15%

CONTROLES:

Clic izquierdo: Revelar casilla
Clic derecho: Marcar/desmarcar mina
ESC: Salir del juego

ARCHIVOS DE LOG:
El juego genera automaticamente archivos de log con:

Fecha y hora de inicio/fin de partida
Registro de todos los clics con coordenadas
Resultado de la partida

NOTAS TECNICAS:

Resolucion recomendada: 1366x768 o superior
El proyecto utiliza memoria dinamica para el tablero
Implementa recursividad para revelar casillas vacias
Sistema de eventos SDL para manejo de entrada======================