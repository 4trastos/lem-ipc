#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

const char *team_colors[] = {
    BACKGROUND_BLACK,
    BACKGROUND_RED,
    BACKGROUND_GREEN,
    BACKGROUND_YELLOW,
    BACKGROUND_BLUE,
    BACKGROUND_MAGENTA,
    BACKGROUND_CYAN,
    BACKGROUND_WHITE,
};

const char *get_team_color(int team_id)
{
    if (team_id > 0)
        return (team_colors[team_id % (sizeof(team_colors) / sizeof(team_colors[0]))]);
    return (RESET_COLOR);
}

void    clear_screen(void)
{
    ft_printf("\033[H\033[J");
}

void    draw_board(int *game_board, int board_dim, int players, int teams)
{
    int team_id;

    clear_screen();
    ft_printf("----------------------------------\n");
    ft_printf("       LEMI-IPC VISUALIZER      \n");
    ft_printf("----------------------------------\n");
    ft_printf("    Players: %d | Teams: %d\n", players, teams);
    ft_printf("----------------------------------\n\n");
    ft_printf("------------ LEGEND --------------\n");
    for (int i = 1; i <= teams; i++) {
        ft_printf("    %s  %s Team %d | ", get_team_color(i), RESET_COLOR, i);
    }
    ft_printf("\n---------------------------------\n\n");

    for (int y = 0; y < board_dim; y++)
    {
        for (int x = 0; x < board_dim; x++)
        {
            team_id = game_board[y * board_dim + x];
            if (team_id == 0)
                ft_printf(" . ");
            else
                ft_printf("%s %2d %s", get_team_color(team_id), team_id, RESET_COLOR);
        }
        ft_printf("\n");
    }
    ft_printf("----------------------------------\n");
    ft_printf("Press CTRL+C to exit...\n");
}

int main(int argc, char **argv)
{
    key_t   key;
    int     shmid;
    int     board_dim;
    int     players;
    int     teams;
    int     board_size;
    int     *game_board;
    void    *shm_prt;

    if (argc != 2 || ft_gameratoi(argv[1]) != 42)
    {
        ft_printf("❌ Error: Use => ./visualizer <ID proyect: 42> ❌\n");
        return (1);
    }

    key = ftok(".", ft_gameratoi(argv[1]));
    if (key == -1)
    {
        ft_printf("❌ Error: ftok failed ❌\n");
        return (1);
    }

    shmid = shmget(key, 0, 0666);
    if (shmid == -1)
    {
        ft_printf("❌ Error: Failed to get shared memory. Is a game running? ❌\n");
        return (1);
    }

    shm_prt = shmat(shmid, NULL, 0);
    if (shm_prt == (void *)-1)
    {
        ft_printf("❌ Error: Failed to attach shared memory ❌\n");
        return (1);
    }

    board_size = *(int *)shm_prt;
    board_dim = (int)sqrt(board_size);
    players = *(int *)(shm_prt + sizeof(int));
    teams = *(int *)(shm_prt + 2 * sizeof(int));
    game_board = (int *)(shm_prt + 3 *sizeof(int));

    while (1)
    {   
        players = *(int *)(shm_prt + sizeof(int));
        teams = *(int *)(shm_prt + 2 * sizeof(int));
        draw_board(game_board, board_dim, players, teams);
        usleep(10000);
    }
    
    shmdt(shm_prt);
    return (0);
}