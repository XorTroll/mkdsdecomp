cmake_minimum_required(VERSION 3.13)

project(mkdsutil)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include ../common/include)

set(SOURCES
    source/main.cpp
    ../common/source/mk/save/save_Crypto.cpp
    ../common/source/mk/save/save_Format.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
