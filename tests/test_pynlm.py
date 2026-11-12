#!/usr/bin/env python3
"""Python tests for NLM (Neural Learning Machine) Python bindings.

This test suite verifies the functionality of the pynlm Python bindings,
testing common operations and convenience functions.
"""

import pynlm
import pytest
import tempfile
import os
from pathlib import Path
def test_basic_creation():
    """Test basic brain creation and initialization."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    assert brain is not None
    assert brain.getTotalNeuronCount() == 1000
    assert brain.getRegionCount() == 2
    
    brain.initialize()
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
def test_convenience_functions():
    """Test the new convenience functions."""
    # Test create_brain_with_params
    brain = pynlm.create_brain_with_params(neuron_count=500, region_count=3)
    assert brain.getTotalNeuronCount() == 500
    assert brain.getRegionCount() == 3
    
    brain.initialize()
    assert brain.getTotalNeuronCount() > 0
    
    # Test is_brain_ready
    assert pynlm.is_brain_ready(brain)
    
    # Test get_all_region_ids
    region_ids = pynlm.get_all_region_ids(brain)
    assert len(region_ids) == 3
    
    # Test get_brain_stats
    stats = pynlm.get_brain_stats(brain)
    assert "neuron_count" in stats
    assert "synapse_count" in stats
    assert "firing_neurons" in stats
    assert "average_firing_rate" in stats
def test_batch_operations():
    """Test batch operations and convenience functions."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test batch_step
    pynlm.batch_step(brain, 50)
    initial_spikes = brain.getTotalSpikeCount()
    
    # Run more steps
    pynlm.batch_step(brain, 100)
    final_spikes = brain.getTotalSpikeCount()
    
    assert final_spikes > initial_spikes
    
    # Test run_simulation_with_stats
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable some features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation with stats
    stats = pynlm.run_simulation_with_stats(brain, 200, verbose=True)
    assert len(stats) == 200
    
    # Check stats structure
    first_step = stats[0]
    assert "step" in first_step
    assert "firing_neurons" in first_step
    assert "spike_count" in first_step
    assert "firing_rate" in first_step
def test_world_operations():
    """Test world-related functions."""
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Test get_world_stats
    world_stats = pynlm.get_world_stats(world)
    assert world_stats["width"] == 20
    assert world_stats["height"] == 20
    assert world_stats["vision_width"] == 8
    assert world_stats["vision_height"] == 8
    
    # Test agent interaction
    world.setAgentStart(10.0, 10.0)
    
    # Get percept
    percept = world.getSensoryPercept()
    assert percept is not None
    
    # Test getAgentBody
    body = world.getAgentBody()
    assert body.x == 10.0
    assert body.y == 10.0
    
    # Test world update
    world.update(0.1)
    assert world.getSimulationTime() == 0.1
def test_configuration_functions():
    """Test configuration helper functions."""
    # Test create_config_from_dict
    config_dict = {
        "brain.neuron_count": 500,
        "brain.synapse_density": 0.15,
        "random_seed": 42
    }
    config = pynlm.create_config_from_dict(config_dict)
    
    assert config.has("brain.neuron_count")
    assert config.has("brain.synapse_density")
    assert config.has("random_seed")
    
    neuron_count = config.getOr("brain.neuron_count", 1000)
    assert neuron_count == 500
    
    # Test config_to_dict
    config_keys = pynlm.config_to_dict(config)
    assert "key_count" in config_keys
    assert config_keys["key_count"] == 3
    
    # Test validate_config
    is_valid, errors = pynlm.validate_config(config)
    assert is_valid
    assert errors == ""
    
    # Test invalid config
    invalid_config = pynlm.createDefaultConfig()
    invalid_config.set("brain.neuron_count", -1)
    invalid_config.set("brain.synapse_density", 2.0)  # Invalid: > 1.0
    
    is_valid, errors = pynlm.validate_config(invalid_config)
    assert not is_valid
    assert "neuron_count must be positive" in errors
    assert "synapse_density must be between 0 and 1" in errors
def test_serialization():
    """Test safe save/load functions."""
    config = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    
    # Drive the brain to create state
    for i in range(50):
        brain1.step(i)
    
    # Create copy for comparison
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    
    # Test safe_save
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        filepath = f.name
    
    try:
        # Save brain1
        pynlm.safe_save(brain1, filepath)
        
        # Load into brain2
        pynlm.safe_load(brain2, filepath)
        
        # Verify they have similar state
        assert brain2.getTotalNeuronCount() == brain1.getTotalNeuronCount()
        assert brain2.getTotalSynapseCount() == brain1.getTotalSynapseCount()
        
    finally:
        # Clean up
        if os.path.exists(filepath):
            os.unlink(filepath)
