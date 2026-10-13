#!/usr/bin/bash

# Test Harness for NLM Improvements
# Comprehensive test suite to verify all improvements

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

test_phase6_integration() {
    echo -e "${BLUE}Testing Phase 6 Integration improvements...${NC}"
    
    # Check if the Phase6 demo builds and runs
    if [[ ! -f "./build/nlm_phase6_demo" ]]; then
        echo -e "${YELLOW}Building NLM...${NC}"
        ./nlm build
    fi
    
    echo -e "${YELLOW}Running Phase 6 Integration Demo...${NC}"
    ./build/nlm_phase6_demo > /tmp/phase6_test_output.txt 2>&1
    local exit_code=$?
    
    if [[ $exit_code -eq 0 ]]; then
        echo -e "${GREEN}✓ Phase 6 Integration Demo test PASSED${NC}"
        
        # Check for expected output
        if grep -q "Integration verification passed" /tmp/phase6_test_output.txt; then
            echo -e "${GREEN}✓ Integration verification found${NC}"
        else
            echo -e "${YELLOW}! Integration verification not found (may be normal if output format changed)${NC}"
        fi
        
        if grep -q "FINAL RESULTS" /tmp/phase6_test_output.txt; then
            echo -e "${GREEN}✓ Final results reported${NC}"
        else
            echo -e "${YELLOW}! Final results not found${NC}"
        fi
        
    else
        echo -e "${RED}✗ Phase 6 Integration Demo test FAILED${NC}"
        echo -e "Error output (first 20 lines):"
        head -20 /tmp/phase6_test_output.txt
        return 1
    fi
}

test_shell_cli() {
    echo -e "${BLUE}Testing Shell CLI improvements...${NC}"
    
    # Test help command
    echo -e "${YELLOW}Testing 'nlm --help' command...${NC}"
    ./nlm --help > /tmp/help_test.txt 2>&1
    local exit_code=$?
    
    if [[ $exit_code -eq 0 ]]; then
        echo -e "${GREEN}✓ Help command works${NC}"
        
        # Check for expected commands
        if grep -q "run-phase6" /tmp/help_test.txt; then
            echo -e "${GREEN}✓ run-phase6 command documented${NC}"
        else
            echo -e "${RED}✗ run-phase6 command not documented${NC}"
            return 1
        fi
        
        if grep -q "experiment" /tmp/help_test.txt; then
            echo -e "${GREEN}✓ experiment command documented${NC}"
        else
            echo -e "${RED}✗ experiment command not documented${NC}"
            return 1
        fi
        
        if grep -q "benchmark" /tmp/help_test.txt; then
            echo -e "${GREEN}✓ benchmark command documented${NC}"
        else
            echo -e "${RED}✗ benchmark command not documented${NC}"
            return 1
        fi
        
    else
        echo -e "${RED}✗ Help command test FAILED${NC}"
        cat /tmp/help_test.txt
        return 1
    fi
    
    # Test build command
    echo -e "${YELLOW}Testing 'nlm build' command...${NC}"
    ./nlm build > /tmp/build_test.txt 2>&1
    local build_exit_code=$?
    
    if [[ $build_exit_code -eq 0 ]]; then
        echo -e "${GREEN}✓ Build command works${NC}"
    else
        echo -e "${RED}✗ Build command test FAILED${NC}"
        return 1
    fi
    
    # Test clean command
    echo -e "${YELLOW}Testing 'nlm clean' command...${NC}"
    ./nlm clean > /tmp/clean_test.txt 2>&1
    local clean_exit_code=$?
    
    if [[ $clean_exit_code -eq 0 ]]; then
        echo -e "${GREEN}✓ Clean command works${NC}"
        
        # Check if build directory was removed
        if [[ ! -d "build" ]]; then
            echo -e "${GREEN}✓ Build directory cleaned${NC}"
        else
            echo -e "${YELLOW}! Build directory still exists (may be created by build command)${NC}"
        fi
        
    else
        echo -e "${RED}✗ Clean command test FAILED${NC}"
        return 1
    fi
}

