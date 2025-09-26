#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

t_gamer *global_gamer = NULL;

void    cleanup_handler(int sig)
{
    t_messenger cleanup_msg;
    int         players = 0;

    if (!global_gamer || !global_gamer->alive)
        exit (0);

    ft_printf("🛑 Received signal %d, cleaning up...\n", sig);
    players = *(int *)(global_gamer->board_ptr + sizeof(int));

    if (players < 1)
        players = 1;

    cleanup_msg.mtype    = 99;
    cleanup_msg.data     = 1;
    cleanup_msg.team_id  = 0;
    cleanup_msg.target   = 0;
    cleanup_msg.target_x = 0;
    cleanup_msg.target_y = 0;

    for (int i = 0; i < players; i++)
    {
        if (msgsnd(global_gamer->msg_id, &cleanup_msg, sizeof(t_messenger) - sizeof(long), IPC_NOWAIT) == -1)
            msgsnd(global_gamer->msg_id, &cleanup_msg, sizeof(t_messenger) - sizeof(long), 0);
    }
    
    global_gamer->alive = false;

    if (semctl(global_gamer->sem_id, 0, IPC_RMID) == -1)
        ft_printf("❌ Warning: semctl(IPC_RMID) failed (errno %d)\n", errno);
    else
        ft_printf("⚠️ Semaphore removed\n");

    if (msgctl(global_gamer->msg_id, 0, IPC_RMID) == -1)
        ft_printf("❌ Warning: msgctl(IPC_RMID) failed (errno %d)\n", errno);
    else
        ft_printf("⚠️ Messages removed\n");
    if (shmctl(global_gamer->shm_id, IPC_RMID, NULL))
         ft_printf("❌ Failed to remove shared memory (errno %d) ❌\n", errno);
    else
        ft_printf("⚠️  Shared memory removed \n");
        

    cleanup_ipc(global_gamer);
    free(global_gamer);
    global_gamer = NULL;
    exit (0);
}

int main(int argc, char **argv)
{
    t_gamer  *gamer;
    pid_t   pid;
    key_t   key;                    //  identificador único del segmento de memoria
    int     board;
    int     player;

    player = 0;
    board = 0;
    if (argc < 3 || argc > 4 || (ft_gameratoi(argv[2]) != 42))
    {
        ft_printf("❌ Use: %s <team> <ID proyect: 42> <Board (opcional)> ❌\n", argv[0]);
        return (1);
    }

    if (argument_parsing(argv, &board) == -1)
        return (1);

    key = ftok(".", ft_gameratoi(argv[2]));
    if (key == -1)
    {
        ft_printf("❌ Error: ftok failed ❌\n");
        return (1);
    }
    cleanup_orphaned_ipc(key);

    gamer = malloc(sizeof(t_gamer));
    if (!gamer)
        return (1);
    ft_memset(gamer, 0, sizeof(t_gamer));

    global_gamer = gamer;
    signal(SIGINT, cleanup_handler);
    signal(SIGTERM, cleanup_handler);
    signal(SIGSEGV, cleanup_handler);

    pid = getpid();
    gamer->pid = pid;
    gamer->team_id = ft_gameratoi(argv[1]);
    gamer->alive = true;
    gamer->victory = false;

    player = ft_resconf(gamer, key, board);
    if (player == 1)
    {
        gamer->board_size = board;
        gamer->board_dim = (int)sqrt(gamer->board_size);
        if (player_one(gamer, key) == -1)
        {
            free(gamer);
            return (1);
        }
    }
    else
    {
        if (other_player(gamer, key) == -1)
        {
            free(gamer);
            return (1);
        }
    }

    ft_printf("PID: %d - Team: %d - Player: %d - Key: %d - Board: %d - RAM: %p\n", gamer->pid, gamer->team_id, gamer->player,key ,board, gamer->board_ptr);
    
    while (gamer->alive == true && gamer->victory == false)
        play_turn(gamer);
    
    cleanup_ipc(gamer);
    free (gamer);
    global_gamer = NULL;
    return (0); 
}