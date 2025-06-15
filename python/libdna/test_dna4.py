# SPDX-License-Identifier: MIT
# Copyright 2024 (C) Fabian Klötzl

from dna4 import *


def test_basic():
    assert revcomp("ACGT") == "ACGT"
    assert len(random(100, 0)) == 100
    assert count_mismatches_rc("AACGT", "ACGTT") == 0
    a = pack_2bits("ACGT")
    assert unpack_2bits(a, 4) == "ACGT"


def test_count_mismatches_rc():
    a = "TAACCGCCCTTGGG"
    b = "CCCAAGTGCGATTA"
    assert count_mismatches_rc(a, b) == 2


def test_random():
    # Should give same output as C version
    assert random(10, 1729) == "AAGCCGTTCC"
