#!/usr/bin/env python3
"""
Quick Start Guide for NLM Enhanced Python Bindings

This script provides a comprehensive introduction to using the NLM Python bindings,
demonstrating all the new features and capabilities added to the enhanced version.
"""

import pynlm
import numpy as np
import matplotlib.pyplot as plt

def print_banner():
    print("=" * 70)
    print("NLM Enhanced Python Bindings - Quick Start Guide")
    print("=" * 70)
    print()

def print_section(title):
    print(f"\n{'-' * 70}")
    print(f"{title}")
    print(f"{'-' * 70}")

def example_basic_usage():
    print_section("1. Basic Usage - Creating and Running a Brain")
    print("""
This is the most basic way to create and run an NLM brain.
""")
    
    # Create brain
    brain = pynlm.createBrain()
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run simulation
    for i in range(50):
        brain.step(i)
    
    print(f"Ran 50 simulation steps")
    print(f"Final firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")

def example_world_agent():
    print_section("2. Complete Agent System - Brain, World, and Agent")
    print("""
This example shows how to create a complete agent system with:
- A neural brain
- A simulated world
- An agent that connects them
- Learning capabilities
""")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # Create agent with all learning systems
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enable_all_systems()
    
    print(f"Created brain ({brain.getTotalNeuronCount()} neurons)")
    print(f"Created world ({world.getWidth()}x{world.getHeight()} units)")
    print(f"Created agent with {agent.getSensoryInputSize()} sensory inputs")
    print(f"Enabled all learning systems: Reward={agent.isRewardModulationEnabled()}, "
          f"Curiosity={agent.isCuriosityEnabled()}, Development={agent.isDevelopmentEnabled()}")
    
    # Run simulation
    print("\nRunning complete simulation...")
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        if step % 10 == 0:
            agent.applyRewardModulation(0.5, 0.3)
    
    print(f"Simulation complete!")
    print(f"Final firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")

def example_advanced_features():
    print_section("3. Advanced Features - Learning and Development")
    print("""
This example demonstrates advanced features like:
- Developmental stages
- Memory systems
- Neuromodulation
- Performance monitoring
""")
    
    # Create advanced brain
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Test developmental stages
    print("\nTesting developmental stages:")
    stages = [pynlm.DevelopmentalStage.Initial, pynlm.DevelopmentalStage.CriticalPeriod,
             pynlm.DevelopmentalStage.Maturation, pynlm.DevelopmentalStage.Adult]
    
    for stage in stages:
        brain.setDevelopmentalStage(stage)
        print(f"  {stage}: {brain.getDevelopmentalStage()}")
    
    # Test memory systems
    print("\nTesting memory systems:")
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    if working_memory:
        print(f"  Working memory capacity: {working_memory.getCapacity()}")
        print(f"  Current working memory size: {working_memory.getSize()}")
    
    if episodic_memory:
        print(f"  Episodic memory capacity: {episodic_memory.getCapacity()}")
    
    # Test neuromodulation
    print("\nTesting neuromodulation:")
    print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"  Total spike count: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f} Hz")

def example_configuration():
    print_section("4. Configuration Management - Custom Brain Setup")
    print("""
This example shows how to create custom brain configurations
with specific parameters for different use cases.
""")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    print("Default configuration created")
    print(f"Available keys: {config.getKeys()}")
    
    # Apply custom configuration
    custom_config = {
        "brain.neuron_count": 2000,
        "brain.synapse_density": 0.15,
        "brain.region_count": 8,
        "brain.development.enabled": True,
        "brain.plasticity.enabled": True,
        "brain.memory.working.capacity": 300,
        "brain.memory.episodic.capacity": 200,
        "brain.prediction.enabled": True,
        "brain.cognition.enabled": True
    }
    
    print(f"\nApplying custom configuration:")
    for key, value in custom_config.items():
        config.set(key, value)
        print(f"  {key}: {value}")
    
    # Create brain with custom config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"\nCreated brain with custom configuration:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Region count: {brain.getRegionCount()}")

def example_error_handling():
    print_section("5. Error Handling and Validation")
    print("""
This example demonstrates the enhanced error handling and validation
features that make the NLM Python bindings robust and user-friendly.
""")
    
    # Test configuration validation
    print("\nTesting configuration validation:")
    config = pynlm.createDefaultConfig()
    
    # Set valid configuration
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    
    print("✓ Valid configuration accepted")
    
    # Try to set invalid configuration
    print("\nTesting invalid configuration handling:")
    try:
        config.set("brain.neuron_count", -1)  # Invalid!
        print("✗ This should have failed!")
    except Exception as e:
        print(f"✓ Invalid configuration rejected: {type(e).__name__}")
    
    # Test brain operations
    print("\nTesting brain operations:")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    try:
        # These operations should work
        brain.step(0)
        firing_rate = brain.getAverageFiringRate()
        print(f"✓ Basic brain operations successful (firing rate: {firing_rate:.3f} Hz)")
    except Exception as e:
        print(f"✗ Brain operations failed: {e}")

