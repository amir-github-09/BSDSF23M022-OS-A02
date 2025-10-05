

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
SRC = src/ls-v1.0.0.c
BIN = bin/ls

all: $(BIN)

$(BIN): $(SRC)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(BIN) *.o

.PHONY: all clean

