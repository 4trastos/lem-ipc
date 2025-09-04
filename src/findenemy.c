#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

bool    surrounded(t_gamer *gamer)
{
    int limits;
    int blocked_count;

    limits = gamer->board_dim;
    blocked_count = 0;

    if ((gamer->x + 1 < limits) && (gamer->board_ptr[gamer->y * limits + gamer->x + 1] != 0))
        blocked_count++;
    if ((gamer->x - 1 >= 0) && (gamer->board_ptr[gamer->y * limits + gamer->x - 1] != 0))
        blocked_count++;
    if ((gamer->y + 1 < limits) && (gamer->board_ptr[(gamer->y + 1) * limits + gamer->x] != 0))
        blocked_count++;
    if ((gamer->y - 1 >= 0) && (gamer->board_ptr[(gamer->y -1) * limits + gamer->x] != 0))
        blocked_count++;
    if ((gamer->x + 1 < limits) && (gamer->y + 1 < limits) && (gamer->board_ptr[(gamer->y + 1) * limits + gamer->x + 1] != 0))
        blocked_count++;
    if ((gamer->x + 1 < limits) && (gamer->y - 1 >= 0) && (gamer->board_ptr[(gamer->y - 1) * limits + gamer->x + 1] != 0))
        blocked_count++;
    if ((gamer->x - 1 >= 0) && (gamer->y + 1 < limits) && (gamer->board_ptr[(gamer->y + 1) * limits + gamer->x - 1] != 0))
        blocked_count++;
    if ((gamer->x - 1 >= 0) && (gamer->y - 1 >= 0) && (gamer->board_ptr[(gamer->y - 1) * limits + gamer->x - 1] != 0))
        blocked_count++;
    if (blocked_count >= 7)
    {
        gamer->alive = false;
        ft_printf("Player: %d - Team: %d is surrounded and eliminated.\n", gamer->player, gamer->team_id);
        return (true);
    }
    else
        return (false);
}

void    ft_move(t_gamer *gamer)
{
    int target_y;
    int target_x;

    if (surrounded(gamer))
        return;
    if (find_enemy_target(gamer, &target_y, &target_x))
        to_moveplayer(gamer, target_y, target_x);
    else
        ft_printf("Waiting for an opponent\n");
}

bool    find_enemy_target(t_gamer *gamer, int *target_y, int *target_x)
{
    int     y;
    int     x;
    int     cell_value;
    int     enemy_distance;
    int     min_distance;
    bool    found_enemy;    

    min_distance = INT_MAX;
    cell_value = 0;
    for (y = 0; y < gamer->board_dim; y++)
    {
        for (x = 0; x < gamer->board_dim; x++)
        {
            cell_value = gamer->board_ptr[y * gamer->board_dim + x];
            if (cell_value != gamer->team_id && cell_value != 0)
            {
                enemy_distance = abs(gamer->x - x) + abs(gamer->y - y);
                if (enemy_distance < min_distance)
                {
                    min_distance = enemy_distance;
                    *target_y = y;
                    *target_x = x;
                    found_enemy = true;
                }
            }
        }
    }
    return (found_enemy);
}