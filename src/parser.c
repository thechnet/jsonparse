/*
parser.c - jsonparse
Modified 2022-09-17
*/

/* Header-specific includes. */
#include "parser.h"

/* Implementation-specific includes. */
#include "tools.h"
#include <assert.h>
#include <wchar.h>

/* Constants. */
#define WFOPEN_RMODE L"r,ccs=UTF-8"
#define SIZE_STRING 16
#define SIZE_NUMBER 16
#define SIZE_ARRAY 16
#define SIZE_OBJECT 16

/* Helpers. */
#define CHARACTER_IS_WHITESPACE(wc) \
  (wc == L' ' || wc == L'\n' || wc == L'\r' || wc == '\t')
#define CHARACTER_IS_DIGIT(wc) \
  (wc >= L'0' && wc <= '9')

/*
*** Interface.
*/

json_parser_state *json_parser_create(wchar_t *path)
{
  /* Internal errors. */
  assert(path != NULL);
  
  /* Create parser state. */
  json_parser_state *ps = malloc(sizeof(*ps));
  if (ps == NULL)
    return NULL;
  
  /* Zero parser state. */
  *ps = (json_parser_state){
    .stream = NULL,
    .wc = WEOF,
    .error = JSON_ERROR_none_
  };
  
  /* Open file stream. */
  FILE *stream = _wfopen(path, WFOPEN_RMODE);
  if (stream == NULL) {
    ps->error = JSON_ERROR_FILE;
    return ps;
  }
  
  /* Store and prepare file stream. */
  ps->stream = stream;
  json_parser_advance(ps);
  
  /* Return parser state. This pointer acts as handle for outsiders. */
  return ps;
}

void json_parser_destroy(json_parser_state *ps)
{
  /* Internal errors. */
  assert(ps != NULL);
  
  /* Clean up file stream. */
  if (ps->stream != NULL)
    fclose(ps->stream);
  
  /* Deallocate memory. */
  free(ps);
}

void json_parser_advance(json_parser_state *ps)
{
  /* Internal errors. */
  assert(ps != NULL);
  assert(ps->stream != NULL);
  
  #ifdef JSON_PARSER_PRINT_PARSED_CHARACTERS
  wprintf(L"%c", ps->wc);
  #endif
  
  /* Get next character. */
  ps->wc = fgetwc(ps->stream);
}

void json_parse_whitespace(json_parser_state *ps)
{
  /* Internal errors. */
  assert(ps != NULL);
  assert(ps->stream != NULL);
  
  /* Advance until next non-whitespace character. */
  while (CHARACTER_IS_WHITESPACE(ps->wc))
    json_parser_advance(ps);
}

bool json_parse_character(json_parser_state *ps, wint_t wc)
{
  /* Internal errors. */
  assert(ps != NULL);
  assert(ps->stream != NULL);
  
  /* Advance if current character matches. */
  if (ps->wc != wc)
    return false;
  json_parser_advance(ps);
  return true;
}

bool json_parse_literal(json_parser_state *ps, wchar_t *literal)
{
  /* Internal errors. */
  assert(ps != NULL);
  assert(ps->stream != NULL);
  assert(literal != NULL);
  
  /* Advance past literal or return early in case of failure. */
  for (size_t i=0; i<wcslen(literal); i++)
    if (!json_parse_character(ps, literal[i]))
      return false;
  return true;
}

json_value json_parse_number(json_parser_state *ps)
{
   /* Internal errors. */
  assert(ps != NULL);
  assert(ps->stream != NULL);
  
  /* Prepare. */
  json_value value = {
    .type = JSON_TYPE_none_,
    .as.integer = 0
  };
  size_t buffer_idx = 0;
  size_t buffer_size = SIZE_NUMBER;
  wchar_t *buffer = malloc(buffer_size*sizeof(*buffer));
  if (buffer == NULL) {
    ps->error = JSON_ERROR_MEMORY;
    return value;
  }
  
  /* Read number as string. */
  do {
    /* Ensure sufficient space in buffer. */
    while (buffer_idx+1 /* NUL. */ >= buffer_size) {
      buffer_size *= 2;
      wchar_t *buffer_new = realloc(buffer, buffer_size*sizeof(*buffer));
      if (buffer_new == NULL) {
        free(buffer);
        ps->error = JSON_ERROR_MEMORY;
        return value;
      }
      buffer = buffer_new;
    }
    /* Add character to buffer. */
    buffer[buffer_idx++] = ps->wc;
    json_parser_advance(ps);
  } while (CHARACTER_IS_DIGIT(ps->wc));
  
  if (ps->wc != L'.') {
    /* Return integer. */
    buffer[buffer_idx] = L'\0';
    json_integer integer;
    if (!wcs_to_json_integer(buffer, &integer))
      ps->error = JSON_ERROR_INTEGER;
    else
      value = (json_value){
        .type = JSON_TYPE_INTEGER,
        .as.integer = integer
      };
    free(buffer);
    return value;
  }
  
  /* Try to parse floating point number. */
  
  /* Skip decimal point. */
  buffer[buffer_idx++] = ps->wc; /* '.' */
  json_parser_advance(ps);
  
  size_t idx_before_decimals = buffer_idx;
  
  /* Read decimal digits. */
  while (CHARACTER_IS_DIGIT(ps->wc)) {
    /* Ensure sufficient space in buffer. */
    while (buffer_idx+1 /* NUL. */ >= buffer_size) {
      buffer_size *= 2;
      wchar_t *buffer_new = realloc(buffer, buffer_size*sizeof(*buffer));
      if (buffer_new == NULL) {
        free(buffer);
        ps->error = JSON_ERROR_MEMORY;
        return value;
      }
      buffer = buffer_new;
    }
    /* Add character to buffer. */
    buffer[buffer_idx++] = ps->wc;
    json_parser_advance(ps);
  }
  
  /* No digits after decimal point. */
  if (buffer_idx == idx_before_decimals) {
    ps->error = JSON_ERROR_FLOATING;
    free(buffer);
    return value;
  }
  
  /* Return floating point number. */
  buffer[buffer_idx] = L'\0';
  json_floating floating;
  if (!wcs_to_json_floating(buffer, &floating))
    ps->error = JSON_ERROR_FLOATING;
  else
    value = (json_value){
      .type = JSON_TYPE_FLOATING,
      .as.floating = floating
    };
  free(buffer);
  return value;
}

