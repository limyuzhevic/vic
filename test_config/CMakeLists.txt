cmake_minimum_required(VERSION 3.16)
project(nlm_config_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find nlm_core library
find_library(NLM_CORE_LIB nlm_core)

# Test executable
add_executable(config_test test_config_fixes.cpp)
target_link_libraries(config_test PRIVATE nlm_core)
target_compile_features(config_test PRIVATE cxx_std_20)

# Add test
enable_testing()
add_test(NAME config_test COMMAND config_test)
