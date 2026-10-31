#!/usr/bin/env python3
# NLM Examples
# Educational examples for using the NLM neural simulation framework

"""
This directory contains example scripts demonstrating various NLM use cases.

Each example shows:
1. Basic NLM concepts
2. Practical usage patterns
3. Common tasks and workflows

The examples are designed to be educational and easy to follow.
"""

import os
import sys
import json
import time
from pathlib import Path

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent))

import pynlm as nlm

def example_1_basic_brain():
    """
    Example 1: Basic Brain Simulation
    =================================
    
    This example demonstrates how to create and run a basic neural simulation.
    """
    print("=== Example 1: Basic Brain Simulation ===")
    
    # Create a basic brain configuration
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("simulation_timestep", 0.001)
    
    # Create and initialize the brain
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Configuration: {brain.getConfig().summary()}")
    
    # Run simulation
    print("\nRunning simulation (10 steps)...")
    start_time = time.time()
    
    for step in range(10):
        brain.step(step)
    
    end_time = time.time()
    
    print(f"Simulation completed in {end_time - start_time:.3f} seconds")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    return brain

def example_2_brain_in_world():
    """
    Example 2: Brain in a Simple World
    =================================
    
    This example demonstrates how to use NLM with a simulated world environment.
    """
    print("\n=== Example 2: Brain in a Simple World ===")
    
    # Setup brain
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    # Create world
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, vision_width=8, vision_height=8)
    world.reset()
    
    # Place agent in world
    world.setAgentStart(10, 10)
    
    # Create agent brain
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"World size: {world.getWidth()} x {world.getHeight()}")
    print(f"Agent start position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    
    # Run agent simulation
    print("\nRunning agent simulation (20 steps)...")
    
    for step in range(20):
        # Update world
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        print(f"Step {step}: Agent sees vision {percept.getVisionWidth()}x{percept.getVisionHeight()}")
        
        # Tell the brain what it sees
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        print(f"  Action: {action.getName()}")
        
        # Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print final statistics
    print(f"\nFinal agent state:")
    print(f"  Position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    print(f"  Orientation: {world.getAgentBody().orientation}°")
    print(f"  Energy: {world.getAgentBody().energy}")
    print(f"  Health: {world.getAgentBody().health}")
    
    return brain, world, agent

def example_3_learning_brain():
    """
    Example 3: Learning Brain
    ==========================
    
    This example demonstrates how to create a learning brain with reward modulation.
    """
    print("\n=== Example 3: Learning Brain ===")
    
    # Create configuration with learning enabled
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    config.set("enable_reward_modulation", True)
    config.set("enable_curiosity", True)
    
    # Create brain
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = nlm.createAgentBrain(brain)
    
    # Create simple world
    world = nlm.createSimpleWorld()
    world.configure(width=15, height=15, vision_width=6, vision_height=6)
    world.reset()
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    print("Learning brain created with enabled features:")
    print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"  Curiosity: {agent.isCuriosityEnabled()}")
    print(f"  Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
    print(f"  Development: {agent.isDevelopmentEnabled()}")
    
    # Run learning simulation
    print("\nRunning learning simulation (50 steps)...")
    
    for step in range(50):
        world.update(0.1)
        
        # Get percept
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Provide learning signal based on outcome
        if step % 10 == 0:
            # Simple reward based on position
            reward = world.getAgentBody().energy / 100.0
            predicted = agent.getPredictionError()
            
            agent.applyRewardModulation(reward, predicted)
            print(f"  Step {step}: Reward={reward:.2f}, Error={predicted:.2f}")
    
    print(f"\nLearning completed:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    
    return brain, world, agent

def example_4_memory_brain():
    """
    Example 4: Memory Brain
    =========================
    
    This example demonstrates memory formation and retrieval.
    """
    print("\n=== Example 4: Memory Brain ===")
    
    # Create brain
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    # Get memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    print("Memory systems initialized:")
    print(f"  Working memory capacity: {working_memory.getCapacity()}")
    print(f"  Episodic memory max episodes: {episodic_memory.getMaxEpisodes()}")
    
    # Store some information in working memory
    print("\nStoring information in working memory...")
    for i in range(5):
        working_memory.storeToNeuron(nlm.NeuronId(i), float(i) / 10.0)
        working_memory.update(0.001)
    
    # Get current memory traces
    memory_neuons = working_memory.getMemoryNeurons()
    print(f"  Current memory traces: {len(memory_neuons)} neurons")
    
    # Create an episode (like a memory)
    print("\nCreating episodic memory...")
    episode = nlm.EpisodicMemoryItem()
    episode.timestamp = 100
    episode.reward = 0.8
    
    for i in range(10):
        episode.activeNeurons.append(nlm.NeuronId(i))
        episode.neuronActivations.append(float(i) / 20.0)
    
    episodic_memory.storeEpisode(episode)
    
    print(f"  Episode created with {len(episode.activeNeurons)} active neurons")
    
    # Get all episodes
    all_episodes = episodic_memory.getEpisodes()
    print(f"  Total episodes in memory: {len(all_episodes)}")
    
    # Demonstrate pattern completion (associative memory)
    print("\nDemonstrating pattern completion...")
    # This would typically involve presenting a partial pattern
    # and retrieving the complete stored pattern
    
    return brain

def example_5_custom_configuration():
    """
    Example 5: Custom Configuration
    ================================
    
    This example shows how to create and use custom configurations.
    """
    print("\n=== Example 5: Custom Configuration ===")
    
    # Method 1: Create config from scratch
    config1 = nlm.createDefaultConfig()
    config1.set("neuron_count", 2000)
    config1.set("region_count", 3)
    config1.set("connection_probability", 0.2)
    
    print("Custom configuration 1:")
    print(f"  Neurons: {config1.getOr('neuron_count', 1000)}")
    print(f"  Regions: {config1.getOr('region_count', 1)}")
    print(f"  Connectivity: {config1.getOr('connection_probability', 0.15)}")
    
    # Method 2: Load from file
    config_file = Path(__file__).parent / "nlm_config.cfg"
    if config_file.exists():
        # This would normally load a config file
        print(f"\nConfiguration file: {config_file}")
        print("(Note: File loading would require custom parser)")
    
    # Method 3: Use configuration manager
    print("\nConfiguration management example:")
    print("  - Save configuration to file")
    print("  - Load configuration from different formats (JSON, YAML, NLM)")
    print("  - Compare different configurations")
    print("  - Create and use presets")
    
    return config1, config2 if 'config2' in locals() else None

def main():
    """
    Run all examples
    """
    print("NLM Examples")
    print("============")
    print("This script demonstrates various NLM usage patterns.")
    print("Each example builds on the previous one to show")
    print("increasing complexity and functionality.\n")
    
    examples = [
        ("Basic Brain Simulation", example_1_basic_brain),
        ("Brain in a World", example_2_brain_in_world),
        ("Learning Brain", example_3_learning_brain),
        ("Memory Brain", example_4_memory_brain),
        ("Custom Configuration", example_5_custom_configuration),
    ]
    
    for example_name, example_func in examples:
        try:
            result = example_func()
            print(f"\n✓ {example_name} completed successfully")
        except Exception as e:
            print(f"\n✗ {example_name} failed: {e}")
            import traceback
            traceback.print_exc()
        
        print("\n" + "="*50 + "\n")
    
    print("All examples completed!")
    print("\nFor more information:")
    print("- Check the detailed documentation (NLM_Advanced_Guide.md)")
    print("- Run individual examples: python3 example_X.py")
    print("- See the NLM documentation for advanced topics")

if __name__ == "__main__":
    main()
