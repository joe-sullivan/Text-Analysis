#include <ctype.h>
#include <dirent.h>
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"

#define BUFFER_SIZE 16*1024

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
Node *trie;
struct arg_struct {
	char* path;
	int num;
};

int _pguess = INT_MAX; // ensure REALLY large words are included
int _guess = 8;

char inalpha(char c) {
	// edge cases
	if (c == '\'') return c;

	// or char with 0x00100000 to convert to lower
	char lower = c | 32;
	// check if 'a' <= lower <= 'z'
	if ((unsigned)(lower-'a') < 'z'-'a') // this creates a single branch
		return lower;
	return 0;
}
void* load_file(void *vargp) {
	struct arg_struct *args = (struct arg_struct *)vargp;
	char* path = args->path;
	int num = args->num;
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
					if (string.length > _guess && string.length <= _pguess) {
						pthread_mutex_lock(&mutex);
						insert(trie, &string, num);
						pthread_mutex_unlock(&mutex);
					}
					string.length = 0;
				}
			}
		}
	}
	fclose(file);
	D printf("Finished loading file[%d]: %s\n", num, path);
	D free(path);
	pthread_exit(NULL);
}

void load_dir(char* path) {
	D printf("Looking at directory: %s\n", path);

	DIR* d;
	struct dirent *dir;
	d = opendir(path);
	if (d) {
		char* fullpath;
		int id = 0; // source text id

		pthread_t tid[NUMBER_OF_SOURCES];
		struct arg_struct args[NUMBER_OF_SOURCES];
		while (id < NUMBER_OF_SOURCES && (dir = readdir(d)) != NULL) {
			char* ext = strchr(dir->d_name, '.');
			if (ext && !strcmp(ext, ".txt")) { // files with .txt extention
				// build full file path
				fullpath = (char*)calloc(1024, sizeof(char));
				strcat(fullpath, path);
				strcat(fullpath, "/"); // ensure slash
				strcat(fullpath, dir->d_name);

				// load words into dictionary
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

int compare(const void* b, const void* a) {
	int len_a = ((String*)a)->length;
	int len_b = ((String*)b)->length;
	return (len_a > len_b) - (len_a < len_b);
}

bool doit(Node* trie, char* path) {
	// use first argument as path
	load_dir(path);

	// setup array to hold the longest common words
	String longest_common_words[NUMBER_OF_COMMON];
	for (int i = 0; i < NUMBER_OF_COMMON; i++)
		longest_common_words[i].length = 0;

	// find longest common words
	if (!longest(trie, longest_common_words)) return false;

	// sort
	D qsort(longest_common_words, NUMBER_OF_COMMON, sizeof(String), compare);

	// print them
	for (int i = 0; i < NUMBER_OF_COMMON; i++)
		printf("%.*s\n", longest_common_words[i].length,
		                 longest_common_words[i].data);

	// cleanup
	D cleanup(trie);

	return true;
}

int main(int argc, char* argv[]) {
	// initialize structure to hold words
	trie = get_node();

	if (argc > 2)
		_guess = atoi(argv[2]);
	D printf("Guess: %d\n", _guess);

	while (!doit(trie, argv[1]) && _guess) {
		_pguess = _guess; // save previous guess
		_guess /= 2; // decrease guess size
		D printf("Lowering guess from %d to %d\n", _pguess, _guess);
	}

	return 0;
}
