#!/usr/bin/env python3
"""
Comprehensive Examples and Tutorials for NLM (Neural Learning Machine)

This file provides a collection of example scripts demonstrating how to use
the NLM Python bindings to create, configure, and run neural simulations.
Each example showcases different aspects of the NLM framework.
"""

import pynlm
import random
import time

def example_1_basic_brain():
    """
    Example 1: Basic NLM Brain Setup and Simulation
    
    This example demonstrates the fundamental steps to create and run
    a simple NLM brain simulation.
    """
    print("=" * 60)
    print("Example 1: Basic NLM Brain Setup")
    print("=" * 60)
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Modify configuration if needed
    config.set("neuron_count", 500)
    config.set("region_count", 2)
    
    # Create brain
    brain = pynlm.createBrain(config)
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Initialize brain
    if brain.initialize():
        print("Brain initialization successful")
    else:
        print("Brain initialization failed")
        return
    
    # Run simulation
    print("Running simulation...")
    start_time = time.time()
    
    for step in range(100):
        brain.step(step)
        if step % 20 == 0:
            print(f"Step {step}: Active neurons = {brain.getActiveNeuronCount()}")
    
    end_time = time.time()
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    
    # Get and display statistics
    stats = pynlm.getBrainStatistics(brain)
    print(f"\nFinal Statistics:")
    print(f"  Total Neurons: {stats['neuron_count']}")
    print(f"  Total Synapses: {stats['synapse_count']}")
    print(f"  Total Spikes: {stats['total_spikes']}")
    print(f"  Average Firing Rate: {stats['avg_firing_rate']:.4f} Hz")
    print(f"  E/I Ratio: {stats['e_i_ratio']:.2f}")
    
    print("\nExample 1 completed successfully!\n")

def example_2_learning_simulation():
    """
    Example 2: Learning Simulation with Stimulus Patterns
    
    This example demonstrates how to create and apply stimulus patterns
    to a brain to induce learning and observe neural adaptation.
    """
    print("=" * 60)
    print("Example 2: Learning Simulation with Stimulus Patterns")
    print("=" * 60)
    
    # Create brain with learning-oriented configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 300)
    config.set("plasticity_rate", 0.8)  # Enable strong plasticity
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create a stimulus pattern (simulating sensory input)
    # This pattern represents a sequence of sensory inputs
    stimulus_pattern = [10.0, 15.0, 8.0, 12.0, 20.0, 5.0, 18.0, 7.0]
    
    print(f"Applying stimulus pattern: {stimulus_pattern}")
    print("Pattern represents current injection to sensory neurons")
    
    # Apply the stimulus pattern
    pynlm.applyStimulusPattern(brain, stimulus_pattern, 300)
    
    # Analyze learning results
    stats = pynlm.getBrainStatistics(brain)
    print(f"\nLearning Analysis:")
    print(f"  Pattern applied to {len(stimulus_pattern)} time steps")
    print(f"  Total spikes generated: {stats['total_spikes']}")
    print(f"  Average firing rate: {stats['avg_firing_rate']:.4f} Hz")
    print(f"  Network activity level: {stats['avg_firing_rate'] / 20:.1%}")
    
    # Check if learning occurred (measure network response)
    if stats['avg_firing_rate'] > 1.0:
        print("  ✓ Learning detected: Network showed significant response to stimulus")
    else:
        print("  ⚠ Limited response: Network may need stronger stimulation")
    
    print("\nExample 2 completed!\n")

