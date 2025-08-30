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

¡Vamos a ver “manual de `pid_t`” paso a paso y con todo detalle! 😄

# ¿Qué es `pid_t`?

* **Tipo POSIX** (no existe en C estándar) para **identificadores de proceso**.
* **Signed integer** (con signo). POSIX exige que su **anchura nunca sea mayor que `long`** en el entorno soportado.
* Se usa para:

  * **PID** (process ID) → identificador de un proceso.
  * **PGID** (process group ID) → identificador de grupo de procesos.
  * **SID** (session ID) → identificador de sesión.
  * En Linux, también se usa para **TID** (thread ID) de hilos tipo NPTL (p. ej., `gettid()`).

## Cabeceras

* Definición del tipo: `#include <sys/types.h>`
* Según qué llamadas uses, también lo traen: `<unistd.h>`, `<sys/wait.h>`, `<signal.h>`, `<sched.h>`, `<termios.h>`, etc.

  * Ej.: `getpid()`/`getppid()` en `<unistd.h>`, `waitpid()` en `<sys/wait.h>`, `kill()` en `<signal.h>`.

---

# Propósito y por qué es “signed”

* Muchas APIs POSIX aceptan **valores especiales negativos** con semántica distinta:

  * `kill(pid, sig)`:

    * `pid > 0` → señal al proceso `pid`.
    * `pid = 0` → a **tu grupo de procesos** actual.
    * `pid = -1` → a **todos los procesos** a los que tengas permiso (excepto PID 1).
    * `pid < -1` → a **todos los procesos con PGID = -pid**.
  * `waitpid(pid, …)`:

    * `pid > 0` → esperar a ese hijo concreto.
    * `pid = -1` → esperar a **cualquier** hijo.
    * `pid = 0` → a **cualquier hijo en tu grupo de procesos**.
    * `pid < -1` → a **cualquier hijo en el grupo** `-pid`.
* Por eso POSIX **exige** que `pid_t` sea con signo.

---

# Rango y tamaño (portabilidad)

* POSIX solo garantiza: **signed** y **ancho ≤ `long`**.
* En Linux moderno `pid_t` suele ser **32 bits con signo**, incluso en x86\_64.
* El **máximo PID real** del sistema no depende del tamaño del tipo, sino de `pid_max`:

  * Linux: `/proc/sys/kernel/pid_max` (típicamente entre 32 768 y 4 194 304).
* **No asumas** que cabe en `int` (aunque en la práctica casi siempre sí). Para imprimir, lo más portable:

  ```c
  pid_t pid = getpid();
  printf("pid=%ld\n", (long)pid);  // porque width(pid_t) ≤ width(long)
  ```

---

# Funciones típicas que usan/devuelven `pid_t`

* **Procesos:**

  * `pid_t fork(void);`

    * Padre: devuelve **PID del hijo** (>0)
    * Hijo: devuelve **0**
    * Error: **-1**
  * `pid_t getpid(void);` → tu PID
  * `pid_t getppid(void);` → PID de tu padre
  * `pid_t waitpid(pid_t pid, int *status, int options);`
* **Grupos y sesiones:**

  * `pid_t getpgid(pid_t pid);` / `pid_t getpgrp(void);`
  * `int setpgid(pid_t pid, pid_t pgid);`
  * `pid_t getsid(pid_t pid);`
  * `pid_t setsid(void);` (devuelve el **SID** del nuevo líder de sesión)
* **Señales:**

  * `int kill(pid_t pid, int sig);` (con los valores especiales descritos)
  * `int sigqueue(pid_t pid, int sig, const union sigval value);`
* **Linux específico:**

  * `pid_t gettid(void);` (TID del hilo actual; en glibc moderno existe wrapper; si no, syscall)
  * `int pidfd_open(pid_t pid, unsigned int flags);` → **devuelve un FD**, no un `pid_t`, pero se le pasa un `pid_t`. Sirve para evitar *race conditions* por **reutilización** de PIDs.

---

# Semántica importante (cosas que te pueden examinar)

