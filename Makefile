CC=cc
INCS=-I/usr/local/include/
LIBS=-L/usr/local/lib/
CFLAGS=-Wno-implicit-function-declaration

all:
	${CC} src/wavplay/wavplay.c ${CFLAGS} ${LIBS} ${INCS} ${CFLAGS} -lasound -o wavplay -g
	${CC} src/tunez/tunez.c ${LIBS} ${INCS} ${CFlAGS} -lmenu -lncurses -o tunez -g
	mv wavplay bin/
	mv tunez bin/
