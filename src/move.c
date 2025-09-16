#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

void to_moveplayer(t_gamer *gamer, int target_y, int target_x)
{
    int step_x = 0;
    int step_y = 0;
    int new_x;
    int new_y;
    int *game_board = (int *)(gamer->board_ptr + 4 * sizeof(int));
    int min_distance = INT_MAX;
    bool move = false;

    int moves[4][2] = {{0, -1}, {0, 1}, {1, 0}, {-1, 0}}; // Arriba, Abajo, Derecha, Izquierda
    int best_moves[4][2];
    int best_count = 0;

    for (int i = 0; i < 4; i++)
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
                best_count = 0;
                best_moves[best_count][0] = moves[i][0];
                best_moves[best_count][1] = moves[i][1];
                best_count = 1;
                move = true;
            }
            else if (current_distance == min_distance)
            {
                best_moves[best_count][0] = moves[i][0];
                best_moves[best_count][1] = moves[i][1];
                best_count++;
                move = true;
            }
        }
    }

    if (move)
    {
        // Elegir un movimiento al azar entre los mejores
        int idx = rand() % best_count;
        step_x = best_moves[idx][0];
        step_y = best_moves[idx][1];

        new_x = gamer->x + step_x;
        new_y = gamer->y + step_y;

        int oldx = gamer->x;
        int oldy = gamer->y;

        gamer->x = new_x;
        gamer->y = new_y;

        game_board[oldy * gamer->board_dim + oldx] = 0;
        game_board[gamer->y * gamer->board_dim + gamer->x] = gamer->team_id;
        ft_printf("🚀 Player: %d - Team: %d moved to (%d, %d) 🚀\n",
                  gamer->player, gamer->team_id, gamer->y, gamer->x);
    }
    else
    {
        ft_printf("⛔ Player: %d - Team: %d. No available moves, waiting... ⛔\n",
                  gamer->player, gamer->team_id);
    }
}
