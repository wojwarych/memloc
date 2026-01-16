CC=clang
CFLAGS=-std=c99 -Wall -Werror -Wextra
OBJ=./objs
SRC=./src
BINDIR=./bin
BIN=libmemloc.so

debug_bin: $(OBJ)/memloc.o
	$(CC) $(CFLAGS) -o $(BINDIR)/debug $<

debug: CFLAGS += -DDEBUG -g
debug: $(SRC)/memloc.c
	$(CC) -c $< -o $(OBJ)/memloc.o $(CFLAGS) -fPIC -D_DEFAULT_SOURCE

$(BIN): $(OBJ)/memloc.o
	$(CC) -shared $(CFLAGS) -o $(BINDIR)/$@ $<

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -c $< -o $@ $(CFLAGS) -fPIC -D_DEFAULT_SOURCE

clean:
	rm -r $(OBJ)/* $(BINDIR)/*
