# Lab 02 - Task 4: Private vs. Shared Memory (`mmap`)

**Laboratorio 02 - Tarea 4: Comparación entre Memoria Privada y Memoria Compartida**

This program demonstrates the difference between private process memory and explicitly shared memory using the `mmap()` system call. It illustrates how processes communicate through shared memory and how memory isolation works after calling `fork()`.

Este programa demuestra la diferencia entre la memoria privada de los procesos y la memoria compartida explícita mediante la llamada al sistema `mmap()`. También muestra cómo se comunican los procesos mediante memoria compartida y cómo funciona el aislamiento de memoria después de ejecutar `fork()`.

---

## 1. Metadata / Metadatos

- **Source file / Archivo fuente:** `task4_shared_memory.c`
- **Author / Autora:** Daniela Méndez Ramírez
- **Date / Fecha:** 2026-10-11
- **Version / Versión:** 1.0
- **Course / Materia:** Operating Systems (COMP2040)
- **Lab / Práctica:** Lab 02 - Task 4

---

## 2. Objectives / Objetivos

The objectives of this exercise are:

- Understand the difference between private and shared memory.
- Observe how `fork()` creates a child process.
- Demonstrate how `mmap()` allows processes to access a shared memory region.
- Use `waitpid()` to synchronize the parent process with the child.
- Compare shared memory with pipes as inter-process communication (IPC) mechanisms.

Los objetivos de este ejercicio son:

- Comprender la diferencia entre memoria privada y memoria compartida.
- Observar cómo `fork()` crea un proceso hijo.
- Demostrar cómo `mmap()` permite que varios procesos accedan a una región de memoria compartida.
- Utilizar `waitpid()` para sincronizar el proceso padre con el hijo.
- Comparar la memoria compartida con las tuberías (*pipes*) como mecanismos de comunicación entre procesos (IPC).

---

## 3. Core Operating System Concepts / Conceptos Clave

### 3.1. Process Isolation / Aislamiento de Procesos

When a process calls `fork()`, the operating system creates a child process with its own virtual address space. The child initially inherits the parent's memory contents, but ordinary private memory is logically independent between the two processes.

When either process modifies a private variable, the change does not modify the corresponding variable in the other process. Operating systems commonly implement this behavior using copy-on-write.

Cuando un proceso ejecuta `fork()`, el sistema operativo crea un proceso hijo con su propio espacio de direcciones virtuales. El hijo hereda inicialmente el contenido de la memoria del padre, pero la memoria privada es lógicamente independiente entre ambos procesos.

Cuando uno de los procesos modifica una variable privada, el cambio no modifica la variable correspondiente del otro proceso. Los sistemas operativos suelen implementar este comportamiento mediante *copy-on-write*.

### 3.2. Shared Memory / Memoria Compartida

Shared memory is an inter-process communication (IPC) mechanism that allows multiple processes to access the same memory region.

Instead of sending data through an intermediate kernel buffer, processes can read and write directly to the mapped shared region. This can reduce communication overhead, especially when exchanging large amounts of data.

La memoria compartida es un mecanismo de comunicación entre procesos (IPC) que permite que varios procesos accedan a una misma región de memoria.

En lugar de enviar datos mediante un búfer intermedio del kernel, los procesos pueden leer y escribir directamente en la región compartida. Esto puede reducir el costo de comunicación, especialmente cuando se intercambian grandes cantidades de datos.

### 3.3. The `mmap()` System Call

The `mmap()` system call creates a mapping between a memory region and a process's virtual address space.

The program uses the following parameters:

- `NULL`: Allows the operating system to select the mapping address.
- `sizeof *shared_value`: Specifies the size required to store an integer.
- `PROT_READ | PROT_WRITE`: Grants permission to read and write the mapped memory.
- `MAP_SHARED`: Makes updates to the mapping visible to other processes sharing the same region.
- `MAP_ANONYMOUS`: Creates an anonymous mapping that is not backed by a regular file.
- `-1`: Used as the file descriptor for an anonymous mapping.
- `0`: Specifies the mapping offset.

