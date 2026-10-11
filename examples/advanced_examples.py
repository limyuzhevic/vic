#!/usr/bin/env python3
"""
NLM Python Bindings - Advanced Examples

This file demonstrates advanced usage patterns and features of the NLM Python bindings.
It covers context managers, iterators, list comprehensions, and more.

To use these examples, make sure you have pynlm installed:
    pip install pynlm
"""

import pynlm as nlm
import json

def basic_usage_example():
    """Example 1: Basic brain usage"""
    print("=== Basic Usage Example ===")
    
    # Create brain with default configuration
    brain = nlm.createBrain(nlm.createDefaultConfig())
    print(f"Created brain with {brain.total_neurons} neurons")
    
    # Initialize brain
    brain.initialize()
    print("Brain initialized")
    
    # Use context manager for automatic cleanup
    with nlm.brain_simulation(brain):
        print("Brain simulation context entered")
        
        # Run multiple steps
        for i in range(10):
            brain.step(i)
            if i % 5 == 0:
                print(f"  Step {i}: {brain.firing_neurons} neurons firing")
    
    print("Brain simulation context exited\n")

def advanced_config_example():
    """Example 2: Advanced configuration handling"""
    print("=== Advanced Configuration Example ===")
    
    # Create configuration
    config = nlm.createDefaultConfig()
    
    # Set multiple values
    config["neuron_count"] = 2000
    config["synapse_density"] = 0.15
    config["stdp_learning_rate"] = 0.005
    config["enable_structural_plasticity"] = True
    
    print("Configuration set:")
    print(f"  Neurons: {config.get('neuron_count')}")
    print(f"  Synapse density: {config.get('synapse_density')}")
    print(f"  STDP rate: {config.get('stdp_learning_rate')}")
    
    # Convert to dictionary
    config_dict = nlm.config_to_dict(config)
    print(f"\nConfiguration as dict: {dict(config_dict)}")
    
    # Create brain with advanced config
    brain = nlm.createBrain(config)
    brain.initialize()
    print(f"Brain created with {brain.total_neurons} neurons")
    
    # Modify config after creation
    config["enable_curiosity"] = True
    print("Added curiosity feature\n")

def agent_simulation_example():
    """Example 3: Complete agent simulation"""
    print("=== Agent Simulation Example ===")
    
    # Create components
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    agent = nlm.createAgentBrain(brain)
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Starting agent simulation...")
    
    # Run episode with advanced options
    options = {
        "enable_learning": True,
        "enable_curiosity": True,
        "timestep": 0.05
    }
    
    result = nlm.simulate_agent(agent, world, 100, options)
    
    print(f"Simulation completed:")
    print(f"  Total reward: {result['total_reward']:.2f}")
    print(f"  Steps completed: {result['steps_completed']}")
    print(f"  Average reward: {result['avg_reward']:.2f}")
    print(f"  Final world time: {result['final_time']:.2f}")
    
    # Get detailed stats
    agent_stats = nlm.get_agent_stats(agent)
    print(f"\nAgent stats:")
    for key, value in agent_stats.items():
        print(f"  {key}: {value}")
    
    brain_stats = nlm.get_brain_stats(brain)
    print(f"\nBrain stats:")
    for key, value in brain_stats.items():
        print(f"  {key}: {value}")
    
    print()

def data_analysis_example():
    """Example 4: Data analysis with iterators"""
    print("=== Data Analysis Example ===")
    
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    # Run some steps to generate data
    for i in range(100):
        brain.step(i)
    
    # Use list comprehensions to analyze data
    active_neuron_ids = nlm.get_active_neuron_ids(brain)
    firing_neuron_ids = nlm.get_firing_neuron_ids(brain)
    region_names = nlm.get_region_names(brain)
    
    print(f"Total neurons: {brain.total_neurons}")
    print(f"Active neurons: {len(active_neuron_ids)}")
    print(f"Firing neurons: {len(firing_neuron_ids)}")
    print(f"Regions: {region_names}")
    
    # Advanced analysis with generators
    print("\nUsing iterator pattern:")
    regions_iterator = nlm.iter_regions(brain)
    
    # Process regions one by one (memory efficient)
    region_count = 0
    for region in regions_iterator:
        region_count += 1
        neuron_count = region.get_neuron_count()
        print(f"  Region {region_count}: {neuron_count} neurons")
        
        if region_count >= 3:  # Limit output
            break
    
    # Get neurons iterator for specific region
    if region_count > 0:
        first_region_id = region.get_id()
        neurons_iterator = nlm.iter_neurons(brain, first_region_id)
        print(f"\nFirst {min(5, region.get_neuron_count())} neurons in region {first_region_id}:")
        
        neuron_count = 0
        for neuron in neurons_iterator:
            if neuron_count >= 5:
                break
            print(f"  Neuron ID: {neuron.value}")
            neuron_count += 1
    
    print()

def batch_operations_example():
    """Example 5: Batch operations for performance"""
    print("=== Batch Operations Example ===")
    
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    # Use batch operations for better performance
    print("Running batch brain steps...")
    spike_count = nlm.batch_steps(brain, 1000)
    print(f"After 1000 steps: {spike_count} total spikes")
    
    world = nlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent and run batch world updates
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("\nRunning batch world updates...")
    final_time = nlm.batch_world_updates(world, 500, 0.02)
    print(f"World simulation time: {final_time:.2f}")
    
    # Use batch operations for complex simulations
    print("\nRunning complex simulation...")
    operations = {
        "steps": 200,
        "initialize": True
    }
    
    results = nlm.batch_brain_operations(brain, operations)
    print(f"Batch operation results: {results}")
    
    print()

