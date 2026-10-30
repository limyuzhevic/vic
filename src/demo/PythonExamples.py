"""
NLM Python Examples - Neural Learning Machine

This module provides examples and tutorials for using NLM's Python API.
It covers the main use cases from simple brain creation to complex agent simulations.
"""

import pynlm
import time

def example_simple_brain():
    """
    Example 1: Create and run a simple brain.
    
    This is the absolute minimum example - create a brain and run it for a few steps.
    """
    print("=== Example 1: Simple Brain ===")
    
    # Step 1: Create brain with default configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Step 2: Initialize the brain
    brain.initialize()
    
    # Step 3: Run the brain for 100 steps
    for step in range(100):
        brain.step(step)
    
    # Step 4: Get statistics
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    print("✓ Simple brain example complete!\n")

def example_brain_world():
    """
    Example 2: Brain interacting with a world.
    
    Shows how a brain perceives and acts in an environment.
    """
    print("=== Example 2: Brain in a World ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent to connect brain and world
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning capabilities
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    print("Running agent simulation...")
    for step in range(200):
        # Update world
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Brain processes sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks/decides
        brain.step(step)
        
        # Brain produces action
        action = agent.decodeMotorCommand()
        
        # Action affects world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress
        if step % 50 == 0:
            print(f"  Step {step}:")
            print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"    World time: {world.getSimulationTime():.1f}s")
    
    print("✓ Brain-world interaction example complete!\n")

def example_learning():
    """
    Example 3: Demonstrate learning and plasticity.
    
    Shows how synapses strengthen/weaken through experience.
    """
    print("=== Example 3: Learning and Plasticity ===")
    
    # Create configuration optimized for learning
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent for learning
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Starting learning simulation...")
    
    # Phase 1: Exploration (curiosity-driven)
    print("Phase 1: Curiosity-driven exploration")
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Phase 2: Reward-based learning
    print("Phase 2: Reward-based learning")
    for step in range(100, 250):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward from world and apply to agent
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
    
    # Phase 3: Memory consolidation
    print("Phase 3: Memory consolidation")
    for step in range(250, 350):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print learning statistics
    print(f"\nLearning Results:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Total reward experienced: {agent.getPredictionError():.3f}")
    
    print("✓ Learning example complete!\n")

def example_checkpointing():
    """
    Example 4: Save and load brain states (checkpointing).
    
    Demonstrates how to save progress and resume later.
    """
    print("=== Example 4: Checkpointing ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some simulation
    for step in range(100):
        brain.step(step)
    
    # Save checkpoint
    checkpoint_file = "brain_checkpoint.bin"
    print(f"Saving checkpoint to: {checkpoint_file}")
    if brain.save(checkpoint_file):
        print("✓ Checkpoint saved successfully!")
        
        # Create new brain from scratch
        brain2 = pynlm.createBrain(config)
        brain2.initialize()
        
        # Load saved state
        print(f"Loading checkpoint from: {checkpoint_file}")
        if brain2.load(checkpoint_file):
            print("✓ Checkpoint loaded successfully!")
            print(f"  Loaded spikes: {brain2.getTotalSpikeCount()}")
            print(f"  Loaded neurons: {brain2.getTotalNeuronCount()}")
        else:
            print("✗ Failed to load checkpoint")
    else:
        print("✗ Failed to save checkpoint")
    
    print("✓ Checkpointing example complete!\n")

def example_development():
    """
    Example 5: Brain development over time.
    
    Shows how brains change and mature through developmental stages.
    """
    print("=== Example 5: Brain Development ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 150)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent for development
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable development
    agent.enableDevelopment(True)
    
    print("Running developmental simulation...")
    
    # Run simulation for extended time to observe development
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print developmental stage periodically
        if step % 100 == 0:
            print(f"  Step {step}:")
            print(f"    Developmental stage: {brain.getDevelopmentalStage()}")
            print(f"    Firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    print("✓ Development example complete!\n")

def example_visualization():
    """
    Example 6: Basic visualization tools.
    
    Shows how to use NLM's visualization capabilities.
    """
    print("=== Example 6: Visualization ===")
    
    print("Note: Visualization requires GUI/display support.")
    print("For command-line visualization, see:")
    print("  - nlm_phase6_demo (Phase 6 integration demo)")
    print("  - VisualizationInterface.hpp in source code")
    
    # Create brain for visualization example
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some steps to get activity
    for step in range(50):
        brain.step(step)
    
    print(f"Brain ready for visualization:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    print(f"  Firing: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    print("✓ Visualization example complete!\n")

def main():
    """
    Run all examples.
    
    This demonstrates the main capabilities of NLM:
    - Simple brain creation and operation
    - Brain-world interaction
    - Learning and plasticity
    - Checkpointing and persistence
    - Brain development
    - Visualization
    """
    print("=" * 60)
    print("NLM Python Examples - Neural Learning Machine")
    print("=" * 60)
    print()
    
    # Run examples sequentially
    example_simple_brain()
    example_brain_world()
    example_learning()
    example_checkpointing()
    example_development()
    example_visualization()
    
    print("=" * 60)
    print("All examples completed successfully!")
    print("=" * 60)
    print()
    print("To run individual examples:")
    print("  - Import pynlm and call specific example functions")
    print("  - Use command-line tools: ./nlm_demo, ./nlm_phase2_demo, ./nlm_phase4_demo")
    print("  - Check documentation for more details")

if __name__ == "__main__":
    main()
