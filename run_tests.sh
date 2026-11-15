#!/usr/bin/env bash
# NLM Performance Test Script
# This script runs comprehensive tests on the improved NLM system

# Test configuration
TEST_DIR="test_results"
LOG_FILE="$TEST_DIR/test_run.log"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)

# Create test directory
mkdir -p $TEST_DIR

# Test function
run_test() {
    local test_name=$1
    local command=$2
    local expected_reward=$3
    local expected_time=$4
    
    echo "=== Running $test_name ===" >> $LOG_FILE
    echo "Command: $command" >> $LOG_FILE
    echo "Expected reward: $expected_reward" >> $LOG_FILE
    echo "Expected time: $expected_time seconds" >> $LOG_FILE
    echo "Time: $(date)" >> $LOG_FILE
    echo "" >> $LOG_FILE
    
    # Run the test with timeout
    timeout 300 bash -c "$command" 2>> $LOG_FILE || {
        echo "✗ $test_name TIMEOUT" >> $LOG_FILE
        return 1
    }
    
    echo "" >> $LOG_FILE
}

# Main test function
main() {
    echo "NLM Test Suite - $(date)" >> $LOG_FILE
    echo "============================================" >> $LOG_FILE
    echo "" >> $LOG_FILE
    
    # Test 1: Phase 3 Basic Learning
    echo "Running Phase 3 test..."
    run_test "Phase 3 Basic Learning" "./nlm_phase3_demo 2 100" "0.01" "60"
    echo "Phase 3 result: $?" >> $LOG_FILE
    
    # Test 2: Phase 5 Checkpoint Round-trip
    echo "Running Phase 5 test..."
    run_test "Phase 5 Checkpoint Round-trip" "./nlm_phase5_demo" "0.0" "30"
    echo "Phase 5 result: $?" >> $LOG_FILE
    
    # Test 3: Phase 6 Integration
    echo "Running Phase 6 test..."
    run_test "Phase 6 Integration" "./nlm_phase6_demo" "0.05" "120"
    echo "Phase 6 result: $?" >> $LOG_FILE
    
    # Test 4: Phase 2 Basic Neural Computation
    echo "Running Phase 2 test..."
    run_test "Phase 2 Basic Neural Computation" "./nlm" "0.0" "60"
    echo "Phase 2 result: $?" >> $LOG_FILE
    
    # Generate summary
    echo "============================================" >> $LOG_FILE
    echo "Test Summary:" >> $LOG_FILE
    echo "All tests completed." >> $LOG_FILE
    echo "Results saved to: $LOG_FILE" >> $LOG_FILE
    
    echo "Test suite completed. Results saved to $LOG_FILE"
}

# Run main function
main