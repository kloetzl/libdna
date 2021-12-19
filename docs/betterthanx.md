# Why libdna is better than X

There are a number of great bioinformatics library out there. Some of the more popular are [Bio++](http://biopp.univ-montp2.fr), [Biopython](https://biopython.org) and [Seqan](https://docs.seqan.de/seqan3.html). They bundle a lot of functionality including alignment, file parsing and population genomics. In contrast, libdna is designed to only deal with DNA (and RNA). It contains only very few procedures, but the implementations are fine tuned.

*libdna is simple.* Following the UNIX principle, libdna has very few functions but implements them very well. Almost every tool that handles DNA has to compute the reverse complement at some point. Interestingly, computing the reverse complement properly is not as simple as one might think. libdna provides two implementations, one very fast, and one generic but still fast. Thereby libdna gives users a great basis to build their programs upon.

*libdna links dynamically.* Dynamic linking means that your program always uses the latest version of libdna currently installed on the machine. This has the advantage that libdna can change independently of your program code. For instance, whenever libdna receives a new optimization for a function or CPU your program gets that improvement without any code change.

*libdna is fast.* Anyone can write a function that computes the reverse complement. However, using a switch statement is really slow, roughly eight times slower than using a simple lookup table. Performance matters when one deals with DNA where a string can be millions of nucleotides long. Hence, libdna uses optimized implementations sometimes down to assembly level. Thereby libdna is much faster than other tools. For instance, `dna4_revcomp` needs just two instructions to reverse and complement sixteen characters and is thus 80 times faster than a switch based approach.


# Why libdna is worse than X

*libdna is simple.* It might not have all the functionality you need. In particular it does not support every usecase anyone could ever need.

*libdna links dynamically.* Dynamic libraries are slightly harder to use as dependencies than just copying a file into your project.

*libdna is fast.* Sometimes high performance requires compromises. For libdna that means it uses a reduced alphabet. All `dna4_*` functions are very fast but are limited to the four letters `ACGT`. As an alternative `dnax_*` functions are provided which are slightly slower but more generally applicable.
