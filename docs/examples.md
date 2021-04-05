# Process canonical kmers

Given a long genome, do something with the canonical kmers of length K. Assume the genome is AGTNAGCC. The first 3mer is AGT. However, it is not canonical, because its reverse complement is lexicographically smaller ACT < AGT. So ACT is passed to the callback. The next 3mer GTN is invalid as N is an ambiguous base. Thus processing continues beyond that position.

```C++
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
```

# Compute GC content

This code shows how to compute the GC content of a sequence. Note that it counts Cs, Gs, and Ss that is either G or C. Also lower case letters are allowed.

```C++
#include <kloetzl/dna>

double
gc_content(const std::string &str)
{
	auto counts = dna::count(str);
	auto gc = counts['G'] + counts['C'] + counts['S'] + counts['g'] +
			  counts['c'] + counts['s'];
	return (double)gc / str.size();
}
```

# Compute LCP array using Kasai algorithm

This algorithm computes the LCP array using the method from Kasai et al.

```C++
#include <kloetzl/dna.h>

void
kasai(const char *T, const int *SA, const int *ISA, size_t n, int *LCP)
{
	int k = 0;
	LCP[0] = -1;
	LCP[n] = -1;

	int l = 0;

	for (int i = 0; i < n; i++) {
		int j = ISA[i];
		if (j <= 0) continue;

		k = SA[j - 1];
		char *ptr = dna4_first_mismatch(T + k + l, T + i + l);
		l = ptr - T - k - l;

		LCP[j] = l;
		l = l ? l - 1 : 0;
	}
}
```
