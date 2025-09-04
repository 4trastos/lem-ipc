#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

void    to_moveplayer(t_gamer *gamer, int target_y, int target_x)
{
    int limits;
    int step_y = 0;
    int step_x = 0;
    int new_x;
    int new_y;

    limits = gamer->board_dim;
    if (target_x > gamer->x)
        step_x = 1;
    else if (target_x < gamer->x)
        step_x = -1;

    if (target_y > gamer->y)
        step_y = 1;
    else if (target_y < gamer->y)
        step_y = -1;

    new_y = gamer->y + step_y;
    new_x = gamer->x + step_x;
    if ((new_y >= 0 && new_y < limits) && (new_x >= 0 && new_x < limits)
        && (gamer->board_ptr[new_y * limits + new_x] == 0))
    {
        gamer->board_ptr[gamer->y * limits + gamer->x] = 0;
        gamer->y = new_y;
        gamer->x = new_x;
        gamer->board_ptr[gamer->y * limits + gamer->x] = gamer->team_id;
        ft_printf("Player: %d - Team: %d moved to (%d, %d)\n", gamer->player, gamer->team_id, gamer->x, gamer->y);
    }
}