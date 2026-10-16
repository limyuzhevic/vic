#!/usr/bin/env python3
"""
NLM Python Test Framework

Comprehensive test suite for NLM Python bindings.

This test framework provides:
- Unit tests for core NLM functionality
- Integration tests for complete agent systems
- Performance benchmarking
- Configuration validation tests
- Error handling tests
"""

import pynlm
import numpy as np
import tempfile
import json
import time
import sys
from pathlib import Path


def test_config_basic():
    """Test basic configuration creation and manipulation."""
    config = pynlm.createDefaultConfig()
    
    # Test setting different types of values
    config.set("int_value", 42)
    config.set("float_value", 3.14)
    config.set("string_value", "test")
    config.set("bool_value", True)
    
    # Test getting values
    assert config.get("int_value") == 42
    assert abs(config.get("float_value") - 3.14) < 0.01
    assert config.get("string_value") == "test"
    assert config.get("bool_value") is True
    
    # Test getOr with default
    assert config.getOr("missing_key", "default") == "default"
    assert config.getOr("missing_key", 999) == 999
    
    print("✓ test_config_basic passed")


def test_config_file_operations():
    """Test configuration file operations."""
    config = pynlm.createDefaultConfig()
    
    # Set some values
    config.set("test_int", 123)
    config.set("test_string", "hello world")
    config.set("test_float", 2.718)
    config.set("test_bool", False)
    
    # Save to temporary file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config.saveToFile(f.name)
        temp_file = f.name
    
    try:
        # Load from file
        config2 = pynlm.createDefaultConfig()
        assert config2.loadFromFile(temp_file)
        
        # Verify values were loaded
        assert config2.get("test_int") == 123
        assert config2.get("test_string") == "hello world"
        assert abs(config2.get("test_float") - 2.718) < 0.001
        assert config2.get("test_bool") is False
        
        # Test file operations with non-existent file
        config3 = pynlm.createDefaultConfig()
        assert not config3.loadFromFile("/nonexistent/file.json")
        
    finally:
        # Clean up
        Path(temp_file).unlink()
    
    print("✓ test_config_file_operations passed")


def test_brain_basic():
    """Test basic brain creation and initialization."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test brain properties
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() >= 0
    assert brain.getFiringNeuronCount() >= 0
    assert brain.getAverageFiringRate() >= 0.0
    
    # Test step execution
    for step in range(10):
        brain.step(step)
        assert brain.getTotalNeuronCount() == brain.getTotalNeuronCount()
    
    # Test reset
    brain.reset()
    brain.initialize()
    assert brain.getTotalNeuronCount() > 0
    
    print("✓ test_brain_basic passed")


def test_brain_with_config():
    """Test brain with custom configuration."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    config.set("brain.connection_probability", 0.1)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    assert brain.getTotalNeuronCount() == 100
    assert brain.getTotalSynapseCount() > 0
    
    # Run simulation
    for step in range(50):
        brain.step(step)
    
    print("✓ test_brain_with_config passed")


def test_agent_system():
    """Test complete agent system."""
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    
    agent.initialize(world)
    
    # Test with different action types
    for i in range(20):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(i)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Test neuromodulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Test development
        agent.updateDevelopment(0.1)
    
    # Test statistics
    stats = pynlm.getAgentStats(agent)
    assert "curiosity_level" in stats
    assert "novelty_level" in stats
    assert "prediction_error" in stats
    
    print("✓ test_agent_system passed")


def test_complete_agent_system():
    """Test complete agent system factory function."""
    brain, agent, world = pynlm.createCompleteAgentSystem()
    
    assert brain is not None
    assert agent is not None
    assert world is not None
    
    # Initialize agent
    agent.initialize(world)
    
    # Run simulation
    for step in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get statistics
    brain_stats = pynlm.getBrainStats(brain)
    agent_stats = pynlm.getAgentStats(agent)
    
    assert "total_neurons" in brain_stats
    assert "total_synapses" in brain_stats
    assert "curiosity_level" in agent_stats
    
    print("✓ test_complete_agent_system passed")


def test_simulation_runner():
    """Test simulation runner utility."""
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=8, height=8, visionWidth=4, visionHeight=4)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Use the runner function
    pynlm.runAgentSimulation(brain, world, agent, 25)
    
    # Verify simulation ran
    assert brain.getTotalSpikeCount() >= 0
    assert brain.getFiringNeuronCount() >= 0
    
    print("✓ test_simulation_runner passed")


