#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

int argument_parsing(t_game *game, char **argv)
{
    t_game  *aux;

    aux = game;
}

int main(int argc, char **argv)
{
    t_game  *game;
    key_t   key;

    if (argc < 3 || argc > 4)
    {
        // ./lemipc 1 42 50
        ft_printf("Error: Use => ./lemipc <team> <ID proyect> <board>\n");
        return (1);
    }
    if (argument_parsing(&game, argv) == -1)
        return (1);
    // 1. Configuración de los Recursos de IPC
    key = ftok(".", 42);
    // 2. Lógica del Juego y del Jugador
    // 3. Limpieza de Recursos de IPC
    return (0); 
}