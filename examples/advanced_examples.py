#!/usr/bin/env python3
"""
Example script demonstrating advanced NLM Python usage.

This script shows how to create sophisticated NLM applications
with proper configuration, simulation management, and advanced features.
"""

import pynlm
import time
import json

def example_1_basic_brain():
    """Example 1: Basic brain simulation."""
    print("=== Example 1: Basic Brain Simulation ===")
    
    # Create brain with custom configuration
    config = pynlm.createDefaultConfig()
    
    # Customize configuration
    config.set("brain.neuron_count", 200)
    config.set("brain.region_count", 2)
    config.set("brain.connection_probability", 0.2)
    config.set("random_seed", 42)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(50):
        brain.step(step)
        
        if step % 10 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print(f"  Final spike count: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print()

def example_2_agent_world():
    """Example 2: Agent-world interaction."""
    print("=== Example 2: Agent-World Interaction ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setRandomSeed(123)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation episode
    print("  Running simulation episode...")
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        if percept.getInternal():
            reward = percept.getInternal()[0]
            agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 20 == 0:
            print(f"    Step {step}:")
            print(f"      Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"      Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"      Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"      Development stage: {brain.getDevelopmentalStage()}")
    
    print(f"  Episode completed!")
    print(f"  Final metrics: {brain.getFiringNeuronCount()} firing neurons")
    print()

def example_3_neuromodulation():
    """Example 3: Neuromodulation features."""
    print("=== Example 3: Neuromodulation Features ===")
    
    # Create brain with neuromodulation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable specific neuromodulation features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    # Disable structural plasticity for this example
    agent.enableStructuralPlasticity(False)
    
    print("  Running neuromodulation simulation...")
    
    # Run with varying reward signals
    for step in range(80):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Simulate different reward scenarios
        if step % 20 == 0:
            # Positive reward
            reward = 1.0
        elif step % 20 == 10:
            # Negative reward
            reward = -1.0
        else:
            # Neutral reward
            reward = 0.0
        
        agent.applyRewardModulation(reward, 0.0)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.updateDevelopment(0.1)
        
        if step % 20 == 0:
            print(f"    Step {step}:")
            print(f"      Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
            print(f"      Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"      Prediction error: {agent.getPredictionError():.3f}")
    
    print(f"  Neuromodulation simulation completed!")
    print()

def example_4_checkpoints():
    """Example 4: Checkpoint saving and loading."""
    print("=== Example 4: Checkpoint Management ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation and save checkpoint
    print("  Running simulation...")
    for step in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.updateDevelopment(0.1)
    
    # Save checkpoint
    checkpoint_file = "simulation_checkpoint.bin"
    if brain.save(checkpoint_file):
        print(f"  Checkpoint saved to: {checkpoint_file}")
    else:
        print(f"  Warning: Checkpoint save failed")
    
    # Create new simulation for loading
    print("  Creating new simulation for checkpoint loading...")
    config2 = pynlm.createDefaultConfig()
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    # Load checkpoint
    if brain2.load(checkpoint_file):
        print(f"  Checkpoint loaded successfully!")
        print(f"  Loaded brain has {brain2.getTotalNeuronCount()} neurons")
        print(f"  Loaded brain has {brain2.getTotalSpikeCount()} spikes")
    else:
        print(f"  Warning: Checkpoint load failed")
    
    print()

def example_5_development():
    """Example 5: Developmental stages."""
    print("=== Example 5: Developmental Stages ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"  Initial developmental stage: {brain.getDevelopmentalStage()}")
    
    # Run simulation with development
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable development
    agent.enableDevelopment(True)
    
    print("  Running development simulation...")
    
    for step in range(60):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.updateDevelopment(0.1)
        
        # Check for developmental stage changes
        current_stage = brain.getDevelopmentalStage()
        if current_stage != brain.getDevelopmentalStage():
            print(f"    Developmental stage changed to: {current_stage}")
        
        if step % 15 == 0:
            print(f"    Step {step}: Stage {current_stage}, "
                  f"Neurons: {brain.getFiringNeuronCount()}, "
                  f"Learning: {agent.getNeuromodulationLevel():.3f}")
    
    print(f"  Final developmental stage: {brain.getDevelopmentalStage()}")
    print(f"  Total spikes generated: {brain.getTotalSpikeCount()}")
    print()

def example_6_pattern_analysis():
    """Example 6: Pattern analysis and statistics."""
    print("=== Example 6: Pattern Analysis ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation and collect statistics
    stats = []
    print("  Running pattern analysis simulation...")
    
    for episode in range(3):
        episode_stats = {
            'episode': episode,
            'steps': 40,
            'spikes': 0,
            'firing_neurons': [],
            'firing_rates': [],
            'development_stages': [],
            'curiosity_levels': [],
        }
        
        # We need world and agent for this to work properly
        # For simplicity, we'll just show the brain statistics
        
        for step in range(40):
            brain.step(step)
            
            # Collect statistics
            episode_stats['spikes'] = brain.getTotalSpikeCount()
            episode_stats['firing_neurons'].append(brain.getFiringNeuronCount())
            episode_stats['firing_rates'].append(brain.getAverageFiringRate())
            
            # Note: brain doesn't have getDevelopmentalStage() in the current bindings
            # So we'll skip that for now
        
        stats.append(episode_stats)
        print(f"    Episode {episode}: {episode_stats['spikes']} spikes, "
              f"avg {sum(episode_stats['firing_rates'])/len(episode_stats['firing_rates']):.2f} Hz firing rate")
    
    # Analyze patterns
    print("  Pattern analysis:")
    all_spikes = [s['spikes'] for s in stats]
    all_firing_rates = [rate for s in stats for rate in s['firing_rates']]
    
    print(f"    Spike count range: {min(all_spikes)} - {max(all_spikes)}")
    print(f"    Average firing rate range: {min(all_firing_rates):.2f} - {max(all_firing_rates):.2f} Hz")
    print(f"    Coefficient of variation: {(max(all_firing_rates) - min(all_firing_rates)) / (sum(all_firing_rates) / len(all_firing_rates)):.3f}")
    
    print()

def main():
    """Run all example scripts."""
    print("=" * 70)
    print("NLM Python Bindings - Advanced Usage Examples")
    print("=" * 70)
    print()
    
    examples = [
        example_1_basic_brain,
        example_2_agent_world,
        example_3_neuromodulation,
        example_4_checkpoints,
        example_5_development,
        example_6_pattern_analysis,
    ]
    
    for example in examples:
        try:
            example()
        except Exception as e:
            print(f"  Error in {example.__name__}: {e}")
            print()
    
    print("=" * 70)
    print("All examples completed!")
    print("=" * 70)

if __name__ == "__main__":
    main()