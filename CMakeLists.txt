cmake_minimum_required(VERSION 4.0.0)

project(Daedalus LANGUAGES CXX)

set(DAEDALUS_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/daedalus/containers/array_interface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/daedalus/containers/heap_array.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/daedalus/containers/heap_vector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/daedalus/containers/hybrid_vector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/daedalus/containers/stack_array.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/daedalus/containers/stack_vector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/daedalus/containers/vector_interface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/daedalus/fileio.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/daedalus/str_utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/daedalus/thread_utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/daedalus/timer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/daedalus/triple_buffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/daedalus/types.h
)

set(DAEDALUS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fileio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/str_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/thread_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/timer.cpp
)

add_library(daedalus
    ${DAEDALUS_SOURCES}
    ${DAEDALUS_HEADERS}
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
