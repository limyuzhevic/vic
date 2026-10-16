#!/usr/bin/env python3
"""
Advanced NLM Python Examples
===========================

This file demonstrates advanced usage of the NLM Python API,
showing how to use all the integrated brain systems effectively.

Author: NLM Development Team
Version: 0.1.0
Phase: 6 - Final Integration
"""

import pynlm
import time
import random

def basic_brain_example():
    """Example 1: Basic brain creation and simulation"""
    print("Example 1: Basic Brain Creation and Simulation")
    print("=" * 50)
    
    # Create brain with custom configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("simulation.timestep", 0.001)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Initial spike count: {brain.getTotalSpikeCount()}")
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    print(f"After 100 steps: {brain.getTotalSpikeCount()} total spikes")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    print()

def memory_system_example():
    """Example 2: Using memory systems"""
    print("Example 2: Memory Systems Integration")
    print("=" * 50)
    
    # Create brain with memory subsystems enabled
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("memory.working_capacity", 100)
    config.set("memory.episodic.max_episodes", 500)
    config.set("memory.associative.threshold", 0.7)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    print("Memory systems initialized:")
    print(f"  Working memory capacity: {working_memory.getActiveTraces()} traces")
    print(f"  Episodic memory episodes: {episodic_memory.getEpisodeCount()} stored")
    print(f"  Associative memory associations: {associative_memory.getAssociationsCount()}")
    
    # Simulate some memory storage
    for step in range(50):
        # Store in working memory
        if random.random() < 0.3:
            neuron_id = pynlm.INVALID_NEURON_ID  # Would need actual neuron ID
            working_memory.storeToNeuron(neuron_id, random.random())
        
        # Create and store episodic memory
        if step % 25 == 0:
            from pynlm import EpisodicMemoryItem
            episode = EpisodicMemoryItem()
            episode.timestamp = step
            episode.metadata = f"Step {step} experience"
            episodic_memory.storeEpisode(episode)
        
        brain.step(step)
    
    print(f"Final working memory traces: {working_memory.getActiveTraces()}")
    print(f"Final episodic memory episodes: {episodic_memory.getEpisodeCount()}")
    print()

def neuromodulation_example():
    """Example 3: Neuromodulation and learning"""
    print("Example 3: Neuromodulation and Learning")
    print("=" * 50)
    
    # Create brain with neuromodulation enabled
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("neuromodulation.dopamine.scale", 1.0)
    config.set("neuromodulation.curiosity.enable", True)
    config.set("neuromodulation.novelty.enable", True)
    config.set("neuromodulation.prediction_error.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with neuromodulation
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableNovelty(True)
    
    # Create simple world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent.initialize(world)
    
    print("Neuromodulation systems enabled:")
    print(f"  Dopamine level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent.getPredictionError():.3f}")
    
    # Run learning simulation
    for step in range(200):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action and apply to world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 1.0 if random.random() < 0.2 else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log statistics every 50 steps
        if step % 50 == 0:
            print(f"  Step {step}:")
            print(f"    Dopamine: {agent.getNeuromodulationLevel():.3f}")
            print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"    Dev Stage: {brain.getDevelopmentalStage()}")
    
    print()

