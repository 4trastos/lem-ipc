#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        ft_printf("Error: Use => ./lemipc <player> <gane code> <table>\n");
        return (1);
    }
    ft_printf("Arguments: Player: %s - Game Code: %s - Table: %s\n", argv[1], argv[2], argv[3]);
    // 1. Configuración de los Recursos de IPC
    // 2. Lógica del Juego y del Jugador
    // 3. Limpieza de Recursos de IPC
    return (0); 
}