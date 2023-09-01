CC=c++
BIN=tunez
INCs=-I/usr/local/include
LIBS=-L/usr/local/lib

all:
	$(CC) src/main.cpp -std=c++11 -I/usr/local/include -L/usr/local/lib -lSDL2 -lSDL2_mixer -lSDL2main -lm -g -o $(BIN)
	mkdir -p bin
	mv $(BIN) bin/$(BIN)
run:
	./bin/$(BIN) test/i.wav
