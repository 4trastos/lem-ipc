#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

bool    surrounded(t_gamer *gamer)
{
    int     y;
    int     x;
    int     team_id;
    int     team_counts[100];
    bool    is_surrounded;

    is_surrounded = false;
    ft_memset(team_counts, 0, sizeof(team_counts));

    for (y = gamer->y - 1; y <= gamer->y + 1; y++)
    {
        for (x = gamer->x -1; x <= gamer->x + 1; x++)
        {
             if (y >= 0 && y < gamer->board_dim && x >= 0 && x < gamer->board_dim
                && (y != gamer->y || x != gamer->x))
            {
                team_id = gamer->board_ptr[y * gamer->board_dim + x];
                if (team_id != 0 && team_id != gamer->team_id)
                    team_counts[team_id]++;
            }
        }
    }
    
    for (size_t i = 0; i < sizeof(team_counts); i++)
    {
        if (team_counts[i] >= 2)
        {
            is_surrounded = true;
            break;
        }
    }

    if (is_surrounded)
    {
        gamer->alive = false;
        gamer->board_ptr[y * gamer->board_dim + x] = 0;
        ft_printf("Player: %d - Team: %d is surrounded and eliminated.\n", gamer->player, gamer->team_id);
    }
    return (is_surrounded);
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