cmake_minimum_required(VERSION 3.21)
project(examples)

set(CMAKE_CXX_STANDARD 11)

set(JACC_DIR ..)
set(CPP_DIR ../..)

set(JACC_SOURCE_FILES
    ${JACC_DIR}/grammar.cpp   ${JACC_DIR}/jacc.cpp  ${JACC_DIR}/jacc_jamp.cpp
    ${JACC_DIR}/jacc_spacer.cpp  ${JACC_DIR}/lalr.cpp  ${JACC_DIR}/lalr_read.cpp
    ${JACC_DIR}/jacc_clos.cpp  ${JACC_DIR}/jacc_glue.cpp  ${JACC_DIR}/jacc_lexer.cpp
    ${JACC_DIR}/jacc_symtype.cpp  ${JACC_DIR}/lalr_print.cpp ${JACC_DIR}/template_instances.cpp
    ${JACC_DIR}/jacc_tab.c  ${CPP_DIR}/util/src/assert.cpp)

include_directories(${JACC_DIR}/..)
add_executable(jacc ${JACC_SOURCE_FILES})


macro(generate_parser_source NAME)
    add_custom_command(
        COMMAND jacc -t ${JACC_DIR}/jacc.tmpl ${NAME}.j
        WORKING_DIRECTORY ..
        OUTPUT  ../${NAME}_parser.cpp  ../${NAME}_parser.h  ../${NAME}_sym.h
        MAIN_DEPENDENCY ${NAME}.j
        DEPENDS jacc)
endmacro()

macro(build_parser NAME)
    generate_parser_source(${NAME})
    add_executable(${NAME} ${NAME}_parser.cpp ${JACC_DIR}/runtime/lr_parser.cpp ${ARGN})
endmacro()

include_directories(${JACC_DIR}/runtime)
build_parser(ab  ab_main.cpp)
build_parser(calc  calc_main.cpp)
build_parser(java12  java12_lex.cpp)
