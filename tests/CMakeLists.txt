cmake_minimum_required(VERSION 3.16)

# Test configuration with modern CMake practices
set(TEST_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Include directories
include_directories(${NLM_SOURCE_DIR})

# Create test executable with better organization
add_executable(nlm_test
    test_main.cpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
    test_comprehensive.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)

# Apply modern C++ standards and features
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Compiler flags for tests
target_compile_options(nlm_test PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Werror>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
)

# Enable profiling and debugging for tests
if(NLM_ENABLE_PROFILING)
    target_compile_options(nlm_test PRIVATE -pg)
    target_link_options(nlm_test PRIVATE -pg)
endif()

if(NLM_ENABLE_ASAN)
    target_compile_options(nlm_test PRIVATE -fsanitize=address)
    target_link_options(nlm_test PRIVATE -fsanitize=address)
endif()

if(NLM_ENABLE_TSAN)
    target_compile_options(nlm_test PRIVATE -fsanitize=thread)
    target_link_options(nlm_test PRIVATE -fsanitize=thread)
endif()

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Create a benchmark executable for performance testing
add_executable(nlm_benchmark benchmark.cpp)
target_link_libraries(nlm_benchmark PRIVATE nlm_core)
target_compile_features(nlm_benchmark PRIVATE cxx_std_20)

# Add benchmark tests
enable_testing()
add_test(NAME nlm_benchmark COMMAND nlm_benchmark)
