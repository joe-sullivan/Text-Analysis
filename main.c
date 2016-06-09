#include <ctype.h>
#include <dirent.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct Node* trie;

struct arg_struct {
	char* path;
	struct Node* trie;
	int num;
};

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

void* load_file(void *vargp) {
	struct arg_struct *args = (struct arg_struct *)vargp;
	char* path = args->path;
	int num = args->num;

	D printf("Loading file[%d]: %s\n", num, path);
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
				if (i > 0) { // only store if word is not empty
					// pthread_mutex_lock(&mutex);
					insert(trie, word, num);
					// pthread_mutex_unlock(&mutex);
				}
				memset(word, 0, MAX_WORD_SIZE);
				i = 0;
			}
		}
		fclose(file);
	}
}

void load_dir(char* path) {
	D printf("Looking at directory: %s\n", path);

	DIR *d;
	struct dirent *dir;
	d = opendir(path);
	if (d) {
		char fullpath[1024];
		int id = 0; // source text id
		pthread_t tid[NUMBER_OF_SOURCES];

		struct arg_struct args[NUMBER_OF_SOURCES];
		while ((dir = readdir(d)) != NULL) {
			char* ext = strchr(dir->d_name, '.');
			if (ext && !strcmp(ext, ".txt")) { // files with .txt extention
				// build full file path
				memset(fullpath, 0, 1024);
				strcat(fullpath, path);
				strcat(fullpath, "/"); // ensure slash
				strcat(fullpath, dir->d_name);

				// asynchronously load words into dictionary
				args[id].path = fullpath;
				args[id].num = id;
				pthread_create(&tid[id], NULL, load_file, &args[id]);
				id++;
			} else D printf("Ignoring file: %s\n", dir->d_name);
		}
		closedir(d);

		// wait for thread to finish
		for (int i = 0; i < NUMBER_OF_SOURCES; i++)
			pthread_join(tid[i], NULL);
	}
}

int main(int argc, char *argv[]) {
	// initialize structure to hold words
	trie = get_node();
	// use first argument as path
	load_dir(argv[1]);

	// retrieve longest common word
	D printf("Getting longest common words...\n");
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
