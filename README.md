# lem-ipc
The goal of the project is to make two processes communicate and interact. The FAQ included in the faq.txt file is part of the subject.

---

## 📌 **Resumen de Lem-IPC**

### 🎯 Objetivo

Implementar el comando `lemipc` (no existe en Unix real).
El propósito es **hacer que varios procesos se comuniquen e interactúen** usando **IPC (Inter-Process Communication)** en C.

---

### 🕹️ **Reglas del juego**

* Hay un tablero **2D** (guardado en memoria compartida).
* Los jugadores pertenecen a **equipos**.
* Gana el equipo que quede como **último en el tablero**.
* Cada casilla solo puede tener **1 jugador**.
* Un jugador muere si está rodeado por **al menos 2 jugadores del mismo equipo contrario**, en casillas adyacentes (incluyendo diagonales).
* Cuando un jugador detecta que está “rodeado”, debe salir del tablero y terminar su proceso.

---

### ⚙️ **Restricciones técnicas**

* **Un solo ejecutable** (`lemipc`).
* Cada jugador es un **proceso**.

  * El **primer jugador** crea los recursos IPC (SHM, MSGQ, SEM).
  * El **último jugador en salir** debe liberar esos recursos (`ipcs`, `ipcrm`).
* **Mapa**: almacenado en **shared memory**.
* **Sincronización**: mediante **semaforos**.
* **Comunicación entre jugadores**: mediante **message queue**.
* Cada jugador puede ver:

  * si una casilla está vacía,
  * o si está ocupada por un jugador (solo ve el número de equipo, no la identidad individual).
* Se debe **mostrar el estado del tablero**:

  * En **texto** (ej. el primer jugador imprime en terminal cada turno),
  * o en **modo gráfico** (otro ejecutable o librería gráfica).

---

### 📚 **Reglas de código**

* Escribirlo en **C**.
* Gestionar errores (sin segfaults).
* Sin memory leaks.
* Usar solo libc estándar (salvo funciones cubiertas por tu `libft`, que debes usar desde ahí).
* Makefile obligatorio.

---

### 🎨 **Bonus**

* Interfaz gráfica bonita (ej: ncurses, SDL, etc.).
* Otras mejoras útiles y 100% funcionales.

*(Recuerda: los bonus solo cuentan si la parte obligatoria está **perfecta**).*

---

👉 En resumen: es un **juego multijugador en memoria compartida**, donde cada jugador es un proceso independiente que debe coordinarse usando **IPC clásicos de UNIX (shm, sem, msgq)**, y donde el reto real es **gestionar concurrencia, comunicación y sincronización** sin que nada se rompa.

---
