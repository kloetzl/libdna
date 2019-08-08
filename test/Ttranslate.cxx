#include "catch.hpp"

#include <dna.h>
#include <string>

TEST_CASE("Some simple checks")
{
	auto mrna = std::string("AAA-AAC-AAG-AAT-ACAACCACGACT");
	char *aa = new char[mrna.size() / 3 + 10];

	char *ptr =
		dnax_translate_quirks(mrna.data(), mrna.data() + mrna.size(), aa);
	*ptr = 0;

	REQUIRE(std::string(aa) == "KNKNTTTT");

	free(aa);
}