def debugging_example():
    """Example 6: Debugging and diagnostics"""
    print("=== Debugging Example ===")
    
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    # Run some simulation steps
    for i in range(50):
        brain.step(i)
    
    # Get detailed debug information
    debug_info = nlm.debug_brain_state(brain)
    
    print("Debug information:")
    for key, value in debug_info.items():
        if isinstance(value, dict):
            print(f"  {key}:")
            for subkey, subvalue in value.items():
                print(f"    {subkey}: {subvalue}")
        elif isinstance(value, list):
            print(f"  {key}: [{len(value)} items]")
            for i, item in enumerate(value[:3]):  # Show first 3
                print(f"    [{i}]: {item}")
            if len(value) > 3:
                print(f"    ... and {len(value) - 3} more")
        else:
            print(f"  {key}: {value}")
    
    # Performance monitoring
    print("\nPerformance monitoring:")
    brain_stats = nlm.get_brain_stats(brain)
    for key, value in brain_stats.items():
        print(f"  {key}: {value}")
    
    print()

def serialization_example():
    """Example 7: Serialization and persistence"""
    print("=== Serialization Example ===")
    
    # Create and train a brain
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    for i in range(100):
        brain.step(i)
    
    # Serialize brain to file
    filepath = "trained_brain.bin"
    print(f"Serializing brain to {filepath}...")
    nlm.serialize_brain(brain, filepath)
    print("Serialization completed")
    
    # Deserialize brain from file
    print(f"Deserializing brain from {filepath}...")
    loaded_brain = nlm.deserialize_brain(filepath)
    
    if loaded_brain:
        print(f"Successfully loaded brain with {loaded_brain.total_neurons} neurons")
        print(f"Loaded brain state: {loaded_brain.developmental_stage}")
    else:
        print("Failed to load brain")
    
    # Clean up
    import os
    if os.path.exists(filepath):
        os.remove(filepath)
        print("Cleaned up temporary file")
    
    print()

def context_manager_example():
    """Example 8: Context manager usage"""
    print("=== Context Manager Example ===")
    
    # Create components
    brain = nlm.createBrain(nlm.createDefaultConfig())
    agent = nlm.createAgentBrain(brain)
    world = nlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent.initialize(world)
    
    print("Using context managers for automatic resource management...")
    
    # Use brain context manager
    with nlm.brain_simulation(brain):
        print("  Brain context: Brain initialized")
        print(f"  Brain state: {brain.total_neurons} neurons")
        brain.step(0)
    
    print("  Brain context: Reset completed")
    
    # Use world context manager
    with nlm.world_context(world):
        print("  World context: World reset")
        print(f"  World dimensions: {world.width}x{world.height}")
        world.update(0.1)
    
    print("  World context: Cleanup completed")
    
    # Use agent context manager
    with nlm.agent_context(agent):
        print("  Agent context: Agent available")
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        action = agent.decodeMotorCommand()
        print(f"  Agent action: {action.type}")
    
    print("  Agent context: Cleanup completed")
    
    print()

def advanced_config_example_2():
    """Example 9: Advanced configuration operations"""
    print("=== Advanced Configuration Example 2 ===")
    
    # Create config from string
    config_str = """
    # NLM Configuration Example
    # Basic settings
    neuron_count = 1500
    region_count = 3
    connection_probability = 0.08
    
    # Plasticity settings
    stdp_ltp_weight = 0.01
    stdp_ltd_weight = 0.012
    hebbian_learning_rate = 0.001
    
    # Neuromodulation
    dopamine_scale = 1.0
    curiosity_enabled = true
    novelty_enabled = true
    """
    
    config = nlm.load_config_from_string(config_str)
    
    print("Configuration loaded from string:")
    print(f"  Neuron count: {config.get('neuron_count')}")
    print(f"  Region count: {config.get('region_count')}")
    print(f"  Connection probability: {config.get('connection_probability')}")
    print(f"  STDP LTP weight: {config.get('stdp_ltp_weight')}")
    print(f"  Curiosity enabled: {config.get('curiosity_enabled')}")
    
    # Convert to dict and back
    config_dict = nlm.config_to_dict(config)
    print(f"\nConfiguration as Python dict: {dict(config_dict)}")
    
    # Modify using dictionary
    config_dict["neuron_count"] = 2000
    config_dict["new_setting"] = "test_value"
    
    # Convert back to config
    updated_config = nlm.dict_to_config(nlm.createDefaultConfig(), config_dict)
    print(f"\nUpdated neuron count: {updated_config.get('neuron_count')}")
    print(f"New setting exists: {updated_config.has('new_setting')}")
    
    print()

def main():
    """Run all examples"""
    print("NLM Python Bindings - Advanced Examples")
    print("=" * 50)
    print()
    
    examples = [
        basic_usage_example,
        advanced_config_example,
        agent_simulation_example,
        data_analysis_example,
        batch_operations_example,
        debugging_example,
        serialization_example,
        context_manager_example,
        advanced_config_example_2,
    ]
    
    for example in examples:
        try:
            example()
        except Exception as e:
            print(f"Error in {example.__name__}: {e}")
            print()
    
    print("All examples completed!")
    print("\nFor more information, refer to:")
    print("  - HOW_TO_USE.md: Comprehensive usage guide")
    print("  - easy_usage.md: Beginner-friendly guide")
    print("  - docs/ARCHITECTURE.md: System architecture")

if __name__ == "__main__":
    main()