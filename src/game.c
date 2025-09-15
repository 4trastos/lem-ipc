#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

int get_total_teams(t_gamer *gamer)
{
    int     team_count = 0;
    int     cell_value;
    int     max_teams = 100;
    bool    team_present[max_teams + 1];
    int     *game_board = (int *)(gamer->board_ptr + 4 * sizeof(int));

    ft_printf("[DEBUG - 07] GET TOTAL TEAMS\n");

    for (int i = 0; i <= max_teams; i++)
        team_present[i] = false;
    
    for (int y = 0; y < gamer->board_dim; y++)
    {
        for (int x = 0; x < gamer->board_dim; x++)
        {
            cell_value = game_board[y * gamer->board_dim + x];
            if (cell_value != 0)
            {
                if (cell_value > max_teams || cell_value < 0)
                {
                    ft_printf("❌ Error: Invalid team ID (%d) found on the board. The maximum team ID supported is %d.\n", cell_value, max_teams);
                    return (-1);
                }
                if (!team_present[cell_value])
                {
                    team_present[cell_value] = true;
                    team_count++;
                }
            }
        }
    }
    return (team_count);
}

int check_game_status(t_gamer *gamer)
{
    int current_teams;
    int players;
    int initial_teams;

    ft_printf("[DEBUG - 06] CHECK GAME ESTATUS\n");
    initial_teams = *(int*)(gamer->board_ptr + 3 * sizeof(int));
    current_teams = get_total_teams(gamer);
    players = *(int*)(gamer->board_ptr + sizeof(int));
    ft_printf("[DEBUG - 08] EQUIPOS DE INICIO: ( %d )\n", initial_teams);
    ft_printf("[DEBUG - 09] EQUIPOS EN EL TABLERO: ( %d ) - JUGADORES EN EL TABLERO: ( %d )\n", current_teams, players);

    if (current_teams == -1)
        return (-1);
    if (players == 0)
        return(GAME_OVER);
    if (initial_teams > 1 && current_teams == 1)
        return(VICTORY);
    return(ON_GOING);
}

void    play_turn(t_gamer *gamer)
{
    struct sembuf   sops;
    int             game_status;
    int             *game_board;
    bool            cleanup_mem = false;
    
    ft_printf("Player: %d - Team: %d. Attempted to access the dashboard...\n", gamer->player, gamer->team_id);
    
    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    if (semop(gamer->sem_id, &sops, 1) == -1)
    {
        if (errno == EIDRM || errno == EINVAL)
        {
            ft_printf("Player: %d - Team: %d. Game ended. The semaphore has been removed. 🏆 YOU WIN!!! 🏆\n", gamer->player, gamer->team_id);
            gamer->alive = false;
            return;
        }
        ft_printf("❌ Error: semop failed (lock) with code: ( %d ) ❌\n", errno);
        return;
    }
    
    ft_printf("Player: %d - Team: %d. Board access granted.\n", gamer->player, gamer->team_id);
    game_board = (int *)(gamer->board_ptr + 4 * sizeof(int));
    game_status = check_game_status(gamer);

    if (game_status == -1)
    {
        sops.sem_op = 1;
        semop(gamer->sem_id, &sops, 1);
        return;
    }

    if (game_status == GAME_OVER || game_status == VICTORY)
    {
        if (game_status == VICTORY)
        {
            ft_printf("Player: %d - Team: %d. 🏆 YOU WIN!!! 🏆\n", gamer->player, gamer->team_id);
            gamer->victory = true;
        }
        else
            ft_printf("⚠️ GAME OVER. No players left on the board. ⚠️\n");
        
        cleanup_mem = true;
    }
    else
    {
        if (is_surrounded(gamer))
        {
            ft_printf("Player: %d - Team: %d. ☠️ You are surrounded and eliminated. ☠️\n", gamer->player, gamer->team_id);
            gamer->alive = false;
            game_board[gamer->y * gamer->board_dim + gamer->x] = 0;
            *(int *)(gamer->board_ptr + sizeof(int)) -= 1;
            maybe_decrement_team(gamer, gamer->team_id);
        }
        else
            ft_move(gamer);
    }

    ft_printf("✅ Player: %d - Team: %d. Board released\n", gamer->player, gamer->team_id);

    sops.sem_num = 0;
    sops.sem_op = 1;
    sops.sem_flg = 0;
    if (semop(gamer->sem_id, &sops, 1) == -1)
    {
        ft_printf("❌ Error: semop failed (unlock) ❌\n");
        return;
    }

    if (cleanup_mem)
    {
        ft_printf("DEBUG: Last man standing or last player left. 🧹 Cleaning up IPC resources... 🧹\n");
        if (shmctl(gamer->shm_id, IPC_RMID, NULL) == -1)
            ft_printf("Error: Failed to remove shared memory\n");
        if (semctl(gamer->sem_id, 0, IPC_RMID) == -1)
            ft_printf("Error: Failed to remove semaphore\n");
        if (msgctl(gamer->msg_id, IPC_RMID, NULL) == -1)
            ft_printf("Error: Failde to remove message queue\n");

        gamer->alive = false;
    }

    usleep(100000);
}
