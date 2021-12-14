#include <kloetzl/dna>

double
gc_content(const std::string &str)
{
	auto counts = dnax::count(str);
	auto gc = counts['G'] + counts['C'] + counts['S'] + counts['g'] +
			  counts['c'] + counts['s'];
	return (double)gc / str.size();
}
