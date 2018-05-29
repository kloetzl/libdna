/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 (C) Fabian Klötzl
 */

#include "utils.h"
#include <math.h>
#include <stdlib.h>

extern double log(double x) __attribute__((weak));

static float dna_utils_logf(float x);

double dna_utils_log(double x)
{
	// https://developer.apple.com/library/content/documentation/MacOSX/Conceptual/BPFrameworks/Concepts/WeakLinking.html
	// https://leondong1993.github.io/2017/04/15/strong-weak-symbol/
	if (log == NULL)
		return dna_utils_logf(x);
	else
		return log(x);
}

// https://www.ebayinc.com/stories/blogs/tech/fast-approximate-logarithms-part-i-the-basics/

static float dna_utils_logf(float x)
{
	// static const float a = -0.4326728;
	// static const float b = 2.276597;
	// static const float c = -1.843924;
	static const float a = -0.333333;
	static const float b = 2.0;
	static const float c = -1.666667;
	static const float log2_e = 1.442695;

	union {
		float f;
		unsigned int i;
	} ux1, ux2;

	// get exponent
	ux1.f = x;
	int exp = (ux1.i & 0x7F800000) >> 23;
	float fexp = exp - 127;

	// get the mantissa
	ux2.i = (ux1.i & 0x007FFFFF) | 0x3f800000;
	float mant = ux2.f;

	// approximate the logarithm
	float log2 = fexp + a * mant * mant + b * mant + c;
	return log2 / log2_e;
}
