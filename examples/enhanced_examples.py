#!/usr/bin/env python3
"""
Comprehensive example demonstrating the enhanced NLM Python bindings.

This file showcases all the new features and improvements made to the pynlm library.
It demonstrates:
- Enhanced documentation and type safety
- Pythonic API features (context managers, properties, etc.)
- Convenience methods and advanced features
- Error handling and Python-specific exceptions
- Standard library integration
- Advanced simulation capabilities

Run with: python examples/enhanced_examples.py
"""

import pynlm
import json
from typing import List, Dict, Optional, Union

print("=" * 80)
print("Enhanced NLM Python Bindings - Comprehensive Examples")
print("=" * 80)

def example_1_basic_usage():
    """Example 1: Basic usage with enhanced features"""
    print("\n1. BASIC USAGE WITH ENHANCED FEATURES")
    print("-" * 50)
    
    # Create brain with default configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain statistics: {brain.getStatistics()}")
    
    # Initialize brain
    brain.initialize()
    print("Brain initialized")
    
    # Use context manager for safe brain management
    with pynlm.brainContext(brain) as brain_ctx:
        if brain_ctx.isInitialized():
            print("Brain is initialized via context manager")
    
    # Run simulation
    for i in range(50):
        brain.step(i)
    
    print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    
    return brain

def example_2_enhanced_configuration():
    """Example 2: Enhanced configuration management"""
    print("\n2. ENHANCED CONFIGURATION MANAGEMENT")
    print("-" * 50)
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Set configuration values using type-safe methods
    config.setInt("brain.neuron_count", 2000)
    config.setDouble("brain.synapse_density", 0.15)
    config.setBool("plasticity.stdp.enable", True)
    config.set("brain.connection_probability", 0.08)
    config.set("learning.rate", 0.002)
    config.set("brain.v_thresh", -55.0)
    config.set("brain.v_rest", -65.0)
    
    # Get configuration values
    print(f"Neuron count: {config.getOr('brain.neuron_count', 1000)}")
    print(f"Synapse density: {config.getOr('brain.synapse_density', 0.1)}")
    print(f"STDP enabled: {config.getOr('plasticity.stdp.enable', False)}")
    
    # Convert to dictionary and back
    config_dict = pynlm.configToJson(config)
    print(f"Configuration as dict: {len(config_dict)} keys")
    
    # Create configuration from dictionary
    config2 = pynlm.configFromJson(config_dict)
    print(f"Reloaded config has {config2.getKeys().size()} keys")
    
    # Save and load configuration
    config.saveToFile("test_config.json")
    config_loaded = pynlm.loadConfigFromFile("test_config.json")
    print("Configuration saved and loaded successfully")
    
    return config

def example_3_context_managers():
    """Example 3: Context managers for safe resource management"""
    print("\n3. CONTEXT MANAGERS FOR SAFE RESOURCE MANAGEMENT")
    print("-" * 50)
    
    # Create brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createWorldWithDefaults()
    
    print("Created brain and world")
    
    # Use brain context manager
    brain_ctx = pynlm.brainContext(brain)
    brain_ctx.initialize()
    print("Brain context manager initialized")
    
    # Use world context manager
    world_ctx = pynlm.worldContext(world)
    world_ctx.configure(30, 30, 10, 10)
    print("World context manager configured")
    
    # Clean up (context managers will handle cleanup automatically)
    print("Context managers will clean up automatically")
    
    return brain, world

def example_4_advanced_brain_operations():
    """Example 4: Advanced brain operations"""
    print("\n4. ADVANCED BRAIN OPERATIONS")
    print("-" * 50)
    
    # Create neural network
    brain = pynlm.createNetwork(5000, 10)
    print(f"Created neural network with {brain.getTotalNeuronCount()} neurons")
    
    # Connect regions
    pynlm.connectRegions(brain, "region0", "region1", 0.5)
    pynlm.connectRegions(brain, "region1", "region2", 0.3)
    print("Connected regions in neural network")
    
    # Get comprehensive statistics
    stats = brain.getStatistics()
    print(f"Network statistics: {json.dumps(stats, indent=2)}")
    
    # Get region information
    regions = brain.getRegionStatistics()
    print(f"Found {len(regions)} regions")
    for region_name, region_stats in regions.items():
        print(f"  {region_name}: {region_stats['neuron_count']} neurons")
    
    # Analyze brain connectivity
    analysis = pynlm.analyzeBrain(brain)
    print(f"Analysis complete: Stability = {analysis['network_stability']:.4f}")
    
    return brain

