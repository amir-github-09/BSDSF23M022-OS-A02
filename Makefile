

CC = gcc
CFLAGS = -Wall -g
SRC = src/ls-v1.5.0.c
BIN = bin/ls

all: $(BIN)

$(BIN): $(SRC)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(BIN) *.o

.PHONY: all clean

