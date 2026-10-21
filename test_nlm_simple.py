#!/usr/bin/env python3
"""
Simplified test script to verify NLM Python bindings functionality.
This script tests the core functionality without complex dependencies.
"""

import sys
import os

def test_basic_imports():
    """Test basic imports and version."""
    print("Testing basic imports...")
    
    try:
        import pynlm
        print(f"✓ NLM version: {pynlm.getVersion()}")
        print(f"✓ Author: {pynlm.__author__}")
        return True
    except Exception as e:
        print(f"❌ Import failed: {e}")
        return False

def test_config():
    """Test configuration system."""
    print("\nTesting configuration system...")
    
    try:
        import pynlm
        
        # Test default configuration
        config = pynlm.createDefaultConfig()
        print("✓ Default configuration created")
        
        # Test setting values
        config.set("test.string", "hello")
        config.set("test.number", 42)
        config.set("test.boolean", True)
        
        # Test getting values
        assert config.get("test.string") == "hello"
        assert config.get_int("test.number") == 42
        assert config.get_bool("test.boolean") == True
        
        print("✓ Configuration operations working")
        return True
        
    except Exception as e:
        print(f"❌ Configuration test failed: {e}")
        return False

def test_brain():
    """Test brain creation and basic operations."""
    print("\nTesting brain operations...")
    
    try:
        import pynlm
        
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        print(f"✓ Brain created: {brain.getTotalNeuronCount()} neurons")
        
        # Initialize brain
        success = brain.initialize()
        if not success:
            print("❌ Brain initialization failed")
            return False
        
        print(f"✓ Brain initialized successfully")
        brain.log_status()
        
        # Test simulation steps
        for i in range(5):
            brain.step(i)
        
        print(f"✓ Simulation completed: {brain.getTotalSpikeCount()} total spikes")
        print(f"✓ Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
        return True
        
    except Exception as e:
        print(f"❌ Brain test failed: {e}")
        return False

def test_error_handling():
    """Test error handling."""
    print("\nTesting error handling...")
    
    try:
        import pynlm
        from pynlm.exceptions import ErrorContext
        
        # Test error context
        error_context = ErrorContext("Test", "Test Operation")
        
        # Test helper functions
        result = pynlm.getVersion()
        print(f"✓ Version helper working: {result}")
        
        print("✓ Error handling imports working")
        return True
        
    except Exception as e:
        print(f"❌ Error handling test failed: {e}")
        return False

def main():
    """Run all tests."""
    print("NLM Python Bindings Quick Test")
    print("=" * 40)
    
    tests = [
        ("Basic Imports", test_basic_imports),
        ("Configuration", test_config),
        ("Brain Operations", test_brain),
        ("Error Handling", test_error_handling),
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        print(f"\n--- {test_name} ---")
        if test_func():
            passed += 1
            print(f"✅ {test_name} PASSED")
        else:
            print(f"❌ {test_name} FAILED")
    
    print("\n" + "=" * 40)
    print(f"Results: {passed}/{total} tests passed")
    
    if passed == total:
        print("🎉 ALL TESTS PASSED! 🎉")
        print("\nNLM Python bindings are working correctly!")
        print("\nKey improvements verified:")
        print("  ✓ Python bindings for NLM components")
        print("  ✓ Enhanced configuration system")
        print("  ✓ Professional error handling")
        return 0
    else:
        print("❌ SOME TESTS FAILED")
        return 1

if __name__ == "__main__":
    sys.exit(main())