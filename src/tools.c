/*
tools.c - jsonparse
Modified 2022-09-16
*/

/* Header-specific includes. */
#include "tools.h"

/* Implementaton-specific includes. */
#include <wchar.h>

/*
Convert a wchar_t string into a json_integer integer.
Extracted from thechnet/une 0.9.1 codebase, adapted.
*/
bool wcs_to_json_integer(wchar_t *wcs, json_integer *dest)
{
  wchar_t *wcs_end = wcs+wcslen(wcs);
  wchar_t *int_end;
  *dest = wcstoll(wcs, &int_end, 10);
  return int_end == wcs_end ? true : false;
}

/*
Convert a wchar_t string into a une_flt floating pointer number.
Extracted from thechnet/une 0.9.1 codebase, adapted.
*/
bool wcs_to_json_floating(wchar_t *wcs, json_floating *dest)
{
  wchar_t *wcs_end = wcs+wcslen(wcs);
  wchar_t *flt_end;
  *dest = wcstod(wcs, &flt_end);
  return flt_end == wcs_end ? true : false;
}
