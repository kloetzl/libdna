#include <cassert>
#include <cstdint>
#include <dna.hpp>
#include <iostream>
#include <string>

const char table[] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, 8,  7,  4,  11, -1, -1, 2,  13, -1, -1, 3,
	-1, 12, 15, -1, -1, -1, 10, 6,  1,  1,  14, 9,  -1, 5,  -1, -1, -1, -1, -1,
	-1, -1, 8,  7,  4,  11, -1, -1, 2,  13, -1, -1, 3,  -1, 12, 15, -1, -1, -1,
	10, 6,  1,  1,  14, 9,  -1, 5,  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1};

int16_t
pack_triplet(const char str[3])
{
	int16_t ret = table[(unsigned char)str[0]];
	ret <<= 4;
	ret |= table[(unsigned char)str[1]];
	ret <<= 4;
	ret |= table[(unsigned char)str[2]];

	return ret;
}

int
main()
{
	char triplet[4] = {0};

	auto tcag = std::string("TCAG");
	auto table = std::string();
	auto prefix = std::string("   ");
	auto header = std::string("T C A G \n\n");

	table += prefix + header;

	for (char n0 : tcag) {
		for (size_t i = 0; i < tcag.size(); i++) {
			char n2 = tcag[i];
			table += i == 1 ? std::string(1, n0) + "  " : prefix;
			for (char n1 : tcag) {
				triplet[0] = n0;
				triplet[1] = n1;
				triplet[2] = n2;

				auto aa = dnax::translate(triplet);
				table += aa + ' ';
			}
			table += std::string("  ") + n2 + '\n';
		}
		table += '\n';
	}

	std::cout << table;

	char index[4097] = {0};
	auto nucls = std::string("acgtWSMKRYBDHVN");
	for (char n0 : nucls) {
		for (char n1 : nucls) {
			for (char n2 : nucls) {
				triplet[0] = n0;
				triplet[1] = n1;
				triplet[2] = n2;
				auto t = pack_triplet(triplet);
				assert(t >= 0 && t < 4096);
				index[t] = dnax::translate(triplet)[0];
				// std::cerr << t << dnax::translate(triplet) << std::endl;
			}
		}
	}

	std::string tab;
	tab.reserve(4096);
	for (int16_t i = 0; i < 4096; i++) {
		if (index[i]) {
			tab += index[i];
		} else {
			tab += "\\0";
		}
	}
	std::cerr << tab << std::endl;
}
