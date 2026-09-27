#!/usr/bin/env python3
"""
Comprehensive test suite for enhanced NLM Python bindings.

This test suite verifies:
1. Enhanced binding functionality
2. Pythonic API design
3. Error handling
4. Serialization
5. Numpy integration
6. Context managers
7. Batch processing
"""

import sys
import os
import tempfile
import numpy as np
import json
from pathlib import Path

# Add the python module to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

# Import enhanced bindings
from enhanced_bindings import (
    EnhancedConfig,
    EnhancedBrain,
    EnhancedAgentBrain,
    EnhancedSimpleWorld,
    create_enhanced_brain,
    create_enhanced_world,
    create_enhanced_agent_brain,
    create_vision_from_numpy,
    create_audio_from_numpy,
    array_to_internal_signals,
    run_simulation,
    brain_simulation,
    world_simulation,
    BatchSimulator,
    NLMError,
    ConfigurationError,
    SimulationError,
    SerializationError,
)


def test_config_creation():
    """Test enhanced config creation and manipulation."""
    print("Testing config creation...")
    
    # Test default config
    config = EnhancedConfig()
    assert config.get_keys() == []
    
    # Test setting values
    config.set("brain.neuron_count", 1000)
    assert config.has("brain.neuron_count")
    assert config.get("brain.neuron_count") == 1000
    
    # Test with default
    assert config.get("brain.synapse_density", 0.1) == 0.1
    
    # Test getting keys
    config.set("test.key1", 1)
    config.set("test.key2", 2)
    keys = config.get_keys()
    assert len(keys) == 3
    assert "test.key1" in keys
    
    # Test clear
    config.clear()
    assert config.get_keys() == []
    
    print("  ✓ Config creation test passed")


def test_config_file_io():
    """Test config file I/O operations."""
    print("Testing config file I/O...")
    
    # Create test config
    config = EnhancedConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.2)
    config.set("plasticity.stdp.enable", True)
    
    # Create temporary file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config_path = f.name
    
    try:
        # Save config
        config.save_to_file(config_path)
        
        # Create new config and load
        config2 = EnhancedConfig()
        config2.load_from_file(config_path)
        
        # Verify values
        assert config2.get("brain.neuron_count") == 500
        assert config2.get("brain.synapse_density") == 0.2
        assert config2.get("plasticity.stdp.enable") == True
        
        print("  ✓ Config file I/O test passed")
    finally:
        # Cleanup
        if os.path.exists(config_path):
            os.unlink(config_path)


def test_brain_basic_functionality():
    """Test basic brain functionality."""
    print("Testing brain basic functionality...")
    
    # Create brain with config
    config = EnhancedConfig()
    config.set("brain.neuron_count", 100)
    
    brain = create_enhanced_brain(config)
    
    # Test initialization
    assert brain.initialize() == True
    assert brain._initialized == True
    
    # Test properties
    assert brain.total_neuron_count > 0
    assert brain.total_synapse_count >= 0
    assert brain.region_count == 0  # Default has no regions
    
    # Test stepping
    brain.step(0)
    brain.step(1, 0.1)  # With time
    
    # Test statistics
    assert brain.total_neuron_count >= 0
    assert brain.firing_neuron_count >= 0
    assert brain.total_spike_count >= 0
    assert brain.average_firing_rate >= 0.0
    assert brain.excitation_inhibition_ratio >= 0.0
    
    # Test region management
    region_id = brain.add_region("cortex")
    assert region_id is not None
    assert brain.region_count == 1
    
    region = brain.get_region(region_id)
    assert region is not None
    
    # Test action production
    action = brain.produce_action()
    assert action is not None
    assert action.get_type() is not None
    
    # Test reset
    brain.reset()
    assert brain._initialized == False
    
    print("  ✓ Brain basic functionality test passed")


def test_brain_save_load():
    """Test brain save and load functionality."""
    print("Testing brain save/load...")
    
    # Create and initialize brain
    config = EnhancedConfig()
    brain = create_enhanced_brain(config)
    brain.initialize()
    
    # Let it do some steps
    for i in range(10):
        brain.step(i)
    
    # Save to temporary file
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        save_path = f.name
    
    try:
        # Save brain state
        result = brain.save(save_path)
        assert result == True
        
        # Reset brain
        brain.reset()
        assert brain._initialized == False
        
        # Load brain state
        result = brain.load(save_path)
        assert result == True
        assert brain._initialized == True
        
        # Verify neurons were loaded
        assert brain.total_neuron_count > 0
        
        print("  ✓ Brain save/load test passed")
    finally:
        if os.path.exists(save_path):
            os.unlink(save_path)


