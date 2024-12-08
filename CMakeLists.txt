cmake_minimum_required(VERSION 3.20)
PROJECT(guideline)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(UniquePtr)
add_executable(SharedPtr)
add_executable(Tuple)
add_executable(Pair)
add_executable(Main)
target_sources(UniquePtr PRIVATE src/UniquePtr.cpp)
target_sources(SharedPtr PRIVATE src/SharedPtr.cpp)
target_sources(Tuple PRIVATE src/Tuple.cpp)
target_sources(Pair PRIVATE src/Pair.cpp)
target_sources(Main PRIVATE src/main.cpp)

include_directories(include)

add_compile_options(
    $<$<CXX_COMPILER_ID:GNU>:
        $<$<CONFIG:Debug>:-Wall -pedantic -g>
        $<$<CONFIG:Release>:-O3>
    >
    $<$<CXX_COMPILER_ID:Clang>:
        $<$<CONFIG:Debug>: -g -Wno-c++98-compat -Wall>
        $<$<CONFIG:Release>:-O3 >
    >
)

