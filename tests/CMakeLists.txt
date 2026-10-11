cmake_minimum_required(VERSION 3.16)

# Find GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Find required packages
find_package(pybind11 CONFIG REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

# Configure build options
option(NLM_TESTS "Build test suite" ON)
option(NLM_PERFORMANCE_TESTS "Build performance tests" ON)
option(NLM_INTEGRATION_TESTS "Build integration tests" ON)
option(NLM_BENCHMARKS "Build performance benchmarks" ON)

# Configure include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

if(NLM_TESTS)
    # Create unit test executables
    
    # Brain unit tests
    add_executable(nlm_test_brain brain_tests.cpp)
    target_link_libraries(nlm_test_brain PRIVATE nlm_core GTest::gtest GTest::gtest_main)
    target_include_directories(nlm_test_brain PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    target_compile_features(nlm_test_brain PRIVATE cxx_std_20)
    add_test(NAME nlm_test_brain COMMAND nlm_test_brain)
    
    # Config unit tests
    add_executable(nlm_test_config config_tests.cpp)
    target_link_libraries(nlm_test_config PRIVATE nlm_core GTest::gtest GTest::gtest_main)
    target_include_directories(nlm_test_config PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    target_compile_features(nlm_test_config PRIVATE cxx_std_20)
    add_test(NAME nlm_test_config COMMAND nlm_test_config)
    
    # Performance unit tests
    add_executable(nlm_test_performance performance_tests.cpp)
    target_link_libraries(nlm_test_performance PRIVATE nlm_core nlm_performance GTest::gtest GTest::gtest_main)
    target_include_directories(nlm_test_performance PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    target_compile_features(nlm_test_performance PRIVATE cxx_std_20)
    add_test(NAME nlm_test_performance COMMAND nlm_test_performance)
    
    # Integration tests
    add_executable(nlm_test_integration integration_tests.cpp)
    target_link_libraries(nlm_test_integration PRIVATE nlm_core nlm_agent nlm_world GTest::gtest GTest::gtest_main)
    target_include_directories(nlm_test_integration PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    target_compile_features(nlm_test_integration PRIVATE cxx_std_20)
    add_test(NAME nlm_test_integration COMMAND nlm_test_integration)
    
    # Visualization unit tests
    add_executable(nlm_test_visualization visualization_tests.cpp)
    target_link_libraries(nlm_test_visualization PRIVATE nlm_core GTest::gtest GTest::gtest_main)
    target_include_directories(nlm_test_visualization PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    target_compile_features(nlm_test_visualization PRIVATE cxx_std_20)
    add_test(NAME nlm_test_visualization COMMAND nlm_test_visualization)
endif()

if(NLM_PERFORMANCE_TESTS)
    # Performance benchmark executable
    add_executable(nlm_benchmark benchmark.cpp)
    target_link_libraries(nlm_benchmark PRIVATE nlm_core nlm_performance)
    target_include_directories(nlm_benchmark PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    target_compile_features(nlm_benchmark PRIVATE cxx_std_20)
    
    # Add benchmark target
    add_custom_target(benchmark_all
        COMMAND nlm_benchmark --benchmark-list
        DEPENDS nlm_benchmark
        COMMENT "Run all performance benchmarks"
    )
endif()

if(NLM_BENCHMARKS)
    # Python benchmarks
    add_executable(nlm_python_benchmark python_benchmark.cpp)
    target_link_libraries(nlm_python_benchmark PRIVATE nlm_core)
    target_include_directories(nlm_python_benchmark PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    target_compile_features(nlm_python_benchmark PRIVATE cxx_std_20)
endif()

# Configure test directory for different platforms
if(WIN32)
    target_compile_definitions(nlm_test_brain PRIVATE _WIN32_WINNT=0x0601)
    target_compile_definitions(nlm_test_config PRIVATE _WIN32_WINNT=0x0601)
    target_compile_definitions(nlm_test_performance PRIVATE _WIN32_WINNT=0x0601)
    target_compile_definitions(nlm_test_integration PRIVATE _WIN32_WINNT=0x0601)
    target_compile_definitions(nlm_test_visualization PRIVATE _WIN32_WINNT=0x0601)
endif()

# Platform-specific compiler flags
if(MSVC)
    target_compile_options(nlm_test_brain PRIVATE /W4 /permissive-)
    target_compile_options(nlm_test_config PRIVATE /W4 /permissive-)
    target_compile_options(nlm_test_performance PRIVATE /W4 /permissive-)
    target_compile_options(nlm_test_integration PRIVATE /W4 /permissive-)
    target_compile_options(nlm_test_visualization PRIVATE /W4 /permissive-)
else()
    target_compile_options(nlm_test_brain PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(nlm_test_config PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(nlm_test_performance PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(nlm_test_integration PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(nlm_test_visualization PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Enable testing for CTest
enable_testing()
