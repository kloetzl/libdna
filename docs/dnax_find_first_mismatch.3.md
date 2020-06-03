# dnax_find_first_mismatch(3) - find the first mismatching character

LIBDNA, 2020-06-03 "LIBDNA"

    #include <dna.h>
    
    char *dnax_find_first_mismatch(const char *begin, const char *end, char *other);


## Description

The **dnax_find_first_mismatch**() function compares two strings and returns the position of the first mismatching character. One input string is delimited by _begin_ and _end_ (exclusive). The other is simply given by its beginning _other_.


## Return Value

The return value points to the mismatching character, or to _end_ if no such character exists.


## Example

    const char str1[] = "ACGGWaA";
    const char str2[] = "ACGGWAAT";

    const char *ptr = dnax_find_first_mismatch(str1, str1 + 7, str2);
    assert(ptr - str1 == 5);
    printf("common prefix: %.*s\n", (int)(ptr - str1), str1);


## See Also

**memcmp**(3),
**strcmp**(3)
