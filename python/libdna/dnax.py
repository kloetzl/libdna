# SPDX-License-Identifier: MIT
# Copyright 2024 (C) Fabian Klötzl

from collections import Counter


def count(seq: str) -> Counter[str]:
	return Counter(seq)


def count_mismatches(seq1: str, seq2: str) -> int:
	return sum(a != b for a, b in zip(seq1, seq2))


def _replace(from_: str, to: str) -> str:
	import re
	exclude = re.compile(f"[^{from_}]")
	tab = str.maketrans(from_, to)

	return lambda seq: exclude.sub("", seq).translate(tab)


_extract_dna4 = _replace("acgtACGTuU", "ACGTACGTTT")


def extract_dna4(seq: str) -> str:
	return _extract_dna4(seq)


def _first_pos_where(it) -> int:
	for i, a in enumerate(it):
		if a:
			return i
		m = i
	return m + 1


def find_first_mismatch(seq1: str, seq2: str) -> int:
	return _first_pos_where(a != b for a,b in zip(seq1, seq2))


def find_first_of(seq: str, chars) -> int:
	return _first_pos_where(c in chars for c in seq)


def find_first_not_of(seq: str, chars) -> int:
	return _first_pos_where(c not in chars for c in seq)


def replace(seq: str, from_: str, to: str) -> str:
	return _replace(from_, to)(seq)


_revcomp = _replace("acgtACGTuU", "tgcaTGCAaA")


def revcomp(seq: str) -> str:
	return _revcomp(seq[::-1])

