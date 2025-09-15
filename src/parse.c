#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

void maybe_decrement_team(t_gamer *gamer, int team_id)
{
    int *game_board = (int *)(gamer->board_ptr + 4 * sizeof(int));
    bool team_still_exists = false;
    
    for (int i = 0; i < gamer->board_size; i++)
    {
        if (game_board[i] == team_id)
        {
            team_still_exists = true;
            break;
        }
    }
    
    if (!team_still_exists)
    {
        int *total_teams = (int *)(gamer->board_ptr + 2 * sizeof(int));
        if (*total_teams > 0)
        {
            (*total_teams)--;
            ft_printf("Team %d eliminated! Total teams now: %d\n", team_id, *total_teams);
        }
    }
}

int argument_parsing(char **argv, int *board)
{
    int     i;
    int     size;

    i = 1;
    size = 0;
    *board = 10000;
    while (argv[i] != NULL)
    {
        if (ft_gameratoi(argv[i]) == -1)
        {
            ft_printf("Error: Invalid argument. just 'int'.\n", argv[i]);
            return (-1);
        }
        if (i == 3)
        {
            size = ft_gameratoi(argv[i]);
            if (size <= 0 || size > 100)
            {
                ft_printf("Error: The length of the board side must be a value between 1 and 100.\n");
                return (-1);
            }
            *board = size * size *sizeof(int);
        }
        i++;
    }
    return (0);
}