# SPDX-License-Identifier: MIT
# Copyright 2024 (C) Fabian Klötzl


def version() -> int:
    return 0


def _uint64(value: int) -> int:
    return value & 0xFFFFFFFFFFFFFFFF


def ihash(value: int) -> int:
    key = _uint64(value)
    key = _uint64(key * 0x7B215EEDEC9E1967)
    key ^= key >> 37
    key = _uint64(key + 0x16C5C874EA637686)
    key ^= _uint64(key << 11)
    key = _uint64(key * 0xFF7DBF225491D985)
    key ^= key >> 15
    key = _uint64(key * 0x3D5FE41DE2EA4E4F)
    key ^= key >> 19
    key = _uint64(key + 0x44EE2CFCCA48954E)
    key ^= key >> 23
    key = _uint64(key * 0x4F7659E92097460B)
    key ^= key >> 29
    return key


def ihash_invert(value: int) -> int:
    key = value
    tmp = key
    tmp = key ^ key >> 29
    tmp = key ^ tmp >> 29
    key = key ^ tmp >> 29
    key = _uint64(key * 0x66E53F4C3FFE95A3)
    tmp = key ^ key >> 23
    tmp = key ^ tmp >> 23
    key = key ^ tmp >> 23
    key = _uint64(key - 0x44EE2CFCCA48954E)
    tmp = key ^ key >> 19
    tmp = key ^ tmp >> 19
    tmp = key ^ tmp >> 19
    key = key ^ tmp >> 19
    key = _uint64(key * 0xF304917F13FE08AF)
    tmp = key ^ key >> 15
    tmp = key ^ tmp >> 15
    tmp = key ^ tmp >> 15
    key = key ^ tmp >> 15
    key = _uint64(key * 0x398B71BE7AAE374D)
    tmp = key ^ key << 11
    tmp = key ^ tmp << 11
    tmp = key ^ tmp << 11
    tmp = key ^ tmp << 11
    tmp = key ^ tmp << 11
    tmp = key ^ tmp << 11
    key = key ^ tmp << 11
    key = _uint64(key - 0x16C5C874EA637686)
    key ^= key >> 37
    key = _uint64(key * 0xEE84443B086EF257)
    return key
