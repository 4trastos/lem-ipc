#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

int    ft_resconf(t_game *game, key_t  key, int board)
{
    t_game  *aux;

    aux = game;
    aux->shmid = shmget(key, board, IPC_CREAT | IPC_EXCL | 0666);
    if (aux->shmid == -1)
        return (2);
    else
        return (1);
}

int    player_one(t_game *game, key_t  key)
{
    t_game  *aux;

    aux = game;
    aux->player = 1;
    aux->board_ptr = (int *)shmat(aux->shmid, NULL, 0);
    if (aux->board_ptr == (int *)-1)
    {
        ft_printf("Error: shmat failed\n");
        return (-1);
    }
    aux->semid = semget(key, 1, IPC_CREAT | 0666);
    if (aux->semid == -1)
    {
        ft_printf("Error: semget failed\n");
        return (-1);
    }
    return (0);
}

int    other_player(t_game *game, key_t key)
{
    t_game  *aux;

    aux = game;
    aux->player = 2;
    aux->shmid = shmget(key, 0, 0);         // Nos unimos a la memoria compartida existente
    aux->board_ptr = (int *)shmat(aux->shmid, NULL, 0);
    if (aux->board_ptr == (int *)-1)
    {
        ft_printf("Error: shmat failed\n");
        return (-1);
    }
    return (0);
}

//1. Configuración de los Recursos de IPC

// Paso 1.1: Identificador Único (key_t)
// Paso 1.2: Memoria Compartida (shm)
// Paso 1.3: Semaforos (sem)
// Paso 1.4: Colas de Mensajes (msgq)
// Paso 1.5: La Lógica del Primer Jugador

/* Esta fase se centra en la creación y el control de los recursos de IPC. 
Es crucial porque si esta parte falla, el resto del proyecto no funcionará.

Paso 1.1: Identificador Único (key_t)

    Investiga la función ftok. Necesitas una forma de generar un identificador 
    único para tus recursos IPC a partir de una ruta de archivo y un caracter. 
    Esto asegura que todos los procesos (players) se conecten a los mismos recursos.

Paso 1.2: Memoria Compartida (shm)

    Utiliza shmget para crear o conectarte a un segmento de memoria compartida. 
    El primer jugador debe crearlo con los permisos adecuados. Los siguientes jugadores se conectarán al segmento ya existente.

    Utiliza shmat para adjuntar el segmento de memoria compartida a tu espacio de direcciones. 
    El puntero que devuelve será la dirección del tablero.

    El tamaño de la memoria compartida debe ser lo suficientemente grande para almacenar el tablero (ancho x alto x sizeof(int)).

Paso 1.3: Semaforos (sem)

    Utiliza semget para crear un semáforo. Solo necesitas uno para controlar el acceso al tablero.

    El primer jugador debe inicializar el semáforo a 1. Investiga semctl para inicializar el valor.

Paso 1.4: Colas de Mensajes (msgq)

    Utiliza msgget para crear o conectarte a una cola de mensajes.

    Esta será la vía de comunicación de los jugadores.

Paso 1.5: La Lógica del Primer Jugador

    Necesitas un mecanismo para determinar si eres el primer jugador. 
    Un buen truco es intentar crear los IPC con IPC_CREAT | IPC_EXCL. 
    Si semget falla con EEXIST, significa que otro jugador ya los ha creado. */