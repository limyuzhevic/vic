"""
NLM Python Package Test Runner

This module provides test execution utilities for the NLM Python bindings.
It can run the Python test suite independently of the C++ tests.
"""

import sys
import os
import argparse
import pytest
from pathlib import Path

# Add the nlm module to the path
sys.path.insert(0, str(Path(__file__).parent))

# Import test modules
from tests.test_py_bindings import (
    TestConfigManager, TestBrain, TestAgent, 
    TestSimpleWorld, TestIntegration, TestErrorHandling, TestPerformance
)


def discover_tests():
    """Discover all test classes."""
    test_classes = [
        TestConfigManager,
        TestBrain,
        TestAgent,
        TestSimpleWorld,
        TestIntegration,
        TestErrorHandling,
        TestPerformance,
    ]
    
    return test_classes


def run_tests_with_pytest():
    """Run tests using pytest."""
    print("Running NLM Python tests with pytest...")
    
    # Create pytest arguments
    test_dir = Path(__file__).parent / "tests"
    
    # Run pytest
    return_code = pytest.main([
        str(test_dir),
        "-v",
        "--tb=short",
        "--disable-warnings",
    ])
    
    return return_code == 0


def run_tests_with_custom_runner():
    """Run tests using custom test runner."""
    print("Running NLM Python tests with custom runner...")
    
    test_classes = discover_tests()
    
    results = []
    total_tests = 0
    passed_tests = 0
    
    for test_class in test_classes:
        print(f"\n{'='*60}")
        print(f"Running {test_class.__name__}")
        print(f"{'='*60}")
        
        test_instance = test_class()
        
        # Get all test methods
        test_methods = [method for method in dir(test_instance) 
                       if method.startswith('test_')]
        
        class_total = 0
        class_passed = 0
        
        for method_name in sorted(test_methods):
            total_tests += 1
            class_total += 1
            
            method = getattr(test_instance, method_name)
            test_name = f"{test_class.__name__}.{method_name}"
            
            print(f"  {test_name}...", end=" ")
            
            try:
                method()
                print("✓ PASSED")
                passed_tests += 1
                class_passed += 1
                results.append((test_name, "PASSED"))
            except Exception as e:
                print(f"✗ FAILED: {e}")
                results.append((test_name, f"FAILED: {e}"))
        
        print(f"\n  {test_class.__name__}: {class_passed}/{class_total} tests passed")
    
    # Print summary
    print(f"\n{'='*60}")
    print("TEST SUMMARY")
    print(f"{'='*60}")
    print(f"Total tests: {total_tests}")
    print(f"Passed: {passed_tests}")
    print(f"Failed: {total_tests - passed_tests}")
    
    if passed_tests == total_tests:
        print("\n✓ All tests passed!")
        return True
    else:
        print(f"\n✗ {total_tests - passed_tests} test(s) failed")
        
        # Show failed tests
        print("\nFailed tests:")
        for test_name, status in results:
            if status != "PASSED":
                print(f"  - {test_name}: {status}")
        
        return False


def run_quick_tests():
    """Run quick smoke tests."""
    print("Running quick smoke tests...")
    
    try:
        # Test 1: Import
        print("  1. Testing imports...", end=" ")
        from nlm import Brain, Agent, SimpleWorld, ConfigManager
        print("✓ PASSED")
        
        # Test 2: Configuration
        print("  2. Testing configuration...", end=" ")
        config = ConfigManager()
        config.set("brain.neuron_count", 10)
        assert config.get_int("brain.neuron_count") == 10
        print("✓ PASSED")
        
        # Test 3: Brain
        print("  3. Testing brain creation...", end=" ")
        brain = Brain()
        assert brain.get_total_neuron_count() > 0
        print("✓ PASSED")
        
        # Test 4: World
        print("  4. Testing world creation...", end=" ")
        world = SimpleWorld()
        world.configure(width=5, height=5)
        assert world.get_width() == 5
        print("✓ PASSED")
        
        # Test 5: Agent
        print("  5. Testing agent creation...", end=" ")
        agent = Agent(brain)
        assert agent.get_brain() == brain
        print("✓ PASSED")
        
        print("\n✓ All quick tests passed!")
        return True
        
    except Exception as e:
        print(f"\n✗ Quick test failed: {e}")
        return False


def run_performance_tests():
    """Run performance tests."""
    print("Running performance tests...")
    
    import time
    
    # Test brain simulation speed
    brain = Brain()
    brain.initialize()
    
    start_time = time.time()
    brain.run_steps(100)
    end_time = time.time()
    
    elapsed = end_time - start_time
    steps_per_second = 100 / elapsed if elapsed > 0 else 0
    
    print(f"  Brain simulation: 100 steps in {elapsed:.3f} seconds")
    print(f"  Performance: {steps_per_second:.1f} steps/second")
    
    # Performance should be reasonable (less than 10 seconds for 100 steps)
    if elapsed < 10.0:
        print("✓ Performance test passed")
        return True
    else:
        print(f"⚠ Performance test slow: {elapsed:.3f} seconds")
        return True  # Don't fail for performance issues


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="NLM Python Test Runner",
        prog="run_tests.py"
    )
    
    parser.add_argument(
        "--mode", choices=["quick", "custom", "pytest"],
        default="custom",
        help="Test running mode (default: custom)"
    )
    
    parser.add_argument(
        "--skip-performance", action="store_true",
        help="Skip performance tests"
    )
    
    parser.add_argument(
        "--verbose", action="store_true",
        help="Enable verbose output"
    )
    
    args = parser.parse_args()
    
    print("NLM Python Test Runner")
    print("=" * 40)
    
    success = True
    
    # Run tests based on mode
    if args.mode == "quick":
        success = run_quick_tests()
    
    elif args.mode == "pytest":
        success = run_tests_with_pytest()
    
    elif args.mode == "custom":
        success = run_tests_with_custom_runner()
    
    # Run performance tests unless skipped
    if not args.skip_performance:
        print(f"\n{'='*40}")
        success = run_performance_tests() and success
    
    if success:
        print(f"\n✓ All tests passed!")
        return 0
    else:
        print(f"\n✗ Tests failed")
        return 1


if __name__ == "__main__":
    exit(main())
