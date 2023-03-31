/**
 * SPDX-License-Identifier: MIT
 * Copyright 2018 - 2023 (C) Fabian Klötzl
 */

#include "dna_internal.h"

// For compatibility with v1.3 and before
// This declaration has to precede the definition in dna.h.
DNA_PUBLIC
extern char *
dnax_find_first_of(const char *table, const char *begin, const char *end);

#include "dna.h"
