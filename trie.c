#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "trie.h"

#define CHAR_TO_INDEX(c) ((int)c - (int)'a')
#define INDEX_TO_CHAR(i) ((char)i + 'a')

// Returns new trie node (initialized to NULLs)
struct Node *get_node(void) {
	struct Node *pNode = NULL;
	pNode = (struct Node*)malloc(sizeof(struct Node));
	if (pNode) {
		for (int i = 0; i < NUMBER_OF_SOURCES; i++)
			pNode->is_leaf[i] = false;
		for (int i = 0; i < ALPHABET_SIZE; i++)
			pNode->children[i] = NULL;
	}
	return pNode;
}

void insert(struct Node *root, const char *key, int source_id) {
	int level;
	int length = strlen(key);
	int index;

	struct Node *pCrawl = root;

	for (level = 0; level < length; level++) {
		index = CHAR_TO_INDEX(key[level]);
		if (!pCrawl->children[index])
			pCrawl->children[index] = get_node();
		pCrawl = pCrawl->children[index];
	}

	// mark last node as leaf
	pCrawl->is_leaf[source_id] = true;
}

// Returns true if key presents in trie, else false
bool search(struct Node *root, const char *key, int source_id) {
	int level;
	int length = strlen(key);
	int index;
	struct Node *pCrawl = root;

	for (level = 0; level < length; level++) {
		index = CHAR_TO_INDEX(key[level]);

		if (!pCrawl->children[index])
			return false;

		pCrawl = pCrawl->children[index];
	}

	return (pCrawl != NULL && pCrawl->is_leaf[source_id]);
}

void longest(struct Node *node, char* word, int idx, char* best[]) {
	// if word is new longest then copy to _best_ buffer
	bool is_leaf = true;
	for (int i = 0; i < NUMBER_OF_SOURCES; i++) {
		if (!node->is_leaf[i]) {
			is_leaf = false;
			break;
		}
	}

	// find smallest candidate
	int smallest_id = 0;
	for (int i = 0; i < NUMBER_OF_COMMON; i++) {
		if (strlen(best[i]) < strlen(best[smallest_id]))
			smallest_id = i;
	}

	// replace smallest with current word
	if (is_leaf && (idx >= strlen(best[smallest_id]))) {
		D printf("[%s][%s]\n", best[smallest_id], word);
		strcpy(best[smallest_id], word);
	}

	// iterate over children
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		// store character in
		word[idx] = INDEX_TO_CHAR(i);

		struct Node *child = node->children[i];
		if (child) // continue to search branch
			longest(child, word, idx+1, best);

		// clear current character for next iteration
		word[idx] = 0;
	}
}

void cleanup(struct Node *node) {
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		struct Node *child = node->children[i];
		if (child)
			cleanup(child);
	}
	free(node);
}
