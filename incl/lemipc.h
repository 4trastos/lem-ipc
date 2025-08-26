#ifndef LEMIPC_H
# define LEMIPC_H

# include <sys/types.h>
# include <sys/ipc.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>

//*** struct functions ***

//*** explicit functions ***
key_t   ftok(const char *pathname, int proj_id);

//*** auxiliary functions ***

#endif