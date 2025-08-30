#define _GNU_SOURCE
#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

int main(int argc, char **argv)
{
    t_game  *game;
    pid_t   pid;
    key_t   key;
    int     shmflg;

    if (argc < 3 || argc > 4)
    {
        // ./lemipc 1 42 50
        ft_printf("Error: Use => ./lemipc <team> <ID proyect> <board>\n");
        return (1);
    }
    if (argument_parsing(argv) == -1)
        return (1);
    // 1. Configuración de los Recursos de IPC
    game = malloc(sizeof(t_game));
    pid = getpid();
    key = ftok(".", pid);
    game->pid = pid;
    game->team_id = ft_gameatoi(argv[1]);
    game->player = 1;
    if (game->player == 1)
        shmflg = IPC_CREAT | IPC_EXCL;
    else
        shmflg = IPC_CREAT;
    game->shmid = shmget(key, ft_gameatoi(argv[2]), shmflg);
    if (game->shmid == -1)
        return (1);
    game->board_ptr = (int *)shmat(game->shmid, NULL, shmflg);
    // 2. Lógica del Juego y del Jugador
    // 3. Limpieza de Recursos de IPC
    free (game);
    return (0); 
}