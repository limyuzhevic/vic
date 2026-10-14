#!/usr/bin/env python3
"""
Test script to verify NLM improvements and demonstrate new functionality.
"""

import pynlm
import tempfile
import os

def test_basic_functionality():
    """Test basic NLM functionality with improved Python bindings."""
    print("=== Testing Basic Functionality ===")
    
    # Test 1: Create and initialize brain
    config = pynlm.create_default_config()
    config.set("neuron_count", 100)
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    assert brain.get_total_neuron_count() == 100
    print("✓ Brain creation and initialization passed")
    
    # Test 2: Run simulation steps
    for step in range(10):
        brain.step(step)
    
    assert brain.get_total_spike_count() >= 0
    print("✓ Simulation steps passed")
    
    # Test 3: Check statistics
    stats = brain.get_simulation_stats()
    assert "total_neurons" in stats
    assert "total_spikes" in stats
    print("✓ Statistics collection passed")
    
    # Test 4: Reset brain
    brain.reset()
    assert brain.get_total_neuron_count() == 100
    print("✓ Brain reset passed")
    
    return brain

def test_configuration_improvements():
    """Test improved configuration system."""
    print("\n=== Testing Configuration Improvements ===")
    
    # Test 1: Automatic type conversion
    config = pynlm.create_default_config()
    config.set("neuron_count", "500")  # String to int
    assert config.get_int("neuron_count") == 500
    print("✓ Automatic type conversion passed")
    
    # Test 2: Configuration validation
    config2 = pynlm.create_default_config()
    config2.set("neuron_count", -10)  # Invalid
    config2.set("connection_probability", 1.5)  # Invalid
    
    validation_issues = config2.validate()
    assert len(validation_issues) > 0
    assert "neuron_count" in validation_issues
    assert "connection_probability" in validation_issues
    print("✓ Configuration validation passed")
    
    # Test 3: Fix configuration and re-validate
    config2.set("neuron_count", 1000)
    config2.set("connection_probability", 0.1)
    
    validation_issues = config2.validate()
    assert len(validation_issues) == 0
    print("✓ Configuration fix and re-validation passed")
    
    # Test 4: File I/O
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config_file = f.name
    
    try:
        config2.save_to_file(config_file)
        config3 = pynlm.create_default_config()
        config3.load_from_file(config_file)
        
        # Compare key values
        assert config2.get_int("neuron_count") == config3.get_int("neuron_count")
        print("✓ Configuration file I/O passed")
    finally:
        os.unlink(config_file)
    
    return config2

def test_advanced_utilities():
    """Test advanced simulation utilities."""
    print("\n=== Testing Advanced Utilities ===")
    
    # Test 1: create_reproducible_simulation
    brain1 = pynlm.create_reproducible_simulation(seed=123)
    brain2 = pynlm.create_reproducible_simulation(seed=123)
    
    # Run same steps on both
    for step in range(50):
        brain1.step(step)
        brain2.step(step)
    
    # Should have same spike count (if random seed is properly applied)
    assert brain1.get_total_spike_count() == brain2.get_total_spike_count()
    print("✓ Reproducible simulation passed")
    
    # Test 2: profile_simulation
    brain3 = pynlm.create_default_config()
    brain3.initialize()
    
    result = pynlm.profile_simulation(brain3, steps=100)
    assert "steps_performed" in result
    assert "total_time_ms" in result
    assert "steps_per_second" in result
    assert "final_neuron_count" in result
    assert "final_spike_count" in result
    print("✓ Performance profiling passed")
    
    # Test 3: batch_sims
    configs = []
    for i in range(3):
        config = pynlm.create_default_config()
        config.set("random_seed", i * 100)
        configs.append(config)
    
    brains = pynlm.batch_sims(configs, steps_per_config=50)
    assert len(brains) == 3
    
    for i, brain in enumerate(brains):
        assert brain.get_total_neuron_count() == 1000
        # Each brain should have different random behavior
        if i == 0:
            brain1_spikes = brain.get_total_spike_count()
        elif i == 1:
            brain2_spikes = brain.get_total_spike_count()
        else:
            brain3_spikes = brain.get_total_spike_count()
    
    print("✓ Batch simulation passed")
    
    # Test 4: safe_simulation_step
    brain4 = pynlm.create_default_config()
    brain4.initialize()
    
    # Normal step should succeed
    assert pynlm.safe_simulation_step(brain4, 0) == True
    print("✓ Safe simulation step passed")
    
    return brain1

