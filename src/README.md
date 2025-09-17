# Algoritmo de Descenso del Gradiente (Gradient Descent)

Voy a diseccionar esto **línea a línea**, explicar exactamente qué hace el algoritmo y luego ver, con ejemplos y razonamiento, **por qué elegir aleatoriamente entre los “mejores” pasos** puede llevar a que varios jugadores alcancen un objetivo y, en conjunto, rodeen a un enemigo.

# 1) Resumen rápido (antes de entrar en detalle)

El `to_moveplayer`:

* prueba las 4 casillas vecinas (arriba/abajo/izquierda/derecha),
* filtra las que están dentro del tablero y están libres,
* calcula la **distancia Manhattan** desde cada casilla candidata al objetivo,
* recoge todas las casillas que **minimizan** esa distancia (lista `best_moves`),
* elige **aleatoriamente** una de esas mejores casillas y se mueve allí (actualiza `gamer->x/y` y el `game_board`).

Es, por tanto, un **descenso por gradiente discreto** (reduce distancia cada paso) con **desempate aleatorio** entre las opciones igualmente buenas.

---

# 2) Explicación paso a paso (línea a línea, en detalle)

Voy a numerar el flujo lógico del código y explicar cada pieza:

1. Declaraciones e inicializaciones

```c
int step_x = 0, step_y = 0;
int new_x, new_y;
int *game_board = (int *)(gamer->board_ptr + 4 * sizeof(int));
int min_distance = INT_MAX;
bool move = false;
int moves[4][2] = {{0, -1}, {0, 1}, {1, 0}, {-1, 0}}; // dx, dy
int best_moves[4][2];
int best_count = 0;
```

* `game_board` apunta al inicio del tablero en memoria compartida.
* `moves` contiene offsets (dx,dy) para los 4 vecinos: arriba (0,-1), abajo (0,1), derecha (1,0), izquierda (-1,0).
* `min_distance` guarda la menor distancia encontrada entre las candidatas.
* `best_moves` almacenará los offsets de las casillas con esa distancia mínima. `best_count` cuenta cuántas hay.

2. Loop sobre las 4 posibles direcciones:

```c
for (int i = 0; i < 4; i++) {
    new_x = gamer->x + moves[i][0];
    new_y = gamer->y + moves[i][1];
    // comprobar límites y si está libre
    if (new_y >= 0 && new_y < gamer->board_dim &&
        new_x >= 0 && new_x < gamer->board_dim &&
        game_board[new_y * gamer->board_dim + new_x] == 0) {
```

* Para cada vecino calcula `new_x,new_y`.
* Comprueba que no se salga del tablero.
* Comprueba que la casilla esté libre (`== 0`). Si está ocupada la ignora.

3. Calcula la **distancia Manhattan** desde esa casilla candidata al objetivo:

```c
int current_distance = abs(new_x - target_x) + abs(new_y - target_y);
```

* Manhattan = |dx| + |dy|. Es apropiado para movimientos ortogonales (sin diagonales).

4. Actualiza la lista de “mejores”:

```c
if (current_distance < min_distance) {
    min_distance = current_distance;
    best_count = 0;
    best_moves[best_count][0] = moves[i][0];
    best_moves[best_count][1] = moves[i][1];
    best_count = 1;
    move = true;
} else if (current_distance == min_distance) {
    best_moves[best_count][0] = moves[i][0];
    best_moves[best_count][1] = moves[i][1];
    best_count++;
    move = true;
}
```

* Si `current_distance` es **mejor** (menor) que el `min_distance` previo, reinicia la lista y añade esta casilla.
* Si es **empate** (igual), la añade a la lista.
* `move = true` indica que hay al menos una casilla válida.

5. Si hay alguna casilla válida, elige una al azar y actualiza:

```c
int idx = rand() % best_count;
step_x = best_moves[idx][0];
step_y = best_moves[idx][1];
new_x = gamer->x + step_x;
new_y = gamer->y + step_y;
int oldx = gamer->x, oldy = gamer->y;
gamer->x = new_x; gamer->y = new_y;
game_board[oldy * gamer->board_dim + oldx] = 0;
game_board[gamer->y * gamer->board_dim + gamer->x] = gamer->team_id;
```

* `idx` selecciona aleatoriamente entre las mejores `best_count`.
* Actualiza `gamer->x/y` y el tablero: borra la antigua posición y marca la nueva con su `team_id`.

6. Si `move` es falso (no hay vecinos libres), imprime que espera.

---

# 3) Observaciones importantes sobre la implementación

