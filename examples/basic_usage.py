#!/usr/bin/env python3
"""
NLM Python Usage Examples
========================

This file demonstrates basic usage of the NLM (Neural Learning Machine) Python bindings.
It's designed to be easy to understand for newcomers while providing useful examples
for more advanced users.
"""

import sys
import os
from pathlib import Path

# Add the build directory to Python path
sys.path.insert(0, str(Path(__file__).parent / "build"))

try:
    import pynlm
    print("NLM Python Bindings")
    print("=" * 50)
except ImportError as e:
    print(f"Error: Could not import pynlm: {e}")
    print("Please build the bindings first:")
    print("  pip install -e .")
    sys.exit(1)

def basic_brain_example():
    """Create and configure a basic brain."""
    print("\n1. Basic Brain Example")
    print("-" * 30)
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Create a brain with this configuration
    brain = pynlm.createBrain(config)
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain regions: {brain.getRegionCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Initialize the brain
    brain.initialize()
    print("Brain initialized")
    
    return brain

def simple_world_example():
    """Create and use a simple world environment."""
    print("\n2. Simple World Example")
    print("-" * 30)
    
    # Create a world
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 84, 84)  # width, height, visionWidth, visionHeight
    
    # Set agent starting position
    world.setAgentStart(5.0, 5.0)
    
    # Get agent body
    agent = world.getAgentBody()
    print(f"Agent position: ({agent.x}, {agent.y})")
    print(f"Agent orientation: {agent.orientation} degrees")
    
    # Add some resources to the world
    resource = pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Resource, 100.0)
    world.addObject(resource)
    
    print(f"World size: {world.getWidth()} x {world.getHeight()}")
    print(f"Agent energy: {agent.energy}")
    
    return world

def agent_brain_example(world):
    """Create and use an agent brain."""
    print("\n3. Agent Brain Example")
    print("-" * 30)
    
    # Create a brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Create an agent brain that interfaces with the brain and world
    agent_brain = pynlm.createAgentBrain(brain)
    
    # Initialize with the world
    agent_brain.initialize(world)
    
    # Create a sensory percept
    percept = pynlm.SensoryPercept()
    
    # Get vision from the world
    vision = world.getSensoryPercept().getVision()
    print(f"Vision dimensions: {vision.getWidth()} x {vision.getHeight()} x {vision.getChannels()}")
    
    # Process sensory input
    agent_brain.processSensoryInput(percept)
    
    # Get motor command
    action = agent_brain.decodeMotorCommand()
    print(f"Decoded motor command: {action.getType()}")
    if action.getType() == pynlm.ActionType.Custom:
        print(f"Action parameters: {action.getParameters()}")
    
    return agent_brain

def config_example():
    """Demonstrate configuration management."""
    print("\n4. Configuration Example")
    print("-" * 30)
    
    config = pynlm.createDefaultConfig()
    
    # Check current configuration
    print("Default configuration:")
    summary = config.summary()
    # Show first few lines of summary
    lines = summary.split('\n')[:5]
    for line in lines:
        print(f"  {line}")
    
    # Modify configuration
    config.set("neuron_count", 500)
    config.set("connection_probability", 0.15)
    
    print(f"\nModified neuron_count: {config.get('neuron_count')}")
    print(f"Modified connection_probability: {config.get('connection_probability')}")
    
    # Save configuration
    import tempfile
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        temp_file = f.name
    
    try:
        if config.saveToFile(temp_file):
            print(f"Configuration saved to: {temp_file}")
    finally:
        if os.path.exists(temp_file):
            os.unlink(temp_file)
    
    return config

def world_interaction_example(world, brain):
    """Example of agent interacting with the world."""
    print("\n5. World Interaction Example")
    print("-" * 30)
    
    # Update world simulation
    world.update(0.001)  # timestep
    
    # Get agent percept and brain status
    agent = world.getAgentBody()
    print(f"Agent position: ({agent.x:.1f}, {agent.y:.1f})")
    print(f"Agent energy: {agent.energy:.1f}")
    print(f"Agent age: {agent.age}")
    
    # Create an action (move forward)
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"Agent moved to: ({agent.x:.1f}, {agent.y:.1f})")
    
    return brain

def developmental_example():
    """Demonstrate developmental features."""
    print("\n6. Developmental Example")
    print("-" * 30)
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable development features
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.enableDevelopment(True)
    agent_brain.enableCuriosity(True)
    agent_brain.enableRewardModulation(True)
    
    print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")
    
    # Simulate development over time
    for age_step in range(1, 6):
        agent_brain.updateDevelopment(0.001)  # timestep
        print(f"Stage {age_step}: {brain.getDevelopmentalStage()}")
    
    print(f"Final neuromodulation level: {agent_brain.getNeuromodulationLevel():.3f}")
    print(f"Final curiosity level: {agent_brain.getCuriosityLevel():.3f}")

def main():
    """Run all examples."""
    print("\n" + "=" * 60)
    print("NLM Python Bindings Examples")
    print("=" * 60)
    
    try:
        # Run examples
        brain = basic_brain_example()
        world = simple_world_example()
        agent_brain = agent_brain_example(world)
        config = config_example()
        world_interaction_example(world, brain)
        developmental_example()
        
        print("\n" + "=" * 60)
        print("All examples completed successfully!")
        print("=" * 60)
        
    except Exception as e:
        print(f"\nError in example: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()