def test_world_functionality():
    """Test world functionality."""
    print("Testing world functionality...")
    
    # Create world
    world = create_enhanced_world()
    
    # Configure world
    world.configure(width=20, height=20, vision_width=8, vision_height=8)
    
    # Test properties
    assert world.width == 20
    assert world.height == 20
    assert world.width == 20
    assert world.height == 20
    
    # Reset world
    world.reset()
    assert world.simulation_time == 0.0
    
    # Set agent start
    world.set_agent_start(10.0, 10.0)
    
    # Update world
    world.update(0.1)
    assert world.simulation_time == 0.1
    
    # Get agent body
    body = world.get_agent_body()
    assert body is not None
    assert body.x == 10.0
    assert body.y == 10.0
    
    # Test configuration methods
    world.set_max_energy(200.0)
    assert world.max_energy == 200.0
    
    # Test object management
    obj = world.add_object(world)
    # Note: This is a simplified test - actual object management
    # depends on the WorldObject class definition
    
    print("  ✓ World functionality test passed")


def test_agent_brain_functionality():
    """Test agent brain functionality."""
    print("Testing agent brain functionality...")
    
    # Create brain and agent brain
    config = EnhancedConfig()
    brain = create_enhanced_brain(config)
    brain.initialize()
    
    agent = create_enhanced_agent_brain(brain)
    
    # Create world
    world = create_enhanced_world()
    world.configure(width=20, height=20, vision_width=8, vision_height=8)
    world.reset()
    
    # Initialize agent
    agent.initialize(world)
    
    # Test enable/disable methods
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(False)
    agent.enable_development(True)
    agent.enable_curiosity(False)
    
    # Verify enabled states
    assert agent.get_brain().get_brain().isRewardModulationEnabled() == True
    assert agent.get_brain().get_brain().isStructuralPlasticityEnabled() == False
    assert agent.get_brain().get_brain().isDevelopmentEnabled() == True
    assert agent.get_brain().get_brain().isCuriosityEnabled() == False
    
    # Test property access
    assert isinstance(agent.curiosity_level, float)
    assert isinstance(agent.novelty_level, float)
    assert isinstance(agent.prediction_error, float)
    assert isinstance(agent.neuromodulation_level, float)
    assert isinstance(agent.developmental_stage, type)  # DevelopmentalStage enum
    
    # Test processing sensory input
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    
    # Test decoding motor command
    action = agent.decode_motor_command()
    assert action is not None
    assert action.get_type() is not None
    
    # Test reward modulation
    agent.apply_reward_modulation(1.0, 0.5)
    
    # Test development update
    agent.update_development(0.1)
    
    # Test reset
    agent.reset()
    
    print("  ✓ Agent brain functionality test passed")


def test_numpy_integration():
    """Test numpy integration functionality."""
    print("Testing numpy integration...")
    
    # Create vision from numpy array
    vision_array = np.random.rand(64, 64, 3).astype(np.float32)
    vision = create_vision_from_numpy(vision_array, normalize=True)
    
    # Verify vision properties
    assert vision.getWidth() == 64
    assert vision.getHeight() == 64
    assert vision.getChannels() == 3
    
    # Create audio from numpy array
    audio_array = np.random.randn(44100).astype(np.float32)
    audio = create_audio_from_numpy(audio_array, sample_rate=44100)
    
    # Verify audio properties
    assert audio.getSampleRate() == 44100
    assert audio.getNumSamples() == 44100
    
    # Create internal signals from array
    signals_array = np.array([0.1, 0.5, 0.9, 1.0, 0.2], dtype=np.float32)
    signals = array_to_internal_signals(signals_array.tolist())
    
    # Verify signals
    assert signals.getData() == signals_array.tolist()
    
    print("  ✓ Numpy integration test passed")


def test_context_managers():
    """Test context managers."""
    print("Testing context managers...")
    
    # Test brain context manager
    config = EnhancedConfig()
    with brain_simulation(config, num_steps=50) as brain:
        assert brain._initialized == True
        for i in range(50):
            brain.step(i)
        assert brain.firing_neuron_count >= 0
    
    # After context, brain should be reset
    assert brain._initialized == False
    
    # Test world context manager
    with world_simulation(width=10, height=10, vision_width=4, vision_height=4) as world:
        assert world.width == 10
        assert world.height == 10
        world.set_agent_start(5.0, 5.0)
        world.reset()
    
    print("  ✓ Context managers test passed")