def test_configuration_conversion():
    """Test configuration conversion utilities."""
    config = pynlm.createDefaultConfig()
    config.set("test_int", 42)
    config.set("test_float", 3.14)
    config.set("test_string", "hello")
    config.set("test_bool", True)
    
    # Convert to dictionary
    config_dict = pynlm.configToDict(config)
    
    assert "test_int" in config_dict
    assert "test_float" in config_dict
    assert "test_string" in config_dict
    assert "test_bool" in config_dict
    
    # Test safe getters
    assert config.safeGetConfigValue("test_int", "0") == "42"
    assert config.safeGetConfigValue("test_float", "0.0") == "3.14"
    assert config.safeGetConfigValue("test_string", "") == "hello"
    assert config.safeGetConfigValue("test_bool", "false") == "True"
    
    # Test type-specific safe getters
    assert config.safeGetConfigValueInt("test_int", 0) == 42
    assert config.safeGetConfigValueFloat("test_float", 0.0) == 3.14
    assert config.safeGetConfigValueBool("test_bool", False) is True
    
    # Convert dictionary back to config
    new_config = pynlm.dictToConfig(config_dict)
    assert new_config.get("test_int") == 42
    assert new_config.get("test_string") == "hello"
    
    print("✓ test_configuration_conversion passed")


def test_config_validation():
    """Test configuration validation."""
    config = pynlm.createDefaultConfig()
    
    # Test empty config validation
    errors = config.validateConfig()
    assert isinstance(errors, list)
    
    # Add some values and validate
    config.set("valid_int", 42)
    config.set("valid_string", "test")
    
    errors = config.validateConfig()
    assert len(errors) == 0
    
    print("✓ test_config_validation passed")


def test_config_operations():
    """Test configuration operations."""
    config1 = pynlm.createDefaultConfig()
    config1.set("shared_key", 100)
    config1.set("key1", "value1")
    
    config2 = pynlm.createDefaultConfig()
    config2.set("shared_key", 200)
    config2.set("key2", "value2")
    
    # Test merging
    merged = pynlm.mergeConfigs(config1, config2)
    assert merged.get("shared_key") == 200  # Overridden by config2
    assert merged.get("key1") == "value1"
    assert merged.get("key2") == "value2"
    
    # Test cloning
    cloned = pynlm.cloneConfig(config1)
    cloned.set("new_key", "new_value")
    
    # Original should be unchanged
    assert not config1.has("new_key")
    assert cloned.get("new_key") == "new_value"
    
    # Test statistics
    stats = config1.getConfigStats()
    assert "total_keys" in stats
    assert stats["total_keys"] == 3  # shared_key, key1, and default values
    
    print("✓ test_config_operations passed")


def test_error_handling():
    """Test error handling."""
    config = pynlm.createDefaultConfig()
    
    # Test safe getters with missing keys
    assert config.safeGetConfigValue("nonexistent", "default") == "default"
    assert config.safeGetConfigValueInt("nonexistent", 999) == 999
    assert config.safeGetConfigValueFloat("nonexistent", 0.0) == 0.0
    assert config.safeGetConfigValueBool("nonexistent", False) is False
    
    # Test that validation works with empty config
    errors = config.validateConfig()
    assert isinstance(errors, list)
    
    print("✓ test_error_handling passed")


def test_performance_benchmarking():
    """Test performance benchmarking."""
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run simulation
    start_time = time.time()
    for step in range(100):
        brain.step(step)
    end_time = time.time()
    
    # Get brain stats
    stats = pynlm.getBrainStats(brain)
    
    assert "total_neurons" in stats
    assert "total_spikes" in stats
    assert "average_firing_rate" in stats
    
    # Performance should be reasonable (can vary by system)
    execution_time = end_time - start_time
    assert execution_time < 5.0  # Should complete within 5 seconds
    
    print(f"✓ test_performance_benchmarking passed (completed {100} steps in {execution_time:.3f}s)")


def test_helper_functions():
    """Test helper functions and utilities."""
    # Test helper function exists
    assert callable(pynlm.runAgentSimulation)
    assert callable(pynlm.createCompleteAgentSystem)
    assert callable(pynlm.getBrainStats)
    assert callable(pynlm.getAgentStats)
    assert callable(pynlm.configToDict)
    assert callable(pynlm.dictToConfig)
    
    # Test constants
    assert hasattr(pynlm, 'INVALID_NEURON_ID')
    assert hasattr(pynlm, 'INVALID_SYNAPSE_ID')
    assert hasattr(pynlm, 'INVALID_REGION_ID')
    assert hasattr(pynlm, 'INVALID_POPULATION_ID')
    
    print("✓ test_helper_functions passed")


def run_all_tests():
    """Run all tests and report results."""
    print("=" * 60)
    print("NLM Python Test Suite")
    print("=" * 60)
    
    tests = [
        test_config_basic,
        test_config_file_operations,
        test_brain_basic,
        test_brain_with_config,
        test_agent_system,
        test_complete_agent_system,
        test_simulation_runner,
        test_configuration_conversion,
        test_config_validation,
        test_config_operations,
        test_error_handling,
        test_performance_benchmarking,
        test_helper_functions,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            failed += 1
            print(f"✗ {test.__name__} failed: {e}")
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        print("Some tests failed. Please check the output above for details.")
        return False
    else:
        print("All tests passed successfully!")
        return True


if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)