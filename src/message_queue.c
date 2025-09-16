#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

void    send_message(t_gamer *gamer, int target_y, int target_x)
{
    t_messenger message;
    int         *game_board = (int *)(gamer->board_ptr + 3 * sizeof(int));

    message.mtype = gamer->team_id;
    message.team_id = gamer->team_id;
    message.target_x = target_x;
    message.target_y = target_y;
    message.target = game_board[target_y * gamer->board_dim + target_x];

    if (msgsnd(gamer->msg_id, &message, sizeof(t_messenger) - sizeof(long), IPC_NOWAIT) == -1)
        ft_printf("❌ Error sending message to queue. The queue may be full. ❌\n");
    else
        ft_printf("✉️  Player: %d - Team: %d sent message about target Team: %d at (%d, %d) ✉️\n",
                gamer->player, gamer->team_id, message.target, target_x, target_y);
}

int receive_message(t_gamer *gamer)
{
    t_messenger message;
    int         *game_board = (int *)(gamer->board_ptr + 3 * sizeof(int));

    if (msgrcv(gamer->msg_id, &message, sizeof(t_messenger) - sizeof(long), gamer->team_id, IPC_NOWAIT) > 0)
    {
        ft_printf("📩 PlaYer: %d - Team: %d received a message! 🎯 Target at (%d, %d) - Team: %d 📩\n",
            gamer->player, gamer->team_id, message.target_x, message.target_y, message.target);
        
        if (game_board[message.target_y * gamer->board_dim + message.target_x] == message.target && 
            game_board[message.target_y * gamer->board_dim + message.target_x] != 0)
        {
            to_moveplayer(gamer, message.target_y, message.target_x);
            return (1);
        }
        else
            ft_printf("⚠️  Obsolete message ignored (target gone)\n");
    }
    return (0);
}