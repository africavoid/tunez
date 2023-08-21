CC=cc
BIN=mex
INCS=-I/usr/local/include
LIBS=-L/usr/local/lib
CFLAGS=-Wno-implicit-function-declaration

all:
	mkdir -p bin/
	$(CC) src/main.c src/audio.c $(INCS) $(LIBS) $(CFLAGS) -lSDL2 -lSDL2_mixer -lSDL2main -lncurses -lmenu -lm -o $(BIN) -g
	mv $(BIN) bin/
run:
	./bin/$(BIN)
