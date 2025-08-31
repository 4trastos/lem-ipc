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