#!/usr/bin/env python3
"""
NLM Basic Demo Examples

This module provides basic demonstration scripts for the NLM (Neural Learning Machine)
framework, showcasing simple and practical usage patterns for getting started
with the library.

Author: NLM Development Team
License: MIT
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def basic_brain_demo():
    """
    Demonstrate the basic brain creation and usage.
    
    This is the simplest possible example showing how to create and use
    a neural brain with the NLM library.
    """
    print("="*60)
    print("BASIC BRAIN DEMO")
    print("="*60)
    
    # Step 1: Create a default configuration
    print("\nStep 1: Creating default configuration...")
    config = pynlm.createDefaultConfig()
    
    # Step 2: Create the brain
    print("Step 2: Creating brain...")
    brain = pynlm.createBrain(config)
    
    # Step 3: Initialize the brain
    print("Step 3: Initializing brain...")
    brain.initialize()
    
    # Step 4: Run simulation steps
    print("Step 4: Running simulation steps...")
    for i in range(10):
        brain.step(i)
        if i % 5 == 0:
            print(f"  Step {i}: {brain.getFiringNeuronCount()} neurons firing,")
            print(f"          {brain.getTotalSpikeCount()} total spikes")
    
    # Step 5: Print final statistics
    print("\nStep 5: Final statistics...")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Currently firing: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    print("\nBasic brain demo completed successfully!")
    return brain

def agent_world_demo():
    """
    Demonstrate complete agent-world interaction.
    
    This demo shows how to create a complete agent that interacts with
    a simulated environment (world).
    """
    print("\n" + "="*60)
    print("AGENT-WORLD DEMO")
    print("="*60)
    
    # Step 1: Create brain and configure
    print("\nStep 1: Creating brain and agent...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Step 2: Create and configure world
    print("Step 2: Creating and configuring world...")
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    # Set agent starting position
    world.setAgentStart(7.5, 7.5)
    
    # Step 3: Initialize agent with world
    print("Step 3: Initializing agent with world...")
    agent.initialize(world)
    
    # Enable learning capabilities
    print("Step 4: Enabling learning capabilities...")
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Step 5: Run simulation
    print("Step 5: Running agent-world simulation...")
    brain.initialize()
    
    for step in range(50):
        # Update world
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Tell the brain
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward based on environment interaction
        agent.applyRewardModulation(0.1, 0.0)  # Small reward for interaction
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 10 == 0:
            print(f"  Step {step}:")
            print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"    Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"    Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"    Development stage: {brain.getDevelopmentalStage()}")
    
    # Step 6: Print final statistics
    print("\nStep 6: Final simulation statistics...")
    print(f"  Total steps: 50")
    print(f"  Final firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Final novelty: {agent.getNoveltyLevel():.3f}")
    print(f"  Final development stage: {brain.getDevelopmentalStage()}")
    
    print("\nAgent-world demo completed successfully!")
    return brain, agent, world

def complete_simulation_demo():
    """
    Demonstrate a complete simulation with all features enabled.
    
    This is a comprehensive demo showing all major features of the NLM library
    working together: brain, world, agent, learning, development, etc.
    """
    print("\n" + "="*60)
    print("COMPLETE SIMULATION DEMO")
    print("="*60)
    
    # Create all components
    print("\nStep 1: Creating all simulation components...")
    
    # Configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    config.set("brain.synapse_density", 0.1)
    
    # Brain
    brain = pynlm.createBrain(config)
    
    # Agent
    agent = pynlm.createAgentBrain(brain)
    
    # World
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.setMaxEnergy(150.0)
    
    # Setup
    world.reset()
    agent.initialize(world)
    brain.initialize()
    
    # Enable all capabilities
    print("Step 2: Enabling all capabilities...")
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Add some objects to the world
    print("Step 3: Adding objects to the world...")
    world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0))
    world.addObject(pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Resource, value=2.0))
    world.addObject(pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Hazard, value=1.0))
    
    # Run extended simulation
    print("Step 4: Running extended simulation...")
    num_steps = 100
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward (based on agent's predictions and actual outcome)
        reward = agent.getNeuromodulationLevel() * 0.5
        agent.applyRewardModulation(reward, reward * 0.8)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print periodic progress
        if step % 25 == 0:
            print(f"  Step {step}/{num_steps}:")
            print(f"    Agent at ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"    Brain state: {brain.getTotalSpikeCount()} spikes, "
                  f"{brain.getFiringNeuronCount()} firing neurons")
            print(f"    Agent state: curiosity={agent.getCuriosityLevel():.3f}, "
                  f"novelty={agent.getNoveltyLevel():.3f}")
            print(f"    Development: {brain.getDevelopmentalStage()}")
            print(f"    Energy: {world.getAgentBody().energy:.1f}/{world.getMaxEnergy()}")
    
    # Print final comprehensive report
    print("\nStep 5: Final comprehensive report...")
    print("="*60)
    print("FINAL SIMULATION REPORT")
    print("="*60)
    print(f"Simulation completed: {num_steps} steps")
    print(f"\nBrain Performance:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Final spike count: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  E/I balance: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    print(f"\nAgent Performance:")
    print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Final prediction error: {agent.getPredictionError():.3f}")
    print(f"  Reward modulation enabled: {agent.isRewardModulationEnabled()}")
    print(f"  Structural plasticity enabled: {agent.isStructuralPlasticityEnabled()}")
    print(f"  Development enabled: {agent.isDevelopmentEnabled()}")
    print(f"  Curiosity enabled: {agent.isCuriosityEnabled()}")
    
    print(f"\nWorld State:")
    print(f"  Simulation time: {world.getSimulationTime():.1f}")
    print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    print(f"  Agent energy: {world.getAgentBody().energy:.1f}/{world.getMaxEnergy()}")
    
    print("\nComplete simulation demo completed successfully!")
    return brain, agent, world

def quick_start_demo():
    """
    Quick start demonstration for beginners.
    
    This is the fastest possible way to get started with NLM,
    showing the absolute minimum required code.
    """
    print("\n" + "="*60)
    print("QUICK START DEMO")
    print("="*60)
    
    print("\nThis demo shows the minimal code required to create and run")
    print("a neural simulation with NLM. Just follow these steps:")
    
    print("\n1. Import pynlm")
    print("2. Create a brain with default config")
    print("3. Initialize the brain")
    print("4. Run a few steps")
    print("5. Print results")
    
    print("\n" + "-"*60)
    print("CODE EXAMPLE:")
    print("-"*60)
    print("""
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
""")
    
    # Run the example
    print("\nRunning the example...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    brain.step(0)
    
    print(f"\nYour brain has {brain.getTotalNeuronCount()} neurons!")
    print(f"And generated {brain.getTotalSpikeCount()} spikes in {brain.getFiringNeuronCount()} neurons!")
    
    print("\nQuick start demo completed successfully!")
if __name__ == "__main__":
    print("NLM Basic Demo Examples")
    print("Available demos:")
    print("  1. Basic Brain Demo - Simple brain creation and usage")
    print("  2. Agent-World Demo - Complete agent-environment interaction")
    print("  3. Complete Simulation Demo - Full feature demonstration")
    print("  4. Quick Start Demo - Minimal code example")
    print("  5. Exit")
    
    # Uncomment to run specific demos
    # basic_brain_demo()
    # agent_world_demo()
    # complete_simulation_demo()
    # quick_start_demo()
    
    print("\nTo run demos, uncomment the appropriate function call in main()")
    print("Or run specific demos by importing and calling functions directly.")
