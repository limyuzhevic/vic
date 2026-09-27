#!/usr/bin/env python3
"""
Basic test script for the enhanced NLM Python bindings.

This script tests the core functionality of the enhanced bindings.
"""

import sys
import os

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm

def test_basic_imports():
    """Test that all required modules can be imported"""
    print("Testing basic imports...")
    
    # Test main module
    assert hasattr(pynlm, '__version__'), "Missing __version__ attribute"
    assert hasattr(pynlm, '__api_version__'), "Missing __api_version__ attribute"
    assert pynlm.__version__ == "0.1.0", f"Wrong version: {pynlm.__version__}"
    assert pynlm.__api_version__ == "enhanced", f"Wrong API version: {pynlm.__api_version__}"
    
    # Test exception classes
    assert hasattr(pynlm, 'NLMException'), "Missing NLMException"
    assert hasattr(pynlm, 'ConfigurationError'), "Missing ConfigurationError"
    assert hasattr(pynlm, 'SimulationError'), "Missing SimulationError"
    assert hasattr(pynlm, 'InitializationError'), "Missing InitializationError"
    
    # Test helper classes
    assert hasattr(pynlm, 'PythonBrain'), "Missing PythonBrain"
    assert hasattr(pynlm, 'PythonWorld'), "Missing PythonWorld"
    assert hasattr(pynlm, 'PythonAgentBrain'), "Missing PythonAgentBrain"
    assert hasattr(pynlm, 'ConfigBuilder'), "Missing ConfigBuilder"
    assert hasattr(pynlm, 'SimulationRunner'), "Missing SimulationRunner"
    assert hasattr(pynlm, 'VisualizationHelper'), "Missing VisualizationHelper"
    
    print("✓ Basic imports test passed")

def test_basic_types():
    """Test basic type definitions"""
    print("Testing basic types...")
    
    # Test NeuronId
    neuron_id = pynlm.NeuronId(123)
    assert neuron_id.value == 123, f"Wrong NeuronId value: {neuron_id.value}"
    assert neuron_id.index() == 123, "Wrong NeuronId index"
    
    # Test SynapseId
    synapse_id = pynlm.SynapseId(456)
    assert synapse_id.value == 456, f"Wrong SynapseId value: {synapse_id.value}"
    
    # Test RegionId
    region_id = pynlm.RegionId(789)
    assert region_id.value == 789, f"Wrong RegionId value: {region_id.value}"
    
    # Test enums
    assert pynlm.NeuronType.Excitatory, "Missing NeuronType.Excitatory"
    assert pynlm.NeuronType.Inhibitory, "Missing NeuronType.Inhibitory"
    assert pynlm.NeuronType.Modulatory, "Missing NeuronType.Modulatory"
    assert pynlm.NeuronType.Sensory, "Missing NeuronType.Sensory"
    assert pynlm.NeuronType.Motor, "Missing NeuronType.Motor"
    assert pynlm.NeuronType.Internal, "Missing NeuronType.Internal"
    
    print("✓ Basic types test passed")

def test_config_builder():
    """Test configuration builder"""
    print("Testing ConfigBuilder...")
    
    # Test basic config
    config = pynlm.ConfigBuilder().setBasicConfig().build()
    assert config is not None, "Failed to create basic config"
    
    # Test advanced config
    config = pynlm.ConfigBuilder().setAdvancedConfig().build()
    assert config is not None, "Failed to create advanced config"
    
    # Test exploration config
    config = pynlm.ConfigBuilder().setExplorationConfig().build()
    assert config is not None, "Failed to create exploration config"
    
    # Test production config
    config = pynlm.ConfigBuilder().setProductionConfig().build()
    assert config is not None, "Failed to create production config"
    
    # Test custom config
    config = pynlm.ConfigBuilder() \
        .setCustom("test_param", 42) \
        .setCustom("learning_rate", 0.01) \
        .setCustom("enabled", True) \
        .build()
    assert config is not None, "Failed to create custom config"
    
    print("✓ ConfigBuilder test passed")

def test_factory_functions():
    """Test factory functions"""
    print("Testing factory functions...")
    
    # Test config factory
    config = pynlm.ConfigBuilder().createDefault()
    assert config is not None, "Failed to create default config"
    
    config = pynlm.ConfigBuilder().createAdvanced()
    assert config is not None, "Failed to create advanced config"
    
    config = pynlm.ConfigBuilder().createExploration()
    assert config is not None, "Failed to create exploration config"
    
    # Test brain factory
    config = pynlm.ConfigBuilder().createDefault()
    brain = pynlm.createBrain(config)
    assert brain is not None, "Failed to create brain"
    
    # Test world factory
    world = pynlm.createSimpleWorld()
    assert world is not None, "Failed to create world"
    
    # Test agent factory
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None, "Failed to create agent"
    
    print("✓ Factory functions test passed")

def test_constants():
    """Test constant values"""
    print("Testing constants...")
    
    # Test invalid ID constants
    assert pynlm.INVALID_NEURON_ID.value != 0, "Invalid neuron ID should not be zero"
    assert pynlm.INVALID_SYNAPSE_ID.value != 0, "Invalid synapse ID should not be zero"
    assert pynlm.INVALID_REGION_ID.value != 0, "Invalid region ID should not be zero"
    assert pynlm.INVALID_POPULATION_ID.value != 0, "Invalid population ID should not be zero"
    
    print("✓ Constants test passed")

def test_type_checking():
    """Test type checking functions"""
    print("Testing type checking...")
    
    # Test NeuronId validation
    neuron_id = pynlm.NeuronId(123)
    pynlm.validateNeuronId(neuron_id)  # Should not raise
    
    # Test SynapseId validation
    synapse_id = pynlm.SynapseId(456)
    pynlm.validateSynapseId(synapse_id)  # Should not raise
    
    # Test RegionId validation
    region_id = pynlm.RegionId(789)
    pynlm.validateRegionId(region_id)  # Should not raise
    
    # Test PopulationId validation
    population_id = pynlm.PopulationId(999)
    pynlm.validatePopulationId(population_id)  # Should not raise
    
    print("✓ Type checking test passed")

def test_visualization_helper():
    """Test visualization helper"""
    print("Testing VisualizationHelper...")
    
    viz = pynlm.VisualizationHelper()
    
    # Test string encoding
    text = viz.toUtf8("Hello World")
    assert isinstance(text, str), "toUtf8 should return string"
    
    bytes_data = viz.toBytes("data")
    assert isinstance(bytes_data, str), "toBytes should return string"
    
    # Test encoding/decoding
    encoded = viz.encode("test", "utf-8")
    decoded = viz.decode(encoded, "utf-8")
    assert decoded == "test", "encode/decode should be reversible"
    
    print("✓ VisualizationHelper test passed")

def test_simulation_runner():
    """Test simulation runner"""
    print("Testing SimulationRunner...")
    
    runner = pynlm.SimulationRunner()
    assert runner is not None, "Failed to create SimulationRunner"
    
    print("✓ SimulationRunner test passed")

def main():
    """Run all tests"""
    print("=" * 60)
    print("Enhanced NLM Python Bindings - Basic Tests")
    print("=" * 60)
    print()
    
    tests = [
        test_basic_imports,
        test_basic_types,
        test_config_builder,
        test_factory_functions,
        test_constants,
        test_type_checking,
        test_visualization_helper,
        test_simulation_runner,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ {test.__name__} failed: {e}")
            failed += 1
        print()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("All tests passed! ✓")
        return 0
    else:
        print("Some tests failed. ✗")
        return 1

if __name__ == "__main__":
    sys.exit(main())
