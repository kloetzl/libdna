# dnax_find_first_of(3) - find the first character of a set

LIBDNA, 2020-08-29

    #include <kloetzl/dna.h>
    
    char *dnax_find_first_of( const char *table, const char *begin, const char *end);


# Description

The **dnax\_find\_first\_of**() function checks all characters of the input string against a _table_. It returns the position of the first character having an entry unequal to -1. The input string is delimited by _begin_ and _end_ (exclusive).


# Return Value

The return value points to the first character of the set.


# Example

    const char str[] = "ACGT-AA";
    char table[256] = {0};
    memset(table, -1, 256);
    table['-'] = 2;
    
    const char *gap = dnax_find_first_of(table, str, str + 7);
    assert(gap - str == 5);


# See Also

**strchr**(3),
**dnax_find_first_not_of**(3)
