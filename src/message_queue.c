#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

void    send_message(t_gamer *gamer, int target_x, int target_y)
{
    t_messenger message;

    message.mtype = gamer->team_id;
    message.team_id = gamer->team_id;
    message.target_x = target_x;
    message.target_y = target_y;
    message.target = (gamer->board_ptr[target_y * gamer->board_dim + target_x]);

    if (msgsnd(gamer->msg_id, &message, sizeof(t_messenger), IPC_NOWAIT) == -1)
        ft_printf("❌ Error sending message to queue. The queue may be full. ❌\n");
    
    ft_printf("\nMessage sent\n");
    ft_printf("I am player: %d - Team:  %d and these are my coordinates: (%d, %d)\n", gamer->player, gamer->team_id, gamer->x, gamer->y);
    ft_printf("I pursue this enemy (Team): %d - At these coordinates: (%d, %d)\n\n", message.target, target_x, target_y);
}

int receive_message(t_gamer *gamer)
{
    t_messenger message;

    if (msgrcv(gamer->msg_id, &message, sizeof(t_messenger) - sizeof(long), gamer->team_id, IPC_NOWAIT) > 0)
    {
        ft_printf("📩 Plarer: %d - Team: %d received a message! 🎯 Target at (%d, %d) - Team: %d 📩\n",
            gamer->player, gamer->team_id, message.target_x, message.target_y, message.target);
        to_moveplayer(gamer, message.target_y, message.target_x);
        return (1);
    }
    return (0);
}