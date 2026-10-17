#!/usr/bin/env python3
"""
Comprehensive NLM Python Examples

This script demonstrates all the new features and enhancements in the NLM Python bindings.

Author: NLM Project
Date: 2026
License: MIT
"""

import pynlm
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime

def example_basic_usage():
    """Example 1: Basic usage with enhanced features"""
    print("=" * 60)
    print("Example 1: Basic Usage with Enhanced Features")
    print("=" * 60)
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.region_count", 2)
    
    # Create brain with context manager
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
        
        # Run basic simulation
        for i in range(100):
            brain.step(i)
            
        print(f"Total spikes after 100 steps: {brain.getTotalSpikeCount()}")
        print(f"Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")

def example_advanced_analysis():
    """Example 2: Advanced statistical analysis"""
    print("\n" + "=" * 60)
    print("Example 2: Advanced Statistical Analysis")
    print("=" * 60)
    
    # Create brain for analysis
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run longer simulation to generate rich data
    for i in range(2000):
        brain.step(i)
    
    # Perform comprehensive analysis
    print("Running advanced analysis...")
    
    # Analyze activity patterns
    activity_result = brain.analyzeActivityPattern()
    print(f"Activity Analysis: {activity_result}")
    
    # Compute network metrics
    network_metrics = brain.computeNetworkMetrics()
    print(f"Network Metrics:")
    print(f"  Connectivity Density: {network_metrics.connectivity_density:.4f}")
    print(f"  Path Length: {network_metrics.path_length:.4f}")
    print(f"  Clustering Coefficient: {network_metrics.clustering_coefficient:.4f}")
    
    # Get synchrony metrics
    synchrony_metrics = brain.getSynchronyMetrics()
    print(f"Synchrony Metrics:")
    print(f"  Mean Synchronization: {synchrony_metrics.mean_synchronization:.4f}")
    print(f"  Max Synchrony: {synchrony_metrics.max_synchrony:.4f}")
    
    # Detect bursts
    bursts = brain.detectBursts(threshold=2.5)
    print(f"Detected {len(bursts)} burst events")
    
    # Get frequency spectrum
    spectrum = brain.getFrequencySpectrum(sampling_rate=2000.0)
    print(f"Frequency spectrum length: {len(spectrum)}")
    
    # Create visualization
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Plot 1: Network metrics
    metrics_names = ['Connectivity', 'Path Length', 'Clustering', 'Small World', 'Assortativity']
    metrics_values = [network_metrics.connectivity_density, 
                     network_metrics.path_length,
                     network_metrics.clustering_coefficient,
                     network_metrics.small_world_coefficient,
                     network_metrics.assortativity]
    
    axes[0, 0].bar(metrics_names, metrics_values)
    axes[0, 0].set_title('Network Metrics')
    axes[0, 0].set_ylabel('Value')
    axes[0, 0].tick_params(axis='x', rotation=45)
    
    # Plot 2: Synchrony metrics
    synchrony_names = ['Mean Sync', 'Max Sync', 'Variance']
    synchrony_values = [synchrony_metrics.mean_synchronization,
                       synchrony_metrics.max_synchrony,
                       synchrony_metrics.synchrony_variance]
    
    axes[0, 1].bar(synchrony_names, synchrony_values, color=['skyblue', 'lightgreen', 'salmon'])
    axes[0, 1].set_title('Synchrony Metrics')
    axes[0, 1].set_ylabel('Value')
    
    # Plot 3: Activity histogram (simplified)
    firing_counts = [10, 25, 50, 75, 100, 150]
    axes[1, 0].plot(firing_counts, 'b-o', linewidth=2)
    axes[1, 0].set_title('Firing Count Distribution')
    axes[1, 0].set_xlabel('Bin')
    axes[1, 0].set_ylabel('Count')
    
    # Plot 4: Burst activity over time (simplified)
    time_points = np.linspace(0, 1000, 100)
    burst_activity = np.random.normal(0.5, 0.2, 100)
    axes[1, 1].plot(time_points, burst_activity, 'r-')
    axes[1, 1].set_title('Burst Activity (Simulated)')
    axes[1, 1].set_xlabel('Time Step')
    axes[1, 1].set_ylabel('Activity Level')
    
    plt.tight_layout()
    plt.savefig('analysis_visualization.png', dpi=150)
    print("Visualization saved as 'analysis_visualization.png'")
    
    plt.close()

