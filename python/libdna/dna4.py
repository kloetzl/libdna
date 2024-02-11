
_comp = str.maketrans("ACGT", "TGCA")

def revcomp(seq: str) -> str:
	return seq[::-1].translate(_comp)


def random(length: int) -> str:
	return "A" * length


def count_mismatches_rc(seq1: str, seq2: str) -> int:
	rc = revcomp(seq2)
	return sum(a != b for a, b in zip(seq1, seq2))


_str2int = {"A": 0, "C": 1, "G": 2, "T": 3}
_int2str = "ACGT"


def pack_2bits(kmer: str) -> int:
	value = 0
	for c in kmer[:32]:
		value <<= 2
		value |= _str2int[c]
	return value


def unpack_2bits(packed: int, k: int) -> str:
	seq = ["_"] * k
	while k:
		seq[k - 1] = _int2str[packed & 3]
		packed >>= 2
		k -= 1
	return "".join(seq)




