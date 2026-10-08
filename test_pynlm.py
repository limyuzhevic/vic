#!/usr/bin/env python3
"""
Python Test Suite for NLM (Neural Learning Machine)

Comprehensive tests for the pynlm Python bindings and neural simulation framework.
"""

import sys
import os
import tempfile
import time
import json
from pathlib import Path
import numpy as np

# Add the Python module path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python', 'build'))

import pynlm as nlm

def test_import():
    """Test that the pynlm module can be imported and basic classes exist."""
    print("Testing pynlm import...")
    
    # Test basic classes
    assert hasattr(nlm, 'createDefaultConfig'), "createDefaultConfig function missing"
    assert hasattr(nlm, 'createBrain'), "createBrain function missing"
    assert hasattr(nlm, 'createSimpleWorld'), "createSimpleWorld function missing"
    assert hasattr(nlm, 'createAgentBrain'), "createAgentBrain function missing"
    
    # Test enums
    assert hasattr(nlm, 'LogLevel'), "LogLevel enum missing"
    assert hasattr(nlm, 'NeuronType'), "NeuronType enum missing"
    assert hasattr(nlm, 'ActionType'), "ActionType enum missing"
    
    print("  ✓ Import test passed")

def test_config():
    """Test configuration functionality."""
    print("Testing configuration...")
    
    # Test default config creation
    config = nlm.createDefaultConfig()
    assert config is not None, "Failed to create default config"
    
    # Test setting values
    config.set("test_int", 42)
    assert config.has("test_int"), "Failed to set int value"
    
    config.set("test_double", 3.14)
    assert config.has("test_double"), "Failed to set double value"
    
    config.set("test_string", "hello")
    assert config.has("test_string"), "Failed to set string value"
    
    config.set("test_bool", True)
    assert config.has("test_bool"), "Failed to set bool value"
    
    # Test getting values
    assert config.getOr("test_int", 0) == 42, "Failed to get int value"
    assert config.getOr("test_double", 0.0) > 3.13, "Failed to get double value"
    assert config.getOr("test_string", "") == "hello", "Failed to get string value"
    assert config.getOr("test_bool", False), "Failed to get bool value"
    
    # Test missing keys
    assert not config.has("missing_key"), "Should not have missing key"
    assert config.getOr("missing_key", 100) == 100, "Should return default for missing key"
    
    # Test keys
    keys = config.getKeys()
    assert "test_int" in keys, "test_int should be in keys"
    assert "test_double" in keys, "test_double should be in keys"
    assert "test_string" in keys, "test_string should be in keys"
    assert "test_bool" in keys, "test_bool should be in keys"
    
    # Test summary
    summary = config.summary()
    assert isinstance(summary, str), "Summary should return string"
    assert "test_int" in summary, "Summary should contain test_int"
    
    print("  ✓ Configuration test passed")

def test_config_file():
    """Test configuration file loading and saving."""
    print("Testing configuration file I/O...")
    
    # Create a temporary config file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        f.write('test_int = 123\n')
        f.write('test_string = "hello world"\n')
        f.write('test_double = 2.718\n')
        f.write('test_bool = true\n')
        config_file = f.name
    
    try:
        config = nlm.createDefaultConfig()
        
        # Load from file
        success = config.loadFromFile(config_file)
        assert success, "Failed to load config from file"
        
        # Verify loaded values
        assert config.getOr("test_int", 0) == 123, "Failed to load int from file"
        assert config.getOr("test_string", "") == "hello world", "Failed to load string from file"
        assert abs(config.getOr("test_double", 0.0) - 2.718) < 0.001, "Failed to load double from file"
        assert config.getOr("test_bool", False), "Failed to load bool from file"
        
    finally:
        os.unlink(config_file)
    
    print("  ✓ Configuration file I/O test passed")

def test_brain_creation():
    """Test brain creation and basic functionality."""
    print("Testing brain creation...")
    
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    
    # Create brain
    brain = nlm.createBrain(config)
    assert brain is not None, "Failed to create brain"
    
    # Test initialization
    brain.initialize()
    
    # Test brain properties
    assert brain.getTotalNeuronCount() == 100, "Wrong neuron count"
    assert brain.getTotalRegionCount() == 1, "Wrong region count"
    
    # Test stepping
    brain.step(0, 0.001)
    assert brain.getTotalSpikeCount() >= 0, "Should have recorded spikes"
    
    # Test reset
    brain.reset()
    brain.initialize()
    
    print("  ✓ Brain creation test passed")

