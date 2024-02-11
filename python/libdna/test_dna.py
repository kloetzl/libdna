from dna import *


def test_basic():
	a = ihash(1729)
	assert ihash_invert(a) == 1729

