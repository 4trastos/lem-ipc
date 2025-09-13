#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

static bool bfs_find_step(t_gamer *gamer, int target_y, int target_x, int *step_y, int *step_x)
{
    int dim = gamer->board_dim;
    int *game_board = (int *)(gamer->board_ptr + 3 * sizeof(int));
    bool visited[dim][dim];
    t_node *queue = malloc(sizeof(t_node) * dim * dim);
    int front = 0, rear = 0;

    if (!queue)
        return false;

    ft_memset(visited, false, sizeof(visited));

    queue[rear++] = (t_node){gamer->x, gamer->y, -1, -1};
    visited[gamer->y][gamer->x] = true;

    int moves[4][2] = {{0,-1},{0,1},{1,0},{-1,0}};

    while (front < rear)
    {
        t_node current = queue[front++];

        // Si la celda actual es la adyacente al objetivo, encontramos el camino.
        if (abs(current.x - target_x) + abs(current.y - target_y) == 1 && game_board[current.y * dim + current.x] == 0)
        {
            t_node path_node = current;
            t_node prev_node = current;
            while (path_node.px != -1 && path_node.py != -1)
            {
                prev_node = path_node;
                bool found_parent = false;
                for (int i = 0; i < rear; i++)
                {
                    if (queue[i].x == path_node.px && queue[i].y == path_node.py)
                    {
                        path_node = queue[i];
                        found_parent = true;
                        break;
                    }
                }
                if (!found_parent)
                    break;
            }
            *step_x = prev_node.x - gamer->x;
            *step_y = prev_node.y - gamer->y;
            free(queue);
            return true;
        }

        for (int i = 0; i < 4; i++)
        {
            int nx = current.x + moves[i][0];
            int ny = current.y + moves[i][1];

            if (nx < 0 || nx >= dim || ny < 0 || ny >= dim)
                continue;
            if (visited[ny][nx])
                continue;
            
            // Solo podemos movernos a celdas vacías.
            if (game_board[ny * dim + nx] == 0)
            {
                visited[ny][nx] = true;
                queue[rear++] = (t_node){nx, ny, current.x, current.y};
            }
        }
    }
    free(queue);
    return false;
}

void to_moveplayer(t_gamer *gamer, int target_y, int target_x)
{
    int step_x = 0, step_y = 0;
    int new_x, new_y;
    int *game_board = (int *)(gamer->board_ptr + 3 * sizeof(int));

    if (bfs_find_step(gamer, target_y, target_x, &step_y, &step_x))
    {
        new_x = gamer->x + step_x;
        new_y = gamer->y + step_y;

        // Mover jugador
        game_board[gamer->y * gamer->board_dim + gamer->x] = 0;
        gamer->x = new_x;
        gamer->y = new_y;
        game_board[gamer->y * gamer->board_dim + gamer->x] = gamer->team_id;

        ft_printf("🚀 Player %d moved to (%d, %d)\n", gamer->player, gamer->y, gamer->x);
    }
    else
        ft_printf("⛔ Player %d - No path to target\n", gamer->player);
}


/* void    to_moveplayer(t_gamer *gamer, int target_y, int target_x)
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
} */