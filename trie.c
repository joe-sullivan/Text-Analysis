#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "trie.h"

#define CHAR_TO_INDEX(c) ((c=='\'') ? 26 : (int)c - (int)'a')
#define INDEX_TO_CHAR(i) ((i==26) ? '\'' : (char)i + 'a')

// Returns new empty trie node
Node* get_node() {
	return (Node*)calloc(1, sizeof(Node));
}

void insert(Node* root, String* string, int source_id) {
	int level;
	int index;

	Node *crawl = root;

	for (level = 0; level < string->length; level++) {
		index = CHAR_TO_INDEX(string->data[level]);
		if (!crawl->children[index])
			crawl->children[index] = get_node();
		crawl = crawl->children[index];
	}

	// mark last node as leaf
	crawl->is_leaf[source_id] = true;
}

// Returns true if key presents in trie, else false
bool search(Node* root, const char* key, int source_id) {
	int level;
	int length = strlen(key);
	int index;
	Node* crawl = root;

	for (level = 0; level < length; level++) {
		index = CHAR_TO_INDEX(key[level]);

		if (!crawl->children[index])
			return false;

		crawl = crawl->children[index];
	}

	return (crawl != NULL && crawl->is_leaf[source_id]);
}

int _longest(Node* node, char* word, int idx, String best[],int max_idx) {
	// set is_leaf
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
		if (best[i].length < best[smallest_id].length)
			smallest_id = i;
	}
	if (smallest_id > max_idx) max_idx = smallest_id;

	// replace smallest with current word
	if (is_leaf && (idx >= best[smallest_id].length)) {
		D printf("[%s][%s]\n", best[smallest_id].data, word);
		strcpy(best[smallest_id].data, word);
		best[smallest_id].length = idx;
	}

	// iterate over children
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		// store character in
		word[idx] = INDEX_TO_CHAR(i);

		Node* child = node->children[i];
		if (child) // continue to search branch
			max_idx = _longest(child, word, idx+1, best, max_idx);

		// clear current character for next iteration
		word[idx] = 0;
	}
	return max_idx;
}

bool longest(Node* node, String best[]) {
	char buffer[MAX_WORD_SIZE];
	int size = _longest(node, buffer, 0, best, 0);
	return size == (NUMBER_OF_COMMON-1);
}

void cleanup(Node* node) {
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		Node* child = node->children[i];
		if (child)
			cleanup(child);
	}
	free(node);
}
