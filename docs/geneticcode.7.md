# geneticcode(7) - the standard genetic code

LIBDNA, 2018-05-15


## Description

The standard genetic code specifies how triplets of nucleotides are translated into amino acids for protein construction. This code is universal in that it is used by almost all living beings.


### Table

The first base of a triplet determines the block, the second base the column and the third base the row. Stop codons are depicted by a star ‘\*’.

|     |**U**|**C**|**A**|**G**| |
|----:|-----|-----|-----|-----|:--  
|     | Phe | Ser | Tyr | Cys | **U**
|**U**| Phe | Ser | Tyr | Cys | **C**
|     | Leu | Ser |  *  |  *  | **A**
|     | Leu | Ser |  *  | Try | **G**
|-----|-----|-----|-----|-----|---
|     | Leu | Pro | His | Arg | **U**
|**C**| Leu | Pro | His | Arg | **C**
|     | Leu | Pro | Glu | Arg | **A**
|     | Leu | Pro | Glu | Arg | **G**
|-----|-----|-----|-----|-----|---
|     | Ile | Thr | Asn | Ser | **U** |
|**A**| Ile | Thr | Asn | Ser | **C** |
|     | Ile | Thr | Lys | Arg | **A** |
|     | Met | Thr | Lys | Arg | **G** |
|-----|-----|-----|-----|-----|---
|     | Val | Ala | Asp | Gly | **U**
|**G**| Val | Ala | Asp | Gly | **C**
|     | Val | Ala | Glu | Gly | **A**
|     | Val | Ala | Glu | Gly | **G**


## Notes


### References

Nirenberg M, Leder P, Bernfield M, et al. “RNA codewords and protein synthesis, VII. On the general nature of the RNA code.” _Proceedings of the National Academy of Sciences of the United States of America._ 1965;53(5):1161-1168.


## See Also

[iupac](iupac.7.md)(7)
