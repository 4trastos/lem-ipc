#include "../incl/lemipc.h"
#include "../lib/printf/ft_printf.h"

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