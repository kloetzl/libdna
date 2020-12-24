#include <kloetzl/dna.h>

typedef void (*process_fn)(const char *, size_t);

size_t
process_canonical_kmers(
	const char *begin, const char *end, size_t k, process_fn callback)
{
	size_t length = end - begin;
	char *revcomp = malloc(length + 1);
	// the revcomp of non-dna4 bases is arbitrary
	dna4_revcomp(begin, end, revcomp);
	revcomp[length] = '\0'; // for debugging purposes

	// [start, stop) is an interval of only dna4 bases
	const char *start = dna4_find_first_of(dnax_to_dna4_table, begin, end);
	while (start < end - k + 1) {
		const char *stop =
			dna4_find_first_not_of(dnax_to_dna4_table, start + 1, end);

		const char *rc = revcomp + (start - begin);
		for (; start < stop - k + 1; start++, rc++) {
			int cmp = memcmp(start, rc, k);
			process_fn(cmp < 0 ? start : rc, k);
		}

		// if stop is end computing stop + 1 is UB.
		start = dna4_find_first_of(dnax_to_dna4_table, stop, end);
	}

	free(revcomp);
}
