/*
structure.h - jsonparse
Modified 2022-09-16
*/

#ifndef JSON_STRUCTURE_H
#define JSON_STRUCTURE_H

/* Header-specific includes. */
#include "common.h"
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>

/*
Helpers.
*/

#define JSON_TYPE_HAS_MEANING(type) \
  (type > JSON_TYPE_none_ && type < JSON_TYPE_max_)

#define JSON_ARRAY_IS_INTEGROUS(value) \
  (value.type == JSON_TYPE_ARRAY && value.as.array != NULL && value.as.array[0].type == JSON_TYPE_SIZE && value.as.array[0].as.integer >= 0)

#define JSON_OBJECT_IS_INTEGROUS(value) \
  (value.type == JSON_TYPE_OBJECT && value.as.object.pairs != NULL)

#define JSON_PAIR_IS_INTEGROUS(pair) \
  (pair.key != NULL && JSON_TYPE_HAS_MEANING(pair.value.type))

/*
JSON data types and interface.
*/

#define JSON_WPRI_INTEGER L"%" PRId64
#define JSON_WPRI_FLOATING L"%g"
#define JSON_WPRI_STRING L"%ls"

#include "structure_public.h"

void json_pair_represent(json_pair pair);

#endif /* JSON_STRUCTURE_H */
