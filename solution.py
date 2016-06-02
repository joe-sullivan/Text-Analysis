#!/usr/bin/env python3

import argparse, os, re

class Logging:
	def __init__(self, enable=False):
		self.enabled = enable

	def write(self, msg):
		if self.enabled:
			print(msg)

class Trie:
	def __init__(self):
		self.end = '_end'
		self.root = dict()

	def insert(self, word):
		current_dict = self.root
		for letter in word:
			current_dict = current_dict.setdefault(letter, {})
		current_dict[self.end] = self.end

	def search(self, word):
		current_dict = self.root
		for letter in word:
			if letter in current_dict:
				return False
			current_dict = current_dict[letter]
		if self.end in current_dict:
			return True
		return False

	def get(self):
		return self.root

class Container:
	def __init__(self):
		self.list = []

	def populate(self, word_gen):
		for word in word_gen:
			if word not in self.list:
				self.list.append(word)

	def include(self, word_gen):
		new_list = []
		for word in word_gen:
			if word in self.list and word not in new_list:
				new_list.append(word)
		self.list = new_list

	def get(self, sort=False):
		if sort: self.list.sort(key=len)
		return self.list

	def stats(self):
		LOG.write('Number of words: %d' % len(self.list))

def _scrub(word):
	# convert to lowercase and remove non-alphanumeric characters
	regex = re.compile('[^a-z]+')
	return regex.sub('', word.lower())

def _word_gen(file):
	for line in file:
		for word in line.split():
			yield _scrub(word)

def read_src(path):
	c = Container() # container for words
	# get files
	files = [os.path.join(path, x) for x in os.listdir(path)]
	# sort by smallest first (simple way to limit search space)
	sorted_files = sorted(files, key=os.path.getsize)
	for file in sorted_files:
		# read file into data structure
		with open(os.path.join(path, file), 'r') as f:
			LOG.write('**FILENAME [%s]**' % file)
			if file == sorted_files[0]:
				c.populate(_word_gen(f))
			else:
				c.include(_word_gen(f))
		c.stats()
	return c

if __name__ == '__main__':
	# parse arguments
	parser = argparse.ArgumentParser(description='Find longest common word')
	parser.add_argument('-v', '--verbose', action='store_true', help='be verbose')
	parser.add_argument('-p', '--path', default='samples', help='path to source texts')
	args = parser.parse_args()

	global LOG
	LOG = Logging(enable=args.verbose)

	if not args.path:
		print('No path provided')
		exit(0)

	# read in source text
	words = read_src(os.path.abspath(args.path))

	# get largest word and print
	common = words.get(sort=True)
	print(common)
	largest = common[-1]
	print('Largest common word is: %s' % largest)

	# TRIE TEST
	# s = 'foo bar baz'
	# t = Trie()
	# for word in s:
	# 	t.insert(word)
	# # print(t.get())
	# print(t.search('hello'))