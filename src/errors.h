/*
errors.h - jsonparse
Modified 2022-09-16
*/

#ifndef JSON_ERRORS_H
#define JSON_ERRORS_H

/* Header-specific includes. */
#include "common.h"

/*
*** Helpers.
*/

#define JSON_ERROR_TYPE_HAS_MEANING(type) \
  (type > JSON_ERROR_none_ && type < JSON_ERROR_max_)

/*
*** Error types.
*/

typedef enum json_error_type {
  JSON_ERROR_none_,
  JSON_ERROR_FILE,
  JSON_ERROR_MEMORY,
  JSON_ERROR_OBJECTOPEN,
  JSON_ERROR_OBJECTCLOSE,
  JSON_ERROR_PAIRSEPERATOR,
  JSON_ERROR_STRINGOPEN,
  JSON_ERROR_STRINGCLOSE,
  JSON_ERROR_STRINGESCAPE,
  JSON_ERROR_VALUE,
  JSON_ERROR_ARRAYOPEN,
  JSON_ERROR_ARRAYCLOSE,
  JSON_ERROR_TRUE,
  JSON_ERROR_FALSE,
  JSON_ERROR_NULL,
  JSON_ERROR_FLOATING,
  JSON_ERROR_INTEGER,
  JSON_ERROR_max_
} json_error_type;

extern wchar_t *json_error_type_desc[];

#endif /* JSON_ERRORS_H */
