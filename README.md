# mk42 — Recursive Makefile Generator for C Projects

## Overview

`mk42` is a lightweight tool designed to automatically generate a `Makefile` for your C projects. It recursively scans your project directory for all `.c` and `.h` files, generates appropriate `SRCS`, `OBJS`, and `HEADER` variables, and creates rules to compile your project into a single executable.

This tool helps simplify compiling projects that span multiple directories, saving you time and avoiding tedious manual Makefile maintenance.

---

## Features

- Recursively scans all subdirectories for `.c` and `.h` files.
- Automatically creates Makefile variables for sources (`SRCS`), objects (`OBJS`), and headers (`HEADER`).
- Generates a Makefile with standard rules: `all`, `clean`, `fclean`, and `re`.
- Compatible with projects organized with nested folders.
- Simple to install and use with a setup script.
- Works seamlessly on Linux and macOS environments.
- Supports Bash and Zsh shells, including integration with Oh My Zsh.

---

## Installation

### Requirements

- C compiler (e.g., `gcc` or `cc`) installed on your system.
- Bash or Zsh shell.

### Steps

1. Make sure the `setup.sh` script and `mk42.c` source file are in the same directory.

2. Make the setup script executable (if not already):

   ```sh
   chmod +x setup.sh
    ```
The script will:

    Compile mk42.c into the mk42 executable.

    Move the executable to ~/bin/mk42.

    Add ~/bin to your PATH environment variable in your shell configuration (.bashrc or .zshrc).

    If you use Zsh with Oh My Zsh, it will update .zshenv as well.

    Reload your shell configuration to apply the changes immediately.

Verify the installation by running:

mk42 --help

    Note: The current version does not have a help option; this is a placeholder for future features.

Usage

Navigate to the root directory of your C project:

cd /path/to/your/project

Run mk42 with the desired executable name:

mk42 myapp

This will generate a Makefile that:

    Recursively detects all .c and .h files.

    Creates SRCS, OBJS, and HEADER variables listing all source files, object files, and headers.

    Defines standard build rules to compile and link your project.

Compile your project using:

make

Additional useful make commands:

    make clean — removes all object (.o) files.

    make fclean — removes object files and the compiled executable.

    make re — cleans and rebuilds the entire project.

How It Works

    mk42 scans the current directory and all subdirectories recursively.

    For every .c file found, it adds its relative path to SRCS.

    For every .h file found, it adds its relative path to HEADER.

    The generated Makefile:

        Defines SRCS with all .c files.

        Defines OBJS by replacing .c extensions in SRCS with .o.

        Defines HEADER with all .h files.

        Makes the executable depend on both OBJS and HEADER to ensure correct recompilation.

        Uses cc with flags -Wall -Wextra -Werror for compilation.

Development & Contribution

Contributions are welcome! Some ideas to improve mk42:

    Add command-line options (e.g., specify directories or compiler flags).

    Support other languages or build systems.

    Add verbose/debug modes for troubleshooting.

    Implement a help menu for user guidance.

    Enhance error handling and reporting.

License

This project is provided "as-is" without warranty. Use and modify it freely.
