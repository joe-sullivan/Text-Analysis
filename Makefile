CC=gcc
CFLAGS=-c

all: solution clean

.PHONY: debug
debug: CFLAGS+=-DDEBUG
debug: solution

.PHONY: profile
profile: CC+= -pg
profile: solution

solution: main.o trie.o
	$(CC) main.o trie.o -o solution

main.o: main.c
	$(CC) $(CFLAGS) main.c

trie.o: trie.h trie.c
	$(CC) $(CFLAGS) trie.c

.PHONY: clean
clean:
	rm -f *.o
cleanall: clean
	rm -f solution
