# small-shell

A simple shell written in C.

## What is small-shell?

**small-shell** is a basic terminal program designed to demonstrate how a GNU/Linux system manages programs. It focuses on three fundamental system calls:

- **`fork()`** — Creates a new process by duplicating the current one.
- **`exec()`** — Replaces the current process image with a new program.
- **`wait()`** — Suspends execution until a child process finishes.

Together, these illustrate the core lifecycle of process creation and execution in Unix-like systems.

## How to Use

### Requirements

- **Operating System:** GNU/Linux  
- **Tools:**  
  - C compiler (`gcc` or `clang`)  
  - `make`

### Build and Run

Run the following command in your terminal:

```bash
make 
```

## License
MIT
