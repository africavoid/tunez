CC=cc
INCS=-I/usr/local/include/
LIBS=-L/usr/local/lib/
CFLAGS=-Wno-implicit-function-declaration

all:
	${CC} src/wavplay/wavplay.c ${CFLAGS} ${LIBS} ${INCS} ${CFLAGS} -lasound -o wavplay -g
	${CC} src/tunez/main.c src/tunez/audio.c ${LIBS} ${INCS} ${CFlAGS} -lmenu -lSDL2 -lncurses -o tunez -g
	mv wavplay bin/
	mv tunez bin/
