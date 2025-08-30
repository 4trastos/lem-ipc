#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

int argument_parsing(char **argv)
{
    int     i;

    i = 1;
    while (argv[i] != NULL)
    {
        if (ft_gameatoi(argv[i]) == -1)
        {
            ft_printf("Error: Invalid argument. just 'int'.\n", argv[i]);
            return (-1);
        }
        i++;
    }
    
    return (i);
}

int ft_gameatoi(char *str)
{
    long long num;
    int i;

    if (!str || !*str)
        return (-1);
    
    i = 0;
    while (str[i] != '\0')
    {
        if (str[i] < 0 || str[i] > 9)
            return (-1);
        i++;
    }

    num = 0;
    i = 0;
    while (str[i] != '\0')
    {
        num = num * 10 + (str[i] - '0');
        if (num > INT_MAX)
            return (-1);
        i++; 
    }

    return ((int)num);
}