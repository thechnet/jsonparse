cmake_minimum_required(VERSION 3.10)

project(
  jsonparse
  VERSION 1.0.0
  DESCRIPTION "Simple JSON parser"
  LANGUAGES C
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
  message(STATUS "Build type not specified, using Release.")
endif()

list(APPEND options "DEBUG_ENABLE_WATCHDOG|Enable Watchdog.|ON")
list(APPEND options "PARSER_PRINT_PARSED_CHARACTERS|Print parsed characters.|OFF")
foreach(option IN LISTS options)
  string(REPLACE "|" ";" option_list ${option})
  list(GET option_list 0 id)
  list(GET option_list 1 desc)
  list(GET option_list 2 val)
  option(JSON_${id} "${desc}." ${val})
endforeach()

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

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  target_compile_options(jsonparse PRIVATE -Wall -Wextra)
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
  target_compile_options(jsonparse PRIVATE -Weverything -Werror=embedded-directive)
endif()
target_compile_options(
  jsonparse PRIVATE
  -O3 -Wno-unused-parameter -Wno-disabled-macro-expansion -Wno-gnu-zero-variadic-macro-arguments -Wno-declaration-after-statement -Wno-used-but-marked-unused -Wno-padded -Wno-keyword-macro
)

foreach(option IN LISTS options)
  string(REPLACE "|" ";" option_list ${option})
  list(GET option_list 0 id)
  if("${JSON_${id}}" STREQUAL "ON")
    add_definitions(-DJSON_${id})
  endif()
endforeach()

file(READ src/structure_public.h FILE_STRUCTURE_PUBLIC_H)
configure_file(src/jsonparse.h.in jsonparse.h)