wchar_t *json_parse_string(json_parser_state *ps)
{
  /* Internal errors. */
  assert(ps != NULL);
  assert(ps->stream != NULL);
  
  /* Whitespace. */
  json_parse_whitespace(ps);
  
  /* '"' */
  if (!json_parse_character(ps, L'"')) {
    ps->error = JSON_ERROR_STRINGOPEN;
    return NULL;
  }
  
  /* Allocate string. */
  size_t string_size = SIZE_STRING;
  wchar_t *string = malloc(string_size*sizeof(*string));
  if (string == NULL) {
    ps->error = JSON_ERROR_MEMORY;
    return NULL;
  }
  size_t string_idx = 0;
  
  /* 'string' */
  while (ps->wc != L'"' && ps->wc != WEOF) {
    /* Ensure buffer is big enough. */
    while (string_idx+1 >= string_size) {
      string_size *= 2;
      wchar_t *string_new = realloc(string, string_size*sizeof(*string));
      if (string_new == NULL) {
        ps->error = JSON_ERROR_MEMORY;
        free(string);
        return NULL;
      }
      string = string_new;
    }
    /* Handle character escapes. */
    if (ps->wc == L'\\') {
      json_parser_advance(ps);
      switch (ps->wc) {
        case L'"':
        case L'\\':
          break;
        case L'n':
          ps->wc = L'\n';
          break;
        default:
          ps->error = JSON_ERROR_STRINGESCAPE;
          free(string);
          return NULL;
      }
    }
    /* Accept character. */
    string[string_idx++] = ps->wc;
    json_parser_advance(ps);
  }
  /* Terminate string. */
  string[string_idx] = L'\0';
  
  /* '"' */
  if (!json_parse_character(ps, L'"')) {
    ps->error = JSON_ERROR_STRINGCLOSE;
    free(string);
    return NULL;
  }
  
  return string;
}

json_value json_parse_value(json_parser_state *ps)
{
   /* Internal errors. */
  assert(ps != NULL);
  assert(ps->stream != NULL);
  
  /* Whitespace. */
  json_parse_whitespace(ps);
  
  /* Prepare. */
  json_value value = {
    .type = JSON_TYPE_none_,
    .as.integer = 0
  };
  
  /* Determine value type and parse it. */
  switch (ps->wc) {
    case L'"':
      value = (json_value){
        .type = JSON_TYPE_STRING,
        .as.string = json_parse_string(ps)
      };
      break;
    case L't':
      if (!json_parse_literal(ps, L"true"))
        ps->error = JSON_ERROR_TRUE;
      else
        value = (json_value){
          .type = JSON_TYPE_BOOLEAN,
          .as.integer = 1
        };
      break;
    case L'f':
      if (!json_parse_literal(ps, L"false"))
        ps->error = JSON_ERROR_FALSE;
      else
        value = (json_value){
          .type = JSON_TYPE_BOOLEAN,
          .as.integer = 0
        };
      break;
    case L'n':
      if (!json_parse_literal(ps, L"null"))
        ps->error = JSON_ERROR_NULL;
      else
        value = (json_value){
          .type = JSON_TYPE_NULL,
          .as.integer = 0
        };
      break;
    case L'[':
      value = json_parse_array(ps);
      break;
    case L'{':
      value = json_parse_object(ps);
      break;
    default:
      if (ps->wc == L'-' || (ps->wc >= '0' && ps->wc <= '9')) {
        value = json_parse_number(ps);
        if (ps->error == JSON_ERROR_none_)
          break;
      }
      ps->error = JSON_ERROR_VALUE;
  }
  
  return value;
}

