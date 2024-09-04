## Introduction

- A command line based 8086 Emulator that attempts to emulate every aspect of 
the 8086 CPU.
- The emulator comes with the following features:
    - dissembles binary assembly code into the original readable assembly code 
    (There are a few remarks)
    - estimate CPU clock cycle under ideal scenario (assume BIU is always fill 
    up the instruction queue and EU does not fast execute)
    - simulates instructions execution and side effects on the virtual 8086 CPU 
    and virtual 8086 memory
    - TUI based simulation

## State of This Project

- The accuracy of this dissembler and simulator is not guaranteed. 8086 has a very
simple and small instructions set compare to others. However, there are edge cases
and quirks for some instructions when some specific fields are set. It's no surprise
I missed a handful of them when I implemented it against the 8086 family manual.
- Most of the testing are done by compared results from other existing 8086 emulators 
or manually deriving the result against the 8086 family manual.
- Not all instructions can be recognized, decoded, and simulated. Read 
[here](https://github.com/Dekr0/8086-sim/blob/bd1bea267dca4d591cdb696e3ccf6d18a913efcc/instruction.h#L28) to see what
instructions can be recognized and decoded, and [here](https://github.com/Dekr0/8086-sim/blob/bd1bea267dca4d591cdb696e3ccf6d18a913efcc/sim.c#L63)
to see what instructions can be simulated.
- The execution simulator does not behaves exactly like a physical CPU in terms of
instruction fetching, decoding, and execution.
- However, the hardware simulation is something I want to implement and capture in the future,
especially given that dealing with circuits and digital logic are part of what I did during
 my degree. A prime example of this is hardware simulation in Xilinx Vivado and Cadence.
- The source code for TUI was badly written. I implemented it as I learn about 
how to make TUI application with ANSI escape sequence and `Notcures`. UI performance 
will degrade under high rendering rate along with drastic screen change.

## Usage

### Prerequisite 

- `gcc` and `Makefile`
- `Notcurses`

### Compiling

- Run `Make main` to compile the binary executable

### Execution

- Run `./main [binary_assembly_file] [output_disassembly_file]` to output the 
dissembled assembly code to `[output_disassembly_file]`
- Here's a list of flags that are available
    - `-e`: perform instructions simulation upon dissembling, and output 
    side effect of each instruction as well as the final result
    - `-i`: run the emulator in the interactive mode
        - `-stdout` need to be disabled
    - `--auto-run [time between rendering in us]`: auto execution with timeout between each render 
        - must come after `-i` flag
    - `-stdout`: display all the results in the `stdout`
    - `-mdump [memory_dump_file]`: dump 1MB virtual memory of 8086 into 
    `[memory_dump_file]`
        - `[memory_dump_file]` must follow right flag `[-mdump`]`
- During interactive mode,
    - press `q` to exit the TUI
    - press `j` or `k` to navigate the source code previewer
    - press `h` or `l` to navigate the memory previewer
    - press `n` to execute the next instruction

## Remark

- There will be some amount of loss in human readable information when dissembles 
binary assembly code such as jump labels, negative values in the operand. 
- This does not affect the reassemble since 8086 CPU does not care about the 
meaning of jump labels and negative values in the operand. 
- The 8086 CPU simply view them as a series of binary number. Thus, the 
reassemble binary assembly code will be identical to the original one.
- The interactive mode is untested, and the TUI is made in a rush (TUI are drawn 
in a very imperative way - manually manipulating the cursor and write output via 
ANSI escape sequence). There will be a high chance that running interactive 
mode will result crashes.

## Design

- The current state of this project is plagued by early abstraction. What I'm 
planning to do (should be do in the first place) is to write out all possible 
branch case with minimal amount of abstraction and code reuse in the first place
for each process (disassemble, simulation, clock estimation). This is for seeing 
the whole picture (piggy backing). 
- Then, attempt to come up with a reasonable refactor and abstraction design. 
Even if the attempt is failure, what I have is a functional code base that can 
be read very imperatively and sequentially without wrapping our head around and 
jump between functions and files. The constant mental context switching when hopping 
functions and files introduce most of the bugs I wrote during this project.

### Decoding Flow Control

1. Create 1MB bytes array in the heap to present the 8086 memory
2. Load the entire binary assembly file into the 8086 memory, and mark down
where the source assembly code ends in the 8086 memory (Let's called the source 
boundary)
3. Start decoding process from address 0 in the 8086 memory as the process walk 
through the 8086 memory until reaching the source boundary
4. Store each decoded instruction into a vector
5. Iterate each instruction and write the result to the provided file stream
