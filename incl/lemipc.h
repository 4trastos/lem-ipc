#ifndef LEMIPC_H
# define LEMIPC_H

# include <sys/types.h>
# include <sys/wait.h>
# include <sys/ipc.h>
# include <sys/mman.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <signal.h>
# include <limits.h>

typedef struct s_game
{
    int     team_id;        // Equipo
    int     player;         // Nº de jugador
    int     shmid;          // Memoria Compartida
    int     semid;          // Semaforos
    int     msgid;          // Colas de Mensajes
    int     *board_ptr;     //  RAM
    int     x;              // Posición X en tablero
    int     y;              // Posición Y en tablero
    pid_t   pid;            // Proceso
}   t_game;

//*** struct functions ***

//*** explicit functions ***
key_t   ftok(const char *pathname, int proj_id);
int     shmget(key_t key, int size, int shmflg);
void    *shmat(int shmid, const void *shmaddr, int shmflg);

//*** auxiliary functions ***
int     ft_gameatoi(char *str);
int     argument_parsing(char **argv);

#endif