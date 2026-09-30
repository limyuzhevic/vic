#!/usr/bin/env python3
"""
NLM Python Examples - Enhanced API

This file contains comprehensive examples demonstrating the enhanced NLM Python API.

## Usage

Run these examples to see the enhanced API in action:

    python example_01_basic_brain.py
    python example_02_agent_simulation.py
    python example_03_batch_operations.py
    python example_04_performance_monitoring.py
    python example_05_error_handling.py
    python example_06_configuration_validation.py

Each example demonstrates different aspects of the enhanced API.
"""

import time
import sys
import os

# Add current directory to path for imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm


def example_01_basic_brain():
    """
    Example 01: Basic Brain Usage with Enhanced API
    
    Demonstrates the basic usage of the enhanced NLM Python API.
    Shows both old and new patterns.
    """
    print("=== Example 01: Basic Brain Usage ===")
    
    # NEW WAY - Using enhanced BrainBuilder
    print("\n1. NEW WAY - Using BrainBuilder:")
    brain_builder = pynlm.BrainBuilder()
    brain = brain_builder.create_brain()
    brain.initialize()
    
    # Run simulation using batch operation
    print("   Running 100 steps using batch operation...")
    brain.run_batch(100)
    
    # Get statistics
    spikes, firing_rate = brain.run_batch_with_stats(50)
    print(f"   Generated {spikes} spikes with {firing_rate:.2f} avg firing rate")
    
    # OLD WAY - Original API (still supported)
    print("\n2. OLD WAY - Original API:")
    config = pynlm.create_default_config()
    brain_old = pynlm.create_brain(config)
    brain_old.initialize()
    
    for i in range(50):
        brain_old.step(i)
    
    print(f"   Generated {brain_old.get_total_spike_count()} spikes")
    
    print("\n=== Example 01 Complete ===\n")


def example_02_agent_simulation():
    """
    Example 02: Complete Agent Simulation
    
    Demonstrates a complete agent simulation with world interaction,
    learning subsystems, and proper setup using enhanced API.
    """
    print("=== Example 02: Complete Agent Simulation ===")
    
    # NEW WAY - Using factory function
    print("\n1. NEW WAY - Using factory function:")
    brain, agent, world = pynlm.BrainBuilder().create_agent_setup(
        neurons=500,
        width=15,
        height=15
    )
    
    # Configure world
    world.configure(15, 15, 8, 8)
    world.setAgentStart(7.5, 7.5)
    
    # Initialize agent with learning subsystems
    agent.initialize(world)
    
    # Enable learning
    agent.enable_subsystem("reward_modulation", True)
    agent.enable_subsystem("curiosity", True)
    agent.enable_subsystem("development", True)
    
    # Run simulation
    print("   Running 200-step agent simulation...")
    for step in range(200):
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command and act
        cmd = agent.decode_motor_command()
        world.apply_action(cmd, world.get_simulation_time())
        
        # Apply learning
        if step % 50 == 0 and agent.is_subsystem_enabled("reward_modulation"):
            reward = percept.get_internal()[0] if percept.get_internal() else 0.0
            agent.apply_reward_modulation(reward, 0.0)
        
        # Update development
        agent.update_development(0.1)
        
        # Progress reporting
        if step % 50 == 0:
            print(f"   Step {step}: curiosity={agent.get_curiosity_level():.2f}, "
                  f"novelty={agent.get_novelty_level():.2f}")
    
    # Get final statistics
    print("\n   Final statistics:")
    print(f"   Total spikes: {brain.get_total_spike_count()}")
    print(f"   Curiosity level: {agent.get_curiosity_level():.2f}")
    print(f"   Novelty level: {agent.get_novelty_level():.2f}")
    print(f"   Development stage: {brain.get_developmental_stage()}")
    
    # Get summary using enhanced API
    summary = pynlm.get_simulation_summary(brain)
    print(f"   Brain summary: {summary}")
    
    # OLD WAY - Manual setup
    print("\n2. OLD WAY - Manual setup:")
    config = pynlm.create_default_config()
    brain_old = pynlm.create_brain(config)
    brain_old.initialize()
    
    agent_old = pynlm.create_agent_brain(brain_old)
    world_old = pynlm.create_simple_world()
    world_old.configure(10, 10, 8, 8)
    world_old.reset()
    
    agent_old.initialize(world_old)
    
    print(f"   Created old-style setup: {brain_old.get_total_neuron_count()} neurons")
    
    print("\n=== Example 02 Complete ===\n")


