cmake_minimum_required(VERSION 3.10)

project(
  jsonparse
  VERSION 1.0.0
  DESCRIPTION "Simple JSON parser"
  LANGUAGES C
)

add_library(
  jsonparse
  STATIC
  src/errors.c
  src/parser.c
  src/structure.c
  src/tools.c
  src/public.c
)

include_directories(../watchdog/build)

file(READ src/structure_public.h FILE_STRUCTURE_PUBLIC_H)

configure_file(src/jsonparse.h.in jsonparse.h)

target_compile_options(
  jsonparse PRIVATE
  -O3 -Weverything -Werror=embedded-directive -Wno-unused-parameter -Wno-disabled-macro-expansion -Wno-gnu-zero-variadic-macro-arguments -Wno-declaration-after-statement -Wno-used-but-marked-unused -Wno-padded -Wno-keyword-macro
)
