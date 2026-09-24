#!/usr/bin/env python3
"""
Comprehensive NLM Test Suite

This script runs all comprehensive test suites for the NLM project:
1. Python Bindings Testing - Edge cases, memory management, concurrency
2. C++ Core Testing - Brain components, plasticity, neuromodulation, memory, development, prediction
3. Integration Testing - Full brain-world-agent integration, cross-system communication, state persistence
4. Performance Testing - Performance regression, memory leaks, scaling, resource usage
5. Stress Testing - High-load simulation, complex networks, long-running, resource exhaustion
6. Fuzz Testing - Random input, configuration, parameter, state fuzzing
7. Randomized Testing - Monte Carlo testing, statistical validation, probability distributions, seed reproducibility
8. Regression Testing - Historical behavior, API compatibility, feature regression, performance regression

Usage:
    python3 comprehensive_test_runner.py [--single <test_type>] [--all]

Options:
    --single <test_type>  Run a specific test suite
    --all                 Run all test suites (default)
    --help                Show this help message

Test Types:
    python_bindings      Python bindings edge cases
    cpp_core            C++ core components edge cases
    integration         Integration edge cases
    performance         Performance regression tests
    stress             Stress testing
    fuzz               Fuzz testing
    randomized         Randomized testing
    regression         Regression testing

Return Codes:
    0 - All tests passed
    1 - Some tests failed
    2 - Invalid arguments or errors
"""

import subprocess
import sys
import os
import argparse
import time

# Test paths
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

TEST_SUITES = {
    'python_bindings': 'tests/comprehensive/test_python_bindings_edge_cases.py',
    'cpp_core': 'tests/comprehensive/test_cpp_core_edge_cases.cpp',
    'integration': 'tests/comprehensive/test_integration_edge_cases.cpp',
    'performance': 'tests/performance/test_performance_regression.cpp',
    'stress': 'tests/stress/test_stress_edge_cases.cpp',
    'fuzz': 'tests/fuzz/test_fuzz_edge_cases.cpp',
    'randomized': 'tests/randomized/test_randomized_behavior.cpp',
    'regression': 'tests/regression/test_regression_edge_cases.cpp'
}

