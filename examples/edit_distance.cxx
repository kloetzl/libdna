#include <assert.h>
#include <dna.hpp>
#include <string_view>
#include <vector>

size_t
edit_distance(std::string_view s1, std::string_view s2, ssize_t max)
{
	std::vector<size_t> v(max * 2 + 2);
	auto V = std::begin(v) + max + 1;

	for (ssize_t D = 0; D <= max; D++) {
		for (ssize_t k = -D; k <= D; k += 2) {
			size_t x;
			if (k == -D || (k != D && V[k - 1] < V[k + 1])) {
				x = V[k + 1];
			} else {
				x = V[k - 1] + 1;
			}
			auto y = x - k;
			auto mm = dnax::find_first_mismatch(s1.substr(x), s2.substr(y));
			V[k] = x + mm;
			if (x + mm >= s1.size() || y + mm >= s2.size()) {
				return D;
			}
		}
	}

	return max + 1;
}

#include <iostream>

int
main(int argc, char **argv)
{
	std::string s1 = argc >= 2 ? argv[1] : "AATCC";
	std::string s2 = argc >= 3 ? argv[2] : "GATCC";
	std::cerr << s1 << ' ' << s2 << std::endl;

	auto ed = edit_distance(s1, s2, 10);
	printf("%zu\n", ed);
}
