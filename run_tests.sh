#!/usr/bin/env bash
# NLM Test Runner Script
# This script runs the NLM tests using the improved test framework

# Make sure the script is executable
chmod 755 "$0"

set -e

cd "$(dirname "$0")/.."

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
VERBOSE=false
PARALLEL=false
TIMEOUT=300
OUTPUT_FORMAT="text"
TEST_FILTER=""
REPORT_FILE="test_report.json"
REPORT_HTML="test_report.html"

# Test framework paths
CPP_TEST_PATH="python/pynlm/example_tests.cpp"
PYTHON_TEST_PATH="python/pynlm/example_tests.py"

# Print usage
print_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help              Show this help message"
    echo "  -v, --verbose           Enable verbose output"
    echo "  -p, --parallel          Run tests in parallel (if supported)"
    echo "  -t, --timeout SECONDS   Set timeout for test execution (default: 300)"
    echo "  -f, --filter PATTERN    Filter tests by name pattern"
    echo "  -o, --output FORMAT     Output format: text, json, html (default: text)"
    echo "  -r, --report FILE       Write test report to file"
    echo "  --cpp                  Run C++ tests"
    echo "  --python                Run Python tests (default)"
    echo "  --all                   Run all tests (C++ and Python)"
    echo ""
    echo "Examples:"
    echo "  $0                                    Run all Python tests"
    echo "  $0 --cpp                             Run C++ tests only"
    echo "  $0 --filter "brain"                  Run tests containing 'brain'"
    echo "  $0 -o json -r report.json            Run tests and save JSON report"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            print_usage
            exit 0
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -p|--parallel)
            PARALLEL=true
            shift
            ;;
        -t|--timeout)
            TIMEOUT="$2"
            shift 2
            ;;
        -f|--filter)
            TEST_FILTER="$2"
            shift 2
            ;;
        -o|--output)
            OUTPUT_FORMAT="$2"
            shift 2
            ;;
        -r|--report)
            REPORT_FILE="$2"
            shift 2
            ;;
        --cpp)
            RUN_CPP=true
            shift
            ;;
        --python)
            RUN_PYTHON=true
            shift
            ;;
        --all)
            RUN_CPP=true
            RUN_PYTHON=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            print_usage
            exit 1
            ;;
    esac
done

# Default: run Python tests if no flags specified
if [[ -z "${RUN_CPP:-}" && -z "${RUN_PYTHON:-}" ]]; then
    RUN_PYTHON=true
fi

# Check for required tools
check_tools() {
    echo "Checking required tools..."
    
    # Check for g++
    if ! command -v g++ &> /dev/null; then
        echo -e "${RED}Error: g++ compiler not found${NC}"
        exit 1
    fi
    
    # Check for cmake
    if ! command -v cmake &> /dev/null; then
        echo -e "${RED}Error: cmake not found${NC}"
        exit 1
    fi
    
    # Check for python
    if ! command -v python3 &> /dev/null; then
        echo -e "${RED}Error: python3 not found${NC}"
        exit 1
    fi
    
    # Check for pip
    if ! command -v pip3 &> /dev/null; then
        echo -e "${RED}Error: pip3 not found${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}All required tools found${NC}"
}

# Run C++ tests
run_cpp_tests() {
    echo -e "${YELLOW}Running C++ tests...${NC}"
    
    # Create build directory
    mkdir -p build_test
    cd build_test
    
    # Configure with CMake
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_CXX_STANDARD=17 \
          -DNLM_TESTING=ON \
          ..
    
    # Build tests
    make -j$(nproc) nlm_tests
    
    # Run tests
    if [[ "$PARALLEL" == true ]]; then
        ./nlm_tests --parallel
    else
        ./nlm_tests
    fi
    
    # Capture test results
    if [[ -f "test_report.json" ]]; then
        echo -e "${GREEN}C++ tests completed. Report saved to test_report.json${NC}"
    fi
    
    cd ..
}

# Run Python tests
run_python_tests() {
    echo -e "${YELLOW}Running Python tests...${NC}"
    
    # Install test dependencies
    echo "Installing test dependencies..."
    pip3 install pytest numpy pytest-json-report
    
    # Run Python tests
    python3 -m pytest python/pynlm/example_tests.py \
        -v \
        --tb=short \
        --timeout=$TIMEOUT \
        --json-report \
        --json-report-file="$REPORT_FILE" \
        ${TEST_FILTER:+-k "$TEST_FILTER"}
    
    echo -e "${GREEN}Python tests completed${NC}"
}

# Generate comprehensive test report
generate_report() {
    echo -e "${YELLOW}Generating test report...${NC}"
    
    case $OUTPUT_FORMAT in
        "json")
            echo "JSON report generated: $REPORT_FILE"
            ;;
        "html")
            # Convert JSON report to HTML (if tools available)
            if command -v jq &> /dev/null; then
                echo "Converting JSON report to HTML..."
                python3 -c "
import json
import sys
from datetime import datetime

# Read JSON report
with open('$REPORT_FILE', 'r') as f:
    report = json.load(f)

# Generate simple HTML report
html = f'''
<!DOCTYPE html>
<html>
<head>
    <title>NLM Test Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; }}
        .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
        .test-suite {{ margin: 10px 0; padding: 10px; border: 1px solid #ddd; border-radius: 3px; }}
        .test-case {{ margin: 5px 0; padding: 5px; background-color: #f9f9f9; }}
        .passed {{ color: green; }}
        .failed {{ color: red; }}
        .error {{ color: darkred; }}
        .skipped {{ color: orange; }}
    </style>
</head>
<body>
    <div class='header'>
        <h1>NLM Test Report</h1>
        <p>Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
    </div>
    
    <h2>Summary</h2>
    <p>Total Tests: {report.get('total_tests', 0)}</p>
    <p>Passed: {report.get('passed_tests', 0)}</p>
    <p>Failed: {report.get('failed_tests', 0)}</p>
    <p>Skipped: {report.get('skipped_tests', 0)}</p>
    <p>Error: {report.get('error_tests', 0)}</p>
    
    <h2>Test Results</h2>
''';

for suite_name, suite_data in report.get('suites', {}).items():
    html += f'''
    <div class='test-suite'>
        <h3>{suite_name}</h3>
        <p>Description: {suite_data.get('description', '')}</p>
        <p>Result: {suite_data.get('result', 'UNKNOWN')}</p>
''';

    for test_name, test_data in suite_data.get('tests', {}).items():
        status_class = test_data.get('result', '').lower();
        html += f'''
        <div class='test-case'>
            <strong>{test_name}</strong> <span class='{status_class}'>({test_data.get('result', '')})</span>
            <p>{test_data.get('description', '')}</p>
        </div>
        ''';

    html += '</div>';

html += '''
</body>
</html>
''';

with open('$REPORT_HTML', 'w') as f:
    f.write(html)

print(f"HTML report generated: $REPORT_HTML")
else
    echo "jq not found, skipping HTML report generation"
fi
            "
                ;;
        *)
            echo "Unknown output format: $OUTPUT_FORMAT"
            ;;
    esac
}

# Main execution
main() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE} NLM Test Runner${NC}"
    echo -e "${BLUE}========================================${NC}"
    
    check_tools
    
    # Run tests based on flags
    if [[ -n "${RUN_CPP:-}" ]]; then
        run_cpp_tests
    fi
    
    if [[ -n "${RUN_PYTHON:-}" ]]; then
        run_python_tests
    fi
    
    # Generate report
    generate_report
    
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN} Test run completed${NC}"
    echo -e "${GREEN}========================================${NC}"
}

# Run main function
main
