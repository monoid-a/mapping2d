cmake_minimum_required (VERSION 3.16)

project (mapping2d_prj_src)

add_subdirectory(mapping2d)
add_subdirectory(mapping2d_qt)

file(GLOB mapping2d_qt_SRC
     "mapping2d_qt/*.h"
     "mapping2d_qt/*.cpp"
     "mapping2d_qt/mapper2d_qt.ui"
     "mapping2d_qt/mapper2d_qt.qrc"
)

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${QT_INCLUDES}
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(mapping2d_prj_src ${mapping2d_qt_SRC})

target_precompile_headers(mapping2d_prj_src PRIVATE ${CMAKE_SOURCE_DIR}/src/mapping2d_qt/pch.h)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

cmake_policy(SET CMP0074 NEW)

find_package(Boost)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

target_link_libraries(mapping2d_prj_src PUBLIC mapping2d Qt5::Widgets Boost::boost)

set_property(TARGET mapping2d_prj_src PROPERTY CXX_STANDARD 14)