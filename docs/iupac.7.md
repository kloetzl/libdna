# iupac(7) - IUPAC codes for amino acids and nucleotide bases

LIBDNA, 2018-05-15


# Description

The International Union of Pure and Applied Chemistry (IUPAC) specifies a code for abbreviation of amino acids and nucleotide bases. The following tables show the code with the addition of Uracil. The given triplet codes are taken from the standard genetic code.


## Nucleotides

.TS
l c l l.
Mnemonic	Symbol	Meaning	Complement
_
_A_denine	A		T
_C_ytosine	C		G
_G_uanine	G		C
_T_hymine	T		A
_U_racil	U	T	A
_W_eak	W	A, T	W
_S_trong	S	C, G	S
A_m_ino	M	A, C	K
_K_eto	K	G, T	M
Pu_r_ine	R	A, G	Y
P_y_rimidine	Y	C, T	R
not A	B	C, G, T	V
not C	D	A, G, T	H
not G	H	A, C, T	D
not T	V	A, C, G	B
a_n_y	N	A, C, G, T	N



.TE


## Amino Acids

.TS
l c l.
Amino Acid	Symbol	Triplet
_
Alanine	A	GCN
Arginine	R	CGN and AGR
Asparagine	N	AAY
Aspartic acid	D	GAY
Aspartic acid or aspraragine	B	RAY
Cysteine	C	TGY
Glutamic acid	E	GAR
Glutamic acid or glutamine	Z	SAR
Glutamine	Q	CAR
Glycine	G	GGN
Histidine	H	CAY
Isoleucine	I	ATH
Leucine	L	CTN and TTR
Lysine	K	AAR
Methionine	M	ATG
Phenylalanine	F	TTY
Proline	P	CCN
Serine	S	TCN and AGY
Threonine	T	ACN
Tryptophan	W	TGG
Tyrosine	Y	TAY
Valine	V	GTN
Terminator	*	TAR and TGA
Unknown	X	NNN
.TE


# Notes


## History

The code given here was specified in _Nomenclature for incomplete specified bases in nucleic acid sequences_ Nomenclature Committee of the International Union of Biochemistry (1985). Common but non-standard extensions to the code include **X** for masked nucleotides, **-** for gaps of indeterminate lengths, **Z** for zero nucleotides and lower case letters to convey further information.


# See Also

**ascii**(7),
**charsets**(7),
**geneticcode**(7)
