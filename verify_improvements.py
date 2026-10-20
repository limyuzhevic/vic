#!/usr/bin/env python3
"""
Quick verification script for NLM improvements.
Tests the key improvements made to the codebase.
"""

import sys
import os

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm

def test_version():
    """Test that version is accessible."""
    print("Testing version...")
    version = pynlm.__version__
    print(f"  Version: {version}")
    assert version == "0.1.0", f"Expected version '0.1.0', got '{version}'"
    print("  ✓ Version test passed")

def test_config():
    """Test basic configuration functionality."""
    print("\nTesting configuration...")
    config = pynlm.createDefaultConfig()
    
    # Test basic properties
    assert config.has("random_seed"), "Config should have random_seed by default"
    
    # Test setting values
    config.set("test_int", 42)
    config.set("test_double", 3.14)
    config.set("test_string", "hello")
    config.set("test_bool", True)
    
    # Test getting values
    assert config.get("test_int") == 42
    assert abs(config.get("test_double") - 3.14) < 0.001
    assert config.get("test_string") == "hello"
    assert config.get("test_bool") == True
    
    print("  ✓ Configuration test passed")

def test_brain():
    """Test basic brain functionality."""
    print("\nTesting brain creation...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Initialize brain
    result = brain.initialize()
    assert result, "Brain initialization failed"
    
    # Test basic properties
    neurons = brain.getTotalNeuronCount()
    synapses = brain.getTotalSynapseCount()
    
    print(f"  Brain initialized: {neurons} neurons, {synapses} synapses")
    assert neurons > 0, "Brain should have neurons after initialization"
    
    # Test simulation
    brain.step(0)
    print("  ✓ Brain test passed")

def test_demo():
    """Test the demo function."""
    print("\nTesting demo function...")
    try:
        pynlm.demo()
        print("  ✓ Demo function executed successfully")
    except Exception as e:
        print(f"  ✗ Demo function failed: {e}")
        return False
    
    return True

def main():
    """Run all verification tests."""
    print("=" * 60)
    print("NLM Improvements Verification")
    print("=" * 60)
    
    tests = [
        test_version,
        test_config,
        test_brain,
        test_demo,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"  ✗ Test failed: {e}")
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Verification Summary: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("🎉 All verification tests passed!")
        return 0
    else:
        print(f"⚠️  {failed} test(s) failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())