## 1) **Reutilización de PIDs**

* Los PIDs se **reutilizan**. Un `pid_t` no identifica un proceso “para siempre”.
* Si guardas un PID y mucho después intentas actuar sobre él, **puede ser otro proceso**.
* Mitigación:

  * Señal “nula”: `kill(pid, 0)` comprueba existencia/permisos (pero sigue habiendo carrera).
  * **Solución moderna:** `pidfd_open()` + operar sobre el **pidfd** (FD estable).

## 2) **PIDs por *namespace***

* En contenedores, los PIDs son **por espacio de nombres**. El PID que ves dentro del contenedor puede **no coincidir** con el del host.

## 3) **Hilos en Linux**

* `getpid()` devuelve el **PID del grupo de hilos** (el del hilo líder).
* `gettid()` devuelve el **TID** (identificador único del hilo en el kernel). Ambos suelen tiparse como `pid_t`.

## 4) **Grupo de procesos y sesiones**

* El **líder de grupo** tiene `PGID == PID`.
* El **líder de sesión** tiene `SID == PID` (tras `setsid()`).
* Útil en **job control** (terminales) y **señales a grupos**.

---

# Ejemplos prácticos

## A) Crear un hijo y esperarle

```c
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    pid_t pid = fork();
    if (pid == -1) { perror("fork"); return 1; }

    if (pid == 0) { // Hijo
        printf("Hijo: pid=%ld, ppid=%ld\n", (long)getpid(), (long)getppid());
        _exit(0);
    } else { // Padre
        int status;
        pid_t w = waitpid(pid, &status, 0);
        if (w == -1) { perror("waitpid"); return 1; }
        if (WIFEXITED(status))
            printf("Padre: hijo %ld salió con %d\n", (long)w, WEXITSTATUS(status));
    }
}
```

## B) Enviar señal a todo tu **grupo** de procesos

```c
#include <signal.h>
#include <unistd.h>

int main(void) {
    // pid=0 => grupo actual
    if (kill(0, SIGTERM) == -1) {
        perror("kill group");
        return 1;
    }
    return 0;
}
```

## C) Cambiar grupo de procesos

```c
#include <unistd.h>
#include <stdio.h>

int main(void) {
    pid_t pid = getpid();
    if (setpgid(0, 0) == -1) { // 0,0 => me convierto en líder de mi nuevo grupo
        perror("setpgid");
        return 1;
    }
    printf("PID=%ld, PGID=%ld\n", (long)pid, (long)getpgid(0));
}
```

## D) Comprobar PID máximo del sistema (Linux)

```c
#include <unistd.h>
#include <stdio.h>

int main(void) {
    long page = sysconf(_SC_PAGESIZE);
    printf("Page size: %ld\n", page);

    // Leer pid_max en runtime (vía /proc)
    FILE *f = fopen("/proc/sys/kernel/pid_max", "r");
    if (f) {
        long pid_max;
        if (fscanf(f, "%ld", &pid_max) == 1)
            printf("kernel pid_max: %ld\n", pid_max);
        fclose(f);
    }
    return 0;
}
```

---

# Buenas prácticas con `pid_t`

* **Usa `pid_t` siempre** para PIDs/PGIDs/SIDs/TIDs (no `int`).
* **Imprime casteando a `long`** y `%ld` por portabilidad POSIX.
* **No confíes** en PIDs “antiguos”: pueden haberse reutilizado.
* Para evitar carreras al señalar/esperar procesos “por PID” en Linux moderno, **considera `pidfd_open()`**.
* Recuerda que **`fork()` puede fallar** (devuelve `-1`) y **`waitpid()` también** (devuelve `-1`); comprueba `errno`.

---

# Referencias útiles para explorar (man pages)

* `man 2 fork`, `man 2 getpid`, `man 2 getppid`, `man 2 getpgid`, `man 2 getsid`,
  `man 2 kill`, `man 2 waitpid`, `man 2 pidfd_open`, `man 2 gettid`,
  `man 7 credentials`, `man 7 signal`.
