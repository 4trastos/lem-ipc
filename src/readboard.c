#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

void    ft_move(t_gamer *player)
{
    int target_y;
    int target_x;
    int distance = player->board_size;

    enemy_target(player, &target_y, &target_x, distance);
}

void    enemy_target(t_gamer *player, int *target_y, int *target_x, int distance)
{
    int     y;
    int     x;
    int     cell_value;
    int     enemy_distance;

    cell_value = 0;
    for (y = 0; y < player->board_dim; y++)
    {
        for (x = 0; x < player->board_dim; x++)
        {
            cell_value = player->board_ptr[y * player->board_dim + x];
            if (cell_value != player->team_id)
            {
                *target_y = y;
                *target_x = x;
            }
        }
    }
}