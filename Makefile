CC=gcc
CFLAGS=-c -std=c99 -O3
LIBRARIES=-lpthread

.PHONY: all
all: solution

solution: main.o trie.o
	$(CC) main.o trie.o -o solution $(LIBRARIES)

main.o: main.c
	$(CC) $(CFLAGS) main.c

trie.o: trie.h trie.c
	$(CC) $(CFLAGS) trie.c

.PHONY: rebuild
rebuild: cleanall solution clean

.PHONY: debug
debug: CFLAGS+=-DDEBUG -g
debug: cleanall solution clean

.PHONY: profile
profile: CC+= -pg
profile: cleanall solution clean

.PHONY: clean
clean:
	rm -f *.o

.PHONY: cleanall
cleanall:
	rm -f *.o solution
