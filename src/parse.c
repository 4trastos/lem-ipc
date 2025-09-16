#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

void maybe_decrement_team(t_gamer *gamer, int team_id)
{
    int *game_board = (int *)(gamer->board_ptr + 4 * sizeof(int));
    int *teams_in_game = (int *)(gamer->board_ptr + 2 * sizeof(int));
    bool team_exists = false;
    
    for (int i = 0; i < gamer->board_size; i++)
    {
        if (game_board[i] == team_id)
        {
            team_exists = true;
            break;
        }
    }
    
    if (!team_exists)
    {
        if (*teams_in_game > 0)
        {
            (*teams_in_game)--;
            ft_printf("☠️ Team %d eliminated! Total teams now: %d ☠️\n", team_id, *teams_in_game);
        }
    }
    else
        ft_printf("Team %d still has members (no decrement)\n", team_id);
}

int argument_parsing(char **argv, int *board)
{
    int     i;
    int     side;

    i = 1;
    side = 40;
    *board = 0;
    while (argv[i] != NULL)
    {
        if (ft_gameratoi(argv[i]) == -1)
        {
            ft_printf("Error: Invalid argument. just 'int'.\n", argv[i]);
            return (-1);
        }
        if (i == 3) // Argumento opcional: lado del tablero
        {
            side = ft_gameratoi(argv[i]);
            if (side <= 0 || side > 100)
            {
                ft_printf("Error: The length of the board side must be a value between 1 and 100.\n");
                return (-1);
            }
        }
        i++;
    }
    *board = side * side;
    return (0);
}