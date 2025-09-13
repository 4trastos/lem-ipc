#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

bool is_surrounded(t_gamer *gamer)
{
    int cell_value;
    int enemy_count = 0;
    int *game_board = (int *)(gamer->board_ptr + 3 * sizeof(int));

    ft_printf("[DEBUG - 02] Check if surrounded. Player: %d\n", gamer->player);

    for (int y = gamer->y - 1; y <= gamer->y + 1; y++)
    {
        for (int x = gamer->x - 1; x <= gamer->x + 1; x++)
        {
            if (y >= 0 && y < gamer->board_dim && x >= 0 && x < gamer->board_dim
                && (y != gamer->y || x != gamer->x))
            {
                cell_value = game_board[y * gamer->board_dim + x];
                if (cell_value != 0 && cell_value != gamer->team_id)
                    enemy_count++;
            }
        }
    }

    ft_printf("[DEBUG - 02] Player %d has %d enemies around\n", gamer->player, enemy_count);

    return (enemy_count >= 2);
}


void    ft_move(t_gamer *gamer)
{
    int target_y = 0;
    int target_x = 0;

    if (receive_message(gamer))
        return;

    if (find_enemy_target(gamer, &target_y, &target_x))
    {
        to_moveplayer(gamer, target_y, target_x);
        send_message(gamer, target_y, target_x);
    }
    else
        ft_printf("⚠️ Waiting for an opponent ⚠️\n");
}

bool    find_enemy_target(t_gamer *gamer, int *target_y, int *target_x)
{
    int     *game_board = (int *)(gamer->board_ptr + 3 * sizeof(int));
    int     cell_value;
    int     enemy_distance;
    int     min_distance;
    int     target_team;
    bool    found_target;    

    min_distance = INT_MAX;
    target_team = 0;
    found_target = false;

    ft_printf("[DEBUG - 04] ENTRO A BUSCAR UN ONJETIVO- Player: %d\n", gamer->player);

    for (int y = 0; y < gamer->board_dim; y++)
    {
        for (int x = 0; x < gamer->board_dim; x++)
        {
            cell_value = game_board[y * gamer->board_dim + x];
            if (cell_value != 0 && cell_value != gamer->team_id)
            {
                enemy_distance = abs(gamer->x - x) + abs(gamer->y - y);
                if (enemy_distance < min_distance)
                {
                    min_distance = enemy_distance;
                    *target_y = y;
                    *target_x = x;
                    target_team = cell_value; 
                    found_target = true;
                }
            }
        }
    }
    if (found_target)
        ft_printf("## OBJETIVO ENCONTRADO => Team: %d - Coordendas: (%d, %d)\n", target_team, *target_x, *target_y);
    
    return (found_target);
}