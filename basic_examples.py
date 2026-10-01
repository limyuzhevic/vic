#!/usr/bin/env python3
"""
NLM - Basic Usage Examples

This script demonstrates the enhanced Python API for NLM (Neural Learning Machine).
It covers basic usage, agent learning, and configuration management.

To run: python basic_examples.py
"""

import sys
import os
import json
import time

# Add the current directory to the path for imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import pynlm - this will use our enhanced bindings
import pynlm

def example_1_basic_brain():
    """Example 1: Basic brain simulation"""
    print("=== Example 1: Basic Brain Simulation ===")
    
    # Create default configuration
    config = pynlm.ConfigWrapper()
    
    # Configure brain parameters
    config.configureBrain(neuronCount=500, connectionProbability=0.1)
    
    # Create brain
    brain = pynlm.BrainWrapper(config.getConfig())
    
    print(f"Created brain with {brain.getStatistics()['total_neurons']} neurons")
    
    # Initialize brain
    if brain.initialize():
        print("Brain initialized successfully")
    else:
        print("Brain initialization failed")
        return
    
    # Run simulation
    print("Running 100 simulation steps...")
    start_time = time.time()
    brain.runSimulation(100)
    end_time = time.time()
    
    # Get statistics
    stats = brain.getStatistics()
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    print(f"Total neurons: {stats['total_neurons']}")
    print(f"Total synapses: {stats['total_synapses']}")
    print(f"Firing neurons: {stats['firing_neurons']}")
    print(f"Total spikes: {stats['total_spikes']}")
    print(f"Average firing rate: {stats['avg_firing_rate']:.2f} Hz")
    print()

def example_2_agent_learning():
    """Example 2: Agent learning in a world"""
    print("=== Example 2: Agent Learning in a World ===")
    
    # Create configuration
    config = pynlm.ConfigWrapper()
    config.configureBrain(neuronCount=1000)
    config.configureWorld(width=30, height=30, visionWidth=8, visionHeight=8)
    
    # Create brain and initialize
    brain = pynlm.BrainWrapper(config.getConfig())
    if not brain.initialize():
        print("Failed to initialize brain")
        return
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(30, 30, 8, 8)
    world.reset()
    
    # Set agent starting position
    world.setAgentStart(15.0, 15.0)
    
    # Create agent brain
    agent = pynlm.createAgentBrain(brain.getBrain())
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Running agent simulation with learning...")
    print(f"Initial development stage: {agent.getDevelopmentalStage()}")
    print(f"Initial curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Initial novelty level: {agent.getNoveltyLevel():.3f}")
    
    # Run simulation
    for step in range(100):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getStatistics()['firing_neurons']}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {agent.getDevelopmentalStage()}")
    
    print("Agent simulation completed!")
    print()

def example_3_configuration_management():
    """Example 3: Configuration management"""
    print("=== Example 3: Configuration Management ===")
    
    # Try to load configuration from file
    try:
        config = pynlm.createConfig("configs/default.cfg")
        print("Loaded configuration from file")
        
        # Validate configuration
        validation_result = pynlm.validateConfiguration("configs/default.cfg")
        print(f"Configuration valid: {validation_result['valid']}")
        print(f"Has neuron count: {validation_result['has_neuron_count']}")
        print(f"Has connection probability: {validation_result['has_connection_probability']}")
        
    except Exception as e:
        print(f"Failed to load config file: {e}")
        print("Creating default configuration...")
        
        config = pynlm.ConfigWrapper()
        config.configureBrain(neuronCount=500, connectionProbability=0.05)
        config.configureWorld(width=20, height=20, visionWidth=6, visionHeight=6)
        
        # Save configuration
        config.getConfig().saveToFile("config_example.json")
        print("Saved example configuration to config_example.json")
    
    # Create brain with configuration
    brain = pynlm.BrainWrapper(config.getConfig())
    if brain.initialize():
        print("Brain created and initialized successfully")
        
        # Get configuration summary
        summary = config.summary()
        print(f"Configuration summary: {summary}")
        
        # Modify configuration
        config.setInt("brain.neuron_count", 750)
        config.setFloat("brain.connection_probability", 0.15)
        print("Modified configuration: increased neurons to 750, connection probability to 0.15")
        
        # Save modified configuration
        config.getConfig().saveToFile("modified_config.json")
        print("Saved modified configuration to modified_config.json")
        
    else:
        print("Failed to initialize brain")
    
    print()

