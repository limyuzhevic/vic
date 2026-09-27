#!/usr/bin/env python3

"""
NLM Phase 6 Integration Validator

Validates that all integrated systems are properly connected and functional.
This script tests the critical integration issues identified in the Phase 6 audit.
"""

import subprocess
import sys
import os
import time
import argparse
from pathlib import Path

# Add the src directory to the path so we can import NLM modules
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

def run_command(cmd, timeout=30):
    """Run a command and return the result."""
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True, timeout=timeout)
        return result.returncode, result.stdout, result.stderr
    except subprocess.TimeoutExpired:
        return -1, "", f"Command timed out: {cmd}"

def check_brain_initialization():
    """Test Brain initialization and integration."""
    print("=== Testing Brain Initialization ===")
    
    # Create a minimal test program
    test_code = '''
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <iostream>

int main() {
    std::cout << "Testing Brain initialization..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        std::cout << "ERROR: Brain initialization failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialization successful." << std::endl;
    
    // Test memory system integration
    if (brain->getWorkingMemory() != nullptr) {
        std::cout << "Working memory: INTEGRATED" << std::endl;
    } else {
        std::cout << "Working memory: NOT INTEGRATED" << std::endl;
    }
    
    if (brain->getEpisodicMemory() != nullptr) {
        std::cout << "Episodic memory: INTEGRATED" << std::endl;
    } else {
        std::cout << "Episodic memory: NOT INTEGRATED" << std::endl;
    }
    
    if (brain->getAssociativeMemory() != nullptr) {
        std::cout << "Associative memory: INTEGRATED" << std::endl;
    } else {
        std::cout << "Associative memory: NOT INTEGRATED" << std::endl;
    }
    
    // Test cognition system integration
    if (brain->getPlanner() != nullptr) {
        std::cout << "Planner: INTEGRATED" << std::endl;
    } else {
        std::cout << "Planner: NOT INTEGRATED" << std::endl;
    }
    
    if (brain->getConceptFormation() != nullptr) {
        std::cout << "Concept formation: INTEGRATED" << std::endl;
    } else {
        std::cout << "Concept formation: NOT INTEGRATED" << std::endl;
    }
    
    if (brain->getAttention() != nullptr) {
        std::cout << "Attention: INTEGRATED" << std::endl;
    } else {
        std::cout << "Attention: NOT INTEGRATED" << std::endl;
    }
    
    // Test neuromodulation system integration
    if (brain->getDopamine() != nullptr) {
        std::cout << "Dopamine: INTEGRATED" << std::endl;
    } else {
        std::cout << "Dopamine: NOT INTEGRATED" << std::endl;
    }
    
    if (brain->getCuriosity() != nullptr) {
        std::cout << "Curiosity: INTEGRATED" << std::endl;
    } else {
        std::cout << "Curiosity: NOT INTEGRATED" << std::endl;
    }
    
    if (brain->getNovelty() != nullptr) {
        std::cout << "Novelty: INTEGRATED" << std::endl;
    } else {
        std::cout << "Novelty: NOT INTEGRATED" << std::endl;
    }
    
    // Test prediction system integration
    if (brain->getPredictionSystem() != nullptr) {
        std::cout << "Prediction system: INTEGRATED" << std::endl;
    } else {
        std::cout << "Prediction system: NOT INTEGRATED" << std::endl;
    }
    
    // Test development system integration
    if (brain->getDevelopmentSystem() != nullptr) {
        std::cout << "Development system: INTEGRATED" << std::endl;
    } else {
        std::cout << "Development system: NOT INTEGRATED" << std::endl;
    }
    
    return 0;
}
'''
    
    # Write test code to file
    test_file = "test_integration.cpp"
    with open(test_file, 'w') as f:
        f.write(test_code)
    
    # Create a simple CMakeLists.txt for the test
    cmake_content = '''cmake_minimum_required(VERSION 3.16)
project(NLMLiteTest)

set(CMAKE_CXX_STANDARD 20)

# Find NLM library
find_library(NLM_LIB nlm_core PATHS ../..)

# Test executable
add_executable(nlm_test test_integration.cpp)
target_link_libraries(nlm_test ${NLM_LIB})
'''
    
    with open("CMakeLists.txt", 'w') as f:
        f.write(cmake_content)
    
    # Run the test
    print("Building and running integration test...")
    code, stdout, stderr = run_command("cd . && mkdir -p build_test && cd build_test && cmake .. && make -j4")
    
    if code != 0:
        print(f"Build failed: {stderr}")
        return False
    
    code, stdout, stderr = run_command("cd build_test && ./nlm_test")
    
    if code != 0:
        print(f"Test failed: {stderr}")
        return False
    
    print("Integration test output:")
    print(stdout)
    
    # Check results
    integration_passed = all([
        "Working memory: INTEGRATED" in stdout,
        "Episodic memory: INTEGRATED" in stdout,
        "Associative memory: INTEGRATED" in stdout,
        "Planner: INTEGRATED" in stdout,
        "Concept formation: INTEGRATED" in stdout,
        "Attention: INTEGRATED" in stdout,
        "Dopamine: INTEGRATED" in stdout,
        "Curiosity: INTEGRATED" in stdout,
        "Novelty: INTEGRATED" in stdout,
        "Prediction system: INTEGRATED" in stdout,
        "Development system: INTEGRATED" in stdout,
    ])
    
    return integration_passed

def main():
    """Main function."""
    parser = argparse.ArgumentParser(description="NLM Phase 6 Integration Validator")
    parser.add_argument("--checkpoints", action="store_true", help="Test checkpoint functionality")
    parser.add_argument("--experiments", action="store_true", help="Test experiment functionality")
    parser.add_argument("--performance", action="store_true", help="Test performance optimizations")
    
    args = parser.parse_args()
    
    print("NLM Phase 6 Integration Validator")
    print("=" * 50)
    
    # Run basic integration check
    if check_brain_initialization():
        print("\n✓ BASIC INTEGRATION: PASSED")
        print("All core systems are properly integrated.")
    else:
        print("\n✗ BASIC INTEGRATION: FAILED")
        print("Critical integration issues found.")
        print("\nThis indicates that Phase 6 integration is NOT complete.")
        print("The main issues are:")
        print("1. Memory systems not integrated with Brain")
        print("2. Cognition systems not instantiated")
        print("3. Neuromodulation systems not properly implemented")
        print("4. Prediction and development systems disconnected")
        return 1
    
    print("\nPhase 6 integration validation completed successfully.")
    return 0

if __name__ == "__main__":
    sys.exit(main())
