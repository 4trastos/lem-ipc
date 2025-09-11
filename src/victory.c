#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

bool    check_for_victory(t_gamer *gamer)
{
    if (get_total_teams(gamer) == 1)
        return (true);
    return (false);
}
