#!/usr/bin/env python3
"""
Advanced NLM Usage Examples

This file demonstrates advanced usage patterns for the NLM Python API.
These examples are intended for users who already have basic NLM experience
and want to leverage the full power of the neural simulation framework.
"""

import pynlm
import json
import numpy as np
from pathlib import Path

def example_advanced_configuration():
    """
    Advanced configuration example showing JSON configuration
    and parameter optimization for complex simulations.
    """
    print("=== Advanced Configuration Example ===")
    
    # Create configuration from JSON with advanced parameters
    advanced_config_json = """
    {
        "brain": {
            "neuron_count": 5000,
            "region_count": 3,
            "connection_probability": 0.15,
            "initial_weight_mean": 0.5,
            "initial_weight_std": 0.08
        },
        "plasticity": {
            "stdp": {
                "enable": true,
                "learning_rate": 0.002,
                "tau_plus": 25.0,
                "tau_minus": 25.0
            },
            "hebbian": {
                "enable": true,
                "learning_rate": 0.001
            },
            "structural": {
                "enable": true,
                "synaptogenesis_rate": 0.002,
                "pruning_rate": 0.0002
            }
        },
        "neuromodulation": {
            "dopamine": {
                "scale": 1.5
            },
            "curiosity": {
                "enable": true
            },
            "novelty": {
                "enable": true
            }
        },
        "development": {
            "synaptogenesis_rate": 0.003,
            "pruning_rate": 0.0003,
            "maturation_rate": 0.01
        },
        "memory": {
            "working_memory_capacity": 100,
            "episodic_memory_size": 1000
        },
        "simulation": {
            "timestep": 0.001,
            "max_steps": 50000,
            "enable_checkpointing": true,
            "checkpoint_interval": 1000
        }
    }
    """
    
    config = pynlm.createConfigFromJSON(advanced_config_json)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Advanced brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Configuration summary: {brain.getConfig().summary()}")
    
    return brain

def example_batch_operations():
    """
    Demonstrate batch operations and efficient simulation control.
    """
    print("\n=== Batch Operations Example ===")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.2)
    
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
    world.setRandomSeed(12345)
    
    # Enable all advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation in batches for better performance monitoring
    batch_size = 1000
    num_batches = 20
    
    for batch in range(num_batches):
        batch_start_time = world.getSimulationTime()
        batch_steps = min(batch_size, 50000 - batch * batch_size)
        
        for step in range(batch_steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(batch * batch_size + step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        batch_end_time = world.getSimulationTime()
        batch_duration = batch_end_time - batch_start_time
        
        # Get batch statistics
        firing_rate = brain.getAverageFiringRate()
        neurons_firing = brain.getFiringNeuronCount()
        total_spikes = brain.getTotalSpikeCount()
        curiosity_level = agent.getCuriosityLevel()
        novelty_level = agent.getNoveltyLevel()
        
        print(f"Batch {batch + 1}: {batch_steps} steps in {batch_duration:.2f}s, "
              f"firing_rate={firing_rate:.1f}Hz, "
              f"curiosity={curiosity_level:.3f}, novelty={novelty_level:.3f}")
    
    return brain, agent, world

def example_reinforcement_learning():
    """
    Reinforcement learning example with reward shaping and memory.
    """
    print("\n=== Reinforcement Learning Example ===")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    
    # Enable reinforcement learning components
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Create a simple reward function
    def get_reward(action, percept):
        """Example reward function that encourages exploration and efficient behavior"""
        reward = 0.0
        
        # Positive reward for moving forward (progress)
        if action == pynlm.MotorCommand.MoveForward:
            reward += 1.0
        
        # Positive reward for exploring (curiosity bonus)
        if agent.getCuriosityLevel() > 0.5:
            reward += 0.5
            
        # Negative reward for hitting obstacles/hazards
        vision = percept.getVision()
        if any(v < 0.3 for v in vision):  # Hazard in vision
            reward -= 2.0
            
        # Positive reward for discovering resources
        if any(v > 0.7 for v in vision):  # Resource in vision
            reward += 1.5
            
        return reward
    
    # Run training episode
    world.reset()
    total_reward = 0.0
    episode_length = 0
    max_steps = 1000
    
    print("Starting reinforcement learning episode...")
    
    for step in range(max_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Get reward based on current state
        reward = get_reward(action, percept)
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply neuromodulation with prediction
        predicted_reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted_reward)
        
        total_reward += reward
        episode_length += 1
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}: reward={reward:.2f}, "
                  f"curiosity={agent.getCuriosityLevel():.3f}, "
                  f"total={total_reward:.2f}")
        
        # Stop if agent has learned well enough or reached max steps
        if agent.getCuriosityLevel() < 0.1 and episode_length > 200:
            print(f"Episode completed early at step {step}")
            break
    
    print(f"Episode finished: {episode_length} steps, {total_reward:.2f} total reward")
    print(f"Final learned preference: {agent.getLearnedPreference():.3f}")
    
    return brain, agent, world, total_reward

