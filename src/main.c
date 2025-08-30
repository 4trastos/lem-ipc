#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

int main(int argc, char **argv)
{
    t_game  *game;
    pid_t   pid;
    key_t   key;
    int     board = 100;

    if (argc < 2 || argc > 3)
    {
        ft_printf("Error: Use => ./lemipc <team> <ID proyect: 42>\n");
        return (1);
    }

    if (argument_parsing(argv) == -1)
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
    game->shmid = shmget(key, board, IPC_CREAT | IPC_EXCL | 0666);
    if (game->shmid == -1)
    {
        game->player = 2;
        game->shmid = shmget(key, 0, 0);                            // Nos unimos a la memoria compartida existente
        game->board_ptr = (int *)shmat(game->shmid, NULL, 0);
        if (game->board_ptr == (int *)-1)
        {
            ft_printf("Error: shmat failed\n");
            free(game);
            return (1);
        }
    }
    else
    {
        game->player = 1;
        game->board_ptr = (int *)shmat(game->shmid, NULL, 0);
        if (game->board_ptr == (int *)-1)
        {
            ft_printf("Error: shmat failed\n");
            free(game);
            return (1);
        }
        game->semid = semget(key, 1, IPC_CREAT | 0666);
        if (game->semid == -1)
        {
            ft_printf("Error: semget failed\n");
            free(game);
            return(1);
        }

        // Inicialización del semáforo (un paso futuro)
        // struct sembuf sops;
        // sops.sem_num = 0;
        // sops.sem_op = 1;
        // sops.sem_flg = 0;
        // semop(game->semid, &sops, 1);
    }

    ft_printf("PID: %d, Team: %d, Player: %d\n", game->pid, game->team_id, game->player);
    // 2. Lógica del Juego y del Jugador

    // 3. Limpieza de Recursos de IPC
    // Las funciones de limpieza se llamarán al final del juego, no aquí.

    free (game);
    return (0); 
}