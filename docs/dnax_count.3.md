# dnax_count(3) - count characters

LIBDNA, 2020-06-03

    #include <dna.h>
    
    void dnax_count(size_t *table, const char *begin, const char *end);


# Description

The **dnax\_count**() function counts the occurences of characters and stores them in the given _table_. The input string is delimited by _begin_ and _end_ (exclusive).


# Example

    double gc_content(const char *begin, const char *end)
    {
    	size_t table[256] = {0};
    	dnax_count(table, begin, end);
    	double gc = table['G'] + table['C'] + table['g'] + table['c'];
    	return gc / (end - begin);
    }
