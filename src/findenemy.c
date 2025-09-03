#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

void    surrounded(t_gamer *gamer)
{
    int y = 0;
    int x = 0;
    int limit = gamer->board_dim;
    while (gamer->y != limit && gamer->x != limit)
    {
        if (gamer->y )
    }
    
}

void    ft_move(t_gamer *gamer)
{
    int target_y;
    int target_x;

    if (find_enemy_target(gamer, &target_y, &target_x))
    {
        surrounded(gamer);
        if (gamer->alive)
            to_moveplayer(gamer, target_y, target_x);
    }
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