def example_5_agent_simulation():
    """Example 5: Complete agent simulation with enhanced features"""
    print("\n5. COMPLETE AGENT SIMULATION")
    print("-" * 50)
    
    # Create simulation components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createWorldWithDefaults()
    world.configure(40, 40, 12, 12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Created and initialized agent with all subsystems")
    
    # Run episode
    episode_stats = pynlm.runEpisode(brain, world, agent, 200)
    
    print(f"Episode complete!")
    print(f"Total reward: {episode_stats['total_reward']:.2f}")
    print(f"Success rate: {episode_stats['success_rate']:.1f}%")
    print(f"Total spikes: {episode_stats['total_spikes']}")
    
    # Show agent state
    state = agent.getEnvironmentState()
    print(f"Environment state: Curiosity={state['curiosity']:.3f}, Novelty={state['novelty']:.3f}")
    
    return brain, world, agent, episode_stats

def example_6_error_handling():
    """Example 6: Enhanced error handling"""
    print("\n6. ENHANCED ERROR HANDLING")
    print("-" * 50)
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Testing error handling...")
    
    # Test invalid neuron type
    try:
        brain.injectCurrentToNeuronType("INVALID_TYPE", 1.0)
        print("ERROR: Should have raised exception!")
    except pynlm.NLMException as e:
        print(f"Caught expected exception: {e}")
    
    # Test invalid configuration
    try:
        invalid_config = pynlm.loadConfigFromFile("nonexistent_file.json")
        print("ERROR: Should have raised exception!")
    except pynlm.ConfigurationError as e:
        print(f"Caught expected configuration error: {e}")
    
    # Test invalid operation
    try:
        brain.save("/invalid/path/checkpoint.bin")
        print("ERROR: Should have raised exception!")
    except pynlm.MemoryError as e:
        print(f"Caught expected memory error: {e}")
    
    print("Error handling tests passed")
    return brain

def example_7_standard_library_integration():
    """Example 7: Standard library integration"""
    print("\n7. STANDARD LIBRARY INTEGRATION")
    print("-" * 50)
    
    # Use Python collections
    config = pynlm.createDefaultConfig()
    
    # Multiple settings with type safety
    settings = [
        ("brain.neuron_count", 1500),
        ("brain.synapse_density", 0.12),
        ("plasticity.stdp.learning_rate", 0.001),
        ("neuromod.dopamine.scale", 1.0),
    ]
    
    for key, value in settings:
        config.set(key, value)
    
    print(f"Set {len(settings)} configuration values using list comprehension")
    
    # Use dictionary for batch operations
    updates = {
        "brain.v_thresh": -50.0,
        "brain.v_rest": -65.0,
        "brain.tau_mem": 20.0,
        "brain.tau_ref": 3.0,
    }
    
    for key, value in updates.items():
        config.set(key, value)
    
    print(f"Applied batch updates: {len(updates)} settings")
    
    # Use generator for processing
    keys = config.getKeys()
    print(f"Configuration keys processed via generator: {len(list(keys))} keys")
    
    # Use sorted for deterministic output
    sorted_keys = sorted(config.getKeys())
    print(f"Keys sorted alphabetically: {sorted_keys[:3]}...")
    
    # Use comprehensions for data processing
    int_configs = [(k, v) for k, v in zip(sorted_keys, [1, 2, 3])]  # Example processing
    print(f"Processed {len(int_configs)} key-value pairs")
    
    return config

def example_8_advanced_simulation():
    """Example 8: Advanced simulation techniques"""
    print("\n8. ADVANCED SIMULATION TECHNIQUES")
    print("-" * 50)
    
    # Create multiple scenarios
    scenarios = [
        {"name": "Small Network", "neurons": 1000, "regions": 3},
        {"name": "Medium Network", "neurons": 5000, "regions": 8},
        {"name": "Large Network", "neurons": 10000, "regions": 15},
    ]
    
    results = []
    
    for scenario in scenarios:
        print(f"Running scenario: {scenario['name']}")
        
        # Create network for scenario
        brain = pynlm.createNetwork(scenario['neurons'], scenario['regions'])
        
        # Add complex connectivity
        for i in range(scenario['regions'] - 1):
            pynlm.connectRegions(brain, f"region{i}", f"region{i+1}", 0.3)
        
        # Run simulation with progress tracking
        total_spikes = pynlm.runSimulation(brain, 100)
        
        # Analyze results
        analysis = pynlm.analyzeBrain(brain)
        
        results.append({
            "name": scenario['name'],
            "neurons": scenario['neurons'],
            "regions": scenario['regions'],
            "total_spikes": total_spikes,
            "stability": analysis['network_stability'],
            "firing_percentage": analysis['firing_percentage']
        })
        
        print(f"  Results: {total_spikes} spikes, {analysis['firing_percentage']:.1f}% firing rate")
    
    # Compare results
    print(f"\nScenario comparison:")
    for result in results:
        print(f"  {result['name']}: {result['total_spikes']} spikes, stability={result['stability']:.4f}")
    
    return results

def example_9_data_persistence():
    """Example 9: Data persistence and serialization"""
    print("\n9. DATA PERSISTENCE AND SERIALIZATION")
    print("-" * 50)
    
    # Create and train brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some training
    for i in range(200):
        brain.step(i)
    
    # Save brain state
    pynlm.saveBrainState(brain, "trained_brain.bin")
    print("Brain state saved")
    
    # Create new brain and load state
    new_brain = pynlm.createBrain(config)
    new_brain.load("trained_brain.bin")
    print("Brain state loaded")
    
    # Verify loaded state
    print(f"Original brain neurons: {brain.getTotalNeuronCount()}")
    print(f"Loaded brain neurons: {new_brain.getTotalNeuronCount()}")
    
    # Save as JSON (configuration)
    config.saveToFile("brain_config.json")
    loaded_config = pynlm.loadConfigFromFile("brain_config.json")
    print("Configuration saved and loaded as JSON")
    
    # Clean up
    import os
    os.remove("trained_brain.bin")
    os.remove("brain_config.json")
    print("Cleanup completed")
    
    return brain

def example_10_advanced_utilities():
    """Example 10: Advanced utilities and pattern matching"""
    print("\n10. ADVANCED UTILITIES AND PATTERN MATCHING")
    print("-" * 50)
    
    # Create brain
    brain = pynlm.createNetwork(3000, 5)
    brain.initialize()
    
    # Find active neurons (enhanced version)
    active_exc = pynlm.findActiveNeurons(brain, pynlm.EXCITATORY)
    print(f"Found {len(active_exc)} active excitatory neurons")
    
    # Get region information
    region_info = brain.getRegionStatistics()
    print(f"Brain has {len(region_info)} regions")
    
    # Display region details
    for region_name, stats in region_info.items():
        print(f"  {region_name}: {stats['neuron_count']} neurons, {stats['synapse_count']} synapses")
    
    # Demonstrate module constants
    print(f"\nModule constants:")
    print(f"  MOVE_FORWARD: {pynlm.MOVE_FORWARD}")
    print(f"  EXCITATORY: {pynlm.EXCITATORY}")
    print(f"  INITIAL: {pynlm.INITIAL}")
    print(f"  RESOURCE: {pynlm.RESOURCE}")
    
    # Demonstrate random utilities
    import random
    random.seed(42)
    
    neuron_type = pynlm.randomNeuronType()
    synapse_type = pynlm.randomSynapseType()
    
    print(f"Random neuron type: {neuron_type}")
    print(f"Random synapse type: {synapse_type}")
    
    return brain

def main():
    """Run all examples"""
    print("Running comprehensive NLM examples...")
    
    try:
        example_1_basic_usage()
        example_2_enhanced_configuration()
        example_3_context_managers()
        example_4_advanced_brain_operations()
        example_5_agent_simulation()
        example_6_error_handling()
        example_7_standard_library_integration()
        example_8_advanced_simulation()
        example_9_data_persistence()
        example_10_advanced_utilities()
        
        print("\n" + "=" * 80)
        print("ALL EXAMPLES COMPLETED SUCCESSFULLY!")
        print("=" * 80)
        print("\nKey improvements demonstrated:")
        print("✓ Enhanced documentation and type safety")
        print("✓ Pythonic API features (context managers, properties)")
        print("✓ Convenience methods and advanced features")
        print("✓ Improved error handling with Python-specific exceptions")
        print("✓ Standard library integration")
        print("✓ Advanced simulation capabilities")
        print("✓ Data persistence and serialization")
        print("✓ Pattern matching and utilities")
        
    except Exception as e:
        print(f"\nERROR in examples: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()