def example_configuration_management():
    """Example 3: Configuration management utilities"""
    print("\n" + "=" * 60)
    print("Example 3: Configuration Management")
    print("=" * 60)
    
    # Create configuration with validation
    config = pynlm.createDefaultConfig()
    
    # Set various parameters
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.connection_probability", 0.05)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Validate configuration
    try:
        pynlm.validateConfig(config)
        print("✓ Configuration validated successfully")
    except pynlm.NLMConfigurationError as e:
        print(f"✗ Configuration error: {e}")
        return
    
    # Save configuration to file
    config.saveToFile("advanced_config.json")
    print("✓ Configuration saved to 'advanced_config.json'")
    
    # Load configuration from file
    loaded_config = pynlm.createDefaultConfig()
    if loaded_config.loadFromFile("advanced_config.json"):
        print("✓ Configuration loaded from file")
        
        # Show loaded configuration
        print(f"Loaded neurons: {loaded_config.get('brain.neuron_count')}")
        print(f"Loaded synapse density: {loaded_config.get('brain.synapse_density')}")
        
        # Modify loaded configuration
        loaded_config.set("brain.neuron_count", 1500)
        print("✓ Modified neuron count to 1500")
    
    # Show all configuration keys
    print(f"\nAll configuration keys: {config.getKeys()}")
    
    # Remove a key
    config.remove("neuromod.curiosity.enable")
    print("✓ Removed 'neuromod.curiosity.enable' from configuration")

def example_agent_simulation():
    """Example 4: Complete agent simulation with advanced features"""
    print("\n" + "=" * 60)
    print("Example 4: Complete Agent Simulation")
    print("=" * 60)
    
    # Setup agent simulation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation with monitoring
    num_steps = 500
    performance_data = []
    
    print(f"Running {num_steps}-step agent simulation...")
    
    for step in range(num_steps):
        # World update
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Get and apply motor command
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply neuromodulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect performance data
        if step % 50 == 0:
            metrics = {
                'step': step,
                'firing_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'dev_stage': brain.getDevelopmentalStage(),
                'prediction_error': agent.getPredictionError()
            }
            performance_data.append(metrics)
            
            print(f"Step {step}:")
            print(f"  Firing neurons: {metrics['firing_neurons']}")
            print(f"  Curiosity: {metrics['curiosity']:.3f}")
            print(f"  Novelty: {metrics['novelty']:.3f}")
            print(f"  Dev Stage: {metrics['dev_stage']}")
    
    # Analyze simulation results
    if performance_data:
        print(f"\nSimulation Results:")
        final_step = performance_data[-1]
        print(f"Final Firing Neurons: {final_step['firing_neurons']}")
        print(f"Final Curiosity: {final_step['curiosity']:.3f}")
        print(f"Final Novelty: {final_step['novelty']:.3f}")
        print(f"Final Development Stage: {final_step['dev_stage']}")
        
        avg_firing = np.mean([d['firing_neurons'] for d in performance_data])
        max_curiosity = max([d['curiosity'] for d in performance_data])
        
        print(f"\nPerformance Summary:")
        print(f"  Average Firing Neurons: {avg_firing:.1f}")
        print(f"  Maximum Curiosity Level: {max_curiosity:.3f}")
        print(f"  Final Developmental Stage: {final_step['dev_stage']}")
        print(f"  Total Steps Completed: {num_steps}")
    
    # Create agent simulation report
    with open("agent_simulation_report.txt", "w") as f:
        f.write("NLM Agent Simulation Report\n")
        f.write("=" * 50 + "\n")
        f.write(f"Generated: {datetime.now()}\n\n")
        f.write(f"Simulation Parameters:\n")
        f.write(f"  Total Steps: {num_steps}\n")
        f.write(f"  World Size: 30x30\n")
        f.write(f"  Vision: 10x10\n\n")
        
        if performance_data:
            final = performance_data[-1]
            f.write(f"Final Statistics:\n")
            f.write(f"  Firing Neurons: {final['firing_neurons']}\n")
            f.write(f"  Total Spikes: {final['total_spikes']}\n")
            f.write(f"  Curiosity Level: {final['curiosity']:.3f}\n")
            f.write(f"  Novelty Level: {final['novelty']:.3f}\n")
            f.write(f"  Development Stage: {final['dev_stage']}\n")
            f.write(f"  Prediction Error: {final['prediction_error']:.3f}\n")
        
        f.write(f"\nSimulation completed successfully!\n")
    
    print(f"✓ Simulation report saved to 'agent_simulation_report.txt'")

