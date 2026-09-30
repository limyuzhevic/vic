#!/usr/bin/env python3
"""
Comprehensive test suite for improved NLM Python bindings.

This test verifies all the improvements made to the Python bindings:
1. Modular structure with clear separation of concerns
2. Enhanced documentation and examples
3. Improved error handling with custom exceptions
4. Convenience methods and factory functions
5. Type safety and validation utilities
6. Advanced APIs for experienced users
"""

import sys
import os
import numpy as np

# Add the Python module to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm as nlm
def test_basic_types():
    """Test basic type definitions and their methods."""
    print("Testing basic types...")
    
    # Test NeuronId
    neuron_id = nlm.NeuronId(123)
    assert neuron_id.value == 123
    assert neuron_id.index() == 123
    assert hash(neuron_id) == hash(123)
    assert neuron_id == nlm.NeuronId(123)
    assert neuron_id != nlm.NeuronId(456)
    
    # Test SynapseId
    synapse_id = nlm.SynapseId(456)
    assert synapse_id.value == 456
    assert synapse_id.index() == 456
    
    # Test RegionId
    region_id = nlm.RegionId(789)
    assert region_id.value == 789
    
    # Test PopulationId
    pop_id = nlm.PopulationId(999)
    assert pop_id.value == 999
    
    # Test enumerations
    assert nlm.NeuronType.Excitatory == 0
    assert nlm.NeuronType.Inhibitory == 1
    assert nlm.NeuronType.Modulatory == 2
    assert nlm.NeuronType.Sensory == 3
    assert nlm.NeuronType.Motor == 4
    assert nlm.NeuronType.Internal == 5
    
    print("✓ Basic types test passed")
def test_enums():
    """Test enumeration values."""
    print("Testing enumerations...")
    
    # Test NeuronType
    for neuron_type in nlm.NeuronType:
        assert isinstance(neuron_type, nlm.NeuronType)
    
    # Test SynapseType
    assert nlm.SynapseType.Excitatory == 0
    assert nlm.SynapseType.Inhibitory == 1
    assert nlm.SynapseType.Modulatory == 2
    assert nlm.SynapseType.Electrical == 3
    assert nlm.SynapseType.GapJunction == 4
    
    # Test DevelopmentalStage
    assert nlm.DevelopmentalStage.Initial == 0
    assert nlm.DevelopmentalStage.CriticalPeriod == 1
    assert nlm.DevelopmentalStage.Maturation == 2
    assert nlm.DevelopmentalStage.Adult == 3
    assert nlm.DevelopmentalStage.Aging == 4
    
    # Test FiringState
    assert nlm.FiringState.Resting == 0
    assert nlm.FiringState.Active == 1
    assert nlm.FiringState.Refractory == 2
    assert nlm.FiringState.Inhibited == 3
    
    print("✓ Enumerations test passed")
def test_config():
    """Test configuration management with enhanced API."""
    print("Testing configuration...")
    
    # Test default config
    config = nlm.Config()
    assert config.has("brain.neuron_count")
    assert config.has("brain.synaptic_weight")
    
    # Test getting values
    neuron_count = config.getOr("brain.neuron_count", 100)
    assert neuron_count == 100
    
    synaptic_weight = config.getOr("brain.synaptic_weight", 0.1)
    assert synaptic_weight == 0.1
    
    # Test setting values with different types
    config.set("test_int", 42)
    config.set("test_float", 3.14)
    config.set("test_bool", True)
    config.set("test_string", "test")
    config.set("test_vector", [1.0, 2.0, 3.0])
    
    # Test getting with type-specific methods
    assert config.getInt("test_int") == 42
    assert config.getDouble("test_float") == 3.14
    assert config.getBool("test_bool") == True
    assert config.getString("test_string") == "test"
    
    # Test getOr with None
    none_value = config.getOr("nonexistent", None)
    assert none_value is None
    
    # Test getOr with custom default
    custom_default = config.getOr("nonexistent", 999)
    assert custom_default == 999
    
    print("✓ Configuration test passed")
def test_factory_functions():
    """Test convenience factory functions."""
    print("Testing factory functions...")
    
    # Test createDefaultConfig
    config = nlm.createDefaultConfig()
    assert config.has("brain.neuron_count")
    assert config.getOr("brain.neuron_count", 0) == 100
    
    # Test createBrain with default config
    brain = nlm.Brain(config)
    assert brain is not None
    
    # Test createSimpleWorld
    world = nlm.createSimpleWorld(100, 100, 10, 10)
    assert world.getWidth() == 100
    assert world.getHeight() == 100
    
    # Test createAgentBrain
    agent_brain = nlm.createAgentBrain(brain)
    assert agent_brain is not None
    
    # Test config from dict
    config_dict = {"brain.neuron_count": 200, "brain.synaptic_weight": 0.2}
    dict_config = nlm.createConfigFromDict(config_dict)
    assert dict_config.getOr("brain.neuron_count", 0) == 200
    assert dict_config.getOr("brain.synaptic_weight", 0.0) == 0.2
    
    print("✓ Factory functions test passed")
