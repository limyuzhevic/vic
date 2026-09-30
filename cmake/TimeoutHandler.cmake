# Timeout Handler CMake Script
# This script handles test timeouts and provides timeout management.

cmake_minimum_required(VERSION 3.16)

# Timeout configuration
set(NLM_TEST_TIMEOUT ${NLM_TEST_TIMEOUT} CACHE STRING "Test timeout in seconds")
set(NLM_TIMEOUT_WARNING_THRESHOLD ${NLM_TIMEOUT_WARNING_THRESHOLD} CACHE STRING "Warning threshold for test timeout")

# Timeout monitoring
set(TIMEOUT_LOG_FILE ${CMAKE_BINARY_DIR}/timeout.log)
set(TIMEOUT_STATS_FILE ${CMAKE_BINARY_DIR}/timeout_stats.json)

# Create timeout monitoring script
set(TIMEOUT_SCRIPT ${CMAKE_BINARY_DIR}/timeout_handler.cmake)

file(WRITE ${TIMEOUT_SCRIPT}
"# Timeout handler for NLM tests
// This script monitors and manages test timeouts.

// Timeout management functions
function(check_test_timeout test_name start_time current_time)
    math(EXPR elapsed_time "${current_time} - ${start_time}")
    
    if(elapsed_time GREATER ${NLM_TEST_TIMEOUT})
        message(FATAL_ERROR \"Test ${test_name} timed out after ${elapsed_time} seconds (limit: ${NLM_TEST_TIMEOUT}s)\")
    endif()
    
    if(elapsed_time GREATER ${NLM_TIMEOUT_WARNING_THRESHOLD})
        message(WARNING \"Test ${test_name} approaching timeout: ${elapsed_time}s (warning threshold: ${NLM_TIMEOUT_WARNING_THRESHOLD}s)\")
    endif()
endfunction()
")

# Create timeout statistics
file(WRITE ${TIMEOUT_STATS_FILE}
"{
    \"timestamp\": \"${CMAKE_TIMESTAMP}\",
    \"test_timeout\": ${NLM_TEST_TIMEOUT},
    \"warning_threshold\": ${NLM_TIMEOUT_WARNING_THRESHOLD},
    \"total_tests_run\": ${NLM_TOTAL_TESTS},
    \"timeouts_occurred\": ${NLM_TIMEOUTS_OCCURRED},
    \"average_test_time\": ${NLM_AVG_TEST_TIME}
}
")

# Create timeout log
set(TIMEOUT_LOG_CONTENT
"# NLM Test Timeout Log
# Generated: ${CMAKE_TIMESTAMP}

Test Configuration:
  Test Timeout: ${NLM_TEST_TIMEOUT} seconds
  Warning Threshold: ${NLM_TIMEOUT_WARNING_THRESHOLD} seconds
  Total Tests: ${NLM_TOTAL_TESTS}

Timeout Events:
  // Timeout events would be logged here

Summary:
  Total Timeouts: ${NLM_TIMEOUTS_OCCURRED}
  Timeout Rate: ${NLM_TIMEOUT_RATE}%
""
)

file(WRITE ${TIMEOUT_LOG_FILE} "${TIMEOUT_LOG_CONTENT}")

message(STATUS "Timeout handler configured:")
message(STATUS "  Log file: ${TIMEOUT_LOG_FILE}")
message(STATUS "  Stats file: ${TIMEOUT_STATS_FILE}")
message(STATUS "  Timeout: ${NLM_TEST_TIMEOUT} seconds")
message(STATUS "  Warning threshold: ${NLM_TIMEOUT_WARNING_THRESHOLD} seconds")
