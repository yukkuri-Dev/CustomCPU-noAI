CC := gcc
CFLAGS := -std=c2x -Wall -Wextra -g
TARGET := customcpu
SRCS := main-switch.c vBus/vbus_root.c $(wildcard vBus/hardware/*.c)
OBJS := $(SRCS:.c=.o) 

ifeq ($(OS),Windows_NT)
  EXE := .exe
endif

.PHONY: all clean run

all: $(TARGET)$(EXE)

$(TARGET)$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: all
	./$(TARGET)$(EXE)

LDLIBS := -lgdi32

ifeq ($(OS),Windows_NT)
RM := cmd /C del /Q
QOBJS := $(foreach f,$(OBJS),\"$(f)\")
QTARGET := \"$(TARGET)$(EXE)\"
else
RM := rm -f
QOBJS := $(OBJS)
QTARGET := $(TARGET)$(EXE)
endif

clean:
	$(RM) $(QOBJS) $(QTARGET)