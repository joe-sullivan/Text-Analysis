CC=clang
CFLAGS=-c

all: solution clean

debug: CFLAGS += -DDEBUG
debug: solution

solution: main.o trie.o
	$(CC) main.o trie.o -o solution

main.o: main.c
	$(CC) $(CFLAGS) main.c

trie.o: trie.h trie.c
	$(CC) $(CFLAGS) trie.c

clean:
	rm -f *.o

cleanall: clean
	rm -f solution
