"""
NLM Python Bindings Examples Runner

This script provides an interface for running all the NLM Python bindings examples.
It demonstrates the core functionality and capabilities of the NLM Python API.
"""

import sys
import os
import time
from pathlib import Path

# Add current directory to path for imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
import numpy as np


def print_header(title):
    """Print formatted header."""
    print("\n" + "=" * 70)
    print(f" {title}")
    print("=" * 70)

def print_footer():
    """Print formatted footer."""
    print("\n" + "=" * 70)
    print(" Example completed successfully!")
    print("=" * 70)

def example_1_basic_brain():
    """Example 1: Basic brain creation and initialization."""
    print_header("Example 1: Basic Brain Creation")
    
    # Create a brain with default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run some simulation steps
    for i in range(10):
        brain.step(i)
    
    # Get some statistics
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"Developmental stage: {brain.getDevelopmentalStage().name}")
    
    print_footer()

def example_2_sensorimotor_loop():
    """Example 2: Complete sensorimotor loop."""
    print_header("Example 2: Sensorimotor Loop")
    
    # Create everything
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation loop
    print("Running sensorimotor simulation...")
    for step in range(50):
        world.update(0.1)  # Update world
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Tell the brain
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Agent decides action
        action = agent.decodeMotorCommand()
        
        # Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 10 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"action: {action.name}")
    
    print_footer()

def example_3_memory_and_development():
    """Example 3: Memory and development systems."""
    print_header("Example 3: Memory and Development")
    
    # Create configuration with development enabled
    config = pynlm.createDefaultConfig()
    config.set("enable_development", True)
    config.set("enable_curiosity", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(pynlm.createSimpleWorld())
    
    # Run longer simulation to observe development
    print("Running development simulation...")
    for step in range(100):
        brain.step(step)
        agent.updateDevelopment(0.01)
        
        if step % 25 == 0:
            stage = brain.getDevelopmentalStage()
            curiosity = agent.getCuriosityLevel()
            neuromodulation = agent.getNeuromodulationLevel()
            print(f"Step {step}: Stage {stage.name}, "
                  f"Curiosity: {curiosity:.3f}, "
                  f"Neuromodulation: {neuromodulation:.3f}")
    
    # Check memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    print(f"Working memory capacity: {working_memory.getCapacity() if working_memory else 'N/A'}")
    print(f"Active working memory traces: {working_memory.getActiveTraces() if working_memory else 'N/A'}")
    print(f"Episodic memory episodes: {episodic_memory.getEpisodeCount() if episodic_memory else 'N/A'}")
    
    print_footer()

def example_4_checkpointing():
    """Example 4: Checkpoint save/load."""
    print_header("Example 4: Checkpointing")
    
    # Create and initialize brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run some steps to generate interesting brain state
    for step in range(50):
        brain.step(step)
    
    # Save checkpoint
    checkpoint_path = "/tmp/nlm_checkpoint_example.bin"
    print(f"Saving checkpoint to: {checkpoint_path}")
    
    if brain.save(checkpoint_path):
        print("✓ Checkpoint saved successfully")
        
        # Create new brain and load
        brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
        brain2.initialize()
        
        if brain2.load(checkpoint_path):
            print("✓ Checkpoint loaded successfully")
            
            # Compare statistics
            print(f"Original brain spikes: {brain.getTotalSpikeCount()}")
            print(f"Loaded brain spikes: {brain2.getTotalSpikeCount()}")
        else:
            print("✗ Failed to load checkpoint")
    else:
        print("✗ Failed to save checkpoint")
    
    print_footer()

def example_5_advanced_configuration():
    """Example 5: Advanced configuration."""
    print_header("Example 5: Advanced Configuration")
    
    # Create custom configuration
    config = pynlm.config.create_simulation_config(
        neuron_count=1000,
        region_count=2,
        connection_probability=0.2,
        simulation_timestep=0.001,
        enable_development=True,
        enable_curiosity=True,
        enable_reward_modulation=True,
        enable_structural_plasticity=True,
    )
    
    # Validate configuration
    if pynlm.config.validate_config(config):
        print("✓ Configuration is valid")
    else:
        print("✗ Configuration is invalid")
        return
    
    # Use the configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation with custom settings
    for step in range(75):
        brain.step(step)
    
    print(f"Brain created with custom settings: {brain.getTotalNeuronCount()} neurons")
    print(f"Connection probability: {config.get_float('connection_probability')}")
    print(f"Development enabled: {config.get('enable_development')}")
    
    print_footer()

def example_6_neuromodulation():
    """Example 6: Neuromodulation and learning."""
    print_header("Example 6: Neuromodulation and Learning")
    
    # Create configuration with neuromodulation
    config = pynlm.createDefaultConfig()
    config.set("enable_reward_modulation", True)
    config.set("enable_curiosity", True)
    
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    
    # Run simulation with neuromodulation
    print("Running neuromodulation simulation...")
    for step in range(80):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.get_simulation_time())
        
        # Apply reward modulation (simulate reward)
        reward = 1.0 if action != pynlm.MotorCommand.Wait else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.01)
        
        if step % 20 == 0:
            neuromod = agent.getNeuromodulationLevel()
            curiosity = agent.getCuriosityLevel()
            print(f"Step {step}: Neuromodulation: {neuromod:.3f}, "
                  f"Curiosity: {curiosity:.3f}")
    
    print_footer()

