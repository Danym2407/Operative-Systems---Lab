# Laboratorio 02 - Tarea 2: Reemplazo de Proceso con `exec()` y Estado de Salida

Este ejercicio demuestra la secuencia canónica de UNIX para lanzar programas (`fork` + `exec` + `wait`), mostrando cómo un proceso hijo sustituye su imagen de memoria por un binario completamente nuevo (`task2_worker`) sin cambiar de PID, y cómo el proceso padre recolecta su código de terminación.

---

## ¿Qué está pasando realmente? (El flujo completo)

1. **El Proceso Padre inicia:** Ejecuta `task2_fork_exec`.
2. **Bifurcación (`fork()`):** El padre crea un proceso hijo clonado que tiene su propio PID nuevo.
3. **El Hijo antes del cambio:** El hijo imprime `"before exec: PID=..."` mostrando su identificador asignado.
4. **La sustitución (`execl()`):**
   * El hijo invoca `execl("./task2_worker", "task2_worker", "COMP2040", (char *)NULL)`.
   * El sistema operativo destruye el código anterior del hijo (`task2_fork_exec`) y carga en ese mismo espacio el binario `task2_worker`.
   * **Crucial:** El PID **no cambia**. Sigue siendo el mismo proceso hijo, pero con un programa diferente.
5. **Ejecución del Worker:** `task2_worker` recibe `"COMP2040"` en `argv[1]`, imprime su PID (que coincide con el anterior) y termina con `return 0`.
6. **El Padre recolecta el resultado:** El padre estaba detenido en `waitpid(pid, &status, 0)`. Cuando el worker finaliza, el padre se desbloquea, inspecciona el resultado con `WIFEXITED` y muestra que el hijo terminó con código `0`[cite: 1, 4].

---

## Conceptos Clave

* **`argc` y `argv`:** Parámetros de la función `main` en C[cite: 5]. `argc` almacena el conteo total de argumentos de consola y `argv` contiene el arreglo de cadenas de texto[cite: 5]. `argv[0]` es siempre el nombre del ejecutable y `argv[1]` es el primer argumento suministrado[cite: 5].
* **Familia `exec` (`execl`):** Funciones del sistema que reemplazan la imagen de memoria del proceso que las llama por un nuevo archivo ejecutable[cite: 1, 4]. Si la llamada tiene éxito, **nunca retorna** al código original[cite: 4].
* **Diferencia entre `fork()` y `exec()`:**
  * `fork()` crea un **nuevo proceso** (nuevo PID) duplicando el actual[cite: 1, 4].
  * `exec()` **no crea un nuevo proceso**; sustituye el código del proceso actual manteniendo el mismo PID.
* **`_exit(127)`:** Terminación inmediata del proceso hijo sin limpiar búferes estándar de E/S de C, utilizado típicamente cuando una llamada a `exec` falla[cite: 4].
* **Macros de estado (`<sys/wait.h>`):**
  * `WIFEXITED(status)`: Evalúa a verdadero si el hijo terminó de forma controlada (mediante `exit()` o `return`)[cite: 4, 5].
  * `WEXITSTATUS(status)`: Extrae los 8 bits inferiores que representan el valor real de retorno del hijo[cite: 4].

---

## Archivos Involucrados

* `task2_worker.c`: Programa de destino. Valida los argumentos pasados y muestra su PID junto con la palabra recibida[cite: 1, 5].
* `task2_fork_exec.c`: Programa controlador. Crea el hijo, lanza al worker mediante `execl` y recolecta el estado con `waitpid`[cite: 1, 4].

---

## Compilación y Ejecución

Compila ambos programas en el mismo directorio:
```bash
gcc -std=c11 -Wall -Wextra task2_worker.c -o task2_worker
gcc -std=c11 -Wall -Wextra task2_fork_exec.c -o task2_fork_exec
```
---
## Salida Esperada en la Terminal

```bash
before exec: PID=71240
worker: PID=71240 word=COMP2040
parent: child exited with status 0
```

### Detalles Avanzados de Implementación

#### ¿Por qué es necesario `fflush(stdout)` antes de `exec`?
En C, las funciones de salida estándar como `printf()` almacenan los datos temporalmente en un búfer de memoria (*line buffering*). Al ejecutar `execl()`, el sistema operativo descarta toda la memoria del proceso hijo para cargar el nuevo binario. Si no se llama explícitamente a `fflush(stdout)`, los datos en el búfer que aún no se hayan volcado a la terminal podrían destruirse antes de que el nuevo programa comience su ejecución.

#### Mecanismo de Reemplazo de Proceso (`execl`)
La llamada `execl()` toma la ruta del ejecutable seguido de los argumentos de la línea de comandos (`argv[0]`, `argv[1]`, ...) y debe finalizar obligatoriamente con un puntero nulo `(char *)NULL`. 
* Si la carga tiene éxito, el hilo de ejecución continúa en la función `main` de `task2_worker` conservando el mismo PID.
* Si el archivo no existe o no tiene permisos de ejecución, `execl()` retorna `-1`, permitiendo que el bloque de error ejecute `perror()` y finalice el proceso con `_exit(127)` para indicar fallo de comando.

#### Inspección del Código de Terminación (`waitpid` y Macros)
El proceso padre utiliza `waitpid(pid, &status, 0)` para suspenderse hasta la finalización del hijo y registrar su estado:
* `WIFEXITED(status)`: Retorna verdadero si el hijo finalizó normalmente (mediante `return` o `exit`).
* `WEXITSTATUS(status)`: Extrae los 8 bits del código de salida devuelto por el hijo (en este caso `0`, correspondiente al término exitoso del worker).