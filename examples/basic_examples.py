#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Basic Usage Examples

This file contains examples demonstrating how to use the NLM Python API.

Installation:
    pip install nlm

Basic Usage:
    >>> import nlm
    >>> brain = nlm.Brain()
    >>> brain.initialize()
    >>> result = brain.run_experiment(1000)
"""

import nlm
import json

def basic_usage_example():
    """Example: Basic brain initialization and simulation"""
    print("=== Basic Usage Example ===")
    
    # Create brain with default configuration
    brain = nlm.Brain()
    
    # Run simulation for 500 steps
    print("Running simulation for 500 steps...")
    result = brain.run_steps(500)
    
    # Display results
    print(f"Average reward: {result['total_reward']:.4f}")
    print(f"Average firing rate: {result['average_firing_rate']:.2f} Hz")
    print(f"Active steps: {result['active_steps']:.2%}")
    print(f"Total spikes: {result['total_spikes']}")
    print()

def configuration_example():
    """Example: Configuration management"""
    print("=== Configuration Example ===")
    
    # Create configuration
    config = nlm.Config()
    
    # Set parameters
    config.set("neuron_count", 2000)
    config.set("simulation_timestep", 0.002)
    config.set("random_seed", 12345)
    config.set("connection_probability", 0.15)
    
    # Create brain with custom configuration
    brain = nlm.Brain(config)
    
    # Display configuration
    print("Configuration keys:", config.get_keys())
    print(f"Neuron count: {config.get_int('neuron_count')}")
    print(f"Simulation timestep: {config.get_double('simulation_timestep')} s")
    print()
    
    # Run simulation
    result = brain.run_steps(200)
    print(f"Simulation result - Reward: {result['total_reward']:.4f}")
    print()

def memory_example():
    """Example: Memory system usage"""
    print("=== Memory System Example ===")
    
    brain = nlm.Brain()
    
    # Check memory systems
    working_mem = brain.get_working_memory()
    episodic_mem = brain.get_episodic_memory()
    
    print("Working Memory:", working_mem)
    print("Episodic Memory:", episodic_mem)
    
    # Run some steps to create memories
    brain.run_steps(300)
    
    # Check memory after simulation
    working_mem = brain.get_working_memory()
    episodic_mem = brain.get_episodic_memory()
    
    print("After simulation:")
    print("Working Memory:", working_mem)
    print("Episodic Memory:", episodic_mem)
    print()

def neuromodulation_example():
    """Example: Neuromodulation system"""
    print("=== Neuromodulation Example ===")
    
    brain = nlm.Brain()
    
    # Check neuromodulation levels
    neuromod = brain.get_neuromodulation()
    print("Initial neuromodulation levels:")
    for key, value in neuromod.items():
        print(f"  {key}: {value}")
    
    # Run simulation
    result = brain.run_steps(200)
    
    # Check neuromodulation after simulation
    neuromod = brain.get_neuromodulation()
    print("\nNeuromodulation after simulation:")
    for key, value in neuromod.items():
        print(f"  {key}: {value}")
    print()

def checkpoint_example():
    """Example: Checkpoint save/load"""
    print("=== Checkpoint Example ===")
    
    # Create and train brain
    brain = nlm.Brain()
    print("Training brain for 500 steps...")
    brain.run_steps(500)
    
    # Get initial stats
    initial_stats = brain.get_stats()
    print(f"Initial total spikes: {initial_stats['total_spikes']}")
    
    # Save checkpoint
    checkpoint_file = "nlm_checkpoint_test.bin"
    print(f"Saving checkpoint to {checkpoint_file}")
    brain.save_checkpoint(checkpoint_file)
    
    # Create new brain and load checkpoint
    new_brain = nlm.Brain()
    print("Loading checkpoint...")
    if new_brain.load_checkpoint(checkpoint_file):
        print("Checkpoint loaded successfully!")
        
        # Compare stats
        loaded_stats = new_brain.get_stats()
        print(f"Loaded total spikes: {loaded_stats['total_spikes']}")
        
        if initial_stats['total_spikes'] == loaded_stats['total_spikes']:
            print("✓ Checkpoint/Reload successful!")
        else:
            print("⚠ Checkpoint reload may have issues")
    else:
        print("✗ Failed to load checkpoint")
    
    print()

def experiment_example():
    """Example: Phase 6 experiment integration"""
    print("=== Phase 6 Experiment Example ===")
    
    # Create experiment
    experiment = nlm.Phase6Experiment()
    
    # Verify integration
    print("Verifying system integration...")
    if experiment.verify_integration():
        print("✓ All systems integrated successfully")
    else:
        print("✗ Integration verification failed")
        return
    
    # Run test integration
    print("\nTesting memory integration...")
    if experiment.test_memory_integration():
        print("✓ Memory integration test passed")
    
    print("Testing neuromodulation integration...")
    if experiment.test_neuromodulation_integration():
        print("✓ Neuromodulation integration test passed")
    
    # Run full experiment
    config = {
        "neuronCount": 500,
        "maxSteps": 1000,
        "enableCheckpointing": True,
        "enableReplay": True,
        "enableDevelopment": True
    }
    
    print("\nRunning full integration experiment...")
    result = experiment.run_experiment(config)
    
    print("Experiment Results:")
    print(f"  Total reward: {result['totalReward']:.4f}")
    print(f"  Average firing rate: {result['avgFiringRate']:.2f} Hz")
    print(f"  Episodes stored: {result['memoryEpisodesStored']}")
    print(f"  Wall clock time: {result['wallClockTime']:.2f}s")
    
    print("\nIntegration Status:")
    integration = result['integration']
    for key, value in integration.items():
        status = "✓" if value else "✗"
        print(f"  {status} {key}: {value}")
    
    print()

def performance_example():
    """Example: Performance monitoring"""
    print("=== Performance Monitoring Example ===")
    
    import time
    
    # Get system stats
    system_stats = nlm.PerformanceMonitor.get_system_stats()
    print("System Performance:")
    print(f"  CPU usage: {system_stats['cpu_usage']:.1f}%")
    print(f"  Memory usage: {system_stats['memory_usage']:.1f}%")
    print(f"  Thread count: {system_stats['thread_count']}")
    print(f"  Simulation speed: {system_stats['simulation_speed']:.1f} steps/sec")
    
    # Benchmark different configurations
    print("\nBenchmarking different configurations...")
    
    configs = [
        (100, 100),
        (500, 200),
        (1000, 500)
    ]
    
    for neurons, steps in configs:
        config = nlm.Config()
        config.set("neuron_count", neurons)
        
        brain = nlm.Brain(config)
        start_time = time.time()
        
        brain.run_steps(steps)
        
        end_time = time.time()
        duration = end_time - start_time
        
        print(f"  {neurons:4d} neurons, {steps:4d} steps: {duration:.3f}s ({steps/duration:.0f} steps/sec)")
    
    print()

def quick_start_example():
    """Example: Quick start for beginners"""
    print("=== Quick Start Example ===")
    print("This example demonstrates the simplest possible NLM usage:")
    print()
    
    # One-liner example
    print("1. Create brain with default settings:")
    print("   brain = nlm.Brain()")
    print()
    
    print("2. Run simulation:")
    print("   result = brain.run_steps(1000)")
    print()
    
    print("3. Check results:")
    print("   print(f\"Reward: {result['total_reward']:.3f}\")")
    print()
    
    # Execute the example
    brain = nlm.Brain()
    result = brain.run_steps(100)
    print(f"   Reward: {result['total_reward']:.3f}")
    print()

def advanced_configuration_example():
    """Example: Advanced configuration with JSON/YAML"""
    print("=== Advanced Configuration Example ===")
    
    # Create JSON configuration
    json_config = """
    {
        "neuron_count": 2000,
        "region_count": 2,
        "connection_probability": 0.2,
        "simulation_timestep": 0.001,
        "random_seed": 42,
        "enable_checkpointing": true,
        "checkpoint_path": "../checkpoints/experiment1.bin"
    }
    """
    
    # Create YAML configuration
    yaml_config = """
    neuron_count: 2000
    region_count: 2
    connection_probability: 0.2
    simulation_timestep: 0.001
    random_seed: 42
    enable_checkpointing: true
    checkpoint_path: "../checkpoints/experiment1.bin"
    """
    
    # Load configurations
    config1 = nlm.Config()
    config1.load_from_json(json_config)
    
    print("JSON configuration loaded:")
    print(f"  Neuron count: {config1.get_int('neuron_count')}")
    print(f"  Region count: {config1.get_int('region_count')}")
    print()
    
    # Convert to JSON and YAML
    json_output = config1.to_json()
    print("Convert back to JSON:")
    print(json.dumps(json.loads(json_output), indent=2))
    print()
    
    yaml_output = config1.to_yaml()
    print("Convert to YAML:")
    print(yaml_output)
    print()

def main():
    """Run all examples"""
    print("NLM (Neural Learning Machine) - Usage Examples")
    print("=" * 50)
    print()
    
    examples = [
        ("Basic Usage", basic_usage_example),
        ("Configuration", configuration_example),
        ("Memory Systems", memory_example),
        ("Neuromodulation", neuromodulation_example),
        ("Checkpoints", checkpoint_example),
        ("Experiments", experiment_example),
        ("Performance", performance_example),
        ("Quick Start", quick_start_example),
        ("Advanced Configuration", advanced_configuration_example),
    ]
    
    for name, example_func in examples:
        try:
            example_func()
            print("✓ Example completed successfully")
        except Exception as e:
            print(f"✗ Example failed: {e}")
        
        print("-" * 50)
    
    print("All examples completed!")
    print("\nFor more information, see the NLM documentation.")

if __name__ == "__main__":
    main()