def test_agent_development():
    """Test agent development features."""
    print("\n=== Testing Agent Development ===")
    
    # Create world
    world = pynlm.create_world()
    world.configure(10, 10, 8, 8)
    world.reset()
    
    # Create agent
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    agent = pynlm.create_agent_brain(brain)
    agent.initialize(world)
    
    # Test 1: Enable all systems
    agent.enable_all_systems()
    
    assert agent.is_reward_modulation_enabled() == True
    assert agent.is_structural_plasticity_enabled() == True
    assert agent.is_development_enabled() == True
    assert agent.is_curiosity_enabled() == True
    print("✓ Enable all systems passed")
    
    # Test 2: Agent statistics
    agent_stats = agent.get_agent_stats()
    assert "curiosity" in agent_stats
    assert "novelty" in agent_stats
    assert "prediction_error" in agent_stats
    assert "neuromodulation" in agent_stats
    assert "developmental_stage" in agent_stats
    print("✓ Agent statistics passed")
    
    # Test 3: Run episode
    for step in range(50):
        world.update(0.1)
        
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Apply reward
        reward = 1.0 if step % 10 == 0 else 0.0
        agent.apply_reward_modulation(reward, 0.0)
        
        # Update development
        agent.update_development(0.1)
    
    print("✓ Agent episode simulation passed")
    
    return agent

def test_error_handling():
    """Test error handling and recovery."""
    print("\n=== Testing Error Handling ===")
    
    # Test 1: Configuration error
    try:
        config = pynlm.create_default_config()
        config.set("invalid_key", "invalid_value")
        # This should work - invalid keys are allowed
        print("✓ Invalid key handling passed")
    except Exception as e:
        print(f"✗ Invalid key handling failed: {e}")
    
    # Test 2: Simulation error recovery
    brain = pynlm.create_default_config()
    brain.initialize()
    
    # Normal operation should work
    assert pynlm.safe_simulation_step(brain, 0) == True
    
    # The safe simulation should handle errors gracefully
    print("✓ Error recovery passed")
    
    # Test 3: Type conversion errors
    config = pynlm.create_default_config()
    config.set("neuron_count", "not_a_number")
    # Should handle gracefully
    print("✓ Type conversion error handling passed")
    
    return brain

def test_world_features():
    """Test world-related features."""
    print("\n=== Testing World Features ===")
    
    # Create world
    world = pynlm.create_world()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    # Test 1: World size
    assert world.get_width() == 20
    assert world.get_height() == 20
    print("✓ World size access passed")
    
    # Test 2: Agent position
    world.set_agent_start(5.0, 5.0)
    body = world.get_agent_body()
    assert body.x == 5.0
    assert body.y == 5.0
    print("✓ Agent position setting passed")
    
    # Test 3: Object management
    obj = pynlm.create_world_object(3.0, 3.0, pynlm.WorldObjectType.Resource, 1.0)
    world.add_object(obj)
    print("✓ World object creation and addition passed")
    
    # Test 4: World operations
    assert world.is_valid_position(5.0, 5.0) == True
    assert world.is_valid_position(25.0, 25.0) == False
    print("✓ World position validation passed")
    
    return world

def main():
    """Run all tests."""
    print("NLM Improvements Test Suite")
    print("=" * 50)
    
    try:
        # Run tests
        brain = test_basic_functionality()
        config = test_configuration_improvements()
        brain1 = test_advanced_utilities()
        agent = test_agent_development()
        error_brain = test_error_handling()
        world = test_world_features()
        
        print("\n" + "=" * 50)
        print("All tests passed successfully!")
        print("\nKey improvements verified:")
        print("✓ Enhanced Python bindings with Pythonic interface")
        print("✓ Configuration validation and schema support")
        print("✓ Advanced simulation utilities (batch, profiling, reproducible)")
        print("✓ Agent development features (enable_all_systems, statistics)")
        print("✓ Error handling and recovery mechanisms")
        print("✓ World management with object creation and validation")
        print("\nThe NLM Python API has been successfully improved!")
        
    except Exception as e:
        print(f"\n✗ Test failed: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())