def example_performance():
    print_section("6. Performance Monitoring")
    print("""
This example demonstrates performance monitoring capabilities
to help optimize simulation performance.
""")
    
    import time
    
    # Create brain
    brain = pynlm.createBrain()
    brain.initialize()
    
    print("\nPerformance benchmarking:")
    
    # Benchmark small simulation
    start_time = time.time()
    for i in range(100):
        brain.step(i)
    end_time = time.time()
    
    small_perf = 100 / (end_time - start_time)
    print(f"  Small simulation (100 steps): {small_perf:.0f} steps/sec")
    
    # Benchmark larger simulation
    start_time = time.time()
    for i in range(1000):
        brain.step(i)
    end_time = time.time()
    
    large_perf = 1000 / (end_time - start_time)
    print(f"  Large simulation (1000 steps): {large_perf:.0f} steps/sec")
    
    print(f"  Performance ratio: {large_perf/small_perf:.1f}x")

def example_visualization():
    print_section("7. Visualization and Analysis")
    print("""
This example demonstrates visualization capabilities and analysis tools
for understanding brain activity.
""")
    
    # Create and run simulation
    brain = pynlm.createBrain()
    brain.initialize()
    
    print("\nRunning simulation for analysis...")
    firing_rates = []
    spike_counts = []
    
    for i in range(50):
        brain.step(i)
        firing_rates.append(brain.getAverageFiringRate())
        spike_counts.append(brain.getTotalSpikeCount())
    
    print(f"Simulation completed")
    print(f"Average firing rate over simulation: {np.mean(firing_rates):.3f} Hz")
    print(f"Total spike count: {spike_counts[-1]}")
    
    # Try to visualize (if matplotlib is available)
    try:
        plt.figure(figsize=(10, 6))
        
        plt.subplot(2, 1, 1)
        plt.plot(firing_rates, 'b-', linewidth=2)
        plt.title("Brain Firing Rate Over Time")
        plt.ylabel("Firing Rate (Hz)")
        plt.grid(True, alpha=0.3)
        
        plt.subplot(2, 1, 2)
        plt.plot(spike_counts, 'r-', linewidth=2)
        plt.title("Cumulative Spike Count")
        plt.ylabel("Total Spikes")
        plt.xlabel("Simulation Step")
        plt.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig("simulation_plot.png", dpi=150, bbox_inches='tight')
        print(f"\nVisualization saved to 'simulation_plot.png'")
        
    except Exception as e:
        print(f"\nVisualization not available: {e}")

def main():
    print_banner()
    
    print("Welcome to the NLM Enhanced Python Bindings Quick Start Guide!")
    print("\nThis guide demonstrates all the features and capabilities")
    print("available in the enhanced NLM Python bindings.")
    print("\nWe'll walk through:")
    print("  1. Basic usage - creating and running a brain")
    print("  2. Complete agent system - brain, world, and agent")
    print("  3. Advanced features - learning and development")
    print("  4. Configuration management - custom brain setup")
    print("  5. Error handling and validation")
    print("  6. Performance monitoring")
    print("  7. Visualization and analysis")
    print("\nEach section builds upon the previous one, so let's get started!")
    
    try:
        example_basic_usage()
        example_world_agent()
        example_advanced_features()
        example_configuration()
        example_error_handling()
        example_performance()
        example_visualization()
        
        print_section("Tutorial Complete!")
        print("""
You have successfully completed the NLM Enhanced Python Bindings tutorial!

Key takeaways:

1. ✓ Basic brain creation and simulation
2. ✓ Complete agent system with world interaction
3. ✓ Advanced features (development, learning, memory)
4. ✓ Configuration management and customization
5. ✓ Error handling and validation
6. ✓ Performance monitoring
7. ✓ Visualization and analysis

What you can do now:

• Explore the documentation for detailed API reference
• Try the advanced examples in the examples/ directory
• Run the test suite to verify functionality
• Create your own simulation scenarios
• Use the performance benchmarks for optimization

For more information:
• Check the docs/API.md for complete API reference
• Visit the NLM project for more examples and tutorials
• Join the community for support and discussions

Happy neural simulation! 🎯
""")
        
    except Exception as e:
        print(f"\nERROR: Tutorial failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())