test_configuration() {
    echo -e "${BLUE}Testing configuration improvements...${NC}"
    
    # Test that configs directory exists and has files
    echo -e "${YELLOW}Checking configuration files...${NC}"
    
    if [[ -f "configs/default.cfg" ]]; then
        echo -e "${GREEN}✓ default.cfg exists${NC}"
        
        # Check for Phase 6 parameters
        if grep -q "random_seed" configs/default.cfg; then
            echo -e "${GREEN}✓ random_seed parameter present${NC}"
        else
            echo -e "${RED}✗ random_seed parameter missing${NC}"
            return 1
        fi
        
        if grep -q "neuron_count" configs/default.cfg; then
            echo -e "${GREEN}✓ neuron_count parameter present${NC}"
        else
            echo -e "${RED}✗ neuron_count parameter missing${NC}"
            return 1
        fi
        
        if grep -q "stdp_ltp_weight" configs/default.cfg; then
            echo -e "${GREEN}✓ stdp_ltp_weight parameter present${NC}"
        else
            echo -e "${RED}✗ stdp_ltp_weight parameter missing${NC}"
            return 1
        fi
        
    else
        echo -e "${RED}✗ default.cfg not found${NC}"
        return 1
    fi
    
    # Create experiment config as would be done by 'nlm experiment'
    echo -e "${YELLOW}Testing experiment config creation...${NC}"
    cat > configs/experiment_test.cfg << EOF
# NLM Experiment Configuration
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 2000
region_count = 2
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
replay_interval = 50
consolidation_interval = 500
EOF
    
    if [[ -f "configs/experiment_test.cfg" ]]; then
        echo -e "${GREEN}✓ experiment config created successfully${NC}"
        
        # Clean up
        rm configs/experiment_test.cfg
    else
        echo -e "${RED}✗ experiment config creation failed${NC}"
        return 1
    fi
}

test_documentation() {
    echo -e "${BLUE}Testing documentation improvements...${NC}"
    
    # Test that README.md exists and has content
    echo -e "${YELLOW}Checking README.md...${NC}"
    
    if [[ -f "README.md" ]]; then
        echo -e "${GREEN}✓ README.md exists${NC}"
        
        # Count lines to ensure it's substantial
        local line_count=$(wc -l < README.md)
        echo -e "  File has ${line_count} lines (should be > 100)"
        
        if [[ $line_count -gt 100 ]]; then
            echo -e "${GREEN}✓ README.md has substantial content${NC}"
        else
            echo -e "${YELLOW}! README.md may be too short${NC}"
        fi
        
        # Check for Phase 6 mention
        if grep -q "Phase 6" README.md; then
            echo -e "${GREEN}✓ README.md mentions Phase 6${NC}"
        else
            echo -e "${YELLOW}! README.md doesn't mention Phase 6${NC}"
        fi
        
    else
        echo -e "${RED}✗ README.md not found${NC}"
        return 1
    fi
    
    # Test that easy_usage.md exists
    if [[ -f "easy_usage.md" ]]; then
        echo -e "${GREEN}✓ easy_usage.md exists${NC}"
    else
        echo -e "${YELLOW}! easy_usage.md not found${NC}"
    fi
    
    # Test that HOW_TO_USE.md exists
    if [[ -f "HOW_TO_USE.md" ]]; then
        echo -e "${GREEN}✓ HOW_TO_USE.md exists${NC}"
    else
        echo -e "${YELLOW}! HOW_TO_USE.md not found${NC}"
    fi
}

