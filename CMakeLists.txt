cmake_minimum_required(VERSION 3.20)
PROJECT(guideline)

add_executable(${PROJECT_NAME} )
target_sources(${PROJECT_NAME} PRIVATE src/UniquePtr.cpp)
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES 
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON

)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_compile_options(${PROJECT_NAME} PRIVATE 
    $<$<CXX_COMPILER_ID:GNU>:
        $<$<CONFIG:Debug>:-Wall -pedantic -g>
        $<$<CONFIG:Release>:-O3>
    >
    $<$<CXX_COMPILER_ID:Clang>:
        $<$<CONFIG:Debug>: -g -Wno-c++98-compat -Wall>
        $<$<CONFIG:Release>:-O3 >
    >
)

