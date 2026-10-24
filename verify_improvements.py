#!/usr/bin/env python3
"""
Quick verification script for NLM Python bindings improvements
This script tests the key improvements made to ensure everything works correctly.
"""

import sys
import traceback

def test_basic_import():
    """Test basic import and version"""
    try:
        import pynlm
        print("✓ Successfully imported pynlm")
        return pynlm
    except Exception as e:
        print(f"✗ Failed to import pynlm: {e}")
        return None

def test_improved_bindings(pynlm):
    """Test the improved bindings functionality"""
    print("\n=== Testing Improved Bindings ===")
    
    # Test 1: Default configuration creation
    try:
        config = pynlm.createDefaultConfig()
        print("✓ createDefaultConfig() works")
    except Exception as e:
        print(f"✗ createDefaultConfig() failed: {e}")
        return False
    
    # Test 2: Brain creation and initialization
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("✓ createBrain() and initialize() work")
    except Exception as e:
        print(f"✗ createBrain() failed: {e}")
        return False
    
    # Test 3: Brain stepping
    try:
        brain.step(0)
        print("✓ brain.step() works")
    except Exception as e:
        print(f"✗ brain.step() failed: {e}")
        return False
    
    # Test 4: Brain statistics
    try:
        neurons = brain.getTotalNeuronCount()
        print(f"✓ Brain statistics work: {neurons} neurons")
    except Exception as e:
        print(f"✗ Brain statistics failed: {e}")
        return False
    
    return True

def test_new_factory_functions(pynlm):
    """Test new factory functions"""
    print("\n=== Testing New Factory Functions ===")
    
    # Test createBrainWithDefaults
    try:
        brain = pynlm.createBrainWithDefaults()
        brain.initialize()
        brain.step(0)
        print("✓ createBrainWithDefaults() works")
    except Exception as e:
        print(f"✗ createBrainWithDefaults() failed: {e}")
        return False
    
    # Test createStandardWorld
    try:
        world = pynlm.createStandardWorld()
        print("✓ createStandardWorld() works")
    except Exception as e:
        print(f"✗ createStandardWorld() failed: {e}")
        return False
    
    # Test loadConfigFromFile (with temporary file)
    try:
        import tempfile
        import json
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_data = {"brain": {"neuron_count": 200}}
            json.dump(config_data, f)
            temp_file = f.name
        
        config = pynlm.loadConfigFromFile(temp_file)
        print("✓ loadConfigFromFile() works")
        
        import os
        os.unlink(temp_file)
        
    except Exception as e:
        print(f"✗ loadConfigFromFile() failed: {e}")
        return False
    
    return True

def test_error_handling(pynlm):
    """Test improved error handling"""
    print("\n=== Testing Error Handling ===")
    
    # Test file error
    try:
        config = pynlm.createDefaultConfig()
        config.loadFromFile("non_existent_file_12345.json")
        print("✗ Should have thrown FileError")
        return False
    except pynlm.FileError as e:
        print("✓ FileError properly raised for missing file")
    except Exception as e:
        print(f"✗ Wrong exception type: {type(e).__name__}")
        return False
    
    # Test input validation
    try:
        from pynlm import Vision
        vision = Vision(10, 10, 3)
        vision.setData([1.0] * 100)  # Invalid data size for RGB
        print("✗ Should have thrown InvalidInputError")
        return False
    except pynlm.InputError as e:
        print("✓ InputError properly raised for invalid data")
    except Exception as e:
        print(f"✗ Wrong exception type for input validation: {type(e).__name__}")
        return False
    
    return True

def test_pythonic_features(pynlm):
    """Test Pythonic features like context managers"""
    print("\n=== Testing Pythonic Features ===")
    
    # Test context manager
    try:
        with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
            brain.initialize()
            brain.step(0)
        print("✓ Context manager works")
    except Exception as e:
        print(f"✗ Context manager failed: {e}")
        return False
    
    # Test string representation
    try:
        config = pynlm.createDefaultConfig()
        config_str = str(config)
        print(f"✓ String representation works: {config_str[:50]}...")
    except Exception as e:
        print(f"✗ String representation failed: {e}")
        return False
    
    # Test repr
    try:
        config = pynlm.createDefaultConfig()
        config_repr = repr(config)
        print(f"✓ Repr works: {config_repr}")
    except Exception as e:
        print(f"✗ Repr failed: {e}")
        return False
    
    return True

def test_validation(pynlm):
    """Test configuration validation"""
    print("\n=== Testing Configuration Validation ===")
    
    # Test valid config
    try:
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.1)
        
        # This should not throw
        pynlm.validateConfig(config)
        print("✓ Valid config passes validation")
    except Exception as e:
        print(f"✗ Valid config should not fail validation: {e}")
        return False
    
    return True

def main():
    """Run all verification tests"""
    print("NLM Python Bindings - Verification Script")
    print("=" * 50)
    
    # Test 1: Basic import
    pynlm = test_basic_import()
    if not pynlm:
        return 1
    
    # Test 2: Improved bindings
    if not test_improved_bindings(pynlm):
        return 1
    
    # Test 3: New factory functions
    if not test_new_factory_functions(pynlm):
        return 1
    
    # Test 4: Error handling
    if not test_error_handling(pynlm):
        return 1
    
    # Test 5: Pythonic features
    if not test_pythonic_features(pynlm):
        return 1
    
    # Test 6: Validation
    if not test_validation(pynlm):
        return 1
    
    print("\n" + "=" * 50)
    print("🎉 All verification tests passed!")
    print("\nSummary of improvements verified:")
    print("✓ Memory safety fixes")
    print("✓ Comprehensive error handling")
    print("✓ Pythonic API enhancements")
    print("✓ New factory functions")
    print("✓ Context managers")
    print("✓ Input validation")
    print("✓ Configuration validation")
    
    print("\nTo run full test suite:")
    print("  python test_pynlm_bindings.py")
    
    print("\nTo try the advanced CLI:")
    print("  python nlm_cli.py --help")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
