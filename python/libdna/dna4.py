# SPDX-License-Identifier: MIT
# Copyright 2024 (C) Fabian Klötzl

_comp = str.maketrans("ACGT", "TGCA")

_NOISE1 = 0xb5297a4d  # 0b1011'0101'0010'1001'0111'1010'0100'1101
_NOISE2 = 0x68e31da4  # 0b0110'1000'1110'0011'0001'1101'1010'0100
_NOISE3 = 0x1b56c4e9  # 0b0001'1011'0101'0110'1100'0100'1110'1001
_NOISE4 = 0xaaea97a5  # determined by fair dice roll

_str2int = {"A": 0, "C": 1, "G": 2, "T": 3}
_int2str = "ACGT"


def _uint32(value: int) -> int:
	return value & 0xffffffff


def _squirrel3(n: int, seed: int) -> int:
	n = _uint32(n * _NOISE1)
	n = _uint32(n + seed)
	n = _uint32(n ^ n >> 8)
	n = _uint32(n + _NOISE2)
	n = _uint32(n ^ n << 13)
	n = _uint32(n * _NOISE3)
	n = _uint32(n ^ n >> 17)
	return n


def _int2chars(value: int) -> str:
	a = _int2str[value & 3]
	value >>= 8
	b = _int2str[value & 3]
	value >>= 8
	c = _int2str[value & 3]
	value >>= 8
	d = _int2str[value & 3]
	return a + b + c + d


def revcomp(seq: str) -> str:
	"""Compute the reverse complement.
	"""
	return seq[::-1].translate(_comp)


def random(length: int, seed: int) -> str:
	"""Generate a string of random nucleotides.
	"""
	length4 = length // 4 + 1
	seed = _squirrel3(seed, _NOISE4)
	ints = (_squirrel3(value, seed) for value in range(length4))
	chars = (_int2chars(value) for value in ints)
	return "".join(chars)[:length]


def count_mismatches_rc(seq1: str, seq2: str) -> int:
	"""Compute the number of mismatches between one sequence and the revcomp of another.
	"""
	rc = revcomp(seq2)
	return sum(a != b for a, b in zip(seq1, rc))


def pack_2bits(kmer: str) -> int:
	"""Pack a kmer into an int using two bits per nucleotide.
	"""
	value = 0
	for c in kmer[:32]:
		value <<= 2
		value |= _str2int[c]
	return value


def unpack_2bits(packed: int, k: int) -> str:
	"""Unpack a kmer from an integer using two bits per nucleotides.
	"""
	seq = ["_"] * k
	while k:
		seq[k - 1] = _int2str[packed & 3]
		packed >>= 2
		k -= 1
	return "".join(seq)




