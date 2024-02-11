from dnax import *


def test_simple():
	assert count_mismatches("AACGT", "AACGT") == 0
	assert extract_dna4("acgt") == "ACGT"
	assert find_first_mismatch("ACGT", "ACGt") == 3
	assert find_first_of("ACGTt!", "acgt") == 4
	assert find_first_not_of("ACGTt!", "acgt") == 0
	assert find_first_not_of("ACGT", set("ACGT")) == 4


