cmake_minimum_required(VERSION 3.16)
project(NLM_Fallback_Tests VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Force use of fallback implementations
set(NLOHMANN_JSON_FALLBACK 1)
set(YAML_CPP_FALLBACK 1)

# Add external include directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Test executable
add_executable(test_fallback test_fallback.cpp
    ../src/core/Config/Config.cpp
    ../src/core/Random/Random.cpp
    ../src/core/Logger/Logger.cpp
    ../src/core/SimulationClock/SimulationClock.cpp
)

target_include_directories(test_fallback PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)

# Add the main directory for Config.hpp
include_directories(../src)
