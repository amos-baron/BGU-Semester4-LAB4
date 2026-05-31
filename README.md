# Lab 4: ELF Parsing and Binary Patching

## Overview
This repository contains the implementation of a custom binary editor (`hexeditplus`) and documents the methodology for analyzing, modifying, and patching 32-bit ELF (Executable and Linkable Format) binaries. The objective is to understand how compiled programs are structured on disk, how they are loaded into memory, and how to alter their execution flow without access to the original source code.

---

## Technical Background

### 1. The ELF File Format
ELF is the standard file format for executables, object code, and shared libraries in Unix-like operating systems. An ELF file is divided into specific structural components:
* **ELF Header:** Located at the very beginning of the file (offset 0). It contains global information such as the architecture (32-bit vs 64-bit), endianness, and the Entry Point (the memory address where execution begins).
* **Section Header Table:** A directory of the various sections within the file.
* **Sections:**
    * `.text`: Contains the executable machine code instructions.
    * `.data`: Contains initialized global and static variables.
    * `.rodata`: Contains read-only data (e.g., string literals).
    * `.symtab`: The symbol table, mapping function and variable names to their locations.

### 2. Virtual Memory vs. File Offset
This is the core concept required for binary patching. 
* **Virtual Address (VA):** The memory location where a piece of data or code will reside in RAM *while the program is running*. Debuggers and symbol tables (`readelf -s`) report these addresses.
* **File Offset:** The physical byte location of that data or code *on the hard drive* within the raw file. Hex editors operate using file offsets.

Because the operating system maps sections of the file into specific virtual memory regions, the Virtual Address does not equal the File Offset.

### 3. The Translation Formula
To patch a binary, you must convert a function's Virtual Address into a File Offset to know which physical bytes to modify. The relative distance from the start of a section to a function remains constant whether on disk or in memory.

**The Formula:**
`File_Offset = Function_VA - Section_VA + Section_Offset`

* `Function_VA`: Found in the Symbol Table.
* `Section_VA`: Found in the Section Header Table.
* `Section_Offset`: Found in the Section Header Table.

### 4. Binary Patching and Opcodes
Binary patching involves directly overwriting machine code instructions (opcodes) in the compiled executable. 
* **`C3` (RET):** The x86 opcode for "Return". Placing this at the start of a function forces it to terminate immediately and return to the caller, effectively neutralizing the function.
* **`90` (NOP):** The x86 opcode for "No Operation". The CPU reads this and does nothing, moving to the next instruction. This is used to overwrite unwanted instructions without altering the overall byte alignment.

---

## Tools Used
* **`readelf`:** A standard Linux command-line utility used to extract structural information from ELF files (e.g., `readelf -h` for headers, `-S` for sections, `-s` for symbols).
* **`hexeditplus`:** A custom C program developed in this lab to read, display, and modify raw hexadecimal data in binary files using calculated offsets.
* **`gcc`:** Used for compiling patches. Requires specific flags (`-m32 -fno-pie -fno-stack-protector`) to ensure the generated machine code is strictly 32-bit and uses absolute addressing compatible with older or simpler binaries.

---

## Lab Tasks Summary

### Task 1: `hexeditplus` Implementation
A robust hexadecimal editor capable of reading units of 1, 2, or 4 bytes from an executable file into a local memory buffer, modifying that buffer, and writing the changes back to the exact physical offset on the disk without corrupting the file structure.

### Task 2: Hijacking the Entry Point (`deep_thought`)
* **Objective:** Change the start location of an executable.
* **Method:** Analyzed the symbol table to find the Virtual Address of the `_start` function. Used `hexeditplus` to overwrite the `e_entry` field in the ELF Header (located at file offset `0x18`) with this correct address, fixing the broken execution flow.

### Task 3: Neutralizing a Function (`offensive`)
* **Objective:** Disable a specific function (`main`) so the program exits immediately.
* **Method:** Calculated the physical file offset of the `main` function using the translation formula. Used `hexeditplus` to overwrite the first byte of `main` with the `RET` opcode (`C3`), forcing an immediate return.

### Task 4: Fixing a Bugged Binary (`ntsc`)
* **Objective:** Repair a program that throws a Segmentation Fault due to flawed logic in its `count_digits` function.
* **Method:**
    1. Wrote a corrected, self-contained C function.
    2. Compiled it into a 32-bit ELF object without position-independent code (PIE) or stack protectors.
    3. Calculated the file offset and size of the corrected function in the new binary, and the offset of the flawed function in the target binary.
    4. Used `hexeditplus` to extract the raw machine code of the correct function and overwrite the flawed function in the target binary.

---

## Compilation Instructions
To build the tools developed in this lab, execute the `make` command using the provided `makefile`.

```bash
make all
