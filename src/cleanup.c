#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

void    player_death(t_gamer *gamer)
{
    int *game_board = (int *)(gamer->board_ptr + 4 * sizeof(int));
    int *total_players = (int *)(gamer->board_ptr + sizeof(int));

    game_board[gamer->y * gamer->board_dim + gamer->x] = 0;
    (*total_players)--;
    maybe_decrement_team(gamer, gamer->team_id);
    gamer->alive = false;
    ft_printf("☠️ Player: %d - Team: %d You are surrounded and eliminated. Players left: %d ☠️\n", 
        gamer->player, gamer->team_id, *total_players);
}

void    cleanup_ipc(t_gamer *gamer)
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

    struct shmid_ds shm_info;
    if (shmctl(gamer->shm_id, IPC_STAT, &shm_info) != -1)
    {
        if (shm_info.shm_nattch == 0)
        {
            ft_printf("📢 Last process detached (nattch==0). 🧹 Cleaning up IPC resources... 🧹\n");
            
            if (shmctl(gamer->shm_id, IPC_RMID, NULL) == -1 && errno != EINVAL && errno != EIDRM)
                ft_printf("❌ Failed to remove shared memory (errno %d) ❌\n", errno);
            if (semctl(gamer->sem_id, 0, IPC_RMID) == -1 && errno != EINVAL && errno != EIDRM)
                ft_printf("❌ Failed to remove semaphore (errno %d)❌\n", errno);
            if (msgctl(gamer->msg_id, IPC_RMID, NULL) == -1 && errno != EINVAL && errno != EIDRM)
                ft_printf("❌ Failed to remove message queue (errno %d)❌\n", errno);
            
            ft_printf("✅ IPC resources fully removed\n");
        }
    }
    else
    {
        ft_printf("❌ shmctl(IPC_STAT) failed (errno %d)\n", errno);
        return;
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
