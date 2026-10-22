#!/usr/bin/env python3
"""
NLM High-Level API Examples

This module demonstrates the usage of high-level convenience functions
for the NLM (Neural Learning Machine) framework.

These examples show how the convenience functions simplify common operations
that were previously shown in the documentation.
"""

import sys
import os

# Add the project root to Python path
sys.path.insert(0, str(os.path.dirname(os.path.abspath(__file__))))

from python.high_level_api import (
    create_agent_brain_with_config,
    run_simulation_loop,
    run_agent_simulation,
    get_brain_stats,
    enable_brain_features,
    save_brain_with_timestamp,
    load_brain_with_backup,
    configure_brain_from_dict,
    create_world_from_config
)

def example_1_basic_usage():
    """
    Example 1: Basic usage with defaults
    
    This shows the simplest way to create and use a brain.
    """
    print("=== Example 1: Basic Usage ===\n")
    
    # Create brain with default configuration
    print("1. Creating brain with default configuration...")
    brain = create_agent_brain_with_config()
    print(f"   ✓ Brain created with {brain.getTotalNeuronCount()} neurons")
    
    # Show brain statistics
    print("\n2. Getting brain statistics...")
    stats = get_brain_stats(brain)
    print(f"   Total neurons: {stats['neuron_count']}")
    print(f"   Total synapses: {stats['synapse_count']}")
    print(f"   Average firing rate: {stats['average_firing_rate']:.2f}")
    
    # Run a short simulation
    print("\n3. Running a short simulation...")
    for step in range(100):
        brain.step(step)
    
    print(f"   ✓ Simulation complete")
    print(f"   Total spikes: {stats['total_spike_count']}")
    
    print("\n" + "="*50 + "\n")
def example_2_simulation_loop():
    """
    Example 2: Complete simulation loop with world and agent
    
    This shows how to run a complete simulation loop with all components.
    """
    print("=== Example 2: Simulation Loop ===\n")
    
    # Import pynlm for this example
    import pynlm
    
    print("1. Setting up simulation components...")
    
    # Create brain and agent
    brain = create_agent_brain_with_config()
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable agent features
    enable_brain_features(agent, reward_modulation=True, 
                         structural_plasticity=True, development=True,
                         curiosity=True)
    
    print(f"   ✓ Components created")
    print(f"     - Brain neurons: {brain.getTotalNeuronCount()}")
    print(f"     - World size: {world.getWidth()}x{world.getHeight()}")
    
    # Run simulation loop
    print("\n2. Running simulation loop (200 steps)...")
    results = run_simulation_loop(brain, world, agent, 200, verbose=True)
    
    print(f"\n   ✓ Simulation results:")
    print(f"     - Steps completed: {results['steps']}")
    print(f"     - Spikes generated: {results['spike_increase']}")
    print(f"     - Average firing rate: {results['average_firing_rate']:.3f}")
    print(f"     - Total time: {results['total_time']:.2f} seconds")
    
    print("\n" + "="*50 + "\n")
def example_3_complete_agent_simulation():
    """
    Example 3: Complete agent simulation (from documentation)
    
    This replicates the complete example from HOW_TO_USE.md using
    high-level convenience functions.
    """
    print("=== Example 3: Complete Agent Simulation ===\n")
    
    print("Running complete agent simulation with all features...")
    
    # Run complete agent simulation
    results = run_agent_simulation(
        steps=500,
        enable_features=True
    )
    
    print(f"\n✓ Simulation Results:")
    print(f"  Steps completed: {results['steps']}")
    print(f"  Spikes generated: {results['spike_increase']}")
    print(f"  Average firing rate: {results['average_firing_rate']:.3f}")
    print(f"  Final developmental stage: {results['final_developmental_stage']}")
    print(f"  Final curiosity level: {results['final_curiosity']:.3f}")
    print(f"  Final novelty level: {results['final_novelty']:.3f}")
    
    print("\n" + "="*50 + "\n")
