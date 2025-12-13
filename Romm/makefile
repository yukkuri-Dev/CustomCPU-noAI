ASM := ./bin/assmbler.exe
ASMSRC := test.asm
OUTBIN := test.bin

.PHONY: all clean run

all: $(OUTBIN)

$(OUTBIN): $(ASMSRC)
	@echo "Assembling $< -> $@ using $(ASM)"
	$(ASM) $(ASMSRC)

run: all
	@echo "Running $(OUTBIN)"
	@./$(OUTBIN) || true

ifeq ($(OS),Windows_NT)
RM := cmd /C del /Q
else
RM := rm -f
endif

clean:
	$(RM) $(OUTBIN)