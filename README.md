# Buscaminas (TP - Tópicos de Programación 2025)

Este proyecto es una implementación del clásico juego de Buscaminas desarrollado como Trabajo Práctico para la materia Tópicos de Programación del primer cuatrimestre de 2025. El objetivo principal es demostrar el uso efectivo de la biblioteca SDL2 para la creación de interfaces gráficas, así como la aplicación de los conceptos y estructuras de datos fundamentales de la programación en C/C++.

## Características

### Funcionalidades Principales

* **Generación Aleatoria de Minas:** El tablero del juego se genera dinámicamente al inicio de cada partida, distribuyendo las minas en posiciones aleatorias para garantizar la variabilidad en cada juego.
* **Configuración Externa:** El comportamiento del juego se puede personalizar mediante un archivo de configuración externo llamado `buscaminas.conf`. Esto permite ajustar la dificultad y la presentación del juego sin necesidad de recompilar el código.
    * **Dimensiones del Tablero:** El archivo de configuración permite establecer las dimensiones del tablero de juego. El tablero es siempre cuadrado, con un tamaño mínimo de 8x8 casillas y un tamaño máximo de 32x32 casillas.
    * **Cantidad de Minas:** La cantidad de minas presentes en el tablero se puede definir de dos maneras:
        * Cantidad absoluta: Especificando el número exacto de minas (ej., `cantidad_minas = 10`).
        * Porcentaje del tablero: Indicando la densidad de minas como un porcentaje (ej., `cantidad_minas = 10%`, lo que significa una mina por cada 10 casillas).
* **Interacción del Usuario:** El juego responde a las acciones del usuario a través de la interfaz gráfica proporcionada por SDL2:
    * **Clic Izquierdo:** Al hacer clic izquierdo en una casilla, se revela su contenido.
        * Si la casilla contiene una mina, el juego termina, se revelan todas las minas en el tablero, y se notifica al jugador la derrota.
        * Si la casilla no contiene una mina, se muestra un número que indica la cantidad de minas adyacentes a esa casilla.
    * **Clic Derecho:** Al hacer clic derecho en una casilla, se coloca o se quita un indicador de mina. Esto permite al jugador marcar las casillas que sospecha que contienen minas, evitando clics accidentales en ellas.
* **Registro de Eventos (Log):** El juego registra los eventos importantes en un archivo de log, incluyendo la fecha y hora de inicio y fin de la partida, así como cada clic realizado por el usuario. Para cada clic, se registran las coordenadas de la casilla en la matriz del juego.

### Características Opcionales (Implementadas)

* **Cantidad de Minas Restantes:** Se muestra en la pantalla la cantidad de minas que aún no han sido marcadas por el jugador.
* **Guardar/Cargar Partida:** El estado actual de una partida se puede guardar en un archivo binario, lo que permite al jugador retomar la partida en una sesión posterior.
* **Ingreso de Usuario:** El nombre del jugador se ingresa a través de una interfaz gráfica de SDL2 al inicio de la partida.
* **Estadísticas de Juego:** Se guardan las estadísticas de las últimas 10 partidas en un archivo de texto, y estas estadísticas se muestran al jugador utilizando SDL2.
* **Cheat:** Se implementó un "cheat" que resalta sutilmente el borde de las casillas adyacentes a una mina al pasar el mouse por encima.

### Estructuras de Datos y Técnicas de Programación

Este proyecto hace uso de las siguientes estructuras de datos y técnicas de programación, tal como se especificó en la consigna del trabajo práctico:

* Memoria dinámica (para la gestión eficiente del tablero).
* Punteros a funciones (si se utilizaron para implementar alguna funcionalidad específica).
* Recursividad (si se utilizó en algún algoritmo, como la revelación en cadena de casillas vacías).

### Diseño Gráfico

El diseño gráfico del juego utiliza una paleta de colores clara y una interfaz intuitiva para facilitar la jugabilidad. Los números de adyacencia se muestran con diferentes colores para mejorar la legibilidad.. Se ha tenido en cuenta la recomendación de no exceder una resolución de 1366x768 para asegurar la compatibilidad con la mayoría de las pantallas.

### Instrucciones de Compilación y Ejecución (C)

Para compilar y ejecutar este juego (escrito en C), se requiere el siguiente software y configuración:

1.  **Compilador de C:** Se recomienda utilizar MinGW GCC.
2.  **IDE (Entorno de Desarrollo Integrado):** Code::Blocks (u otro IDE compatible con MinGW).
3.  **Biblioteca SDL2:**
    * Descargar SDL2 de https://github.com/libsdl-org/SDL/releases/download/release-2.0.22/SDL2-devel-2.0.22-mingw.zip.
    * Descomprimir la carpeta de SDL2 en la raíz del proyecto.
    * Configurar las rutas de inclusión y la librería en Code::Blocks (o tu IDE):
        * **Rutas de Inclusión:** Project -> Build Options -> Search Directories -> Compiler: \[Ruta a la carpeta include de SDL2] (ej., `SDL2-2.0.22\x86_64-w64-mingw32\include`).
        * **Rutas de Librería:** Project -> Build Options -> Search Directories -> Linker: \[Ruta a la carpeta lib de SDL2] (ej., `SDL2-2.0.22\x86_64-w64-mingw32\lib`).
        * **Opciones del Enlazador:** Project -> Build Options -> Linker Settings -> Other linker options: `-lmingw32 -mconsole`
        * **Librerías a Enlazar:** Project -> Build Options -> Linker Settings -> Link libraries: `SDL2`, `SDL2main`.
            * **Importante:** Asegúrate de que Code::Blocks esté configurado para compilar como un proyecto en C.  Esto generalmente se configura al crear un nuevo proyecto.
    * Copiar el archivo `SDL2.dll` desde la carpeta de SDL2 a la carpeta donde se genera el ejecutable (bin/Debug o bin/Release).
