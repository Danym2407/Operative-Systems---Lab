# Operating Systems Laboratory (COMP2040)

Practical lab exercises for the Operating Systems course at VinUniversity. The goal is to bridge high-level application code and low-level kernel abstractions: how library calls map to system calls, how processes and memory behave, and how POSIX interfaces work in practice.

Each lab lives in its own folder with its own README documenting what was done, how to compile it, and the answers to the lab questions.

## Labs

| Lab | Topic | Folder | Status |
|-----|-------|--------|--------|
| 01 | C foundations, OS services and system calls | [`Lab01/`](Lab01/) | In progress |
| 02 | _TBD_ | `Lab02/` | Not started |
| 03 | _TBD_ | `Lab03/` | Not started |

## Repository Structure

```text
.
├── README.md        # This file: index of all labs
├── .gitignore       # Ignores compiled binaries
├── Lab01/
│   ├── README.md    # What was done in Lab 01
│   ├── task1_hello.c
│   ├── task1_hello.png
│   ├── task2_modularExercise/
│   │   ├── main.c
│   │   ├── math_operations.c
│   │   └── math_operations.h
│   └── ...
├── Lab02/
│   └── README.md
└── ...
```

## Environment

| Item | Value |
|------|-------|
| OS | Ubuntu on WSL (Windows Subsystem for Linux) |
| Editor | Visual Studio Code with the WSL extension |
| Compiler | `gcc` |
| Flags | `-Wall -Wextra -g` |
| Language | C (POSIX API) |

## Quick Start

```bash
git clone https://github.com/Danym2407/Operative-Systems---Lab.git
cd Operative-Systems---Lab
```

Compile and run a single-file program from inside a lab folder:

```bash
cd Lab01
gcc -Wall -Wextra task1_hello.c -o task1_hello
./task1_hello
```

Compile a multi-file program (for example Lab 01, Task 2):

```bash
cd Lab01/task2_modularExercise
gcc -Wall -Wextra main.c math_operations.c -o math_demo
./math_demo
```

Each lab README lists the exact commands for its own tasks.

## Conventions

- One folder per lab: `LabNN/`, each with its own `README.md`.
- Source files are named `taskN_description.c`; multi-file tasks go in their own subfolder.
- Terminal screenshots used as evidence are saved next to the code (`taskN_*.png`).
- Compiled binaries are not committed (see `.gitignore`).

## Adding a New Lab

```bash
mkdir Lab02
cd Lab02
touch README.md
```

Then add a row to the **Labs** table above and describe the work in `Lab02/README.md`.

## Author

Daniela Méndez Ramírez, VinUniversity (exchange semester), Hanoi, Vietnam.