def example_7_multiple_agents():
    """Example 7: Multiple agents and world interaction."""
    print_header("Example 7: Multiple Agents")
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=8, visionHeight=8)
    
    # Create multiple agents
    agents = []
    brains = []
    
    for i in range(3):
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        brains.append(brain)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enable_curiosity(True)
        agents.append(agent)
    
    # Run simulation with multiple agents
    print("Running multi-agent simulation...")
    for step in range(60):
        world.update(0.1)
        
        for i, agent in enumerate(agents):
            # Each agent gets its own perception
            percept = world.getSensoryPercept()
            
            # Agent processes input
            agent.processSensoryInput(percept)
            
            # Brain processes
            agent.getBrain().step(step)
            
            # Agent decides action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.get_simulation_time())
            
            # Apply some reward based on agent index
            reward = 1.0 if (step + i) % 10 == 0 else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.01)
        
        if step % 20 == 0:
            print(f"Step {step}: World simulation with {len(agents)} agents")
    
    print_footer()

def run_all_examples():
    """Run all examples in sequence."""
    print_header("NLM Python Bindings - Complete Examples")
    print("This script demonstrates the core functionality and capabilities")
    print("of the NLM Python API through a series of practical examples.")
    print()
    print("Each example demonstrates a specific aspect of NLM:")
    print("  - Basic brain operations")
    print("  - Complete sensorimotor loops")
    print("  - Memory and development systems")
    print("  - Checkpoint persistence")
    print("  - Advanced configuration")
    print("  - Neuromodulation and learning")
    print("  - Multi-agent interactions")
    print()
    print("Examples will run sequentially. Press Ctrl+C to interrupt.")
    print()
    
    examples = [
        ("Basic Brain Operations", example_1_basic_brain),
        ("Sensorimotor Loop", example_2_sensorimotor_loop),
        ("Memory and Development", example_3_memory_and_development),
        ("Checkpointing", example_4_checkpointing),
        ("Advanced Configuration", example_5_advanced_configuration),
        ("Neuromodulation and Learning", example_6_neuromodulation),
        ("Multiple Agents", example_7_multiple_agents),
    ]
    
    completed = 0
    failed = 0
    
    for name, example_func in examples:
        print(f"\nRunning: {name}")
        try:
            example_func()
            completed += 1
            print(f"✓ {name} completed successfully")
        except Exception as e:
            failed += 1
            print(f"✗ {name} failed: {e}")
            import traceback
            traceback.print_exc()
    
    print_header("Examples Summary")
    print(f"Total examples: {len(examples)}")
    print(f"Completed: {completed}")
    print(f"Failed: {failed}")
    print(f"Success rate: {completed/len(examples)*100:.1f}%")
    
    if failed > 0:
        print(f"\n⚠ {failed} examples failed. Check error messages above.")
    
    print("\nAll examples have been demonstrated!")
    print("\nTo learn more about NLM Python bindings:")
    print("  - Read the main README.md for installation instructions")
    print("  - Explore the examples/beginner directory for more examples")
    print("  - Check the PRIORITY_IMPROVEMENTS.md for development roadmap")
    print("  - Visit the NLM documentation for API reference")


if __name__ == "__main__":
    run_all_examples()
