#!/usr/bin/env python3
"""
Comprehensive Python Examples for NLM (Neural Learning Machine)

This file demonstrates all the enhanced Pythonic features available in pynlm,
including context managers, convenience methods, and advanced debugging tools.

Run this script with: python examples.py
"""

import pynlm
import time

def example_basic_usage():
    """Example 1: Basic usage with enhanced Pythonic API"""
    print("=== Example 1: Basic Enhanced Usage ===")
    
    # Create brain using enhanced convenience methods
    config = pynlm.createDefaultConfig()
    
    # Configure with Pythonic dictionary-style access
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    
    print(f"Configuration created with {len(config.getKeys())} keys")
    print(f"Neuron count: {config.get_int('brain.neuron_count', 1000)}")
    print(f"Synapse density: {config.get_float('brain.synapse_density', 0.1)}")
    print(f"STDP enabled: {config.get_bool('plasticity.stdp.enable', True)}")
    
    # Create brain with context manager for automatic resource management
    print("\n--- Using BrainContext for automatic management ---")
    with pynlm.BrainContext(pynlm.createBrain(config)) as brain:
        brain.initialize()
        
        # Use simulate() convenience method
        print("Running simulation with enhanced convenience methods...")
        brain.simulate(steps=200, timestep=0.05, verbose=True)
        
        # Get comprehensive statistics
        stats = brain.get_statistics()
        print(f"\nBrain Statistics:")
        print(f"  Total neurons: {stats['total_neurons']}")
        print(f"  Total synapses: {stats['total_synapses']}")
        print(f"  Firing neurons: {stats['firing_neurons']}")
        print(f"  Average firing rate: {stats['average_firing_rate']:.3f} Hz")
        print(f"  Development stage: {stats['developmental_stage']}")
        
        # Use visualize_brain() for basic debugging
        brain.visualize_brain()
        
    print("Brain automatically reset and resources cleaned up\n")

def example_agent_with_world():
    """Example 2: Complete agent simulation with enhanced world interaction"""
    print("=== Example 2: Enhanced Agent-World Simulation ===")
    
    # Create components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setRandomSeed(42)
    
    # Initialize with learning subsystems
    agent.initialize(world)
    agent.setup_learning(reward=True, curiosity=True, development=True)
    
    print(f"Agent initialized with world {world.getWidth()}x{world.getHeight()}")
    print(f"Agent stats: {agent.get_agent_stats()}")
    
    # Run complete episode using enhanced method
    print("\n--- Running complete episode ---")
    start_time = time.time()
    
    # The run_episode method from Brain handles the complete loop
    brain.run_episode(world, agent, steps=100, timestep=0.1, verbose=True)
    
    end_time = time.time()
    print(f"Episode completed in {end_time - start_time:.2f} seconds")
    
    # Show final agent statistics
    final_stats = agent.get_agent_stats()
    print(f"\nFinal Agent Stats:")
    print(f"  Curiosity level: {final_stats['curiosity_level']:.3f}")
    print(f"  Novelty level: {final_stats['novelty_level']:.3f}")
    print(f"  Prediction error: {final_stats['prediction_error']:.3f}")
    print(f"  Neuromodulation level: {final_stats['neuromodulation_level']:.3f}")
    print(f"  Developmental stage: {final_stats['developmental_stage']}")
    
    print("\n")

