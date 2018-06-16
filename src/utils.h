/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 (C) Fabian Klötzl
 */

#pragma once

double dna_utils_log(double);

#define UNLIKELY(X) __builtin_expect((X), 0)
#define LIKELY(X) __builtin_expect((X), 1)
