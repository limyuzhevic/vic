cmake_minimum_required(VERSION 3.16)
project(NLM_EnhancedConfigTest VERSION 0.1.0 LANGUAGES CXX)

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

# Test target for enhanced configuration
add_executable(test_enhanced_config
    test_enhanced_config.cpp
    include/nlm/Config.cpp
)

target_include_directories(test_enhanced_config PRIVATE include)

# Add test to run
add_test(NAME enhanced_config_test COMMAND test_enhanced_config)