def example_advanced_configuration():
    """Example 3: Advanced configuration using Pythonic APIs"""
    print("=== Example 3: Advanced Configuration ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Method 1: Direct Pythonic setting
    config["brain.neuron_count"] = 1000
    config["plasticity.stdp.learning_rate"] = 0.002
    
    # Method 2: Update multiple values at once
    updates = {
        "brain.connection_probability": 0.08,
        "neuromod.curiosity.enable": True,
        "neuromod.novelty.enable": True,
        "plasticity.hebbian.enable": True
    }
    config.update(updates)
    
    # Method 3: Save configuration as dictionary
    config_dict = config.save_dict()
    print("Configuration saved as dictionary:")
    for key, value in config_dict.items():
        print(f"  {key}: {value}")
    
    # Method 4: Create brain and demonstrate configuration flexibility
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"\nBrain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    
    # Modify configuration after creation (advanced usage)
    brain_config = brain.getConfig()
    brain_config.set("learning.rate", 0.001)
    brain_config.set("debug.level", "high")
    
    print("Configuration updated after brain creation")
    print("\n")

def example_error_handling():
    """Example 4: Enhanced error handling and debugging"""
    print("=== Example 4: Enhanced Error Handling ===")
    
    try:
        # Demonstrate Pythonic error handling
        config = pynlm.createDefaultConfig()
        
        # Try to access non-existent key with default value
        value = config.get_string("nonexistent.key", "default_value")
        print(f"Accessed non-existent key with default: {value}")
        
        # Try to use __getitem__ with error handling
        try:
            invalid_value = config["invalid.key"]
        except KeyError as e:
            print(f"Caught KeyError as expected: {e}")
        
        # Demonstrate type checking and conversion
        config["test.int"] = 42
        config["test.float"] = 3.14
        config["test.bool"] = True
        
        print("Configuration types:")
        print(f"  test.int: {config.get_int('test.int')} (type: {type(config.get_int('test.int'))})")
        print(f"  test.float: {config.get_float('test.float')} (type: {type(config.get_float('test.float'))})")
        print(f"  test.bool: {config.get_bool('test.bool')} (type: {type(config.get_bool('test.bool'))})")
        
    except Exception as e:
        print(f"Error in advanced configuration: {e}")
    
    print("\n")

def example_experimental_setup():
    """Example 5: Experimental setup with enhanced features"""
    print("=== Example 5: Experimental Setup ===")
    
    # Create comprehensive experimental setup
    config = pynlm.createDefaultConfig()
    
    # Advanced experimental configuration
    experimental_config = {
        "brain.neuron_count": 2000,
        "brain.synapse_density": 0.12,
        "plasticity.stdp.enable": True,
        "plasticity.stdp.learning_rate": 0.001,
        "plasticity.hebbian.enable": True,
        "plasticity.structural.enable": False,  # Save resources
        "neuromod.dopamine.scale": 1.0,
        "neuromod.curiosity.enable": True,
        "neuromod.curiosity.strength": 0.8,
        "neuromod.novelty.enable": True,
        "neuromod.novelty.sensitivity": 0.7,
        "development.enabled": True,
        "development.initial_stage": "CriticalPeriod",
        "debug.enabled": True,
        "debug.log_level": "INFO",
        "performance.monitoring": True
    }
    
    config.update(experimental_config)
    
    print("Experimental configuration created:")
    print(f"  Neurons: {config.get_int('brain.neuron_count')}")
    print(f"  Synapse density: {config.get_float('brain.synapse_density')}")
    print(f"  Curiosity enabled: {config.get_bool('neuromod.curiosity.enable')}")
    print(f"  Development enabled: {config.get_bool('development.enabled')}")
    
    # Create brain with advanced setup
    brain = pynlm.createBrain(config)
    
    # Use context manager for controlled experiment
    print("\n--- Running controlled experiment ---")
    with pynlm.BrainContext(brain) as experimental_brain:
        experimental_brain.initialize()
        
        # Run detailed simulation with monitoring
        experimental_brain.simulate(steps=500, timestep=0.01, verbose=True)
        
        # Get comprehensive statistics for analysis
        stats = experimental_brain.get_statistics()
        print(f"\nExperiment Results:")
        print(f"  Total neurons simulated: {stats['total_neurons']}")
        print(f"  Total spikes recorded: {stats['total_spikes']}")
        print(f"  Average firing rate: {stats['average_firing_rate']:.3f} Hz")
        print(f"  Network efficiency: {stats['firing_neurons']/stats['total_neurons']*100:.1f}%")
        
        # Demonstrate visualization
        experimental_brain.visualize_brain("experiment_output.png")
        
    print("\nExperiment completed successfully")
    print("\n")

def main():
    """Run all examples"""
    print("NLM Python Examples - Enhanced Features")
    print("=" * 50)
    print()
    
    example_basic_usage()
    example_agent_with_world()
    example_advanced_configuration()
    example_error_handling()
    example_experimental_setup()
    
    print("All examples completed successfully!")
    print("\nKey features demonstrated:")
    print("  ✓ Context managers for automatic resource management")
    print("  ✓ Pythonic convenience methods (simulate(), run_episode(), get_statistics())")
    print("  ✓ Enhanced configuration APIs (operator[], update(), save_dict())")
    print("  ✓ Error handling and type conversion")
    print("  ✓ Advanced debugging and visualization tools")
    print("  ✓ Experimental setup capabilities")

if __name__ == "__main__":
    main()
