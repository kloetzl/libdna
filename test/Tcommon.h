#pragma once

#include <dna.hpp>
#include <string>
#include <unordered_map>

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

namespace chi2
{
enum class randomness { fine, suspect, reject };

std::string
stringify(randomness value)
{
	if (value == chi2::randomness::fine) return "fine";
	if (value == chi2::randomness::suspect) return "suspect";
	if (value == chi2::randomness::reject) return "reject";
	return "unkown";
}

template <typename T, typename I>
randomness
equi(I begin, I end, const std::vector<T> &categories)
{
	if (categories.size() < 2) return randomness::reject;

	std::unordered_map<T, size_t> counts;
	for (; begin < end; ++begin) {
		counts[*begin]++;
	}

	double Y = 0.0;
	double N = 0.0;
	for (auto [key, count] : counts) {
		Y += count * count;
		N += count;
	}

	double V = Y * categories.size() / N - N;

	auto thresholds = std::vector<std::tuple<double, double, double>>{
		{0.00016, 3.841, 6.635}, {0.02010, 5.991, 9.210},
		{0.1148, 7.815, 11.34}, // 3
		{0.0, 0.0, 0.0},         {0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0},         {0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0},         {0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0},         {0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0},         {0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0},         {5.229, 25.00, 30.58}};

	if (thresholds.size() + 2 < categories.size()) return randomness::reject;

	auto k = categories.size() - 2;
	auto [toogood, weak, bad] = thresholds[k];
	if (V < toogood) return randomness::suspect;
	if (V > bad) return randomness::reject;
	if (V > weak) return randomness::suspect;
	return randomness::fine;
}
} // namespace chi2
