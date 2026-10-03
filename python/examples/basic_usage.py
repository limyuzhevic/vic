#!/usr/bin/env python3
"""
Basic NLM Usage Examples
=========================

This script demonstrates the basic usage of the NLM Python bindings.
These examples show how to create and use virtual brains, worlds, and agents.

Examples run with: python examples/basic_usage.py
"""

import sys
import os

# Add the nlm module to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'src'))

import nlm


def example_1_silent_brain():
    """
    Example 1: Silent Brain (Just Neurons)
    
    This is the simplest possible example that creates a virtual brain
    and runs a short simulation.
    """
    print("=== Example 1: Silent Brain ===")
    
    # One line to create a virtual brain
    brain = nlm.createBrain(nlm.createDefaultConfig())
    
    # Initialize it
    brain.initialize()
    
    # Make it active
    brain.step(0)
    
    print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
    print(f"Total spikes so far: {brain.getTotalSpikeCount()}")
    print()


def example_2_brain_watching_world():
    """
    Example 2: Brain Watching a World
    
    This example shows how a brain interacts with a world environment.
    """
    print("=== Example 2: Brain Watching a World ===")
    
    # Setup
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Watch the world for 30 steps
    for i in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
        
    print(f"Watched world for 30 steps")
    print(f"Firing rate: {brain.getAverageFiringRate()}")
    print(f"Neurons firing: {brain.getFiringNeuronCount()}")
    print()


def example_3_complete_agent():
    """
    Example 3: Complete Agent
    
    This example demonstrates a complete agent simulation with learning capabilities.
    """
    print("=== Example 3: Complete Agent ===")
    
    # Setup
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    world = nlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run agent
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 20 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    print("Agent simulation complete!")
    print(f"Final firing rate: {brain.getAverageFiringRate()}")
    print()


def example_4_simulation_patterns():
    """
    Example 4: Common Patterns
    
    This example shows common simulation patterns used in NLM.
    """
    print("=== Example 4: Common Patterns ===")
    
    # Pattern 1: Run a Simulation
    print("Pattern 1: Run a Simulation")
    def run_simulation(brain, world, agent, num_steps):
        for step in range(num_steps):
            # 1. Update world
            world.update(0.1)
            
            # 2. Get what the agent sees
            percept = world.getSensoryPercept()
            
            # 3. Tell the brain
            agent.processSensoryInput(percept)
            
            # 4. Brain thinks
            brain.step(step)
            
            # 5. Get action from brain
            action = agent.decodeMotorCommand()
            
            # 6. Do action in world
            world.applyMotorCommand(action, world.getSimulationTime())
    
    # Use it
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    world = nlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    run_simulation(brain, world, agent, 100)
    print(f"  Simulation completed {brain.getTotalSpikeCount()} spikes")
    print()
    
    # Pattern 2: Get Brain Stats
    print("Pattern 2: Get Brain Stats")
    for i in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
    
    print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Avg firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Dev stage: {brain.getDevelopmentalStage()}")
    print()


def example_5_configuration():
    """
    Example 5: Configuration
    
    This example shows how to configure the brain simulation.
    """
    print("=== Example 5: Configuration ===")
    
    # Create configuration
    config = nlm.createDefaultConfig()
    
    # Modify configuration parameters
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.2)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    
    # Create brain with custom config
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Using custom configuration")
    print()


def example_6_troubleshooting():
    """
    Example 6: Troubleshooting
    
    This example shows common troubleshooting tips.
    """
    print("=== Example 6: Troubleshooting ===")
    
    print("Common issues and solutions:")
    print()
    print("1. 'My brain isn't doing anything':")
    print("   - Did you call brain.initialize()?")
    print("   - Try increasing the number of steps")
    print()
    print("2. 'The agent isn't moving':")
    print("   - Did you call world.applyMotorCommand()?")
    print("   - Check that world.update() is being called")
    print()
    print("3. 'Everything is 0':")
    print("   - Brains need time to 'warm up' - try more steps")
    print("   - Some neurons need input to fire - make sure sensory input is connected")
    print()


def main():
    """Run all examples"""
    print("NLM Python Bindings - Basic Usage Examples")
    print("=" * 50)
    print()
    
    try:
        example_1_silent_brain()
        example_2_brain_watching_world()
        example_3_complete_agent()
        example_4_simulation_patterns()
        example_5_configuration()
        example_6_troubleshooting()
        
        print("All examples completed successfully!")
        print()
        print("Next steps:")
        print("1. Read HOW_TO_USE.md for more details")
        print("2. Read docs/ARCHITECTURE.md to understand how it all works")
        print("3. Experiment with different configurations!")
        
    except Exception as e:
        print(f"Error running examples: {e}")
        print("Please check your NLM installation and dependencies.")
        sys.exit(1)


if __name__ == "__main__":
    main()