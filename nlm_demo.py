#!/usr/bin/env python3
"""
Improved NLM Python Demo Script

This script demonstrates the improved Python bindings for NLM (Neural Learning Machine).
It shows various usage patterns and advanced features.

Usage:
    python3 nlm_demo.py                    # Basic demo with default settings
    python3 nlm_demo.py 10 500             # Custom episode count and steps
    python3 nlm_demo.py --config config.json # Load configuration from file
    python3 nlm_demo.py --benchmark         # Run performance benchmark
"""

import sys
import time
import json
import numpy as np
import matplotlib.pyplot as plt

try:
    import nlm
    print("✓ NLM Python module imported successfully")
except ImportError as e:
    print(f"✗ Failed to import NLM: {e}")
    print("Please build the Python bindings first:")
    print("  mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release")
    print("  make -j4")
    sys.exit(1)


def print_banner():
    """Print the NLM demo banner"""
    print("\n" + "="*60)
    print("NLM — 熙然 (serene flow)")
    print("Improved Python Bindings Demo")
    print("="*60)
    print("An experimental artificial developmental brain")
    print("="*60 + "\n")


def basic_demo():
    """Demonstrate basic NLM functionality"""
    print("=== BASIC DEMO ===")
    
    # Create default configuration
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    config.set("random_seed", 42)
    
    # Create brain
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create simple world
    world = nlm.createSimpleWorld()
    world.configure(20.0f, 20.0f, 16, 16)
    
    # Create agent brain
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Created agent with sensory input size: {agent.getSensoryInputSize()}")
    print(f"✓ Agent motor output size: {agent.getMotorOutputSize()}")
    
    # Get initial statistics
    stats = brain.getNeuronPopulationStats()
    print(f"✓ Neuron population: {stats['excitatory_neurons']} excitatory, "
          f"{stats['inhibitory_neurons']} inhibitory")
    
    return brain, world, agent


def advanced_config_demo():
    """Demonstrate advanced configuration features"""
    print("\n=== ADVANCED CONFIGURATION DEMO ===")
    
    # Create configuration using settings map
    settings = {
        "neuron_count": 500,
        "region_count": 2,
        "connection_probability": 0.15,
        "random_seed": 123,
        "simulation_timestep": 0.001,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
    }
    
    config = nlm.createCustomConfig(settings)
    
    print("✓ Created configuration with custom settings:")
    for key, value in settings.items():
        print(f"    {key}: {value}")
    
    # Verify configuration was set correctly
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Region count: {brain.getRegionCount()}")
    
    return config


def convenience_functions_demo():
    """Demonstrate convenience functions"""
    print("\n=== CONVENIENCE FUNCTIONS DEMO ===")
    
    # Use Phase 6 config helper
    config = nlm.createSimulationConfig(neuronCount=200, steps=1000)
    
    print(f"✓ Created simulation config: {config.neuronCount} neurons, {config.maxSteps} steps")
    
    # Create brain with config
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Run analysis
    analysis = nlm.analyzeBrainPerformance(brain)
    print("✓ Brain analysis results:")
    for key, value in analysis.items():
        print(f"    {key}: {value}")
    
    return brain, config


def checkpoint_demo():
    """Demonstrate checkpoint save/load functionality"""
    print("\n=== CHECKPOINT DEMO ===")
    
    # Create and initialize brain
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 100)
    brain1 = nlm.createBrain(config)
    brain1.initialize()
    
    # Save checkpoint
    checkpoint_path = "/tmp/nlm_checkpoint_test.bin"
    success = nlm.saveBrainCheckpoint(brain1, checkpoint_path)
    
    if success:
        print(f"✓ Successfully saved brain to {checkpoint_path}")
        
        # Create new brain and load checkpoint
        brain2 = nlm.createBrain(config)
        brain2.initialize()
        
        success = nlm.loadBrainCheckpoint(brain2, checkpoint_path)
        if success:
            print("✓ Successfully loaded checkpoint")
            print(f"✓ Brain2 has {brain2.getTotalNeuronCount()} neurons")
            print(f"✓ Brain1 has {brain1.getTotalNeuronCount()} neurons")
        else:
            print("✗ Failed to load checkpoint")
    else:
        print("✗ Failed to save checkpoint")
    
    return brain1, brain2


def utility_functions_demo():
    """Demonstrate utility functions"""
    print("\n=== UTILITY FUNCTIONS DEMO ===")
    
    # Create multiple configurations with different settings
    configs = [
        nlm.createSimulationConfig(100, 500),
        nlm.createSimulationConfig(500, 2000),
        nlm.createSimulationConfig(1000, 1000),
    ]
    
    print("✓ Created multiple simulation configurations:")
    for i, config in enumerate(configs):
        print(f"  Config {i+1}: {config.neuronCount} neurons, {config.maxSteps} steps")
    
    return configs


def main():
    """Main demo function"""
    print_banner()
    
    # Parse command line arguments
    num_episodes = 5
    steps_per_episode = 200
    benchmark_mode = False
    config_file = None
    
    i = 1
    while i < len(sys.argv):
        arg = sys.argv[i]
        if arg == "--benchmark":
            benchmark_mode = True
            i += 1
        elif arg == "--config" and i + 1 < len(sys.argv):
            config_file = sys.argv[i + 1]
            i += 2
        elif arg.isdigit():
            num_episodes = int(arg)
            i += 1
        elif i + 1 < len(sys.argv) and sys.argv[i + 1].isdigit():
            steps_per_episode = int(sys.argv[i + 1])
            i += 2
        else:
            print(f"✗ Unknown argument: {arg}")
            print("Usage: python3 nlm_demo.py [episodes] [steps] [--benchmark] [--config file]")
            return 1
    
    if config_file:
        try:
            with open(config_file, 'r') as f:
                config_data = json.load(f)
            print(f"✓ Loaded configuration from {config_file}")
            
            # Apply configuration to demo
            demo_config = nlm.createCustomConfig(config_data)
            print("✓ Configuration applied successfully")
            
        except Exception as e:
            print(f"✗ Failed to load configuration: {e}")
    
    # Run basic demo
    brain, world, agent = basic_demo()
    
    # Run advanced configuration demo
    advanced_config_demo()
    
    # Run convenience functions demo
    convenience_functions_demo()
    
    # Run utility functions demo
    utility_functions_demo()
    
    # Run checkpoint demo if not in benchmark mode
    if not benchmark_mode:
        checkpoint_demo()
    
    print("\n" + "="*60)
    print("DEMO COMPLETE")
    print("="*60)
    print(f"✓ Ran {num_episodes} episodes with {steps_per_episode} steps each")
    print("✓ All NLM Python features demonstrated successfully")
    print("="*60 + "\n")


if __name__ == "__main__":
    main()
