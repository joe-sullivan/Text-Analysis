class Trie:
	def __init__(self):
		self.root = {'':{}}
		self.parent = '_parent'
		self.end = '_end'

	def add(self, word):
		current = self.root
		for letter in word:
			if letter not in current:
				current[letter] = {self.parent: current}
			current = current[letter]
		if self.end in current:
			x = current[self.end]
			current[self.end] = x+1
		else:
			current[self.end] = 0

	def remove(self, word):
		current = self.root
		for i, letter in enumerate(word):
			if letter not in current:
				return # word is not in trie
			current = current[letter]
		if self.end in current:
			if current[self.end]:
				x = current[self.end]
				current[self.end] = x-1
				return current[self.end]
			else:
				del current[self.end]
		return -1

	def search(self, word):
		current = self.root
		for letter in word:
			if letter not in current:
				return False
			current = current[letter]
		return self.end in current

	def longest(self, word='', node=None):
		if node is None:
			node = self.root
		return self

	def __str__(self):
		return str(self.root)

if __name__ == '__main__':
	t = Trie()

	print('***ADD***')
	t.add('foo')
	t.add('food')
	t.add('bar')
	t.add('bar')
	t.add('bark')
	t.add('baz')
	t.add('baz')
	t.add('baz')
	print(t)

	print('***FIND***')
	if t.search('foo'):
		print('has foo')
	if t.search('bar'):
		print('has bar')
	if t.search('barr'):
		print('has barr')
	if t.search('ba'):
		print('has ba')

	print('***REMOVE***')
	print(t.remove('bar'))
	print(t.remove('bar'))
	if t.search('bar'):
		print('has bar')
	if t.search('bark'):
		print('has bark')
	print(t)

	print('***LONGEST***')
	print(t.longest())
