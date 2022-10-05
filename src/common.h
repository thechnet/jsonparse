/*
common.h - jsonparse
Modified 2022-10-05
*/

#ifndef JSON_COMMON_H
#define JSON_COMMON_H

/* Debugging. */
#if !defined(NDEBUG) && defined(JSON_DEBUG_ENABLE_WATCHDOG)
#define WATCHDOG
#endif

/* Header-specific includes. */
#include <stdbool.h>
#include "watchdog.h"
#include "structure.h"

#endif /* !JSON_COMMON_H */
