

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(VectorTest)
add_executable(VectorTest2)
add_executable(ExpressionTest)
add_executable(Function)


target_sources(VectorTest PRIVATE vector_test.cpp)
target_sources(VectorTest2 PRIVATE test_vector.cpp)
target_sources(ExpressionTest PRIVATE expression_template.cpp)
target_sources(Function PRIVATE function.cpp)


include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

add_compile_options(
    $<$<CXX_COMPILER_ID:GNU>:
        $<$<CONFIG:Debug>:-Wall -pedantic -g -Wextra>
        $<$<CONFIG:Release>:-O3>
    >
    $<$<CXX_COMPILER_ID:Clang>:
        $<$<CONFIG:Debug>: -g -Wno-c++98-compat -Wall -Weverything>
        $<$<CONFIG:Release>:-O3 >
    >
)