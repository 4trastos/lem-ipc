#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

void    to_moveplayer(t_gamer *gamer, int target_y, int target_x)
{
    int     step_y = 0;
    int     step_x = 0;
    int     new_x;
    int     new_y;
    int     *game_board = (int *)(gamer->board_ptr + 3 * sizeof(int));
    int     min_distance = INT_MAX;
    bool    move = false;

    ft_printf("[DEBUG: 05] ENTRO A VER SI PUEDO MOVER. Player: %d\n", gamer->player);

    int moves[4][2] = {{0, -1}, {0, 1}, {1, 0}, {-1, 0}};

    for (int i = 0; i < gamer->board_dim; i++)
    {
        new_x = gamer->x + moves[i][0];
        new_y = gamer->y + moves[i][1];

        if (new_y >= 0 && new_y < gamer->board_dim &&
            new_x >= 0 && new_x < gamer->board_dim &&
            game_board[new_y * gamer->board_dim + new_x] == 0)
        {
            int current_distance = abs(new_x - target_x) + abs(new_y - target_y);
            if (current_distance < min_distance)
            {
                min_distance = current_distance;
                step_x = moves[i][0];
                step_y = moves[i][1];
                move = true;
            }
        }
    }

    if (move)
    {
        new_x = gamer->x + step_x;
        new_y = gamer->y + step_y;

        if (new_x >= 0 && new_x < gamer->board_dim &&
            new_y >= 0 && new_y < gamer->board_dim &&
            game_board[new_y * gamer->board_dim + new_x] == 0)
        {
            int oldx = gamer->x;
            int oldy = gamer->y;
    
            gamer->x = new_x;
            gamer->y = new_y;
            
            game_board[oldy * gamer->board_dim + oldx] = 0;
            game_board[gamer->y * gamer->board_dim + gamer->x] = gamer->team_id;
            ft_printf("🚀 Player: %d - Team: %d moved to (%d, %d) 🚀\n", gamer->player, gamer->team_id, gamer->y, gamer->x);
        }
        else
            ft_printf("⛔ Move became invalid during calculation ⛔\n");

    }
    else
    {
        ft_printf("⛔ Player: %d - Team: %d. No available moves, waiting... ⛔\n", gamer->player, gamer->team_id);
        ft_printf("⛔ Another player is needed ⛔\n");
    }
}