#include <ctype.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"

bool is_end(char c) {
	switch(c) {
		case ' ':
		case '\n':
		case '\t':
		case '.':
		case '!':
		case '?':
		case ',':
		case ';':
		case '/':
		case '|':
		case '(':
		case ')':
		case '[':
		case ']':
		case '-':
			return true;
		default:
			return false;
	}
}

void load_file(char* path, struct Node* trie, int num) {
	D printf("Loading file: %s\n", path);
	int c;
	char word[MAX_WORD_SIZE];

	FILE *file;
	file = fopen(path, "r");
	if (file) {
		int i = 0;
		while ((c = getc(file)) != EOF) {
			if (!is_end(c)) {
				if (isalpha(c))
					word[i++] = tolower(c);
			} else { // save word and clear buffer
				word[i] = 0; // null terminator
				if (i > 0) // only store if word is not empty
					insert(trie, word, num);
				memset(word, 0, MAX_WORD_SIZE);
				i = 0;
			}
		}
		fclose(file);
	}
}

void load_dir(char* path, struct Node* trie) {
	D printf("Looking at directory: %s\n", path);

	DIR *d;
	struct dirent *dir;
	d = opendir(path);
	if (d) {
		char fullpath[1024];
		int id = 0; // source text id
		while ((dir = readdir(d)) != NULL) {
			char* ext = strchr(dir->d_name, '.');
			if (ext && !strcmp(ext, ".txt")) { // files with .txt extention
				// build full file path
				memset(fullpath, 0, 1024);
				strcat(fullpath, path);
				strcat(fullpath, "/"); // ensure slash
				strcat(fullpath, dir->d_name);

				// load words into dictionary
				load_file(fullpath, trie, id++);
			} else D printf("Ignoring file: %s\n", dir->d_name);
		}
		closedir(d);
	}

	// return trie;
}

int main(int argc, char *argv[]) {
	// initialize structure to hold words
	struct Node* trie = get_node();
	// use first argument as path
	load_dir(argv[1], trie);

	// retrieve longest common word
	char buffer1[MAX_WORD_SIZE];
	char* longest_common_words[NUMBER_OF_COMMON];
	for (int i = 0; i < NUMBER_OF_COMMON; i++)
		longest_common_words[i] = (char*)calloc(MAX_WORD_SIZE, sizeof(char));
	longest(trie, buffer1, 0, longest_common_words);
	for (int i = 0; i < NUMBER_OF_COMMON; i++)
		printf("%s ", longest_common_words[i]);
	printf("\n");

	// cleanup
	for (int i = 0; i < NUMBER_OF_COMMON; i++)
		free(longest_common_words[i]);
	cleanup(trie);

	return 0;
}
