#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

int get_total_teams(t_gamer *gamer)
{
    int     team_count = 0;
    int     cell_value;
    int     max_teams = 100;
    bool    team_present[max_teams + 1];
    int     *game_board = (int *)(gamer->board_ptr + 3 * sizeof(int));

    ft_printf("[DEBUG - 07] GET TOTAL TEAMS\n");
    for (int i = 0; i <= max_teams; i++)
        team_present[i] = false;
    
    for (int y = 0; y < gamer->board_dim; y++)
    {
        for (int x = 0; x < gamer->board_dim; x++)
        {
            cell_value = game_board[y * gamer->board_dim + x];
            //ft_printf("[DEBUG - 06] VALOR DE LA CELDA DE RAM CONTANDO EQIUPOS: %d\n", cell_value);
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
    int teams;
    int players;
    int initial_teams;

    ft_printf("[DEBUG - 06] CHECK GAME ESTATUS\n");
    initial_teams = *(int*)(gamer->board_ptr + 2 * sizeof(int));
    teams = get_total_teams(gamer);
    players = *(int*)(gamer->board_ptr + sizeof(int));
    ft_printf("[DEBUG - 08] EQUIPOS DE INICIO: ( %d )\n", initial_teams);
    ft_printf("[DEBUG - 09] EQUIPOS EN EL TABLERO: ( %d ) - JUGADORES EN EL TABLERO: ( %d )\n", teams, players);

    if (teams == -1)
        return (-1);
    if (players == 0)
        return(GAME_OVER);
    if (initial_teams > 1 && teams == 1)
        return(VICTORY);
    return(ON_GOING);
}

void    play_turn(t_gamer *gamer)
{
    struct sembuf   sops;
    int             game_status;
    int             *game_board;
    
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
    game_board = (int *)(gamer->board_ptr + 3 * sizeof(int));
    game_status = check_game_status(gamer);

    if (game_status == -1)
    {
        sops.sem_op = 1;
        semop(gamer->sem_id, &sops, 1);
        return;
    }

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
            game_board[gamer->y * gamer->board_dim + gamer->x] = 0;
            gamer->alive = false;
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

    usleep(100000);
}