* **Distancia**: se usa Manhattan porque los movimientos son ortogonales (no diagonales). Es una heurística local (solo mira al vecino inmediato).
* **Comportamiento local**: cada jugador decide **solo** con la información de su vecino y la posición del objetivo (y el tablero global), no hace planificación larga ni coordina posiciones futuras.
* **Aleatoriedad**: se usa sólo como desempate entre movimientos que reducen la distancia exactamente la misma cantidad.
* **Atomicidad / coherencia**: es crucial que sólo un proceso actualice el tablero a la vez: en tu programa esto se controla con el semáforo (lock) en `play_turn`. Si no estuviera protegido, dos jugadores podrían leer la misma casilla como libre y ambos intentar escribirla (race). Con semáforo protege la lectura-escritura del tablero por turno.

---

# 4) ¿Por qué con ese criterio + desempate aleatorio los jugadores **pueden** rodear a un enemigo?

Hay varias razones combinadas: la heurística, la naturaleza discreta del espacio y la aleatoriedad distribuyen agentes. explico con ejemplos.

### a) Descenso hacia el objetivo (gradiente)

Todos los atacantes aplican la misma regla: **cada paso reduce la distancia al objetivo** (si hay movimiento posible). Si varios atacantes se encuentran en distintos lugares alrededor del objetivo, sus movimientos individuales tienden a **converger** hacia ese punto desde diferentes direcciones.

Ejemplo:

* Objetivo en (5,5). Atacantes en (3,5), (5,3) y (7,5).

  * A(3,5) verá que moverse +x (derecha) reduce distancia → se mueve a (4,5).
  * B(5,3) verá que +y (abajo) reduce → (5,4).
  * C(7,5) verá que -x (izquierda) reduce → (6,5).
    Tras unas iteraciones se aproximan por lados distintos — ya tienes acercamiento de varios flancos.

### b) Empates y diversificación

Si un atacante está en diagonal respecto al objetivo (por ejemplo en (3,3) hacia (5,5)), **moverse a (4,3) o a (3,4)** reduce la distancia en la misma cantidad. Si todos eligieran siempre la misma (por ejemplo (4,3)) terminarían alineados y seguirían en columna/filas (persecución). **La elección aleatoria entre las dos** con alta probabilidad hará que algunos elijan x y otros y → se reparte el ataque en dos ejes: eso **rompe la sincronía** y produce un acercamiento desde direcciones distintas.

### c) Efecto colectivo (emergente)

Rodear a un enemigo no lo hace un solo agente: se necesita que **en varios pasos distintos, distintos agentes ocupen las casillas que rodean al objetivo**. El algoritmo favorece acercarse, y dado que la selección entre movimientos equivalentes puede ir a izquierda o derecha (o arriba/abajo), distintos agentes tienden a “repartirse” espontáneamente por las casillas alrededor del enemigo. Con suficiente número de atacantes y espacio libre, la probabilidad de cubrir todas las casillas adyacentes aumenta.

#### Probabilidad (intuitiva)

Si hay `k` posiciones frontera alrededor del objetivo libres y `m` atacantes que se acercan, y cada atacante tiene varias opciones equivalentes que lo acercan a distintas fronteras, la probabilidad de que **al menos una** elija una posición dada crece con `m`. Con muchos atacantes y desempates aleatorios independientes, es muy probable que se llenen las diferentes casillas alrededor del objetivo (ataque distribuido).

### d) Evitar congestión

La comprobación `game_board[...] == 0` evita pisar casillas ocupadas, así si varios se acercan a la misma celda en el mismo turno (pero sólo uno actuará por semáforo), los demás la verán ocupada la siguiente vez y elegirán otra opción igualmente buena — eso hace que los atacantes reintenten o tomen alternativas, favoreciendo que la masa de atacantes se "espacie" alrededor del objetivo.

---

# 5) Ejemplo concreto paso a paso (mini-simulación mental)

Tablero pequeño, objetivo T en (5,5).
Atacantes: A(3,5), B(4,6), C(6,4).

**Turno 1** (cada uno con la regla):

* A: vecinos libres que reducen distancia → (4,5) es único → A→(4,5).
* B: vecinos que reducen → (4,5) y (5,6) quizá, pero (4,5) ahora será elegido por A (si semáforo serializa, B verá la nueva ocupación y elegirá la otra).
* C: elige (5,4) o (6,5) según desempate.

Al cabo de unas iteraciones, alguno ocupará (5,4), otro (4,5), otro (6,5) — ya tienes flancos. Si hay más atacantes, acabarán ocupando más posiciones alrededor de (5,5) hasta cercarlo.