def example_performance_benchmarking():
    """Example 5: Performance benchmarking and metrics"""
    print("\n" + "=" * 60)
    print("Example 5: Performance Benchmarking")
    print("=" * 60)
    
    # Create brain for benchmarking
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Test different step counts
    step_counts = [100, 500, 1000, 2000]
    benchmark_results = {}
    
    print("Running performance benchmarks...")
    
    for steps in step_counts:
        brain.reset()
        brain.initialize()
        
        # Measure performance
        start_time = datetime.now()
        for i in range(steps):
            brain.step(i)
        end_time = datetime.now()
        
        duration = (end_time - start_time).total_seconds()
        spikes = brain.getTotalSpikeCount()
        
        benchmark_results[steps] = {
            'duration': duration,
            'spikes': spikes,
            'steps_per_second': steps / duration if duration > 0 else 0,
            'spikes_per_second': spikes / duration if duration > 0 else 0
        }
        
        print(f"Steps: {steps:4d} | Duration: {duration:.3f}s | "
              f"Steps/sec: {steps/duration:8.1f} | "
              f"Spikes: {spikes:6d}")
    
    # Create benchmark report
    print(f"\nBenchmark Report:")
    for steps, results in benchmark_results.items():
        print(f"  {steps:4d} steps: {results['duration']:6.3f}s, "
              f"{results['steps_per_second']:8.1f} steps/sec, "
              f"{results['spikes_per_second']:8.1f} spikes/sec")
    
    # Save benchmark results
    with open("benchmark_results.txt", "w") as f:
        f.write("NLM Performance Benchmark Results\n")
        f.write("=" * 50 + "\n\n")
        for steps, results in benchmark_results.items():
            f.write(f"Steps: {steps}\n")
            f.write(f"  Duration: {results['duration']:.3f}s\n")
            f.write(f"  Steps per second: {results['steps_per_second']:.1f}\n")
            f.write(f"  Spikes per second: {results['spikes_per_second']:.1f}\n")
            f.write(f"  Total spikes: {results['spikes']}\n\n")
    
    print(f"✓ Benchmark results saved to 'benchmark_results.txt'")

def example_training_analysis():
    """Example 6: Training and learning analysis"""
    print("\n" + "=" * 60)
    print("Example 6: Training and Learning Analysis")
    print("=" * 60)
    
    # Create brain for training
    config = pynlm.createDefaultConfig()
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create training input patterns
    input_pattern_1 = [0.5] * 100  # Regular pattern
    input_pattern_2 = [0.0] * 50 + [1.0] * 50  # Spike pattern
    input_pattern_3 = [0.2, 0.5, 0.8, 0.5, 0.2] * 20  # Oscillating pattern
    
    training_patterns = [input_pattern_1, input_pattern_2, input_pattern_3]
    
    # Train with each pattern
    print("Training with different input patterns...")
    
    for i, pattern in enumerate(training_patterns, 1):
        print(f"\nPattern {i}:")
        print(f"  Pattern type: {['Regular', 'Spike', 'Oscillating'][i-1]}")
        
        # Train with repeated presentations
        for epoch in range(5):
            brain.reset()
            brain.initialize()
            
            for step in range(100):
                brain.receiveSensoryInput(pattern)
                brain.step(step)
            
            spikes = brain.getTotalSpikeCount()
            print(f"    Epoch {epoch+1}: {spikes} spikes")
            
            if epoch < 4:
                # Apply some learning
                agent = pynlm.createAgentBrain(brain)
                agent.applyRewardModulation(0.5, 0.3)
                agent.updateDevelopment(0.1)
    
    # Evaluate performance
    print(f"\nTraining Evaluation:")
    print(f"Final neuron count: {brain.getTotalNeuronCount()}")
    print(f"Final synapse count: {brain.getTotalSynapseCount()}")
    print(f"Total spike count: {brain.getTotalSpikeCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Save training results
    with open("training_results.txt", "w") as f:
        f.write("NLM Training and Learning Analysis\n")
        f.write("=" * 50 + "\n\n")
        f.write(f"Training completed successfully!\n\n")
        f.write(f"Final Statistics:\n")
        f.write(f"  Total Neurons: {brain.getTotalNeuronCount()}\n")
        f.write(f"  Total Synapses: {brain.getTotalSynapseCount()}\n")
        f.write(f"  Total Spikes: {brain.getTotalSpikeCount()}\n")
        f.write(f"  Firing Neurons: {brain.getFiringNeuronCount()}\n")
        f.write(f"  Average Firing Rate: {brain.getAverageFiringRate():.2f} Hz\n")
        f.write(f"  E/I Ratio: {brain.getExcitationInhibitionRatio():.3f}\n")
        f.write(f"  Developmental Stage: {brain.getDevelopmentalStage()}\n")
        f.write(f"\nTraining completed with {len(training_patterns)} different patterns!\n")
    
    print(f"✓ Training results saved to 'training_results.txt'")

def main():
    """Run all examples"""
    print("NLM Python Examples")
    print("=" * 60)
    print("This script demonstrates all new features in NLM Python bindings.")
    print()
    
    # Run all examples
    example_basic_usage()
    example_advanced_analysis()
    example_configuration_management()
    example_agent_simulation()
    example_performance_benchmarking()
    example_training_analysis()
    
    print("\n" + "=" * 60)
    print("All examples completed successfully!")
    print("=" * 60)
    print("\nKey features demonstrated:")
    print("1. ✓ Basic usage with context managers")
    print("2. ✓ Advanced statistical analysis and visualization")
    print("3. ✓ Configuration validation and management")
    print("4. ✓ Complete agent simulation with neuromodulation")
    print("5. ✓ Performance benchmarking")
    print("6. ✓ Training and learning analysis")
    print("\nCheck the output files and visualizations in the current directory.")

if __name__ == "__main__":
    main()