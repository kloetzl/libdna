# SPDX-License-Identifier: MIT
# Copyright 2024 (C) Fabian Klötzl

from dnax import *


def test_simple():
    assert count_mismatches("AACGT", "AACGT") == 0
    assert extract_dna4("acgt") == "ACGT"
    assert find_first_mismatch("ACGT", "ACGt") == 3
    assert find_first_of("ACGTt!", "acgt") == 4
    assert find_first_not_of("ACGTt!", "acgt") == 0
    assert find_first_not_of("ACGT", set("ACGT")) == 4

    a = "AACGTACGT"
    b = "AACGTACCT"
    assert count_mismatches(a, b) == 1
    aa = revcomp(a)
    bb = revcomp(b)
    assert count_mismatches(aa, bb) == 1


def test_count_mismatches():
    seq1 = "ACGTACGTACGT"
    seq2 = "ACGTTCGTACGA"
    assert count_mismatches(seq1, seq2) == 2


def test_revcomp():
    seq = "ACGTACGTACGT"
    assert revcomp(seq) == seq

    seq = "ACGTacgtACGT!"
    assert revcomp(seq) == "ACGTacgtACGT"
