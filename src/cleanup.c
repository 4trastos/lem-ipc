#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

void    clearmemsem(t_gamer *gamer)
{
    if (!gamer)
        return;
    
    ft_printf("🧹 Cleaning up IPC resources for PID %d...\n", getpid());

    if (gamer->board_ptr)
    {
        if (shmdt(gamer->board_ptr) == -1)
            ft_printf("❌ Error: Failed to detach shared memory ❌\n");
        gamer->board_ptr = NULL;
        ft_printf("✅ Shared memory detached\n");
    }

    ft_printf("✅ Process cleanup completed\n");
}

void    cleanup_orphaned_ipc(key_t key)
{
    int shm_id;
    int sem_id;
    int msg_id;

    shm_id = shmget(key, 0, 0666);
    if (shm_id != -1)
    {
        struct shmid_ds shm_info;
        if (shmctl(shm_id, IPC_STAT, &shm_info) == 0)
        {
            if (shm_info.shm_nattch == 0)
            {
                ft_printf("🧹 Cleaning orphaned shared memory...\n");
                shmctl(shm_id, IPC_RMID, NULL);
            }
        }
    }

    sem_id = semget(key, 0, 0);
    if (sem_id != -1)
    {
        ft_printf("🧹 Cleaning orphaned semaphore with ID %d...\n", sem_id);
        if (semctl(sem_id, 0, IPC_RMID) == -1)
            ft_printf("Error: Failed to remove semaphore with ID %d\n", sem_id);
    }

    msg_id = msgget(key, 0);
    if (msg_id != -1)
    {
        ft_printf("🧹 Cleaning orphaned message queue with ID %d...\n", msg_id);
        if (msgctl(msg_id, IPC_RMID, NULL) == -1)
            ft_printf("Error: Failed to remove message queue with ID %d\n", msg_id);
    }
}
