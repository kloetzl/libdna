#include <assert.h>
#include <dna.h>
#include <string>

auto
begin_data(std::string &str)
{
	return str.data();
}

auto
end_data(std::string &str)
{
	return str.data() + str.size();
}

auto
repeat(std::string in, int count)
{
	auto ret = std::string{};
	ret.reserve(in.size() * count);

	for (int i = 0; i < count; i++) {
		ret += in;
	}

	return ret;
}

int
main(int argc, char const *argv[])
{
	auto forward = repeat("ACGT", 10);

	auto buffer = new char[forward.size() + 1];

	dna4_revcomp(begin_data(forward), end_data(forward), buffer);
	assert(forward == buffer);

	dnax_revcomp(
		dnax_revcomp_table, begin_data(forward), end_data(forward), buffer);
	assert(forward == buffer);

	return 0;
}
