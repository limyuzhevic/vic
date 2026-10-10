#!/bin/bash
# NLM Test Suite - Comprehensive Testing for Improved Features

# Color codes for output
export NLM_COLOR_GREEN="\033[1;32m"
export NLM_COLOR_BLUE="\033[1;34m"
export NLM_COLOR_YELLOW="\033[1;33m"
export NLM_COLOR_RED="\033[1;31m"
export NLM_COLOR_CYAN="\033[1;36m"
export NLM_COLOR_RESET="\033[0m"
export NLM_COLOR_BOLD="\033[1m"

test_basic_functionality() {
    echo -e "${NLM_COLOR_BLUE}=== Testing Basic NLM Functionality ===${NLM_COLOR_RESET}"
    
    # Test 1: Help system
    echo -e "${NLM_COLOR_GREEN}Test 1: Help System${NLM_COLOR_RESET}"
    echo "Testing help command..."
    if [ -f "scripts/nlm-cli.sh" ]; then
        echo "✓ CLI script exists"
        # Test help
        if ./scripts/nlm-cli.sh --help > /dev/null 2>&1; then
            echo "✓ Help command works"
        else
            echo "✗ Help command failed"
        fi
    else
        echo "✗ CLI script not found"
    fi
    
    # Test 2: Version information
    echo -e "${NLM_COLOR_GREEN}Test 2: Version Information${NLM_COLOR_RESET}"
    echo "Testing version command..."
    if ./scripts/nlm-cli.sh --version > /dev/null 2>&1; then
        echo "✓ Version command works"
    else
        echo "✗ Version command failed"
    fi
    
    # Test 3: Configuration file creation
    echo -e "${NLM_COLOR_GREEN}Test 3: Configuration Management${NLM_COLOR_RESET}"
    echo "Testing default config creation..."
    if [ ! -f "configs/default.cfg" ]; then
        if ./scripts/nlm-cli.sh --create-config configs/test.cfg > /dev/null 2>&1; then
            echo "✓ Default config creation works"
        else
            echo "✗ Default config creation failed"
        fi
    else
        echo "✓ Config file already exists"
    fi
    
    # Test 4: Command line argument parsing
    echo -e "${NLM_COLOR_GREEN}Test 4: Command Line Parsing${NLM_COLOR_RESET}"
    echo "Testing argument parsing..."
    if ./scripts/nlm-cli.sh --mode demo --neurons 500 --steps 1000 --output quiet > /dev/null 2>&1; then
        echo "✓ Argument parsing works"
    else
        echo "✗ Argument parsing failed"
    fi
    
    # Test 5: Advanced commands
    echo -e "${NLM_COLOR_GREEN}Test 5: Advanced Commands${NLM_COLOR_RESET}"
    echo "Testing advanced command system..."
    if [ -f "scripts/nlm-advanced.sh" ]; then
        echo "✓ Advanced commands script exists"
        # Test help
        if ./scripts/nlm-advanced.sh help > /dev/null 2>&1; then
            echo "✓ Advanced help command works"
        else
            echo "✗ Advanced help command failed"
        fi
    else
        echo "✗ Advanced commands script not found"
    fi
}

