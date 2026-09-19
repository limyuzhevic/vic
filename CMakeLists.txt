cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core library - list all .cpp files recursively
file(GLOB_RECURSE NLM_CORE_SOURCES
    "${NLM_SOURCE_DIR}/*.cpp"
)

# Filter out files we don't want (tests and Python bindings)
list(FILTER NLM_CORE_SOURCES EXCLUDE REGEX ".*(test|bindings)\.cpp$")

add_library(nlm_core STATIC ${NLM_CORE_SOURCES})

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Main executable
add_executable(nlm ${NLM_SOURCE_DIR}/main.cpp)
target_link_libraries(nlm PUBLIC nlm_core)

# Enable testing
enable_testing()
add_subdirectory(tests)

# Install targets
install(TARGETS nlm nlm_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${NLM_SOURCE_DIR}/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)