def example_3_multi_region_brain():
    """
    Example 3: Multi-Region Brain with Different Neuron Types
    
    This example demonstrates how to create a complex brain with multiple
    regions containing different types of neurons (sensory, motor, internal).
    """
    print("=" * 60)
    print("Example 3: Multi-Region Brain with Different Neuron Types")
    print("=" * 60)
    
    # Create multi-region brain
    config = pynlm.createDefaultConfig()
    config.set("region_count", 4)  # Four distinct regions
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Creating neural populations in different regions...")
    
    # Create sensory processing region
    sensory_region, sensory_pop = pynlm.createNeuralPopulation(
        brain, 100, pynlm.NeuronType.Sensory, "SensoryCortex"
    )
    print(f"  Created Sensory Cortex with 100 neurons")
    
    # Create motor control region
    motor_region, motor_pop = pynlm.createNeuralPopulation(
        brain, 50, pynlm.NeuronType.Motor, "MotorCortex"
    )
    print(f"  Created Motor Cortex with 50 neurons")
    
    # Create association region for internal processing
    association_region, association_pop = pynlm.createNeuralPopulation(
        brain, 200, pynlm.NeuronType.Internal, "AssociationCortex"
    )
    print(f"  Created Association Cortex with 200 neurons")
    
    # Create visual processing region
    visual_region, visual_pop = pynlm.createNeuralPopulation(
        brain, 150, pynlm.NeuronType.Sensory, "VisualCortex"
    )
    print(f"  Created Visual Cortex with 150 neurons")
    
    # Run simulation with regional connectivity
    print("\nRunning multi-region simulation...")
    for step in range(500):
        brain.step(step)
        
        # Log progress at key intervals
        if step % 100 == 0:
            print(f"  Step {step}: Running simulation...")
    
    # Analyze by region
    stats = pynlm.getBrainStatistics(brain)
    print(f"\nMulti-Region Analysis:")
    print(f"  Total neurons across all regions: {stats['neuron_count']}")
    print(f"  Number of regions: {stats['region_count']}")
    print(f"  Average firing rate: {stats['avg_firing_rate']:.4f} Hz")
    
    # Display region information
    print(f"\nRegion Breakdown:")
    for i, region in enumerate(stats['regions']):
        print(f"  Region {i+1}: {region['neuron_count']} neurons, "
              f"{region['active_neurons']} active")
    
    print("\nExample 3 completed!\n")

def example_4_agent_world_simulation():
    """
    Example 4: Agent-World Simulation
    
    This example demonstrates how to create an agent that interacts with
    a simulated world, using the NLM brain for decision making.
    """
    print("=" * 60)
    print("Example 4: Agent-World Simulation")
    print("=" * 60)
    
    # Create world
    world = pynlm.createSimpleWorld(20, 20)  # 20x20 world
    print(f"Created {world.getWidth()}x{world.getHeight()} world")
    
    # Set up agent start position
    world.setAgentStart(10, 10)
    
    # Create neural brain for the agent
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 200)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    print("Created agent with neural brain for decision making")
    
    # Simulation loop
    print("\nRunning agent-world simulation...")
    for step in range(200):
        # Update world
        world.update(0.01)  # 10ms timestep
        
        # Agent processes sensory input
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        
        # Agent produces action
        action = agent_brain.decodeMotorCommand()
        
        # Apply action to world
        if action is not None:
            print(f"  Step {step}: Agent took action {action.getType()}")
        
        # Get agent's brain state for monitoring
        stats = pynlm.getBrainStatistics(brain)
        
        if step % 50 == 0:
            print(f"  Step {step}: Firing rate = {stats['avg_firing_rate']:.3f} Hz")
    
    # Display final results
    print(f"\nAgent Simulation Results:")
    print(f"  Final firing rate: {stats['avg_firing_rate']:.4f} Hz")
    print(f"  Total spikes: {stats['total_spikes']}")
    print(f"  Neuromodulation level: {agent_brain.getNeuromodulationLevel():.3f}")
    
    print("\nExample 4 completed!\n")

