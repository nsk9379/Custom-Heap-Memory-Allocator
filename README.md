# Custom Heap Memory Allocator

This project presents a **heap memory allocator** in C that mimics basic functionalities of dynamic memory allocation (`malloc` and `free`) within a fixed-size heap. It is designed for educational and demonstration purposes, illustrating how low-level memory management works under the hood.

## Features

- **Statically Allocated Heap:** 900-byte array that serves as a custom heap.
- **Custom Allocation and Free:** Functions to allocate and release blocks within the heap, inspired by `malloc`/`free`.
- **Block Splitting:** Large free blocks are split on allocation to reduce internal fragmentation.
- **Block Merging:** Adjacent free blocks are merged during release to reduce external fragmentation.
- **Metadata Per Block:** Each block contains metadata (size, status, next pointer).
- **Menu-driven Interface:** Simple command-line interface to interact with the allocator.
- **Heap Visualization:** Displays the complete memory layout, including free and allocated blocks.

## Code Overview

- `main.c` – Main source file. All logic is contained here.

### Key Components

- **Heap Array:** `char Heap[HeapSize]` is the static memory space.
- **MetaBlock Structure:** Stores the size, status ('a' = allocated, 'f' = free), and next block pointer.
- **Functions:**
  - `Initialize()` - Prepares the heap and initial metadata.
  - `Allocate(size_t)` - Allocates a block, splitting when possible.
  - `Free(void *)` - Frees a block, with error checking.
  - `Merge()` - Merges consecutive free blocks post-free.
  - `DisplayHeap()` - Shows current heap/block layout.
  - `DisplayMenu()` - Prints menu options.
  - `main()` - Program entry point with interactive menu.

## How to Build & Run

1. **Compile:**
- gcc -o heap_allocator main.c
2. **Run:**
- ./heap_allocator

3. **Usage:**
- Choose options via menu to allocate, free, or view heap blocks.

## Example Interaction
===== HEAP ALLOCATOR MENU =====

Allocate memory

Free memory

Display heap status

Exit
Enter your choice (1-4): 1
Enter size to allocate (in bytes): 100
Successfully allocated 100 bytes at address 0x55d3...
Allocated pointer #0

## Educational Value

- Understand metadata overhead, memory fragmentation (internal/external), basic memory block management.
- Learn how allocation and deallocation work conceptually without relying on the C standard library for memory management.

## Limitations

- Fixed heap size and allocation table.
- No support for `realloc`, multithreading, or advanced allocation policies.
- No dynamic expansion; heap overflow cannot be handled beyond defined capacity.


