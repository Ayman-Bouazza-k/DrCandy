# Implementation Done — Primera Entrega: Dr. Candy

**Asignatura:** Programació 2 · Universitat Autònoma de Barcelona  
**Fecha:** Marzo 2026

---

## 1. Resumen de la Arquitectura: Lo Proporcionado vs. Lo Implementado

El proyecto parte de una base de código ya estructurada. Los módulos `candy` (`candy.h` / `candy.cpp`), `graphics` (`graphics.h`), `controller` (`controller.h`) y `util` no han sido modificados en ningún momento; su funcionalidad se da por correcta tal como fue entregada.

El trabajo realizado en esta primera entrega se ha centrado exclusivamente en:

- **`board.h` y `board.cpp`:** Implementación completa de toda la lógica del tablero, incluyendo `shouldExplode`, `explodeAndDrop`, `dump` y `load`, así como las estructuras de datos privadas `m_board` y `m_storage`.
- **`game.cpp`:** Implementación parcial y acotada a las funciones `Game::dump()` y `Game::load()`, que actúan como envoltorio de las operaciones equivalentes del tablero, añadiendo la serialización del estado de juego (`m_frameCounter`, `m_score`, `m_gameOver`).

Esta distribución responde directamente al objetivo de la Primera Entrega: **aislar la lógica del tablero del motor gráfico**, de forma que el `Board` sea una unidad autocontenida y testable independientemente de cualquier representación visual.

---

## 2. Lógica de Explosiones: El Núcleo del Tablero

### `shouldExplode(int x, int y)`

Esta función determina si la celda en la posición `(x, y)` forma parte de una línea explosiva. La lógica es la siguiente:

1. Se obtiene el tipo de la pieza central. Si la celda está vacía (`nullptr`), se devuelve `false` inmediatamente.
2. Se evalúan **cuatro ejes independientes**: horizontal, vertical, diagonal principal (↘) y diagonal secundaria (↗).
3. Para cada eje, se recorre hacia los dos sentidos contando las piezas consecutivas del mismo tipo, acumulando el total en un contador que parte de 1 (la propia pieza).
4. Si en cualquiera de los cuatro ejes el total acumulado alcanza o supera la constante `SHORTEST_EXPLOSION_LINE` (definida como 3 en `board.h`), la función devuelve `true` de inmediato.

El uso de `getCell()` como accessor en lugar de acceder directamente a `m_board` garantiza que los accesos fuera de límites devuelvan `nullptr` de forma controlada, evitando lecturas de memoria inválidas.

### `explodeAndDrop()`

Esta es la función más crítica del tablero. Se ha tomado una decisión de diseño importante: **separar el proceso en dos fases durante cada pasada**.

- **Fase 1 — Marcar:** Se recorre el tablero completo llamando a `shouldExplode` en cada celda. Las coordenadas de todas las piezas que deben explotar se acumulan en un vector `to_explode`, sin modificar el tablero todavía.
- **Fase 2 — Borrar y Caer:** Una vez identificadas todas las piezas a eliminar, se borran de golpe y, a continuación, se aplica la gravedad columna a columna, compactando hacia abajo las piezas supervivientes mediante un puntero de escritura (`write_y`).

La razón de esta separación es fundamental: si se eliminasen las piezas en el mismo momento en que se detectan, el estado del tablero quedaría alterado a mitad de la iteración. Esto provocaría que `shouldExplode`, en llamadas posteriores dentro del mismo bucle, evaluase un tablero ya parcialmente modificado, generando **falsos negativos** —piezas que formarían una línea explosiva en el tablero original, pero que ya no la forman tras los borrados parciales previos—. La fase de marcado garantiza que todas las decisiones de explosión se toman sobre un estado consistente del tablero.

El proceso completo se repite en un bucle `while` hasta que ninguna pasada produce nuevas explosiones, modelando así las **explosiones en cadena**.

---

## 3. Gestión de Memoria en la Carga de Ficheros

### Estrategia en `Board::load()`

La carga de un tablero desde fichero presenta un riesgo de memoria si no se gestiona con cuidado. Una implementación naïve podría crear objetos `Candy` como variables locales dentro del bucle de lectura y almacenar sus direcciones en `m_board`. Sin embargo, al salir del ámbito del bucle, esas variables serían destruidas, dejando `m_board` llena de **punteros colgantes** (*dangling pointers*), cuya desreferencia constituiría comportamiento indefinido.

Para evitar esto, se ha empleado la siguiente estrategia, visible en `board.h` y `board.cpp`:

- Se declara en la clase una **matriz estática de objetos** `Candy m_storage[MAX_SIZE][MAX_SIZE]`. Esta matriz tiene la misma vida útil que el propio objeto `Board`, es decir, su memoria permanece válida mientras el tablero exista.
- Durante la carga, cada `Candy` se construye directamente en su posición de `m_storage` mediante asignación: `m_storage[x][y] = Candy(...)`.
- Acto seguido, `m_board[x][y]` se inicializa apuntando a `&m_storage[x][y]`, es decir, a una dirección de memoria **estable y garantizada**.

De esta forma, los punteros en `m_board` apuntan siempre a objetos con duración de almacenamiento bien definida, sin necesidad de gestión dinámica de memoria (`new`/`delete`).

---

## 4. Autocrítica y Próximos Pasos

Se identifican dos aspectos del código actual que, aunque funcionalmente correctos, se reconocen como susceptibles de mejora para la Segunda Entrega.

### Repetitividad en `shouldExplode()`

Los cuatro bloques de la función siguen el mismo patrón: dos bucles simétricos que recorren un eje en sentidos opuestos. El código es correcto y legible, pero presenta una clara repetición estructural. La refactorización natural sería definir un array de **pares de deltas direccionales** —por ejemplo, `{(1,0), (0,1), (1,1), (1,-1)}`— e iterar sobre él, aplicando la misma lógica de conteo para cada dirección. Esto reduciría las cuatro secciones a un único bucle genérico, facilitando el mantenimiento y la adición de nuevas direcciones en el futuro.

### Doble apertura del fichero en `Game::load()`

Al cargar una partida, el fichero se abre dos veces de forma secuencial: primero dentro de `Board::load()`, que lee las dimensiones y el contenido del tablero, y después en `Game::load()`, que vuelve a abrir el mismo fichero, avanza manualmente hasta el final de los datos del tablero leyendo valores *dummy*, y finalmente lee el estado de juego.

Esta redundancia es un compromiso deliberado para mantener la **encapsulación**: se quería que `Board` fuese capaz de leerse a sí mismo de forma autónoma, sin que `Game` tuviese que abrirle la puerta pasándole un stream ya posicionado. Para la Segunda Entrega, lo más limpio sería refactorizar ambas funciones para que `Game::load()` abra el fichero una única vez, pase el `std::ifstream` a `Board::load()`, y continúe leyendo desde ahí, eliminando así la lectura doble sin sacrificar la cohesión.
