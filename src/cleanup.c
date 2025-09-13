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

void cleanup_orphaned_ipc(key_t key)
{
    int  shm_id;
    
    shm_id = shmget(key, 0, 0666);
    if (shm_id != -1)
    {
        struct shmid_ds shm_info;
        if (shmctl(shm_id, IPC_STAT, &shm_info) == 0 && shm_info.shm_nattch == 0)
        {
            ft_printf("🧹 Cleaning orphaned shared memory...\n");
            shmctl(shm_id, IPC_RMID, NULL);
        }
        else
            ft_printf("⚠️  Shared memory ID %d has %d processes - keeping\n", shm_id, shm_info.shm_nattch);
    }
}
