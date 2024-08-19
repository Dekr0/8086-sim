## Introduction

- A command line based 8086 Emulator that attempts to emulate every aspect of 
the 8086 CPU.
- The emulator comes with the following features:
    - dissembles binary assembly code into the original readable assembly code 
    (There are a few remarks)
    - simulates instructions execution and side effects on the virtual 8086 CPU 
    and virtual 8086 memory
    - More to comes in future ??? (Visualization)

## Usage

### Prerequisite 

- `gcc` and `Makefile`

### Compiling

- Run `Make main` to compile the binary executable

### Execution

- Run `./main [binary_assembly_file]` to output the dissembled assembly code.
- Here's a list of flags that are available
    - `--exec`: perform instructions simulation upon dissembling, and output 
    side effect of each instruction as well as the final result

## Remark

- There will be some amount of loss in human readable information when dissembles 
binary assembly code such as jump labels, negative values in the operand. 
- This does not affect the reassemble since 8086 CPU does not care about the 
meaning of jump labels and negative values in the operand. 
- The 8086 CPU simply view them as a series of binary number. Thus, the 
reassemble binary assembly code will be identical to the original one.
