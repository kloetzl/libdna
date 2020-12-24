# dnax_replace(3) - replace characters

LIBDNA, 2020-06-08

    #include <dna.h>
    
    char *dnax_replace(const char *table, const char *begin, const char *end, char *dest);


# Description

The **dnax\_replace**() function replaces characters according to the given table. The input string is delimited by _begin_ and _end_ (exclusive). The result is stored at _dest_.

The _table_ parameter can be used to customize the replacement of individual characters. A value of -1 can be used to indicate a character which should be skipped. Thus, the resulting string may be shorter than the input. LibDNA comes with _dnax\_to\_dna4\_table_ which can be used to convert RNA and non-canonical DNA to DNA4.


# Return Value

The return value points one byte past the last character written. You may want to write a null byte there.


# Example

    char in[] = "AaCGT";
    char out[6] = {0};

    char table[256] = {};
    memset(table, -1, 256);
    table['A'] = 'a';
    table['C'] = 'c';
    table['G'] = 'g';
    table['T'] = 't';

    char *end = dnax_replace(table, in, in + 5, out);
    *end = '\\0';

    assert(strncmp(out, "acgt", 4) == 0);


# See Also

**iupac**(7),
**dnax_count**(3)
