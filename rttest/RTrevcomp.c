#include <dna.h>
#include <stdio.h>
#include <string.h>

int
main()
{
	char str[] = "ACGT";
	char buffer[5] = {0};
	dna4_revcomp(str, str + 4, buffer);
	int check = memcmp(str, buffer, 4);
	printf("%s %s\n", str, buffer);
	return check;
}
