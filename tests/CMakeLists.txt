cmake_minimum_required(VERSION 3.16)

# Test configuration with cross-platform support
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Platform detection for tests
if(WIN32)
    set(TEST_PLATFORM "Windows")
    set(NLM_PLATFORM_WINDOWS 1)
    message(STATUS "Building tests for Windows platform")
elseif(APPLE)
    set(TEST_PLATFORM "macOS")
    set(NLM_PLATFORM_MACOS 1)
    message(STATUS "Building tests for macOS platform")
else()
    set(TEST_PLATFORM "Linux")
    set(NLM_PLATFORM_LINUX 1)
    message(STATUS "Building tests for Linux platform")
endif()

# Compiler detection for tests
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(TEST_COMPILER "MSVC")
    set(NLM_COMPILER_MSVC 1)
    message(STATUS "Using MSVC compiler for tests: ${CMAKE_CXX_COMPILER_ID}")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(TEST_COMPILER "Clang")
    set(NLM_COMPILER_CLANG 1)
    message(STATUS "Using Clang compiler for tests: ${CMAKE_CXX_COMPILER_ID}")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(TEST_COMPILER "GCC")
    set(NLM_COMPILER_GCC 1)
    message(STATUS "Using GCC compiler for tests: ${CMAKE_CXX_COMPILER_ID}")
endif()

# Test build options
option(NLM_BUILD_TESTS "Build all test executables" ON)
option(NLM_ENABLE_UNIT_TESTS "Enable unit test framework" ON)
option(NLM_ENABLE_INTEGRATION_TESTS "Enable integration tests" ON)
option(NLM_ENABLE_PERFORMANCE_TESTS "Enable performance benchmarks" ON)
option(NLM_ENABLE_COVERAGE_TESTS "Enable coverage analysis" OFF)
option(NLM_ENABLE_VALGRIND_TESTS "Enable Valgrind memory leak detection" OFF)

# Platform-specific test compilation flags
if(MSVC)
    add_compile_options(/W4 /WX)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type)
    add_compile_options(-Wshadow -Wformat=2 -Wconversion -Wsign-conversion)
    add_compile_options(-DNOMINMAX -D_POSIX_C_SOURCE=200809L)
    
    # Additional warnings for Clang
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-Wnull-dereference -Weverything)
    endif()
endif()

# Create test executable with comprehensive source files
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
    test_optimizations.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Platform-specific optimizations for tests
if(NLM_ENABLE_SIMD AND NOT MSVC)
    target_compile_options(nlm_test PRIVATE -march=native -mavx2)
endif()

# Enable testing framework
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Add test metadata
if(NLM_ENABLE_COVERAGE_TESTS)
    target_compile_options(nlm_test PRIVATE --coverage)
    target_link_libraries(nlm_test PRIVATE --coverage)
endif()

# Performance benchmark executable
add_executable(nlm_benchmark
    ../src/benchmark.cpp
)

target_link_libraries(nlm_benchmark PUBLIC nlm_core nlm_agent nlm_world)
target_compile_features(nlm_benchmark PRIVATE cxx_std_20)

# Platform-specific optimizations for benchmark
if(NLM_ENABLE_SIMD AND NOT MSVC)
    target_compile_options(nlm_benchmark PRIVATE -march=native -mavx2)
endif()

# Optimization verification executable  
add_executable(nlm_optimization_tests
    ../src/benchmark.cpp
)

target_link_libraries(nlm_optimization_tests PUBLIC nlm_core nlm_agent nlm_world)
target_compile_features(nlm_optimization_tests PRIVATE cxx_std_20)

# Platform-specific optimizations for optimization tests
if(NLM_ENABLE_SIMD AND NOT MSVC)
    target_compile_options(nlm_optimization_tests PRIVATE -march=native -mavx2)
endif()

# Test deployment configuration
set(TEST_INSTALL_BINDIR bin CACHE PATH "Test installation directory for executables")

# Test reporting configuration
if(DEFINED ENV{CI} OR DEFINED ENV{GITHUB_ACTIONS})
    # In CI/CD environments, ensure tests are run
    set(TEST_CI_MODE ON)
    message(STATUS "Running in CI/CD environment - enforcing test execution")
else()
    set(TEST_CI_MODE OFF)
endif()

# Custom test commands for different platforms
if(PYTHON_PLATFORM_MACOS)
    # macOS-specific test configuration
    add_custom_target(test_all
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS nlm_test nlm_benchmark nlm_optimization_tests
        COMMENT "Running all tests on macOS"
    )
elseif(PYTHON_PLATFORM_WINDOWS)
    # Windows-specific test configuration
    add_custom_target(test_all
        COMMAND ctest --output-on-failure
        DEPENDS nlm_test nlm_benchmark nlm_optimization_tests
        COMMENT "Running all tests on Windows"
    )
else()
    # Linux/Unix-specific test configuration
    add_custom_target(test_all
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS nlm_test nlm_benchmark nlm_optimization_tests
        COMMENT "Running all tests on Linux/Unix"
    )
endif()