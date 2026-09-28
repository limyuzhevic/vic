cmake_minimum_required(VERSION 3.16)
project(test_logger VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create test executable for enhanced logging
add_executable(test_logger_test
    ../tests/test_logger.cpp
    ../src/core/Logger/Logger.cpp
    ../src/core/Config/Config.cpp
    ../src/core/Types/Types.cpp
    ../src/core/Random/Random.cpp
    ../src/core/SimulationClock/SimulationClock.cpp
)

target_compile_features(test_logger_test PRIVATE cxx_std_20)
