# see https://github.com/Krande/nanobind-minimal/tree/main

cmake_minimum_required(VERSION 3.28)

project(_dohop_farestore_nb LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Python 3 COMPONENTS Interpreter Development.Module REQUIRED)

execute_process(
    COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE NB_DIR
)

message(STATUS "NanoBind Cmake directory: " ${NB_DIR})
list(APPEND CMAKE_PREFIX_PATH "${NB_DIR}")

message("Generated with config types: ${CMAKE_CONFIGURATION_TYPES}")

find_package(nanobind CONFIG REQUIRED)

add_library(judy STATIC IMPORTED)
set_target_properties(judy PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/judy-1.0.5/src/libJudy.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/judy-1.0.5/src"
)

nanobind_add_module(
    _judy_nb
    STABLE_ABI FREE_THREADED
    src/judy/cpp/judy.cpp
    src/judy/cpp/judy_int_int_mapping.cpp
    src/judy/cpp/judy_int_set.cpp
)

nanobind_add_stub(
    _judy_nb_stub
    MODULE _judy_nb
    OUTPUT _judy_nb.pyi
    PYTHON_PATH $<TARGET_FILE_DIR:_judy_nb>
    DEPENDS _judy_nb
)

target_link_libraries(_judy_nb PRIVATE judy)

install(TARGETS _judy_nb LIBRARY DESTINATION judy)
