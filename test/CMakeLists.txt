cmake_minimum_required(VERSION 3.16)
project(test_enhanced_config)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(test_enhanced_config test_enhanced_config.cpp src/core/Config/Config.cpp)

target_include_directories(test_enhanced_config PRIVATE src)