def example_5_performance_monitoring():
    """
    Example 5: Performance Monitoring and Debugging
    
    This example demonstrates how to use NLM's built-in performance
    monitoring and debugging capabilities to optimize simulations.
    """
    print("=" * 60)
    print("Example 5: Performance Monitoring and Debugging")
    print("=" * 60)
    
    # Create configuration with performance monitoring enabled
    config = pynlm.createDefaultConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Initialize global monitoring
    pynlm.initializeGlobalMonitoring()
    
    print("Performance monitoring initialized")
    print("Running simulation with detailed logging...")
    
    # Run simulation with monitoring
    for step in range(100):
        brain.step(step)
        
        # Log performance metrics periodically
        if step % 25 == 0:
            # Get current performance metrics
            perf_metrics = pynlm.getGlobalPerformanceMetrics()
            print(f"  Step {step}: Active neurons = {brain.getActiveNeuronCount()}, "
                  f"Spikes = {brain.getTotalSpikeCount()}")
    
    # Generate performance report
    perf_metrics = pynlm.getGlobalPerformanceMetrics()
    report = perf_metrics.generateReport()
    print(f"\nPerformance Report Generated:")
    print(f"  Report length: {len(report)} characters")
    print(f"  Total steps: {perf_metrics.getCurrentStep()}")
    
    # Show first few lines of report
    report_lines = report.split('\n')
    for i, line in enumerate(report_lines[:10]):
        print(f"    {line}")
    if len(report_lines) > 10:
        print(f"    ... ({len(report_lines) - 10} more lines)")
    
    # Clean up
    pynlm.shutdownGlobalMonitoring()
    
    print("\nExample 5 completed!\n")

def example_6_stdp_learning_demo():
    """
    Example 6: Spike-Timing-Dependent Plasticity (STDP) Learning Demo
    
    This example demonstrates Hebbian learning rules, specifically STDP,
    where synaptic connections strengthen when pre-synaptic and
    post-synaptic neurons fire in close temporal proximity.
    """
    print("=" * 60)
    print("Example 6: Spike-Timing-Dependent Plasticity (STDP) Learning")
    print("=" * 60)
    
    # Create brain with STDP enabled
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 150)
    config.set("stdp_enabled", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Created brain with STDP plasticity enabled")
    print("Demonstrating Hebbian learning through correlated activity...")
    
    # Simulate correlated activity that triggers STDP
    # This creates conditions where pre-synaptic and post-synaptic
    # neurons fire in close temporal proximity
    
    # Phase 1: Correlated pre-post activity (should strengthen synapses)
    print("  Phase 1: Simulating correlated pre->post activity (LTP)...")
    for step in range(200):
        # Inject current to create correlated firing
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)
        brain.step(step)
        
        if step % 50 == 0:
            print(f"    Step {step}: Running correlated activity phase")
    
    # Phase 2: Anti-correlated activity (should weaken synapses)
    print("  Phase 2: Simulating anti-correlated activity (LTD)...")
    for step in range(200, 400):
        # Inject different currents to create anti-correlation
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0)
        brain.step(step)
        
        if step % 50 == 0:
            print(f"    Step {step}: Running anti-correlated activity phase")
    
    # Analyze learning results
    stats = pynlm.getBrainStatistics(brain)
    print(f"\nSTDP Learning Analysis:")
    print(f"  Total learning steps: 400")
    print(f"  Total spikes: {stats['total_spikes']}")
    print(f"  Average firing rate: {stats['avg_firing_rate']:.4f} Hz")
    print(f"  E/I ratio: {stats['e_i_ratio']:.2f}")
    
    # Check if significant learning occurred
    if stats['avg_firing_rate'] > 2.0:
        print("  ✓ Strong learning detected: Network showed high activity")
        print("  ✓ STDP rules are effectively modifying synaptic weights")
    else:
        print("  ⚠ Limited learning response")
    
    print("\nExample 6 completed!\n")

