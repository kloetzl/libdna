#include <kloetzl/dna>
#include <stdio.h>

int
main()
{
	int v = dna::version();
	printf("%d\n", v);
	return 0;
}
