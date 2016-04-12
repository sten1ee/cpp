cmake_minimum_required(VERSION 3.5)
project(SmartOp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories("../..")
set(SOURCE_FILES
        assert.cpp
        initterm.cpp
        jp.cpp
        parsetre.cpp
        profile.cpp
        readterm.cpp
        smartop.cpp
        )
add_executable(SmartOp ${SOURCE_FILES})