La llamada al sistema `mmap()` crea un mapeo entre una región de memoria y el espacio de direcciones virtuales de un proceso.

El programa utiliza los siguientes parámetros:

- `NULL`: Permite que el sistema operativo seleccione la dirección del mapeo.
- `sizeof *shared_value`: Especifica el tamaño necesario para almacenar un entero.
- `PROT_READ | PROT_WRITE`: Permite leer y escribir en la memoria mapeada.
- `MAP_SHARED`: Hace que las modificaciones sean visibles para otros procesos que comparten la región.
- `MAP_ANONYMOUS`: Crea un mapeo anónimo que no está respaldado por un archivo convencional.
- `-1`: Se utiliza como descriptor de archivo para el mapeo anónimo.
- `0`: Especifica el desplazamiento inicial del mapeo.

### 3.4. The `fork()` System Call

The `fork()` system call creates a child process by duplicating the calling process.

Its return value determines which process is executing:

- `-1`: An error occurred.
- `0`: The code is executing in the child process.
- A positive value: The code is executing in the parent process, and the value is the child's process ID.

La llamada al sistema `fork()` crea un proceso hijo a partir del proceso que la ejecuta.

Su valor de retorno permite identificar el proceso que está ejecutando el código:

- `-1`: Ocurrió un error.
- `0`: El código se está ejecutando en el proceso hijo.
- Un valor positivo: El código se está ejecutando en el proceso padre y el valor corresponde al identificador del hijo.

### 3.5. The `waitpid()` System Call

The `waitpid()` system call allows the parent process to wait for a specific child process to terminate.

In this exercise, it ensures that the child finishes writing to shared memory before the parent reads and prints the result.

La llamada al sistema `waitpid()` permite que el proceso padre espere a que termine un proceso hijo específico.

En este ejercicio, garantiza que el hijo termine de escribir en la memoria compartida antes de que el padre lea e imprima el resultado.

### 3.6. The `munmap()` System Call

The `munmap()` system call removes a memory mapping from the calling process's virtual address space.

The parent uses it to release its mapping after the result has been printed.

La llamada al sistema `munmap()` elimina un mapeo de memoria del espacio de direcciones virtuales del proceso que la ejecuta.

El padre la utiliza para liberar su mapeo después de imprimir el resultado.

---

## 4. Pipes vs. Shared Memory / Comparación entre Pipes y Memoria Compartida

| Feature / Característica | Pipes (Task 3) | Shared Memory (Task 4) |
|---|---|---|
| Communication model / Modelo de comunicación | Kernel-buffered byte stream / Flujo de bytes administrado por el kernel | Direct access to a shared memory region / Acceso directo a una región compartida |
| System calls / Llamadas al sistema | Uses `read()` and `write()` for data transfer / Utiliza `read()` y `write()` para transferir datos | Uses `mmap()` for setup; normal memory accesses for reading and writing / Utiliza `mmap()` para configurar el mapeo y accesos normales a memoria para leer y escribir |
| Data copying / Copia de datos | Typically copies data through a kernel-managed pipe buffer / Normalmente copia los datos mediante un búfer del kernel | Avoids the repeated buffer copies associated with ordinary pipe transfers / Evita las copias repetidas de búfer asociadas con las transferencias convencionales mediante pipes |
| Synchronization / Sincronización | The kernel manages blocking when a pipe is empty or full / El kernel administra el bloqueo cuando una tubería está vacía o llena | Requires explicit coordination when processes access shared data concurrently / Requiere coordinación explícita cuando los procesos acceden simultáneamente a los datos |
| Performance / Rendimiento | Convenient for streams and relatively small messages / Conveniente para flujos de datos y mensajes relativamente pequeños | Can be efficient for frequent communication and large data regions / Puede ser eficiente para comunicación frecuente y regiones de datos grandes |
| Typical use / Uso habitual | Sequential communication between processes / Comunicación secuencial entre procesos | Frequent access to common data / Acceso frecuente a datos comunes |

