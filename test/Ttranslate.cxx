#include "Tcommon.h"
#include "catch.hpp"

#include <dna.h>
#include <string>

TEST_CASE("Some simple checks")
{
	auto mrna = std::string("AAA-AAC-AAG-AAT-ACAACCACGACT");
	char *aa = new char[mrna.size() / 3 + 10];

	char *ptr =
		dnax_translate(mrna.data(), mrna.data() + mrna.size(), aa);
	*ptr = 0;

	REQUIRE(std::string(aa) == "KNKNTTTT");

	delete[] aa;
}

TEST_CASE("Compact table")
{
	// the following table is not exhaustive.
	auto table = std::vector<std::tuple<std::string, std::string, std::string>>{
		{"Alanine", "A", "GCN"},
		{"Arginine", "R", "CGN"},
		{"Arginine", "R", "AGR"},
		{"Asparagine", "N", "AAY"},
		{"Aspartic acid", "D", "GAY"},
		{"Aspartic acid or aspraragine", "B", "RAY"},
		{"Cysteine", "C", "TGY"},
		{"Glutamic acid", "E", "GAR"},
		{"Glutamic acid or glutamine", "Z", "SAR"},
		{"Glutamine", "Q", "CAR"},
		{"Glycine", "G", "GGN"},
		{"Histidine", "H", "CAY"},
		{"Isoleucine", "I", "ATH"},
		{"Leucine", "L", "CTN"},
		{"Leucine", "L", "TTR"},
		{"Lysine", "K", "AAR"},
		{"Methionine", "M", "ATG"},
		{"Phenylalanine", "F", "TTY"},
		{"Proline", "P", "CCN"},
		{"Serine", "S", "TCN"},
		{"Serine", "S", "AGY"},
		{"Threonine", "T", "ACN"},
		{"Tryptophan", "W", "TGG"},
		{"Tyrosine", "Y", "TAY"},
		{"Valine", "V", "GTN"},
		{"Terminator", "*", "TAR"},
		{"Terminator", "*", "TGA"},
		{"Unknown", "X", "NNN"}};

	for (const auto &code : table) {
		char buffer[4];
		auto amino_acid_end = dnax_translate(
			dna::begin(std::get<2>(code)), dna::end(std::get<2>(code)), buffer);
		auto amino_acid = std::string(buffer, amino_acid_end - buffer);
		REQUIRE(amino_acid == std::get<1>(code));
	}
}
