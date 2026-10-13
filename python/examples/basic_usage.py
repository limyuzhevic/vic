"""NLM Python Examples and Tutorials
This file contains examples and tutorials for using the NLM Python bindings.

The NLM Python bindings provide access to the Neural Learning Machine (NLM)
simulation framework, allowing you to create, configure, and run neural simulations
from Python.
"""

import pynlm
import numpy as np
import json
import time
from typing import List, Dict, Optional

print("NLM Python Examples and Tutorials")
print("=" * 50)

def basic_example():
    """Basic example: Creating a simple brain configuration."""
    print("\n1. Basic Example: Creating a Brain")
    print("-" * 40)
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    
    # Modify configuration
    config.set("neuron_count", 1000)
    config.set("region_count", 2)
    config.set("connection_probability", 0.1)
    config.set("random_seed", 42)
    
    print("Configuration created:")
    print(f"  - Neurons: {config.getOr('neuron_count', 0)}")
    print(f"  - Regions: {config.getOr('region_count', 0)}")
    print(f"  - Connection probability: {config.getOr('connection_probability', 0.0)}")
    print(f"  - Random seed: {config.getOr('random_seed', 0)}")
    
    return config

def brain_example():
    """Example: Creating and initializing a brain."""
    print("\n2. Brain Example: Creating and Initializing a Brain")
    print("-" * 50)
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("region_count", 1)
    
    # Create brain
    brain = pynlm.createBrain(config)
    
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain has {brain.getTotalSynapseCount()} synapses")
    print(f"Brain status: {'Active' if brain.getTotalSpikeCount() > 0 else 'Ready'}")
    
    # Initialize brain
    success = brain.initialize()
    if success:
        print("Brain initialized successfully")
        brain.logStatus()
    else:
        print("Failed to initialize brain")
    
    return brain

def simulation_example():
    """Example: Running a simulation."""
    print("\n3. Simulation Example: Running Neural Simulation")
    print("-" * 50)
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 200)
    config.set("region_count", 1)
    config.set("simulation_timestep", 0.001)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Starting simulation with {brain.getTotalNeuronCount()} neurons")
    print("Running 1000 simulation steps...")
    
    # Run simulation
    start_time = time.time()
    for step in range(1000):
        brain.step(step, step * 0.001)
        
        # Inject sensory input every 100 steps
        if step % 100 == 0:
            # Create some sensory input
            input_data = np.random.randn(10).tolist()
            sensory_input = pynlm.SensoryInput()
            sensory_input.setData(input_data)
            brain.receiveSensoryInput(sensory_input)
    
    end_time = time.time()
    
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    return brain

def agent_example():
    """Example: Using agent brain interface."""
    print("\n4. Agent Example: Agent Brain Interface")
    print("-" * 45)
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 300)
    config.set("region_count", 1)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    print(f"Agent created with sensory input size: {agent.getSensoryInputSize()}")
    print(f"Agent has motor output size: {agent.getMotorOutputSize()}")
    
    # Create a simple world
    world = pynlm.createSimpleWorld()
    world.configure(20.0, 20.0, 16, 16)
    
    # Initialize agent with world
    agent.initialize(world)
    
    print("Agent initialized with world")
    print(f"World dimensions: {world.getWidth()} x {world.getHeight()}")
    
    # Run a simple agent episode
    print("\nRunning agent episode...")
    for step in range(50):
        # Get sensory percept from world
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Simulate brain
        brain.step(step, step * 0.01)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        print(f"Step {step}: Motor command = {motor_cmd}")
        
        # Apply reward modulation
        agent.applyRewardModulation(1.0, 0.5)
        
        # Update world
        world.update(0.01)
    
    
    print("Agent episode completed")
    return brain, agent, world

def config_example():
    """Example: Working with configurations."""
    print("\n5. Configuration Example: Working with Config Objects")
    print("-" * 45)
    
    # Create configuration from file
    config = pynlm.Config()
    config.loadFromFile("configs/default.cfg")
    
    # Add custom configuration values
    config.set("custom_parameter_1", 1.5)
    config.set("custom_parameter_2", "test_value")
    
    print("Configuration keys:")
    for key in config.getKeys():
        print(f"  - {key}")
    
    print(f"\nConfiguration summary: {config.summary()}")
    
    return config

def memory_example():
    """Example: Working with memory systems."""
    print("\n6. Memory Example: Exploring Memory Systems")
    print("-" * 35)
    
    # This example demonstrates how to work with different memory systems
    # Note: In a real implementation, you would need to enable specific memory components
    
    print("Memory system examples would show:")
    print("  - Working memory capacity and usage")
    print("  - Episodic memory encoding and retrieval")
    print("  - Semantic memory pattern matching")
    print("  - Procedural memory learning progress")
    print("  - Associative memory connections")
    
    # Example code would be included here for demonstration
    print("\nNote: Full memory system examples require Phase 4 integration.")
    
    return None
def error_handling_example():
    """Example: Error handling and validation."""
    print("\n7. Error Handling Example: Robust Error Management")
    print("-" * 40)
    
    try:
        # Example of creating and using configuration
        config = pynlm.Config()
        
        # Try to load a non-existent file
        config.loadFromFile("non_existent_file.cfg")
        print("Successfully loaded configuration (fallback to defaults)")
        
        # Validate configuration
        config.validate()
        print("Configuration validated successfully")
        
    except Exception as e:
        print(f"Error caught: {e}")
    
    return config
def quickstart_guide():
    """Quick start guide for NLM Python users."""
    print("\nQuick Start Guide for NLM Python Users")
    print("=" * 50)
    
    print("\nGetting Started:")
    print("1. Import the library: import pynlm")
    print("2. Create configuration: config = pynlm.createDefaultConfig()")
    print("3. Configure brain: config.set('neuron_count', 100)")
    print("4. Create brain: brain = pynlm.createBrain(config)")
    print("5. Initialize: brain.initialize()")
    print("6. Run simulation: brain.step(step, time)")
    
    print("\nCommon Configuration Parameters:")
    print("  - neuron_count: Number of neurons in the brain")
    print("  - region_count: Number of brain regions")
    print("  - connection_probability: Probability of synaptic connections")
    print("  - random_seed: Seed for random number generator")
    print("  - simulation_timestep: Time step for simulation")
    
    print("\nCommon Brain Methods:")
    print("  - getTotalNeuronCount(): Get total number of neurons")
    print("  - getTotalSynapseCount(): Get total number of synapses")
    print("  - getTotalSpikeCount(): Get total spikes generated")
    print("  - getAverageFiringRate(): Get average firing rate (Hz)")
    print("  - step(step, time): Advance simulation by one step")
    print("  - receiveSensoryInput(input): Inject sensory input")
    
    print("\nFor more examples and documentation, visit:")
    print("  https://github.com/neural-learning-machine/nlm")

if __name__ == "__main__":
    print("Running NLM Python Examples")
    print("=" * 50)
    
    # Run all examples
    basic_example = basic_example()
    brain_example = brain_example()
    simulation_example = simulation_example()
    config_example = config_example()
    agent_example = agent_example()
    
    # Show quick start guide
    quickstart_guide()
    
    print("\n" + "=" * 50)
    print("All examples completed successfully!")
    print("For more complex examples, refer to the documentation.")
    print("=" * 50)