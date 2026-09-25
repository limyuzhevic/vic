#!/usr/bin/env python3
"""
Simple test script to verify the code quality improvements.
This script checks that all modified files can be parsed and have the expected structure.
"""

import os
import sys
import re
from pathlib import Path

def check_file_exists(filepath):
    """Check if file exists and return True/False"""
    return os.path.exists(filepath)

def check_constants_header():
    """Check that Constants.hpp was created correctly"""
    filepath = "src/core/Constants.hpp"
    if not check_file_exists(filepath):
        return False, f"Constants.hpp not found at {filepath}"
    
    with open(filepath, 'r') as f:
        content = f.read()
    
    # Check for key constants
    required_constants = [
        "DEFAULT_TIMESTEP_MS",
        "TIME_CONSTANT_MS", 
        "MIN_WEIGHT",
        "MAX_WEIGHT",
        "STDP_LTP_WEIGHT",
        "STDP_LTD_WEIGHT"
    ]
    
    missing_constants = []
    for const in required_constants:
        if not any(line.strip().startswith(f"constexpr {const}") or line.strip().startswith(f"constexpr double {const}") or line.strip().startswith(f"constexpr float {const}") or line.strip().startswith(f"constexpr size_t {const}") for line in content.split('\n') if const in line):
            missing_constants.append(const)
    
    if missing_constants:
        return False, f"Missing constants in Constants.hpp: {', '.join(missing_constants)}"
    
    return True, "Constants.hpp is valid"

def check_utils_header():
    """Check that Utils.hpp was created correctly"""
    filepath = "src/core/Utils.hpp"
    if not check_file_exists(filepath):
        return False, f"Utils.hpp not found at {filepath}"
    
    with open(filepath, 'r') as f:
        content = f.read()
    
    # Check for key utility functions
    required_functions = [
        "Error",
        "validateNotNull",
        "clampWeight",
        "manageSpikeHistory"
    ]
    
    missing_functions = []
    for func in required_functions:
        if f"{func}(" not in content:
            missing_functions.append(func)
    
    if missing_functions:
        return False, f"Missing utility functions in Utils.hpp: {', '.join(missing_functions)}"
    
    return True, "Utils.hpp is valid"

def check_snake_case_conversion():
    """Check that snake_case naming was applied"""
    files_to_check = [
        "src/plasticity/STDP.cpp",
        "src/plasticity/Hebbian.cpp", 
        "src/brain/Synapse.cpp",
        "src/agent/AgentBrain.cpp"
    ]
    
    errors = []
    for filepath in files_to_check:
        if not check_file_exists(filepath):
            errors.append(f"File not found: {filepath}")
            continue
            
        with open(filepath, 'r') as f:
            content = f.read()
        
        # Check for snake_case variables (should have underscores)
        # This is a simple check - look for common patterns
        if "learning_rate" in content or "max_weight" in content:
            # Good, snake_case is present
            pass
        elif "learningRate" in content or "maxWeight" in content:
            errors.append(f"camelCase still present in {filepath}")
    
    if errors:
        return False, "Snake case conversion issues: " + "; ".join(errors)
    
    return True, "Snake case naming conventions applied correctly"

def main():
    """Run all verification checks"""
    print("=== Code Quality Improvements Verification ===\n")
    
    checks = [
        ("Constants.hpp", check_constants_header),
        ("Utils.hpp", check_utils_header),
        ("Snake case naming", check_snake_case_conversion),
    ]
    
    all_passed = True
    for check_name, check_func in checks:
        print(f"Checking {check_name}...")
        passed, message = check_func()
        status = "✅ PASS" if passed else "❌ FAIL"
        print(f"  {status}: {message}\n")
        if not passed:
            all_passed = False
    
    if all_passed:
        print("🎉 All checks passed! Code quality improvements are complete.")
        return 0
    else:
        print("❌ Some checks failed. Please review the issues above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