def example_checkpoints_and_persistence():
    """
    Example demonstrating checkpoint save/load functionality.
    """
    print("\n=== Checkpoint and Persistence Example ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some steps to create a trained brain state
    print("Training brain for checkpoint...")
    for step in range(500):
        brain.step(step)
    
    print(f"Brain state: {brain.getTotalNeuronCount()} neurons, "
          f"{brain.getTotalSynapseCount()} synapses, "
          f"{brain.getTotalSpikeCount()} spikes")
    
    # Save checkpoint
    checkpoint_file = "trained_brain_checkpoint.bin"
    print(f"Saving checkpoint to {checkpoint_file}")
    brain.save(checkpoint_file)
    
    # Create a new brain and load the checkpoint
    config2 = pynlm.createDefaultConfig()
    brain2 = pynlm.createBrain(config2)
    
    print(f"Loading checkpoint into new brain...")
    loaded = brain2.load(checkpoint_file)
    if loaded:
        print(f"Checkpoint loaded successfully!")
        print(f"Loaded brain state: {brain2.getTotalNeuronCount()} neurons, "
              f"{brain2.getTotalSynapseCount()} synapses, "
              f"{brain2.getTotalSpikeCount()} spikes")
    else:
        print("Failed to load checkpoint!")
    
    # Clean up
    Path(checkpoint_file).unlink(missing_ok=True)
    
    return brain, brain2

def example_advanced_world_interaction():
    """
    Advanced world interaction with complex object management.
    """
    print("\n=== Advanced World Interaction Example ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=16, visionHeight=16)
    world.setMaxEnergy(200.0)
    
    # Create a complex world with multiple objects
    print("Creating complex world environment...")
    
    # Add resources at different positions
    world.addObject(pynlm.WorldObject(x=10.0, y=10.0, type=pynlm.WorldObjectType.Resource, value=10.0))
    world.addObject(pynlm.WorldObject(x=30.0, y=30.0, type=pynlm.WorldObjectType.Resource, value=15.0))
    world.addObject(pynlm.WorldObject(x=20.0, y=20.0, type=pynlm.WorldObjectType.Hazard, value=5.0))
    world.addObject(pynlm.WorldObject(x=5.0, y=35.0, type=pynlm.WorldObjectType.Wall, value=1.0))
    
    # Set agent starting position
    world.setAgentStart(5.0, 5.0)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation with object interaction
    episode_reward = 0.0
    objects_interacted = 0
    
    for step in range(300):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Apply action and get reward
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get internal reward signals
        internal_signals = percept.getInternal()
        if internal_signals:
            episode_reward += internal_signals[0]
        
        # Count object interactions (simplified)
        if step % 50 == 0:
            objects_interacted += 1
        
        if step % 100 == 0:
            print(f"Step {step}: reward={internal_signals[0] if internal_signals else 0:.2f}, "
                  f"energy={agent.getBrain().getLearnedPreference():.3f}")
    
    print(f"Episode complete: {episode_reward:.2f} total reward, "
          f"{objects_interacted} object interactions")
    
    return brain, agent, world

def example_json_configuration_management():
    """
    Demonstrate advanced JSON configuration management.
    """
    print("\n=== JSON Configuration Management Example ===")
    
    # Create initial configuration
    config1 = pynlm.createDefaultConfig()
    config1.set("brain.neuron_count", 1000)
    config1.set("brain.synapse_density", 0.1)
    config1.set("plasticity.stdp.learning_rate", 0.001)
    
    # Save to JSON
    config_file = "initial_config.json"
    config1.saveToFile(config_file)
    print(f"Saved initial configuration to {config_file}")
    
    # Load from file
    config2 = pynlm.createConfigFromFile(config_file)
    print(f"Loaded configuration from {config_file}")
    
    # Modify loaded configuration
    config2.set("brain.neuron_count", 2000)
    config2.set("neuromodulation.curiosity.enable", True)
    
    # Create brain with modified configuration
    brain = pynlm.createBrain(config2)
    brain.initialize()
    
    print(f"Modified brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Configuration: {brain.getConfig().summary()}")
    
    # Clean up
    Path(config_file).unlink(missing_ok=True)
    
    return brain, config2

if __name__ == "__main__":
    print("Advanced NLM Usage Examples")
    print("=" * 50)
    
    try:
        example_advanced_configuration()
        example_batch_operations()
        example_reinforcement_learning()
        example_checkpoints_and_persistence()
        example_advanced_world_interaction()
        example_json_configuration_management()
        
        print("\n" + "=" * 50)
        print("All advanced examples completed successfully!")
        print("\nThese examples demonstrate:")
        print("1. Advanced JSON configuration loading")
        print("2. Batch operations for efficient simulation")
        print("3. Reinforcement learning with reward shaping")
        print("4. Checkpoint save/load for persistence")
        print("5. Complex world interaction with object management")
        print("6. Advanced configuration file management")
        
    except Exception as e:
        print(f"\nError running examples: {e}")
        import traceback
        traceback.print_exc()