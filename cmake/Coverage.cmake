# NLM Coverage Configuration
# Code coverage setup and reporting for NLM.

# Check for coverage tools
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Find gcov
    find_program(GCOV_EXECUTABLE gcov)
    find_program(LCOV_EXECUTABLE lcov)
    find_program(GENHTML_EXECUTABLE genhtml)
    
    if(GCOV_EXECUTABLE AND LCOV_EXECUTABLE AND GENHTML_EXECUTABLE)
        set(NLM_HAS_COVERAGE_TOOLS TRUE)
        message(STATUS "Coverage tools found: gcov, lcov, genhtml")
    else()
        message(WARNING "Coverage tools not found - coverage reporting disabled")
    endif()
endif()

# Coverage configuration options
option(NLM_COVERAGE_ENABLED "Enable code coverage collection" ON)
option(NLM_COVERAGE_DETAILED "Enable detailed coverage reporting" OFF)
option(NLM_COVERAGE_HTML "Generate HTML coverage report" ON)
option(NLM_COVERAGE_XML "Generate XML coverage report" OFF)
option(NLM_COVERAGE_TEXT "Generate text coverage report" ON)

# Coverage thresholds
set(NLM_COVERAGE_MINIMUM acceptable_coverage 80 CACHE STRING "Minimum acceptable coverage percentage")
set(NLM_COVERAGE_WARNING_THRESHOLD 70 CACHE STRING "Warning threshold for coverage")

# Coverage data directories
set(NLM_COVERAGE_BASE_DIR ${CMAKE_BINARY_DIR}/coverage CACHE PATH "Base directory for coverage data")
set(NLM_COVERAGE_EXECUTABLES_DIR ${NLM_COVERAGE_BASE_DIR}/executables CACHE PATH "Directory for executable coverage data")
set(NLM_COVERAGE_INCLUDES_DIR ${NLM_COVERAGE_BASE_DIR}/includes CACHE PATH "Directory for include coverage data")

# Create coverage directories
file(MAKE_DIRECTORY ${NLM_COVERAGE_EXECUTABLES_DIR})
file(MAKE_DIRECTORY ${NLM_COVERAGE_INCLUDES_DIR})

# Coverage function setup
function(setup_nlm_coverage target_name)
    if(NOT NLM_HAS_COVERAGE_TOOLS)
        return()
    endif()
    
    # Add coverage flags to target
    target_compile_options(${target_name} PRIVATE
        --coverage
    )
    
    target_link_options(${target_name} PRIVATE
        --coverage
    )
    
    # Add coverage dependencies
    if(TARGET nlm_core)
        target_compile_options(nlm_core PRIVATE --coverage)
        target_link_options(nlm_core PRIVATE --coverage)
    endif()
endfunction()

# Coverage data collection
function(collect_coverage_data target_name)
    if(NOT NLM_HAS_COVERAGE_TOOLS OR NOT NLM_COVERAGE_ENABLED)
        return()
    endif()
    
    # Create coverage data collection script
    set(COVERAGE_SCRIPT ${NLM_COVERAGE_BASE_DIR}/collect_coverage.cmake)
    file(WRITE ${COVERAGE_SCRIPT}
        "# Coverage collection script\r\n"
        "cmake_minimum_required(VERSION 3.16)\r\n\r\n"
        "# Collect coverage data from executables\r\n"
        "execute_process(\r\n"
        "    COMMAND ${LCOV_EXECUTABLE} --directory ${CMAKE_CURRENT_BINARY_DIR} --capture --output-file ${LCOV_COVERAGE_DATA}\r\n"
        "    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}\r\n"
        ")\r\n\r\n"
        "# Generate HTML report\r\n"
        "if(${NLM_COVERAGE_HTML})\r\n"
        "    execute_process(\r\n"
        "        COMMAND ${GENHTML_EXECUTABLE} ${LCOV_COVERAGE_DATA} --output-directory ${LCOV_HTML_REPORT}\r\n"
        "        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}\r\n"
        "    )\r\n"
        "endif()\r\n"
    )
    
    # Execute coverage collection
    execute_process(
        COMMAND ${CMAKE_COMMAND} -P ${COVERAGE_SCRIPT}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        RESULT_VARIABLE coverage_result
    )
    
    if(coverage_result EQUAL 0)
        message(STATUS "Coverage data collected successfully")
    else()
        message(WARNING "Coverage data collection failed")
    endif()
