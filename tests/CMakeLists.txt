# tests/CMakeLists.txt

# Test configuration
cmake_minimum_required(VERSION 3.20)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Core test sources
set(NLM_TEST_SOURCES
    test_main.cpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
    test_logger.cpp
    test_main_app.cpp
)

# Create test executable
add_executable(nlm_test ${NLM_TEST_SOURCES})

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Set output directory
set_target_properties(nlm_test PROPERTIES
    OUTPUT_NAME "nlm_test"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
)

# Add test helper functions
add_library(nlm_test_helper STATIC
    # Add any test helper files here if needed
)

target_include_directories(nlm_test_helper PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Configure test options
if(NLM_ENABLE_COVERAGE)
    target_compile_options(nlm_test PRIVATE --coverage)
    target_link_options(nlm_test PRIVATE --coverage)
endif()

# Add memory leak detection for Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(nlm_test PRIVATE -fsanitize=address,undefined)
    target_link_options(nlm_test PRIVATE -fsanitize=address,undefined)
endif()