def plasticity_example():
    """Example 4: Plasticity mechanisms"""
    print("Example 4: Plasticity Mechanisms")
    print("=" * 50)
    
    # Create brain with plasticity enabled
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.01)
    config.set("plasticity.hebbian.learning_rate", 0.005)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get plasticity systems
    stdp = brain.getSTDP()
    hebbian = brain.getHebbian()
    structural = brain.getStructuralPlasticity()
    
    print("Plasticity systems configured:")
    print(f"  STDP: {'enabled' if stdp else 'disabled'}")
    print(f"  Hebbian: {'enabled' if hebbian else 'disabled'}")
    print(f"  Structural: {'enabled' if structural else 'disabled'}")
    
    # Configure STDP parameters
    if stdp:
        stdp.setLTPWeight(0.02)
        stdp.setLTDWeight(0.018)
        stdp.setTau(20.0)
        print(f"  STDP LTP weight: {stdp.getLTPWeight()}")
        print(f"  STDP LTD weight: {stdp.getLTDWeight()}")
    
    # Configure Hebbian learning
    if hebbian:
        hebbian.setLearningRate(0.008)
        print(f"  Hebbian learning rate: {hebbian.getLearningRate()}")
    
    # Configure structural plasticity
    if structural:
        structural.setSynaptogenesisRate(0.0001)
        structural.setPruningRate(0.00001)
        print(f"  Synaptogenesis rate: {structural.getSynaptogenesisRate()}")
        print(f"  Pruning rate: {structural.getPruningRate()}")
    
    # Run plasticity simulation
    for step in range(100):
        # Apply repeated input patterns to stimulate learning
        for i in range(20):
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)
        
        brain.step(step)
        
        if step % 20 == 0:
            print(f"  Step {step}:")
            print(f"    Total spikes: {brain.getTotalSpikeCount()}")
            print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
    
    print(f"Final E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    print()

def development_example():
    """Example 5: Development stages"""
    print("Example 5: Development Stages")
    print("=" * 50)
    
    # Create brain with development enabled
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1200)
    config.set("development.enable", True)
    config.set("development.maturation_rate", 0.1)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Development system initialized")
    print(f"  Initial developmental stage: {brain.getDevelopmentalStage()}")
    
    # Get development system
    development = brain.getDevelopmentSystem()
    
    # Run developmental simulation
    for step in range(300):
        # Update development
        if development:
            development.update(0.1)
            stage = development.getCurrentStage()
            plasticity_mod = development.getPlasticityModulation()
            
            if step % 50 == 0:
                print(f"  Step {step}:")
                print(f"    Developmental stage: {stage}")
                print(f"    Plasticity modulation: {plasticity_mod:.2f}")
        
        brain.step(step)
        
        # Simulate age-related changes
        if step % 100 == 0 and step > 0:
            brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
            print(f"    Advanced to Adult stage")
    
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
    print()

def checkpoint_example():
    """Example 6: Checkpoint and persistence"""
    print("Example 6: Checkpoint and Persistence")
    print("=" * 50)
    
    # Create and train brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Training brain for 100 steps...")
    for step in range(100):
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0)
        brain.step(step)
    
    # Save checkpoint
    checkpoint_file = "nlm_checkpoint.bin"
    if brain.save(checkpoint_file):
        print(f"✓ Brain state saved to {checkpoint_file}")
    else:
        print(f"✗ Failed to save checkpoint")
    
    # Simulate time passing and changes
    print("Simulating time and changes...")
    for step in range(100, 200):
        brain.step(step)
    
    # Load checkpoint
    brain.reset()
    if brain.load(checkpoint_file):
        print(f"✓ Brain state loaded from {checkpoint_file}")
    else:
        print(f"✗ Failed to load checkpoint")
    
    print(f"Post-recovery spike count: {brain.getTotalSpikeCount()}")
    print()

def world_interaction_example():
    """Example 7: Complete world interaction"""
    print("Example 7: Complete World Interaction")
    print("=" * 50)
    
    # Create complete agent setup
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("world.width", 30)
    config.set("world.height", 30)
    config.set("vision.width", 10)
    config.set("vision.height", 10)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Complete agent system initialized")
    print(f"  World size: {world.getWidth()}x{world.getHeight()}")
    print(f"  Vision range: {world.getAgentBody().orientation:.1f} degrees")
    
    # Run complete simulation
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Process in brain
        brain.step(step)
        
        # Get and apply action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning signals
        reward = 1.0 if step % 25 == 0 else 0.0
        novelty = agent.getNoveltyLevel()
        agent.applyRewardModulation(reward, novelty)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print status every 25 steps
        if step % 25 == 0:
            body = world.getAgentBody()
            print(f"  Step {step}:")
            print(f"    Position: ({body.x:.1f}, {body.y:.1f})")
            print(f"    Energy: {body.energy:.1f}/{body.health:.1f}")
            print(f"    Age: {body.age} steps")
            print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
    
    print(f"Simulation complete at ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    print()

def main():
    """Run all examples"""
    print("NLM Python API - Advanced Examples")
    print("Phase 6: Final Integration Examples")
    print("=" * 60)
    print()
    
    basic_brain_example()
    memory_system_example()
    neuromodulation_example()
    plasticity_example()
    development_example()
    checkpoint_example()
    world_interaction_example()
    
    print("=" * 60)
    print("All examples completed successfully!")
    print("These examples demonstrate the full capabilities of Phase 6")
    print("NLM with all integrated memory, prediction, cognition, and")
    print("neuromodulation systems working together.")

if __name__ == "__main__":
    main()
