CC := gcc
CFLAGS := -g

SRC_DIR := . 
ASM_DIR := ./source
BIN_DIR := ./assemble
DASM_DIR := ./disassemble
RBIN_DIR := ./reassemble

SRCS := $(shell find $(SRC_DIR) -name '*.c')
ASMS := $(shell find $(ASM_DIR) -name '*.asm' \
		! -name "test_05.asm" \
		! -name "test_E*" \
		)

UNIT_TEST_TARGET := 16
UNIT_TEST_FILE := $(BIN_DIR)/test_$(UNIT_TEST_TARGET)
UNIT_TEST_DSM_OUTPUT := $(DASM_DIR)/test_$(UNIT_TEST_TARGET).asm
UNIT_TEST_BIN_OUTPUT := $(RBIN_DIR)/test_$(UNIT_TEST_TARGET)
UNIT_TEST_FLAG :=-stdout

# append .o suffix
OBJS := $(SRCS:%=%.o)
# replace $(ASM_DIR) with $(BIN_DIR), delete .asm
BINS := $(ASMS:$(ASM_DIR)/%.asm=$(BIN_DIR)/%)
DASMS := $(ASMS:$(ASM_DIR)/%.asm=$(DASM_DIR)/%.asm)
RBINS := $(ASMS:$(ASM_DIR)/%.asm=$(RBIN_DIR)/%)
DIFFS := $(ASMS:$(ASM_DIR)/%.asm=%)

format:
	clang-format -Werror --style="{BasedOnStyle: llvm, IndentWidth: 4}" -i *.c *.h

main: $(OBJS)
	$(CC) $^ -o $@
	rm *.o

assemble: clean-assemble $(BINS)

unit_test_all: $(RBINS)
	diff -bur $(BIN_DIR) $(RBIN_DIR)

unit_test_single:
	./main $(UNIT_TEST_FLAG) $(UNIT_TEST_FILE) > $(UNIT_TEST_DSM_OUTPUT)
	nasm $(UNIT_TEST_DSM_OUTPUT) -o $(UNIT_TEST_BIN_OUTPUT)
	diff $(UNIT_TEST_FILE) $(UNIT_TEST_BIN_OUTPUT)

# suffix .c.o is replaced by .c
$(OBJS): %.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

$(BINS): $(BIN_DIR)/% : $(ASM_DIR)/%.asm
	nasm $< -o $@

$(DASMS): $(DASM_DIR)/%.asm : $(BIN_DIR)/%
	./main $(UNIT_TEST_FLAG) $< > $@

$(RBINS): $(RBIN_DIR)/%: $(DASM_DIR)/%.asm
	nasm $< -o $@

$(DIFFS):
	diff $(BIN_DIR)/$< $(RBIN_DIR)/$<

clean:
	rm *.o
clean-assemble:
	rm ./assemble/* > /dev/null
clean-disassemble:
	rm ./disassemble/* > /dev/null
clean-reassemble:
	rm ./reassemble/* > /dev/null
