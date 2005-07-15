
COMPILE=gcc -I/usr/include/SDL/ -lpthread -lSDL -o

all:	1 2 3 4

1: 1.c
	${COMPILE} 1 1.c
2: 2.c
	${COMPILE} 2 2.c
3: 3.c 
	${COMPILE} 3 3.c
4: 4.c
	${COMPILE} 4 4.c

clean:
	rm 1 2 3 4 2>/dev/null || exit 0
