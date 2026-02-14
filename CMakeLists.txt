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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/debugging/Lifetime.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/attributes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/fileio.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/fileio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/str_utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/str_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/thread_utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/thread_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/timer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/timer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/triple_buffer.h
)

set(DAEDALUS_WINDOWS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/fileio_windows.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/program_windows.cpp
)

set(DAEDALUS_LINUX_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/fileio_linux.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daedalus/program_linux.cpp
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
