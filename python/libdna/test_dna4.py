from dna4 import *

def test_basic():
	assert revcomp("ACGT") == "ACGT"
	assert len(random(100)) == 100
	assert count_mismatches_rc("ACGT", "ACGT") == 0
	a = pack_2bits("ACGT")
	assert unpack_2bits(a, 4) == "ACGT"


