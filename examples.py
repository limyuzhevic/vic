#!/usr/bin/env python3
"""
Example scripts demonstrating NLM Python API usage.

This file provides comprehensive examples showing how to use the NLM Python bindings
to create and simulate artificial brains with neural networks, memory systems,
neuromodulation, and environmental interaction.
"""

import pynlm
import numpy as np

def minimal_example():
    """The simplest possible example - just create and run a brain."""
    print("=== Minimal Example ===")
    
    # One line to create a virtual brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Initialize it
    brain.initialize()
    
    # Make it active
    brain.step(0)
    
    print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
    print()
def basic_usage_example():
    """Basic usage example from HOW_TO_USE.md."""
    print("=== Basic Usage Example ===")
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    
    # Optionally configure specific parameters
    # config.set("brain.neuron_count", 1000)
    # config.set("brain.synapse_density", 0.1)
    
    # Create and initialize the brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run simulation for 1000 steps
    for step in range(1000):
        brain.step(step)
        
        # Log statistics periodically
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print()
def complete_agent_example():
    """Complete agent example from HOW_TO_USE.md."""
    print("=== Complete Agent Example ===")
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation loop
    for step in range(100):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Print progress
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    print("Agent simulation complete!")
    print()
def agent_watching_world_example():
    """Agent watching a world example from easy_usage.md."""
    print("=== Agent Watching World Example ===")
    
    # Setup
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Watch the world for 30 steps
    for i in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
    
    print("Watched world for 30 steps")
    print(f"Firing rate: {brain.getAverageFiringRate():.2f}")
    print()
def world_interaction_example():
    """Example showing world-object interaction."""
    print("=== World-Object Interaction Example ===")
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    
    # Add some objects to the world
    from pynlm import WorldObject, WorldObjectType
    
    # Add a resource
    resource = WorldObject(x=3.0, y=3.0, 
                          obj_type=WorldObjectType.RESOURCE, 
                          value=1.0)
    world.addObject(resource)
    
    # Add a hazard
    hazard = WorldObject(x=7.0, y=7.0,
                         obj_type=WorldObjectType.HAZARD,
                         value=0.5)
    world.addObject(hazard)
    
    # Add a wall
    wall = WorldObject(x=5.0, y=5.0,
                       obj_type=WorldObjectType.WALL,
                       value=0.0)
    world.addObject(wall)
    
    # Check if positions are valid
    print(f"Position (2,2) valid: {world.isValidPosition(2.0, 2.0)}")
    print(f"Position (5,5) valid: {world.isValidPosition(5.0, 5.0)}")  # Should be False (wall)
    
    # Get world properties
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    print(f"Max energy: {world.getMaxEnergy()}")
    
    # Update world
    world.update(0.1)
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    print(f"Vision data size: {len(percept.getVision())}")
    print(f"Touch data size: {len(percept.getTouch())}")
    
    print()
def configuration_example():
    """Example showing configuration operations."""
    print("=== Configuration Example ===")
    
    config = pynlm.createDefaultConfig()
    
    # Check if configuration has a key
    print(f"Has 'brain.neuron_count': {config.has('brain.neuron_count')}")
    
    # Get all keys
    keys = config.getKeys()
    print(f"Configuration keys: {keys}")
    
    # Modify configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    
    # Get configuration values
    neuron_count = config.getOr("brain.neuron_count", 1000)
    print(f"Neuron count: {neuron_count}")
    
    # Check configuration summary
    print("Configuration summary:")
    print(config.summary())
    
    # Save and load configuration
    try:
        config.saveToFile("test_config.json")
        print("Configuration saved to test_config.json")
        
        new_config = pynlm.createDefaultConfig()
        new_config.loadFromFile("test_config.json")
        print("Configuration loaded successfully")
    except Exception as e:
        print(f"Configuration file operations failed (expected in this demo): {e}")
    
    print()
def enum_examples():
    """Examples showing enumeration usage."""
    print("=== Enumeration Examples ===")
    
    print("Neuron types:")
    print(f"  Excitatory: {pynlm.NeuronType.EXCITATORY}")
    print(f"  Inhibitory: {pynlm.NeuronType.INHIBITORY}")
    print(f"  Sensory: {pynlm.NeuronType.SENSORY}")
    print(f"  Motor: {pynlm.NeuronType.MOTOR}")
    print(f"  Internal: {pynlm.NeuronType.INTERNAL}")
    
    print("\nDevelopmental stages:")
    print(f"  Initial: {pynlm.DevelopmentalStage.INITIAL}")
    print(f"  CriticalPeriod: {pynlm.DevelopmentalStage.CRITICALPERIOD}")
    print(f"  Maturation: {pynlm.DevelopmentalStage.MATURATION}")
    print(f"  Adult: {pynlm.DevelopmentalStage.ADULT}")
    print(f"  Aging: {pynlm.DevelopmentalStage.AGING}")
    
    print("\nAction types:")
    print(f"  MoveForward: {pynlm.ActionType.MOVEFORWARD}")
    print(f"  TurnLeft: {pynlm.ActionType.TURNLEFT}")
    print(f"  Interact: {pynlm.ActionType.INTERACT}")
    print(f"  Eat: {pynlm.ActionType.EAT}")
    
    print("\nWorld object types:")
    print(f"  Empty: {pynlm.WorldObjectType.EMPTY}")
    print(f"  Resource: {pynlm.WorldObjectType.RESOURCE}")
    print(f"  Hazard: {pynlm.WorldObjectType.HAZARD}")
    print(f"  Wall: {pynlm.WorldObjectType.WALL}")
    
    print()
def main():
    """Run all example scripts."""
    print("NLM Python Bindings - Example Scripts")
    print("=" * 50)
    print()
    
    # Run all examples
    minimal_example()
    basic_usage_example()
    complete_agent_example()
    agent_watching_world_example()
    world_interaction_example()
    configuration_example()
    enum_examples()
    
    print("All examples completed successfully!")
    print()
    print("To use NLM in your own projects:")
    print("1. Import pynlm")
    print("2. Create brain: pynlm.createBrain(pynlm.createDefaultConfig())")
    print("3. Initialize: brain.initialize()")
    print("4. Run simulation: for step in range(num_steps): brain.step(step)")
    print("5. Use agent interface for world interaction")

if __name__ == "__main__":
    main()