/*
structure.c - jsonparse
Modified 2022-09-16
*/

/* Header-specific includes. */
#include "structure.h"

/* Implementation-specific includes. */
#include <assert.h>
#include <wchar.h>

/*
*** Interface.
*/

void json_value_free(json_value value)
{
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wswitch-enum"
  switch (value.type) {
    case JSON_TYPE_SIZE:
    case JSON_TYPE_NULL:
    case JSON_TYPE_BOOLEAN:
    case JSON_TYPE_INTEGER:
    case JSON_TYPE_FLOATING:
      break;
    case JSON_TYPE_STRING:
      if (value.as.string != NULL)
        free(value.as.string);
      break;
    case JSON_TYPE_OBJECT:
      if (value.as.object.pairs == NULL)
        break;
      for (size_t i=0; i<value.as.object.pair_count; i++) {
        free(value.as.object.pairs[i].key);
        json_value_free(value.as.object.pairs[i].value);
      }
      free(value.as.object.pairs);
      break;
    case JSON_TYPE_ARRAY:
      if (value.as.array == NULL)
        break;
      assert(value.as.array[0].type == JSON_TYPE_SIZE);
      for (json_integer i=0; i<1+value.as.array[0].as.integer; i++)
        json_value_free(value.as.array[i]);
      free(value.as.array);
      break;
    default:
      assert(false);
  }
  #pragma clang diagnostic pop
}

void json_value_represent(json_value value)
{
  assert(JSON_TYPE_HAS_MEANING(value.type));
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wswitch"
  switch (value.type) {
    case JSON_TYPE_NULL:
      wprintf(L"null");
      break;
    case JSON_TYPE_BOOLEAN:
      if (value.as.integer)
        wprintf(L"true");
      else
        wprintf(L"false");
      break;
    case JSON_TYPE_INTEGER:
      wprintf(JSON_WPRI_INTEGER, value.as.integer);
      break;
    case JSON_TYPE_FLOATING:
      wprintf(JSON_WPRI_FLOATING, value.as.floating);
      break;
    case JSON_TYPE_STRING:
      wprintf(L"\"" JSON_WPRI_STRING L"\"", value.as.string);
      break;
    case JSON_TYPE_ARRAY:
      wprintf(L"[");
      assert(JSON_ARRAY_IS_INTEGROUS(value));
      if (value.as.array[0].as.integer > 0) {
        json_value_represent(value.as.array[1]);
        for (json_integer i=1; i<value.as.array[0].as.integer; i++) {
          wprintf(L",");
          json_value_represent(value.as.array[1+i]);
        }
      }
      wprintf(L"]");
      break;
    case JSON_TYPE_OBJECT:
      wprintf(L"{");
      assert(JSON_OBJECT_IS_INTEGROUS(value));
      if (value.as.object.pair_count > 0) {
        json_pair_represent(value.as.object.pairs[0]);
        for (size_t i=1; i<value.as.object.pair_count; i++) {
          wprintf(L",");
          json_pair_represent(value.as.object.pairs[i]);
        }
      }
      wprintf(L"}");
      break;
  }
  #pragma clang diagnostic pop
}

void json_pair_represent(json_pair pair)
{
  assert(JSON_PAIR_IS_INTEGROUS(pair));
  wprintf(L"\"" JSON_WPRI_STRING L"\":", pair.key);
  json_value_represent(pair.value);
}
