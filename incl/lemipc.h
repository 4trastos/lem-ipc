#ifndef LEMIPC_H
# define LEMIPC_H

# include <sys/types.h>
# include <sys/ipc.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>

typedef struct s_game
{
    int     team_id;    // Equipo
    int     shmid;      // Memoria Compartida
    int     semid;      // Semaforos
    int     msgid;      // Colas de Mensajes
    int     *board_ptr; //  RAM
    int     x;          // Posición X en tablero
    int     y;          // Posición Y en tablero
    pid_t   pid;        // Proceso
}   t_game;

//*** struct functions ***

//*** explicit functions ***
key_t   ftok(const char *pathname, int proj_id);

//*** auxiliary functions ***
int     ft_atoi(char *str);

#endif