cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create modular brain test executable
add_executable(test_modular_brain
    test_modular_brain.cpp
)

target_link_libraries(test_modular_brain PRIVATE nlm_core)
target_compile_features(test_modular_brain PRIVATE cxx_std_20)

# Add test
enable_testing()
add_test(NAME test_modular_brain COMMAND test_modular_brain)