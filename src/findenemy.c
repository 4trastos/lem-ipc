#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

bool    surrounded(t_gamer *gamer)
{
    int cell_value;
    int oponents_team[100];

    ft_memset(oponents_team, 0, sizeof(oponents_team));

    for (int y = gamer->y - 1; y <= gamer->y + 1; y++)
    {
        for (int x = gamer->x -1; x <= gamer->x + 1; x++)
        {
             if (y >= 0 && y < gamer->board_dim && x >= 0 && x < gamer->board_dim
                && (y != gamer->y || x != gamer->x))
            {
                cell_value = gamer->board_ptr[y * gamer->board_dim + x];
                if (cell_value != 0 && cell_value != gamer->team_id)
                    oponents_team[cell_value]++;
            }
        }
    }
    
    for (size_t i = 0; i < 100; i++)
    {
        if (oponents_team[i] >= 2)
        {
            gamer->alive = false;
            gamer->board_ptr[gamer->y * gamer->board_dim + gamer->x] = 0;
            ft_printf("Player: %d - Team: %d is surrounded and eliminated.\n", gamer->player, gamer->team_id);
            return (true);
        }
    }

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