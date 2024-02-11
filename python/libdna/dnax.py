from collections import Counter, defaultdict


def count(seq: str) -> Counter[str]:
	return Counter(seq)


def count_mismatches(seq1: str, seq2: str) -> int:
	return sum(a != b for a, b in zip(seq1, seq2))

_ex_in = "acgtACGTuU"
_ex_out = "ACGTACGTTT"
_ex_else = "QWERYIOPSDFHJKLZXVBNM"
_ex_tab = str.maketrans(_ex_in, _ex_out, _ex_else)

def extract_dna4(seq: str) -> str:
	return seq.translate(_ex_tab)


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


def replace(seq: str, map) -> str:
	tab = str.maketrans(map)
	return seq.translate(tab)


_rc_tab = str.maketrans(
	"acgtACGTuU",
	"tgcaTGCAtT",
	"zZ"  # TODO: extend
)


def revcomp(seq: str) -> str:
	return seq[::-1].translate(_rc_tab)

