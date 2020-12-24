# C++ API

```C
#include <dna>
```

The C++17 API is a thin wrapper around the C functions. While the old functions are still available, it sorts them into namespaces and provides more convenient versions. Below are the prototypes of these functions. The `dna` namespace further provides a few utility functions for handling strings.

## DNA

```C++
namespace dna
{
	char *begin(std::string &str);
	char *end(std::string &str);
	const char *begin(const std::string_view &str);
	const char *end(const std::string_view &str);
	const char *cbegin(const std::string_view &str);
	const char *cend(const std::string_view &str);
	int version();
}
```

## DNA4

```C++
namespace dna4
{
	size_t count_mismatches(const std::string_view &s1, const std::string_view &s2);
	std::string revcomp(const std::string_view &str);
	uint64_t pack_2bits(size_t k, const std::string_view &str);
	std::string unpack_2bits(size_t k, uint64_t packed);
}
```

## DNAX

```C++
namespace dnax
{
	auto pack_4bits(const std::string_view &str);
	auto unpack_4bits(const std::vector<unsigned char> &vec);
	std::string revcomp(const char *table, const std::string_view &str);
	std::string translate(const std::string_view &str);
	std::array<size_t, 256> count(const std::string_view &str);
	std::string extract_dna4(const std::string_view &str);
	std::string replace(const char *table, const std::string_view &str);
	std::string_view::size_type find_first_mismatch(const std::string_view &s1, const std::string_view &s2);
	std::string_view::size_type find_first_of(const char *table, const std::string_view &str);
	std::string_view::size_type find_first_not_of(const char *table, const std::string_view &str);
}
```
