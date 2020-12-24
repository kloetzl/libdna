# Why libdna is better than X

There are a number of great bioinformatics library out there. Some of the more popular are [Bio++](http://biopp.univ-montp2.fr) and [Seqan](https://docs.seqan.de/seqan3.html). They bundle a lot of functionality including alignment, file parsing and population genomics. In contrast, libdna is designed to only deal with DNA (and RNA). It contains only very few procedures, but the implementations are fine tuned.

*libdna is simple.* Following the UNIX principle, libdna has very few functions but implements them very well. Almost every tool that handles DNA has to compute the reverse complement at some point. Interestingly, computing the reverse complement properly is not as simple as one might think. libdna provides two implementations, one very fast, and one generic but still fast. Thereby libdna gives users a great basis to build their programs upon.

*libdna links dynamically.* When I get round to optimize my functions for a new CPU you and your programs users get the update without any change to your code. Easy!

# Why libdna is worse than X

*libdna is simple.* It might not have all the functionality you need. In particular it does not support every usecase anyone could ever need.

*libdna links dynamically.* Dynamic libraries are slightly harder to use as dependencies than just copying a file into your project.

