"""
Basic NLM Examples

This file demonstrates fundamental NLM usage patterns and introduces
the enhanced features available in version 0.1.0.
"""

import pynlm

def example_1_simplest_brain():
    """Example 1: The absolute simplest possible example."""
    print("Example 1: Simplest Brain")
    print("-" * 30)
    
    # One line to create a virtual brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Initialize it
    brain.initialize()
    
    # Make it active
    brain.step(0)
    
    print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
    print(f"Your brain has {brain.getTotalSpikeCount()} spikes so far!")
    print()

def example_2_basic_simulation():
    """Example 2: Basic brain simulation."""
    print("Example 2: Basic Brain Simulation")
    print("-" * 30)
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Starting simulation with {brain.getTotalNeuronCount()} neurons...")
    
    for step in range(10):
        brain.step(step)
        if step % 5 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, {brain.getTotalSpikeCount()} total spikes")
    
    print(f"Final spike count: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    print()

def example_3_world_simulation():
    """Example 3: Brain in a world environment."""
    print("Example 3: Brain in a World")
    print("-" * 30)
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("Running agent in world simulation...")
    
    # Run simulation
    for step in range(20):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 5 == 0:
            print(f"Step {step}: Action {action.getType()}, Firing: {brain.getFiringNeuronCount()}")
    
    print("Simulation complete!")
    print()

def example_4_with_learning():
    """Example 4: Agent with learning enabled."""
    print("Example 4: Agent with Learning Features")
    print("-" * 30)
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(15, 15, 6, 6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Running agent with learning enabled...")
    
    for step in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation (learning)
        reward = 1.0
        predicted = 0.5
        agent.applyRewardModulation(reward, predicted)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        if step % 10 == 0:
            print(f"Step {step}: Reward={reward}, Curiosity={agent.getCuriosityLevel():.2f}, Development={agent.getDevelopmentalStage()}")
    
    print("Learning simulation complete!")
    print()

if __name__ == "__main__":
    print("Basic NLM Examples")
    print("=" * 50)
    print()
    
    example_1_simplest_brain()
    example_2_basic_simulation()
    example_3_world_simulation()
    example_4_with_learning()
    
    print("All basic examples completed successfully!")
