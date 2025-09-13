#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

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