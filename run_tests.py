#!/usr/bin/env python3
# Simple integration test for NLM optimizations

import subprocess
import sys
import os

def run_test(test_name, command):
    print(f"\n{'='*60}")
    print(f"Running {test_name}")
    print(f"{'='*60}")
    
    try:
        result = subprocess.run(command, shell=True, capture_output=True, text=True, timeout=60)
        if result.returncode == 0:
            print(f"✓ {test_name} PASSED")
            if result.stdout:
                # Show last few lines of output
                lines = result.stdout.strip().split('\n')
                for line in lines[-5:]:
                    if line.strip():
                        print(f"  {line}")
            return True
        else:
            print(f"✗ {test_name} FAILED")
            print(f"Error: {result.stderr}")
            return False
    except subprocess.TimeoutExpired:
        print(f"✗ {test_name} TIMEOUT")
        return False
    except Exception as e:
        print(f"✗ {test_name} ERROR: {e}")
        return False

def main():
    print("NLM Optimization Integration Test")
    print("="*60)
    
    # Change to project directory
    project_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(project_dir)
    
    tests = [
        ("Brain Tests", "nlm_test"),
        ("STDP Tests", "nlm_test"),  # Same test binary includes STDP tests
        ("Brain Step Tests", "nlm_test"),
        ("Optimization Verification", "nlm_optimization_tests"),
        ("Benchmark", "nlm_benchmark"),
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_cmd in tests:
        if run_test(test_name, test_cmd):
            passed += 1
        else:
            print(f"  Skipping subsequent tests due to failure")
            break
    
    print(f"\n{'='*60}")
    print(f"Test Results: {passed}/{total} passed")
    
    if passed == total:
        print("✓ All tests passed! Optimizations are working correctly.")
        return 0
    else:
        print("✗ Some tests failed. Please review the implementation.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