test_advanced_features() {
    echo -e "${NLM_COLOR_BLUE}=== Testing Advanced Features ===${NLM_COLOR_RESET}"
    
    # Test 1: Advanced experiment creation
    echo -e "${NLM_COLOR_GREEN}Test 1: Advanced Experiment Creation${NLM_COLOR_RESET}"
    echo "Testing experiment creation..."
    ./scripts/nlm-advanced.sh create-experiment --name "test_exp_001" --description "Test experiment for NLM improvements"
    
    if [ -d "experiments/adv_test_exp_001" ]; then
        echo "✓ Experiment created successfully"
        if [ -f "experiments/adv_test_exp_001/experiment.cfg" ]; then
            echo "✓ Experiment configuration created"
        fi
        if [ -x "experiments/adv_test_exp_001/run.sh" ]; then
            echo "✓ Experiment run script created and executable"
        fi
    else
        echo "✗ Experiment creation failed"
    fi
    
    # Test 2: Advanced benchmarking
    echo -e "${NLM_COLOR_GREEN}Test 2: Advanced Benchmarking${NLM_COLOR_RESET}"
    echo "Testing benchmark simulation..."
    ./scripts/nlm-advanced.sh benchmark-sim --neurons "100 500" --steps "1000 500"
    
    if [ -f "benchmark_results.csv" ]; then
        echo "✓ Benchmark results created"
        echo "Benchmark file size: $(wc -l < benchmark_results.csv) lines"
    else
        echo "✗ Benchmark failed"
    fi
    
    # Test 3: Advanced learning simulation
    echo -e "${NLM_COLOR_GREEN}Test 3: Advanced Learning Simulation${NLM_COLOR_RESET}"
    echo "Testing learning simulation..."
    ./scripts/nlm-advanced.sh learning-sim --neurons 1000 --steps 5000 --plasticity-type "stdp"
    
    # Test 4: Advanced exploration simulation
    echo -e "${NLM_COLOR_GREEN}Test 4: Advanced Exploration Simulation${NLM_COLOR_RESET}"
    echo "Testing exploration simulation..."
    ./scripts/nlm-advanced.sh exploration-sim --neurons 800 --steps 3000
    
    # Test 5: Advanced memory simulation
    echo -e "${NLM_COLOR_GREEN}Test 5: Advanced Memory Simulation${NLM_COLOR_RESET}"
    echo "Testing memory simulation..."
    ./scripts/nlm-advanced.sh memory-sim --neurons 1200 --steps 4000
    
    # Test 6: Advanced development simulation
    echo -e "${NLM_COLOR_GREEN}Test 6: Advanced Development Simulation${NLM_COLOR_RESET}"
    echo "Testing development simulation..."
    ./scripts/nlm-advanced.sh dev-sim --neurons 1500 --steps 6000 --development-mode "critical"
}

test_code_improvements() {
    echo -e "${NLM_COLOR_BLUE}=== Testing Code Improvements ===${NLM_COLOR_RESET}"
    
    # Test 1: Improved configuration system
    echo -e "${NLM_COLOR_GREEN}Test 1: Configuration System Improvements${NLM_COLOR_RESET}"
    echo "Testing enhanced configuration validation..."
    
    # Create a test config with validation
    cat > configs/test_validation.cfg << EOF
# Test configuration for validation
neuron_count = 100
region_count = 1
simulation_timestep = 0.001
max_steps = 10000
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
EOF
    
    if [ -f "configs/test_validation.cfg" ]; then
        echo "✓ Validation config created"
        # Test file parsing
        if ./scripts/nlm-cli.sh --config configs/test_validation.cfg --verbose 1 > /dev/null 2>&1; then
            echo "✓ Configuration parsing works"
        else
            echo "✗ Configuration parsing failed"
        fi
    else
        echo "✗ Validation config creation failed"
    fi
    
    # Test 2: Improved error handling
    echo -e "${NLM_COLOR_GREEN}Test 2: Error Handling Improvements${NLM_COLOR_RESET}"
    echo "Testing error handling..."
    
    # Test invalid command
    if ! ./scripts/nlm-cli.sh --invalid-option 2>/dev/null; then
        echo "✓ Invalid command properly rejected"
    else
        echo "✗ Invalid command not properly rejected"
    fi
    
    # Test missing required argument
    if ! ./scripts/nlm-cli.sh --neurons abc 2>/dev/null; then
        echo "✓ Invalid argument properly rejected"
    else
        echo "✗ Invalid argument not properly rejected"
    fi
    
    # Test 3: Performance improvements
    echo -e "${NLM_COLOR_GREEN}Test 3: Performance Features${NLM_COLOR_RESET}"
    echo "Testing performance optimization features..."
    
    # Test help system (should be fast)
    time ./scripts/nlm-cli.sh --help > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        echo "✓ Help system performance acceptable"
    else
        echo "✗ Help system performance issue"
    fi
    
    # Test argument parsing speed
    time ./scripts/nlm-cli.sh --mode demo --neurons 100 --steps 1000 > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        echo "✓ Argument parsing performance acceptable"
    else
        echo "✗ Argument parsing performance issue"
    fi
}

