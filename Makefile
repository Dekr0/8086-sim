CC := gcc
CFLAGS := -g

SRC_DIR := . 
ASM_DIR := ./source
BIN_DIR := ./assemble
DSM_DIR := ./disassemble
RSM_DIR := ./reassemble

SRCS := $(shell find $(SRC_DIR) -name '*.c' ! -name "mystring.*" ! -name "unit_test.*")
ASMS := $(shell find $(ASM_DIR) -name '*.asm' ! -name "test_break.asm")

UNIT_TEST_TARGET := a
UNIT_TEST_FILE := $(BIN_DIR)/test_$(UNIT_TEST_TARGET)
UNIT_TEST_DSM_OUTPUT := $(DSM_DIR)/test_$(UNIT_TEST_TARGET).asm
UNIT_TEST_BIN_OUTPUT := $(RSM_DIR)/test_$(UNIT_TEST_TARGET)
UNIT_TEST_FLAG :=

# append .o suffix
OBJS := $(SRCS:%=%.o)
# replace $(ASM_DIR) with $(BIN_DIR), delete .asm
BINS := $(ASMS:$(ASM_DIR)/%.asm=$(BIN_DIR)/%)

main: $(OBJS)
	$(CC) $^ -o $@
	rm *.o

assemble: $(BINS)

# suffix .c.o is replaced by .c
$(OBJS): %.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

$(BINS): $(BIN_DIR)/% : $(ASM_DIR)/%.asm
	nasm $< -o $@

unit_test:
	./main $(UNIT_TEST_FLAG) $(UNIT_TEST_FILE) > $(UNIT_TEST_DSM_OUTPUT)
	nasm $(UNIT_TEST_DSM_OUTPUT) -o $(UNIT_TEST_BIN_OUTPUT)
	diff $(UNIT_TEST_FILE) $(UNIT_TEST_BIN_OUTPUT)

format:
	clang-format -Werror --style="{BasedOnStyle: llvm, IndentWidth: 4}" -i *.c *.h

clean:
	rm *.o
clean-assemble:
	rm ./assemble/*
clean-disassemble:
	rm ./disassemble/*
clean-reassemble:
	rm ./reassemble/*

