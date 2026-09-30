# Aggregate Test Results CMake Script
# This script aggregates test results from multiple test runs.

cmake_minimum_required(VERSION 3.16)

# Test results directory
set(TEST_RESULTS_DIR ${CMAKE_BINARY_DIR}/test_results)

# Create test results summary
set(SUMMARY_FILE ${TEST_RESULTS_DIR}/test_summary.txt)

# Generate summary
file(WRITE ${SUMMARY_FILE}
"Test Results Summary
=====================
Generated: ${CMAKE_TIMESTAMP}

Test Configuration:
  Build Type: ${CMAKE_BUILD_TYPE}
  C++ Standard: ${CMAKE_CXX_STANDARD}
  Platform: ${CMAKE_SYSTEM_NAME}
  Architecture: ${CMAKE_SIZEOF_VOID_P}-bit
  Python: ${Python_VERSION}

Build Configuration:
  NLM_ENABLE_TESTS: ${NLM_ENABLE_TESTS}
  NLM_ENABLE_PYTHON_BINDINGS: ${NLM_ENABLE_PYTHON_BINDINGS}
  NLM_ENABLE_PHASE3_EXPERIMENTS: ${NLM_ENABLE_PHASE3_EXPERIMENTS}
  NLM_ENABLE_PHASE4_EXPERIMENTS: ${NLM_ENABLE_PHASE4_EXPERIMENTS}

Test Execution:
  Total Tests: ${NLM_TOTAL_TESTS}
  Passed Tests: ${NLM_PASSED_TESTS}
  Failed Tests: ${NLM_FAILED_TESTS}
  Success Rate: ${NLM_SUCCESS_RATE}%
  Total Time: ${NLM_TOTAL_TIME} seconds
  Average Time per Test: ${NLM_AVG_TIME_PER_TEST} seconds

Performance:
  Tests per Second: ${NLM_TESTS_PER_SECOND}
  Memory Usage: ${NLM_MEMORY_USAGE} MB

Recommendations:
  // Recommendations would be generated here based on test results
""
)

# Create test results CSV
set(CSV_RESULTS ${TEST_RESULTS_DIR}/test_results.csv)

file(WRITE ${CSV_RESULTS}
"test_name,test_status,test_time,memory_usage,coverage
// Test results would be listed here
")

# Create test results HTML
set(HTML_RESULTS ${TEST_RESULTS_DIR}/test_results.html)

file(WRITE ${HTML_RESULTS}
"<!DOCTYPE html>
<html>
<head>
    <title>NLM Test Results</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; }
        .summary { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }
        .passed { color: green; }
        .failed { color: red; }
        .warning { color: orange; }
    </style>
</head>
<body>
    <h1>NLM Test Results</h1>
    <div class=\"summary\">
        <h2>Summary</h2>
        <p><strong>Generated:</strong> ${CMAKE_TIMESTAMP}</p>
        <p><strong>Total Tests:</strong> ${NLM_TOTAL_TESTS}</p>
        <p><strong>Passed:</strong> <span class=\"passed\">${NLM_PASSED_TESTS}</span></p>
        <p><strong>Failed:</strong> <span class=\"failed\">${NLM_FAILED_TESTS}</span></p>
        <p><strong>Success Rate:</strong> ${NLM_SUCCESS_RATE}%</p>
        <p><strong>Total Time:</strong> ${NLM_TOTAL_TIME} seconds</p>
    </div>
</body>
</html>
")

message(STATUS "Test results aggregated:")
message(STATUS "  Summary: ${SUMMARY_FILE}")
message(STATUS "  CSV: ${CSV_RESULTS}")
message(STATUS "  HTML: ${HTML_RESULTS}")
