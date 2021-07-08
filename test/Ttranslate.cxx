#include "Tcommon.h"

#include <catch2/catch.hpp>
#include <cstring>
#include <dna.h>
#include <string>

template <typename FN>
std::string
make_small_table(FN translate)
{
	auto table = std::string();

	char triplet[4] = {0};
	auto tcag = std::string("TCAG");

	for (char n0 : tcag) {
		for (char n2 : tcag) {
			for (char n1 : tcag) {
				triplet[0] = n0;
				triplet[1] = n1;
				triplet[2] = n2;

				char aa;
				translate(triplet, triplet + 3, &aa);
				table += aa;
			}
			table += '\n';
		}
	}

	return table;
}

static std::string ref_small_table = {
	"FSYC\n" //
	"FSYC\n" //
	"LS**\n" //
	"LS*W\n" //
	//
	"LPHR\n" //
	"LPHR\n" //
	"LPQR\n" //
	"LPQR\n" //
	//
	"ITNS\n" //
	"ITNS\n" //
	"ITKR\n" //
	"MTKR\n" //
	//
	"VADG\n" //
	"VADG\n" //
	"VAEG\n" //
	"VAEG\n" //
};

TEST_CASE("Some simple checks")
{
	auto mrna = std::string("AAA-AAC-AAG-AAT-ACAACCACGACT");
	char *aa = new char[mrna.size() / 3 + 10];

	char *ptr = dnax_translate(mrna.data(), mrna.data() + mrna.size(), aa);
	*ptr = 0;

	REQUIRE(std::string(aa) == "KNKNTTTT");

	INFO("No triplet")
	REQUIRE(dnax::translate("A---A") == "");

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

		INFO(std::get<2>(code));
		REQUIRE(amino_acid == std::get<1>(code));
	}
}

TEST_CASE("Small table")
{
	std::string small_table = make_small_table(dnax_translate);
	REQUIRE(ref_small_table == small_table);
}
