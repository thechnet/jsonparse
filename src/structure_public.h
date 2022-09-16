typedef enum json_type_ {
  JSON_TYPE_none_,
  JSON_TYPE_ERROR,
  JSON_TYPE_SIZE,
  JSON_TYPE_NULL,
  JSON_TYPE_BOOLEAN,
  JSON_TYPE_INTEGER,
  JSON_TYPE_FLOATING,
  JSON_TYPE_STRING,
  JSON_TYPE_ARRAY,
  JSON_TYPE_OBJECT,
  JSON_TYPE_max_
} json_type;

typedef int64_t json_integer;
typedef double json_floating;

typedef struct json_object_ {
  size_t pair_count;
  struct json_pair_ *pairs;
} json_object;

typedef struct json_value_ {
  json_type type;
  union json_value_as_ {
    json_integer integer;
    json_floating floating;
    wchar_t *string;
    json_object object;
    struct json_value_ *array;
  } as;
} json_value;

typedef struct json_pair_ {
  wchar_t *key;
  json_value value;
} json_pair;

void json_value_free(json_value value);
void json_value_represent(json_value value);