json_pair json_parse_pair(json_parser_state *ps)
{
   /* Internal errors. */
  assert(ps != NULL);
  assert(ps->stream != NULL);
  
  /* Prepare. */
  json_pair pair = {
    .key = NULL,
    .value = {
      .type = JSON_TYPE_none_,
      .as.integer = 0
    }
  };
  
  /* '"key"' */
  wchar_t *key = json_parse_string(ps);
  if (ps->error != JSON_ERROR_none_)
    return pair;
  
  /* Whitespace. */
  json_parse_whitespace(ps);
  
  /* ':' */
  if (!json_parse_character(ps, L':')) {
    ps->error = JSON_ERROR_PAIRSEPERATOR;
    free(key);
    return pair;
  }
  
  /* 'value' */
  json_value value = json_parse_value(ps);
  if (ps->error != JSON_ERROR_none_) {
    free(key);
    return pair;
  }
  
  pair.key = key;
  pair.value = value;
  return pair;
}

json_value json_parse_array(json_parser_state *ps)
{
   /* Internal errors. */
  assert(ps != NULL);
  assert(ps->stream != NULL);
  
  /* Prepare array. */
  size_t array_size = SIZE_ARRAY;
  size_t array_idx = 0;
  json_value value = {
    .type = JSON_TYPE_ARRAY,
    .as.array = malloc(array_size*sizeof(*value.as.array))
  };
  if (value.as.array == NULL) {
    ps->error = JSON_ERROR_MEMORY;
    return value;
  }
  
  /* Reserve space to declare array size. */
  assert(array_size >= 1);
  value.as.array[array_idx++] = (json_value){
    .type = JSON_TYPE_SIZE,
    .as.integer = 0
  };
  
  /* Whitespace. */
  json_parse_whitespace(ps);
  
  /* '[' */
  if (!json_parse_character(ps, L'[')) {
    ps->error = JSON_ERROR_ARRAYOPEN;
    return value;
  }
  
  /* Whitespace. */
  json_parse_whitespace(ps);
  
  /* Items. */
  json_value item;
  while (ps->wc != L']') {
    /* Ensure array is big enough. */
    while (array_idx >= array_size) {
      array_size *= 2;
      json_value *array_new = realloc(value.as.array, array_size*sizeof(*value.as.array));
      if (array_new == NULL) {
        ps->error = JSON_ERROR_MEMORY;
        json_value_free(value);
        return value;
      }
      value.as.array = array_new;
    }
    /* Parse value. */
    item = json_parse_value(ps);
    if (ps->error != JSON_ERROR_none_) {
      json_value_free(value);
      return value;
    }
    value.as.array[0].as.integer = (json_integer)array_idx;
    value.as.array[array_idx++] = item;
    /* Whitespace. */
    json_parse_whitespace(ps);
    /* Stop if continuation is not announced. */
    if (!json_parse_character(ps, L','))
      break;
  }
  
  /* ']' */
  if (!json_parse_character(ps, L']')) {
    ps->error = JSON_ERROR_ARRAYCLOSE;
    json_value_free(value);
  }
  
  return value;
}

json_value json_parse_object(json_parser_state *ps)
{
   /* Internal errors. */
  assert(ps != NULL);
  assert(ps->stream != NULL);
  
  /* Prepare object. */
  size_t object_size = SIZE_OBJECT;
  size_t object_idx = 0;
  json_value value = {
    .type = JSON_TYPE_OBJECT,
    .as.object = (json_object){
      .pairs = malloc(object_size*sizeof(*value.as.object.pairs)),
      .pair_count = object_idx
    }
  };
  if (value.as.object.pairs == NULL) {
    ps->error = JSON_ERROR_MEMORY;
    return value;
  }
  
  /* Whitespace. */
  json_parse_whitespace(ps);
  
  /* '{' */
  if (!json_parse_character(ps, L'{')) {
    ps->error = JSON_ERROR_OBJECTOPEN;
    json_value_free(value);
    return value;
  }
  
  /* Whitespace. */
  json_parse_whitespace(ps);
  
  /* Pairs. */
  json_pair pair;
  while (ps->wc != L'}') {
    /* Ensure object is big enough. */
    while (object_idx >= object_size) {
      object_size *= 2;
      json_pair *pairs_new = realloc(value.as.object.pairs, object_size*sizeof(*value.as.object.pairs));
      if (pairs_new == NULL) {
        ps->error = JSON_ERROR_MEMORY;
        json_value_free(value);
        return value;
      }
      value.as.object.pairs = pairs_new;
    }
    /* Parse pair. */
    pair = json_parse_pair(ps);
    if (ps->error != JSON_ERROR_none_) {
      json_value_free(value);
      return value;
    }
    value.as.object.pairs[object_idx++] = pair;
    value.as.object.pair_count = object_idx;
    /* Whitespace. */
    json_parse_whitespace(ps);
    /* Stop if continuation is not announced. */
    if (!json_parse_character(ps, L','))
      break;
  }
  
  /* '}' */
  if (!json_parse_character(ps, L'}')) {
    ps->error = JSON_ERROR_OBJECTCLOSE;
    json_value_free(value);
  }
  
  return value;
}