---

# 6) Limitaciones y situaciones donde **NO** funciona bien

* **Pocos atacantes**: si hay menos atacantes que casillas alrededor del objetivo, no habrá surround completo.
* **Bloqueos/obstáculos**: si hay muros o agentes aliados bloqueando rutas, el greedy Manhattan puede quedar atrapado o dar vueltas.
* **Rejillas estrechas**: todos se alinean y persiguen al mismo objetivo (lockstep) si la aleatoriedad no rompe la simetría.
* **Sin semáforo**: si se protege lectura/escritura, concurrencia puede crear inconsistencias (dos jugadores ocupando misma celda).
* **Oscilaciones**: sin control, dos jugadores vecinos pueden “intercambiar” casillas en turnos sucesivos → comportamiento errático.

---

# 7) Mejoras prácticas (si queremos que el surround sea **más fiable**)

a) **Mejor heurística** (score) a elegir entre candidatas:

```text
score = -distance_to_target + α * adjacent_allies_after_move + β * adjacent_enemy_near_target_after_move
```

Es decir, entre movimientos que reducen distancia, preferir los que:

* aumentan el número de aliados en posiciones útiles (vínculo)
* o maximizan el número de casillas adyacentes al objetivo que quedarían cubiertas.

b) **Reservas / claims**: usar una pequeña tabla (o reservar en la cola de mensajes) donde un jugador «marca» una casilla objetivo próxima para que otros no la reclamen — evita duplicados.

c) **Roles / asignación**: asignar tareas (flanco izquierdo, frontal, bloqueo) mediante mensajes: el primer atacante del grupo “anuncia” las coordenadas que planea cubrir y los otros eligen otras cubiertas.

d) **Lookahead (1-2 pasos)**: no sólo evaluar distancia inmediata sino el siguiente paso posible para evitar callejones.

e) **Desempate ponderado**: en vez de totalmente uniform random, usa `prob ∝ weight(move)` donde `weight` favorece movimientos que aumenten cobertura.

f) **Pheromone / marca temporal**: escribir en memoria compartida marcas temporales para advertir "hoy voy a ocupar la casilla X" (y caducan), usando semáforo para sincronizar.

g) **Pathfinding (A*, BFS)*\* para sortear obstáculos reales. Tu heurística actual es O(1) por vecino (muy barata); A\* costará más pero encontrará rutas si hay bloques.

---

# 8) Problemas prácticos en el código actual

* **Sembrado de `rand()`**: si varios procesos usan la misma seed (ej. nacen al mismo segundo y usan un PID similar) podrían generar sequences correlacionadas. Ya está usada `srand(time(NULL)+pid)` en `place_player_randomly`, lo cual ayuda; hay que asurarse de no resembrar cada turno (se hace solo al inicio).
* **`best_count` == 0** imposible cuando `move == true`, pero siempre vigila que `best_count` > 0 antes de `rand() % best_count`.
* **Protección con semáforo**: confirmo que ya  hago lock alrededor de lectura de tablero → cálculo → escritura. en `play_turn`.
* **Mensajería**: El sistema de mensajes (primer atacante envía target) favorece coordinación: el que recibe prioriza dicho objetivo, lo que ayuda a agrupar atacantes.

---

# 9) Pequeña receta (si queremos implementar una mejora simple, sin romper tu diseño)

Sustituir el desempate puramente aleatorio por un desempate que favorezca **variedad angular**:

1. Para cada `best_move` calcula `angle_bucket = sign(target_x - nx), sign(target_y - ny)` (p. ej. NE, NW, SE, SW o eje).
2. Mantén preferencia por la **dirección menos usada** por aliados cercanos (cuenta aliados en esas bucket-directions).
3. Si todas iguales, desempate aleatorio.

Esto aumenta la probabilidad de distribuir atacantes por diferentes flancos.

---

# 10) Conclusión y respues a tu duda inicial (no me digas que no); **¿Por qué el desempate aleatorio ayuda a rodear?**

* **¿Qué hace exactamente el algoritmo?**
  Revisa vecinos libres, calcula Manhattan, guarda los que **mínimizan la distancia**, y mueve aleatoriamente entre ellos.

* **¿Por qué el desempate aleatorio ayuda a rodear?**
  Porque rompe simetrías: cuando una casilla «reduce distancia» de varias formas (por X o por Y), la elección aleatoria reparte agentes por distintos ejes; combinado con muchos agentes y la regla de no pisar casillas ocupadas, es muy probable que diferentes atacantes ocupen distintas casillas en torno al objetivo — el resultado emergente es «rodear».