def example_03_batch_operations():
    """
    Example 03: Batch Operations and Performance
    
    Demonstrates batch operations for improved performance and
    performance monitoring capabilities.
    """
    print("=== Example 03: Batch Operations ===")
    
    # NEW WAY - Using batch operations
    print("\n1. NEW WAY - Batch operations:")
    brain_builder = pynlm.BrainBuilder()
    brain = brain_builder.create_brain()
    brain.initialize()
    
    # Monitor performance
    print("   Monitoring simulation performance...")
    performance = pynlm.performance_monitor(brain, 1000)
    fps = performance[0]
    total_time = performance[1]
    total_spikes = performance[2]
    
    print(f"   Performance: {fps:.1f} FPS")
    print(f"   Time: {total_time} ms for {total_spikes} spikes")
    
    # Batch current injection
    print("\n2. Batch current injection:")
    neurons = [pynlm.INVALID_NEURON_ID] * 10  # Placeholder neurons
    # In real usage, get actual neuron IDs from brain
    brain.inject_current_batch(neurons, 5.0)
    print(f"   Injected current into {len(neurons)} neurons")
    
    # Batch type injection
    print("\n3. Batch type injection:")
    brain.inject_current_to_type_batch(pynlm.NeuronType.Sensory, 3.0)
    print("   Injected sensory current to all sensory neurons")
    
    # Progress callback
    print("\n4. Progress monitoring:")
    def progress_callback(step, firing_rate):
        if step % 200 == 0:
            print(f"   Step {step}: firing rate = {firing_rate:.2f}")
    
    runner = pynlm.BrainRunner()
    runner.run_simulation(brain, 500, progress_callback)
    
    print("\n=== Example 03 Complete ===\n")


def example_04_performance_monitoring():
    """
    Example 04: Performance Monitoring and Optimization
    
    Demonstrates various performance monitoring capabilities
    and optimization techniques.
    """
    print("=== Example 04: Performance Monitoring ===")
    
    # NEW WAY - Performance monitoring
    print("\n1. Performance comparison:")
    
    # Create brain and run test
    brain = pynlm.BrainBuilder().create_brain()
    brain.initialize()
    
    # Time different approaches
    start_time = time.time()
    for i in range(100):
        brain.step(i)
    sequential_time = time.time() - start_time
    
    start_time = time.time()
    brain.run_batch(100)
    batch_time = time.time() - start_time
    
    print(f"   Sequential: {sequential_time:.3f}s")
    print(f"   Batch:      {batch_time:.3f}s")
    print(f"   Speedup:    {sequential_time/batch_time:.1f}x")
    
    # Memory usage monitoring (simulated)
    print("\n2. Resource monitoring:")
    summary = pynlm.get_simulation_summary(brain)
    print(f"   Memory usage (simulated): {summary['neurons']} neurons, {summary['synapses']} synapses")
    print(f"   Performance metrics: {summary['firing_neurons']} firing neurons, {summary['avg_firing_rate']:.2f} Hz")
    
    # Scalability test
    print("\n3. Scalability test:")
    scales = [100, 500, 1000]
    times = []
    
    for scale in scales:
        test_brain = pynlm.BrainBuilder().create_brain()
        test_brain.initialize()
        
        start_time = time.time()
        test_brain.run_batch(scale)
        elapsed = time.time() - start_time
        times.append(elapsed)
        
        print(f"   {scale:4d} steps: {elapsed:.3f}s ({scale/elapsed:.0f} steps/s)")
    
    # Calculate scaling efficiency
    if len(times) >= 2:
        scale_factor = scales[-1] / scales[0]
        time_factor = times[-1] / times[0]
        efficiency = scale_factor / time_factor
        print(f"   Scaling efficiency: {efficiency:.2f}")
    
    print("\n=== Example 04 Complete ===\n")


