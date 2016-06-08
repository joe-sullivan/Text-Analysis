#include <stdbool.h>
#include <stdio.h>

#ifdef DEBUG
# define D if(1)
#else
# define D if(0)
#endif

#define ALPHABET_SIZE (26)
#define MAX_WORD_SIZE (256)
#define NUMBER_OF_SOURCES (20)
#define NUMBER_OF_COMMON (5)

struct Node {
	struct Node *children[ALPHABET_SIZE];
	bool is_leaf[NUMBER_OF_SOURCES];
};

struct Node *get_node(void);
void insert(struct Node *root, const char *key, int source_id);
bool search(struct Node *root, const char *key, int source_id);
void longest(struct Node *node, char* word, int idx, char* best[]);
void cleanup(struct Node *node);
