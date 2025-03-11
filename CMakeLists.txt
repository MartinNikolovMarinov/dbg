cmake_minimum_required(VERSION 3.2)
project(STL_VIEWER VERSION 0.1.0 LANGUAGES CXX)

set(target_main dbg)

# Standard Requirements:

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generates compile_commands.json

# Add directories from lib cmake projects

if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Release")
    # NO ASSERTS in Release Builds. BE NOT AFRAID.
    set(CORE_ASSERT_ENABLED OFF CACHE BOOL "Enable Asserts in corelib" FORCE)
else()
    set(CORE_ASSERT_ENABLED ON CACHE BOOL "Enable Asserts in corelib" FORCE)
endif()
set(CORE_LIBRARY_SHARED OFF CACHE BOOL "Build core as a shared library." FORCE)
add_subdirectory(lib/core)

# Include cmake modules from:

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake_local")

include(DetectOS)
include(CompilerOptions)
include(Logger)
include(DBGDefaultFlags)

init_logger("[DBG]")

if(NOT OS STREQUAL "linux")
    log_error("This project targets only Linux for now")
endif()

# ---------------------------------------- Begin Options ---------------------------------------------------------------

if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Release")
    set(DBG_DEBUG OFF)
else()
    set(DBG_DEBUG ON)
endif()

# Print Selected Options:

log_info("---------------------------------------------")
log_info("Version:                   ${PROJECT_VERSION}")
log_info("Platform:                  ${OS}")
log_info("Arch:                      ${CMAKE_SYSTEM_PROCESSOR}")
log_info("C++ Version:               ${CMAKE_CXX_STANDARD}")
log_info("Compiler:                  ${CMAKE_CXX_COMPILER_ID}")
log_info("Compiler Version:          ${CMAKE_CXX_COMPILER_VERSION}")
log_info("Debug:                     ${DBG_DEBUG}")
log_info("Use External Vulkan SDK:   ${USE_EXTERNAL_VULKAN_SDK}")
log_info("---------------------------------------------")

# ---------------------------------------- End Options -----------------------------------------------------------------

# ---------------------------------------- Begin Declare Source Files --------------------------------------------------

set(dbg_src
    main.cpp

    src/basic.cpp
    src/stack_allocator.cpp
)

# ---------------------------------------- End Declare Source Files ----------------------------------------------------

# ---------------------------------------- Begin Create Executable -----------------------------------------------------

add_executable(${target_main} ${dbg_src})
target_link_libraries(${target_main} PUBLIC
    core # link with corelib
)
target_include_directories(${target_main} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_definitions(${target_main} PUBLIC
    "DBG_DEBUG=$<BOOL:${DBG_DEBUG}>"
    DBG_TEST_BINARIES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/test_binaries"
)

dbg_target_set_default_flags(${target_main} ${DBG_DEBUG} false)

# ---------------------------------------- END Create Executable -------------------------------------------------------
