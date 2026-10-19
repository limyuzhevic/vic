#!/usr/bin/env python3
"""Example demonstrating improved NLM Python API"""

import pynlm

def example_quick_start():
    """Quick start example - minimal code"""
    print("=== Quick Start Example ===")
    
    # One line to create brain
    brain = pynlm.quickBrain()
    brain.initialize()
    
    # Run simulation
    brain.runSimulation(100)
    
    print(f"Simulation complete: {brain.getActiveNeurons()} neurons firing")
    print()

def example_with_world():
    """Example with agent and world interaction"""
    print("=== Agent-World Example ===")
    
    # Create brain
    brain = pynlm.quickBrain()
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning features
    agent.enableAllDevelopment()
    
    # Run episode
    for step in range(50):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processInput(percept)
        
        action = agent.makeAction()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Brain learns from experience
        # (internal learning happens automatically)
    
    print(f"Episode complete: {brain.getStatistics()}")
    print()

def example_advanced_configuration():
    """Advanced configuration example"""
    print("=== Advanced Configuration Example ===")
    
    # Custom configuration
    config = pynlm.Config(
        neuron_count=1000,
        region_count=2,
        connection_probability=0.15,
        learning_rate=0.001,
        enable_development=True,
        enable_curiosity=True,
        enable_reward_modulation=True
    )
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get statistics
    stats = brain.getStatistics()
    print(f"Brain Configuration: {stats}")
    print()

def example_sensory_and_action():
    """Example creating sensory inputs and actions"""
    print("=== Sensory and Action Example ===")
    
    # Create vision data
    vision = pynlm.Vision.create(8, 8, 3)
    data = [0.1, 0.5, 0.9] * 12  # 8*8*3 = 192 elements
    vision.setData(data)
    
    print(f"Vision created: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
    
    # Create audio
    audio = pynlm.Audio.create(44100, 1024)
    audio_samples = [0.0] * 1024
    audio.setData(audio_samples)
    
    # Create actions
    move_forward = pynlm.Action.createMove("forward", 1.0)
    interact = pynlm.Action.create("interact")
    rest = pynlm.Action.create("rest")
    
    print(f"Actions created: forward, interact, rest")
    print()

def example_development_stages():
    """Example controlling development stages"""
    print("=== Development Stages Example ===")
    
    brain = pynlm.quickBrain()
    brain.initialize()
    
    # Get current development stage
    stage = brain.getDevelopmentalStage()
    print(f"Initial development stage: {stage}")
    
    # Advance development
    brain.developToCriticalPeriod()
    stage = brain.getDevelopmentalStage()
    print(f"After critical period: {stage}")
    
    brain.developToMaturation()
    stage = brain.getDevelopmentalStage()
    print(f"After maturation: {stage}")
    
    brain.developToAdult()
    stage = brain.getDevelopmentalStage()
    print(f"After development to adult: {stage}")
    print()

def example_agent_neuromodulation():
    """Example using agent neuromodulation"""
    print("=== Agent Neuromodulation Example ===")
    
    # Create agent
    brain = pynlm.quickBrain()
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(None)  # No world needed for this example
    
    # Check initial neuromodulation levels
    print(f"Initial neuromodulation: {agent.getNeuromodulationLevel():.3f}")
    print(f"Initial curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"Initial novelty: {agent.getNoveltyLevel():.3f}")
    
    # Apply reward modulation
    agent.applyRewardModulation(0.8, 0.5)  # reward=0.8, predicted=0.5
    
    print(f"After reward modulation: {agent.getNeuromodulationLevel():.3f}")
    print(f"After reward modulation: {agent.getCuriosityLevel():.3f}")
    print()

def example_world_operations():
    """Example using world helper methods"""
    print("=== World Operations Example ===")
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Set agent position
    world.setAgentPosition(5.0, 5.0)
    print(f"Agent set to position (5.0, 5.0)")
    
    # Add objects
    resource = pynlm.WorldObject(2.0, 3.0, pynlm.WorldObjectType.Resource, value=10.0)
    world.addObject(resource)
    
    hazard = pynlm.WorldObject(7.0, 7.0, pynlm.WorldObjectType.Hazard, value=5.0)
    world.addObject(hazard)
    
    wall = pynlm.WorldObject(0.0, 0.0, pynlm.WorldObjectType.Wall, radius=0.8)
    world.addObject(wall)
    
    print(f"World contains {len(world.getObjects())} objects")
    
    # Check if position is valid
    is_valid = world.isValidPosition(1.0, 1.0)
    print(f"Position (1.0, 1.0) is valid: {is_valid}")
    print()

if __name__ == "__main__":
    print("NLM Python API Improvements Demo")
    print("=" * 50)
    print()
    
    example_quick_start()
    example_with_world()
    example_advanced_configuration()
    example_sensory_and_action()
    example_development_stages()
    example_agent_neuromodulation()
    example_world_operations()
    
    print("All examples completed successfully!")