def test_error_handling():
    """Test enhanced error handling with custom exceptions."""
    print("Testing error handling...")
    
    # Test invalid configuration
    config = nlm.Config()
    
    # Test getting non-existent key with type-specific method
    try:
        config.getInt("nonexistent")
        assert False, "Should have raised ConfigError"
    except nlm.ConfigError:
        pass  # Expected
    
    # Test type validation
    try:
        config.validateNeuronId(nlm.INVALID_NEURON_ID)
        assert False, "Should have raised NLMError"
    except nlm.NLMError:
        pass  # Expected
    
    print("✓ Error handling test passed")
def test_numpy_integration():
    """Test NumPy integration utilities."""
    print("Testing NumPy integration...")
    
    # Test vision to numpy conversion
    vision = nlm.Vision(10, 10, 3)
    data = list(range(10 * 10 * 3))
    vision.setData(data)
    
    numpy_array = nlm.visionToNumpy(vision)
    assert isinstance(numpy_array, np.ndarray)
    assert numpy_array.shape == (10 * 10 * 3,)
    assert numpy_array.dtype == np.float32
    
    print("✓ NumPy integration test passed")
def test_batch_operations():
    """Test batch operations for improved performance."""
    print("Testing batch operations...")
    
    # Create a simple world
    world = nlm.createSimpleWorld(50, 50, 5, 5)
    
    # Test batch world updates
    timesteps = [0.01, 0.01, 0.01]
    nlm.batchUpdateWorld(world, timesteps)
    
    # Test agent safety check
    body = world.getAgentBody()
    safety = nlm.checkAgentSafety(world, body)
    assert "is_valid_position" in safety
    assert "is_healthy" in safety
    
    print("✓ Batch operations test passed")
def test_sensory_processing():
    """Test enhanced sensory processing."""
    print("Testing sensory processing...")
    
    # Create vision input
    vision = nlm.Vision(10, 10, 3)
    data = [0.1 * (i + 1) for i in range(10 * 10 * 3)]
    vision.setData(data)
    
    # Create sensory percept
    percept = nlm.SensoryPercept()
    percept.setVision(vision)
    
    # Test percept properties
    assert percept.getVisionWidth() == 10
    assert percept.getVisionHeight() == 10
    
    # Test all signals
    all_signals = percept.getAllSignals()
    assert len(all_signals) == 10 * 10 * 3 + 8 + 4 + 6 + 0
    
    # Test sensory stats
    percept_stats = percept.getSignalSummary()
    assert "vision_width" in percept_stats
    assert "vision_height" in percept_stats
    assert "internal_signals" in percept_stats
    
    print("✓ Sensory processing test passed")
def test_advanced_features():
    """Test advanced APIs for experienced users."""
    print("Testing advanced features...")
    
    # Create a brain
    config = nlm.createDefaultConfig()
    brain = nlm.Brain(config)
    
    # Test connectivity matrix
    connectivity = nlm.getConnectivityMatrix(brain)
    assert isinstance(connectivity, np.ndarray)
    
    # Test activity summary
    activity_summary = nlm.getActivitySummary(brain)
    assert "total_neurons" in activity_summary
    assert "total_synapses" in activity_summary
    assert "average_firing_rate" in activity_summary
    
    # Test type validation
    neuron_id = nlm.NeuronId(123)
    nlm.validateNeuronId(neuron_id)  # Should not raise
    
    try:
        nlm.validateNeuronId(nlm.INVALID_NEURON_ID)
        assert False, "Should have raised NLMError"
    except nlm.NLMError:
        pass
    
    # Test range validation
    nlm.validateRange(5.0, 0.0, 10.0, "test_param")  # Should not raise
    
    try:
        nlm.validateRange(15.0, 0.0, 10.0, "test_param")
        assert False, "Should have raised NLMError"
    except nlm.NLMError:
        pass
    
    print("✓ Advanced features test passed")
def test_backward_compatibility():
    """Test backward compatibility with existing API."""
    print("Testing backward compatibility...")
    
    # Test that all original API still works
    config = nlm.Config()
    
    # Original config methods
    config.loadFromFile("nonexistent.json")  # Should fail silently
    config.saveToFile("nonexistent.json")   # Should fail silently
    
    # Test action creation
    action1 = nlm.Action(nlm.ActionType.MoveForward)
    action2 = nlm.Action(nlm.ActionType.Custom, [0.5, 1.0])
    
    # Test action properties
    assert action1.getType() == nlm.ActionType.MoveForward
    assert action2.getType() == nlm.ActionType.Custom
    
    # Test world creation
    world = nlm.SimpleWorld()
    world.configure(100, 100, 10, 10)
    world.setAgentStart(50, 50)
    world.reset()
    
    # Test brain creation
    brain = nlm.Brain(config)
    brain.initialize()
    brain.reset()
    
    print("✓ Backward compatibility test passed")
def run_all_tests():
    """Run all tests and report results."""
    print("=" * 60)
    print("Running comprehensive NLM Python bindings tests")
    print("=" * 60)
    
    tests = [
        test_basic_types,
        test_enums,
        test_config,
        test_factory_functions,
        test_error_handling,
        test_numpy_integration,
        test_batch_operations,
        test_sensory_processing,
        test_advanced_features,
        test_backward_compatibility,
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
    
    return failed == 0
if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)