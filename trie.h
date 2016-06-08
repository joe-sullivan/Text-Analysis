#include <stdbool.h>
#include <stdio.h>

#ifdef DEBUG
# define D if(1)
#else
# define D if(0)
#endif

#define ALPHABET_SIZE (26)
#define NUMBER_OF_SOURCES (20)

struct Node {
	struct Node *children[ALPHABET_SIZE];
	bool is_leaf[NUMBER_OF_SOURCES];
};

struct Node *get_node(void);
void insert(struct Node *root, const char *key, int source_id);
bool search(struct Node *root, const char *key, int source_id);
char* longest(struct Node *node, char* word, int idx, char* best);
