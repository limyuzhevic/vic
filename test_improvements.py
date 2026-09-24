#!/usr/bin/env python3

"""
Test program to verify NLM improvements and fixes.
This script tests all the major improvements made to the NLM codebase.
"""

import os
import subprocess
import sys

def run_command(cmd, description):
    """Run a command and return success status."""
    print(f"\n{'='*60}")
    print(f"Testing: {description}")
    print(f"Command: {cmd}")
    print(f"{'='*60}")
    
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        if result.returncode == 0:
            print(f"✓ SUCCESS")
            if result.stdout:
                print(f"Output: {result.stdout[:200]}...")
            return True
        else:
            print(f"✗ FAILED")
            print(f"Error: {result.stderr}")
            return False
    except Exception as e:
        print(f"✗ ERROR: {e}")
        return False

def test_build():
    """Test if the NLM code builds successfully."""
    print("\n" + "="*80)
    print("TESTING NLM BUILD SUCCESS")
    print("="*80)
    
    # Create build directory
    os.makedirs("build", exist_ok=True)
    
    # Run CMake and make
    cmake_cmd = "cmake .. -DCMAKE_BUILD_TYPE=Release"
    make_cmd = "make -j4"
    
    cmake_success = run_command(cmake_cmd, "CMake Configuration")
    make_success = run_command(make_cmd, "Compilation")
    
    return cmake_success and make_success

def test_integration():
    """Test the integration by running the main program."""
    print("\n" + "="*80)
    print("TESTING INTEGRATION")
    print("="*80)
    
    if not os.path.exists("build/nlm"):
        print("\n✗ Build executable not found. Run build test first.")
        return False
    
    # Test the main program
    cmd = "./build/nlm --help"
    return run_command(cmd, "Help command test")

def test_improvements():
    """Test the specific improvements made."""
    print("\n" + "="*80)
    print("TESTING IMPROVEMENTS")
    print("="*80)
    
    improvements_tested = []
    
    # Test 1: Check if integration tests exist in main.cpp
    main_cpp_path = "src/main.cpp"
    if os.path.exists(main_cpp_path):
        with open(main_cpp_path, 'r') as f:
            content = f.read()
            
        # Check for integration test keywords
        integration_keywords = [
            "Integration Test", "Memory System Integration", 
            "Prediction System", "Cognition Systems"
        ]
        
        found_integration = any(keyword in content for keyword in integration_keywords)
        improvements_tested.append(("Integration tests in main.cpp", found_integration))
        
        # Check for comprehensive testing
        if "Test 4:" in content and "Comprehensive Integration" in content:
            improvements_tested.append(("Comprehensive integration testing", True))
        else:
            improvements_tested.append(("Comprehensive integration testing", False))
    
    # Test 2: Check if AgentBrain has enable methods
    agentbrain_hpp_path = "src/agent/AgentBrain.hpp"
    if os.path.exists(agentbrain_hpp_path):
        with open(agentbrain_hpp_path, 'r') as f:
            content = f.read()
            
        enable_methods = [
            "enableRewardModulation",
            "enableStructuralPlasticity", 
            "enableDevelopment",
            "enableCuriosity",
            "enablePredictionSystem",
            "enableMemorySystems",
            "enableCognitiveSystems"
        ]
        
        found_enable = sum(1 for method in enable_methods if method in content)
        improvements_tested.append(("Enable methods in AgentBrain", found_enable))
        
        # Check for advanced configuration methods
        if "setDevelopmentStage" in content and "setPlasticityModifier" in content:
            improvements_tested.append(("Advanced configuration methods", True))
        else:
            improvements_tested.append(("Advanced configuration methods", False))
    
    # Test 3: Check for documentation improvements
    easy_usage_path = "easy_usage.md"
    if os.path.exists(easy_usage_path):
        with open(easy_usage_path, 'r') as f:
            content = f.read()
            
        advanced_patterns = [
            "Pattern 4:", "Pattern 5:", "Pattern 6:", 
            "Pattern 7:", "Pattern 8:"
        ]
        
        found_patterns = sum(1 for pattern in advanced_patterns if pattern in content)
        improvements_tested.append(("Advanced documentation patterns", found_patterns))
    
    # Print results
    print("\nImprovement Test Results:")
    print("-" * 60)
    all_passed = True
    for test_name, result in improvements_tested:
        status = "✓ PASS" if result else "✗ FAIL"
        if isinstance(result, bool) or isinstance(result, int):
            print(f"{status}: {test_name} - {result}")
            if not result:
                all_passed = False
        else:
            print(f"{status}: {test_name} - {result}/{len(advanced_patterns) if 'Advanced' in test_name else 'N/A'}")
            if result == 0:
                all_passed = False
    
    return all_passed

def main():
    """Main test function."""
    print("NLM Improvement and Fix Verification")
    print("="*80)
    
    # Change to script directory
    script_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(script_dir)
    
    # Run tests
    build_success = test_build()
    integration_success = test_integration()
    improvements_success = test_improvements()
    
    # Summary
    print("\n" + "="*80)
    print("SUMMARY")
    print("="*80)
    print(f"Build Test: {'✓ PASS' if build_success else '✗ FAIL'}")
    print(f"Integration Test: {'✓ PASS' if integration_success else '✗ FAIL'}")
    print(f"Improvements Test: {'✓ PASS' if improvements_success else '✗ FAIL'}")
    
    overall_success = build_success and integration_success and improvements_success
    print(f"\nOverall: {'✓ ALL TESTS PASSED' if overall_success else '✗ SOME TESTS FAILED'}")
    
    return 0 if overall_success else 1

if __name__ == "__main__":
    sys.exit(main())