# Generate Random FASTA Sequence

Generating random fasta sequences can be very useful for testing bioinformatics tools. The following code shows by example how to generate a random FASTA DNA sequence file. It takes two optional command line parameters, `-l` the number of bytes to generate, and `-s` and seed for reproducibility.

The output is printed to the standard output. First is the header line. It starts with a `>` followed by the sequence name `rnd`. The header line also contains the seed used to generate the sequence. On the next line is the generated DNA sequence, consisting entirely of ACGTs.

```C
#include <dna.h>
#include <err.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int
main(int argc, char *const *argv)
{
	size_t length = 80;
	uint32_t seed = getpid() ^ time(NULL);

	// parse arguments
	int opt;
	while ((opt = getopt(argc, argv, "l:s:")) != -1) {
		if (opt == 'l') {
			length = strtoul(optarg, NULL, 10);
		} else if (opt == 's') {
			seed = strtoul(optarg, NULL, 10);
		} else {
			errx(EXIT_FAILURE, "Usage: %s [-l length] [-s seed]\n", argv[0]);
		}
	}


	// allocate and generate random sequence
	char *buffer = malloc(length);
	if (!buffer) err(EXIT_FAILURE, NULL);

	dna4_fill_random(buffer, buffer + length, seed);

	// print output in FASTA format
	dprintf(STDOUT_FILENO, ">rnd seed=%" PRIu32 "\n", seed);
	write(STDOUT_FILENO, buffer, length);
	dprintf(STDOUT_FILENO, "\n");

	free(buffer);

	return 0;
}
```
