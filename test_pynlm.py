#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings.
Tests all new functionality including convenience methods, type aliases, 
context managers, and advanced features.
"""

import pynlm
import tempfile
import os
import sys
import json
import time
from pathlib import Path

def test_basic_imports():
    """Test that all basic types and functions are available."""
    print("Testing basic imports...")
    
    # Test type aliases
    assert hasattr(pynlm, 'NeuronId')
    assert hasattr(pynlm, 'SynapseId')
    assert hasattr(pynlm, 'RegionId')
    assert hasattr(pynlm, 'PopulationId')
    assert hasattr(pynlm, 'action_type')
    assert hasattr(pynlm, 'motor_command')
    
    # Test enums
    assert hasattr(pynlm, 'NeuronType')
    assert hasattr(pynlm, 'SynapseType')
    assert hasattr(pynlm, 'ActionType')
    assert hasattr(pynlm, 'MotorCommand')
    assert hasattr(pynlm, 'WorldObjectType')
    assert hasattr(pynlm, 'DevelopmentalStage')
    
    # Test factory functions
    assert hasattr(pynlm, 'createBrain')
    assert hasattr(pynlm, 'createDefaultConfig')
    assert hasattr(pynlm, 'createSimpleWorld')
    assert hasattr(pynlm, 'createAgentBrain')
    
    # Test convenience functions
    assert hasattr(pynlm, 'run_simulation')
    assert hasattr(pynlm, 'train_agent')
    assert hasattr(pynlm, 'create_agent_config')
    assert hasattr(pynlm, 'create_rendering_config')
    
    # Test utility functions
    assert hasattr(pynlm, 'save_checkpoint')
    assert hasattr(pynlm, 'load_checkpoint')
    assert hasattr(pynlm, 'benchmark_brain')
    assert hasattr(pynlm, 'profile_simulation')
    
    print("✓ Basic imports test passed")

def test_config_types_and_methods():
    """Test configuration class with new type safety and convenience methods."""
    print("Testing config types and methods...")
    
    config = pynlm.createDefaultConfig()
    
    # Test basic methods still work
    config.saveToFile("test_config.json")
    
    # Load with new config class
    config2 = pynlm.createDefaultConfig()
    config2.loadFromFile("test_config.json")
    
    # Test new convenience methods
    config.set("brain.neuron_count", 1500)
    assert config.get("brain.neuron_count") == 1500
    
    assert config.get_int("brain.neuron_count") == 1500
    assert config.get_float("plasticity.stdp.learning_rate", 0.001) == 0.001
    assert config.get_string("some.key", "default") == "default"
    assert config.get_bool("some.bool.key", False) == False
    
    # Test has method
    assert config.has("brain.neuron_count")
    assert not config.has("nonexistent.key")
    
    # Test summary
    summary = config.summary()
    assert "Configuration" in summary
    
    # Test repr
    repr_str = repr(config)
    assert "Config:" in repr_str
    
    os.remove("test_config.json")
    print("✓ Config types and methods test passed")

def test_agent_config_presets():
    """Test agent configuration presets."""
    print("Testing agent config presets...")
    
    agent_config = pynlm.create_agent_config()
    
    # Verify agent-specific settings
    assert agent_config.get("brain.neuron_count") == 2000
    assert agent_config.get_float("brain.synapse_density") == 0.15
    assert agent_config.get_bool("plasticity.stdp.enable") == True
    assert agent_config.get_bool("neuromod.curiosity.enable") == True
    
    rendering_config = pynlm.create_rendering_config()
    assert rendering_config.get("brain.neuron_count") == 500
    assert rendering_config.get_bool("visualization.enabled") == True
    
    print("✓ Agent config presets test passed")

def test_brain_creation_and_initialization():
    """Test brain creation and initialization with new utility functions."""
    print("Testing brain creation and initialization...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test basic brain operations
    initial_neurons = brain.getTotalNeuronCount()
    initial_spikes = brain.getTotalSpikeCount()
    
    # Run simulation steps
    for i in range(50):
        brain.step(i)
    
    final_neurons = brain.getTotalNeuronCount()
    final_spikes = brain.getTotalSpikeCount()
    
    # Neuron count shouldn't change
    assert initial_neurons == final_neurons
    # Spike count should increase
    assert final_spikes >= initial_spikes
    
    print("✓ Brain creation and initialization test passed")

def test_convenience_functions():
    """Test new convenience functions like run_simulation and train_agent."""
    print("Testing convenience functions...")
    
    # Create all components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test run_simulation convenience function
    run_brain = pynlm.run_simulation(brain, world, agent, 100)
    assert run_brain.getTotalNeuronCount() > 0
    
    # Reset for training test
    config2 = pynlm.createDefaultConfig()
    train_brain = pynlm.createBrain(config2)
    train_brain.initialize()
    train_world = pynlm.createSimpleWorld()
    train_world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    train_world.reset()
    train_agent = pynlm.createAgentBrain(train_brain)
    train_agent.initialize(train_world)
    
    # Test train_agent convenience function
    trained_brain = pynlm.train_agent(train_brain, train_world, train_agent, 10, 20)
    assert trained_brain.getTotalNeuronCount() > 0
    
    print("✓ Convenience functions test passed")

def test_context_managers():
    """Test new context managers for safe resource management."""
    print("Testing context managers...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test SimulationContext
    with pynlm.SimulationContext(brain, world, agent) as sim:
        initial_neurons = brain.getTotalNeuronCount()
        sim.run(50)
        assert brain.getTotalNeuronCount() == initial_neurons
        
        stats = sim.get_stats()
        assert "steps_run" in stats
        assert stats["steps_run"] == 50
    
    # Test ConfigContext
    with pynlm.ConfigContext(config) as cfg:
        cfg.set("brain.neuron_count", 3000)
        cfg.apply_preset("agent")
        assert config.get("brain.neuron_count") == 3000
    
    print("✓ Context managers test passed")

def test_checkpoint_functions():
    """Test checkpoint save/load functions."""
    print("Testing checkpoint functions...")
    
    # Create and run a simulation
    config = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    
    for i in range(100):
        brain1.step(i)
    
    initial_spikes = brain1.getTotalSpikeCount()
    
    # Save checkpoint
    with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as tmp:
        checkpoint_path = tmp.name
    
    pynlm.save_checkpoint(brain1, checkpoint_path)
    
    # Create new brain and load checkpoint
    config2 = pynlm.createDefaultConfig()
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    pynlm.load_checkpoint(brain2, checkpoint_path)
    
    # Verify loaded state
    assert brain2.getTotalNeuronCount() == brain1.getTotalNeuronCount()
    # Spike count should be preserved (approximate)
    assert abs(brain2.getTotalSpikeCount() - initial_spikes) < 10
    
    # Clean up
    os.unlink(checkpoint_path)
    
    print("✓ Checkpoint functions test passed")

def test_deepcopy_functionality():
    """Test deepcopy_brain utility function."""
    print("Testing deepcopy functionality...")
    
    config = pynlm.createDefaultConfig()
    original_brain = pynlm.createBrain(config)
    original_brain.initialize()
    
    # Modify original brain
    for i in range(50):
        original_brain.step(i)
    
    # Create deep copy
    copied_brain = pynlm.deepcopy_brain(original_brain)
    
    # Verify copy has same properties
    assert copied_brain.getTotalNeuronCount() == original_brain.getTotalNeuronCount()
    assert copied_brain.getTotalSynapseCount() == original_brain.getTotalSynapseCount()
    
    # Modify copy
    for i in range(50, 100):
        copied_brain.step(i)
    
    # Original should be unchanged
    assert copied_brain.getTotalSpikeCount() > original_brain.getTotalSpikeCount()
    
    print("✓ Deepcopy functionality test passed")

def test_validation_functions():
    """Test configuration validation and formatting."""
    print("Testing validation functions...")
    
    config = pynlm.createDefaultConfig()
    
    # Add some invalid entries
    config.set("invalid..key", "value")
    config.set("valid.key", "value")
    
    # Test validation
    errors = pynlm.validate_config(config)
    assert isinstance(errors, list)
    # Should detect the invalid key with dots
    
    # Test formatting
    formatted = pynlm.format_config(config, indent=2)
    assert isinstance(formatted, str)
    assert len(formatted) > 0
    
    # Should contain valid key
    assert "valid.key" in formatted
    
    print("✓ Validation functions test passed")

def test_profiling_and_benchmarking():
    """Test profiling and benchmarking utilities."""
    print("Testing profiling and benchmarking...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test benchmark_brain
    metrics = pynlm.benchmark_brain(brain, 200, 50)
    assert "steps_per_second" in metrics
    assert "total_time" in metrics
    assert "memory_usage" in metrics
    assert metrics["steps_per_second"] > 0
    
    # Create world and agent for profiling
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test profile_simulation
    profile = pynlm.profile_simulation(brain, world, agent, 100)
    assert "total_time" in profile
    assert "brain_time" in profile
    assert "world_time" in profile
    assert "agent_time" in profile
    
    print("✓ Profiling and benchmarking test passed")

def test_version_and_utility_functions():
    """Test version information and utility functions."""
    print("Testing version and utility functions...")
    
    # Test version
    version = pynlm.get_version()
    assert isinstance(version, str)
    assert "NLM" in version
    assert "v1.0.0" in version
    
    # Test default config dictionary
    default_config = pynlm.get_default_config()
    assert isinstance(default_config, dict)
    assert "brain.neuron_count" in default_config
    assert "plasticity.stdp.enable" in default_config
    
    print("✓ Version and utility functions test passed")

def test_complete_agent_example():
    """Test a complete end-to-end agent example."""
    print("Testing complete agent example...")
    
    def run_complete_example():
        """Complete agent simulation example."""
        # Setup
        config = pynlm.create_agent_config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 20 == 0:
                print(f"Step {step}: Firing neurons: {brain.getFiringNeuronCount()}")
        
        # Return statistics
        return {
            "final_firing_rate": brain.getAverageFiringRate(),
            "curiosity_level": agent.getCuriosityLevel(),
            "novelty_level": agent.getNoveltyLevel(),
            "dev_stage": brain.getDevelopmentalStage(),
            "total_spikes": brain.getTotalSpikeCount()
        }
    
    stats = run_complete_example()
    
    # Verify results
    assert "final_firing_rate" in stats
    assert "curiosity_level" in stats
    assert "novelty_level" in stats
    assert "dev_stage" in stats
    assert "total_spikes" in stats
    
    print("✓ Complete agent example test passed")

def run_all_tests():
    """Run all tests and report results."""
    print("=" * 60)
    print("NLM Python Bindings - Comprehensive Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_imports,
        test_config_types_and_methods,
        test_agent_config_presets,
        test_brain_creation_and_initialization,
        test_convenience_functions,
        test_context_managers,
        test_checkpoint_functions,
        test_deepcopy_functionality,
        test_validation_functions,
        test_profiling_and_benchmarking,
        test_version_and_utility_functions,
        test_complete_agent_example
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ Test {test.__name__} failed: {e}")
            failed += 1
            import traceback
            traceback.print_exc()
        print()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        print("Some tests failed. Please check the output above.")
        return False
    else:
        print("All tests passed! ✓")
        return True

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)