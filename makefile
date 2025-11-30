CC := gcc
CFLAGS := -std=c2x -Wall -Wextra -g
TARGET := customcpu
SRCS := main.c
OBJS := $(SRCS:.c=.o)

ifeq ($(OS),Windows_NT)
  EXE := .exe
endif

.PHONY: all clean run

all: $(TARGET)$(EXE)

$(TARGET)$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: all
	./$(TARGET)$(EXE)

ifeq ($(OS),Windows_NT)
RM := cmd /C del /Q
else
RM := rm -f
endif

clean:
	$(RM) $(OBJS) $(TARGET)$(EXE)