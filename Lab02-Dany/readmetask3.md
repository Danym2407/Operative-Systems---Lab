# Laboratorio 02 - Tarea 3: Comunicación Inter-Procesos con Tuberías Unidireccionales (`pipe`)

Este programa demuestra cómo dos procesos independientes (padre e hijo) se comunican e intercambian datos a través de los límites de sus memorias privadas utilizando una tubería (*pipe*) unidireccional administrada por el kernel del sistema operativo.

---

## Información del Archivo y Autoría

* **Archivo:** `task3_pipe.c`[cite: 6]
* **Autor:** Daniela Méndez Ramírez
* **Fecha:** 2026-10-11
* **Versión:** 1.0
* **Materia:** Sistemas Operativos (COMP2040) - Práctica 2[cite: 1, 6]

---

## Conceptos Clave de Sistemas Operativos

* **Canal FIFO en el Kernel:** Un *pipe* no reside en la memoria de usuario del proceso, sino en un búfer circular en el espacio del kernel del sistema operativo. Sigue una política de cola FIFO (*First In, First Out*).
* **Descriptores de Archivo (`File Descriptors`):** Al invocar `pipe(fd)`, el sistema operativo asigna dos descriptores enteros en el arreglo `fd`[cite: 6]:
  * `fd[1]`: Extremo de **escritura** (*write end*). Por aquí se envían datos usando `write()`.
  * `fd[0]`: Extremo de **lectura** (*read end*). Por aquí se extraen datos usando `read()`.
* **Unidireccional (*One-way*):** Los datos únicamente pueden fluir en un sentido a través de una misma tubería.
* **Búferes de Bytes y Cadenas en C:** La llamada al sistema `read()` transfiere bytes puros sin interpretar[cite: 10]. Como las funciones como `printf("%s")` exigen un terminador nulo `'\0'`, el programa debe reservar un byte adicional y colocar `'\0'` explícitamente al final de los datos leídos[cite: 6, 10].

---

## ¿Por qué se deben cerrar los extremos no utilizados?

1. **Detección de Fin de Transmisión (*EOF*):** La función `read()` se desbloquea retornando `0` (indicando fin de archivo) solo cuando **todos** los extremos de escritura abiertos en el sistema operativo han sido cerrados[cite: 6]. Si el padre mantiene abierto su propio `fd[1]`, nunca recibirá la señal de fin de datos si espera más transmisiones[cite: 6].
2. **Detección de Tubería Rota (*SIGPIPE*):** Si un proceso intenta escribir en un pipe sin ningún lector activo, el kernel emite la señal `SIGPIPE` para evitar el desperdicio de ciclos de CPU[cite: 6].
3. **Liberación de Descriptores:** Cada proceso tiene un límite máximo de descriptores de archivo abiertos por el kernel.

---

## Flujo Paso a Paso de la Tarea 3

1. **Creación del Pipe:** El padre ejecuta `pipe(fd)` antes del `fork()`[cite: 6].
2. **Duplicación (`fork()`):** El hijo hereda copias idénticas de `fd[0]` y `fd[1]`[cite: 6].
3. **Configuración de Roles:**
   * **El Hijo (Emisor):** Cierra `fd[0]` (no lee)[cite: 6]. Escribe el mensaje en `fd[1]` con `write()`. Cierra `fd[1]` y sale con `_exit(0)`[cite: 6].
   * **El Padre (Receptor):** Cierra `fd[1]` (no escribe)[cite: 6]. Lee desde `fd[0]` con `read()` hacia un búfer de 128 bytes[cite: 6]. Añade el byte `'\0'` al final[cite: 6, 10]. Imprime `"Parent received: "` seguido del mensaje. Cierra `fd[0]`[cite: 6].
4. **Sincronización:** El padre llama a `waitpid()` para esperar la terminación limpia del hijo y evaluar su estado[cite: 2, 6].
---
## Compilación y Ejecución
```bash
gcc -std=c11 -Wall -Wextra task3_pipe.c -o task3_pipe
./task3_pipe
```
---
## Salida en Terminal

```bash
Parent received: Hello from the child
```

# English Version (`README.md`)

```markdown
# Lab 02 - Task 3: Inter-Process Communication via One-Way Pipe

This program demonstrates how two independent processes (parent and child) transfer data across isolated virtual memory address spaces using a unidirectional pipe managed by the operating system kernel.

---

## What is a Pipe in Operating Systems?

In Task 1, we observed that process memory is strictly private. A **pipe** bypasses this memory isolation by providing a kernel-buffered stream communication channel[cite: 1, 2, 6]:
* Operates according to the **FIFO** (*First In, First Out*) principle: bytes written first are read first.
* It is **unidirectional** (*half-duplex*): data moves in a single direction from a writer to a reader.

### Understanding File Descriptors (`fd`)
The `pipe(fd)` system call initializes the kernel buffer and populates a 2-element integer array with file descriptors[cite: 6]:
* **`fd[1]` (Write End):** The stream input. Data submitted via `write()` enters here.
* **`fd[0]` (Read End):** The stream output. Data is extracted here via `read()`.

---

## Step-by-Step Architecture and Flow

1. **Pipe Creation:** The parent invokes `pipe(fd)` to initialize the channel[cite: 6].
2. **Process Forking (`fork()`):** Upon calling `fork()`, the file descriptor table is cloned into the child process[cite: 6]. Both parent and child now reference `fd[0]` and `fd[1]`[cite: 6].
3. **Closing Unused Ends:**
   * **Child Process:** Acts solely as the **writer**[cite: 2]. It immediately closes `fd[0]` via `close(fd[0])`[cite: 6].
   * **Parent Process:** Acts solely as the **reader**[cite: 2]. It immediately closes `fd[1]` via `close(fd[1])`[cite: 6].
4. **Child Writes Data:** The child determines the payload size using `strlen(message)` and writes bytes to `fd[1]`[cite: 6, 10]. It then closes `fd[1]` and exits via `_exit(0)`[cite: 6].
5. **Parent Reads Data:**
   * `read()` retrieves raw bytes; it **does not append a null terminator `\0`**[cite: 10].
   * The parent reads at most `sizeof(buffer) - 1` bytes and manually sets `buffer[bytes_read] = '\0'` before printing[cite: 6, 10].
6. **Harvesting:** The parent closes `fd[0]` and waits for the child's termination using `waitpid()`[cite: 6].

---

## Why Must Unused Pipe Ends Be Closed?

* **End-of-File (EOF) Detection:** The `read()` call blocks until data arrives or until **all** open write descriptors referencing the pipe are closed (signaling EOF and returning `0`). If the parent does not close its copy of `fd[1]`, `read()` could hang indefinitely waiting for input.
* **Broken Pipe Signal (`SIGPIPE`):** If a process writes to a pipe with no active read descriptors, the kernel terminates the writer with a `SIGPIPE` signal to prevent dangling buffer writes.

---

## Build and Run Instructions
```bash
gcc -std=c11 -Wall -Wextra task3_pipe.c -o task3_pipe
./task3_pipe
```
---
## Expected Output
```bash
Parent received: Hello from the child
```
---
