cmake_minimum_required(VERSION 3.20)

project(Daedalus LANGUAGES CXX)

add_library(daedalus
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fileio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/str_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/timer.cpp
)

# C++20 for consumers too
target_compile_features(daedalus
    PUBLIC
        cxx_std_20
)

target_include_directories(daedalus
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
