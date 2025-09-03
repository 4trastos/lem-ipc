#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

// Verificación si los recursos IPC ya existen
int    ft_resconf(t_gamer *gamer, key_t  key, int board)
{
    gamer->shmid = shmget(key, board, IPC_CREAT | IPC_EXCL | 0666);       // La Solicitud del Recurso
    if (gamer->shmid == -1)
        return (2);
    else
        return (1);
}

int    player_one(t_gamer *gamer, key_t  key)
{
    int                 player_count;
    union semaphunion   arg;
    struct sembuf       sops;

    gamer->board_ptr = shmat(gamer->shmid, NULL, 0);                        // Adjuntar la memoria al proceso
    if (gamer->board_ptr == (void *)-1)
    {
        ft_printf("Error: shmat failed\n");
        return (-1);
    }

    ft_memset(gamer->board_ptr, 0, gamer->board_size);
    ft_memcpy(gamer->board_ptr, &gamer->board_dim, sizeof(int));
    
    gamer->semid = semget(key, 1, IPC_CREAT | 0666);
    if (gamer->semid == -1)
    {
        ft_printf("Error: semget failed\n");
        return (-1);
    }
    
    arg.val = 1;
    if (semctl(gamer->semid, 0, SETVAL, arg) == -1)
    {
        ft_printf("Error: semctl failed\n");
        return (-1);
    }
    
    gamer->msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    if (gamer->msgid == -1)
    {
        ft_printf("Error: msgget failed\n");
        return (-1);
    }
    
    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    semop(gamer->semid, &sops, 1);

    player_count = *(int *)(gamer->board_ptr + sizeof(int));
    player_count = player_count + 1;
    *(int *)(gamer->board_ptr + sizeof(int)) = player_count;
    gamer->player = player_count;
    
    sops.sem_op = 1;
    semop(gamer->semid, &sops, 1);

    place_player_randomly(gamer);
    return (0);
}

int    other_player(t_gamer *gamer, key_t key)
{
    int             player_count;
    struct sembuf   sops;

    gamer->shmid = shmget(key, 0, 0);                                     // Unirse a la memoria compartida existente
    gamer->board_ptr = shmat(gamer->shmid, NULL, 0);
    if (gamer->board_ptr == (void *)-1)
    {
        ft_printf("Error: shmat failed\n");
        return (-1);
    }

    ft_memcpy(&gamer->board_dim, gamer->board_ptr, sizeof(int));
    gamer->board_size = gamer->board_dim * gamer->board_dim;

    gamer->semid = semget(key, 0, 0);                                     // Unirse al semaforo existente
    if (gamer->semid == -1)
    {
        ft_printf("Error: semget failed\n");
        return (-1);
    }

    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    semop(gamer->semid, &sops, 1);

    player_count = *(int *)(gamer->board_ptr + sizeof(int));
    player_count = player_count + 1;
    *(int *)(gamer->board_ptr + sizeof(int)) = player_count;
    gamer->player = player_count;

    sops.sem_op = 1;
    semop(gamer->semid, &sops, 1);

    gamer->msgid = msgget(key, 0);                                        // Unirse a la cola de mensajes existente
    if (gamer->msgid == -1)
    {
        ft_printf("Error: msgget failed\n");
        return (-1);
    }

    place_player_randomly(gamer);
    return (0);
}