def example_4_configuration_management():
    """
    Example 4: Configuration management
    
    This shows how to easily configure brains and worlds from dictionaries.
    """
    print("=== Example 4: Configuration Management ===\n")
    
    print("1. Creating brain with custom configuration...")
    
    # Custom brain configuration
    brain_config = {
        "brain": {
            "neuron_count": 2000,
            "synapse_density": 0.15
        },
        "plasticity": {
            "stdp": {"enable": True, "learning_rate": 0.002}
        }
    }
    
    brain = create_agent_brain_with_config(brain_config)
    print(f"   ✓ Custom brain created: {brain.getTotalNeuronCount()} neurons")
    
    # Update configuration
    print("\n2. Updating brain configuration...")
    updates = {
        "brain": {"neuron_count": 3000},
        "neuromod": {"dopamine": {"scale": 1.5}}
    }
    
    configure_brain_from_dict(brain, updates)
    print(f"   ✓ Configuration updated")
    print(f"   New neuron count: {brain.getTotalNeuronCount()}")
    
    # World configuration
    print("\n3. Creating world from configuration...")
    world_config = {
        "width": 40,
        "height": 40,
        "visionWidth": 16,
        "visionHeight": 16,
        "agent_start": (20.0, 20.0)
    }
    
    world = create_world_from_config(world_config)
    print(f"   ✓ World created: {world.getWidth()}x{world.getHeight()}")
    print(f"   Agent start: {world.getAgentStart()}")
    
    print("\n" + "="*50 + "\n")
def example_5_state_management():
    """
    Example 5: Brain state management
    
    This shows how to save and load brain states with automatic backups.
    """
    print("=== Example 5: State Management ===\n")
    
    print("1. Creating and running simulation...")
    
    # Create brain and run some steps
    brain = create_agent_brain_with_config()
    
    for step in range(200):
        brain.step(step)
    
    print(f"   ✓ Simulation completed: {brain.getTotalSpikeCount()} spikes")
    
    # Save with timestamp
    print("\n2. Saving brain state with timestamp...")
    save_path = save_brain_with_timestamp(brain)
    print(f"   ✓ Brain saved to: {save_path}")
    
    # Run more steps
    print("\n3. Continuing simulation...")
    for step in range(200, 400):
        brain.step(step)
    
    print(f"   ✓ Simulation continued: {brain.getTotalSpikeCount()} spikes")
    
    # Save again with different name
    print("\n4. Saving again with custom filename...")
    custom_path = save_brain_with_timestamp(brain, "checkpoint_v2.bin")
    print(f"   ✓ Brain saved to: {custom_path}")\n    
    print("\n" + "="*50 + "\n")
def example_6_error_handling():
    """
    Example 6: Error handling and robustness
    
    This shows how the convenience functions handle common error cases.
    """
    print("=== Example 6: Error Handling ===\n")
    
    import pynlm
    
    print("1. Testing load with backup on failure...")
    
    # Create a brain
    brain = create_agent_brain_with_config()
    
    # Try to load non-existent file (should fail gracefully)
    try:
        load_brain_with_backup(brain, "nonexistent_file.bin", backup_if_failed=True)
    except Exception as e:
        print(f"   ✓ Expected error handled: {type(e).__name__}")
    
    print("\n2. Testing configuration validation...")
    
    # Create brain with minimal config
    minimal_config = {"brain": {"neuron_count": 100}}
    brain = create_agent_brain_with_config(minimal_config)
    
    # Get and display stats
    stats = get_brain_stats(brain)
    print(f"   ✓ Stats retrieved successfully")
    print(f"     - Neurons: {stats['neuron_count']}")
    print(f"     - Regions: {stats['regions_count']}")
    
    print("\n" + "="*50 + "\n")
def run_all_examples():
    """
    Run all examples sequentially.
    """
    print("NLM High-Level API Examples")
    print("==========================\n")
    
    examples = [
        example_1_basic_usage,
        example_2_simulation_loop,
        example_3_complete_agent_simulation,
        example_4_configuration_management,
        example_5_state_management,
        example_6_error_handling
    ]
    
    for example in examples:
        try:
            example()
        except Exception as e:
            print(f"   ✗ Error in {example.__name__}: {e}")
            print(f"\n{'='*50}\n")
    
    print("All examples completed!")
if __name__ == "__main__":
    run_all_examples()