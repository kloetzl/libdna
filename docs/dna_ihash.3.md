# dna_ihash(3) - invertible hash function

LIBDNA, 2022-09-11

    #include <kloetzl/dna.h>
    
    uint64_t dna_ihash(uint64_t data);


## Description

Traditionally, a hash table contains hashes and the associated values. If the table is huge and the values are small, storing both hash and value can incur a memory overhead. The **dna_ihash**() invertible hash function provides a solution to this. If the hash is invertible it can be used to hold the hashed value, or at least parts thereof.

Note that, as **dna_ihash**() is invertible by design, it is not safe for cryptography. Further, it is trivial to create prefix-collisions.


## Example

    std::pair<uint64_t, std::string>
    make_key(const std::string& str) {
        uint64_t prefix = 0;
        std::size_t prefix_length = std::min(sizeof(prefix), str.size());
        std::memcpy(&prefix, str.data(), prefix_length);
        return std::make_pair(dna::ihash(prefix), str.substr(prefix_length));
    }


## See Also

[dna_ihash_invert](dna_ihash_invert.3.md)