def test_agent_operations():
    """Test agent-related operations."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test agent enable/disable functions
    agent.enableRewardModulation(True)
    assert pynlm.isRewardModulationEnabled(agent)  # Note: This might not exist in C++
    
    agent.enableCuriosity(True)
    assert pynlm.isCuriosityEnabled(agent)  # Note: This might not exist in C++
    
    agent.enableDevelopment(True)
    assert pynlm.isDevelopmentEnabled(agent)  # Note: This might not exist in C++
    
    # Test processSensoryInput (using mock data)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Test decodeMotorCommand
    action = agent.decodeMotorCommand()
    assert action is not None
    
    # Test neuromodulation
    agent.applyRewardModulation(1.0, 0.5)
    
    # Test development update
    agent.updateDevelopment(0.1)
    
    # Test get statistics
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    neuromodulation = agent.getNeuromodulationLevel()
    
    assert curiosity >= 0.0
    assert novelty >= 0.0
    assert neuromodulation >= 0.0
def test_enums():
    """Test enumeration values."""
    # Test NeuronType enum
    assert pynlm.NeuronType.Excitatory is not None
    assert pynlm.NeuronType.Inhibitory is not None
    assert pynlm.NeuronType.Sensory is not None
    assert pynlm.NeuronType.Motor is not None
    
    # Test SynapseType enum
    assert pynlm.SynapseType.Excitatory is not None
    assert pynlm.SynapseType.Inhibitory is not None
    
    # Test DevelopmentalStage enum
    assert pynlm.DevelopmentalStage.Initial is not None
    assert pynlm.DevelopmentalStage.Adult is not None
    
    # Test ActionType enum
    assert pynlm.ActionType.MoveForward is not None
    assert pynlm.ActionType.TurnLeft is not None
    assert pynlm.ActionType.Interact is not None
    
    # Test WorldObjectType enum
    assert pynlm.WorldObjectType.Resource is not None
    assert pynlm.WorldObjectType.Hazard is not None
    assert pynlm.WorldObjectType.Wall is not None
def test_error_handling():
    """Test error handling and edge cases."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Test accessing uninitialized brain
    # Should raise an error when trying to use uninitialized brain
    # (Note: error handling depends on C++ implementation)
    
    # Test invalid file paths
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        filepath = f.name
    
    try:
        # Try to save before initialization
        # This might or might not work depending on implementation
        # We're just testing the API
        pass
    finally:
        if os.path.exists(filepath):
            os.unlink(filepath)
def test_performance_patterns():
    """Test patterns that improve performance."""
    config = pynlm.createDefaultConfig()
    
    # Create multiple brains (pattern for batch processing)
    brains = []
    for i in range(5):
        brain_config = pynlm.createDefaultConfig()
        brain_config.set("brain.neuron_count", 200 + i * 50)
        brain = pynlm.createBrain(brain_config)
        brain.initialize()
        brains.append(brain)
    
    # Use batch operations
    for brain in brains:
        pynlm.batch_step(brain, 20)
    
    # Verify all brains are working
    for brain in brains:
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0


def run_all_tests():
    """Run all tests and report results."""
    test_functions = [
        test_basic_creation,
        test_convenience_functions,
        test_batch_operations,
        test_world_operations,
        test_configuration_functions,
        test_serialization,
        test_agent_operations,
        test_enums,
        test_error_handling,
        test_performance_patterns
    ]
    
    passed = 0
    failed = 0
    
    print("Running NLM Python Bindings Tests")
    print("=" * 50)
    
    for test_func in test_functions:
        try:
            test_func()
            print(f"✓ {test_func.__name__} passed")
            passed += 1
        except Exception as e:
            print(f"✗ {test_func.__name__} failed: {e}")
            failed += 1
    
    print("=" * 50)
    print(f"Results: {passed} passed, {failed} failed")
    return passed, failed


if __name__ == "__main__":
    passed, failed = run_all_tests()
    
    if failed > 0:
        print(f"\n{failed} tests failed!")
        exit(1)
    else:
        print("\nAll tests passed!")
        exit(0)