def example_05_error_handling():
    """
    Example 05: Error Handling and Recovery
    
    Demonstrates error handling, validation, and recovery mechanisms.
    """
    print("=== Example 05: Error Handling ===")
    
    # NEW WAY - Configuration validation
    print("\n1. Configuration validation:")
    from pynlm import validate_brain_config
    
    # Valid configuration
    valid_config = pynlm.create_default_config()
    valid_config.set("neuron_count", 500)
    
    try:
        validate_brain_config(valid_config)
        print("   ✓ Valid configuration accepted")
        
        brain = pynlm.Brain(valid_config)
        print("   ✓ Brain created successfully")
    except RuntimeError as e:
        print(f"   ✗ Validation failed: {e}")
    
    # Invalid configuration
    invalid_config = pynlm.create_default_config()
    invalid_config.set("neuron_count", 10)  # Too small
    
    try:
        validate_brain_config(invalid_config)
        print("   ✗ Invalid configuration accepted (should not happen)")
    except RuntimeError as e:
        print(f"   ✓ Invalid configuration rejected: {e}")
    
    # NEW WAY - Safe operations
    print("\n2. Safe operations with recovery:")
    brain = pynlm.BrainBuilder().create_brain()
    brain.initialize()
    
    # Safe stepping
    try:
        pynlm.safe_step(brain, 100)
        print("   ✓ Step completed successfully")
    except Exception as e:
        print(f"   ✗ Step failed: {e}")
        print("   ✓ Brain automatically reset and recovered")
    
    # Invalid operations
    print("\n3. Error recovery:")
    invalid_neurons = [pynlm.INVALID_NEURON_ID]  # Invalid neuron IDs
    
    try:
        brain.inject_current_batch(invalid_neurons, 10.0)
        print("   ⚠ Batch operation completed (may have ignored invalid neurons)")
    except Exception as e:
        print(f"   ✗ Batch operation failed: {e}")
    
    print("\n=== Example 05 Complete ===\n")


def example_06_configuration_validation():
    """
    Example 06: Configuration Validation and Management
    
    Demonstrates advanced configuration features and validation.
    """
    print("=== Example 06: Configuration Validation ===")
    
    # NEW WAY - Advanced configuration
    print("\n1. Advanced configuration:")
    
    # Create custom configuration
    config = pynlm.create_default_config()
    
    # Set various parameters
    config.set("neuron_count", 1500)
    config.set("connection_probability", 0.12)
    config.set("simulation_timestep", 0.002)
    config.set("random_seed", 12345)
    
    # Check configuration
    print(f"   Neuron count: {config.get_int('neuron_count', 1000)}")
    print(f"   Connection probability: {config.get_float('connection_probability', 0.1)}")
    print(f"   Simulation timestep: {config.get_float('simulation_timestep', 0.001)}")
    
    # Configuration validation
    print("\n2. Configuration validation:")
    try:
        validate_brain_config(config)
        brain = pynlm.Brain(config)
        brain.initialize()
        print("   ✓ Configuration validated and brain created")
    except RuntimeError as e:
        print(f"   ✗ Configuration invalid: {e}")
    
    # Configuration comparison
    print("\n3. Configuration comparison:")
    config1 = pynlm.create_default_config()
    config1.set("neuron_count", 1000)
    
    config2 = pynlm.create_default_config()
    config2.set("neuron_count", 2000)
    
    print(f"   Config 1: {config1.get_int('neuron_count', 0)} neurons")
    print(f"   Config 2: {config2.get_int('neuron_count', 0)} neurons")
    
    # Configuration backup and restore
    print("\n4. Configuration backup and restore:")
    original_config = pynlm.create_default_config()
    
    # Modify config
    modified_config = pynlm.create_default_config()
    modified_config.set("neuron_count", 1200)
    
    print(f"   Original: {original_config.get_int('neuron_count', 0)} neurons")
    print(f"   Modified: {modified_config.get_int('neuron_count', 0)} neurons")
    
    # Restore original
    restored_config = pynlm.create_default_config()
    print(f"   Restored: {restored_config.get_int('neuron_count', 0)} neurons")
    
    print("\n=== Example 06 Complete ===\n")


