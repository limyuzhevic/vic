#!/bin/bash
# NLM Test Runner
# Simple test framework for NLM (Neural Learning Machine)

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test results
PASSED=0
FAILED=0
TOTAL=0

# Function to run a single test
run_test() {
    local test_name="$1"
    local executable="$2"
    
    echo -e "\n${YELLOW}Running test: ${test_name}${NC}"
    echo "Command: ${executable}"
    
    if [ -x "$executable" ]; then
        if "$executable" 2>&1; then
            echo -e "${GREEN}✓ PASSED${NC}"
            ((PASSED++))
        else
            echo -e "${RED}✗ FAILED${NC}"
            ((FAILED++))
        fi
    else
        echo -e "${RED}⚠ SKIPPED (executable not found)${NC}"
    fi
    
    ((TOTAL++))
}

# Function to run benchmark mode
run_benchmark() {
    echo -e "\n${YELLOW}Running benchmarks...${NC}"
    
    # Check for benchmark executable
    if [ -x "./nlm_benchmark" ]; then
        if "./nlm_benchmark" 2>&1; then
            echo -e "${GREEN}✓ Benchmarks PASSED${NC}"
            ((PASSED++))
        else
            echo -e "${RED}✗ Benchmarks FAILED${NC}"
            ((FAILED++))
        fi
    else
        echo -e "${RED}⚠ Benchmarks not available${NC}"
    fi
    
    ((TOTAL++))
}

# Function to run integration test
run_integration_test() {
    echo -e "\n${YELLOW}Running integration test...${NC}"
    
    # Check for integration demo executable
    if [ -x "./nlm_phase6_demo" ]; then
        if "./nlm_phase6_demo" 2>&1; then
            echo -e "${GREEN}✓ Integration test PASSED${NC}"
            ((PASSED++))
        else
            echo -e "${RED}✗ Integration test FAILED${NC}"
            ((FAILED++))
        fi
    else
        echo -e "${RED}⚠ Integration demo not available${NC}"
    fi
    
    ((TOTAL++))
}

# Function to run Phase 3 demo test
run_phase3_test() {
    echo -e "\n${YELLOW}Running Phase 3 demo test...${NC}"
    
    # Check for Phase 3 demo executable
    if [ -x "./nlm_phase3_demo" ]; then
        if "./nlm_phase3_demo 5 200" 2>&1; then
            echo -e "${GREEN}✓ Phase 3 demo PASSED${NC}"
            ((PASSED++))
        else
            echo -e "${RED}✗ Phase 3 demo FAILED${NC}"
            ((FAILED++))
        fi
    else
        echo -e "${RED}⚠ Phase 3 demo not available${NC}"
    fi
    
    ((TOTAL++))
}

# Function to check for required files
check_requirements() {
    echo -e "\n${YELLOW}Checking requirements...${NC}"
    
    # Check for main executable
    if [ -x "./nlm" ]; then
        echo -e "  ${GREEN}✓ Main executable (nlm) found${NC}"
    else
        echo -e "  ${YELLOW}⚠ Main executable (nlm) not found - building...${NC}"
        # Try to build with CMake
        if command -v cmake &> /dev/null; then
            echo "  Attempting to build with CMake..."
            mkdir -p build
            cd build
            if cmake .. -DCMAKE_BUILD_TYPE=Release &> /dev/null && make -j4 &> /dev/null; then
                echo -e "  ${GREEN}✓ Build successful${NC}"
                cd ..
            else
                echo -e "  ${RED}✗ Build failed${NC}"
                return 1
            fi
        else
            echo -e "  ${RED}✗ CMake not found - cannot build${NC}"
            return 1
        fi
    fi
    
    # Check for demos
    echo -e "  Demos found:"
    for demo in "./nlm_phase3_demo" "./nlm_phase4_demo" "./nlm_phase6_demo"; do
        if [ -x "$demo" ]; then
            echo -e "    ${GREEN}✓ $demo${NC}"
        else
            echo -e "    ${YELLOW}⚠ $demo (not built)${NC}"
        fi
    done
    
    # Check for Python bindings
    if command -v python3 &> /dev/null; then
        if python3 -c "import pynlm" 2>&1; then
            echo -e "  ${GREEN}✓ Python bindings available${NC}"
        else
            echo -e "  ${YELLOW}⚠ Python bindings not available${NC}"
        fi
    fi
}

# Main test runner function
run_tests() {
    echo "========================================"
    echo "NLM (Neural Learning Machine) Test Suite"
    echo "========================================"
    
    # Check requirements first
    check_requirements || exit 1
    
    echo -e "\n${YELLOW}Starting test suite...${NC}"
    
    # Run individual tests
    run_test "Neuron Dynamics" "./nlm"
    run_test "Phase 3 Demo" "./nlm_phase3_demo"
    run_test "Phase 6 Demo" "./nlm_phase6_demo"
    run_benchmark
    run_integration_test
    run_phase3_test
    
    # Print summary
    echo -e "\n========================================"
    echo "Test Summary"
    echo "========================================"
    echo "Total tests: ${TOTAL}"
    echo -e "${GREEN}Passed: ${PASSED}${NC}"
    echo -e "${RED}Failed: ${FAILED}${NC}"
    
    if [ $FAILED -eq 0 ]; then
        echo -e "\n${GREEN}🎉 All tests passed!${NC}"
        return 0
    else
        echo -e "\n${RED}❌ Some tests failed.${NC}"
        return 1
    fi
}

# Run tests
run_tests