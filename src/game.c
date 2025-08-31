#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

void    play_turn(t_gamer *gamer)
{
    struct sembuf   sops;

    ft_printf("Player: %d, Team: %d. Attempted to access the dashboard...\n", gamer->player, gamer->team_id);

    // --- BLOQUEAR EL SEMÁFORO ---
    sops.sem_num = 0;       // Usamos el semáforo 0
    sops.sem_op = -1;       // Operación de bloqueo (-1)
    sops.sem_flg = 0;       // Sin flags especiales

    if (semop(gamer->semid, &sops, 1) == -1)
    {
        ft_printf("Error: semop failed (lock)\n");
        return;
    }

    ft_printf("Player: %d, team: %d. Board access granted.\n", gamer->player, gamer->team_id);

    // --- SECCIÓN CRÍTICA ---
    // Esta es la única parte del código donde se manipula el tablero.
    // Solo un proceso puede estar aquí a la vez.
    
    // Aquí iría tu lógica de juego
    // Por ejemplo:
    // 1. Leer el tablero para encontrar una posición vacía o un enemigo
    // ****** Paso 2: Lógica de la Muerte: 
    // 2.1. Detectar si está rodeado por dos o más enemigos. Si es así, debe salir del bucle y del proceso.
    // 2.2. Si no está rodeado, decidir su próximo movimiento. La IA debe ser simple: moverse hacia el enemigo más cercano.
    // 3. Mover el jugador a una nueva posición
    // 4. Escribir (Actualizar) el tablero con su nueva posición y su número de equipo.

    // Por ahora, solo simulo que el jugador está haciendo algo

    ft_printf("Player: %d, Team: %d. Manipulating the board...\n", gamer->player, gamer->team_id);
    sleep(1);       // Simulo el tiempo que toma la operación

    // --- LIBERAR EL SEMÁFORO ---
    sops.sem_num = 0;
    sops.sem_op = 1;
    sops.sem_flg = 0;

    if (semop(gamer->semid, &sops, 1) == -1)
    {
        ft_printf("Error: semop failed (unlock)\n");
        return;
    }

    ft_printf("Player: %d, Team:%d. Board released\n", gamer->player, gamer->team_id);

    // 5. Comunicarse con su equipo a través de la cola de mensajes si es necesario (por ejemplo, para coordinar un ataque).
    // 6. Pausar el proceso por un momento (usleep) para no monopolizar el procesador.
}


// Fase 2: Lógica del Juego y del Jugador

/* Esta es la parte principal del proyecto, donde implementas el comportamiento de cada jugador.

Paso 2.1: El Bucle Principal del Jugador

    Cada jugador debe tener un bucle infinito que se ejecute mientras la partida no termine.

    Dentro del bucle, el jugador debe:
        Bloquear el semáforo (semop con un valor negativo).
        Leer el estado del tablero desde la memoria compartida para detectar enemigos, aliados, y su propia posición.
        Detectar si está rodeado por dos o más enemigos. Si es así, debe salir del bucle y del proceso.
        Si no está rodeado, decidir su próximo movimiento. La IA debe ser simple: moverse hacia el enemigo más cercano.
        Actualizar el tablero con su nueva posición y su número de equipo.
        Desbloquear el semáforo (semop con un valor positivo).
        Comunicarse con su equipo a través de la cola de mensajes si es necesario (p. ej., para coordinar un ataque).
        Pausar el proceso por un momento (usleep) para no monopolizar el procesador.

Paso 2.2: Lógica de la Muerte

    Antes de cada movimiento, cada jugador debe revisar si está rodeado por al menos dos enemigos de un mismo equipo.
    Investiga las casillas adyacentes (incluyendo las diagonales) a la posición actual del jugador.
    Mantén un contador para cada equipo enemigo. Si un contador llega a 2 o más, el jugador muere.
    La lógica de la muerte es simple: el proceso debe terminar.

Paso 2.3: Lógica de la Victoria

    Al final de cada turno (después de moverse y de comprobar la muerte), 
    un jugador debe verificar si es el único equipo restante en el tablero.
    Para ello, recorre el tablero y cuenta cuántos equipos diferentes hay.
    Si solo hay uno (su propio equipo), el juego termina. */