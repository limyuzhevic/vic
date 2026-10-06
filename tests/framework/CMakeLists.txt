# CMake integration for NLM Test Framework
# This file enables integration with CMake build system

cmake_minimum_required(VERSION 3.16)

# Find NLM core library
find_package(nlm_core REQUIRED)

# Test framework configuration
option(NLM_ENABLE_TEST_FRAMEWORK "Enable modern test framework" ON)
option(NLM_TEST_VERBOSE "Enable verbose test output" OFF)
option(NLM_TEST_COVERAGE "Enable test coverage analysis" OFF)
option(NLM_TEST_PARALLEL "Enable parallel test execution" OFF)
option(NLM_ENABLE_LEGACY_TESTS "Enable legacy test compatibility" ON)

# Test framework source files
set(TEST_FRAMEWORK_SOURCES
    framework/TestFramework.cpp
    framework/TestInfrastructure.cpp
    framework/TestReporting.cpp
)

# Create test framework library
add_library(nlm_test_framework STATIC ${TEST_FRAMEWORK_SOURCES})

# Target properties
target_include_directories(nlm_test_framework PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/framework
    ${nlm_core_INCLUDE_DIRS}
)

target_compile_features(nlm_test_framework PUBLIC cxx_std_20)

# Link dependencies
if(NLM_ENABLE_LEGACY_TESTS)
    target_link_libraries(nlm_test_framework PUBLIC nlm_core)
endif()

# Test framework configuration
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/framework/TestConfig.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/framework/TestConfig.h
)

# Test discovery
function(find_nlm_tests)
    # Find all test files in the tests directory
    file(GLOB TEST_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/**/*.cpp"
    )
    
    # Filter test files
    set(UNIT_TESTS "")
    set(INTEGRATION_TESTS "")
    set(PERFORMANCE_TESTS "")
    set(MEMORY_TESTS "")
    set(EDGE_CASE_TESTS "")
    set(STRESS_TESTS "")
    set(PROPERTY_TESTS "")
    set(REGRESSION_TESTS "")
    
    foreach(test_file ${TEST_FILES})
        get_filename_component(test_name ${test_file} NAME_WE)
        
        if(test_name MATCHES "^test_" AND NOT test_name STREQUAL "test_main")
            if(test_name MATCHES "unit|basic|simple")
                list(APPEND UNIT_TESTS ${test_file})
            elseif(test_name MATCHES "integration|system")
                list(APPEND INTEGRATION_TESTS ${test_file})
            elseif(test_name MATCHES "performance|bench|perf")
                list(APPEND PERFORMANCE_TESTS ${test_file})
            elseif(test_name MATCHES "memory|leak")
                list(APPEND MEMORY_TESTS ${test_file})
            elseif(test_name MATCHES "edge|boundary")
                list(APPEND EDGE_CASE_TESTS ${test_file})
            elseif(test_name MATCHES "stress|load")
                list(APPEND STRESS_TESTS ${test_file})
            elseif(test_name MATCHES "property|quickcheck")
                list(APPEND PROPERTY_TESTS ${test_file})
            else()
                list(APPEND REGRESSION_TESTS ${test_file})
            endif()
        endif()
    endforeach()
    
    # Create test targets
    if(UNIT_TESTS)
        add_executable(nlm_unit_tests ${UNIT_TESTS})
        target_link_libraries(nlm_unit_tests PRIVATE nlm_test_framework)
        add_test(NAME nlm_unit_tests COMMAND nlm_unit_tests)
    endif()
    
    if(INTEGRATION_TESTS)
        add_executable(nlm_integration_tests ${INTEGRATION_TESTS})
        target_link_libraries(nlm_integration_tests PRIVATE nlm_test_framework)
        add_test(NAME nlm_integration_tests COMMAND nlm_integration_tests)
    endif()
    
    if(PERFORMANCE_TESTS)
        add_executable(nlm_performance_tests ${PERFORMANCE_TESTS})
        target_link_libraries(nlm_performance_tests PRIVATE nlm_test_framework)
        add_test(NAME nlm_performance_tests COMMAND nlm_performance_tests)
    endif()
    
    if(MEMORY_TESTS)
        add_executable(nlm_memory_tests ${MEMORY_TESTS})
        target_link_libraries(nlm_memory_tests PRIVATE nlm_test_framework)
        add_test(NAME nlm_memory_tests COMMAND nlm_memory_tests)
    endif()
    
    # Create custom test targets
    add_custom_target(nlm_tests)
    add_dependencies(nlm_tests nlm_unit_tests nlm_integration_tests)
    
    if(PERFORMANCE_TESTS)
        add_dependencies(nlm_tests nlm_performance_tests)
    endif()
    
    if(MEMORY_TESTS)
        add_dependencies(nlm_tests nlm_memory_tests)
    endif()
    
    # Test framework target
    add_custom_target(test_framework ALL DEPENDS nlm_test_framework)
