#pragma once

/**
 * SPDX-License-Identifier: MIT
 * Copyright 2020 - 2024 © Fabian Klötzl
 *
 * C++ convenience header
 */

#if __cplusplus < 201703L
#error "This header requires C++17"
#endif

#include "dna.h"

#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace dna
{
inline char *
begin(std::string &str)
{
	return str.data();
}

inline char *
end(std::string &str)
{
	return str.data() + str.size();
}

inline const char *
begin(std::string_view str)
{
	return str.data();
}

inline const char *
end(std::string_view str)
{
	return str.data() + str.size();
}

inline const char *
cbegin(std::string_view str)
{
	return str.data();
}

inline const char *
cend(std::string_view str)
{
	return str.data() + str.size();
}

inline int
version()
{
	return dna_version();
}

inline uint64_t
ihash(uint64_t key)
{
	return dna_ihash(key);
}

inline uint64_t
ihash_invert(uint64_t key)
{
	return dna_ihash_invert(key);
}

template <typename Fn>
inline std::string
make_string_for_overwrite(size_t length, Fn fn)
{
#if __cplusplus >= 202110L

	auto ret = std::string();
	ret.resize_and_overwrite(length, [&fn](char *buf, size_t size) {
		auto ptr = fn(buf, buf + size);
		return ptr - buf;
	});
	return ret;

#else

	auto ret = std::string(length, '\0');
	auto ptr = fn(dna::begin(ret), dna::end(ret));
	auto it = ret.begin() + (ptr - dna::begin(ret));
	ret.erase(it, ret.end());
	return ret;

#endif
}

} // namespace dna

namespace dna4
{
__attribute__((deprecated)) inline size_t
count_mismatches(std::string_view s1, std::string_view s2)
{
	// assume s1.size() == s2.size()
	return dnax_count_mismatches(
		dna::cbegin(s1), dna::cend(s1), dna::cbegin(s2));
}

inline size_t
count_mismatches_rc(std::string_view s1, std::string_view s2)
{
	// assume s1.size() == s2.size()
	return dna4_count_mismatches_rc(
		dna::cbegin(s1), dna::cend(s1), dna::cbegin(s2));
}

inline std::string
random(size_t length, uint32_t seed)
{
	return dna::make_string_for_overwrite(
		length, [seed](char *dest_begin, char *dest_end) {
			dna4_fill_random(dest_begin, dest_end, seed);
			return dest_end;
		});
}

inline std::string
revcomp(std::string_view str)
{
	return dna::make_string_for_overwrite(
		str.size(), [str](char *dest_begin, char *) {
			return dna4_revcomp(dna::cbegin(str), dna::cend(str), dest_begin);
		});
}

inline uint64_t
pack_2bits(size_t k, std::string_view str)
{
	return dna4_pack_2bits(dna::cbegin(str), k);
}

inline std::string
unpack_2bits(size_t k, uint64_t packed)
{
	return dna::make_string_for_overwrite(
		k, [packed, k](char *dest_begin, char *dest_end) {
			dna4_unpack_2bits(dest_begin, k, packed);
			return dest_end;
		});
}
} // namespace dna4

namespace dnax
{
inline size_t
count_mismatches(std::string_view s1, std::string_view s2)
{
	// assume s1.size() == s2.size()
	return dnax_count_mismatches(
		dna::cbegin(s1), dna::cend(s1), dna::cbegin(s2));
}

inline auto
pack_4bits(std::string_view str)
{
	auto ret = std::vector<unsigned char>((str.size() + 1) / 2, '\0');
	auto encoded =
		dnax_pack_4bits(dna::cbegin(str), dna::cend(str), ret.data());
	auto bytes = (encoded + 1) / 2;
	ret.erase(ret.begin() + bytes, ret.end());
	return ret;
}

inline auto
unpack_4bits(const std::vector<unsigned char> &vec)
{
	return dna::make_string_for_overwrite(
		vec.size() * 2, [&vec](char *dest_begin, char *dest_end) {
			dnax_unpack_4bits(vec.data(), vec.data() + vec.size(), dest_begin);
			return dest_end;
		});
}

inline std::string
revcomp(const signed char *table, std::string_view str)
{
	return dna::make_string_for_overwrite(
		str.size(), [str, table](char *dest_begin, char *) {
			return dnax_revcomp(
				table, dna::cbegin(str), dna::cend(str), dest_begin);
		});
}

inline std::string
translate(std::string_view str)
{
	return dna::make_string_for_overwrite(
		str.size() / 3, [str](char *dest_begin, char *) {
			return dnax_translate(dna::cbegin(str), dna::cend(str), dest_begin);
		});
}

inline std::array<size_t, 256>
count(std::string_view str)
{
	auto ret = std::array<size_t, 256>();
	dnax_count(std::begin(ret), dna::cbegin(str), dna::end(str));
	return ret;
}

inline std::string
extract_dna4(std::string_view str)
{
	return dna::make_string_for_overwrite(
		str.size(), [str](char *dest_begin, char *) {
			return dnax_extract_dna4(
				dna::cbegin(str), dna::cend(str), dest_begin);
		});
}

inline std::string
replace(const signed char *table, std::string_view str)
{
	return dna::make_string_for_overwrite(
		str.size(), [str, table](char *dest_begin, char *) {
			return dnax_replace(
				table, dna::cbegin(str), dna::cend(str), dest_begin);
		});
}

inline std::string_view::size_type
find_first_mismatch(std::string_view s1, std::string_view s2)
{
	auto pos = dnax_find_first_mismatch(
		dna::cbegin(s1), dna::cend(s1), dna::cbegin(s2));
	return pos - dna::cbegin(s1);
}

inline std::string_view::size_type
find_first_of(const signed char *table, std::string_view str)
{
	auto pos = dnax_find_first_of(table, dna::cbegin(str), dna::cend(str));
	return pos - dna::cbegin(str);
}

inline std::string_view::size_type
find_first_not_of(const signed char *table, std::string_view str)
{
	auto pos = dnax_find_first_not_of(table, dna::cbegin(str), dna::cend(str));
	return pos - dna::cbegin(str);
}

} // namespace dnax
