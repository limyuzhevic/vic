#!/usr/bin/env python3

"""
Simple test script for NLM Python bindings - minimal dependencies.

This script provides basic functionality tests to verify that the NLM Python bindings
are working correctly with minimal dependencies.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_basic_imports():
    """Test basic module imports."""
    print("Testing basic imports...")
    
    try:
        from pynlm import Config
        print("✓ Config imported successfully")
    except ImportError as e:
        print(f"✗ Config import failed: {e}")
        return False
    
    try:
        from pynlm.config import create_default_config
        print("✓ create_default_config imported successfully")
    except ImportError as e:
        print(f"✗ create_default_config import failed: {e}")
        return False
    
    try:
        from pynlm.types import NeuronId, RegionId, NeuronType
        print("✓ Type definitions imported successfully")
    except ImportError as e:
        print(f"✗ Type definitions import failed: {e}")
        return False
    
    print("✓ Basic imports test passed")
    return True

def test_config_creation():
    """Test Config class creation and basic operations."""
    print("Testing Config creation...")
    
    try:
        from pynlm import Config, create_default_config
    except ImportError as e:
        print(f"✗ Import failed: {e}")
        return False
    
    # Test default config creation
    config = create_default_config()
    if not isinstance(config, Config):
        print(f"✗ create_default_config returned wrong type: {type(config)}")
        return False
    
    print("✓ create_default_config returned Config instance")
    
    # Test setting values
    config.set("test_value", 42)
    if config.get("test_value") != 42:
        print(f"✗ Config set/get failed")
        return False
    
    print("✓ Config set/get test passed")
    
    # Test fluent interface
    config.set("fluent_test", "value").set("fluent_test2", 123)
    if config.get("fluent_test") != "value":
        print(f"✗ Fluent interface set failed")
        return False
    
    if config.get("fluent_test2") != 123:
        print(f"✗ Fluent interface set failed")
        return False
    
    print("✓ Fluent interface test passed")
    return True

def test_types():
    """Test type definitions."""
    print("Testing type definitions...")
    
    try:
        from pynlm.types import NeuronId, RegionId, NeuronType, ActionType
    except ImportError as e:
        print(f"✗ Type definitions import failed: {e}")
        return False
    
    # Test NeuronId creation
    neuron_id = NeuronId(123)
    if neuron_id.value != 123:
        print(f"✗ NeuronId creation failed: {neuron_id.value}")
        return False
    
    if neuron_id.index != 123:
        print(f"✗ NeuronId index failed: {neuron_id.index}")
        return False
    
    if str(neuron_id) != "<NeuronId: 123>":
        print(f"✗ NeuronId string representation failed: {str(neuron_id)}")
        return False
    
    # Test RegionId creation
    region_id = RegionId(456)
    if region_id.value != 456:
        print(f"✗ RegionId creation failed: {region_id.value}")
        return False
    
    # Test enums
    if NeuronType.EXCITATORY.value != "excitatory":
        print(f"✗ NeuronType enum failed: {NeuronType.EXCITATORY.value}")
        return False
    
    if ActionType.MOVE_FORWARD.value != "move_forward":
        print(f"✗ ActionType enum failed: {ActionType.MOVE_FORWARD.value}")
        return False
    
    print("✓ Type definitions test passed")
    return True

def test_configuration_templates():
    """Test configuration template functionality."""
    print("Testing configuration templates...")
    
    try:
        from pynlm import Config
    except ImportError as e:
        print(f"✗ Import failed: {e}")
        return False
    
    config = Config()
    
    # Test multiple configuration keys
    config.set("neuron_count", 500)
    config.set("region_count", 5)
    config.set("connection_probability", 0.2)
    config.set("timestep", 0.005)
    
    # Test getting all keys
    keys = config.get_keys()
    required_keys = ["neuron_count", "region_count", "connection_probability", "timestep"]
    
    for key in required_keys:
        if key not in keys:
            print(f"✗ Required key '{key}' not found in {keys}")
            return False
    
    print("✓ Configuration keys test passed")
    
    # Test configuration summary
    summary = config.summary()
    for key in required_keys:
        if key not in summary:
            print(f"✗ Key '{key}' not found in summary")
            return False
    
    print("✓ Configuration summary test passed")
    return True

def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_imports,
        test_config_creation,
        test_types,
        test_configuration_templates,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
                print(f"✗ {test.__name__} failed")
        except Exception as e:
            failed += 1
            print(f"✗ {test.__name__} failed with exception: {e}")
            import traceback
            traceback.print_exc()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("🎉 All tests passed!")
        return 0
    else:
        print("❌ Some tests failed.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
