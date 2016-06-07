#!/usr/bin/env python3

import argparse, os, re

class Logging:
	def __init__(self, enable=False):
		self.enabled = enable

	def write(self, msg):
		if self.enabled:
			print(msg)

class Container:
	def __init__(self):
		self.dict = {}

	def populate(self, word_gen):
		for word in word_gen:
			if word not in self.dict:
				self.dict[word] = True

	def include(self, word_gen):
		new_dict = {}
		for word in word_gen:
			if word in self.dict:
				new_dict[word] = True
		self.dict = new_dict

	def get(self, sort=False):
		common_words = list(self.dict)
		if sort: common_words.sort(key=len)
		return common_words

	def stats(self):
		LOG.write('Number of words: %d' % len(list(self.dict)))

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
	# print(common)
	largest = common[-1]
	print('Largest common word is: %s' % largest)
