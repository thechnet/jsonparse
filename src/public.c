/*
public.c - jsonparse
Modified 2022-09-17
*/

/* Implementation-specific includes. */
#include "common.h"
#include "structure.h"
#include "errors.h"
#include "parser.h"
#include <assert.h>

/*
*** Interface.
*/

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-prototypes"

json_value json_parse_file(wchar_t *path)
{
  /* Prepare. */
  json_value value = {
    .type = JSON_TYPE_ERROR,
    .as.integer = JSON_ERROR_FILE
  };
  
  /* Parse. */
  if (path == NULL)
    return value;
  json_parser_state *ps = json_parser_create(path);
  if (ps == NULL) {
    value.as.integer = JSON_ERROR_MEMORY;
    json_parser_destroy(ps);
    return value;
  }
  value = json_parse_object(ps);
  if (ps->error != JSON_ERROR_none_) {
    value = (json_value){
      .type = JSON_TYPE_ERROR,
      .as.integer = ps->error
    };
    json_parser_destroy(ps);
    return value;
  }
  json_parse_whitespace(ps);
  assert(ps->wc == WEOF);
  json_parser_destroy(ps);
  
  return value;
}

void json_print_error(json_value value)
{
  if (value.type != JSON_TYPE_ERROR || !JSON_ERROR_TYPE_HAS_MEANING(value.as.integer))
    return;
  wprintf(L"\33[0m\33[91mError (jsonparse): %ls\33[0m\n", json_error_type_desc[value.as.integer-1]);
}

#pragma clang diagnostic pop
