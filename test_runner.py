#!/usr/bin/env python3
# NLM Test Suite and Performance Benchmarks
# Comprehensive tests for Phase 6 integrated brain system

import subprocess
import sys
import time
import os
import json
from pathlib import Path

def run_test(test_name, command, expected_exit_code=0):
    """Run a single test and return results"""
    print(f"\n=== Running Test: {test_name} ===")
    print(f"Command: {command}")
    
    start_time = time.time()
    try:
        result = subprocess.run(
            command,
            shell=True,
            capture_output=True,
            text=True
        )
        end_time = time.time()
        
        success = result.returncode == expected_exit_code
        duration = end_time - start_time
        
        print(f"Duration: {duration:.2f}s")
        print(f"Exit code: {result.returncode}")
        if result.stdout:
            print(f"Stdout:\n{result.stdout}")
        if result.stderr:
            print(f"Stderr:\n{result.stderr}")
        
        return {
            "test_name": test_name,
            "command": command,
            "success": success,
            "exit_code": result.returncode,
            "duration": duration,
            "stdout": result.stdout,
            "stderr": result.stderr
        }
    except Exception as e:
        end_time = time.time()
        return {
            "test_name": test_name,
            "command": command,
            "success": False,
            "exit_code": -1,
            "duration": end_time - start_time,
            "stdout": "",
            "stderr": str(e)
        }

def main():
    print("=== NLM Comprehensive Test Suite and Performance Benchmarks ===")
    print("Testing Phase 6 integrated artificial brain system")
    
    tests = []
    
    # Test 1: Basic build and compilation
    tests.append(run_test(
        "Basic Build",
        "mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4",
        0
    ))
    
    # Test 2: Unit tests (existing test suite)
    tests.append(run_test(
        "Unit Tests",
        "cd build && ./nlm_test",
        0
    ))
    
    # Test 3: Convenience functions test
    tests.append(run_test(
        "Convenience Functions Test",
        "cd build && ./convenience_test",
        0
    ))
    
    # Test 4: Performance benchmark
    tests.append(run_test(
        "Performance Benchmark",
        "cd build && ./benchmark_test",
        0
    ))
    
    # Test 5: Integration test
    tests.append(run_test(
        "Integration Test",
        "cd build && ./integration_test",
        0
    ))
    
    # Test 6: Memory management test
    tests.append(run_test(
        "Memory Management Test",
        "cd build && ./memory_test",
        0
    ))
    
    # Test 7: API surface test
    tests.append(run_test(
        "API Surface Test",
        "cd build && ./api_test",
        0
    ))
    
    # Test 8: Phase 6 demo (existing functionality)
    tests.append(run_test(
        "Phase 6 Demo",
        "cd build && ./nlm_phase6_demo",
        0
    ))
    
    # Test 9: Phase 3 demo (existing functionality)
    tests.append(run_test(
        "Phase 3 Demo",
        "cd build && ./nlm_phase3_demo",
        0
    ))
    
    # Test 10: Phase 4 demo (existing functionality)
    tests.append(run_test(
        "Phase 4 Demo",
        "cd build && ./nlm_phase4_demo",
        0
    ))
    
    # Summary
    print("\n" + "="*80)
    print("TEST SUITE SUMMARY")
    print("="*80)
    
    passed = sum(1 for t in tests if t["success"])
    total = len(tests)
    
    print(f"Total Tests: {total}")
    print(f"Passed: {passed}")
    print(f"Failed: {total - passed}")
    print(f"Success Rate: {passed/total*100:.1f}%")
    
    total_duration = sum(t["duration"] for t in tests)
    print(f"Total Test Duration: {total_duration:.2f}s")
    print(f"Average Test Duration: {total_duration/total:.2f}s")
    
    # Performance details
    print("\n" + "="*80)
    print("PERFORMANCE DETAILS")
    print("="*80)
    
    for test in tests:
        status = "PASS" if test["success"] else "FAIL"
        print(f"{status:4} | {test['test_name']:35} | Duration: {test['duration']:6.2f}s")
    
    # Detailed results for failed tests
    failed_tests = [t for t in tests if not t["success"]]
    if failed_tests:
        print("\n" + "="*80)
        print("FAILED TESTS DETAILS")
        print("="*80)
        
        for test in failed_tests:
            print(f"\nTest: {test['test_name']}")
            print(f"Command: {test['command']}")
            if test["stderr"]:
                print(f"Error: {test['stderr']}")
    
    # Write results to file
    results = {
        "total_tests": total,
        "passed": passed,
        "failed": total - passed,
        "success_rate": passed/total*100,
        "total_duration": total_duration,
        "average_duration": total_duration/total,
        "tests": tests,
        "timestamp": time.strftime("%Y-%m-%d %H:%M:%S")
    }
    
    with open("test_results.json", "w") as f:
        json.dump(results, f, indent=2)
    
    print(f"\nTest results saved to: test_results.json")
    
    # Exit with appropriate code
    if failed_tests:
        print(f"\n{total - passed} tests failed!")
        sys.exit(1)
    else:
        print(f"\nAll {total} tests passed!")
        sys.exit(0)

if __name__ == "__main__":
    main()

