# NLM Testing Framework - Modern CMake Configuration

cmake_minimum_required(VERSION 3.16)

# Test configuration using target-based approach
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create test executable using target_sources
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
)

# Configure test target
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Add include directories using target_include_directories
target_include_directories(nlm_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Link against core library
target_link_libraries(nlm_test PRIVATE nlm_core)

# Enable testing and add test
enable_testing()
nlm_add_test(nlm_test)

# Generate test report
include(CTest)
set(CTEST_OUTPUT_ON_FAILURE TRUE)

# Additional test configuration
# Add coverage analysis if requested
option(NLM_ENABLE_COVERAGE "Enable code coverage analysis" OFF)
if(NLM_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(nlm_test PRIVATE --coverage)
        target_link_libraries(nlm_test PRIVATE --coverage)
    endif()
endif()

# Add valgrind testing if available
find_program(VALGRIND valgrind)
if(VALGRIND)
    add_test(NAME nlm_test_valgrind
        COMMAND ${VALGRIND} --leak-check=full --error-exitcode=1 $<TARGET_FILE:nlm_test>
    )
    set_tests_properties(nlm_test_valgrind PROPERTIES WILL_FAIL TRUE)
endif()
