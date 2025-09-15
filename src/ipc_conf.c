#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

// Verificación si los recursos IPC ya existen
int    ft_resconf(t_gamer *gamer, key_t  key, int board)
{
    size_t  total_size;

    // Espacio adicional: Tamaño tablero, Total equipos y jugadores.
    total_size = 4 * sizeof(int) + (board * sizeof(int));
    gamer->shm_id = shmget(key, total_size, IPC_CREAT | IPC_EXCL | 0666);       // La Solicitud del Recurso
    if (gamer->shm_id == -1)
        return (2);
    else
        return (1);
}

void    place_player_randomly(t_gamer *player)
{
    int             x;
    int             y;
    int             *game_board = (int *)(player->board_ptr + 3 * sizeof(int));
    bool            found_spot;  

    found_spot = false;

    srand(time(NULL) + player->pid);
    while (!found_spot)
    {
        x = rand() % player->board_dim;
        y = rand() % player->board_dim;
        if (game_board[y * player->board_dim + x] == 0)
        {
            player->x = x;
            player->y = y;
            game_board[y * player->board_dim + x] = player->team_id;
            found_spot = true;
        }
    }
    
    ft_printf("Player: %d - Team: %d - placed at (%d, %d)\n", player->player, player->team_id, player->x, player->y);
}

int    player_one(t_gamer *gamer, key_t  key)
{
    int                 *player_count;
    int                 *teams_count;
    int                 *total_teams;
    int                 *board_dim;
    size_t              total_size;
    union semaphunion   arg;
    struct sembuf       sops;

    gamer->board_ptr = shmat(gamer->shm_id, NULL, 0);                        // Adjuntar la memoria al proceso
    if (gamer->board_ptr == (void *)-1)
    {
        ft_printf("❌ Error: shmat failed ❌\n");
        return (-1);
    }

    board_dim = (int *)gamer->board_ptr;
    player_count = (int *)(gamer->board_ptr + sizeof(int));
    teams_count = (int *)(gamer->board_ptr + 2 * sizeof(int));
    total_teams = (int *)(gamer->board_ptr + 3 * sizeof(int));
    total_size = 4 * sizeof(int) + (gamer->board_size * sizeof(int));
    ft_memset(gamer->board_ptr, 0, total_size);

    *board_dim = gamer->board_dim;
    *player_count = 1;
    *teams_count = 1;
    *total_teams = 1;
    
    gamer->sem_id = semget(key, 1, IPC_CREAT | 0666);
    if (gamer->sem_id == -1)
    {
        ft_printf("❌ Error: semget failed ❌\n");
        return (-1);
    }
    
    arg.val = 1;
    if (semctl(gamer->sem_id, 0, SETVAL, arg) == -1)
    {
        ft_printf("❌ Error: semctl failed ❌\n");
        return (-1);
    }
    
    gamer->msg_id = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    if (gamer->msg_id == -1)
    {
        ft_printf("❌ Error: msgget failed ❌\n");
        return (-1);
    }
    
    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    semop(gamer->sem_id, &sops, 1);

    gamer->player = *player_count;
    place_player_randomly(gamer);
    
    sops.sem_op = 1;
    semop(gamer->sem_id, &sops, 1);

    return (0);
}

int    other_player(t_gamer *gamer, key_t key)
{
    struct sembuf   sops;
    int             *player_count;
    int             *team_count;
    int             *total_teams;
    int             *game_board;
    bool            is_new_team;

    gamer->shm_id = shmget(key, 0, 0);
    if (gamer->shm_id == -1)
    {
        ft_printf("❌ Error: shmget failed - no existe memoria compartida ❌\n");
        return (-1);
    }

    gamer->board_ptr = shmat(gamer->shm_id, NULL, 0);
    if (gamer->board_ptr == (void *)-1)
    {
        ft_printf("❌ Error: shmat failed ❌\n");
        return (-1);
    }

    gamer->board_dim = *(int *)gamer->board_ptr;
    gamer->board_size = gamer->board_dim * gamer->board_dim;
    player_count = (int *)(gamer->board_ptr + sizeof(int));
    team_count = (int *)(gamer->board_ptr + 2 * sizeof(int));
    total_teams = (int *)(gamer->board_ptr + 3 * sizeof(int));
    game_board = (int *)(gamer->board_ptr + 4 * sizeof(int));

    gamer->sem_id = semget(key, 0, 0);                                     // Unirse al semaforo existente
    if (gamer->sem_id == -1)
    {
        ft_printf("❌ Error: semget failed ❌\n");
        return (-1);
    }

    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    semop(gamer->sem_id, &sops, 1);

    is_new_team = true;
    for (int i = 0; i < gamer->board_size; i++)
    {
        if (game_board[i] == gamer->team_id)
        {
            is_new_team = false;
            break;
        }
    }
    
    if (is_new_team)
    {
        *team_count += 1;
        *total_teams += 1;
        ft_printf("New team detected! - Team number: %d - Total teams: %d\n", gamer->team_id, *total_teams);
    }

    *player_count += 1;
    gamer->player = *player_count;
    gamer->msg_id = msgget(key, 0);
    if (gamer->msg_id == -1)
    {
        ft_printf("❌ Error: msgget failed ❌\n");
        return (-1);
    }

    place_player_randomly(gamer);

    sops.sem_op = 1;
    semop(gamer->sem_id, &sops, 1);

    return (0);
}