class NLMComprehensiveTestRunner:
    def __init__(self):
        self.results = {}
        self.failed_suites = []
        self.passed_suites = []

    def run_python_test(self, test_path):
        """Run a Python test suite."""
        print(f"\n{'='*80}")
        print(f"Running Python Test Suite: {test_path}")
        print(f"{'='*80}")
        
        try:
            result = subprocess.run(
                [sys.executable, test_path],
                cwd=BASE_DIR,
                capture_output=True,
                text=True,
                timeout=300  # 5 minutes timeout
            )
            
            print(result.stdout)
            if result.stderr:
                print("STDERR:")
                print(result.stderr)
            
            return result.returncode == 0
            
        except subprocess.TimeoutExpired:
            print(f"Test suite {test_path} timed out after 5 minutes")
            return False
        except Exception as e:
            print(f"Error running test suite {test_path}: {e}")
            return False

    def run_cpp_test(self, test_path):
        """Run a C++ test suite."""
        print(f"\n{'='*80}")
        print(f"Running C++ Test Suite: {test_path}")
        print(f"{'='*80}")
        
        # First compile the test
        try:
            # Create a temporary build directory
            build_dir = os.path.join(BASE_DIR, "build_test")
            os.makedirs(build_dir, exist_ok=True)
            
            # Compile the test
            compile_cmd = [
                "g++", "-std=c++11", "-O2", "-I", os.path.join(BASE_DIR, "src"),
                "-I", os.path.join(BASE_DIR, "python"),
                "-I", os.path.join(BASE_DIR, "tests"),
                "-I", os.path.join(BASE_DIR, "tests/comprehensive"),
                "-I", os.path.join(BASE_DIR, "tests/performance"),
                "-I", os.path.join(BASE_DIR, "tests/stress"),
                "-I", os.path.join(BASE_DIR, "tests/fuzz"),
                "-I", os.path.join(BASE_DIR, "tests/randomized"),
                "-I", os.path.join(BASE_DIR, "tests/regression"),
                "-L", os.path.join(BASE_DIR, "build"),
                "-o", os.path.join(build_dir, "test_runner"),
                test_path
            ]
            
            print(f"Compiling: {' '.join(compile_cmd)}")
            compile_result = subprocess.run(
                compile_cmd,
                cwd=BASE_DIR,
                capture_output=True,
                text=True
            )
            
            if compile_result.returncode != 0:
                print(f"Compilation failed:")
                print(compile_result.stdout)
                print(compile_result.stderr)
                return False
            
            # Run the compiled test
            run_cmd = [os.path.join(build_dir, "test_runner")]
            run_result = subprocess.run(
                run_cmd,
                cwd=BASE_DIR,
                capture_output=True,
                text=True,
                timeout=300  # 5 minutes timeout
            )
            
            print(run_result.stdout)
            if run_result.stderr:
                print("STDERR:")
                print(run_result.stderr)
            
            return run_result.returncode == 0
            
        except subprocess.TimeoutExpired:
            print(f"Test suite {test_path} timed out after 5 minutes")
            return False
        except Exception as e:
            print(f"Error running C++ test suite {test_path}: {e}")
            return False

    def run_test_suite(self, suite_name):
        """Run a specific test suite."""
        test_path = TEST_SUITES.get(suite_name)
        if not test_path:
            print(f"Unknown test suite: {suite_name}")
            return False
        
        if not os.path.exists(os.path.join(BASE_DIR, test_path)):
            print(f"Test suite not found: {test_path}")
            return False
        
        # Determine if it's a Python or C++ test
        if test_path.endswith('.py'):
            success = self.run_python_test(test_path)
        else:
            success = self.run_cpp_test(test_path)
        
        return success

    def run_all_tests(self):
        """Run all test suites."""
        print("Starting Comprehensive NLM Test Suite")
        print("=" * 80)
        
        all_passed = True
        
        for suite_name in TEST_SUITES.keys():
            print(f"\n{'-'*80}")
            print(f"Running: {suite_name.replace('_', ' ').title()} Tests")
            print(f"{'-'*80}")
            
            success = self.run_test_suite(suite_name)
            
            if success:
                self.passed_suites.append(suite_name)
                print(f"✅ {suite_name.replace('_', ' ').title()} Tests PASSED")
            else:
                self.failed_suites.append(suite_name)
                all_passed = False
                print(f"❌ {suite_name.replace('_', ' ').title()} Tests FAILED")
        
        # Summary
        print(f"\n{'='*80}")
        print("TEST SUMMARY")
        print(f"{'='*80}")
        print(f"Total Test Suites: {len(TEST_SUITES)}")
        print(f"Passed: {len(self.passed_suites)}")
        print(f"Failed: {len(self.failed_suites)}")
        
        if len(self.failed_suites) > 0:
            print(f"\nFailed Test Suites:")
            for suite in self.failed_suites:
                print(f"  - {suite}")
        
        if all_passed:
            print(f"\n🎉 ALL TEST SUITES PASSED!")
            print(f"\nComprehensive test coverage includes:")
            print(f"  ✅ Python Bindings Testing (edge cases, memory, concurrency)")
            print(f"  ✅ C++ Core Testing (brain, plasticity, neuromodulation, memory, development, prediction)")
            print(f"  ✅ Integration Testing (brain-world-agent, cross-system, state persistence)")
            print(f"  ✅ Performance Testing (regression, memory leaks, scaling, resources)")
            print(f"  ✅ Stress Testing (high-load, complex networks, long-running, exhaustion)")
            print(f"  ✅ Fuzz Testing (random input, configuration, parameters, state)")
            print(f"  ✅ Randomized Testing (Monte Carlo, statistical, distributions, seed)")
            print(f"  ✅ Regression Testing (historical, API, features, performance)")
            return True
        else:
            print(f"\n⚠️  {len(self.failed_suites)} TEST SUITES FAILED")
            return False

    def main(self):
        """Main entry point."""
        parser = argparse.ArgumentParser(description='Comprehensive NLM Test Suite')
        parser.add_argument('--single', type=str, help='Run a specific test suite')
        parser.add_argument('--all', action='store_true', help='Run all test suites (default)')
        parser.add_argument('--help', action='store_true', help='Show this help message')
        
        args = parser.parse_args()
        
        if args.help:
            parser.print_help()
            return 0
        
        if args.single:
            # Run single test suite
            if args.single in TEST_SUITES:
                success = self.run_test_suite(args.single)
                return 0 if success else 1
            else:
                print(f"Invalid test suite: {args.single}")
                print(f"Available test suites: {', '.join(TEST_SUITES.keys())}")
                return 2
        
        # Run all test suites
        success = self.run_all_tests()
        return 0 if success else 1

if __name__ == "__main__":
    runner = NLMComprehensiveTestRunner()
    sys.exit(runner.main())
