/*
errors.c - jsonparse
Modified 2022-09-16
*/

/* Header-specific includes. */
#include "errors.h"

/*
*** Error types.
*/

wchar_t *json_error_type_desc[] = {
  L"File not found.",
  L"Out of memory.",
  L"Expected '{' at start of object.",
  L"Expected '}' at end of object.",
  L"Expected ':' after key.",
  L"Expected '\"' at start of string or key.",
  L"Expected '\"' at end of string or key.",
  L"Illegal escape sequence in string.",
  L"Illegal value.",
  L"Expected '[' at start of array.",
  L"Expected ']' at end of array.",
  L"Expected 'true'.",
  L"Expected 'false'.",
  L"Expected 'null'.",
  L"Malformed floating-point number.",
  L"Malformed integer number.",
};
