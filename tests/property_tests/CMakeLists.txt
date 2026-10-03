# Property-based Testing CMake Configuration

cmake_minimum_required(VERSION 3.16)

# Property-based Testing Framework
add_executable(nlm_property_tests
    property_tests/NLMPropertyTestFramework.hpp
    property_tests/property_test_neural_dynamics.cpp
    property_tests/property_test_memory_systems.cpp
    property_tests/property_test_plasticity.cpp
    property_tests/property_test_error_handling.cpp
    property_tests/property_test_performance.cpp
    property_tests/property_test_brain_integration.cpp
    property_tests/main_property_test.cpp
)

target_link_libraries(nlm_property_tests PRIVATE nlm_core)
target_compile_features(nlm_property_tests PRIVATE cxx_std_20)

# Optional: Add CTest integration for property tests
# Enable property test specific options
if(MSVC)
    target_compile_options(nlm_property_tests PRIVATE /W4)
else()
    target_compile_options(nlm_property_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Add property tests to test suite
enable_testing()

# Property tests run separately to manage execution time
add_test(NAME nlm_property_tests_short
    COMMAND nlm_property_tests
    TIMEOUT 300  # 5 minute timeout for property tests
)