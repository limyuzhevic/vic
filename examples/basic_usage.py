#!/usr/bin/env python3
"""
Example usage of NLM Python bindings for beginners.
Demonstrates common use cases and best practices.
"""

from pynlm import (
    # Core types
    NeuronId, SynapseId, RegionId, PopulationId,
    
    # Enums
    NeuronType, SynapseType, DevelopmentalStage, ActionType,
    
    # Main classes
    Config, Brain, SimpleWorld, AgentBrain,
    
    # Sensory and motor
    Vision, Audio, Action, SensoryPercept,
    
    # World objects
    WorldObject, WorldObjectType,
    
    # Results
    ActionResult, AgentBody,
    
    # Factory functions
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    
    # Constants
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID
)

# Simple experiment: Create a basic brain with vision input

def create_simple_vision_brain(width=64, height=64):
    """Create a simple brain configured for vision processing."""
    # Create default configuration
    config = createDefaultConfig()
    
    # Configure for vision processing
    config.set("brain.vision.width", width)
    config.set("brain.vision.height", height)
    config.set("brain.vision.channels", 3)
    config.set("brain.neuron.count", 100)
    
    # Create brain
    brain = createBrain(config)
    return brain

# Advanced usage example
def run_simulation_example():
    """Example of running a simple simulation with sensory input."""
    # Create world
    world = createSimpleWorld()
    world.configure(128, 128, 64, 64)  # width, height, visionWidth, visionHeight
    
    # Set agent starting position
    world.setAgentStart(64, 64)
    
    # Create brain for the agent
    brain_config = createDefaultConfig()
    brain = createBrain(brain_config)
    
    # Create agent brain
    agent_brain = createAgentBrain(brain)
    
    # Initialize everything
    agent_brain.initialize(world)
    
    # Simulation loop
    for step in range(100):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent_brain.processSensoryInput(percept)
        
        # Step world
        world.update(0.01)  # 10ms timestep
        
        # Occasionally print status
        if step % 10 == 0:
            print(f"Step {step}: "
                  f"Position: ({world.getAgentBody().x}, {world.getAgentBody().y}), "
                  f"Energy: {world.getAgentBody().energy:.2f}")
    
    print("Simulation complete!")

# Utility functions for common operations
def get_active_neurons(brain):
    """Get IDs of all currently active neurons."""
    return [neuron.getId() for neuron in brain.getActiveNeurons()]

def create_vision_input(width, height, channels=3):
    """Create a Vision sensory input from numpy array or list."""
    # This would typically take image data and convert to Vision object
    # Implementation depends on how Vision is exposed in bindings
    pass

if __name__ == "__main__":
    print("NLM Python Bindings Example")
    print("==========================")
    print("\nRunning simple vision brain example...")
    brain = create_simple_vision_brain(32, 32)
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    print("\nRunning simulation example...")
    run_simulation_example()
