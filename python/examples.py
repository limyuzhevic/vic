#!/usr/bin/env python3
"""
NLM Python Example Suite

This file contains comprehensive examples demonstrating the NLM Python API.
It's designed to be both educational and practical for users at all levels.

Run this file directly to see examples in action:
    python examples.py
"""

import pynlm
import json
import random
from typing import List, Dict, Any

def example_simplest():
    """The absolute simplest NLM example - just create and run a brain."""
    print("=== Example 1: Simplest Possible Brain ===")
    
    # One line to create a virtual brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Initialize it
    brain.initialize()
    
    # Make it active
    brain.step(0)
    
    print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
    print(f"It generated {brain.getTotalSpikeCount()} spikes in 1 step.")
    print()

def example_basic_simulation():
    """Basic brain simulation with statistics tracking."""
    print("=== Example 2: Basic Simulation with Statistics ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.enable", True)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    firing_counts: List[int] = []
    spike_counts: List[int] = []
    
    for step in range(100):
        brain.step(step)
        
        # Collect statistics every 10 steps
        if step % 10 == 0:
            firing = brain.getFiringNeuronCount()
            spikes = brain.getTotalSpikeCount()
            firing_counts.append(firing)
            spike_counts.append(spikes)
            print(f"Step {step}: {firing} neurons firing, {spikes} total spikes")
    
    # Calculate statistics
    avg_firing = sum(firing_counts) / len(firing_counts)
    total_spikes = spike_counts[-1] if spike_counts else 0
    
    print(f"Average firing rate: {avg_firing:.1f} neurons")
    print(f"Total spikes after 100 steps: {total_spikes}")
    print()

def example_world_interaction():
    """Brain interacting with a simulated world."""
    print("=== Example 3: World Interaction ===")
    
    # Setup brain and world
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run interaction loop
    for step in range(50):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action and execute it
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress
        if step % 10 == 0:
            print(f"Step {step}:")
            print(f"  Position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    
    print("World interaction complete!")
    print()

def example_advanced_configuration():
    """Advanced configuration using JSON and programmatic settings."""
    print("=== Example 4: Advanced Configuration ===")
    
    # Method 1: Programmatic configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("brain.synapse_density", 0.2)
    config.set("brain.v_thresh", -40.0)  # Higher threshold
    config.set("brain.tau_mem", 25.0)    # Slower membrane time
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.005)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    print("Programmatic configuration:")
    for key in config.getKeys():
        value = config.get(key) if config.has(key) else None
        print(f"  {key}: {value}")
    
    # Method 2: Save configuration to JSON
    config.saveToFile("advanced_config.json")
    print(f"Configuration saved to advanced_config.json")
    
    # Method 3: Load configuration from JSON
    new_config = pynlm.createDefaultConfig()
    new_config.loadFromFile("advanced_config.json")
    
    print("Loaded configuration keys:", new_config.getKeys())
    print()

def example_development_learning():
    """Simulation with developmental learning."""
    print("=== Example 5: Developmental Learning ===")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable developmental learning
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    development_stages = []
    
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Process input and think
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward and update development
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        current_stage = brain.getDevelopmentalStage()
        development_stages.append(current_stage)
        
        # Check for developmental stage changes
        if step % 20 == 0:
            print(f"Step {step}: Developmental stage = {current_stage}")
    
    # Analyze development
    unique_stages = len(set(development_stages))
    final_stage = development_stages[-1]
    
    print(f"Development simulation complete!")
    print(f"Stages experienced: {unique_stages}")
    print(f"Final developmental stage: {final_stage}")
    print()

def example_checkpointing():
    """Demonstrate brain checkpointing and loading."""
    print("=== Example 6: Checkpointing ===")
    
    # Create and train a brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Simulate for a while
    for step in range(50):
        brain.step(step)
    
    print(f"Initial brain state:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Spikes: {brain.getTotalSpikeCount()}")
    print(f"  Firing: {brain.getFiringNeuronCount()}")
    
    # Save checkpoint
    brain.save("test_checkpoint.bin")
    print("Brain saved to test_checkpoint.bin")
    
    # Simulate more
    for step in range(50, 100):
        brain.step(step)
    
    print(f"After additional simulation:")
    print(f"  Spikes: {brain.getTotalSpikeCount()}")
    print(f"  Firing: {brain.getFiringNeuronCount()}")
    
    # Reset and load checkpoint
    brain.reset()
    brain.load("test_checkpoint.bin")
    
    print(f"After loading checkpoint:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Spikes: {brain.getTotalSpikeCount()}")  # Should be 50, not 99
    print(f"  Firing: {brain.getFiringNeuronCount()}")
    
    print("Checkpointing demonstration complete!")
    print()

