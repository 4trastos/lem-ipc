#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

void    to_moveplayer(t_gamer *gamer, int target_y, int target_x)
{
    int step_y = 0;
    int step_x = 0;
    int new_x;
    int new_y;

    ft_printf("[DEBUG: 05] ENTRO A VER SI PUEDO MOVER. Player: %d\n", gamer->player);

    if (abs(gamer->x - target_x) >= abs(gamer->y - target_y))
    {
        if (target_x > gamer->x)
            step_x = 1;
        else if (target_x < gamer->x)
            step_x = - 1;
    }
    else
    {
        if (target_y > gamer->y)
            step_y = 1;
        else if (target_y < gamer->y)
            step_y = -1;
    }

    new_y = gamer->y + step_y;
    new_x = gamer->x + step_x;
    if (new_x >= 0 && new_x < gamer->board_dim && new_y >= 0 && new_y < gamer->board_dim
        && gamer->board_ptr[new_y * gamer->board_dim + new_x] == 0)
    {
        gamer->board_ptr[gamer->y * gamer->board_dim + gamer->x] = 0;
        gamer->y = new_y;
        gamer->x = new_x;
        gamer->board_ptr[gamer->y * gamer->board_dim + gamer->x] = gamer->team_id;
        ft_printf("🚀 Player: %d - Team: %d moved to (%d, %d) 🚀\n", gamer->player, gamer->team_id, gamer->x, gamer->y);
        return;
    }

    ft_printf("⛔ Player: %d - Team: %d. No available moves, waiting... ⛔\n", gamer->player, gamer->team_id);
    ft_printf("⛔ Another player is needed ⛔\n");
}