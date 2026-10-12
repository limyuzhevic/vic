#!/usr/bin/env python3
"""
NLM Python Bindings - Comprehensive Examples

This script provides comprehensive examples for using the NLM (Neural Learning Machine)
Python bindings. It demonstrates the improved API features including progress tracking,
memory management, experiment running, and visualization capabilities.

Usage:
    python3 examples.py
"""

import sys
import os
import tempfile
import shutil
import time

# Add the current directory to Python path so we can import pynlm
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '.'))

import pynlm


def example_basic_brain_creation():
    """Example 1: Basic brain creation and simulation"""
    print("=== Example 1: Basic Brain Creation and Simulation ===")
    
    # Create a brain using the new factory function
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons and {brain.getTotalSynapseCount()} synapses")
    
    # Initialize and run a basic simulation
    brain.initialize()
    
    print("Running basic simulation...")
    for step in range(100):
        brain.step(step)
    
    print(f"Simulation complete. Firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Total spikes recorded: {brain.getTotalSpikeCount()}")
    print()


def example_progress_tracking():
    """Example 2: Using progress tracking and callbacks"""
    print("=== Example 2: Progress Tracking and Callbacks ===")
    
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation with progress tracking
    print("Running simulation with progress tracking...")
    start_time = time.time()
    
    for step in range(1000):
        brain.step(step)
        
        # Progress update every 100 steps
        if step % 100 == 0:
            progress = (step / 1000) * 100
            print(f"Progress: {progress:.1f}%, Firing: {brain.getFiringNeuronCount()}, Rate: {brain.getAverageFiringRate():.2f}")
    
    end_time = time.time()
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    print(f"Final statistics - Firing rate: {brain.getAverageFiringRate():.2f}, Total spikes: {brain.getTotalSpikeCount()}")
    print()


def example_agent_world_simulation():
    """Example 3: Complete agent-world simulation"""
    print("=== Example 3: Complete Agent-World Simulation ===")
    
    # Create all components
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation loop
    print("Running complete agent-world simulation...")
    for step in range(500):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        if step % 100 == 0:
            print(f"Step {step}: Firing: {brain.getFiringNeuronCount()}, Curiosity: {agent.getCuriosityLevel():.3f}, Novelty: {agent.getNoveltyLevel():.3f}")
    
    print("Agent simulation complete!")
    print(f"Final development stage: {brain.getDevelopmentalStage()}")
    print()


def example_memory_management():
    """Example 4: Memory management and checkpointing"""
    print("=== Example 4: Memory Management and Checkpointing ===")
    
    # Create brain with memory systems
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Initial brain state:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Working memory available: {brain.getWorkingMemory() is not None}")
    print(f"  Episodic memory available: {brain.getEpisodicMemory() is not None}")
    
    # Run simulation to generate memory
    print("\nRunning simulation to generate memory...")
    for step in range(200):
        brain.step(step)
    
    # Check memory contents
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    print(f"After 200 steps:")
    if working_memory:
        print(f"  Working memory traces: {working_memory.getActiveTraces()}")
    if episodic_memory:
        print(f"  Episodic memory episodes: {episodic_memory.getEpisodeCount()}")
    
    # Save checkpoint
    print("\nSaving checkpoint...")
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as tmp_file:
        checkpoint_path = tmp_file.name
    
    try:
        if brain.save(checkpoint_path):
            print(f"Checkpoint saved to: {checkpoint_path}")
            
            # Create new brain and load checkpoint
            print("Loading checkpoint into new brain...")
            config2 = pynlm.createDefaultConfig()
            brain2 = pynlm.createBrain(config2)
            brain2.initialize()
            
            if brain2.load(checkpoint_path):
                print("Checkpoint loaded successfully!")
                print(f"Loaded brain total neurons: {brain2.getTotalNeuronCount()}")
            else:
                print("Failed to load checkpoint")
        else:
            print("Failed to save checkpoint")
    finally:
        # Clean up
        if os.path.exists(checkpoint_path):
            os.unlink(checkpoint_path)
            print(f"Cleaned up checkpoint file")
    
    print()


def example_utilities():
    """Example 5: Using utility classes (Experiment, DataHandler, Visualization)"""
    print("=== Example 5: Using Utility Classes ===")
    
    # Create a custom experiment using the utility class
    print("Creating custom experiment...")
    config = pynlm.createDefaultConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Enable key features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run multiple episodes
    print("Running multiple episodes...")
    total_reward = 0.0
    total_steps = 0
    
    for episode in range(3):
        print(f"Episode {episode + 1}:")
        episode_reward = 0.0
        
        for step in range(200):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            episode_reward += reward
        
        total_reward += episode_reward
        total_steps += 200
        print(f"  Episode reward: {episode_reward:.2f}, Average: {episode_reward/200:.4f}")
    
    avg_reward = total_reward / 3
    print(f"\nExperiment summary:")
    print(f"  Total episodes: 3")
    print(f"  Total steps: {total_steps}")
    print(f"  Average reward per step: {avg_reward/200:.4f}")
    print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print()


