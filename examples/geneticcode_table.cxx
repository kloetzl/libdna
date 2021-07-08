#include <dna>
#include <iostream>
#include <string>

int main()
{
	char triplet[4] = {0};

	auto tcag = std::string("TCAG");
	auto table = std::string();
	auto prefix = std::string("   ");
	auto header = std::string("T C A G \n\n");

	table += prefix + header;

	for (char n0: tcag) {
		for (size_t i = 0; i < tcag.size(); i++) {
			char n2 = tcag[i];
			table += i == 1 ? std::string(1, n0) + "  " : prefix;
			for (char n1: tcag) {
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
}
