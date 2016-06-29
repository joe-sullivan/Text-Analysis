#include <ctype.h>
#include <dirent.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"

#define BUFFER_SIZE 16*1024

char inalpha(char c) {
	// edge cases
	if (c == '\'') return c;

	// or char with 0x00100000 to convert to lower
	char ret = c | 32;
	// check if ret is valid
	if (ret >= 'a' && ret <= 'z')
		return ret;
	return 0;
}

void load_file(char* path, Node* trie, int num) {
	D printf("Loading file: %s\n", path);

	FILE* file = fopen(path, "r");
	if (file) {
		String string;
		string.length = 0;
		char buf[BUFFER_SIZE];

		// read book into buffer
		int bytes_read;
		while((bytes_read = fread(buf, sizeof(char), BUFFER_SIZE, file))) {
			// parse buffer
			for (int i = 0; i < bytes_read; i++) {
				char c;
				if ((c = inalpha(buf[i]))) {
					// add character to string and increment length
					string.data[string.length++] = c;
				} else if (string.length > 0) { // save word and clear buffer
					insert(trie, &string, num);
					string.length = 0;
				}
			}
		}
	}
	fclose(file);
	D printf("Finished loading file[%d]: %s\n", num, path);
}

void load_dir(char* path, Node* trie) {
	D printf("Looking at directory: %s\n", path);

	DIR* d;
	struct dirent *dir;
	d = opendir(path);
	if (d) {
		char fullpath[1024];
		int id = 0; // source text id
		while (id < NUMBER_OF_SOURCES && (dir = readdir(d)) != NULL) {
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
}

int compare(const void* a, const void* b) {
	int len_a = ((String*)a)->length;
	int len_b = ((String*)b)->length;
	return (len_a > len_b) - (len_a < len_b);
}

int main(int argc, char* argv[]) {
	// initialize structure to hold words
	Node* trie = get_node();

	// use first argument as path
	load_dir(argv[1], trie);

	// setup array to hold the longest common words
	String longest_common_words[NUMBER_OF_COMMON];
	for (int i = 0; i < NUMBER_OF_COMMON; i++)
		longest_common_words[i].length = 0;

	// find longest common words
	longest(trie, longest_common_words);

	// sort
	D qsort(longest_common_words, NUMBER_OF_COMMON, sizeof(String), compare);

	// print them
	for (int i = 0; i < NUMBER_OF_COMMON; i++)
		printf("%s\n", longest_common_words[i].data);

	// cleanup
	D cleanup(trie);

	return 0;
}
