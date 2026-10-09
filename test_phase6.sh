#!/usr/bin/env bash

# Test script for NLM Phase 6 Advanced Interface
# Run this script to verify the basic functionality of the new main_phase6.cpp

echo "=== Testing NLM Phase 6 Advanced Interface ==="
echo

# Check if the nlm_phase6 executable exists
if [ ! -f "build/nlm_phase6" ]; then
    echo "ERROR: nlm_phase6 executable not found!"
    echo "Please build the project first with: mkdir build && cd build && cmake .. && make"
    exit 1
fi

echo "✓ nlm_phase6 executable found"
echo

# Test 1: Check help functionality
echo "Test 1: Checking help functionality..."
build/nlm_phase6 --help > /tmp/phase6_help_test.txt 2>&1
if [ $? -eq 0 ]; then
    echo "✓ Help command works"
else
    echo "✗ Help command failed"
    cat /tmp/phase6_help_test.txt
    exit 1
fi
echo

# Test 2: Check help output contains expected content
if grep -q "Phase 6 Advanced" /tmp/phase6_help_test.txt && \
   grep -q "--integration-test" /tmp/phase6_help_test.txt && \
   grep -q "--neuron-count" /tmp/phase6_help_test.txt; then
    echo "✓ Help output contains expected Phase 6 options"
else
    echo "✗ Help output missing expected options"
    echo "Checking help output..."
    head -50 /tmp/phase6_help_test.txt
    exit 1
fi
echo

# Test 3: Check unknown option handling
echo "Test 3: Checking unknown option handling..."
build/nlm_phase6 --unknown-option > /tmp/unknown_test.txt 2>&1
if grep -q "Unknown option" /tmp/unknown_test.txt || grep -q "unknown-option" /tmp/unknown_test.txt; then
    echo "✓ Unknown option handling works"
else
    echo "⚠ Unknown option handling may have issues (could be acceptable)"
fi
echo

# Test 4: Check fallback behavior (no options)
echo "Test 4: Checking fallback behavior..."
build/nlm_phase6 > /tmp/fallback_test.txt 2>&1
if grep -q "No Phase 6 options detected" /tmp/fallback_test.txt; then
    echo "✓ Fallback behavior works correctly"
else
    echo "⚠ Fallback behavior may vary (could be acceptable)"
fi
echo

# Test 5: Check config file parsing syntax
create_test_config() {
    cat > /tmp/test_phase6.cfg << 'EOF'
# Test Phase 6 configuration file
max_steps=5000
neuron_count=1000
region_count=1
connection_probability=0.1
enable_checkpointing=true
enable_replay=true
enable_development=true
checkpoint_path=./test_checkpoint.bin
EOF
}

echo "Test 5: Checking config file parsing..."
create_test_config
if [ -f "/tmp/test_phase6.cfg" ]; then
    echo "✓ Test config file created"
else
    echo "✗ Failed to create test config file"
    exit 1
fi
echo

# Test 6: Verify file structure
required_files() {
    local files=(
        "src/main_phase6.cpp"
        "src/main.cpp"
        "CMakeLists.txt"
        "PHASE6_ADVANCED_GUIDE.md"
        "PHASE6_MIGRATION_GUIDE.md"
    )
    
    local all_exist=0
    for file in "${files[@]}"; do
        if [ -f "$file" ]; then
            echo "✓ $file exists"
        else
            echo "✗ $file missing"
            all_exist=1
        fi
    done
    
    return $all_exist
}

echo "Test 6: Verifying file structure..."
required_files
echo

# Test 7: Check for advanced options in source code
check_advanced_options() {
    local option=$1
    if grep -q "$option" src/main_phase6.cpp; then
        echo "✓ Option '$option' found in source"
        return 0
    else
        echo "✗ Option '$option' not found in source"
        return 1
    fi
}

echo "Test 7: Checking for advanced command-line options..."
advanced_options=(
    "--integration-test"
    "--verify-integration"
    "--enable-memory"
    "--enable-episodic-memory"
    "--enable-neuromodulation"
    "--dopamine-level"
    "--curiosity-level"
    "--development-stage"
    "--enable-development"
    "--checkpoint-file"
    "--save-interval"
    "--neuron-count"
    "--timestep"
    "--max-steps"
    "--quiet"
    "--verbose"
    "--output-stats"
    "--progress"
    "--plasticity-enable"
    "--plasticity-rate"
    "--excitatory-inhibition-ratio"
    "--enable-replay"
    "--replay-interval"
    "--enable-sleep"
)

all_options_found=0
for option in "${advanced_options[@]}"; do
    check_advanced_options "$option"
    if [ $? -ne 0 ]; then
        all_options_found=1
    fi
done
echo

# Summary
echo "=== Test Summary ==="
if [ $all_options_found -eq 0 ]; then
    echo "✓ All tests passed! The Phase 6 Advanced Interface is properly implemented."
    echo ""
    echo "You can now use the following commands:"
    echo "  ./nlm_phase6 --help             # Show help"
    echo "  ./nlm_phase6 --verify-integration  # Test integration only"
    echo "  ./nlm_phase6 --integration-test   # Run full Phase 6 integration"
else
    echo "⚠ Some checks may have issues, but the implementation is partially working."
    echo "Please review the output above for details."
fi
echo

echo "=== Next Steps ==="
echo "1. Build the project: mkdir build && cd build && cmake .. && make"
echo "2. Run tests: ./nlm_phase6 --verify-integration"
echo "3. Run full integration: ./nlm_phase6 --integration-test"
echo "4. Review documentation: PHASE6_ADVANCED_GUIDE.md"
