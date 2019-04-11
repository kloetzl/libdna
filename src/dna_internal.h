#pragma once

typedef char*(dna4_revcomp_fn)(
	const char *begin,
	const char *end,
	char *dest);

char *
dna4_revcomp_generic(const char *begin, const char *end, char *dest);
char *
dna4_revcomp_sse42(const char *begin, const char *end, char *dest);
char *
dna4_revcomp_avx2(const char *begin, const char *end, char *dest);