def example_noise_injection():
    """Demonstrate noise injection for exploration."""
    print("=== Example 7: Noise Injection for Exploration ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    for step in range(80):
        # Add random noise for exploration (simulating uncertainty)
        noise = random.gauss(0, 0.05)  # Gaussian noise
        world.update(0.1 + noise)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print exploration stats
        if step % 20 == 0:
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            print(f"Step {step}: Curiosity={curiosity:.3f}, Novelty={novelty:.3f}, Noise={noise:.3f}")
    
    print("Noise injection demonstration complete!")
    print()

def example_configuration_validation():
    """Demonstrate configuration validation utilities."""
    print("=== Example 8: Configuration Validation ===")
    
    # Test validation function
    valid_config = {
        "brain.neuron_count": 1000,
        "brain.synapse_density": 0.1,
        "plasticity.stdp.enable": True,
        "unknown.key": "should still work"  # Unknown but valid
    }
    
    try:
        is_valid = pynlm.validate_config(valid_config)
        print(f"Configuration validation result: {is_valid}")
    except Exception as e:
        print(f"Validation error: {e}")
    
    # List available configuration options
    print("\nAvailable configuration options:")
    configs = pynlm.list_available_configs()
    for config in configs[:5]:  # Show first 5
        print(f"  {config}")
    print(f"  ... and {len(configs) - 5} more")
    print()

def example_logging_configuration():
    """Demonstrate logging configuration."""
    print("=== Example 9: Logging Configuration ===")
    
    # Configure different logging levels
    print("Demonstrating different logging levels:")
    
    # Test different log levels
    original_level = logger.level if hasattr(logger, 'level') else logging.WARNING
    
    # Configure debug logging
    pynlm.configure_logging('DEBUG', '%(levelname)s - %(message)s')
    print("Debug logging configured")
    
    # Configure info logging
    pynlm.configure_logging('INFO', '%(asctime)s - %(message)s')
    print("Info logging configured")
    
    # Restore original logging
    pynlm.configure_logging('WARNING')
    print("Logging restored to WARNING level")
    
    print()

def example_type_hints_and_validation():
    """Demonstrate type checking and validation."""
    print("=== Example 10: Type Hints and Validation ===")
    
    # Create brain and try to use it with type safety in mind
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Type-safe operations
    steps_to_run = 30
    for step in range(steps_to_run):
        try:
            brain.step(step)
            firing = brain.getFiringNeuronCount()
            spikes = brain.getTotalSpikeCount()
            
            if firing < 0 or spikes < 0:
                raise pynlm.SimulationError("Invalid neuron or spike count")
            
            if step % 10 == 0:
                print(f"Step {step}: {firing} firing, {spikes} total spikes")
                
        except pynlm.SimulationError as e:
            print(f"Simulation error at step {step}: {e}")
            break
    
    print(f"Simulation completed with {steps_to_run} steps successfully")
    print()

def main():
    """Run all examples."""
    print("NLM Python Example Suite")
    print("=" * 50)
    print()
    
    # Set up logging
    pynlm.configure_logging('WARNING')
    
    # Run all examples
    example_simplest()
    example_basic_simulation()
    example_world_interaction()
    example_advanced_configuration()
    example_development_learning()
    example_checkpointing()
    example_noise_injection()
    example_configuration_validation()
    example_logging_configuration()
    example_type_hints_and_validation()
    
    print("=" * 50)
    print("All examples completed successfully!")
    print()
    print("For more advanced examples and documentation, see:")
    print("  - easy_usage.md: Beginner-friendly examples")
    print("  - HOW_TO_USE.md: Comprehensive technical guide")
    print("  - docs/ARCHITECTURE.md: System architecture details")

if __name__ == "__main__":
    main()