cmake_minimum_required(VERSION 3.16)
project(test_improvements CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build the test executable
add_executable(test_improvements test_improvements.cpp)

# Include source directories
target_include_directories(test_improvements PRIVATE src)

# Link with nlm_core library
find_package(nlohmann_json REQUIRED)

# Create a simple nlm_core library for testing
add_library(nlm_core STATIC
    src/core/Config/Config.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
    ${nlohmann_JSON_INCLUDE_DIRS}
)

target_link_libraries(nlm_core PUBLIC nlohmann_json::nlohmann_json)

target_link_libraries(test_improvements PRIVATE nlm_core)

# Test target
set_target_properties(test_improvements PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)