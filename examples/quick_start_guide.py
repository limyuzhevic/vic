#!/usr/bin/env python3
"""
NLM Quick Start - Getting Started with the Neural Learning Machine
================================================================

This is the quickest way to get started with the NLM framework. It demonstrates
the essential functionality in a few simple examples.
"""

import pynlm
import time

def basic_brain_example():
    """Example 1: Create and run a basic neural brain."""
    print("Example 1: Basic Brain Simulation")
    print("-" * 40)
    
    # Create a brain with default configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain has {brain.getRegionCount()} regions")
    
    # Run some simulation steps
    for step in range(100):
        brain.step(step)
        
        if step % 20 == 0:
            firing_rate = brain.getAverageFiringRate()
            print(f"  Step {step}: {firing_rate:.2f} average firing rate")
    
    print(f"Total simulation completed. Final spike count: {brain.getTotalSpikeCount()}")
    print()

def agent_world_example():
    """Example 2: Create an agent that interacts with a world."""
    print("Example 2: Agent-World Interaction")
    print("-" * 40)
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"Created agent with world: {world.getWidth()}x{world.getHeight()}")
    print(f"Agent sensory input size: {agent.getSensoryInputSize()}")
    print(f"Agent motor output size: {agent.getMotorOutputSize()}")
    
    # Run interaction loop
    for episode in range(3):
        print(f"  Episode {episode}:")
        
        for step in range(50):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 10 == 0:
                print(f"    Step {step}: Action={action}, "
                      f"Firing={brain.getFiringNeuronCount()}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}")
    
    print(f"Episode completed. Final agent state:")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent.getPredictionError():.3f}")
    print()

def learning_example():
    """Example 3: Demonstrate learning capabilities."""
    print("Example 3: Learning and Adaptation")
    print("-" * 40)
    
    # Create agent setup
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Running learning episode with reward modulation and curiosity...")
    
    for episode in range(5):
        episode_reward = 0.0
        
        for step in range(80):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            if reward == 0.0:
                reward = 0.3
            
            # Apply learning
            agent.applyRewardModulation(reward, agent.getPredictionError())
            
            episode_reward += reward
        
        print(f"  Episode {episode}: Total reward={episode_reward:.2f}, "
              f"Average curiosity={agent.getCuriosityLevel():.3f}")
    
    print("Learning completed. Final system state:")
    print(f"  Developmental stage: {agent.getDevelopmentalStage()}")
    print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print()

def advanced_example():
    """Example 4: Advanced features demonstration."""
    print("Example 4: Advanced Features")
    print("-" * 40)
    
    # Create comprehensive configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("development.enable", True)
    config.set("neuromod.curiosity.enable", True)
    
    # Create complete agent system
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Advanced agent created with enhanced features:")
    print(f"  Neural substrate: {brain.getTotalNeuronCount()} neurons, "
          f"{brain.getTotalSynapseCount()} synapses")
    print(f"  Agent features: Reward={agent.isRewardModulationEnabled()}, "
          f"Plasticity={agent.isStructuralPlasticityEnabled()}, "
          f"Development={agent.isDevelopmentEnabled()}, "
          f"Curiosity={agent.isCuriosityEnabled()}")
    
    # Run advanced simulation
    for episode in range(3):
        print(f"\n  Advanced episode {episode}:")
        
        for step in range(60):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply all learning mechanisms
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.5
            agent.applyRewardModulation(reward, agent.getPredictionError())
            agent.updateDevelopment(0.1)
            
            if step % 15 == 0:
                print(f"    Step {step}: Stage={agent.getDevelopmentalStage()}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}, "
                      f"Novelty={agent.getNoveltyLevel():.3f}")
    
    print(f"\nAdvanced simulation completed successfully!")
    print(f"Final developmental stage: {agent.getDevelopmentalStage()}")
    print(f"Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print()

def main():
    """Run all quick start examples."""
    print("NLM Quick Start - Getting Started with the Neural Learning Machine")
    print("=" * 70)
    print("\nThis guide shows you the essentials of using the NLM framework.")
    print("Each example demonstrates a different aspect of the neural simulation.")
    
    # Run all examples
    basic_brain_example()
    agent_world_example()
    learning_example()
    advanced_example()
    
    print("=" * 70)
    print("QUICK START COMPLETED SUCCESSFULLY")
    print("=" * 70)
    print("\nYou now have the basics of NLM programming!")
    print("\nNext steps:")
    print("1. Try the comprehensive examples in examples/ directory")
    print("2. Explore advanced Python bindings for more features")
    print("3. Create your own agent implementations")
    print("4. Study the documentation for advanced techniques")
    
    print("\nEssential NLM concepts learned:")
    print("  ✓ Creating neural substrates (brains)")
    print("  ✓ Setting up environments (worlds)")
    print("  ✓ Building agent interfaces")
    print("  ✓ Running simulation loops")
    print("  ✓ Processing sensory input")
    print("  ✓ Decoding motor output")
    print("  ✓ Applying reward-based learning")
    print("  ✓ Using curiosity-driven exploration")
    print("  ✓ Managing developmental processes")

if __name__ == "__main__":
    main()