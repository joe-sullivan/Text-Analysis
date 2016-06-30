#ifdef DEBUG
# define D if(1)
#else
# define D if(0)
#endif

#define ALPHABET_SIZE 27
#define MAX_WORD_SIZE 128
#define NUMBER_OF_SOURCES 20
#define NUMBER_OF_COMMON 5

typedef struct _node {
	struct _node* children[ALPHABET_SIZE];
	bool is_leaf[NUMBER_OF_SOURCES];
} Node;

typedef struct _string {
	int length;
	char data[MAX_WORD_SIZE];
} String;

Node* get_node();
void insert(Node* root, String* string, int source_id);
bool search(Node* root, const char* key, int source_id);
bool longest(Node* node, String best[]);
void cleanup(Node* node);
