cmake_minimum_required(VERSION 3.16)

project(rootlyze VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE ROOTLYZE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_executable(rootlyze ${ROOTLYZE_SOURCES})

target_include_directories(rootlyze
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

if (MSVC)
    target_compile_options(rootlyze PRIVATE /W4 /permissive-)
else()
    target_compile_options(rootlyze PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Wconversion
        -Wsign-conversion
        -fstack-protector-strong
        -D_FORTIFY_SOURCE=2
    )
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (MSVC)
        target_compile_options(rootlyze PRIVATE /O2)
    else()
        target_compile_options(rootlyze PRIVATE -O3 -DNDEBUG)
    endif()
endif()