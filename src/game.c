#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

int get_total_teams(t_gamer *gamer)
{
    int     team_count;
    int     cell_value;
    int     max_teams = 100;
    bool    team_present[max_teams + 1];
    int     *game_board = (int *)(gamer->board_ptr + 2 * sizeof(int));

    for (int i = 0; i <= max_teams; i++)
        team_present[i] = false;
    
    for (int y = 0; y < gamer->board_dim; y++)
    {
        for (int x = 0; x < gamer->board_dim; x++)
        {
            cell_value = game_board[y * gamer->board_dim + x];
            if (cell_value != 0 && !team_present[cell_value])
            {
                team_present[cell_value] = true;
                team_count++;
            }
        }
    }
    return (team_count);
}

int check_game_status(t_gamer *gamer)
{
    int teams;
    int players;

    teams = get_total_teams(gamer);
    players = *(int*)(gamer->board_ptr + sizeof(int));

    if (players == 0)
        return(GAME_OVER);
    if (teams == 1)
        return(VICTORY);
    return(ON_GOING);
}

void    play_turn(t_gamer *gamer)
{
    struct sembuf   sops;
    int game_status;
    
    ft_printf("Player: %d - Team: %d. Attempted to access the dashboard...\n", gamer->player, gamer->team_id);
    
    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    if (semop(gamer->sem_id, &sops, 1) == -1)
    {
        ft_printf("❌ Error: semop failed (lock) ❌\n");
        return;
    }
    
    ft_printf("Player: %d - Team: %d. Board access granted.\n", gamer->player, gamer->team_id);
    
    game_status = check_game_status(gamer);

    if (game_status == GAME_OVER)
    {
        ft_printf("⚠️ GAME OVER. No players left on the board. ⚠️\n");
        usleep(500000);
        sops.sem_op = 1;
        semop(gamer->sem_id, &sops, 1);
        return;
    }

    if (game_status == VICTORY)
    {
        ft_printf("Player: %d - Team: %d. 🏆 YOU WIN!!! 🏆\n", gamer->player, gamer->team_id);
        gamer->alive = false;
        gamer->victory = true;
        sops.sem_op = 1;
        semop(gamer->sem_id, &sops, 1);
        return;
    }
    else
    {
        if (is_surrounded(gamer))
        {
            ft_printf("Player: %d - Team: %d. ☠️ You are surrounded and eliminated. ☠️\n", gamer->player, gamer->team_id);
            *(int *)(gamer->board_ptr + sizeof(int)) -= 1;
            gamer->board_ptr[gamer->y * gamer->board_size + gamer->x] = 0;
            gamer->alive = false;
        }
        else
            ft_move(gamer);
    }

    ft_printf("✅ Player: %d - Team:%d. Board released\n", gamer->player, gamer->team_id);

    sops.sem_num = 0;
    sops.sem_op = 1;
    sops.sem_flg = 0;
    if (semop(gamer->sem_id, &sops, 1) == -1)
    {
        ft_printf("❌ Error: semop failed (unlock) ❌\n");
        return;
    }

    usleep(100000);
}
