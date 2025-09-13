#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

void    *ft_memset(void *s, int c, size_t n)
{
    unsigned char   *ptr;
    unsigned char   value;
    
    ptr = s;
    value = c;
    while (n-- > 0)
        *ptr++ = value;
    return (s);
}

void    *ft_memcpy(void *dst, const void *sc, size_t n)
{
    unsigned char       *dest;
    unsigned const char *src;

    dest = dst;
    src = sc;
    while (n-- > 0)
        *dest++ = *src++;
    return (dst);
}

int ft_gameratoi(char *str)
{
    long long num;
    int i;

    if (!str || !*str)
        return (-1);
    
    i = 0;
    while (str[i] != '\0')
    {
        if (str[i] < '0' || str[i] > '9')
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