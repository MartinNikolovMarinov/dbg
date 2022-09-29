cmake_minimum_required(VERSION 3.2)

project(dbg)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generates compile_commands.json

# Source files for executable
set(SRC_FILES
    main.cpp
    init.h
    init.cpp
    str_serializer.h
    str_serializer.cpp
    elf_parser.h
    elf_section.h
    elf_section.cpp
)
set(LINENOISE_SRC_FILES
    libs/linenoise/linenoise.c
)

add_executable(${PROJECT_NAME}
    ${SRC_FILES}
    ${LINENOISE_SRC_FILES}
)

# Compiler options:
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
        -O0 -g -ggdb
        -std=c++17
        -Wall -Wextra -Wno-unknown-pragmas -Wno-unused-function
    )
else()
    message(FATAL_ERROR "Unsupported compiler!")
endif()

# Definitions:
target_compile_definitions(${PROJECT_NAME} PRIVATE
    -DPATH_TO_DATA="${CMAKE_CURRENT_SOURCE_DIR}/data"
)

# Core library related:
option(CORE_LIBRARY_SHARED "" ON) # FIXME: might want to statically link
target_link_libraries(${PROJECT_NAME} core)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/core/src)

# Fmt library related:
target_link_libraries(${PROJECT_NAME} fmt)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/fmt)

# Target include directories:
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/linenoise
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/fmt/include
)
