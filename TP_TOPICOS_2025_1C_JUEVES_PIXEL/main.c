
/*
Apellido(s), nombre(s): Linares, Guido Hernan
DNI: 43170056
Entrega: S�

Apellido(s), nombre(s): Goldring, Facundo
DNI:   (pongan su DNI)
Entrega: S�

Apellido(s), nombre(s): Calvet, Lucas
DNI:  (pongan su DNI)
Entrega: S�
*/

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error inicializando SDL: %s\n", SDL_GetError());
        return 1;
    }
    printf("SDL se inicializ� correctamente.\n");
    SDL_Quit();

    printf("LISTO EL REPOSITORIO");
    return 0;
}
