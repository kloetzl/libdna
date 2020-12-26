/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 - 2020 (C) Fabian Klötzl
 */

#pragma once

#define UNLIKELY(X) __builtin_expect((X), 0)
#define LIKELY(X) __builtin_expect((X), 1)
