cmake_minimum_required(VERSION 3.6)
project(jacc)

set(CMAKE_CXX_STANDARD 98)

set(SOURCE_FILES  grammar.cpp  jacc.cpp  jacc_jamp.cpp  jacc_spacer.cpp  lalr.cpp  lalr_read.cpp  setlocale.cpp
    jacc_clos.cpp  jacc_glue.cpp  jacc_lexer.cpp  jacc_symtype.cpp  lalr_print.cpp  template_instances.cpp
    jacc_tab.c  ../util/src/assert.cpp)

include_directories(~/src/cpp)

add_executable(jacc ${SOURCE_FILES})