/*
parser.h - jsonparse
Modified 2022-10-05
*/

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

/* Header-specific includes. */
#include <stdio.h>
#include "common.h"
#include "errors.h"

/*
Parser state.
*/

typedef struct json_parser_state_ {
  FILE *stream;
  wint_t wc;
  json_error_type error;
} json_parser_state;

/*
*** Interface.
*/

json_parser_state *json_parser_create(FILE *stream);
void json_parser_destroy(json_parser_state *ps);

void json_parser_advance(json_parser_state *ps);
void json_parse_whitespace(json_parser_state *ps);
bool json_parse_character(json_parser_state *ps, wint_t wc);
bool json_parse_literal(json_parser_state *ps, wchar_t *literal);
json_value json_parse_number(json_parser_state *ps);
wchar_t *json_parse_string(json_parser_state *ps);
json_value json_parse_value(json_parser_state *ps);
json_pair json_parse_pair(json_parser_state *ps);
json_value json_parse_array(json_parser_state *ps);
json_value json_parse_object(json_parser_state *ps);

#endif /* !JSON_PARSER_H */
