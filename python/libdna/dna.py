# SPDX-License-Identifier: MIT
# Copyright 2024 (C) Fabian Klötzl

def version() -> int:
	return 0


def _uint64(value: int) -> int:
	return value & 0xffffffffFFFFFFFF


def ihash(value: int) -> int:
	key = _uint64(value)
	key = _uint64(key * 0x7b215eedec9e1967)
	key ^= key >> 37
	key = _uint64(key + 0x16c5c874ea637686)
	key ^= _uint64(key << 11)
	key = _uint64(key * 0xff7dbf225491d985)
	key ^= key >> 15
	key = _uint64(key * 0x3d5fe41de2ea4e4f)
	key ^= key >> 19
	key = _uint64(key + 0x44ee2cfcca48954e)
	key ^= key >> 23
	key = _uint64(key * 0x4f7659e92097460b)
	key ^= key >> 29
	return key


def ihash_invert(value: int) -> int:
	key = value
	tmp = key
	tmp = key ^ key >> 29
	tmp = key ^ tmp >> 29
	key = key ^ tmp >> 29
	key = _uint64(key * 0x66e53f4c3ffe95a3)
	tmp = key ^ key >> 23
	tmp = key ^ tmp >> 23
	key = key ^ tmp >> 23
	key = _uint64(key - 0x44ee2cfcca48954e)
	tmp = key ^ key >> 19
	tmp = key ^ tmp >> 19
	tmp = key ^ tmp >> 19
	key = key ^ tmp >> 19
	key = _uint64(key * 0xf304917f13fe08af)
	tmp = key ^ key >> 15
	tmp = key ^ tmp >> 15
	tmp = key ^ tmp >> 15
	key = key ^ tmp >> 15
	key = _uint64(key * 0x398b71be7aae374d)
	tmp = key ^ key << 11
	tmp = key ^ tmp << 11
	tmp = key ^ tmp << 11
	tmp = key ^ tmp << 11
	tmp = key ^ tmp << 11
	tmp = key ^ tmp << 11
	key = key ^ tmp << 11
	key = _uint64(key - 0x16c5c874ea637686)
	key ^= key >> 37
	key = _uint64(key * 0xee84443b086ef257)
	return key


