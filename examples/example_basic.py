#!/usr/bin/env python3
# NLM Example Scripts for Phase 2
# Python examples for using the NLM (Neural Learning Machine)
# These scripts demonstrate various usage patterns and features

"""
NLM Example Scripts
===================

This directory contains example scripts that demonstrate how to use the NLM
(Neural Learning Machine) framework. These examples cover:

1. Basic Brain Setup
2. Environment Interaction
3. Learning and Plasticity
4. Advanced Features
5. Performance Testing

Each example is designed to be self-contained and can be run independently.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
import json
import time
from typing import List, Dict, Any

def example_basic_usage():
    """
    Example 1: Basic Brain Setup
    ==============================
    This example shows how to create and initialize a basic NLM brain.
    """
    print("=== Example 1: Basic Brain Setup ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Optionally customize configuration
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Initial firing rate: {brain.getAverageFiringRate():.4f} Hz")
    
    # Run simulation
    print("\nRunning simulation...")
    for i in range(100):
        brain.step(i)
        if i % 20 == 0:
            print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
    
    print(f"Final firing rate: {brain.getAverageFiringRate():.4f} Hz")
    print("✓ Basic brain setup completed successfully")
    print()

def example_brain_in_world():
    """
    Example 2: Brain in a Simple World
    ==================================
    This example shows how to create a complete agent-world interaction system.
    """
    print("=== Example 2: Brain in a Simple World ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Create brain and world
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning capabilities
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run complete agent simulation
    print("Running complete agent simulation...")
    for step in range(200):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.0
        if world.getSensoryPercept().getInternal():
            reward = world.getSensoryPercept().getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    print("✓ Complete agent simulation completed successfully")
    print()

def example_advanced_features():
    """
    Example 3: Advanced Features
    ==============================
    This example demonstrates advanced NLM features and subsystems.
    """
    print("=== Example 3: Advanced Features ===")
    
    # Create configuration with advanced settings
    config = pynlm.createDefaultConfig()
    
    # Configure advanced features
    config.set("brain.neuron_count", 2000)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 2.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.prediction_error.enable", True)
    
    # Create brain with advanced features
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with all subsystems
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enablePredictionError(True)
    
    # Setup advanced configuration
    config.set("development.stage_duration.initial", 60)
    config.set("development.stage_duration.critical_period", 300)
    config.set("development.stage_duration.maturation", 600)
    config.set("neuromod.dopamine.baseline", 0.1)
    config.set("neuromod.dopamine.peak", 2.0)
    
    # Run simulation with advanced features
    print("Running simulation with advanced features...")
    start_time = time.time()
    
    for step in range(500):
        # Update world
        world.update(0.05)
        
        # Get percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply neuromodulation
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.05)
        
        # Print status every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Neurons: {brain.getTotalNeuronCount()}, Firing: {brain.getFiringNeuronCount()}")
            print(f"  Spike Count: {brain.getTotalSpikeCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction Error: {agent.getPredictionError():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"\nSimulation completed in {duration:.2f} seconds")
    print(f"Average speed: {500/duration:.1f} steps/second")
    print("✓ Advanced features simulation completed successfully")
    print()

def example_performance_test():
    """
    Example 4: Performance Test
    ============================
    This example tests NLM performance with different configurations.
    """
    print("=== Example 4: Performance Test ===")
    
    configurations = [
        {"name": "Small", "neurons": 500, "steps": 1000},
        {"name": "Medium", "neurons": 1000, "steps": 2000},
        {"name": "Large", "neurons": 2000, "steps": 3000}
    ]
    
    for config_spec in configurations:
        print(f"\nTesting {config_spec['name']} configuration...")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", config_spec['neurons'])
        config.set("brain.synapse_density", 0.1)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        
        for step in range(config_spec['steps']):
            brain.step(step)
        
        end_time = time.time()
        duration = end_time - start_time
        
        print(f"  Configuration: {config_spec['name']}")
        print(f"  Neurons: {config_spec['neurons']}")
        print(f"  Steps: {config_spec['steps']}")
        print(f"  Duration: {duration:.2f} seconds")
        print(f"  Speed: {config_spec['steps']/duration:.1f} steps/second")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.4f} Hz")
    
    print("\n✓ Performance test completed successfully")
    print()

def example_configuration_export():
    """
    Example 5: Configuration Export/Import
    ======================================
    This example shows how to save and load NLM configurations.
    """
    print("=== Example 5: Configuration Export/Import ===")
    
    # Create and customize configuration
    config = pynlm.createDefaultConfig()
    
    # Customize various settings
    config.set("brain.neuron_count", 1500)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("simulation_timestep", 0.001)
    
    # Export configuration
    config_file = "example_config.cfg"
    config.saveToFile(config_file)
    print(f"Configuration exported to: {config_file}")
    
    # Show configuration content
    print("\nExported configuration content:")
    with open(config_file, 'r') as f:
        print(f.read())
    
    # Import configuration
    print("\nImporting configuration...")
    
    # Create new config and import
    imported_config = pynlm.createDefaultConfig()
    imported_config.loadFromFile(config_file)
    
    print(f"Imported neuron count: {imported_config.get('brain.neuron_count', 0)}")
    print(f"Imported synapse density: {imported_config.get('brain.synapse_density', 0.0)}")
    print(f"STDP enabled: {imported_config.get('plasticity.stdp.enable', False)}")
    print(f"Hebbian enabled: {imported_config.get('plasticity.hebbian.enable', False)}")
    
    # Create brain with imported config
    brain = pynlm.createBrain(imported_config)
    brain.initialize()
    
    print(f"\nBrain created with imported config:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    
    # Clean up
    import os
    os.remove(config_file)
    
    print("\n✓ Configuration export/import example completed successfully")
    print()

def example_reinforcement_learning():
    """
    Example 6: Reinforcement Learning Integration
    =================================================
    This example shows how to integrate NLM with reinforcement learning.
    """
    print("=== Example 6: Reinforcement Learning Integration ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Configure for reinforcement learning
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("simulation_timestep", 0.01)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable RL-specific features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Simple RL loop
    print("Running reinforcement learning simulation...")
    print("(This simulates an agent learning to navigate a simple environment)")
    
    for episode in range(5):
        print(f"\nEpisode {episode + 1}:")
        
        # Reset world for new episode
        world.reset()
        
        # Episode loop
        for step in range(100):
            # Update world
            world.update(0.1)
            
            # Get percept
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            
            # Apply reward modulation
            agent.applyRewardModulation(reward, 0.0)
            
            # Print status
            if step % 20 == 0:
                print(f"  Step {step}: Reward = {reward:.2f}, Curiosity = {agent.getCuriosityLevel():.3f}")
    
    print("\n✓ Reinforcement learning integration example completed successfully")
    print()

def main():
    """Main function to run all examples"""
    print("=" * 60)
    print("NLM (Neural Learning Machine) Example Scripts")
    print("Phase 2: Real Neural Computation")
    print("=" * 60)
    print()
    
    examples = [
        ("Basic Brain Setup", example_basic_usage),
        ("Brain in a Simple World", example_brain_in_world),
        ("Advanced Features", example_advanced_features),
        ("Performance Test", example_performance_test),
        ("Configuration Export/Import", example_configuration_export),
        ("Reinforcement Learning Integration", example_reinforcement_learning),
    ]
    
    for example_name, example_func in examples:
        try:
            example_func()
        except Exception as e:
            print(f"Error in {example_name}: {e}")
            import traceback
            traceback.print_exc()
            print()
    
    print("=" * 60)
    print("All examples completed!")
    print("=" * 60)

if __name__ == "__main__":
    main()