#!/usr/bin/env python3
"""
Demonstration of improved NLM Python bindings.

This script demonstrates the enhanced API improvements made to the NLM bindings,
showing how the new features make the framework more accessible and Pythonic.
"""

import sys
import os
import numpy as np

# Add the python module to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

from enhanced_bindings import (
    EnhancedConfig,
    EnhancedBrain,
    EnhancedAgentBrain,
    EnhancedSimpleWorld,
    create_enhanced_brain,
    create_enhanced_world,
    create_enhanced_agent_brain,
    create_vision_from_numpy,
    run_simulation,
    brain_simulation,
    world_simulation,
)


def demonstrate_basic_usage():
    """Demonstrate basic usage with enhanced API."""
    print("=" * 60)
    print("1. Basic Usage with Enhanced API")
    print("=" * 60)
    
    # Create brain with enhanced config
    config = EnhancedConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.15)
    
    brain = create_enhanced_brain(config)
    
    # Initialize brain
    brain.initialize()
    print(f"✓ Brain initialized with {brain.total_neuron_count} neurons")
    
    # Run simulation with statistics
    print("Running simulation...")
    for step in range(100):
        brain.step(step)
        
        if step % 25 == 0:
            print(f"  Step {step}: {brain.firing_neuron_count} neurons firing, "
                  f"{brain.total_spike_count} total spikes")
    
    print(f"✓ Simulation complete: {brain.total_spike_count} total spikes")
    print()


def demonstrate_pythonic_properties():
    """Demonstrate Pythonic property access."""
    print("=" * 60)
    print("2. Pythonic Property Access")
    print("=" * 60)
    
    config = EnhancedConfig()
    brain = create_enhanced_brain(config)
    brain.initialize()
    
    # Pythonic property access
    print(f"Total neurons: {brain.total_neuron_count}")
    print(f"Firing neurons: {brain.firing_neuron_count}")
    print(f"Average firing rate: {brain.average_firing_rate:.3f}")
    print(f"Total spikes: {brain.total_spike_count}")
    print(f"E/I balance: {brain.excitation_inhibition_ratio:.3f}")
    print(f"Developmental stage: {brain.developmental_stage}")
    print(f"Region count: {brain.region_count}")
    
    # Add a region and access it
    region_id = brain.add_region("cortex")
    print(f"Added region with ID: {region_id}")
    print(f"Number of regions: {brain.region_count}")
    
    print()


def demonstrate_world_agent_integration():
    """Demonstrate world-agent integration."""
    print("=" * 60)
    print("3. World-Agent Integration")
    print("=" * 60)
    
    # Create world
    world = create_enhanced_world()
    world.configure(width=20, height=20, vision_width=8, vision_height=8)
    world.reset()
    world.set_agent_start(10.0, 10.0)
    
    print(f"World created: {world.width}x{world.height}")
    print(f"Agent start position: ({world.get_agent_body().x}, {world.get_agent_body().y})")
    
    # Create brain and agent
    config = EnhancedConfig()
    brain = create_enhanced_brain(config)
    brain.initialize()
    
    agent = create_enhanced_agent_brain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    agent.enable_development(True)
    
    print("Learning enabled: Reward, Curiosity, Development")
    
    # Run a simple world loop
    for step in range(50):
        world.update(0.1)
        
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        
        brain.step(step)
        
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.simulation_time)
        
        # Log stats
        if step % 10 == 0:
            print(f"  Step {step}: Pos({world.get_agent_body().x:.1f}, {world.get_agent_body().y:.1f}), "
                  f"Firing: {brain.firing_neuron_count}, Curiosity: {agent.curiosity_level:.3f}")
    
    print(f"✓ World simulation complete. Final agent position: "
          f"({world.get_agent_body().x:.1f}, {world.get_agent_body().y:.1f})")
    print()


def demonstrate_numpy_integration():
    """Demonstrate numpy integration."""
    print("=" * 60)
    print("4. Numpy Integration")
    print("=" * 60)
    
    # Create vision from numpy array
    print("Creating vision input from numpy array...")
    vision_array = np.random.rand(32, 32, 3).astype(np.float32)
    vision = create_vision_from_numpy(vision_array, normalize=True)
    
    print(f"Vision shape: {vision_array.shape}")
    print(f"Vision width: {vision.getWidth()}, height: {vision.getHeight()}, channels: {vision.getChannels()}")
    
    # Create audio from numpy array
    print("\nCreating audio input from numpy array...")
    audio_array = np.random.randn(8820).astype(np.float32)  # 0.2 seconds at 44.1kHz
    audio = create_audio_from_numpy(audio_array, sample_rate=44100)
    
    print(f"Audio length: {len(audio_array)} samples")
    print(f"Audio sample rate: {audio.getSampleRate()}")
    
    print()


def demonstrate_context_managers():
    """Demonstrate context managers."""
    print("=" * 60)
    print("5. Context Managers")
    print("=" * 60)
    
    # Brain context manager
    print("Using brain context manager...")
    config = EnhancedConfig()
    with brain_simulation(config, num_steps=100) as brain:
        print(f"✓ Brain initialized with {brain.total_neuron_count} neurons")
        
        for i in range(50):
            brain.step(i)
            
            if i % 10 == 0:
                print(f"  Step {i}: {brain.firing_neuron_count} firing neurons")
        
        print("✓ Brain context manager automatically reset brain after use")
    
    print(f"✓ Brain reset: {brain._initialized} (should be False)")
    
    # World context manager
    print("\nUsing world context manager...")
    with world_simulation(width=15, height=15, vision_width=6, vision_height=6) as world:
        print(f"✓ World created: {world.width}x{world.height}")
        world.set_agent_start(7.5, 7.5)
        
        for i in range(10):
            world.update(0.2)
            print(f"  Step {i}: Position ({world.get_agent_body().x:.1f}, {world.get_agent_body().y:.1f})")
    
    print("✓ World context manager completed")
    print()