def test_error_handling():
    """Test error handling."""
    print("Testing error handling...")
    
    # Test configuration error
    try:
        raise ConfigurationError("Invalid configuration")
    except ConfigurationError:
        pass  # Expected
    
    # Test simulation error
    config = EnhancedConfig()
    brain = create_enhanced_brain(config)
    try:
        brain.step(0)  # Should fail since not initialized
        assert False, "Should have raised SimulationError"
    except SimulationError:
        pass  # Expected
    
    # Test serialization error
    try:
        raise SerializationError("Failed to save")
    except SerializationError:
        pass  # Expected
    
    print("  ✓ Error handling test passed")


def test_batch_simulation():
    """Test batch simulation functionality."""
    print("Testing batch simulation...")
    
    config = EnhancedConfig()
    simulator = BatchSimulator(config, num_episodes=10)
    
    # Configure world for simulation
    world_config = {
        'width': 15,
        'height': 15,
        'vision_width': 6,
        'vision_height': 6,
    }
    
    # Run batch simulation
    results = simulator.run_episodes(world_config)
    
    # Verify results structure
    assert 'num_episodes' in results
    assert 'min_neurons' in results
    assert 'max_neurons' in results
    assert 'avg_neurons' in results
    assert 'all_results' in results
    
    assert results['num_episodes'] == 10
    assert len(results['all_results']) == 10
    
    # Verify individual episode results
    for episode_result in results['all_results']:
        assert 'episode' in episode_result
        assert 'neurons' in episode_result
        assert 'spikes' in episode_result
        assert 'firing_rate' in episode_result
        assert episode_result['episode'] < 10
    
    print("  ✓ Batch simulation test passed")


def test_complete_simulation():
    """Test complete simulation workflow."""
    print("Testing complete simulation workflow...")
    
    # Create all components
    config = EnhancedConfig()
    config.set("brain.neuron_count", 100)
    
    brain = create_enhanced_brain(config)
    world = create_enhanced_world()
    world.configure(width=20, height=20, vision_width=8, vision_height=8)
    
    agent = create_enhanced_agent_brain(brain)
    
    # Enable learning features
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    
    # Run simulation
    stats = run_simulation(brain, world, agent, num_steps=100)
    
    # Verify statistics
    assert 'steps' in stats
    assert 'final_neurons' in stats
    assert 'total_spikes' in stats
    assert 'average_firing_rate' in stats
    assert 'curiosity' in stats
    assert 'novelty' in stats
    assert 'prediction_error' in stats
    
    assert stats['steps'] == 100
    assert stats['final_neurons'] >= 0
    assert stats['total_spikes'] >= 0
    assert stats['average_firing_rate'] >= 0.0
    assert stats['curiosity'] >= 0.0
    assert stats['novelty'] >= 0.0
    assert stats['prediction_error'] >= 0.0
    
    print("  ✓ Complete simulation test passed")


def test_utility_functions():
    """Test utility functions."""
    print("Testing utility functions...")
    
    # Test create_checkpoint and load_checkpoint
    config = EnhancedConfig()
    brain = create_enhanced_brain(config)
    brain.initialize()
    
    for i in range(5):
        brain.step(i)
    
    # Create temporary file
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        checkpoint_path = f.name
    
    try:
        # Test create_checkpoint
        result = create_checkpoint(brain, checkpoint_path)
        assert result == True
        
        # Create new brain and load checkpoint
        brain2 = create_enhanced_brain(EnhancedConfig())
        brain2.initialize()
        
        result = load_checkpoint(brain2, checkpoint_path)
        assert result == True
        
        # Verify neurons were loaded
        assert brain2.total_neuron_count > 0
        
        print("  ✓ Utility functions test passed")
    finally:
        if os.path.exists(checkpoint_path):
            os.unlink(checkpoint_path)


def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("Running NLM Enhanced Bindings Tests")
    print("=" * 60)
    
    tests = [
        test_config_creation,
        test_config_file_io,
        test_brain_basic_functionality,
        test_brain_save_load,
        test_world_functionality,
        test_agent_brain_functionality,
        test_numpy_integration,
        test_context_managers,
        test_error_handling,
        test_batch_simulation,
        test_complete_simulation,
        test_utility_functions,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"  ✗ {test.__name__} failed: {e}")
            import traceback
            traceback.print_exc()
            failed += 1
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    return failed == 0


if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
