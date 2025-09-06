#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

bool    check_for_victory(t_gamer *gamer)
{
    int     cell_value;

    cell_value = 0;
    for (int y = 0; y < gamer->board_dim; y++)
    {
        for (int x = 0; x < gamer->board_dim; x++)
        {
            cell_value = gamer->board_ptr[y * gamer->board_dim + x];
            if (cell_value != 0 && cell_value != gamer->team_id)
                return (false);
        }
    }
    return (true);
}