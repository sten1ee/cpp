cmake_minimum_required(VERSION 3.5)
project(SmartOp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ggdb ")

set(SOURCE_FILES
        initterm.cpp
        jp.cpp
        parsetre.cpp
        profile.cpp
        readterm.cpp
        smartop.cpp
        )
add_executable(SmartOp ${SOURCE_FILES})
