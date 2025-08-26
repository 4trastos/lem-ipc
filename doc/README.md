# 📖 Conceptos de IPC: Una Guía Detallada para lem-ipc

El proyecto **lem-ipc** se basa en la comunicación entre procesos mediante los mecanismos de **IPC de System V**.
Estos permiten a procesos independientes (sin relación padre-hijo, a diferencia de los *pipes*) comunicarse y sincronizarse.

---

## 1. Identificador Único (key\_t): La Llave de Acceso

Antes de acceder a cualquier recurso de IPC (memoria compartida, semáforo, cola de mensajes), todos los procesos deben estar de acuerdo en su "nombre". Este nombre es un identificador único, un valor de tipo `key_t`, que se genera a partir de la función `ftok`.

```c
key_t ftok(const char *pathname, int proj_id);
```

* **Propósito:** `ftok` genera una clave única basada en una ruta de archivo existente (`pathname`) y un identificador de proyecto (`proj_id`).
  Todos los procesos que usen la misma combinación de `pathname` y `proj_id` obtendrán la misma clave `key_t`.

* **Consejo:**

  * La ruta de archivo debe existir y ser accesible.
  * Un buen candidato es `.` (directorio actual) o `/tmp`.
  * `proj_id` es un entero pequeño, como `'A'` o `1`.

📚 Documentación: `man 3 ftok`

---

## 2. Memoria Compartida (shm): El Tablero de Juego

La memoria compartida es el mecanismo de IPC más rápido porque una vez que se adjunta, los procesos acceden a ella como si fuera memoria normal, sin pasar por el kernel.

### Paso 1: Obtener el ID (`shmget`)

```c
int shmget(key_t key, size_t size, int shmflg);
```

* **Propósito:** devuelve un identificador de segmento de memoria compartida (`shmid`).
* **Parámetros:**

  * `key`: la clave obtenida con `ftok`.
  * `size`: el tamaño del segmento en bytes (ejemplo: `ANCHO * ALTO * sizeof(int)`).
  * `shmflg`: permisos.

    * `IPC_CREAT`: crea un segmento.
    * `IPC_CREAT | IPC_EXCL`: crea solo si no existe (clave para el primer jugador).

### Paso 2: Adjuntar la Memoria (`shmat`)

```c
void *shmat(int shmid, const void *shmaddr, int shmflg);
```

* **Propósito:** adjunta el segmento al espacio de direcciones del proceso y devuelve un puntero.
* `shmid`: el ID obtenido con `shmget`.
* `shmaddr`: normalmente `NULL`.

### Paso 3: Manipular la Memoria

Una vez con el puntero de `shmat`, se accede como a un array:

```c
int *board = (int*)shmat(...);
```

### Paso 4: Limpiar (`shmdt` y `shmctl`)

* `shmdt`:

  ```c
  int shmdt(const void *shmaddr);
  ```

  Desvincula el segmento (no lo destruye).

* `shmctl`:

  ```c
  int shmctl(int shmid, int cmd, struct shmid_ds *buf);
  ```

  Para eliminar:

  ```c
  shmctl(shmid, IPC_RMID, NULL);
  ```

📚 Documentación: `man 2 shmget`, `man 2 shmat`, `man 2 shmctl`

---

## 3. Semáforos (sem): El Sincronizador de Accesos

Los semáforos controlan el acceso a un recurso compartido. En **lem-ipc**, evitan que dos jugadores se muevan al mismo tiempo y sobrescriban el tablero.

### Paso 1: Obtener el ID (`semget`)

```c
int semget(key_t key, int nsems, int semflg);
```

* **Propósito:** devuelve un ID de conjunto de semáforos (`semid`).
* Para lem-ipc solo necesitas **1**, así que `nsems = 1`.
* `semflg`: igual que en `shmget`.

### Paso 2: Inicializar el Semáforo (`semctl`)

```c
int semctl(int semid, int semnum, int cmd, ...);
```

* **Propósito:** controla el semáforo.
* El primer jugador lo inicializa en **1** (recurso disponible).
* `cmd`: `SETVAL`.
* `semnum`: 0.
* `union semun arg`: contiene el valor inicial.

### Paso 3: Bloquear y Desbloquear (`semop`)

```c
int semop(int semid, struct sembuf *sops, size_t nsops);
```

* **Propósito:** modifica el valor del semáforo.
* `sembuf`:

  * `sem_num = 0`
  * `sem_op = -1` (bloquear) o `+1` (desbloquear)
  * `sem_flg = 0`

Ejemplo de bloqueo:

```c
struct sembuf op = {0, -1, 0};
semop(semid, &op, 1);
```

### Paso 4: Limpiar

```c
semctl(semid, 0, IPC_RMID);
```

📚 Documentación: `man 2 semget`, `man 2 semctl`, `man 2 semop`

---

## 4. Colas de Mensajes (msgq): El Canal de Comunicación

Permiten que los procesos se envíen mensajes estructurados. Útiles para coordinarse (ej: un jugador avisa a otro “persigue a este enemigo”).

### Paso 1: Obtener el ID (`msgget`)

```c
int msgget(key_t key, int msgflg);
```

### Paso 2: Enviar Mensaje (`msgsnd`)

```c
int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
```

* El mensaje debe ser una estructura con un `long` al inicio:

```c
struct s_message {
    long type;
    // datos
};
```

`type` se usa para filtrar mensajes (ej: `1` equipo 1, `2` equipo 2).

### Paso 3: Recibir Mensaje (`msgrcv`)

```c
ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
```

* `msgtyp = 0`: recibe el primero disponible.

### Paso 4: Limpiar (`msgctl`)

```c
msgctl(msqid, IPC_RMID, NULL);
```

📚 Documentación: `man 2 msgget`, `man 2 msgsnd`, `man 2 msgrcv`, `man 2 msgctl`

---

## 🔄 Diagrama de Flujo del Juego

### Jugador 1

1. `ftok` → `key_t`
2. `shmget (IPC_CREAT | IPC_EXCL)` → `shmid`
3. `semget (IPC_CREAT | IPC_EXCL)` → `semid`
4. `msgget (IPC_CREAT | IPC_EXCL)` → `msgid`
5. `semctl` inicializa el semáforo a 1.
6. `shmat` → `board_ptr`
7. Inicia bucle de juego.
8. Al finalizar: `shmctl`, `semctl`, `msgctl` con `IPC_RMID`.

### Jugadores siguientes

1. `ftok` → `key_t`
2. `shmget (IPC_CREAT)` → `shmid`
3. `semget (IPC_CREAT)` → `semid`
4. `msgget (IPC_CREAT)` → `msgid`
5. `shmat` → `board_ptr`
6. Inicia bucle de juego.
7. Al finalizar: solo `shmdt`. No deben eliminar los recursos.

---

## 📌 Nota final

Esta guía te da la base teórica y las funciones exactas de la biblioteca estándar de C que necesitarás.
Ahora, tu tarea es **leer las páginas man** de cada función para entender sus argumentos y valores de retorno, y luego traducirlo a código en tu archivo `resconf.c`.

---
