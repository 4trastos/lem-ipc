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
# include <errno.h>
# include <signal.h>

# define    ON_GOING    0
# define    VICTORY     1
# define    GAME_OVER   2

/* Estructura de memoria corregida:

0x0000: [int] board_dim (4 bytes)
0x0004: [int] player_count (4 bytes)  
0x0008: [int] celda (0,0) (4 bytes)
0x000C: [int] celda (0,1) (4 bytes)
...
0x...: [int] celda (99,99) (4 bytes) */

union semaphunion
{
    int                 val;
    struct sem_id_ds    *buf;
    unsigned short      *array;
};

typedef struct s_gamer
{
    int             team_id;        // Equipo
    int             player;         // Nº de jugador
    int             shm_id;         // ID Memoria Compartida
    int             sem_id;         // ID Semaforos
    int             msg_id;         // ID Colas de Mensajes
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

typedef struct s_messenger
{
    long            mtype;
    int             team_id;
    int             target;
    int             target_x;
    int             target_y;
}   t_messenger;

extern t_gamer *global_gamer;

//*** main functions ***/
void    play_turn(t_gamer *gamer);
void    clearmemsem(t_gamer *gamer);
void    cleanup_orphaned_ipc(key_t key);

//*** game logic ***/
bool    is_surrounded(t_gamer *gamer);
bool    find_enemy_target(t_gamer *gamer, int *target_y, int *target_x);
int     ft_resconf(t_gamer *gamer, key_t key, int board);
int     player_one(t_gamer *gamer, key_t  key);
int     other_player(t_gamer *gamer, key_t key);
int     check_game_status(t_gamer *gamer);
int     get_total_teams(t_gamer *gamer);
int     argument_parsing(char **argv, int *board);
void    to_moveplayer(t_gamer *gamer, int target_y, int target_x);
void    place_player_randomly(t_gamer *gamer);
void    ft_move(t_gamer *player);

//*** comunications */
int     receive_message(t_gamer *gamer);
void    send_message(t_gamer *gamer, int target_y, int target_x);

//*** auxiliary functions ***/
int     ft_gameratoi(char *str);
void    *ft_memset(void *s, int c, size_t n);
void    *ft_memcpy(void *dst, const void *sc, size_t n);

#endif