def test_world():
    """Test world functionality."""
    print("Testing world functionality...")
    
    # Create world
    world = nlm.createSimpleWorld(20, 20)
    assert world is not None, "Failed to create world"
    
    # Set agent start position
    world.setAgentStart(10, 10)
    
    # Create and set body
    body = nlm.AgentBody()
    body.x = 10
    body.y = 10
    body.orientation = 0.0
    
    # Get percept
    percept = world.getSensoryPercept()
    assert percept is not None, "Should have percept"
    
    # Test world boundaries
    assert world.isValidPosition(5, 5), "Should be valid position"
    assert not world.isValidPosition(-1, 5), "Should be invalid position (out of bounds)"
    assert not world.isValidPosition(25, 5), "Should be invalid position (out of bounds)"
    
    print("  ✓ World functionality test passed")

def test_agent_brain():
    """Test agent brain functionality."""
    print("Testing agent brain...")
    
    # Create brain and world
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    world = nlm.createSimpleWorld(20, 20)
    
    // Create agent brain
    agentBrain = nlm.createAgentBrain(brain)
    assert agentBrain is not None, "Failed to create agent brain"
    
    // Initialize agent brain
    agentBrain.initialize(world)
    
    // Test processing sensory input
    percept = world.getSensoryPercept()
    agentBrain.processSensoryInput(percept)
    
    // Test decoding motor command
    action = agentBrain.decodeMotorCommand()
    assert action is not None, "Should decode motor command"
    
    // Test reward modulation
    agentBrain.applyRewardModulation(1.0, 0.5)
    
    // Test development
    agentBrain.updateDevelopment(0.001)
    
    print("  ✓ Agent brain test passed")

def test_serialization():
    """Test brain state serialization."""
    print("Testing serialization...")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    // Add some activity to the brain
    for i in range(10):
        brain.step(i, i * 0.001)
    
    // Create temporary files
    brain_file = tempfile.mktemp(suffix='.nlm')
    
    try:
        // Save brain state
        brain.save(brain_file)
        assert os.path.exists(brain_file), "Brain file should exist after save"
        
        // Create new brain and load
        newConfig = nlm.createDefaultConfig()
        newBrain = nlm.createBrain(newConfig)
        newBrain.load(brain_file)
        
        // Verify spike count
        assert newBrain.getTotalSpikeCount() == brain.getTotalSpikeCount(), \
            "Spike count should match after save/load"
        
    finally:
        if os.path.exists(brain_file):
            os.unlink(brain_file)
    
    print("  ✓ Serialization test passed")

def test_advanced_utilities():
    """Test advanced Python utilities."""
    print("Testing advanced utilities...")
    
    // Create advanced config
    config = nlm.createAdvancedConfig()
    
    // Verify advanced config settings
    assert config.getOr("simulation_timestep", 0.001) == 0.0001, "Wrong timestep in advanced config"
    assert config.getOr("neuron_count", 100) == 5000, "Wrong neuron count in advanced config"
    
    // Create simple brain
    brain = nlm.createSimpleBrain(config)
    assert brain is not None, "Failed to create simple brain"
    
    // Create agent world
    world = nlm.createAgentWorld(30, 30)
    assert world is not None, "Failed to create agent world"
    
    // Run learning experiment
    initial_spikes = brain.getTotalSpikeCount()
    final_spikes = nlm.runLearningExperiment(brain, 100)
    assert final_spikes >= initial_spikes, "Should have increased or maintained spike count"
    
    print("  ✓ Advanced utilities test passed")

def test_statistics():
    """Test brain statistics."""
    print("Testing statistics...")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    // Run some steps
    for i in range(50):
        brain.step(i, i * 0.001)
    
    // Get statistics
    stats = nlm.getBrainStatistics(brain)
    
    // Verify statistics
    assert "total_spikes" in stats, "Should have total_spikes"
    assert "active_neurons" in stats, "Should have active_neurons"
    assert "firing_neurons" in stats, "Should have firing_neurons"
    assert "average_firing_rate" in stats, "Should have average_firing_rate"
    assert "total_neurons" in stats, "Should have total_neurons"
    assert "total_synapses" in stats, "Should have total_synapses"
    
    // Verify types
    assert isinstance(stats["total_spikes"], (int, float)), "total_spikes should be numeric"
    assert isinstance(stats["active_neurons"], (int, float)), "active_neurons should be numeric"
    assert isinstance(stats["firing_neurons"], (int, float)), "firing_neurons should be numeric"
    assert isinstance(stats["average_firing_rate"], (int, float)), "average_firing_rate should be numeric"
    
    print("  ✓ Statistics test passed")

def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Test Suite")
    print("=" * 60)
    
    tests = [
        test_import,
        test_config,
        test_config_file,
        test_brain_creation,
        test_world,
        test_agent_brain,
        test_serialization,
        test_advanced_utilities,
        test_statistics,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"  ✗ {test.__name__} failed: {e}")
            failed += 1
        print()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        return False
    return True

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
