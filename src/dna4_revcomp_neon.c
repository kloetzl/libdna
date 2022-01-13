/**
 * SPDX-License-Identifier: MIT
 * Copyright 2021 - 2022 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"
#include "utils.h"

#include <arm_neon.h>
#include <assert.h>
#include <string.h>

// On Arm there is no need for dispatching. Hence we can drop the _neon suffix
// from the function name.

DNA_PUBLIC
char *
dna4_revcomp(const char *begin, const char *end, char *__restrict dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	size_t length = end - begin;
	typedef uint8x16_t vec_type;

	size_t vec_bytes = sizeof(vec_type);
	size_t vec_length = length / vec_bytes;

	if (UNLIKELY(length < vec_bytes)) {
		return dna4_revcomp_generic(begin, end, dest);
	}

	const vec_type all0 = vdupq_n_u8(0);
	const vec_type all2 = vdupq_n_u8(2);
	const vec_type all4 = vdupq_n_u8(4);
	const vec_type all21 = vdupq_n_u8(21);
	static const unsigned char revdata[16] = //
		{15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
	const vec_type revmask = vld1q_u8(revdata);

	size_t vec_offset = 0;
	for (; vec_offset < vec_length; vec_offset++) {

		const char *from = end - vec_offset * vec_bytes - vec_bytes;
		char *to = dest + vec_offset * vec_bytes;

		vec_type chunk;
		memcpy(&chunk, from, vec_bytes);

		const vec_type reversed = vqtbl1q_u8(chunk, revmask);
		const vec_type is_zero = vtstq_u8(reversed, all2);
		const vec_type blended_mask = vbslq_u8(is_zero, all4, all21);
		const vec_type xored = veorq_u8(blended_mask, reversed);

		memcpy(to, &xored, vec_bytes);
	}

	// problematic for size < vec_bytes!
	for (size_t i = 0; i < length - vec_offset * vec_bytes; i += vec_bytes) {

		const char *from = begin + i;
		char *to = dest + length - vec_bytes - i;

		vec_type chunk;
		memcpy(&chunk, from, vec_bytes);

		const vec_type reversed = vqtbl1q_u8(chunk, revmask);
		const vec_type is_zero = vtstq_u8(reversed, all2);
		const vec_type blended_mask = vbslq_u8(is_zero, all4, all21);
		const vec_type xored = veorq_u8(blended_mask, reversed);

		memcpy(to, &xored, vec_bytes);
	}

	return dest + length;
}
