#include <dna.h>

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
		char *ptr = dnax_find_first_mismatch(T + k + l, T + n, T + i + l);
		l = ptr - T - k - l;

		LCP[j] = l;
		l = l ? l - 1 : 0;
	}
}

int
main(int argc, char const *argv[])
{
	/* code */
	return 0;
}
