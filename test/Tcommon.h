#pragma once

#include <dna>
#include <string>

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
