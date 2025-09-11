#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

void    clearmemsem(t_gamer *gamer)
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
}