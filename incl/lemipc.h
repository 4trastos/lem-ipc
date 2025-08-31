#ifndef LEMIPC_H
# define LEMIPC_H

# include <sys/types.h>
# include <sys/wait.h>
# include <sys/ipc.h>
# include <sys/sem.h>
# include <sys/shm.h>
# include <sys/msg.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <signal.h>
# include <limits.h>
# include <stdbool.h>
# include <time.h>
# include <math.h>

union semaphunion
{
    int             val;
    struct semid_ds *buf;
    unsigned short  *array;
};

typedef struct s_gamer
{
    int             team_id;        // Equipo
    int             player;         // Nº de jugador
    int             shmid;          // ID Memoria Compartida
    int             semid;          // Semaforos
    int             msgid;          // Colas de Mensajes
    unsigned char   *board_ptr;     // RAM
    int             board_size;     // Tamaño tablero
    int             x;              // Posición X en tablero
    int             y;              // Posición Y en tablero
    bool            alive;          // Vivo o Muerto
    bool            victory;        // Victoria de equipo
    pid_t           pid;            // Proceso
}   t_gamer;

//*** gamer functions ***
void    play_turn(t_gamer *gamer);
void    place_player_randomly(t_gamer *gamer);

//*** explicit functions ***
int     ft_resconf(t_gamer *gamer, key_t key, int board);
int     player_one(t_gamer *gamer, key_t  key);
int     other_player(t_gamer *gamer, key_t key);
void    clearmemsem(t_gamer *gamer);

//*** auxiliary functions ***
int     ft_gameratoi(char *str);
int     argument_parsing(char **argv, int *board);

#endif