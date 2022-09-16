/*
common.h - jsonparse
Modified 2022-09-16
*/

#ifndef JSON_TOOLS_H
#define JSON_TOOLS_H

/* Header-specific includes. */
#include "common.h"
#include "structure.h"

/*
*** Helpers.
*/

bool wcs_to_json_integer(wchar_t *wcs, json_integer *dest);
bool wcs_to_json_floating(wchar_t *wcs, json_floating *dest);

#endif /* !JSON_TOOLS_H */