endfunction()

# Coverage report generation
function(generate_nlm_coverage_report)
    if(NOT NLM_HAS_COVERAGE_TOOLS OR NOT NLM_COVERAGE_ENABLED)
        return()
    endif()
    
    # Generate coverage report
    set(COVERAGE_REPORT_DIR ${NLM_COVERAGE_BASE_DIR}/reports)
    file(MAKE_DIRECTORY ${COVERAGE_REPORT_DIR})
    
    # Generate text report
    if(${NLM_COVERAGE_TEXT})
        execute_process(
            COMMAND ${GCOV_EXECUTABLE} --text
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            OUTPUT_FILE ${COVERAGE_REPORT_DIR}/coverage.txt
        )
        message(STATUS "Text coverage report generated: ${COVERAGE_REPORT_DIR}/coverage.txt")
    endif()
    
    # Generate HTML report
    if(${NLM_COVERAGE_HTML})
        execute_process(
            COMMAND ${GENHTML_EXECUTABLE} ${NLM_COVERAGE_BASE_DIR}/lcov.info
                    --output-directory ${COVERAGE_REPORT_DIR}/html
        )
        message(STATUS "HTML coverage report generated: ${COVERAGE_REPORT_DIR}/html/index.html")
    endif()
    
    # Generate XML report
    if(${NLM_COVERAGE_XML})
        execute_process(
            COMMAND ${GENHTML_EXECUTABLE} ${NLM_COVERAGE_BASE_DIR}/lcov.info
                    --output-directory ${COVERAGE_REPORT_DIR}/xml
                    --xml
        )
        message(STATUS "XML coverage report generated: ${COVERAGE_REPORT_DIR}/xml/index.xml")
    endif()
    
    # Check coverage against thresholds
    execute_process(
        COMMAND ${LCOV_EXECUTABLE} --summary ${NLM_COVERAGE_BASE_DIR}/lcov.info
        OUTPUT_VARIABLE coverage_summary
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    
    # Parse coverage summary and check against thresholds
    # This is a simplified check - in practice you'd use more robust parsing
    message(STATUS "Coverage summary:")
    message(STATUS "  ${coverage_summary}")
endfunction()

# Coverage validation
function(validate_nlm_coverage)
    if(NOT NLM_HAS_COVERAGE_TOOLS OR NOT NLM_COVERAGE_ENABLED)
        return()
    endif()
    
    # Check if coverage meets minimum requirements
    if(EXISTS ${NLM_COVERAGE_BASE_DIR}/lcov.info)
        execute_process(
            COMMAND ${LCOV_EXECUTABLE} --summary ${NLM_COVERAGE_BASE_DIR}/lcov.info
            OUTPUT_VARIABLE coverage_summary
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        
        # Parse coverage percentage (simplified)
        string(REGEX REPLACE ".*lines:\s+([0-9.]+)%.*" "\1" COVERAGE_PERCENTAGE "${coverage_summary}")
        
        if(COVERAGE_PERCENTAGE GREATER_EQUAL ${NLM_COVERAGE_MINIMUM})
            message(STATUS "✓ Coverage requirement met: ${COVERAGE_PERCENTAGE}% >= ${NLM_COVERAGE_MINIMUM}%")
        else()
            message(FATAL_ERROR "✗ Coverage requirement not met: ${COVERAGE_PERCENTAGE}% < ${NLM_COVERAGE_MINIMUM}%")
        endif()
    endif()
endfunction()

# Setup coverage for specific targets
if(TARGET nlm_test_core)
    setup_nlm_coverage(nlm_test_core)
endif()

if(TARGET nlm_test_brain)
    setup_nlm_coverage(nlm_test_brain)
endif()

if(TARGET nlm_test_plasticity)
    setup_nlm_coverage(nlm_test_plasticity)
endif()

if(TARGET nlm_test_memory)
    setup_nlm_coverage(nlm_test_memory)
endif()

message(STATUS "Coverage configuration complete:")
message(STATUS "  Coverage tools: ${NLM_HAS_COVERAGE_TOOLS}")
message(STATUS "  Coverage enabled: ${NLM_COVERAGE_ENABLED}")
message(STATUS "  Coverage base dir: ${NLM_COVERAGE_BASE_DIR}")
