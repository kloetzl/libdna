/**
 * SPDX-License-Identifier: MIT
 * Copyright 2021 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"
#include "utils.h"

#include <arm_neon.h>
#include <assert.h>
#include <string.h>

DNA_PUBLIC
size_t
dna4_count_mismatches(const char *begin, const char *end, const char *other)
{
	size_t substitutions = 0;
	size_t length = end - begin;
	size_t offset = 0;

	typedef uint8x16_t vec_type;
	size_t vec_size = sizeof(vec_type);
	size_t vec_length = length / vec_size;

	size_t vec_offset = 0;
	for (; vec_offset < vec_length; vec_offset++) {
		vec_type b;
		memcpy(&b, begin + vec_offset * vec_size, vec_size);
		vec_type o;
		memcpy(&o, other + vec_offset * vec_size, vec_size);

		const vec_type eq = vceqq_u8(b, o);
		const vec_type all1 = vdupq_n_u8(1);
		const vec_type check = vandq_u8(eq, all1);
		char popcount = vaddvq_u8(check);

		substitutions += vec_size - popcount;
	}

	offset = vec_offset * vec_size;
	for (; offset < length; offset++) {
		if (UNLIKELY(begin[offset] != other[offset])) {
			substitutions++;
		}
	}

	return substitutions;
}
