cmake_minimum_required(VERSION 3.20)
PROJECT(CompactSTL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-stdlib=libc++)
    add_link_options(-stdlib=libc++)   
endif()

add_executable(UniquePtr)
add_executable(SharedPtr)
add_executable(Tuple)
add_executable(Pair)
#add_executable(Main)
add_executable(test_vector)

target_sources(UniquePtr PRIVATE test/test_UniquePtr.cpp)
target_sources(SharedPtr PRIVATE test/test_SharedPtr.cpp)
target_sources(Tuple PRIVATE test/test_Tuple.cpp)
target_sources(Pair PRIVATE test/test_Pair.cpp)
#target_sources(Main PRIVATE test/test_main.cpp)
target_sources(test_vector PRIVATE test/test_vector.cpp)

include_directories(include)

add_compile_options(
    $<$<CXX_COMPILER_ID:GNU>:
        $<$<CONFIG:Debug>:-Wall -pedantic -g -Wextra -Wshadow>
        $<$<CONFIG:Release>:-O3>
    >
    $<$<CXX_COMPILER_ID:Clang>:
        $<$<CONFIG:Debug>:-g -Wall -Weverything -Wno-c++98-compat >
        $<$<CONFIG:Release>:-O3>
    >
)