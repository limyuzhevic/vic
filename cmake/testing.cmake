# Testing framework setup for NLM project

include(FetchContent)

# Find and configure Google Test
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Test target with modern CMake patterns
function(nlm_add_test target_name test_sources)
    add_executable(${target_name} ${test_sources})
    target_link_libraries(${target_name} PRIVATE
        nlm_core
        GTest::gtest
        GTest::gtest_main
    )
    target_compile_features(${target_name} PRIVATE cxx_std_20)
    
    # Apply test-specific settings
    target_compile_options(${target_name} PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    )
    
    # Add test to CTest
    add_test(NAME ${target_name}
        COMMAND ${target_name}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    
    # Add test to test all target
    set_target_properties(${target_name} PROPERTIES
        EXCLUDE_FROM_ALL FALSE
    )
endfunction()

# Test configuration with generator expressions
function(configure_test_environment)
    # Enable address sanitizer in debug builds
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(nlm_test_all PRIVATE -fsanitize=address -fno-omit-frame-pointer)
            target_link_libraries(nlm_test_all PRIVATE -fsanitize=address)
        endif()
    endif()
    
    # Thread sanitizer for Linux
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(nlm_test_all PRIVATE -fsanitize=thread)
            target_link_libraries(nlm_test_all PRIVATE -fsanitize=thread)
        endif()
    endif()
endfunction()

# Performance testing setup
function(setup_performance_testing)
    # Add performance test targets
    add_executable(nlm_performance_benchmark
        tests/performance_benchmark.cpp
    )
    target_link_libraries(nlm_performance_benchmark PRIVATE nlm_core)
    target_compile_features(nlm_performance_benchmark PRIVATE cxx_std_20)
    
    add_test(NAME nlm_performance_benchmark
        COMMAND nlm_performance_benchmark
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    
    # Add memory profiling if available
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(nlm_performance_benchmark PRIVATE -g -O0)
        target_link_libraries(nlm_performance_benchmark PRIVATE valgrind/memcheck)
    endif()
endfunction()

# Test coverage setup
function(setup_test_coverage)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            # Find coverage tools
            find_program(GCOV_OBJECT_FORMAT_GNU gcov-tool)
            if (GCOV_OBJECT_FORMAT_GNU)
                target_compile_options(nlm_test_all PRIVATE --coverage)
                target_link_libraries(nlm_test_all PRIVATE --coverage)
                
                # Add coverage target
                add_custom_target(coverage
                    COMMAND ${GCOV_OBJECT_FORMAT_GNU} -o ${CMAKE_BINARY_DIR}/coverage ${CMAKE_BINARY_DIR}/nlm_test_all
                    COMMAND gcov -b -c -d ${CMAKE_BINARY_DIR}/nlm_test_all.gcno
                    COMMAND gcovr --exclude test_*.cpp --exclude *gtest* --print-summary
                    COMMENT "Generate test coverage report"
                    VERBATIM
                )
            endif()
        endif()
    endif()
endfunction()

# Automated test reporting
function(add_test_reporting)
    # Add test time reporting
    add_custom_target(test_timing_report
        COMMAND ${CMAKE_COMMAND} -E echo "Running tests with timing..."
        COMMAND ctest --output-on-failure --output-junit ${CMAKE_BINARY_DIR}/test_results.xml
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_test_report.cmake
        COMMENT "Generate comprehensive test report"
        VERBATIM
    )
    
    # Add test dependency analysis
    add_custom_target(test_dependencies
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/analyze_test_deps.cmake
        COMMENT "Analyze test dependencies"
        VERBATIM
    )
endfunction()