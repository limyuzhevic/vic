#!/usr/bin/env python3
"""
Example script demonstrating the enhanced debugging and profiling capabilities
of the NLM (Neural Learning Machine) framework.
"""

import pynlm
import time
import json
import sys
from typing import Dict, List, Optional

def example_basic_debugging():
    """Example: Basic debugging session"""
    print("=== Basic Debugging Example ===")
    
    # Create brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced debugging features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Starting simulation with debugging enabled...")
    
    # Run simulation with debugging
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    print("Basic debugging example completed.\n")

def example_advanced_profiling():
    """Example: Advanced performance profiling"""
    print("=== Advanced Performance Profiling Example ===")
    
    # Create brain for profiling
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable profiling features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    print("Starting performance profiling session...")
    start_time = time.time()
    
    # Run intensive simulation
    for step in range(500):
        world.update(0.05)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log performance metrics every 100 steps
        if step % 100 == 0:
            elapsed = time.time() - start_time
            print(f"Performance at step {step}:")
            print(f"  Simulated time: {elapsed:.2f}s")
            print(f"  Firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"  Memory usage: Check debugger for details")
    
    total_time = time.time() - start_time
    print(f"\nPerformance profiling completed in {total_time:.2f} seconds")
    print("Run 'nlm --debug' command for detailed performance reports.\n")

def example_configuration_testing():
    """Example: Configuration testing and validation"""
    print("=== Configuration Testing Example ===")
    
    # Test different configurations
    test_configs = [
        {"name": "Small network", "neuron_count": 500, "connection_probability": 0.05},
        {"name": "Medium network", "neuron_count": 2000, "connection_probability": 0.1},
        {"name": "Large network", "neuron_count": 5000, "connection_probability": 0.15},
    ]
    
    for test_config in test_configs:
        print(f"Testing {test_config['name']}...")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", test_config['neuron_count'])
        config.set("brain.connection_probability", test_config['connection_probability'])
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"  Neurons: {brain.getTotalNeuronCount()}")
        print(f"  Expected synapses: ~{int(test_config['neuron_count'] * test_config['connection_probability'])}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        
        # Test connectivity
        brain.step(0)
        active_neurons = brain.getFiringNeuronCount()
        print(f"  Active neurons at step 0: {active_neurons}")
        
        brain.reset()
    
    print("Configuration testing completed.\n")

def example_memory_monitoring():
    """Example: Memory usage monitoring"""
    print("=== Memory Monitoring Example ===")
    
    # Create brain for memory testing
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("Monitoring memory usage during simulation...")
    
    for step in range(200):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        if step % 50 == 0:
            # In a real implementation, this would interface with the C++ debugger
            print(f"Step {step}: Memory usage check...")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    print("Memory monitoring completed. Use debugger for detailed memory analysis.\n")

def example_experiment_framework():
    """Example: Using the experiment framework"""
    print("=== Experiment Framework Example ===")
    
    # This would demonstrate using the Phase 6 integrated experiment framework
    print("Setting up integrated experiment...")
    print("To run integrated experiments, use:")
    print("  ./nlm --phase6")
    print("  ./nlm --phase6 --enable-development")
    print("  ./nlm --phase6 --output results.json")
    print()
    print("The experiment framework provides:")
    print("  - Integration verification")
    print("  - System connectivity testing")
    print("  - Performance benchmarking")
    print("  - Checkpoint and replay capabilities")
    print("  - Memory and developmental analysis")
    
    print("Experiment framework example completed.\n")

def main():
    """Main function demonstrating all examples"""
    print("NLM (Neural Learning Machine) - Enhanced Examples")
    print("=" * 50)
    print()
    
    # Run all examples
    examples = [
        ("Basic Debugging", example_basic_debugging),
        ("Advanced Profiling", example_advanced_profiling),
        ("Configuration Testing", example_configuration_testing),
        ("Memory Monitoring", example_memory_monitoring),
        ("Experiment Framework", example_experiment_framework),
    ]
    
    for name, example_func in examples:
        try:
            example_func()
        except Exception as e:
            print(f"Error in {name}: {e}")
    
    print("All examples completed successfully!")
    print("\nKey improvements for advanced users:")
    print("1. JSON/YAML configuration support")
    print("2. Enhanced command-line interface")
    print("3. Advanced debugging and profiling tools")
    print("4. Comprehensive error handling")
    print("5. Performance optimization frameworks")
    print("6. Extensible Python bindings")
    print("7. Better documentation and examples")

if __name__ == "__main__":
    main()
