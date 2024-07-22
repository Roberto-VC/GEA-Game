cmake_minimum_required(VERSION 3.16)
project(cppGameDev)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

file(GLOB_RECURSE CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/*.cpp")

# Add directory containing print.h
include_directories(${PROJECT_SOURCE_DIR}/path/to/print/header)

# Or if print.h is in the same directory as your source files
include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
set(SDL2_PATH "SDL2-2.30.5\\x86_64-w64-mingw32")
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

add_executable(Game main.cpp)
target_link_libraries(Game ${SDL2_LIBRARY})
