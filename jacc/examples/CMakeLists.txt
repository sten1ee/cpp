cmake_minimum_required(VERSION 3.6)
project(examples)

set(CMAKE_CXX_STANDARD 11)

set(JACC_DIR ..)
set(EXAMPLES_DIR ${JACC_DIR}/examples)

set(JACC_SOURCE_FILES
    ${JACC_DIR}/grammar.cpp   ${JACC_DIR}/jacc.cpp  ${JACC_DIR}/jacc_jamp.cpp
    ${JACC_DIR}/jacc_spacer.cpp  ${JACC_DIR}/lalr.cpp  ${JACC_DIR}/lalr_read.cpp
    ${JACC_DIR}/setlocale.cpp  ${JACC_DIR}/jacc_clos.cpp  ${JACC_DIR}/jacc_glue.cpp
    ${JACC_DIR}/jacc_lexer.cpp  ${JACC_DIR}/jacc_symtype.cpp  ${JACC_DIR}/lalr_print.cpp
    ${JACC_DIR}/template_instances.cpp  ${JACC_DIR}/jacc_tab.c  ${JACC_DIR}/../util/src/assert.cpp)

include_directories(~/src/cpp)

add_executable(jacc ${JACC_SOURCE_FILES})

set(JACC_TEMPLAE ${JACC_DIR}/jacc.tmpl)

include_directories(../runtime)

add_custom_command(
  OUTPUT ${EXAMPLES_DIR}/calc_parser.cpp ${EXAMPLES_DIR}/calc_parser.h ${EXAMPLES_DIR}/calc_sym.h
  DEPENDS jacc calc.j
  COMMAND jacc -t ${JACC_TEMPLAE} calc.j
  WORKING_DIRECTORY ${JACC_DIR}/examples
  )

set(CALC_SOURCE_FILES  calc_main.cpp calc_parser.cpp ../runtime/lr_parser.cpp)
add_executable(calc ${CALC_SOURCE_FILES})

set(AB_SOURCE_FILES  ab_main.cpp ab_parser.cpp ../runtime/lr_parser.cpp)
add_executable(ab ${AB_SOURCE_FILES})

set(JAVA12_SOURCE_FILES  java12_lex.cpp java12_parser.cpp ../runtime/lr_parser.cpp)
add_executable(java12 ${JAVA12_SOURCE_FILES})
