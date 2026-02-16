# EasyMIPS — Educational MIPS32 Assembler & Virtual Machine

**EasyMIPS** is a pedagogically oriented, modular MIPS32 assembler and emulator written in C++17.
It implements the complete compilation pipeline — from lexical analysis to execution — in a way that is transparent, extensible, and suitable for academic study.

EasyMIPS is designed to support:

* 📘 Computer Architecture courses
* 📗 Assembly Language instruction
* 📙 Compiler Construction classes
* 🔬 Research in virtual machines and language tooling
* 🛠 Systems programming experimentation

The project emphasizes clarity of implementation over black-box abstraction, making it ideal for educational environments.

---

# Why EasyMIPS?

Many teaching tools abstract away internal mechanisms. EasyMIPS does the opposite.

It exposes:

* The lexical specification
* The grammar and AST structure
* The assembly process
* The VM execution model

Students can trace every stage of the toolchain and inspect internal state interactively.

---

# Architecture Overview

EasyMIPS follows a classical compiler toolchain architecture:

```
Source (.asm)
    ↓
Lexer (re2c)
    ↓
Parser (treecc)
    ↓
Abstract Syntax Tree (AST)
    ↓
Assembler
    ↓
VM Instructions
    ↓
Virtual Machine Execution
```

Each phase is modular and can be studied, modified, or replaced independently.

This design supports:

* Incremental compiler projects
* Extension assignments
* Research experiments on VM behavior
* Instruction-set modifications

---

# Core Features

## Complete MIPS32 Pipeline

* Lexer generated with `re2c`
* Parser and AST generated with `treecc`
* Assembler with symbol resolution
* Virtual Machine with register and memory model

## Interactive REPL

Powered by `replxx`, providing:

* Context-aware autocompletion (instructions, registers, debugger commands)
* Word-based navigation and editing
* Persistent command history

This makes it suitable for live classroom demonstrations.

## Integrated Debugger

The interactive debugger allows:

* Register inspection
* Memory inspection
* Register modification
* VM reset
* Controlled execution (when enabled)

Example:

```
#show $t0
#show byte 0x1000($sp) hex
#set $t1 = 0xFF
#reset
```

## Extensible Syscall Interface

EasyMIPS supports a plugin-based syscall system via the **Plugin Development Kit (PDK)**.

This enables:

* Custom runtime extensions
* OS abstraction experiments
* Systems course projects

## Cross-Platform

Tested on:

* Linux
* Windows (MinGW and MSVC)

---

# Requirements

* CMake ≥ 3.14
* C++17 compatible compiler (GCC, Clang, MSVC)
* `re2c` (lexer generator)
* `treecc` (AST generator)

Both `re2c` and `treecc` must be available in your system PATH.

---

# Build Instructions

## Linux

```bash
git clone https://github.com/ideras/EasyMIPS.git
cd EasyMIPS
mkdir build
cd build
cmake ..
make
```

## Windows (MSVC)

```bash
cmake -S . -B build
cmake --build build --config Release
```

---

# Usage

## Run an Assembly Program

```bash
./build/EasyMIPS --run asm/examples/add.asm
```

## Start Interactive Mode

```bash
./build/EasyMIPS --interactive
```

---

# Project Structure

```
src/              C++ implementation files (.cpp, .re)
include/          Headers and AST definitions (.h, .tc)
asm/examples/     Example MIPS32 programs
tests/            Unit and integration tests
pdk/              Plugin Development Kit
```

---

# Educational Applications

EasyMIPS can be used for:

### In Computer Architecture

* Instruction decoding
* Register file simulation
* Memory layout exploration
* Pipeline modeling extensions

### In Compiler Courses

* Lexer and parser generation
* AST construction
* Code generation experiments
* Symbol resolution study

### In Systems Programming

* Virtual machine design
* Plugin architecture
* Runtime syscall abstraction

---

# For Contributors

Contributions are welcome in areas such as:

* Additional MIPS32 instruction support
* Improved debugging capabilities
* Step-by-step execution features
* Enhanced memory visualization
* Performance optimizations
* Documentation improvements
* Testing infrastructure expansion

If contributing, please ensure:

* C++17 compliance
* Clear modular design
* Unit tests where applicable
* Consistent code style

---

# Research Opportunities

EasyMIPS provides a compact but realistic platform for:

* Instruction set experimentation
* VM instrumentation research
* Runtime verification experiments
* Teaching methodology studies
* Lightweight compiler architecture analysis

Its clean separation of stages makes it suitable for academic papers or student theses.

---

# Design Philosophy

* Transparency over abstraction
* Modularity over monolithic design
* Educational clarity over excessive optimization
* Extensibility without complexity

---

# License

MIT License
