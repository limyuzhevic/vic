#!/usr/bin/env python3
"""
Checkpointing example: Save and load brain states

This example demonstrates the checkpointing functionality
for saving and loading brain states with metadata.
"""

import time
import os
from nlm import (
    Brain, Config, SimpleWorld, create_default_config,
    save_brain_checkpoint, load_brain_checkpoint,
    get_brain_statistics, SimulationLoop
)

def create_test_brain_with_stats():
    """Create a test brain with known characteristics"""
    config = create_default_config()
    brain = Brain(config)
    world = SimpleWorld()
    world.configure(640, 480, 320, 240)
    
    # Initialize brain
    brain.initialize()
    
    return brain, world

def demonstrate_checkpointing():
    """Demonstrate save and load functionality"""
    print("=== Brain Checkpointing Example ===\n")
    
    # Create initial brain
    print("1. Creating initial brain...")
    brain1, world1 = create_test_brain_with_stats()
    loop1 = SimulationLoop(brain1, world1)
    
    # Run some simulation steps
    print("2. Running initial simulation...")
    initial_stats = loop1.run_episode(10)
    print(f"   Steps taken: {len(initial_stats)}")
    
    # Get initial brain statistics
    initial_brain_stats = get_brain_statistics(brain1)
    print(f"   Initial neurons: {initial_brain_stats['neuron_count']['total']}")
    print(f"   Initial synapses: {initial_brain_stats['synapse_count']['total']}")
    
    # Save checkpoint
    print("\n3. Saving checkpoint...")
    checkpoint_file = "brain_checkpoint.json"
    
    metadata = {
        'checkpoint_id': 1,
        'creation_time': time.time(),
        'simulation_steps': len(initial_stats),
        'creator': 'nlm_demo'
    }
    
    save_success = save_brain_checkpoint(brain1, checkpoint_file, metadata)
    if save_success:
        print(f"   ✓ Checkpoint saved to {checkpoint_file}")
    else:
        print(f"   ✗ Failed to save checkpoint")
        return
    
    # Create new brain and load checkpoint
    print("\n4. Creating new brain and loading checkpoint...")
    brain2, world2 = create_test_brain_with_stats()
    loop2 = SimulationLoop(brain2, world2)
    
    load_success = load_brain_checkpoint(brain2, checkpoint_file)
    if load_success:
        print(f"   ✓ Checkpoint loaded successfully")
    else:
        print(f"   ✗ Failed to load checkpoint")
        return
    
    # Verify loaded brain state
    print("\n5. Verifying loaded brain state...")
    loaded_brain_stats = get_brain_statistics(brain2)
    print(f"   Loaded neurons: {loaded_brain_stats['neuron_count']['total']}")
    print(f"   Loaded synapses: {loaded_brain_stats['synapse_count']['total']}")
    
    # Compare statistics
    neurons_match = initial_brain_stats['neuron_count']['total'] == loaded_brain_stats['neuron_count']['total']
    synapses_match = initial_brain_stats['synapse_count']['total'] == loaded_brain_stats['synapse_count']['total']
    
    print(f"\n6. Comparison:")
    print(f"   Neuron count matches: {neurons_match}")
    print(f"   Synapse count matches: {synapses_match}")
    
    if neurons_match and synapses_match:
        print("   ✓ Checkpoint verification successful!")
    else:
        print("   ⚠ Some differences found (expected due to simulation)")
    
    # Run additional steps with loaded brain
    print("\n7. Running additional steps with loaded brain...")
    additional_actions = loop2.run_episode(5)
    print(f"   Additional steps: {len(additional_actions)}")
    
    # Clean up
    print("\n8. Cleaning up...")
    if os.path.exists(checkpoint_file):
        os.remove(checkpoint_file)
        print(f"   ✓ Removed checkpoint file {checkpoint_file}")
    
    print("\n=== Checkpointing Example Complete ===")

def demonstrate_advanced_checkpointing():
    """Demonstrate advanced checkpointing with multiple versions"""
    print("\n\n=== Advanced Checkpointing Example ===\n")
    
    # Create brain
    brain, world = create_test_brain_with_stats()
    loop = SimulationLoop(brain, world)
    
    # Run some steps
    print("1. Running initial simulation...")
    loop.run_episode(5)
    
    # Save multiple checkpoints with different metadata
    checkpoints = []
    
    # Save intermediate checkpoint
    metadata1 = {
        'checkpoint_id': 1,
        'type': 'intermediate',
        'steps': 5,
        'timestamp': time.time()
    }
    save_brain_checkpoint(brain, 'checkpoint1.json', metadata1)
    checkpoints.append('checkpoint1.json')
    print("   ✓ Saved intermediate checkpoint")
    
    # Run more steps
    print("2. Running additional steps...")
    loop.run_episode(3)
    
    # Save another checkpoint
    metadata2 = {
        'checkpoint_id': 2,
        'type': 'milestone',
        'steps': 8,
        'score': 'high_performance'
    }
    save_brain_checkpoint(brain, 'checkpoint2.json', metadata2)
    checkpoints.append('checkpoint2.json')
    print("   ✓ Saved milestone checkpoint")
    
    # Load different checkpoints
    print("3. Loading different checkpoints...")
    
    # Create new brains
    brain1, world1 = create_test_brain_with_stats()
    brain2, world2 = create_test_brain_with_stats()
    
    # Load checkpoint 1
    if load_brain_checkpoint(brain1, 'checkpoint1.json'):
        print("   ✓ Loaded checkpoint 1")
        stats1 = get_brain_statistics(brain1)
        print(f"     Neurons: {stats1['neuron_count']['total']}")
    
    # Load checkpoint 2
    if load_brain_checkpoint(brain2, 'checkpoint2.json'):
        print("   ✓ Loaded checkpoint 2")
        stats2 = get_brain_statistics(brain2)
        print(f"     Neurons: {stats2['neuron_count']['total']}")
    
    # Clean up
    print("\n4. Cleaning up checkpoints...")
    for checkpoint in checkpoints:
        if os.path.exists(checkpoint):
            os.remove(checkpoint)
            print(f"   ✓ Removed {checkpoint}")
    
    print("\n=== Advanced Checkpointing Example Complete ===")

if __name__ == "__main__":
    try:
        demonstrate_checkpointing()
        demonstrate_advanced_checkpointing()
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
