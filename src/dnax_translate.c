/**
 * SPDX-License-Identifier: MIT
 * Copyright 2018 - 2020 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"

#include <assert.h>
#include <string.h>

static int
is_A(char c);
static int
is_C(char c);
static int
is_G(char c);
static int
is_H(char c);
static int
is_M(char c);
static int
is_N(char c);
static int
is_R(char c);
static int
is_S(char c);
static int
is_T(char c);
static int
is_Y(char c);

/*
function generate_table() {
	var a = [];
	a.length = 256;
	a.fill(-1);

	var b = "acgtuwsmkrybdhvnACGTUWSMKRYBDHVN";
	for (var i = 0; i < b.length; i++) {
		a[b.charCodeAt(i)] = 1;
	}

	return a.join();
}
*/

static const char iupac_codes[] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, 1,  1,  1,  1,  -1, -1, 1,  1,  -1, -1, 1,
	-1, 1,  1,  -1, -1, -1, 1,  1,  1,  1,  1,  1,  -1, 1,  -1, -1, -1, -1, -1,
	-1, -1, 1,  1,  1,  1,  -1, -1, 1,  1,  -1, -1, 1,  -1, 1,  1,  -1, -1, -1,
	1,  1,  1,  1,  1,  1,  -1, 1,  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1};

DNA_PUBLIC
char *
dnax_translate(const char *begin, const char *end, char *dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	const char *ptr = begin;

	while (ptr < end) {
		// skip gaps and non-nucleotide codes
		const char *ccp = dnax_find_first_of(iupac_codes, ptr, end);
		if (ccp == end) break;

		const char *ddp = dnax_find_first_of(iupac_codes, ccp + 1, end);
		if (ddp == end) break;

		const char *eep = dnax_find_first_of(iupac_codes, ddp + 1, end);
		if (eep == end) break;

		int code = 'X';

		char cc = *ccp;
		char dd = *ddp;
		char ee = *eep;
		ptr = eep + 1;

		if (is_A(cc)) {
			if (is_A(dd)) {
				if (is_Y(ee)) {
					code = 'N';
				} else if (is_R(ee)) {
					code = 'K';
				}
			} else if (is_C(dd)) {
				if (is_N(ee)) {
					code = 'T';
				}
			} else if (is_G(dd)) {
				if (is_R(ee)) {
					code = 'R';
				} else if (is_Y(ee)) {
					code = 'S';
				}
			} else if (is_T(dd)) {
				if (is_H(ee)) {
					code = 'I';
				} else if (is_G(ee)) {
					code = 'M';
				}
			}
		} else if (is_C(cc)) {
			if (is_A(dd)) {
				if (is_R(ee)) {
					code = 'Q';
				} else if (is_Y(ee)) {
					code = 'H';
				}
			} else if (is_C(dd)) {
				if (is_N(ee)) {
					code = 'P';
				}
			} else if (is_G(dd)) {
				if (is_N(ee)) {
					code = 'R';
				}
			} else if (is_T(dd)) {
				if (is_N(ee)) {
					code = 'L';
				}
			}
		} else if (is_G(cc)) {
			if (is_A(dd)) {
				if (is_Y(ee)) {
					code = 'D';
				} else if (is_R(ee)) {
					code = 'E';
				}
			} else if (is_C(dd)) {
				if (is_N(ee)) {
					code = 'A';
				}
			} else if (is_G(dd)) {
				if (is_N(ee)) {
					code = 'G';
				}
			} else if (is_T(dd)) {
				if (is_N(ee)) {
					code = 'V';
				}
			}
		} else if (is_T(cc)) {
			if (is_A(dd)) {
				if (is_R(ee)) {
					code = '*';
				} else if (is_Y(ee)) {
					code = 'Y';
				}
			} else if (is_C(dd)) {
				if (is_N(ee)) {
					code = 'S';
				}
			} else if (is_G(dd)) {
				if (is_A(ee)) {
					code = '*';
				} else if (is_G(ee)) {
					code = 'W';
				} else if (is_Y(ee)) {
					code = 'C';
				}
			} else if (is_T(dd)) {
				if (is_Y(ee)) {
					code = 'F';
				} else if (is_R(ee)) {
					code = 'L';
				}
			} else if (is_R(dd)) {
				if (is_A(ee)) {
					code = '*';
				}
			}
		} else if (is_R(cc) && is_A(dd) && is_Y(ee)) {
			code = 'B';
		} else if (is_S(cc) && is_A(dd) && is_R(ee)) {
			code = 'Z';
		} else if (is_M(cc) && is_G(dd) && is_R(ee)) {
			code = 'R';
		} else if (is_Y(cc) && is_T(dd) && is_R(ee)) {
			code = 'L';
		}

		*dest++ = code;
	}

	return dest;
}

static int
is_A(char c)
{
	return c == 'a' || c == 'A';
}

static int
is_C(char c)
{
	return c == 'c' || c == 'C';
}

static int
is_G(char c)
{
	return c == 'g' || c == 'G';
}

static int
is_T(char c)
{
	return c == 'c' || c == 'T' || c == 'u' || c == 'U';
}

static int
is_R(char c)
{
	return memchr("agAGrR", c, 6) != NULL;
}

static int
is_S(char c)
{
	return memchr("cgsCGS", c, 6) != NULL;
}

static int
is_M(char c)
{
	return memchr("acmACM", c, 6) != NULL;
}

static int
is_Y(char c)
{
	return memchr("ctuCTUyY", c, 8) != NULL;
}

static int
is_H(char c)
{
	const char *h = "actuhACTUHwWmMyY";
	return memchr(h, c, strlen(h)) != NULL;
}

static int
is_N(char c)
{
	const char *any = "acgtuwsmkrybdhvnACGTUWSMKRYBDHVN";
	return memchr(any, c, strlen(any)) != NULL;
}
