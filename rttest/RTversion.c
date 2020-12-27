#include <kloetzl/dna.h>
#include <stdio.h>

int
main()
{
	int v = dna_version();
	printf("%d\n", v);
	return 0;
}
