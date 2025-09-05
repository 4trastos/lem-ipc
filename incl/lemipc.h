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
    int             board_size;     // Tamaño tablero
    int             board_dim;      // Tamaño del lateral del tablero
    int             x;              // Posición X en tablero
    int             y;              // Posición Y en tablero
    unsigned char   *board_ptr;     // RAM
    bool            alive;          // Vivo o Muerto
    bool            victory;        // Victoria de equipo
    pid_t           pid;            // Proceso
    char            padding[6];     // Padding explícito (Alineado a 64bits) Total: 56 bytes (múltiplo de 8)
}   t_gamer;

//*** gamer functions ***
void    play_turn(t_gamer *gamer);
void    place_player_randomly(t_gamer *gamer);
bool    surrounded(t_gamer *gamer);

//*** explicit functions ***
int     ft_resconf(t_gamer *gamer, key_t key, int board);
int     player_one(t_gamer *gamer, key_t  key);
int     other_player(t_gamer *gamer, key_t key);
void    clearmemsem(t_gamer *gamer);
void    ft_move(t_gamer *player);
bool    find_enemy_target(t_gamer *gamer, int *target_y, int *target_x);
void    to_moveplayer(t_gamer *gamer, int target_y, int target_x);

//*** auxiliary functions ***
int     ft_gameratoi(char *str);
int     argument_parsing(char **argv, int *board);
void    *ft_memset(void *s, int c, size_t n);
void    *ft_memcpy(void *dst, const void *sc, size_t n);

#endif