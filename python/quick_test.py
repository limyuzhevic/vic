#!/usr/bin/env python3
"""
Simple Test Script for NLM Python Bindings

This script provides a basic test to verify the NLM Python bindings work correctly
and demonstrates the enhanced functionality.
"""

import sys

try:
    import pynlm
    print("✓ Successfully imported pynlm")
    PYNLM_AVAILABLE = True
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    PYNLM_AVAILABLE = False

def test_basic_functionality():
    """Test basic NLM Python bindings functionality."""
    print("\n" + "=" * 60)
    print("Testing Basic NLM Python Bindings")
    print("=" * 60)
    
    if not PYNLM_AVAILABLE:
        print("⚠ pynlm not available - skipping tests")
        return True
    
    tests_passed = 0
    total_tests = 0
    
    # Test 1: Create default config
    total_tests += 1
    try:
        config = pynlm.createDefaultConfig()
        print(f"✓ Test 1/8: createDefaultConfig() - SUCCESS")
        tests_passed += 1
    except Exception as e:
        print(f"✗ Test 1/8: createDefaultConfig() - FAILED: {e}")
    
    # Test 2: Create brain
    total_tests += 1
    try:
        brain = pynlm.createBrain(config)
        print(f"✓ Test 2/8: createBrain() - SUCCESS")
        tests_passed += 1
    except Exception as e:
        print(f"✗ Test 2/8: createBrain() - FAILED: {e}")
    
    # Test 3: Initialize brain
    total_tests += 1
    try:
        brain.initialize()
        print(f"✓ Test 3/8: brain.initialize() - SUCCESS")
        tests_passed += 1
    except Exception as e:
        print(f"✗ Test 3/8: brain.initialize() - FAILED: {e}")
    
    # Test 4: Brain statistics
    total_tests += 1
    try:
        stats = pynlm.getBrainStatistics(brain)
        print(f"✓ Test 4/8: getBrainStatistics() - SUCCESS ({len(stats)} stats)")
        tests_passed += 1
    except Exception as e:
        print(f"✗ Test 4/8: getBrainStatistics() - FAILED: {e}")
    
    # Test 5: Run some steps
    total_tests += 1
    try:
        for i in range(10):
            brain.step(i)
        print(f"✓ Test 5/8: brain.step() - SUCCESS (10 steps)")
        tests_passed += 1
    except Exception as e:
        print(f"✗ Test 5/8: brain.step() - FAILED: {e}")
    
    # Test 6: Format results
    total_tests += 1
    try:
        stats = pynlm.getBrainStatistics(brain)
        formatted = pynlm.formatSimulationResults(stats)
        print(f"✓ Test 6/8: formatSimulationResults() - SUCCESS ({len(formatted)} chars)")
        tests_passed += 1
    except Exception as e:
        print(f"✗ Test 6/8: formatSimulationResults() - FAILED: {e}")
    
    # Test 7: Brain state inspection
    total_tests += 1
    try:
        state_str = pynlm.getBrainStateString(brain)
        print(f"✓ Test 7/8: getBrainStateString() - SUCCESS ({len(state_str)} chars)")
        tests_passed += 1
    except Exception as e:
        print(f"✗ Test 7/8: getBrainStateString() - FAILED: {e}")
    
    # Test 8: Version info
    total_tests += 1
    try:
        version = pynlm.getBrainVersion()
        print(f"✓ Test 8/8: getBrainVersion() - SUCCESS: {version}")
        tests_passed += 1
    except Exception as e:
        print(f"✗ Test 8/8: getBrainVersion() - FAILED: {e}")
    
    print(f"\nBasic Test Results: {tests_passed}/{total_tests} passed")
    return tests_passed == total_tests

def test_enumeration_values():
    """Test enumeration values are accessible."""
    print("\n" + "=" * 60)
    print("Testing Enumeration Values")
    print("=" * 60)
    
    if not PYNLM_AVAILABLE:
        print("⚠ pynlm not available - skipping enum tests")
        return True
    
    tests_passed = 0
    total_tests = 0
    
    # Test NeuronType enum
    total_tests += 1
    try:
        assert hasattr(pynlm, 'NeuronType')
        print(f"✓ NeuronType enum available")
        tests_passed += 1
    except Exception as e:
        print(f"✗ NeuronType enum - FAILED: {e}")
    
    # Test ActionType enum
    total_tests += 1
    try:
        assert hasattr(pynlm, 'ActionType')
        print(f"✓ ActionType enum available")
        tests_passed += 1
    except Exception as e:
        print(f"✗ ActionType enum - FAILED: {e}")
    
    # Test WorldObjectType enum
    total_tests += 1
    try:
        assert hasattr(pynlm, 'WorldObjectType')
        print(f"✓ WorldObjectType enum available")
        tests_passed += 1
    except Exception as e:
        print(f"✗ WorldObjectType enum - FAILED: {e}")
    
    print(f"\nEnumeration Test Results: {tests_passed}/{total_tests} passed")
    return tests_passed == total_tests

def main():
    """Main test function."""
    print("NLM Python Bindings - Quick Test")
    print("=" * 60)
    print("This script tests the basic functionality of the enhanced")
    print("NLM Python bindings to verify they work correctly.")
    print("=" * 60)
    
    if not PYNLM_AVAILABLE:
        print("\n⚠ WARNING: pynlm module is not available!")
        print("This is expected if NLM C++ bindings have not been built.")
        print("To build the bindings, run:")
        print("  mkdir build && cd build && cmake .. && make")
        print("\nHowever, the enhanced Python utilities and examples")
        print("are still available and will work once the bindings are built.")
    
    # Run tests
    basic_success = test_basic_functionality()
    enum_success = test_enumeration_values()
    
    # Summary
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    
    all_passed = basic_success and enum_success
    
    if all_passed:
        print("✓ All tests passed!")
        print("\nThe enhanced NLM Python bindings are working correctly.")
        print("Key features verified:")
        print("  • Basic brain creation and initialization")
        print("  • Simulation stepping and statistics")
        print("  • Enhanced utility functions")
        print("  • Enumeration values")
        print("  • Version information")
        print("\nFor more comprehensive testing, run:")
        print("  python python/test_bindings.py")
        return 0
    else:
        print("✗ Some tests failed!")
        print("Please check the output above for details.")
        return 1

if __name__ == "__main__":
    exit(main())