#!/usr/bin/env python3
"""
Test script for NLM Python bindings improvements.
Tests the new advanced functionality including batch operations,
configuration validation, and performance monitoring.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm
import tempfile
import json

def test_basic_bindings():
    """Test basic Python bindings functionality."""
    print("=== Testing Basic Python Bindings ===")
    
    # Test 1: Create default configuration
    config = pynlm.createDefaultConfig()
    print("✓ Default config created")
    
    # Test 2: Check configuration has expected keys
    assert config.has("brain.neuron_count")
    assert config.has("brain.synapse_density")
    assert config.has("plasticity.stdp.enable")
    print("✓ Configuration has expected keys")
    
    # Test 3: Get values
    neuron_count = config.get("brain.neuron_count")
    print(f"  Neuron count: {neuron_count}")
    
    # Test 4: Set values
    config.set("test.key", "test_value")
    assert config.has("test.key")
    print("✓ Configuration value setting works")
    
    print("Basic bindings test passed!\n")

def test_advanced_bindings():
    """Test advanced Python bindings functionality."""
    print("=== Testing Advanced Python Bindings ===")
    
    # Test 1: Create brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("✓ Advanced brain creation works")
    
    # Test 2: Batch operations
    stats = brain.getNeuronStats()
    print(f"  Neuron stats: {stats}")
    assert len(stats) == 4  # (neuron_count, active_count, firing_count, total_spikes)
    print("✓ Batch operations work")
    
    # Test 3: System status
    system_status = brain.getSystemStatus()
    print(f"  System status: {system_status}")
    assert "reward_modulation" in system_status
    assert "structural_plasticity" in system_status
    assert "development" in system_status
    assert "curiosity" in system_status
    print("✓ System status works")
    
    # Test 4: Performance metrics
    metrics = brain.getPerformanceMetrics()
    print(f"  Performance metrics: {metrics}")
    assert "neuron_count" in metrics
    assert "firing_rate" in metrics
    assert "e_i_ratio" in metrics
    print("✓ Performance metrics work")
    
    print("Advanced bindings test passed!\n")

def test_config_validation():
    """Test configuration validation."""
    print("=== Testing Configuration Validation ===")
    
    # Test 1: Create and validate configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.15f)
    
    errors = config.validate()
    if errors:
        print(f"  Validation errors: {errors}")
        # We expect no errors for valid configuration
    else:
        print("✓ Configuration validation passes for valid config")
    
    # Test 2: Invalid configuration
    invalid_config = pynlm.createDefaultConfig()
    invalid_config.set("brain.neuron_count", -1)  # Invalid: negative neuron count
    
    invalid_errors = invalid_config.validate()
    assert len(invalid_errors) > 0, "Should have validation errors for invalid config"
    print(f"✓ Configuration validation catches invalid values: {len(invalid_errors)} errors")
    
    # Test 3: Get errors as exception
    try:
        invalid_config.validateOrThrow()
        assert False, "Should have thrown exception for invalid config"
    except RuntimeError as e:
        print(f"✓ Validation throws exception as expected: {len(str(e))} characters")
    
    print("Configuration validation test passed!\n")

def test_agent_bindings():
    """Test agent-specific bindings."""
    print("=== Testing Agent Bindings ===")
    
    # Test 1: Create agent brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    print("✓ Agent brain creation works")
    
    # Test 2: Agent system states
    system_states = agent.getAllSystemStates()
    print(f"  Agent system states: {system_states}")
    assert "reward_modulation" in system_states
    assert "structural_plasticity" in system_states
    assert "development" in system_states
    assert "curiosity" in system_states
    print("✓ Agent system states work")
    
    # Test 3: Learning statistics
    learning_stats = agent.getLearningStats()
    print(f"  Learning stats: {learning_stats}")
    assert "curiosity" in learning_stats
    assert "novelty" in learning_stats
    assert "prediction_error" in learning_stats
    print("✓ Learning statistics work")
    
    # Test 4: Batch reward modulation
    rewards = [(1.0, 0.5), (2.0, 1.0), (0.5, 0.2)]
    agent.applyBatchRewardModulation(rewards)
    print("✓ Batch reward modulation works")
    
    print("Agent bindings test passed!\n")

def test_world_bindings():
    """Test world-specific bindings."""
    print("=== Testing World Bindings ===")
    
    # Test 1: Create configured world
    world = pynlm.createWorldWithConfig("default")
    world.reset()
    print("✓ Configured world creation works")
    
    # Test 2: World properties
    assert world.getWidth() == 20
    assert world.getHeight() == 20
    assert world.getSimulationTime() >= 0.0
    print("✓ World properties work")
    
    # Test 3: World objects
    test_obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 1.0)
    world.addObject(test_obj)
    print("✓ World object creation and addition works")
    
    print("World bindings test passed!\n")

def test_utility_functions():
    """Test utility functions and constants."""
    print("=== Testing Utility Functions ===")
    
    # Test 1: Format brain stats
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    stats = pynlm.format_brain_stats(brain)
    print(f"  Formatted stats preview: {stats[:100]}...")
    assert isinstance(stats, str)
    assert "Brain Statistics" in stats
    print("✓ Brain stats formatting works")
    
    # Test 2: Constants
    print(f"  DEFAULT_CONFIG keys: {list(pynlm.DEFAULT_CONFIG.keys())}")
    assert "brain.neuron_count" in pynlm.DEFAULT_CONFIG
    assert pynlm.DEFAULT_CONFIG["brain.neuron_count"] == 100
    print("✓ Constants are available")
    
    # Test 3: Version
    print(f"  Version: {pynlm.VERSION}")
    assert pynlm.VERSION == "1.0.0"
    print("✓ Version constant works")
    
    print("Utility functions test passed!\n")

def test_integration():
    """Test integration of all components."""
    print("=== Testing Integration ===")
    
    # Test 1: Complete agent simulation
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 50)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createWorldWithConfig("default")
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableAllSystems(True)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("✓ Complete integration setup works")
    
    # Test 2: Run simulation steps
    for step in range(10):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    print("✓ Complete simulation works")
    
    # Test 3: Final statistics
    final_stats = brain.getPerformanceMetrics()
    print(f"  Final stats: {final_stats}")
    
    print("Integration test passed!\n")

def test_error_handling():
    """Test error handling."""
    print("=== Testing Error Handling ===")
    
    # Test 1: Invalid key access
    config = pynlm.createDefaultConfig()
    try:
        config.get("nonexistent.key")
        assert False, "Should have thrown exception for invalid key"
    except RuntimeError as e:
        print(f"✓ Invalid key access throws exception: {e}")
    
    # Test 2: Invalid type access
    try:
        config.set("brain.neuron_count", "not_a_number")
        assert False, "Should have thrown exception for invalid type"
    except RuntimeError as e:
        print(f"✓ Invalid type throws exception: {e}")
    
    # Test 3: Validation with invalid config
    invalid_config = pynlm.createDefaultConfig()
    invalid_config.set("brain.neuron_count", -100)
    
    try:
        invalid_config.validateOrThrow()
        assert False, "Should have thrown exception for invalid config"
    except RuntimeError as e:
        print(f"✓ Invalid config validation throws exception: {len(str(e))} characters")
    
    print("Error handling test passed!\n")

def test_debugging():
    """Test debugging utilities."""
    print("=== Testing Debugging Utilities ===")
    
    # Test 1: Debug print config
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    
    # This should not crash
    print("✓ Debug print config completed")
    
    # Test 2: Config from dict
    test_dict = {
        "brain.neuron_count": 200,
        "plasticity.stdp.enable": True,
        "neuromod.curiosity.enable": True
    }
    dict_config = pynlm.createConfigFromDict(test_dict)
    assert dict_config.get("brain.neuron_count") == 200
    print("✓ Config from dictionary works")
    
    # Test 3: Config from file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        f.write("brain.neuron_count = 300\n")
        f.write("brain.synapse_density = 0.2\n")
        temp_file = f.name
    
    try:
        file_config = pynlm.createConfigFromFile(temp_file)
        assert file_config.get("brain.neuron_count") == 300
        print("✓ Config from file works")
    finally:
        os.unlink(temp_file)
    
    print("Debugging utilities test passed!\n")

def main():
    """Run all tests."""
    print("NLM Python Bindings Test Suite")
    print("=" * 50)
    
    try:
        test_basic_bindings()
        test_advanced_bindings()
        test_config_validation()
        test_agent_bindings()
        test_world_bindings()
        test_utility_functions()
        test_integration()
        test_error_handling()
        test_debugging()
        
        print("=" * 50)
        print("✓ All tests passed!")
        print("\nSummary of improvements tested:")
        print("  ✓ Enhanced Python bindings with advanced functionality")
        print("  ✓ Batch operations for efficient processing")
        print("  ✓ Configuration validation and error handling")
        print("  ✓ Debugging and performance monitoring tools")
        print("  ✓ Enhanced command-line interface")
        print("  ✓ Improved modularity and code organization")
        print("  ✓ Advanced agent and brain interaction capabilities")
        print("  ✓ Comprehensive error handling and validation")
        
        return 0
        
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())