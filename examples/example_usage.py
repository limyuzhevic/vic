#!/usr/bin/env python3
"""
Example scripts for NLM Python bindings

This file contains example scripts demonstrating how to use the NLM Python bindings
to create and work with neural learning machines.
"""

import pynlm
import json
import time

def basic_example():
    """
    Basic example showing how to create and work with a simple brain.
    """
    print("=== Basic Brain Example ===")
    
    # Create a brain with default settings
    brain = pynlm.createBrainWithDefaults()
    
    # Print brain statistics
    stats = pynlm.getBrainStats(brain)
    print("Brain Statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
    
    # Run a learning experiment
    print("\nRunning learning experiment...")
    result = pynlm.runLearningExperiment(brain, steps=50)
    
    print(f"Learning Result:")
    print(f"  Initial firing rate: {result.initial_firing_rate:.4f}")
    print(f"  Final firing rate: {result.final_firing_rate:.4f}")
    print(f"  Learning progress: {result.learning_progress:.4f}")
    print(f"  Learning detected: {result.learning_detected}")
    
    print("=== Basic Example Complete ===\n")

def world_example():
    """
    Example showing how to create a world and interact with it.
    """
    print("=== World Example ===")
    
    # Create a brain suitable for agent control
    brain = pynlm.createAgentBrain()
    
    # Create a world
    world = pynlm.createWorld(width=20, height=20, 
                              vision_width=7, vision_height=7)
    
    # Create an agent brain with the world
    agent_brain = pynlm.createAgentBrainWithWorld(brain, world)
    
    # Print brain status
    status = pynlm.getBrainStatus(brain)
    print("Brain Status:")
    for key, value in status.items():
        if isinstance(value, dict):
            print(f"  {key}:")
            for sub_key, sub_value in value.items():
                print(f"    {sub_key}: {sub_value}")
        else:
            print(f"  {key}: {value}")
    
    print("=== World Example Complete ===\n")

def config_example():
    """
    Example showing how to work with configuration.
    """
    print("=== Configuration Example ===")
    
    # Create a default config
    config = pynlm.createDefaultConfig()
    
    # Set some custom values
    config.set("neuron_count", 500)
    config.set("region_count", 2)
    config.set("connection_probability", 0.15)
    config.set("plasticity_learning_rate", 0.01)
    
    # Create a brain with the custom config
    brain = pynlm.createBrain(config)
    
    # Save config to file
    config.saveToFile("example_config.json")
    print("Configuration saved to example_config.json")
    
    # Load config from file
    config2 = pynlm.createDefaultConfig()
    config2.loadFromFile("example_config.json")
    
    # Verify loaded values
    print("Loaded configuration:")
    print(f"  neuron_count: {config2.getOr('neuron_count', 0)}")
    print(f"  region_count: {config2.getOr('region_count', 0)}")
    print(f"  connection_probability: {config2.getOr('connection_probability', 0.0)}")
    
    print("=== Configuration Example Complete ===\n")

def simulation_example():
    """
    Example showing how to run a complete simulation.
    """
    print("=== Simulation Example ===")
    
    # Create brain and world
    brain = pynlm.createAgentBrain()
    world = pynlm.createWorld(width=15, height=15)
    
    # Create agent brain
    agent_brain = pynlm.createAgentBrainWithWorld(brain, world)
    
    # Run multiple simulation steps
    print("Running simulation steps...")
    for step in range(10):
        # Get sensory percept from world
        percept = world.getSensoryPercept(agent_brain.getBrain().getAgentBody())
        
        # Process sensory input in agent brain
        agent_brain.processSensoryInput(percept)
        
        # Step the brain
        brain.step(step)
        
        # Get motor command
        motor_cmd = agent_brain.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, step)
        
        # Get reward
        reward = world.computeReward(agent_brain.getBrain().getAgentBody())
        
        # Apply reward modulation
        agent_brain.applyRewardModulation(reward, 0.0)  # Simplified for example
        
        if step % 5 == 0:
            status = pynlm.getBrainStatus(brain)
            print(f"  Step {step}: firing_rate = {status.get('average_firing_rate', 0.0):.4f}, reward = {reward}")
    
    # Print final brain status
    print("\nFinal Brain Status:")
    status = pynlm.getBrainStatus(brain)
    for key, value in status.items():
        if isinstance(value, dict):
            print(f"  {key}:")
            for sub_key, sub_value in value.items():
                print(f"    {sub_key}: {sub_value}")
        else:
            print(f"  {key}: {value}")
    
    print("=== Simulation Example Complete ===\n")

def learning_example():
    """
    Example showing how to run learning experiments.
    """
    print("=== Learning Example ===")
    
    # Create a brain with custom settings for learning
    brain = pynlm.createBrainCustom(neuron_count=300, connection_prob=0.05)
    
    # Print initial status
    print("Initial brain status:")
    status = pynlm.getBrainStatus(brain)
    print(f"  neurons: {status.get('total_neurons', 0)}")
    print(f"  synapses: {status.get('total_synapses', 0)}")
    
    # Run learning experiment
    print("\nRunning learning experiment...")
    result = pynlm.runLearningExperiment(brain, steps=100)
    
    # Print learning results
    print(f"Learning Results:")
    print(f"  Initial firing rate: {result.initial_firing_rate:.4f}")
    print(f"  Final firing rate: {result.final_firing_rate:.4f}")
    print(f"  Learning progress: {result.learning_progress:.4f}")
    print(f"  Learning detected: {result.learning_detected}")
    print(f"  Number of synapses: {len(result.final_weights)}")
    
    if result.final_weights:
        print(f"  Average final weight: {sum(result.final_weights)/len(result.final_weights):.4f}")
        print(f"  Min final weight: {min(result.final_weights):.4f}")
        print(f"  Max final weight: {max(result.final_weights):.4f}")
    
    print("=== Learning Example Complete ===\n")

def main():
    """
    Run all examples.
    """
    print("NLM Python Bindings Examples\n")
    print("This script demonstrates how to use the NLM Python bindings\n")
    
    try:
        basic_example()
        config_example()
        world_example()
        simulation_example()
        learning_example()
        
        print("All examples completed successfully!")
        
    except Exception as e:
        print(f"Error running examples: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()