#ifndef LEMIPC_H
# define LEMIPC_H

# include <sys/types.h>
# include <sys/wait.h>
# include <sys/ipc.h>
# include <sys/sem.h>
# include <sys/shm.h>
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
int     ft_resconf(t_game *game, key_t key, int board);
int     player_one(t_game *game, key_t  key);
int     other_player(t_game *game, key_t key);
void    clearmemsem(t_game *game);

//*** auxiliary functions ***
int     ft_gameatoi(char *str);
int     argument_parsing(char **argv);

#endif