def example_4_batch_processing():
    """Example 4: Batch processing for parameter exploration"""
    print("=== Example 4: Batch Processing for Parameter Exploration ===")
    
    # Parameter combinations to test
    neuron_counts = [500, 1000, 2000]
    connection_probs = [0.05, 0.1, 0.15]
    
    results = []
    
    for neuron_count in neuron_counts:
        for conn_prob in connection_probs:
            print(f"Testing neuron_count={neuron_count}, conn_prob={conn_prob}")
            
            # Create configuration
            config = pynlm.ConfigWrapper()
            config.configureBrain(neuronCount=neuron_count, connectionProbability=conn_prob)
            
            # Create and run brain
            brain = pynlm.BrainWrapper(config.getConfig())
            if brain.initialize():
                brain.runSimulation(50)
                
                stats = brain.getStatistics()
                result = {
                    "neuron_count": neuron_count,
                    "connection_probability": conn_prob,
                    "total_neurons": stats["total_neurons"],
                    "total_spikes": stats["total_spikes"],
                    "avg_firing_rate": stats["avg_firing_rate"],
                    "e_i_ratio": stats["e_i_ratio"]
                }
                
                results.append(result)
                print(f"  -> Total spikes: {stats['total_spikes']}")
                
            else:
                print(f"  -> Failed to initialize brain")
                results.append({
                    "neuron_count": neuron_count,
                    "connection_probability": conn_prob,
                    "error": "initialization_failed"
                })
    
    # Save results
    with open("batch_results.json", "w") as f:
        json.dump(results, f, indent=2)
    
    print(f"Batch processing completed. Results saved to batch_results.json")
    
    # Find best configuration
    successful_results = [r for r in results if "error" not in r]
    if successful_results:
        best_result = max(successful_results, key=lambda r: r["avg_firing_rate"])
        print(f"Best configuration: {best_result}")
    
    print()

def example_5_learning_agent():
    """Example 5: Complete learning agent with exploration and exploitation"""
    print("=== Example 5: Complete Learning Agent ===")
    
    # Create ready agent using the enhanced factory function
    brain, world, agent = pynlm.createReadyAgent("configs/default.cfg", 200)
    
    print("Created and initialized complete agent system")
    print(f"Initial state - Development Stage: {agent.getDevelopmentalStage()}")
    print(f"Initial state - Curiosity Level: {agent.getCuriosityLevel():.3f}")
    
    # Simulate learning with exploration
    for step in range(200):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        # Agent takes action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward from world
        internal_signals = percept.getInternal()
        reward = internal_signals[0] if internal_signals else 0.0
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 40 == 0:
            stats = brain.getStatistics()
            print(f"Step {step}: {stats['firing_neurons']} firing neurons, {agent.getCuriosityLevel():.3f} curiosity")
    
    print("Learning agent simulation completed!")
    print(f"Final statistics: {brain.getStatistics()}")
    print()

def example_6_quick_test():
    """Example 6: Quick test utility"""
    print("=== Example 6: Quick Test Utility ===")
    
    try:
        result = pynlm.runQuickTest()
        print("Quick test completed successfully!")
        for key, value in result.items():
            print(f"  {key}: {value}")
    except Exception as e:
        print(f"Quick test failed: {e}")
    
    print()

if __name__ == "__main__":
    print("NLM Python Examples - Enhanced API")
    print("====================================")
    print()
    
    try:
        example_1_basic_brain()
        example_2_agent_learning()
        example_3_configuration_management()
        example_4_batch_processing()
        example_5_learning_agent()
        example_6_quick_test()
        
        print("All examples completed successfully!")
        
    except Exception as e:
        print(f"Error running examples: {e}")
        sys.exit(1)
