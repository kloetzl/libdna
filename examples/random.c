#include <dna.h>
#include <err.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

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