def example_7_advanced_python_api():
    """
    Example 7: Advanced Python API Features
    
    This example demonstrates advanced features of the NLM Python API,
    including configuration management, batch operations, and custom
    neural architectures.
    """
    print("=" * 60)
    print("Example 7: Advanced Python API Features")
    print("=" * 60)
    
    # Create custom configuration
    config = pynlm.createDefaultConfig()
    
    # Custom settings
    config.set("neuron_count", 300)
    config.set("region_count", 3)
    config.set("connection_probability", 0.05)  # Sparse connectivity
    config.set("random_seed", 12345)  # Reproducible results
    
    print("Created custom configuration:")
    print(f"  Neurons: {config.getOr('neuron_count', 0)}")
    print(f"  Regions: {config.getOr('region_count', 0)}")
    print(f"  Connection probability: {config.getOr('connection_probability', 0.0)}")
    print(f"  Random seed: {config.getOr('random_seed', 0)}")
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create neural populations with specific characteristics
    print("\nCreating specialized neural populations...")
    
    # High-threshold sensory neurons (for detecting strong stimuli)
    strong_sensory, _ = pynlm.createNeuralPopulation(
        brain, 50, pynlm.NeuronType.Sensory, "StrongSensory"
    )
    
    # Low-threshold sensory neurons (for detecting weak stimuli)
    weak_sensory, _ = pynlm.createNeuralPopulation(
        brain, 100, pynlm.NeuronType.Sensory, "WeakSensory"
    )
    
    # Internal processing neurons
    internal, _ = pynlm.createNeuralPopulation(
        brain, 150, pynlm.NeuronType.Internal, "InternalProcessing"
    )
    
    print(f"  Created {brain.getTotalNeuronCount()} neurons across {brain.getRegionCount()} regions")
    
    # Run simulation with batch operations
    print("\nRunning advanced simulation with batch operations...")
    start_time = time.time()
    
    # Simulate multiple stimulus patterns
    patterns = [
        [5.0, 10.0, 7.0],
        [15.0, 8.0, 12.0],
        [3.0, 20.0, 5.0]
    ]
    
    for pattern_idx, pattern in enumerate(patterns):
        print(f"  Processing pattern {pattern_idx + 1}: {pattern}")
        pynlm.applyStimulusPattern(brain, pattern, 100)
        
        # Get intermediate statistics
        intermediate_stats = pynlm.getBrainStatistics(brain)
        print(f"    Average firing rate: {intermediate_stats['avg_firing_rate']:.3f} Hz")
    
    end_time = time.time()
    print(f"\nAdvanced simulation completed in {end_time - start_time:.2f} seconds")
    
    # Final analysis
    final_stats = pynlm.getBrainStatistics(brain)
    print(f"\nAdvanced API Results:")
    print(f"  Final firing rate: {final_stats['avg_firing_rate']:.4f} Hz")
    print(f"  Total processing steps: {brain.getTotalSpikeCount()}")
    print(f"  Network stability: {final_stats['e_i_ratio']:.2f} (E/I ratio)")
    
    print("\nExample 7 completed!\n")

def main():
    """
    Main function demonstrating all NLM examples
    """
    print("NLM (Neural Learning Machine) - Comprehensive Examples Suite")
    print("=" * 70)
    print("This script demonstrates various ways to use the NLM framework")
    print("for neural simulation, learning, and agent-based control.")
    print("=" * 70)
    
    # Run all examples (uncomment to run specific examples)
    print("\nRunning all examples...")
    
    example_1_basic_brain()
    example_2_learning_simulation()
    example_3_multi_region_brain()
    example_4_agent_world_simulation()
    example_5_performance_monitoring()
    example_6_stdp_learning_demo()
    example_7_advanced_python_api()
    
    print("=" * 70)
    print("All examples completed successfully!")
    print("=" * 70)
    print("\nKey takeaways:")
    print("• NLM provides flexible neural simulation capabilities")
    print("• Python API offers high-level abstractions for complex simulations")
    print("• Performance monitoring helps optimize neural networks")
    print("• Learning rules enable adaptive and intelligent behavior")
    print("• Multi-region architectures support complex neural computation")
    print("\nFor more detailed documentation, see the NLM documentation.")

if __name__ == "__main__":
    main()
