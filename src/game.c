#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

int check_game_status(t_gamer *gamer)
{
    t_messenger     exit;
    int             current_teams;
    int             players;
    int             initial_teams;

    if (msgrcv(gamer->msg_id, &exit, sizeof(int), 99, IPC_NOWAIT) != -1)
    {
        ft_printf("Player: %d - Team: %d. 🧹 Received cleanup message. Exiting...\n", gamer->player, gamer->team_id);
        cleanup_ipc(gamer);
        gamer->alive = false;
        return (-1);
    }

    initial_teams = *(int*)(gamer->board_ptr + 3 * sizeof(int));
    current_teams = *(int *)(gamer->board_ptr + 2 * sizeof(int));
    players = *(int*)(gamer->board_ptr + sizeof(int));

    if (current_teams == -1)
        return (-1);
    if (players == 0)
        return(GAME_OVER);
    if (initial_teams > 1 && current_teams == 1)
        return(VICTORY);
    return(ON_GOING);
}

void    play_turn(t_gamer *gamer)
{
    struct sembuf   sops;
    int             game_status;
    bool            cleanup_mem = false;

    if (gamer->alive == false)
        return;
    
    ft_printf("Player: %d - Team: %d. Attempted to access the dashboard...\n", gamer->player, gamer->team_id);
    
    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    if (semop(gamer->sem_id, &sops, 1) == -1)
    {
        if (errno == EIDRM || errno == EINVAL)
        {
            ft_printf("Player: %d - Team: %d. Game ended. The semaphore has been removed. 🏆 YOU WIN!!! 🏆\n", gamer->player, gamer->team_id);
            gamer->alive = false;
            cleanup_ipc(gamer);
            sops.sem_op = 1;
            semop(gamer->sem_id, &sops, 1);
            return;
        }
        ft_printf("❌ Error: semop failed (lock) with code: ( %d ) ❌\n", errno);
        sops.sem_op = 1;
        semop(gamer->sem_id, &sops, 1);
        return;
    }
    
    ft_printf("Player: %d - Team: %d. Board access granted.\n", gamer->player, gamer->team_id);
    game_status = check_game_status(gamer);

    if (game_status == -1)
    {
        sops.sem_op = 1;
        semop(gamer->sem_id, &sops, 1);
        return;
    }

    if (game_status == GAME_OVER || game_status == VICTORY)
    {
        if (game_status == VICTORY)
        {
            ft_printf("Player: %d - Team: %d. 🏆 YOU WIN!!! 🏆\n", gamer->player, gamer->team_id);
            gamer->victory = true;
        }
        else
            ft_printf("⚠️ GAME OVER. No players left on the board. ⚠️\n");
        gamer->alive = false;
        cleanup_ipc(gamer);
        cleanup_mem = true;
    }
    else
    {
        if (is_surrounded(gamer))
            player_death(gamer);
        else
            ft_move(gamer);
    }

    sops.sem_num = 0;
    sops.sem_op = 1;
    sops.sem_flg = 0;
    if (semop(gamer->sem_id, &sops, 1) == -1)
    {
        ft_printf("❌ Error: semop failed (unlock) ❌\n");
        return;
    }
    
    if (cleanup_mem)
    {
        ft_printf("📢 Last man standing or last player left. 🧹 Cleaning up IPC resources... 🧹\n");
        if (shmctl(gamer->shm_id, IPC_RMID, NULL) == -1 && errno != EINVAL && errno != EIDRM)
        ft_printf("❌ Error: Failed to remove shared memory (errno %d)❌\n", errno);
        if (semctl(gamer->sem_id, 0, IPC_RMID) == -1 && errno != EINVAL && errno != EIDRM)
        ft_printf("❌ Error: Failed to remove semaphore (errno %d)❌\n", errno);
        if (msgctl(gamer->msg_id, IPC_RMID, NULL) == -1 && errno != EINVAL && errno != EIDRM)
        ft_printf("❌ Error: Failde to remove message (errno %d)❌\n", errno);
        
        gamer->alive = false;
    }
    ft_printf("✅ Player: %d - Team: %d. Board released\n", gamer->player, gamer->team_id);
    
    usleep(100000);
}
