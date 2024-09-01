CC := gcc
CFLAGS := -g -Wall

SRC_DIR := . 
ASM_DIR := ./source
BIN_DIR := ./assemble

SRCS := $(shell find $(SRC_DIR) -name '*.c' -maxdepth 1)
ASMS := $(shell find $(ASM_DIR) -name '*.asm' \
		! -name "test_05.asm" \
		! -name "test_E*" \
		)

# append .o suffix
OBJS := $(SRCS:%=%.o)
# replace $(ASM_DIR) with $(BIN_DIR), delete .asm
BINS := $(ASMS:$(ASM_DIR)/%.asm=$(BIN_DIR)/%)

format:
	clang-format -Werror --style="{BasedOnStyle: llvm, IndentWidth: 4}" -i *.c *.h

main: $(OBJS)
	$(CC) $^ -o $@
	rm *.o

assemble: clean-assemble $(BINS)

# suffix .c.o is replaced by .c
$(OBJS): %.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

$(BINS): $(BIN_DIR)/% : $(ASM_DIR)/%.asm
	nasm $< -o $@

clean:
	rm *.o
clean-assemble:
	rm ./assemble/* > /dev/null
clean-disassemble:
	rm ./disassemble/* > /dev/null
clean-reassemble:
	rm ./reassemble/* > /dev/null
