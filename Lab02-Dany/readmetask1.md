---
# Laboratorio 02 - Tarea 1: `fork()` y Memoria Privada de Procesos

Este programa demuestra cómo el sistema operativo crea un proceso hijo independiente usando la llamada al sistema `fork()`, cómo cada proceso mantiene su propio espacio de memoria privado y cómo el proceso padre sincroniza la ejecución esperando a que el hijo termine mediante `waitpid()`.

---

## Conceptos Clave

* **Proceso:** Una instancia de un programa en ejecución gestionada por el sistema operativo, con su propio espacio de memoria virtual y recursos asignados.
* **PID (*Process Identifier*):** Número entero único asignado por el sistema operativo para identificar a cada proceso activo.
* **PPID (*Parent Process Identifier*):** PID del proceso padre que creó al proceso actual.
* **`fork()`:** Llamada al sistema que clona el proceso actual creando un proceso hijo:
  * Retorna el **PID del hijo** al proceso padre.
  * Retorna **`0`** al proceso hijo recién creado.
  * Retorna **`-1`** si ocurre un fallo al crear el proceso.
* **Aislamiento de Memoria (*Copy-on-Write*):** El proceso hijo recibe una copia exacta del espacio de memoria del padre. Cualquier cambio en las variables del hijo no afecta a las variables del padre.
* **`waitpid()`:** Llamada al sistema que bloquea al padre hasta que el hijo indicado termina, evitando condiciones de carrera y la creación de procesos zombi.

---

## Código Fuente Completo (`task1_fork.c`)

## Compilacion y Ejecución:
```bash
gcc -std=c11 -Wall -Wextra task1_fork.c -o task1_fork
./task1_fork
```

## Salida Esperada en terminal:
```bash
Child: PID=23052 PPID=23051 value=99
parent: PID=23051 value=10
```

---

# English Version (`README.md`)

```markdown
# Lab 02 - Task 1: `fork()` and Private Process Memory

This program demonstrates how the operating system creates an independent child process using the `fork()` system call, how each process maintains its own private memory space, and how the parent process synchronizes execution by waiting for the child to terminate using `waitpid()`.

---

## Key Concepts

* **Process:** An instance of a computer program being executed by the operating system, with its own allocated virtual memory space and system resources.
* **PID (Process Identifier):** A unique integer assigned by the operating system to identify each running process.
* **PPID (Parent Process Identifier):** The PID of the parent process that spawned the current process.
* **`fork()`:** A system call that clones the calling process to create a child process:
  * Returns the **child's PID** to the parent process.
  * Returns **`0`** to the newly created child process.
  * Returns **`-1`** if process creation fails.
* **Memory Isolation (Copy-on-Write):** The child process receives an exact replica of the parent's memory space. Any modifications made to variables in the child do not affect the variables in the parent.
* **`waitpid()`:** A system call that suspends the parent process until the specified child has terminated, preventing race conditions and zombie processes.

---

## Full Source Code (`task1_fork.c`)

---

## Compilation an Execution:
```bash
gcc -std=c11 -Wall -Wextra task1_fork.c -o task1_fork
./task1_fork
```

## Expected Terminal Output:
```bash
Child: PID=23052 PPID=23051 value=99
parent: PID=23051 value=10
```

---