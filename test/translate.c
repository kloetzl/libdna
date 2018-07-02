#include "util.h"
#include <dna.h>
#include <glib.h>
#include <string.h>
#include <stdlib.h>

void translate_all()
{
	char *mrna = strdup("AAA-AAC-AAG-AAT-ACAACCACGACT");
	char *aa = malloc(strlen(mrna)/ 3 + 10);

	char *ptr = dnax_translate_quirks(mrna, end(mrna), aa);
	*ptr = 0;

	g_assert_cmpstr(aa, ==, "KNKNTTTT");

	free(mrna);
	free(aa);
}

int main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);
	g_test_add_func("/translate/all", translate_all);

	return g_test_run();
}
