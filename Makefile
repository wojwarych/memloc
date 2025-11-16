CC=clang
CFLAGS=-std=c99 -Wall -Werror -Wextra
OBJ=objs
SRC=src
BINDIR=bin
BIN=libmemloc.so

all: $(BIN)

$(BIN): $(OBJ)/memloc.o
	$(CC) -shared $(CFLAGS) -o $(BINDIR)/$@ $^ -g
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -c -o $@ $^ $(CFLAGS) -fPIC -D_DEFAULT_SOURCE
clean:
	rm -r $(OBJ)/* $(BINDIR)/*
