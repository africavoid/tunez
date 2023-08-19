CC=cc
BIN=mex
INCS=-I/usr/local/include
LIBS=-L/usr/local/lib
CFLAGS=-Wno-implicit-function-declaration

all:
	$(CC) src/main.c src/audio.c $(INCS) $(LIBS) $(CFLAGS) -lSDL2 -lncurses -o $(BIN) -g
run:
	./$(BIN)