def example_configuration():
    """Example 6: Advanced configuration options"""
    print("=== Example 6: Advanced Configuration ===")
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    print("Default configuration:")
    print(f"  Neuron count: {config.has('brain.neuron_count')}")
    print(f"  Synapse density: {config.get('brain.synapse_density', 'N/A')}")
    
    # Modify configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.2)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("neuromod.dopamine.scale", 2.0)
    
    print("\nModified configuration:")
    print(f"  Neuron count: {config.get('brain.neuron_count')}")
    print(f"  Synapse density: {config.get('brain.synapse_density')}")
    print(f"  STDP learning rate: {config.get('plasticity.stdp.learning_rate')}")
    print(f"  Dopamine scale: {config.get('neuromod.dopamine.scale')}")
    
    # Save configuration
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as tmp_file:
        config_path = tmp_file.name
        config.saveToFile(config_path)
        print(f"\nConfiguration saved to: {config_path}")
    
    # Load configuration
    try:
        config2 = pynlm.createDefaultConfig()
        config2.loadFromFile(config_path)
        print(f"Configuration loaded successfully:")
        print(f"  Neuron count: {config2.get('brain.neuron_count')}")
    finally:
        if os.path.exists(config_path):
            os.unlink(config_path)
    
    print()


def example_error_handling():
    """Example 7: Error handling with custom exceptions"""
    print("=== Example 7: Error Handling ===")
    
    try:
        # Try to create a brain with invalid configuration
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Try to save to invalid path
        print("Attempting to save to invalid path...")
        if not brain.save("/invalid/path/brain.bin"):
            print("Save failed as expected (invalid path)")
        
        # Try to load from invalid path
        print("Attempting to load from invalid path...")
        config2 = pynlm.createDefaultConfig()
        brain2 = pynlm.createBrain(config2)
        brain2.initialize()
        
        if not brain2.load("/invalid/path/brain.bin"):
            print("Load failed as expected (invalid path)")
            
    except Exception as e:
        print(f"Caught exception: {type(e).__name__}: {e}")
    
    print("Error handling demonstration complete")
    print()


def example_neuromodulation():
    """Example 8: Neuromodulation and learning"""
    print("=== Example 8: Neuromodulation and Learning ===")
    
    # Create complete agent system
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Initial neuromodulation levels:")
    print(f"  Dopamine: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    
    # Run simulation to observe neuromodulation
    print("\nRunning simulation to observe neuromodulation...")
    for step in range(300):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Simulate reward
        reward = 1.0 if step % 50 == 0 else 0.1
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        if step % 100 == 0:
            print(f"Step {step}: Dopamine={agent.getNeuromodulationLevel():.3f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, Novelty={agent.getNoveltyLevel():.3f}")
    
    print("\nFinal neuromodulation levels:")
    print(f"  Dopamine: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    print()


def example_memory_systems():
    """Example 9: Working memory and episodic memory"""
    print("=== Example 9: Memory Systems ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Memory system availability:")
    print(f"  Working Memory: {brain.getWorkingMemory() is not None}")
    print(f"  Episodic Memory: {brain.getEpisodicMemory() is not None}")
    
    # Run simulation to populate memory
    print("\nRunning simulation to populate memory systems...")
    for step in range(500):
        brain.step(step)
        
        if step % 100 == 0:
            working_mem = brain.getWorkingMemory()
            episodic_mem = brain.getEpisodicMemory()
            
            print(f"Step {step}:")
            if working_mem:
                print(f"  Working memory traces: {working_mem.getActiveTraces()}")
            if episodic_mem:
                print(f"  Episodic memory episodes: {episodic_mem.getEpisodeCount()}")
    
    # Check memory contents
    working_mem = brain.getWorkingMemory()
    episodic_mem = brain.getEpisodicMemory()
    
    print(f"\nFinal memory status:")
    if working_mem:
        print(f"  Working memory traces: {working_mem.getActiveTraces()}")
    if episodic_mem:
        print(f"  Episodic memory episodes: {episodic_mem.getEpisodeCount()}")
    
    # Test replay
    if episodic_mem and episodic_mem.getEpisodeCount() > 0:
        print(f"\nTesting replay system...")
        episodes = episodic_mem.getEpisodesForReplay(3)
        print(f"Retrieved {len(episodes)} episodes for replay")
    
    print()


def example_progress_tracker():
    """Example 10: Using ProgressTracker utility"""
    print("=== Example 10: Using ProgressTracker Utility ===")
    
    # Create experiment with progress tracking
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print("Running experiment with progress tracking...")
    start_time = time.time()
    
    for step in range(500):
        brain.step(step)
        
        # Progress tracking (simulated)
        if step % 50 == 0:
            progress = (step / 500) * 100
            print(f"Progress: {progress:.1f}%, Firing rate: {brain.getAverageFiringRate():.2f}")
    
    end_time = time.time()
    print(f"Experiment completed in {end_time - start_time:.2f} seconds")
    print(f"Final statistics:")
    print(f"  Firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Active neurons: {brain.getFiringNeuronCount()}")
    print()


def main():
    """Run all examples"""
    print("NLM Python Bindings - Comprehensive Examples")
    print("=" * 60)
    print()
    
    examples = [
        example_basic_brain_creation,
        example_progress_tracking,
        example_agent_world_simulation,
        example_memory_management,
        example_utilities,
        example_configuration,
        example_error_handling,
        example_neuromodulation,
        example_memory_systems,
        example_progress_tracker,
    ]
    
    for i, example in enumerate(examples, 1):
        try:
            example()
        except Exception as e:
            print(f"❌ Example {i} failed with error: {e}")
            import traceback
            traceback.print_exc()
            print()
    
    print("=" * 60)
    print("All examples completed!")
    print()
    print("To see more detailed examples, check:")
    print("  - python/pynlm/__init__.py - Module-level documentation")
    print("  - python/utils.py - Utility class documentation")
    print("  - docs/ - Detailed documentation in the repository")


if __name__ == "__main__":
    main()