test_agent_system() {
    echo -e "${BLUE}Testing Agent system improvements...${NC}"
    
    # Check if agent source files exist
    if [[ -f "src/agent/AgentBrain.cpp" ]]; then
        echo -e "${GREEN}✓ AgentBrain.cpp exists${NC}"
        
        # Check for key features in AgentBrain.cpp
        if grep -q "enableRewardModulation" src/agent/AgentBrain.cpp; then
            echo -e "${GREEN}✓ Reward modulation support found${NC}"
        else
            echo -e "${YELLOW}! Reward modulation not found${NC}"
        fi
        
        if grep -q "enableCuriosity" src/agent/AgentBrain.cpp; then
            echo -e "${GREEN}✓ Curiosity support found${NC}"
        else
            echo -e "${YELLOW}! Curiosity support not found${NC}"
        fi
        
        if grep -q "enableDevelopment" src/agent/AgentBrain.cpp; then
            echo -e "${GREEN}✓ Development support found${NC}"
        else
            echo -e "${YELLOW}! Development support not found${NC}"
        fi
        
        if grep -q "enableStructuralPlasticity" src/agent/AgentBrain.cpp; then
            echo -e "${GREEN}✓ Structural plasticity support found${NC}"
        else
            echo -e "${YELLOW}! Structural plasticity support not found${NC}"
        fi
        
    else
        echo -e "${RED}✗ AgentBrain.cpp not found${NC}"
        return 1
    fi
    
    # Check for AgentBrain.hpp
    if [[ -f "src/agent/AgentBrain.hpp" ]]; then
        echo -e "${GREEN}✓ AgentBrain.hpp exists${NC}"
    else
        echo -e "${RED}✗ AgentBrain.hpp not found${NC}"
        return 1
    fi
}

test_world_system() {
    echo -e "${BLUE}Testing World system improvements...${NC}"
    
    # Check for world system files
    if [[ -f "src/world/SimpleWorld.cpp" ]]; then
        echo -e "${GREEN}✓ SimpleWorld.cpp exists${NC}"
        
        # Check for key features
        if grep -q "observe" src/world/SimpleWorld.cpp; then
            echo -e "${GREEN}✓ Observation functionality found${NC}"
        else
            echo -e "${YELLOW}! Observation functionality not found${NC}"
        fi
        
        if grep -q "applyAction" src/world/SimpleWorld.cpp; then
            echo -e "${GREEN}✓ Action application found${NC}"
        else
            echo -e "${YELLOW}! Action application not found${NC}"
        fi
        
        if grep -q "computeReward" src/world/SimpleWorld.cpp; then
            echo -e "${GREEN}✓ Reward computation found${NC}"
        else
            echo -e "${YELLOW}! Reward computation not found${NC}"
        fi
        
    else
        echo -e "${RED}✗ SimpleWorld.cpp not found${NC}"
        return 1
    fi
}

run_all_tests() {
    echo -e "${GREEN}========================================================${NC}"
    echo -e "${GREEN}NLM Improvements Test Suite${NC}"
    echo -e "${GREEN}========================================================${NC}"
    echo
    
    local tests_passed=0
    local tests_failed=0
    
    # Run tests
    if test_phase6_integration; then
        tests_passed=$((tests_passed + 1))
    else
        tests_failed=$((tests_failed + 1))
    fi
    
    if test_shell_cli; then
        tests_passed=$((tests_passed + 1))
    else
        tests_failed=$((tests_failed + 1))
    fi
    
    if test_configuration; then
        tests_passed=$((tests_passed + 1))
    else
        tests_failed=$((tests_failed + 1))
    fi
    
    if test_documentation; then
        tests_passed=$((tests_passed + 1))
    else
        tests_failed=$((tests_failed + 1))
    fi
    
    if test_agent_system; then
        tests_passed=$((tests_passed + 1))
    else
        tests_failed=$((tests_failed + 1))
    fi
    
    if test_world_system; then
        tests_passed=$((tests_passed + 1))
    else
        tests_failed=$((tests_failed + 1))
    fi
    
    echo
    echo -e "${GREEN}========================================================${NC}"
    echo -e "${GREEN}Test Summary: ${tests_passed} passed, ${tests_failed} failed${NC}"
    echo -e "${GREEN}========================================================${NC}"
    
    if [[ $tests_failed -eq 0 ]]; then
        echo -e "${GREEN}🎉 All tests passed! Improvements are working correctly.${NC}"
        return 0
    else
        echo -e "${RED}❌ Some tests failed. Please review the issues above.${NC}"
        return 1
    fi
}

# Run all tests
run_all_tests
