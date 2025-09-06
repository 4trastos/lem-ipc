#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

void    clearmemsem(t_gamer *gamer)
{
    int total_player;

    total_player = *(int *)(gamer->board_ptr + sizeof(int));
    if (total_player == 1 && gamer->victory == true)
    {
        if (shmctl(gamer->shmid, IPC_RMID, 0) == -1)
            ft_printf("Error: Failed to remove shared memory\n");
        if (semctl(gamer->semid, IPC_RMID, 0) == -1)
            ft_printf("Error: Failed to remove semaphore\n");
        if (msgctl(gamer->msgid, IPC_RMID, 0) == -1)
            ft_printf("Error: Failde to remove message queue\n");
    }
}

// Fase 3: Limpieza de Recursos de IPC

/* Esta es la fase final y es tan importante como la inicial para evitar la "contaminación" del sistema.

Paso 3.1: Detección del Último Jugador

    Al salir de la partida (ya sea por muerte o victoria), un jugador debe verificar si es el último en el tablero.
    Esto se puede hacer contando el número de procesos en el tablero de memoria compartida. 
    Si la cuenta llega a cero, el jugador es el último.

Paso 3.2: Eliminación de los Recursos

    Solo el último jugador debe destruir los recursos IPC.
    Utiliza shmctl con IPC_RMID para eliminar la memoria compartida.
    Utiliza semctl con IPC_RMID para eliminar el semáforo.
    Utiliza msgctl con IPC_RMID para eliminar la cola de mensajes.
    ¡No olvides desvincular la memoria compartida con shmdt!

Consideraciones Adicionales

    - MakeFile: Necesitas un Makefile que compile tu proyecto.
    - Gestión de Errores: Todos los IPC pueden fallar. 
    Asegúrate de manejar los errores adecuadamente y salir del programa de forma segura (exit(1)).
    - Variables Globales: Utiliza variables globales para los identificadores de los recursos IPC 
    (shmid, semid, msgid) y las direcciones de la memoria compartida.
    Modo de Visualización: Implementa la visualización del tablero en modo texto. 
    Puedes imprimir el tablero con los números de los equipos. */