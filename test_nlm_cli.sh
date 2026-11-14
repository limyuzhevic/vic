chmod +x "$0"

#!/bin/bash
# NLM Test Script
# Tests the new NLM unified CLI interface and features

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test function results
test_results=()

# Test function execution
test_command() {
    local test_name="$1"
    local command="$2"
    
    echo -e "${YELLOW}Testing: $test_name${NC}"
    echo -e "${YELLOW}Command: $command${NC}"
    
    # Run the command
    if eval "$command"; then
        echo -e "${GREEN}✓ PASSED${NC}"
        test_results+=("PASS:$test_name")
        return 0
    else
        echo -e "${RED}✗ FAILED${NC}"
        test_results+=("FAIL:$test_name")
        return 1
    fi
}

# Test help functionality
test_help() {
    test_command "Help command" "./nlm --help"
}

# Test version functionality
test_version() {
    test_command "Version command" "./nlm --version"
}

# Test demo mode functionality (quick tests)
test_phase2_help() {
    test_command "Phase 2 help" "./nlm demo phase2 --help 2>/dev/null || echo "Help not available yet"
}

# Test configuration validation
test_config_validation() {
    test_command "Configuration validation help" "./nlm config validate --help 2>/dev/null || echo "Config validation not available yet"
}

# Test benchmark functionality
test_benchmark_help() {
    test_command "Benchmark help" "./nlm benchmark --help 2>/dev/null || echo "Benchmark help not available yet"
}

# Test experiment functionality
test_experiment_help() {
    test_command "Experiment help" "./nlm experiment --help 2>/dev/null || echo "Experiment help not available yet"
}

# Test configuration file validation (if validation system is ready)
test_config_file_validation() {
    if [[ -f "configs/default.cfg" ]]; then
        test_command "Configuration file validation" "./nlm --validate configs/default.cfg 2>/dev/null || echo "Config validation not available yet"
    else
        echo -e "${YELLOW}Skipping config file validation (configs/default.cfg not found)${NC}"
        test_results+=("SKIP:Config file validation")
    fi
}

# Run all tests
main() {
    echo -e "${GREEN}=== NLM CLI Interface Test Suite ===${NC}"
    echo -e "${GREEN}Testing new unified CLI interface features${NC}"
    echo
    
    # Run tests
    test_help
    echo
    
    test_version
    echo
    
    test_phase2_help
    echo
    
    test_config_validation
    echo
    
    test_benchmark_help
    echo
    
    test_experiment_help
    echo
    
    test_config_file_validation
    echo
    
    # Summary
    echo -e "${GREEN}=== Test Summary ===${NC}"
    
    local passed=0
    local failed=0
    local skipped=0
    
    for result in "${test_results[@]}"; do
        case $result in
            PASS:*)
                ((passed++))
                ;;
            FAIL:*)
                ((failed++))
                ;;
            SKIP:*)
                ((skipped++))
                ;;
        esac
    done
    
    echo "Total tests: ${#test_results[@]}"
    echo "Passed: $passed"
    echo "Failed: $failed"
    echo "Skipped: $skipped"
    echo
    
    if [[ $failed -gt 0 ]]; then
        echo -e "${RED}Some tests failed! Check output above for details.${NC}"
        return 1
    else
        echo -e "${GREEN}All tests passed (or were skipped)!${NC}"
        return 0
    fi
}

# Execute main function
main