**Important:** Shared memory does not automatically synchronize concurrent accesses. Synchronization mechanisms such as mutexes, semaphores, or process-shared atomic operations may be necessary when multiple processes modify shared data.

**Importante:** La memoria compartida no sincroniza automáticamente los accesos concurrentes. Pueden ser necesarios mecanismos como mutexes, semáforos u operaciones atómicas entre procesos cuando varios procesos modifican datos compartidos.

---

## 5. Program Execution / Ejecución del Programa

The program follows these steps:

1. **Initialization:** The parent initializes `private_value` to `10`.
2. **Shared mapping:** The program allocates an anonymous shared memory region using `mmap()` and initializes `*shared_value` to `10`.
3. **Process creation:** The parent calls `fork()` to create the child process.
4. **Private modification:** The child assigns `99` to its own copy of `private_value`.
5. **Shared modification:** The child assigns `99` to `*shared_value`, modifying the shared memory region.
6. **Child termination:** The child exits using `_exit(0)`.
7. **Synchronization:** The parent waits for the child with `waitpid()`.
8. **Output:** The parent prints its private variable and the value stored in shared memory.
9. **Cleanup:** The parent releases its mapping using `munmap()`.

El programa sigue estos pasos:

1. **Inicialización:** El padre inicializa `private_value` con el valor `10`.
2. **Mapeo compartido:** El programa reserva una región de memoria compartida anónima mediante `mmap()` e inicializa `*shared_value` con el valor `10`.
3. **Creación de procesos:** El padre ejecuta `fork()` para crear el proceso hijo.
4. **Modificación privada:** El hijo asigna `99` a su propia copia de `private_value`.
5. **Modificación compartida:** El hijo asigna `99` a `*shared_value`, modificando la región de memoria compartida.
6. **Finalización del hijo:** El hijo termina mediante `_exit(0)`.
7. **Sincronización:** El padre espera al hijo mediante `waitpid()`.
8. **Salida:** El padre imprime su variable privada y el valor almacenado en la memoria compartida.
9. **Liberación de recursos:** El padre libera su mapeo mediante `munmap()`.

--

## 6. Compilation and Execution / Compilación y Ejecución

### Requirements / Requisitos

- A C compiler such as GCC.
- A Unix-like operating system that supports `fork()` and anonymous shared mappings, such as Linux.
- A terminal.

Se necesita un compilador de C, como GCC, y un sistema operativo compatible con `fork()` y los mapeos anónimos compartidos, como Linux.

### Compilation / Compilación

Run the following command in the directory containing `task4_shared_memory.c`:

```bash
gcc -std=c11 -Wall -Wextra task4_shared_memory.c -o task4_shared_memory
```

Este comando compila el programa utilizando el estándar C11 y habilita advertencias adicionales mediante `-Wall` y `-Wextra`.

### Execution / Ejecución

```bash
./task4_shared_memory
```

### Expected Output / Salida Esperada

```text
parent: private=10 shared=99
```

The output shows that the parent's private variable remains `10`, while the shared variable contains `99`, the value written by the child.

La salida demuestra que la variable privada del padre permanece en `10`, mientras que la variable compartida contiene `99`, el valor escrito por el hijo.

---

## 7. Lab Report Questions / Preguntas del Reporte

### (a) Provide the compilation command and the program output.

**Answer:**

Compilation command:

```bash
gcc -std=c11 -Wall -Wextra task4_shared_memory.c -o task4_shared_memory
```

Program output:

```text
parent: private=10 shared=99
```

**Respuesta en español:**

El comando de compilación utiliza GCC con el estándar C11 y las opciones de advertencia `-Wall` y `-Wextra`. Al ejecutar el programa, se obtiene la salida `parent: private=10 shared=99`.

### (b) Why did only the shared value change in the parent?

