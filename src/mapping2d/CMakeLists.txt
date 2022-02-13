cmake_minimum_required (VERSION 3.16)

project (mapping2d)

file(GLOB mapping2d_SRC
     "*.h"
     "*.cpp"
)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

cmake_policy(SET CMP0074 NEW)

find_package(Boost)

include_directories(${Boost_INCLUDE_DIR})

add_library(mapping2d SHARED ${mapping2d_SRC})

target_precompile_headers(mapping2d PRIVATE ${CMAKE_SOURCE_DIR}/src/mapping2d/pch.h)