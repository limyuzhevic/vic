// Enhanced CMakeLists.txt for tests
// Updated to include new test framework and comprehensive tests

cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Create test executable with enhanced tests
add_executable(nlm_test_enhanced
    test_main.cpp
    test_enhanced_config.cpp
    test_error_handling.cpp
    test_integration.cpp
)

target_link_libraries(nlm_test_enhanced PRIVATE nlm_core nlm_agent)
target_compile_features(nlm_test_enhanced PRIVATE cxx_std_20)

# Create test executable for new test framework
add_executable(nlm_test_framework
    tests/TestFramework.hpp
)

target_link_libraries(nlm_test_framework PRIVATE nlm_core)
target_compile_features(nlm_test_framework PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test_enhanced COMMAND nlm_test_enhanced)

# Set test properties
# Set timeout for tests to prevent hanging
foreach(test_name nlm_test_enhanced)
    set_tests_properties(${test_name} PROPERTIES TIMEOUT 300)
endforeach()

# Add custom test targets
add_custom_target(run_all_tests
    COMMAND ./nlm_test_enhanced
    DEPENDS nlm_test_enhanced
    COMMENT "Run all enhanced NLM tests"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(run_framework_tests
    COMMAND ./nlm_test_framework
    DEPENDS nlm_test_framework
    COMMENT "Run test framework tests"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