**Answer:**

The variable `private_value` belongs to the process's private virtual memory. After `fork()`, the child has an independent logical copy of this variable, so assigning `99` to it does not modify the parent's value. In contrast, `shared_value` points to a memory region created with `mmap()` and the `MAP_SHARED` flag. Both processes access the same shared memory region, so the child's assignment of `99` is visible to the parent.

**Respuesta en español:**

La variable `private_value` pertenece a la memoria virtual privada del proceso. Después de ejecutar `fork()`, el hijo tiene una copia lógica independiente de esta variable, por lo que asignarle `99` no modifica el valor del padre. En cambio, `shared_value` apunta a una región de memoria creada mediante `mmap()` con la bandera `MAP_SHARED`. Ambos procesos acceden a la misma región compartida, por lo que la asignación de `99` realizada por el hijo es visible para el padre.

### (c) How does this IPC mechanism differ from the pipe used in Task 3?

**Answer:**

A pipe transfers data through a kernel-managed buffer, typically using `read()` and `write()` system calls. Shared memory allows processes to access the same mapped memory region directly, avoiding the repeated data-buffer transfers associated with ordinary pipe communication. However, shared memory requires explicit synchronization when concurrent access could cause conflicts.

**Respuesta en español:**

Una tubería transfiere datos mediante un búfer administrado por el kernel, generalmente utilizando las llamadas al sistema `read()` y `write()`. La memoria compartida permite que los procesos accedan directamente a una misma región de memoria mapeada, evitando las transferencias repetidas de datos entre búferes propias de la comunicación convencional mediante pipes. Sin embargo, la memoria compartida requiere sincronización explícita cuando el acceso concurrente puede provocar conflictos.

### (d) If both processes wrote to the shared integer at the same time, what could go wrong?

**Answer:**

A race condition could occur because the result would depend on the order in which the writes take place. If both processes write different values without synchronization, one write may overwrite the other, making the final value unpredictable. Synchronization mechanisms, such as process-shared mutexes or semaphores, can coordinate access and prevent conflicting operations.

**Respuesta en español:**

Podría producirse una condición de carrera (*race condition*), porque el resultado dependería del orden en que se realicen las escrituras. Si ambos procesos escriben valores diferentes sin sincronización, una escritura podría sobrescribir a la otra y el valor final dependería de cuál se ejecute al final. Los mecanismos de sincronización, como mutexes compartidos entre procesos o semáforos, permiten coordinar el acceso y evitar operaciones conflictivas.

---

## 8. Conclusion / Conclusión

This exercise demonstrates the difference between private process memory and explicitly shared memory. After `fork()`, modifications to a private variable in the child do not change the corresponding value in the parent. However, modifications to a memory region mapped using `mmap()` with `MAP_SHARED` are visible to both processes.

The program also demonstrates the importance of synchronization through `waitpid()`, which ensures that the child finishes before the parent reads the shared value. Finally, the comparison with pipes highlights the main trade-off between the two IPC mechanisms: pipes provide a convenient kernel-managed communication channel, while shared memory provides direct access to common data but requires careful coordination when accesses may occur concurrently.

Este ejercicio demuestra la diferencia entre la memoria privada de los procesos y la memoria compartida explícita. Después de ejecutar `fork()`, las modificaciones de una variable privada en el hijo no cambian el valor correspondiente en el padre. Sin embargo, las modificaciones realizadas en una región mapeada mediante `mmap()` con `MAP_SHARED` son visibles para ambos procesos.

El programa también demuestra la importancia de la sincronización mediante `waitpid()`, que garantiza que el hijo termine antes de que el padre lea el valor compartido. Finalmente, la comparación con las tuberías destaca la principal diferencia entre ambos mecanismos IPC: los pipes proporcionan un canal de comunicación administrado por el kernel, mientras que la memoria compartida ofrece acceso directo a los datos comunes, pero requiere coordinación cuidadosa cuando pueden producirse accesos concurrentes.
