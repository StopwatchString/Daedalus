cmake_minimum_required(VERSION 4.0.0)

project(Daedalus LANGUAGES CXX)

set(DAEDALUS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/containers/array_interface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/containers/heap_array.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/containers/heap_vector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/containers/hybrid_vector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/containers/stack_array.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/containers/stack_vector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/containers/vector_interface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/containers/triple_buffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/core/attributes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/debugging/lifetime.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/io/file.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/io/file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/math/math.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/profiling/timer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/profiling/timer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/program/program.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/program/program.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/strings/utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/strings/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/threading/utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/threading/utils.cpp
)

set(DAEDALUS_WINDOWS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/platform/windows/file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/platform/windows/program.cpp
)

set(DAEDALUS_LINUX_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/platform/linux/file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/platform/linux/program.cpp
)

add_library(daedalus
    ${DAEDALUS_SOURCES}
    $<$<PLATFORM_ID:Windows>:${DAEDALUS_WINDOWS_SOURCES}>
    $<$<PLATFORM_ID:Linux>:${DAEDALUS_LINUX_SOURCES}>
)

# C++20 for consumers too
target_compile_features(daedalus
    PUBLIC
        cxx_std_20
)

target_include_directories(daedalus
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
