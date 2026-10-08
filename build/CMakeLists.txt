cmake_minimum_required(VERSION 3.16)
project(test_config_enhanced)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Add the test source files
add_executable(test_config_enhanced
    ../tests/test_config_enhanced.cpp
)

# Test libraries - we'll create a minimal config library
add_library(config_lib STATIC
    ../src/core/Config/Config.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

link_libraries(test_config_enhanced config_lib)

# Add nlohmann/json if available
find_package(nlohmann_json REQUIRED)
link_libraries(test_config_enhanced nlohmann_json::nlohmann_json)

# Add yaml-cpp if available
find_package(yaml-cpp REQUIRED)
link_libraries(test_config_enhanced yaml-cpp)
