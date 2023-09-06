CC=cc
BIN=tunez
LIBS=-L/usr/local/lib
INCS=-I/usr/local/include

all:
	$(CC) $(INCS) $(LIBS) -g -O3 -lpthread -ldl -lm src/main.c -o $(BIN)
	mkdir -p bin
	mv $(BIN) bin/$(BIN)

run:
	./bin/$(BIN)
