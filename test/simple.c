#include <assert.h>
#include <dna.h>
#include <string.h>

int
main(int argc, char const *argv[])
{
	const char *subject = "AACGTACGT";
	const char *subject_end = subject + strlen(subject);
	const char *query = "AACGTACCT";
	const char *query_end = query + strlen(query);

	size_t mismatches = dna4_count_mismatches(subject, subject_end, query);
	assert(mismatches == 1);

	char *rcsubject = strdup(subject);
	dna4_revcomp(subject, subject_end, rcsubject);

	char *rcquery = strdup(query);
	dnax_revcomp(dnax_revcomp_table, query, query_end, rcquery);

	const char *rcsubject_end = rcsubject + strlen(rcsubject);
	size_t rcmismatches =
		dna4_count_mismatches(rcsubject, rcsubject_end, rcquery);
	assert(rcmismatches == 1);

	return 0;
}