endfunction()

# Test framework reporting
function(generate_test_reports)
    set(options)
    set(oneValueArgs OUTPUT_DIR FORMAT)
    set(multiValueArgs)
    cmake_parse_arguments(TEST_REPORT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    if(NOT TEST_REPORT_OUTPUT_DIR)
        set(TEST_REPORT_OUTPUT_DIR "${CMAKE_BINARY_DIR}/test_reports")
    endif()
    
    if(NOT TEST_REPORT_FORMAT)
        set(TEST_REPORT_FORMAT "text")
    endif()
    
    message("Generating test reports in ${TEST_REPORT_OUTPUT_DIR} with format ${TEST_REPORT_FORMAT}")
    
    # Create output directory
    file(MAKE_DIRECTORY ${TEST_REPORT_OUTPUT_DIR})
    
    # Generate different report formats
    if(TEST_REPORT_FORMAT STREQUAL "html")
        add_custom_target(generate_html_report ALL
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/framework/GenerateReports.cmake
            COMMENT "Generating HTML test report"
        )
        add_dependencies(generate_html_report nlm_tests)
    elseif(TEST_REPORT_FORMAT STREQUAL "xml")
        add_custom_target(generate_xml_report ALL
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/framework/GenerateReports.cmake
            COMMENT "Generating XML test report"
        )
        add_dependencies(generate_xml_report nlm_tests)
    else()
        # Text report is generated by default test runner
        add_custom_target(generate_text_report ALL
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/framework/GenerateReports.cmake
            COMMENT "Generating text test report"
        )
        add_dependencies(generate_text_report nlm_tests)
    endif()
endfunction()

# Test framework initialization
function(initialize_test_framework)
    message("Initializing NLM Test Framework")
    
    # Configure based on build type
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message("Debug build - enabling debug features for tests")
        set(NLM_TEST_VERBOSE ON CACHE BOOL "Enable verbose test output" FORCE)
    endif()
    
    # Initialize test framework
    add_custom_target(initialize_tests ALL
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/framework/InitializeFramework.cmake
        COMMENT "Initializing test framework"
    )
endfunction()

# Test framework cleanup
function(cleanup_test_framework)
    message("Cleaning up test framework")
    
    add_custom_target(cleanup_tests
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/framework/CleanupFramework.cmake
        COMMENT "Cleaning up test framework"
    )
endfunction()

# Test configuration template
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/framework/TestConfig.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/framework/TestConfig.h
    @ONLY
)

# Installation
install(TARGETS nlm_test_framework
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/framework/TestFramework.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/framework/TestConfig.h
    DESTINATION include/nlm/test
)

# Summary
message(STATUS "NLM Test Framework configured")
message(STATUS "  - Unit tests target: nlm_unit_tests")
message(STATUS "  - Integration tests target: nlm_integration_tests")
message(STATUS "  - Framework library: nlm_test_framework")

# Auto-discovery
set(NLM_TEST_DISCOVERY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
find_nlm_tests()
initialize_test_framework()
cleanup_test_framework()