def demonstrate_error_handling():
    """Demonstrate improved error handling."""
    print("=" * 60)
    print("6. Improved Error Handling")
    print("=" * 60)
    
    config = EnhancedConfig()
    brain = create_enhanced_brain(config)
    
    # Test error: try to step without initializing
    try:
        brain.step(0)
        print("✗ Should have raised error")
    except Exception as e:
        print(f"✓ Caught error: {type(e).__name__}: {e}")
    
    # Initialize and try again
    brain.initialize()
    print("✓ Brain initialized successfully")
    
    # Now step should work
    brain.step(0)
    print("✓ Step completed successfully")
    
    print()


def demonstrate_batch_processing():
    """Demonstrate batch processing utilities."""
    print("=" * 60)
    print("7. Batch Processing Utilities")
    print("=" * 60)
    
    from enhanced_bindings import BatchSimulator
    
    config = EnhancedConfig()
    print("Creating batch simulator...")
    
    simulator = BatchSimulator(config, num_episodes=5)
    
    # Configure world for batch simulation
    world_config = {
        'width': 10,
        'height': 10,
        'vision_width': 4,
        'vision_height': 4,
    }
    
    print(f"Running {simulator.num_episodes} episodes...")
    
    results = simulator.run_episodes(world_config)
    
    print(f"✓ Batch simulation completed:")
    print(f"  Episodes: {results['num_episodes']}")
    print(f"  Neurons: {results['min_neurons']} - {results['max_neurons']} "
          f"(avg: {results['avg_neurons']:.1f})")
    print(f"  Spikes: {results['min_spikes']} - {results['max_spikes']} "
          f"(avg: {results['avg_spikes']:.1f})")
    print(f"  Firing rates: {results['min_firing_rate']:.3f} - "
          f"{results['max_firing_rate']:.3f} (avg: {results['avg_firing_rate']:.3f})")
    
    # Show some sample results
    print(f"\nSample episode results:")
    for i, episode_result in enumerate(results['all_results'][:3]):
        print(f"  Episode {episode_result['episode']}: "
              f"{episode_result['neurons']} neurons, "
              f"{episode_result['spikes']} spikes, "
              f"rate: {episode_result['firing_rate']:.3f}, "
              f"curiosity: {episode_result['curiosity']:.3f}")
    
    print()


def demonstrate_serialization():
    """Demonstrate serialization utilities."""
    print("=" * 60)
    print("8. Serialization Utilities")
    print("=" * 60)
    
    from enhanced_bindings import create_checkpoint, load_checkpoint
    import tempfile
    import os
    
    # Create and run brain
    config = EnhancedConfig()
    brain = create_enhanced_brain(config)
    brain.initialize()
    
    print("Running brain simulation...")
    for i in range(20):
        brain.step(i)
    
    print(f"Brain state: {brain.total_neuron_count} neurons, "
          f"{brain.total_spike_count} spikes")
    
    # Create temporary checkpoint
    with tempfile.NamedTemporaryFile(suffix='.nlm_checkpoint', delete=False) as f:
        checkpoint_path = f.name
    
    try:
        # Save checkpoint
        print(f"Saving checkpoint to {checkpoint_path}...")
        result = create_checkpoint(brain, checkpoint_path)
        if result:
            print("✓ Checkpoint saved successfully")
        else:
            print("✗ Checkpoint save failed")
            return
        
        # Reset brain
        brain.reset()
        print(f"✓ Brain reset. Neurons: {brain.total_neuron_count}")
        
        # Load checkpoint
        print("Loading checkpoint...")
        result = load_checkpoint(brain, checkpoint_path)
        if result:
            print("✓ Checkpoint loaded successfully")
            print(f"✓ Loaded brain state: {brain.total_neuron_count} neurons")
        else:
            print("✗ Checkpoint load failed")
        
    finally:
        if os.path.exists(checkpoint_path):
            os.unlink(checkpoint_path)
    
    print()


def main():
    """Run all demonstrations."""
    print("\n" + "=" * 80)
    print("NLM Enhanced Python Bindings Demonstration")
    print("=" * 80)
    print("\nThis script demonstrates the improved API features of the NLM Python bindings.")
    print("\nKey improvements:")
    print("  • Pythonic property access (brain.total_neuron_count vs brain.getTotalNeuronCount())")
    print("  • Context managers for resource management")
    print("  • Enhanced error handling with descriptive messages")
    print("  • Numpy integration for sensory input")
    print("  • Batch processing utilities")
    print("  • Serialization utilities with checkpoints")
    print("  • More intuitive API design")
    print("=" * 80 + "\n")
    
    demonstrations = [
        demonstrate_basic_usage,
        demonstrate_pythonic_properties,
        demonstrate_world_agent_integration,
        demonstrate_numpy_integration,
        demonstrate_context_managers,
        demonstrate_error_handling,
        demonstrate_batch_processing,
        demonstrate_serialization,
    ]
    
    for demo in demonstrations:
        try:
            demo()
        except Exception as e:
            print(f"✗ Demonstration failed: {e}")
            import traceback
            traceback.print_exc()
    
    print("=" * 80)
    print("All demonstrations completed!")
    print("=" * 80)


if __name__ == "__main__":
    main()
