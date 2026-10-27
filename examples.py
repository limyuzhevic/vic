#!/usr/bin/env python3
"""
NLM Python Examples - Comprehensive Tutorial

This file contains comprehensive examples for using the NLM (Neural Learning Machine)
Python bindings. Each example demonstrates different aspects of the neural simulation.

Run with: python3 examples.py
"""

from pynlm import (
    createBrain, createDefaultConfig, createSimpleWorld, createAgentBrain
)
from typing import List, Optional
import time

def example_basic_brain():
    """Example 1: Basic brain simulation"""
    print("=" * 60)
    print("Example 1: Basic Brain Simulation")
    print("=" * 60)
    
    # Create a brain with default configuration
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    print("Brain initialized with:")
    print(f"  - {brain.getTotalNeuronCount()} neurons")
    print(f"  - {brain.getRegionCount()} brain regions")
    
    # Run a simple simulation
    for step in range(20):
        brain.step(step)
        
    print(f"\nSimulation complete:")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  - Development stage: {brain.getDevelopmentalStage()}")

def example_brain_world_agent():
    """Example 2: Complete brain-world-agent simulation"""
    print("\n" + "=" * 60)
    print("Example 2: Complete Brain-World-Agent Simulation")
    print("=" * 60)
    
    # Create all components
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    world = createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Starting simulation...")
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    print(f"Agent starts at: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    
    # Run simulation
    for step in range(50):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Execute action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print status every 10 steps
        if step % 10 == 0:
            print(f"Step {step}:")
            print(f"  - Agent at ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"  - Action: {action}")
            print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  - Reward modulation active: {agent.isRewardModulationEnabled()}")
            print(f"  - Curiosity active: {agent.isCuriosityEnabled()}")
    
    print(f"\nSimulation complete!")
    print(f"Final statistics:")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  - Development stage: {brain.getDevelopmentalStage()}")
    print(f"  - Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")

def example_development_simulation():
    """Example 3: Development over time"""
    print("\n" + "=" * 60)
    print("Example 3: Development Over Time")
    print("=" * 60)
    
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    world = createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable development
    agent.enableDevelopment(True)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Running development simulation (200 steps)...")
    print("Watching brain development and learning...")
    
    # Track development progress
    dev_stages = []
    firing_rates = []
    
    for step in range(200):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record development metrics
        dev_stages.append(brain.getDevelopmentalStage())
        firing_rates.append(brain.getAverageFiringRate())
        
        if step % 40 == 0:
            print(f"Step {step}:")
            print(f"  - Development stage: {brain.getDevelopmentalStage()}")
            print(f"  - Average firing rate: {brain.getAverageFiringRate():.3f}")
            print(f"  - Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
            print(f"  - Curiosity level: {agent.getCuriosityLevel():.3f}")
    
    # Analyze development
    print(f"\nDevelopment Analysis:")
    print(f"  - Initial firing rate: {firing_rates[0]:.3f}")
    print(f"  - Final firing rate: {firing_rates[-1]:.3f}")
    print(f"  - Firing rate change: {firing_rates[-1] - firing_rates[0]:+.3f}")
    
    # Count development stage transitions
    unique_stages = set(dev_stages)
    print(f"  - Development stages visited: {len(unique_stages)}")

def example_comparison_simulation():
    """Example 4: Comparing different configurations"""
    print("\n" + "=" * 60)
    print("Example 4: Comparing Different Configurations")
    print("=" * 60)
    
    from pynlm import createBrain, createDefaultConfig
    
    configs = [
        ("Small Brain", {"neuron_count": 200, "region_count": 2}),
        ("Medium Brain", {"neuron_count": 500, "region_count": 1}),
        ("Large Brain", {"neuron_count": 1000, "region_count": 2}),
    ]
    
    results = []
    
    for config_name, config_params in configs:
        print(f"\nTesting {config_name}:")
        config = createDefaultConfig()
        
        # Apply custom parameters
        for key, value in config_params.items():
            if hasattr(config, key):
                setattr(config, key, value)
        
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record results
        result = {
            'config': config_name,
            'neurons': brain.getTotalNeuronCount(),
            'regions': brain.getRegionCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing': brain.getAverageFiringRate(),
            'development_stage': brain.getDevelopmentalStage()
        }
        
        results.append(result)
        
        print(f"  - Neurons: {result['neurons']}")
        print(f"  - Total spikes: {result['total_spikes']}")
        print(f"  - Average firing rate: {result['avg_firing']:.3f}")
        print(f"  - Final development stage: {result['development_stage']}")
    
    # Comparison summary
    print(f"\nConfiguration Comparison:")
    print(f"{'Configuration':<15} {'Neurons':<8} {'Regions':<8} {'Total Spikes':<12} {'Avg Firing':<12} {'Dev Stage':<12}")
    print("-" * 70)
    for result in results:
        print(f"{result['config']:<15} {result['neurons']:<8} {result['regions']:<8} "
              f"{result['total_spikes']:<12} {result['avg_firing']:<12.3f} {result['development_stage']:<12}")

def example_checkpointing():
    """Example 5: Checkpointing (if supported)"""
    print("\n" + "=" * 60)
    print("Example 5: Checkpointing (Advanced Feature)")
    print("=" * 60)
    
    from pynlm import createBrain, createDefaultConfig
    
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    world = createSimpleWorld()
    world.configure(width=20, height=20)
    world.reset()
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    print("Running simulation with checkpoint capability...")
    print("Note: Checkpointing implementation may vary based on build configuration")
    
    # Run partial simulation
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 25 == 0:
            print(f"Step {step} completed")
    
    # Note about checkpointing
    print(f"\nCheckpointing information:")
    print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  - The brain can be saved/loaded if checkpoint support is enabled")

def example_error_handling():
    """Example 6: Error handling and robustness"""
    print("\n" + "=" * 60)
    print("Example 6: Error Handling and Robustness")
    print("=" * 60)
    
    try:
        from pynlm import createBrain, createDefaultConfig
        
        print("Testing error handling...")
        
        # Create and initialize brain
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        # Run simulation with error handling
        for step in range(10):
            try:
                brain.step(step)
                
                if step % 3 == 0:
                    # Test for zero division or other potential issues
                    firing_rate = brain.getAverageFiringRate()
                    if firing_rate != firing_rate:  # NaN check
                        print(f"Step {step}: WARNING - NaN detected!")
                
            except Exception as e:
                print(f"Step {step}: Error occurred - {e}")
                continue
        
        print(f"\nRobust simulation complete:")
        print(f"  - Total steps completed: 10")
        print(f"  - Final neuron count: {brain.getTotalNeuronCount()}")
        print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
        
    except ImportError as e:
        print(f"Import error: {e}")
        print("Make sure pynlm module is properly installed")
    except Exception as e:
        print(f"Unexpected error: {e}")

def run_all_examples():
    """Run all examples in sequence"""
    print("NLM Python Examples - Complete Tutorial")
    print("=" * 60)
    print("This script demonstrates various aspects of the NLM neural simulation")
    print("Each example showcases different features and capabilities.\n")
    
    examples = [
        ("Basic Brain", example_basic_brain),
        ("Brain-World-Agent", example_brain_world_agent),
        ("Development", example_development_simulation),
        ("Configuration Comparison", example_comparison_simulation),
        ("Checkpointing", example_checkpointing),
        ("Error Handling", example_error_handling),
    ]
    
    for name, example_func in examples:
        try:
            example_func()
        except Exception as e:
            print(f"Error in {name}: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 60)
    print("All examples completed!")
    print("=" * 60)

if __name__ == "__main__":
    run_all_examples()