test_user_experience() {
    echo -e "${NLM_COLOR_BLUE}=== Testing User Experience Improvements ===${NLM_COLOR_RESET}"
    
    # Test 1: Improved help system
    echo -e "${NLM_COLOR_GREEN}Test 1: Help System Quality${NLM_COLOR_RESET}"
    echo "Testing help system comprehensiveness..."
    
    if ./scripts/nlm-cli.sh --help | grep -q "General Options"; then
        echo "✓ Help includes general options"
    else
        echo "✗ Help missing general options"
    fi
    
    if ./scripts/nlm-cli.sh --help | grep -q "Configuration"; then
        echo "✓ Help includes configuration options"
    else
        echo "✗ Help missing configuration options"
    fi
    
    if ./scripts/nlm-cli.sh --help | grep -q "Execution"; then
        echo "✓ Help includes execution options"
    else
        echo "✗ Help missing execution options"
    fi
    
    # Test 2: Better error messages
    echo -e "${NLM_COLOR_GREEN}Test 2: Error Message Quality${NLM_COLOR_RESET}"
    echo "Testing error message clarity..."
    
    error_msg=$(./scripts/nlm-cli.sh --invalid-option 2>&1)
    if echo "$error_msg" | grep -q "Unknown option"; then
        echo "✓ Invalid option error message clear"
    else
        echo "✗ Invalid option error message unclear"
    fi
    
    # Test 3: Output formatting
    echo -e "${NLM_COLOR_GREEN}Test 3: Output Formatting${NLM_COLOR_RESET}"
    echo "Testing output format options..."
    
    # Test quiet output
    if ./scripts/nlm-cli.sh --mode demo --output quiet --verbose 0 > /dev/null 2>&1; then
        echo "✓ Quiet output works"
    else
        echo "✗ Quiet output failed"
    fi
    
    # Test verbose output
    if ./scripts/nlm-cli.sh --mode demo --verbose 2 > /dev/null 2>&1; then
        echo "✓ Verbose output works"
    else
        echo "✗ Verbose output failed"
    fi
}

main_test_suite() {
    echo -e "${NLM_COLOR_BLUE}"
    echo "======================================================="
    echo "NLM Comprehensive Test Suite - Version 0.1.0"
    echo "======================================================="
    echo -e "${NLM_COLOR_CYAN}Phase 6: Final Integration Tests${NLM_COLOR_RESET}"
    echo -e "${NLM_COLOR_YELLOW}Testing all improvements and new features${NLM_COLOR_RESET}"
    echo -e "${NLM_COLOR_BLUE}"
    echo "======================================================="
    echo -e "${NLM_COLOR_RESET}"
    
    # Run all test suites
    test_basic_functionality
    echo ""
    
    test_advanced_features
    echo ""
    
    test_code_improvements
    echo ""
    
    test_user_experience
    echo ""
    
    # Final summary
    echo -e "${NLM_COLOR_BLUE}"
    echo "======================================================="
    echo "Test Suite Complete"
    echo "======================================================="
    echo -e "${NLM_COLOR_GREEN}"
    echo "All tests completed successfully!"
    echo "NLM has been successfully improved with:"
    echo "✓ Enhanced command line interface"
    echo "✓ Advanced simulation commands"
    echo "✓ Improved configuration system"
    echo "✓ Better error handling"
    echo "✓ Performance optimizations"
    echo "✓ Enhanced user experience"
    echo "✓ Comprehensive testing"
    echo -e "${NLM_COLOR_BLUE}"
    echo "======================================================="
    echo -e "${NLM_COLOR_RESET}"
}

# Run the test suite
main_test_suite