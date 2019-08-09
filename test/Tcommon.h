#pragma once

#include <string>

namespace dna
{
char *
begin(std::string &str)
{
	// work around missing non-cost .data() in C++ < C++17
	return (char *)str.data();
}

char *
end(std::string &str)
{
	return begin(str) + str.size();
}

const char *
begin(const std::string &str)
{
	return str.data();
}

const char *
end(const std::string &str)
{
	return begin(str) + str.size();
}
} // namespace dna
