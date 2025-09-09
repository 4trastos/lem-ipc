#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

int get_total_players(t_gamer *gamer)
{
    int player_count;

    player_count = *(int *)(gamer->board_ptr + sizeof(int));
    return (player_count);
}

void    place_player_randomly(t_gamer *player)
{
    struct sembuf   spos;
    int             x;
    int             y;
    bool            found_spot;

    found_spot = false;

    spos.sem_num = 0;
    spos.sem_op = -1;
    spos.sem_flg = 0;
    semop(player->sem_id, &spos, 1);

    srand(time(NULL) + player->pid);
    while (!found_spot)
    {
        x = rand() % player->board_dim;
        y = rand() % player->board_dim;
        if (player->board_ptr[y * player->board_dim + x] == 0)
        {
            player->x = x;
            player->y = y;
            player->board_ptr[y * player->board_dim + x] = player->team_id;
            found_spot = true;
        }
    }
    
    ft_printf("Player: %d - Team: %d - placed at (%d, %d)\n", player->player, player->team_id, player->x, player->y);
    spos.sem_op = 1;
    semop(player->sem_id, &spos, 1);
}

void    play_turn(t_gamer *gamer)
{
    struct sembuf   sops;
    
    ft_printf("Player: %d - Team: %d. Attempted to access the dashboard...\n", gamer->player, gamer->team_id);
    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    if (semop(gamer->sem_id, &sops, 1) == -1)
    {
        ft_printf("❌ Error: semop failed (lock) ❌\n");
        return;
    }
    ft_printf("Player: %d - Team: %d. Board access granted.\n", gamer->player, gamer->team_id);
    
    if (is_surrounded(gamer))
    {
        *(int *)(gamer->board_ptr + sizeof(int)) -= 1;
        sops.sem_op = 1;
        semop(gamer->sem_id, &sops, 1);
        return;
    }

    if (check_for_victory(gamer))
    {
        ft_printf("Player: %d - Team: %d. 🏆 YOU WIN!!! 🏆\n", gamer->player, gamer->team_id);
        gamer->alive = false;
        gamer->victory = true;
        sops.sem_op = 1;
        semop(gamer->sem_id, &sops, 1);
        return;
    }

    if (get_total_players(gamer) < 2)
    {
        ft_printf("⚠️ Waiting for another player. Only %d player on the board. ⚠️\n", gamer->player);
        usleep(500000);
        sops.sem_op = 1;
        semop(gamer->sem_id, &sops, 1);
        return;
    }

    ft_move(gamer);
    if (gamer->alive == false)
    {
        ft_printf("Player: %d - Team: %d.You're dead!!\n", gamer->player, gamer->team_id);
        *(int *)(gamer->board_ptr + sizeof(int)) -= 1;
        sops.sem_op = 1;
        semop(gamer->sem_id, &sops, 1);
        return;
    }

    ft_printf("✅ Player: %d - Team:%d. Board released\n", gamer->player, gamer->team_id);

    sops.sem_num = 0;
    sops.sem_op = 1;
    sops.sem_flg = 0;
    if (semop(gamer->sem_id, &sops, 1) == -1)
    {
        ft_printf("❌ Error: semop failed (unlock) ❌\n");
        return;
    }

    usleep(100000);
}
