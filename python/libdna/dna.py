import ctypes

def version() -> int:
	return 0


def uint64(value: int) -> int:
	return value & 0xffffffffFFFFFFFF


def ihash(value: int) -> int:
	key = uint64(value)
	key *= 0x7b215eedec9e1967
	key = uint64(key)
	key ^= key >> 37
	key += 0x16c5c874ea637686
	key = uint64(key)
	key ^= uint64(key << 11)
	key *= 0xff7dbf225491d985
	key = uint64(key)
	key ^= key >> 15
	key *= 0x3d5fe41de2ea4e4f
	key = uint64(key)
	key ^= key >> 19
	key += 0x44ee2cfcca48954e
	key = uint64(key)
	key ^= key >> 23
	key *= 0x4f7659e92097460b
	key = uint64(key)
	key ^= key >> 29
	return key


def ihash_invert(value: int) -> int:
	key = value
	tmp = key
	tmp = key ^ key >> 29
	tmp = key ^ tmp >> 29
	key = key ^ tmp >> 29
	key *= 0x66e53f4c3ffe95a3
	key = uint64(key)
	tmp = key ^ key >> 23
	tmp = key ^ tmp >> 23
	key = key ^ tmp >> 23
	key -= 0x44ee2cfcca48954e
	key = uint64(key)
	tmp = key ^ key >> 19
	tmp = key ^ tmp >> 19
	tmp = key ^ tmp >> 19
	key = key ^ tmp >> 19
	key *= 0xf304917f13fe08af
	key = uint64(key)
	tmp = key ^ key >> 15
	tmp = key ^ tmp >> 15
	tmp = key ^ tmp >> 15
	key = key ^ tmp >> 15
	key *= 0x398b71be7aae374d
	key = uint64(key)
	tmp = key ^ key << 11
	tmp = key ^ tmp << 11
	tmp = key ^ tmp << 11
	tmp = key ^ tmp << 11
	tmp = key ^ tmp << 11
	tmp = key ^ tmp << 11
	key = key ^ tmp << 11
	key -= 0x16c5c874ea637686
	key = uint64(key)
	key ^= key >> 37
	key *= 0xee84443b086ef257
	key = uint64(key)
	return key


