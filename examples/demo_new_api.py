#!/usr/bin/env python3
"""
Example usage of the NLM Python API with new convenience functions
Demonstrates the improved Python bindings for NLM v0.1.0
"""

import pynlm

def example_1_basic_brain():
    """Example 1: Create a brain with optimized defaults"""
    print("Example 1: Basic brain creation")
    
    # Create a brain with optimized defaults for quick testing
    brain = pynlm.createQuickBrain()
    print(f"Created quick brain with {brain.getTotalNeuronCount()} neurons")
    
    # Initialize and run a few steps
    brain.initialize()
    
    for i in range(10):
        brain.step(i)
    
    print(f"Brain fired {brain.getTotalSpikeCount()} spikes")
    print()

def example_2_configurations():
    """Example 2: Different configurations for different simulation types"""
    print("Example 2: Configuration examples")
    
    # Training configuration
    training_config = pynlm.createTrainingConfig()
    print("Training config keys:", training_config.getKeys())
    
    # Exploration configuration  
    exploration_config = pynlm.createExplorationConfig()
    print("Exploration config keys:", exploration_config.getKeys())
    
    # Example config (minimal)
    example_config = pynlm.createExampleConfig()
    print("Example config keys:", example_config.getKeys())
    print()

def example_3_config_validation():
    """Example 3: Configuration validation"""
    print("Example 3: Configuration validation")
    
    # Create a config with potential issues
    config = pynlm.createExampleConfig()
    
    # Add some problematic values to demonstrate validation
    config.set("neuron_count", 50, pynlm.ConfigSource.Runtime)  # Too low
    config.set("region_count", 100, pynlm.ConfigSource.Runtime)  # Too high (exceeds neurons)
    
    # Validate the config
    issues, warnings, suggestions = pynlm.validateConfig(config)
    
    print("Validation results:")
    print("Issues:", issues)
    print("Warnings:", warnings)
    print("Suggestions:", suggestions)
    print()

def example_4_simulation_helpers():
    """Example 4: Using simulation helper functions"""
    print("Example 4: Simulation helper functions")
    
    # Create complete setup
    brain = pynlm.createQuickBrain()
    world = pynlm.createQuickWorld()
    agent = pynlm.createAgentSetup(brain, world)
    
    print("Brain ready:", pynlm.isBrainReady(brain))
    print("World configured:", world.getWidth(), "x", world.getHeight())
    
    # Get brain statistics
    stats = pynlm.getBrainStats(brain)
    print("Brain stats:", stats)
    print()

def example_5_training_episode():
    """Example 5: Complete training episode"""
    print("Example 5: Training episode")
    
    # Create complete setup
    brain = pynlm.createQuickBrain()
    world = pynlm.createQuickWorld()
    agent = pynlm.createAgentSetup(brain, world)
    
    # Run a training episode
    neuron_count, avg_rate, total_reward = pynlm.runTrainingEpisode(
        brain, world, agent, steps=100, enable_learning=True, verbose=True
    )
    
    print(f"Training completed:")
    print(f"  - Neuron count: {neuron_count}")
    print(f"  - Average firing rate: {avg_rate:.4f}")
    print(f"  - Total reward: {total_reward:.4f}")
    print()

def example_6_checkpoints():
    """Example 6: Checkpoint functionality"""
    print("Example 6: Checkpoint functionality")
    
    brain = pynlm.createQuickBrain()
    brain.initialize()
    
    # Run a few steps
    for i in range(50):
        brain.step(i)
    
    # Save checkpoint
    checkpoint_file = "/tmp/brain_checkpoint.bin"
    pynlm.saveBrainCheckpoint(brain, checkpoint_file)
    print(f"Saved brain checkpoint to {checkpoint_file}")
    
    # Create new brain and load checkpoint
    new_brain = pynlm.createQuickBrain()
    pynlm.loadBrainCheckpoint(new_brain, checkpoint_file)
    print(f"Loaded brain with {new_brain.getTotalNeuronCount()} neurons")
    print()

def main():
    """Run all examples"""
    print("=" * 60)
    print("NLM Python API Examples - v0.1.0")
    print("=" * 60)
    print()
    
    try:
        example_1_basic_brain()
        example_2_configurations()
        example_3_config_validation()
        example_4_simulation_helpers()
        example_5_training_episode()
        example_6_checkpoints()
        
        print("=" * 60)
        print("All examples completed successfully!")
        print("=" * 60)
        
    except Exception as e:
        print(f"Error running examples: {e}")
        print("Note: This may be due to the C++ module not being built.")
        print("The Python API structure is correctly defined in bindings.cpp")

if __name__ == "__main__":
    main()