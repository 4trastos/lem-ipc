#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

void    clearmemsem(t_gamer *gamer)
{
    ft_printf("DEBUG: Detaching shared memory...\n");
    if (shmdt(gamer->board_ptr) == -1)
        ft_printf("❌ Error: Failed to detach shared memory ❌\n");
}

void    cleanup_orphaned_ipc(key_t key)
{
    int shm_id;

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
}

/* void    clearmemsem(t_gamer *gamer)
{
    int total_player;
    int *player_count_ptr;

    ft_printf("DEBUG: Cleaning up resources...\n");

    player_count_ptr = (int *)(gamer->board_ptr + sizeof(int));
    total_player = *player_count_ptr;

    ft_printf("DEBUG: Total players = %d\n", total_player);

    if (shmdt(gamer->board_ptr) == -1)
        ft_printf("❌ Error: Failed to detach shared memory ❌\n");
    if (total_player <= 1)
    {
        ft_printf("Last player has left. Cleaning up IPC resources...\n");

        if (shmctl(gamer->shm_id, IPC_RMID, 0) == -1)
            ft_printf("Error: Failed to remove shared memory\n");
        if (semctl(gamer->sem_id, 0, IPC_RMID) == -1)
            ft_printf("Error: Failed to remove semaphore\n");
        if (msgctl(gamer->msg_id, IPC_RMID, 0) == -1)
            ft_printf("Error: Failde to remove message queue\n");
    }
    else
        ft_printf("DEBUG: This player does not release Player: %d\n", gamer->player);
} */