def example_07_learning_subjects():
    """
    Example 07: Learning Subsystems and Interaction
    
    Demonstrates the interaction between different learning subsystems
    and how they affect brain behavior.
    """
    print("=== Example 07: Learning Subsystems ===")
    
    # NEW WAY - Learning subsystems
    print("\n1. Learning subsystem interaction:")
    
    brain, agent, world = pynlm.BrainBuilder().create_agent_setup(
        neurons=800,
        width=20,
        height=20
    )
    
    world.configure(20, 20, 8, 8)
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enable_subsystem("reward_modulation", True)
    agent.enable_subsystem("curiosity", True)
    agent.enable_subsystem("development", True)
    
    # Track system states
    curiosity_levels = []
    novelty_levels = []
    development_stages = []
    
    # Run simulation with different phases
    print("   Running learning simulation...")
    for step in range(300):
        world.update(0.1)
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        brain.step(step)
        
        cmd = agent.decode_motor_command()
        world.apply_action(cmd, world.get_simulation_time())
        
        # Record subsystem states
        curiosity_levels.append(agent.get_curiosity_level())
        novelty_levels.append(agent.get_novelty_level())
        development_stages.append(brain.get_developmental_stage())
        
        # Apply learning
        if step % 50 == 0 and agent.is_subsystem_enabled("reward_modulation"):
            reward = percept.get_internal()[0] if percept.get_internal() else 0.0
            agent.apply_reward_modulation(reward, 0.0)
        
        agent.update_development(0.1)
        
        if step % 100 == 0:
            print(f"   Step {step}: curiosity={curiosity_levels[-1]:.2f}, "
                  f"novelty={novelty_levels[-1]:.2f}")
    
    # Analyze learning progress
    print("\n2. Learning analysis:")
    avg_curiosity = sum(curiosity_levels) / len(curiosity_levels)
    avg_novelty = sum(novelty_levels) / len(novelty_levels)
    stage_changes = len(set(development_stages))
    
    print(f"   Average curiosity: {avg_curiosity:.2f}")
    print(f"   Average novelty: {avg_novelty:.2f}")
    print(f"   Development stage changes: {stage_changes}")
    
    # Check learning effectiveness
    print("\n3. Learning effectiveness:")
    if avg_curiosity > 0.3:
        print("   ✓ Curiosity-driven exploration active")
    if avg_novelty > 0.2:
        print("   ✓ Novelty detection working")
    if stage_changes > 1:
        print("   ✓ Development progression detected")
    if agent.is_subsystem_enabled("reward_modulation"):
        print("   ✓ Reward modulation enabled")
    
    print("\n=== Example 07 Complete ===\n")


def main():
    """
    Main function to run all examples.
    """
    print("=" * 60)
    print("NLM Enhanced Python API Examples")
    print("=" * 60)
    print("\nThis script demonstrates the enhanced NLM Python API")
    print("with high-level convenience classes, batch operations,")
    print("performance monitoring, and comprehensive error handling.\n")
    
    examples = [
        example_01_basic_brain,
        example_02_agent_simulation,
        example_03_batch_operations,
        example_04_performance_monitoring,
        example_05_error_handling,
        example_06_configuration_validation,
        example_07_learning_subjects,
    ]
    
    for i, example in enumerate(examples, 1):
        try:
            example()
        except Exception as e:
            print(f"Error in example {i}: {e}")
            import traceback
            traceback.print_exc()
    
    print("=" * 60)
    print("All examples completed!")
    print("=" * 60)
    print("\nKey takeaways:")
    print("1. Use BrainBuilder for quick setup")
    print("2. Leverage batch operations for performance")
    print("3. Monitor performance with built-in tools")
    print("4. Validate configurations before use")
    print("5. Use error handling for robustness")
    print("6. Enable learning subsystems for realistic behavior")
    print("7. Use agent subsystem control for flexibility")


if __name__ == "__main__":
    main()
