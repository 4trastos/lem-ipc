#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

t_gamer *global_gamer = NULL;

void    cleanup_handler(int sig)
{
    if (global_gamer)
    {
        ft_printf("🛑 Received signal %d, cleaning up...\n", sig);
        clearmemsem(global_gamer);
        free(global_gamer);
        global_gamer = NULL;
    }
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
        ft_printf("❌ Error: Use => ./lemipc <team> <ID proyect: 42> <Board (opcional)> ❌\n");
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
    
    clearmemsem(gamer);
    free (gamer);
    global_gamer = NULL;
    return (0); 
}