#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

// Verificación si los recursos IPC ya existen
int    ft_resconf(t_gamer *gamer, key_t  key, int board)
{
    t_gamer  *aux;

    aux = gamer;
    aux->shmid = shmget(key, board, IPC_CREAT | IPC_EXCL | 0666);       // La Solicitud del Recurso
    if (aux->shmid == -1)
        return (2);
    else
        return (1);
}

int    player_one(t_gamer *gamer, key_t  key)
{
    t_gamer              *aux;
    union semaphunion   arg;
    struct sembuf       sops;

    aux = gamer;
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
    
    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    semop(aux->semid, &sops, 1);
    aux->board_ptr[0]++;
    aux->player = aux->board_ptr[0];
    sops.sem_op = 1;
    semop(aux->semid, &sops, 1);

    place_player_randomly(gamer);
    return (0);
}

int    other_player(t_gamer *gamer, key_t key)
{
    struct sembuf   sops;
    t_gamer         *aux;

    aux = gamer;
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

    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    semop(aux->semid, &sops, 1);
    aux->board_ptr[0]++;
    aux->player = aux->board_ptr[0];
    sops.sem_op = 1;
    semop(aux->semid, &sops, 1);

    aux->msgid = msgget(key, 0);                                        // Unirse a la cola de mensajes existente
    if (aux->msgid == -1)
    {
        ft_printf("Error: msgget failed\n");
        return (-1);
    }

    place_player_randomly(gamer);
    return (0);
}
