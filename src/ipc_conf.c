#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

int    ft_resconf(t_game *game, key_t  key, int board)
{
    t_game  *aux;

    aux = game;
    aux->shmid = shmget(key, board, IPC_CREAT | IPC_EXCL | 0666);       // La Solicitud del Recurso
    if (aux->shmid == -1)
        return (2);
    else
        return (1);
}

int    player_one(t_game *game, key_t  key)
{
    t_game              *aux;
    union semaphunion   arg;

    aux = game;
    aux->player = 1;
    aux->board_ptr = shmat(aux->shmid, NULL, 0);                        // Adjuntar la memoria al proceso
    if (aux->board_ptr == (void *)-1)
    {
        ft_printf("Error: shmat failed\n");
        return (-1);
    }
    memset(aux->board_ptr, 0, aux->board_size);

    aux->semid = semget(key, 1, IPC_CREAT | 0666);
    if (aux->semid == -1)
    {
        ft_printf("Error: semget failed\n");
        return (-1);
    }

    arg.val = 1;
    if (semctl(aux->semid, 0, SETVAL, arg) == -1)
    {
        ft_printf("Error: semctl failed\n");
        return (-1);
    }

    aux->msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    if (aux->msgid == -1)
    {
        ft_printf("Error: msgget failed\n");
        return (-1);
    }
    return (0);
}

int    other_player(t_game *game, key_t key)
{
    t_game  *aux;

    aux = game;
    aux->player = 2;
    aux->shmid = shmget(key, 0, 0);                                     // Unirse a la memoria compartida existente
    aux->board_ptr = shmat(aux->shmid, NULL, 0);
    if (aux->board_ptr == (void *)-1)
    {
        ft_printf("Error: shmat failed\n");
        return (-1);
    }
    aux->semid = semget(key, 0, 0);                                     // Unirse al semaforo existente
    if (aux->semid == -1)
    {
        ft_printf("Error: semget failed\n");
        return (-1);
    } 
    aux->msgid = msgget(key, 0);                                        // Unirse a la cola de mensajes existente
    if (aux->msgid == -1)
    {
        ft_printf("Error: msgget failed\n");
        return (-1);
    }
    return (0);
}
