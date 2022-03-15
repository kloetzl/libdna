#include <dna.hpp>

double
gc_content(const std::string &str)
{
	auto counts = dnax::count(str);
	auto gc = counts['G'] + counts['C'] + counts['S'] + counts['g'] +
			  counts['c'] + counts['s'];
	return (double)gc / str.size();
}

int
main()
{
	double gc = gc_content("ACGT");
	return gc != 0.5; // 0 means success
}
