#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

int main(int argc, char **argv)
{
    t_game  *game;
    pid_t   pid;
    key_t   key;
    int     board;
    int     player;

    player = 0;
    board = 0;
    if (argc < 3 || argc > 4 || (ft_gameatoi(argv[2]) != 42))
    {
        ft_printf("Error: Use => ./lemipc <team> <ID proyect: 42> <Board (opcional)>\n");
        return (1);
    }

    if (argument_parsing(argv, &board) == -1)
        return (1);

    key = ftok(".", ft_gameatoi(argv[2]));
    if (key == -1)
    {
        ft_printf("Error: ftok failed\n");
        return (1);
    }
    
    game = malloc(sizeof(t_game));
    pid = getpid();
    game->pid = pid;
    game->team_id = ft_gameatoi(argv[1]);
    player = ft_resconf(game, key, board);
    if (player == 1)
    {
        if (player_one(game, key) == -1)
        {
            free(game);
            return (1);
        }
        // Inicialización del semáforo (un paso futuro)
        // struct sembuf sops;
        // sops.sem_num = 0;
        // sops.sem_op = 1;
        // sops.sem_flg = 0;
        // semop(game->semid, &sops, 1);
    }
    else
    {
        if (other_player(game, key) == -1)
        {
            free(game);
            return (1);
        }

    }

    ft_printf("PID: %d, Team: %d, Player: %d, Board: %d\n", game->pid, game->team_id, game->player, board);
    // 2. Lógica del Juego y del Jugador

    // 3. Limpieza de Recursos de IPC
    clearmemsem(game);

    free (game);
    return (0); 
}