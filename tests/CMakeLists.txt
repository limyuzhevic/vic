# Enhanced test configuration CMakeLists.txt
# Modern target-based approach with comprehensive testing support

cmake_minimum_required(VERSION 3.16)

# Test configuration with modern CMake practices
# No global commands - use target-based approach

# Define test sources
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
)

# Create comprehensive test executable
add_executable(nlm_test ${NLM_TEST_SOURCES})

# Apply modern platform flags
target_compile_features(nlm_test PRIVATE cxx_std_20)
target_include_directories(nlm_test PRIVATE src)
target_link_libraries(nlm_test PRIVATE nlm_core)

# Apply platform-specific compiler options and configurations
if(MSVC)
    target_compile_options(nlm_test PRIVATE /W4 /WX)
else()
    target_compile_options(nlm_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Sanitizers and coverage
if(ENABLE_SANITIZERS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(nlm_test PRIVATE -fsanitize=address,undefined)
        target_link_options(nlm_test PRIVATE -fsanitize=address,undefined)
    endif()
endif()

if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(nlm_test PRIVATE --coverage -fprofile-arcs -ftest-coverage)
    target_link_options(nlm_test PRIVATE --coverage)
endif()

# Enable testing framework
enable_testing()

# Add comprehensive test targets
add_test(NAME nlm_unit_tests COMMAND nlm_test)

# Test output configuration
set_target_properties(nlm_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
    DEBUG_POSTFIX "_d"
)

# Test coverage configuration
if(ENABLE_COVERAGE)
    # Create coverage target for generating reports
    add_custom_target(coverage
        COMMAND ${CMAKE_COMMAND} -E echo "Generate coverage report..."
        COMMAND lcov --directory . --capture --output-file ${CMAKE_BINARY_DIR}/coverage.info
        COMMAND genhtml -o ${CMAKE_BINARY_DIR}/coverage_report ${CMAKE_BINARY_DIR}/coverage.info
        COMMENT "Generating test coverage report"
        VERBATIM
    )
endif()

# Add test dependencies configuration
if(USE_OPENMP)
    target_link_libraries(nlm_test PRIVATE OpenMP::OpenMP_CXX)
endif()

if(USE_TBB)
    target_link_libraries(nlm_test PRIVATE TBB::TBB)
endif()

# Export test configuration
install(TARGETS nlm_test
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/tests
)
