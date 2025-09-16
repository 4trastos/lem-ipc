#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"
#include "../incl/visualizer.h"
#include <signal.h>
#include <unistd.h>

const char *team_colors[] = {
    BACKGROUND_BLACK TEXT_WHITE,
    BACKGROUND_RED TEXT_WHITE,
    BACKGROUND_GREEN TEXT_WHITE,
    BACKGROUND_YELLOW TEXT_BLACK,
    BACKGROUND_BLUE TEXT_WHITE,
    BACKGROUND_MAGENTA TEXT_WHITE,
    BACKGROUND_CYAN TEXT_BLACK,
    BACKGROUND_WHITE TEXT_BLACK,
};

const char *team_symbols[] = {
    "♚", "♛", "♜", "♝", "♞", "♟",
    "☀", "☁", "★", "☆", "☂", "☃",
    "♠", "♥", "♦", "♣", "☺", "☻"
};

static void handle_sigint(int sig)
{
    (void)sig;
    ft_printf("%s\n👋 Visualizer terminated by user\n%s", RESET_COLOR, RESET_COLOR);
    exit(0);
}

const char *get_team_color(int team_id)
{
    if (team_id > 0)
        return (team_colors[team_id % (sizeof(team_colors) / sizeof(team_colors[0]))]);
    return (RESET_COLOR);
}

const char *get_team_symbol(int team_id)
{
    if (team_id > 0)
        return (team_symbols[team_id % (sizeof(team_symbols) / sizeof(team_symbols[0]))]);
    return " ";
}

void clear_screen(void)
{
    // Solo reposiciona el cursor arriba a la izquierda (sin borrar toda la terminal)
    ft_printf("\033[H");
}

void draw_static_header(int board_dim, int players, int teams)
{
    ft_printf("------------------------------------------\n");
    ft_printf("           LEMI-IPC VISUALIZER           \n");
    ft_printf("------------------------------------------\n");
    ft_printf("   Players: %d | Teams: %d | Board: %dx%d\n", players, teams, board_dim, board_dim);
    ft_printf("------------------------------------------\n\n");

    ft_printf("------------- LEGEND ---------------\n");
    for (int i = 1; i <= teams && i <= 8; i++) {
        ft_printf(" %s %s %sTeam %d%s ", get_team_color(i), get_team_symbol(i),
                   RESET_COLOR, i, (i % 4 == 0) ? "\n" : "");
    }
    ft_printf("\n-------------------------------------\n\n");
}

void draw_board(int *game_board, int board_dim, int players, int teams)
{
    int team_id;
    (void)teams;
    (void)players;
    // Borde superior
    ft_printf("   ");
    for (int x = 0; x < board_dim; x++) {
        ft_printf("---");
    }
    ft_printf("\n");

    for (int y = 0; y < board_dim; y++)
    {
        ft_printf("   |");
        for (int x = 0; x < board_dim; x++)
        {
            team_id = game_board[y * board_dim + x];
            if (team_id == 0)
                ft_printf(" · ");
            else
            {
                ft_printf("%s %s %s", get_team_color(team_id),
                          get_team_symbol(team_id), RESET_COLOR);
            }
        }
        ft_printf("|\n");
    }

    // Borde inferior
    ft_printf("   ");
    for (int x = 0; x < board_dim; x++) {
        ft_printf("---");
    }
    ft_printf("\n");
    ft_printf("Press CTRL+C to exit...\n");
}

int main(int argc, char **argv)
{
    key_t   key;
    int     shmid;
    int     board_dim;
    int     players;
    int     teams;
    int     starting_teams;
    int     board_size;
    int     *game_board;
    void    *shm_prt;

    signal(SIGINT, handle_sigint);

    if (argc != 2 || ft_gameratoi(argv[1]) != 42)
    {
        ft_printf("❌ Error: Use => ./visualizer <ID project: 42> ❌\n");
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

    board_dim = *(int *)shm_prt;
    board_size = board_dim * board_dim;
    players = *(int *)(shm_prt + sizeof(int));
    teams = *(int *)(shm_prt + 2 * sizeof(int));
    starting_teams = *(int *)(shm_prt + 3 * sizeof(int));
    game_board = (int *)(shm_prt + 4 * sizeof(int));

    // Dibujamos encabezado solo una vez
    draw_static_header(board_dim, players, teams);

    while (teams > 1)
    {
        players = *(int *)(shm_prt + sizeof(int));
        teams = *(int *)(shm_prt + 2 * sizeof(int));

        clear_screen(); // Solo reposiciona
        draw_board(game_board, board_dim, players, teams);

        usleep(300000); // más lento para apreciar el movimiento
    }

    clear_screen();
    draw_board(game_board, board_dim, players, teams);

    int winning_team = 0;
    for (int i = 0; i < board_size; i++)
    {
        if (game_board[i] != 0) {
            winning_team = game_board[i];
            break;
        }
    }

    if (winning_team > 0)
        ft_printf("\n🎉 ¡JUEGO TERMINADO! EL EQUIPO %d ES EL GANADOR. 🎉\n", winning_team);
    else if (starting_teams == 0)
        ft_printf("\n💀 JUEGO INICIADO SIN JUGADORES. 💀\n");
    else
        ft_printf("\n💀 JUEGO TERMINADO. NO HAY GANADOR. 💀\n");

    shmdt(shm_prt);
    return (0);
}
