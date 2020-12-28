/**
 * SPDX-License-Identifier: MIT
 * Copyright 2018 - 2020 (C) Fabian Klötzl
 */

#pragma once

#define UNLIKELY(X) __builtin_expect((X), 0)
#define LIKELY(X) __builtin_expect((X), 1)
