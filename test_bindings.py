#!/usr/bin/env python3

"""
Test script for NLM Python bindings.

This script provides basic functionality tests to verify that the NLM Python bindings
are working correctly and can be imported and used as expected.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from pynlm import Config, create_default_config, Brain, NeuronType, ActionType
from pynlm.types import NeuronId, RegionId
import numpy as np

def test_config_creation():
    """Test Config class creation and basic operations."""
    print("Testing Config creation...")
    
    # Test default config creation
    config = create_default_config()
    assert isinstance(config, Config)
    assert config.has("neuron_count")
    assert config.has("region_count")
    
    # Test getting values
    neurons = config.get_or("neuron_count", 100)
    assert neurons == 1000
    
    # Test setting values
    config.set("test_value", 42)
    assert config.get("test_value") == 42
    
    # Test fluent interface
    config.set("fluent_test", "value").set("fluent_test2", 123)
    assert config.get("fluent_test") == "value"
    assert config.get("fluent_test2") == 123
    
    print("✓ Config creation test passed")
    return True

def test_types():
    """Test type definitions."""
    print("Testing type definitions...")
    
    # Test NeuronId creation
    neuron_id = NeuronId(123)
    assert neuron_id.value == 123
    assert neuron_id.index == 123
    assert str(neuron_id) == "<NeuronId: 123>"
    
    # Test RegionId creation
    region_id = RegionId(456)
    assert region_id.value == 456
    assert region_id.index == 456
    
    # Test enums
    assert NeuronType.EXCITATORY.value == "excitatory"
    assert ActionType.MOVE_FORWARD.value == "move_forward"
    
    # Test type validation
    from pynlm.types import validate_neuron_id, validate_region_id
    
    validated_neuron = validate_neuron_id(neuron_id)
    assert isinstance(validated_neuron, NeuronId)
    
    validated_region = validate_region_id(region_id)
    assert isinstance(validated_region, RegionId)
    
    print("✓ Type definitions test passed")
    return True

def test_numpy_integration():
    """Test NumPy integration utilities."""
    print("Testing NumPy integration...")
    
    # Create NumPy arrays
    array1 = np.array([1, 2, 3, 4, 5])
    array2 = np.array([0.1, 0.2, 0.3, 0.4, 0.5])
    
    # Test basic operations
    assert np.sum(array1) == 15
    assert np.mean(array2) == 0.3
    
    # Test array conversion
    list_data = [1.0, 2.0, 3.0, 4.0, 5.0]
    np_array = np.array(list_data)
    assert np_array.shape == (5,)
    
    # Test random generation
    np.random.seed(42)
    random_array = np.random.randn(10)
    assert random_array.shape == (10,)
    
    print("✓ NumPy integration test passed")
    return True

def test_basic_imports():
    """Test basic module imports."""
    print("Testing basic imports...")
    
    # Test main module imports
    assert Config is not None
    assert create_default_config is not None
    assert Brain is not None
    assert NeuronType is not None
    assert ActionType is not None
    assert NeuronId is not None
    assert RegionId is not None
    
    print("✓ Basic imports test passed")
    return True

def test_configuration_templates():
    """Test configuration template functionality."""
    print("Testing configuration templates...")
    
    config = Config()
    
    # Test multiple configuration keys
    config.set("neuron_count", 500)
    config.set("region_count", 5)
    config.set("connection_probability", 0.2)
    config.set("timestep", 0.005)
    
    # Test getting all keys
    keys = config.get_keys()
    assert "neuron_count" in keys
    assert "region_count" in keys
    assert "connection_probability" in keys
    assert "timestep" in keys
    
    # Test configuration summary
    summary = config.summary()
    assert "neuron_count" in summary
    assert "region_count" in summary
    
    print("✓ Configuration templates test passed")
    return True

def test_type_validation():
    """Test type validation utilities."""
    print("Testing type validation...")
    
    from pynlm.types import (
        is_valid_neuron_id, is_valid_synapse_id, is_valid_region_id,
        validate_neuron_id, validate_synapse_id, validate_region_id
    )
    
    # Test valid IDs
    neuron_id = NeuronId(1)
    region_id = RegionId(2)
    synapse_id = SynapseId(3)
    
    assert is_valid_neuron_id(neuron_id)
    assert is_valid_region_id(region_id)
    assert is_valid_synapse_id(synapse_id)
    
    # Test validation
    validated_neuron = validate_neuron_id(neuron_id)
    validated_region = validate_region_id(region_id)
    validated_synapse = validate_synapse_id(synapse_id)
    
    assert isinstance(validated_neuron, NeuronId)
    assert isinstance(validated_region, RegionId)
    assert isinstance(validated_synapse, SynapseId)
    
    print("✓ Type validation test passed")
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
        test_numpy_integration,
        test_configuration_templates,
        test_type_validation,
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
