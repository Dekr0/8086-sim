## Introduction

- A command line based 8086 Emulator that attempts to emulate every aspect of 
the 8086 CPU.
- The emulator comes with the following features:
    - dissembles binary assembly code into the original readable assembly code 
    (There are a few remarks)
    - simulates instructions execution and side effects on the virtual 8086 CPU 
    and virtual 8086 memory
    - More to comes in future ??? (Visualization)

## State of This Projects

- The accuracy of this emulator is not guaranteed since the amount of instructions 
are already quiet large despite 8086 is very simple and small instructions set 
compare to others, and there are numerous edge cases for each instruction when 
some specific fields are set.
    - Most of the testing are done by compared results from other existing 
    8086 emulators.
- Not all instructions are able to be recognized, decoded, and simulated. Read 
[here](url) to see what instructions can be recognized and decoded, and [here](url) 
to see what instructions can be simulated.
- The emulator does not behaves exactly like a physical CPU in terms of decoding 
and executing instructions. Read [here](url)

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

## Design

### Decoding Flow Control

1. Create 1MB bytes array in the heap to present the 8086 memory.
2. Load the entire binary assembly file into the 8086 memory, and mark down the 
where the source assembly code ends in the 8086 memory (Let's called the source 
boundary)
3. Start decoding process from address 0 in the 8086 memory as the process walk 
through the 8086 memory until reaching the source boundary.
4. Store each decoded instruction into a vector

