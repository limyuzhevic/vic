#!/usr/bin/env python3
"""
Advanced NLM Usage Examples and Tutorials

This script demonstrates advanced usage patterns and best practices
for the Neural Learning Machine (NLM) framework with the enhanced
features including batch processing, time series collection,
performance monitoring, and validation.

Examples:
1. Batch processing for efficient simulation runs
2. Time series data collection and analysis
3. Advanced simulation with multiple subsystems
4. Configuration validation and error handling
5. Performance benchmarking
6. Neural activity analysis
"""

import pynlm
import time
import json
import os
from typing import Dict, List, Optional, Callable

def example_1_batch_processing():
    """Example 1: Batch processing for efficient simulation runs"""
    print("=" * 60)
    print("Example 1: Batch Processing for Efficient Simulation")
    print("=" * 60)
    
    # Create configuration optimized for batch processing
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning subsystems for comprehensive training
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Run simulation in large batches
    total_steps = 5000
    batch_size = 500
    
    print(f"Running {total_steps} steps in {batch_size}-step batches...")
    
    for batch in range(0, total_steps, batch_size):
        batch_steps = min(batch_size, total_steps - batch)
        
        # Progress callback to show batch completion
        def progress_callback(step):
            if step % 100 == 0:
                print(f"  Batch progress: Step {step}")
            return True
        
        # Run batch simulation
        pynlm.runBatchSimulation(brain, world, agent, batch_steps, 100, True)
        
        # Collect statistics
        firing_rate = brain.getAverageFiringRate()
        total_spikes = brain.getTotalSpikeCount()
        curiosity = agent.getCuriosityLevel()
        development_stage = brain.getDevelopmentalStage()
        
        print(f"  Batch {batch//batch_size + 1}: Firing rate={firing_rate:.3f}, "
              f"Spikes={total_spikes}, Curiosity={curiosity:.3f}, "
              f"Dev Stage={development_stage}")
    
    print("Batch processing complete!")
    print(f"Final metrics: Firing rate={brain.getAverageFiringRate():.3f}, "
          f"Total spikes={brain.getTotalSpikeCount()}")
    print()

def example_2_time_series_collection():
    """Example 2: Time series data collection and analysis"""
    print("=" * 60)
    print("Example 2: Time Series Data Collection and Analysis")
    print("=" * 60)
    
    # Setup simulation environment
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Collect comprehensive time series data
    print("Collecting time series data over 1000 steps...")
    
    start_time = time.time()
    results = pynlm.collectTimeSeriesData(brain, world, agent, 1000)
    collection_time = time.time() - start_time
    
    print(f"Data collection completed in {collection_time:.2f} seconds")
    print(f"Collected {len(results['spike_counts'])} data points")
    
    # Analyze time series data
    spike_counts = results['spike_counts']
    firing_rates = results['firing_rates']
    simulation_times = results['simulation_times']
    
    # Calculate statistics
    def calculate_stats(data, name):
        if not data:
            return {}
        
        return {
            'mean': sum(data) / len(data),
            'std': (sum((x - sum(data)/len(data))**2 for x in data) / len(data))**0.5,
            'min': min(data),
            'max': max(data),
            'range': max(data) - min(data),
            'stability': 1.0 - (max(data) - min(data)) / (sum(data) / len(data) + 1e-6)
        }
    
    spike_stats = calculate_stats(spike_counts, "spike_counts")
    firing_rate_stats = calculate_stats(firing_rates, "firing_rates")
    
    # Display analysis results
    print("\nTime Series Analysis:")
    print(f"Spike Count Statistics:")
    print(f"  Mean: {spike_stats['mean']:.2f}, Std: {spike_stats['std']:.2f}")
    print(f"  Min: {spike_stats['min']}, Max: {spike_stats['max']}")
    print(f"  Stability: {spike_stats['stability']:.3f}")
    
    print(f"\nFiring Rate Statistics:")
    print(f"  Mean: {firing_rate_stats['mean']:.3f}, Std: {firing_rate_stats['std']:.3f}")
    print(f"  Min: {firing_rate_stats['min']:.3f}, Max: {firing_rate_stats['max']:.3f}")
    print(f"  Range: {firing_rate_stats['range']:.3f}")
    
    # Identify patterns
    stable_periods = sum(1 for sr in firing_rates if sr < firing_rate_stats['mean'] + 0.1 * firing_rate_stats['std'])
    print(f"Stable firing periods (< μ+σ): {stable_periods}/{len(firing_rates)} ({stable_periods/len(firing_rates)*100:.1f}%)")
    
    # Save time series data to file
    output_file = "time_series_data.json"
    with open(output_file, 'w') as f:
        json.dump({
            'spike_counts': spike_counts,
            'firing_rates': firing_rates,
            'simulation_times': simulation_times,
            'statistics': {
                'spike_count': spike_stats,
                'firing_rate': firing_rate_stats
            }
        }, f, indent=2)
    
    print(f"\nTime series data saved to: {output_file}")
    print()

def example_3_advanced_simulation():
    """Example 3: Advanced simulation with multiple subsystems"""
    print("=" * 60)
    print("Example 3: Advanced Simulation with Multiple Subsystems")
    print("=" * 60)
    
    # Create custom configuration for advanced simulation
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 300)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    # Create brain with advanced configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Setup environment
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(12.5, 12.5)
    world.setMaxEnergy(200.0)
    
    # Create agent with advanced capabilities
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Run advanced simulation
    print("Running advanced simulation with all subsystems enabled...")
    print("Simulation parameters:")
    print(f"  Duration: 2000 steps")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Max energy: {world.getMaxEnergy()}")
    print(f"  Vision: {world.getWidth()}x{world.getHeight()} world,")
    print(f"         {world.getWidth()}x{world.getHeight()} vision")
    
    start_time = time.time()
    results = pynlm.runAdvancedSimulation(brain, world, agent, 2000)
    end_time = time.time()
    
    simulation_time = end_time - start_time
    
    # Display advanced simulation results
    print("\nAdvanced Simulation Results:")
    print(f"Simulation completed in {simulation_time:.2f} seconds")
    print(f"  Final spike count: {results['final_spike_count']}")
    print(f"  Final firing neurons: {results['final_firing_neurons']}")
    print(f"  Final firing rate: {results['final_firing_rate']:.3f}")
    print(f"  Final developmental stage: {results['final_developmental_stage']}")
    print(f"  Curiosity level: {results['curiosity_level']:.3f}")
    print(f"  Novelty level: {results['novelty_level']:.3f}")
    print(f"  Prediction error: {results['prediction_error']:.3f}")
    print(f"  Neuromodulation level: {results['neuromodulation_level']:.3f}")
    
    # Analyze developmental progression
    stages = {
        pynlm.DevelopmentalStage.Initial: "Initial",
        pynlm.DevelopmentalStage.CriticalPeriod: "Critical Period",
        pynlm.DevelopmentalStage.Maturation: "Maturation",
        pynlm.DevelopmentalStage.Adult: "Adult",
        pynlm.DevelopmentalStage.Aging: "Aging"
    }
    
    current_stage = stages.get(results['final_developmental_stage'], f"Unknown ({results['final_developmental_stage']})")
    print(f"  Brain development stage: {current_stage}")
    
    # Determine simulation quality
    quality_metrics = []
    if results['final_firing_rate'] > 1.0:
        quality_metrics.append("Good firing activity")
    if results['curiosity_level'] > 0.5:
        quality_metrics.append("High exploration drive")
    if results['neuromodulation_level'] > 0.3:
        quality_metrics.append("Strong learning signals")
    if results['prediction_error'] < 0.5:
        quality_metrics.append("Accurate predictions")
    
    print(f"Simulation quality: {', '.join(quality_metrics) if quality_metrics else 'Standard'}")
    print()

def example_4_configuration_validation():
    """Example 4: Configuration validation and error handling"""
    print("=" * 60)
    print("Example 4: Configuration Validation and Error Handling")
    print("=" * 60)
    
    # Test validation with different configurations
    test_configs = [
        ("Optimal configuration", {
            "brain.neuron_count": 500,
            "simulation_timestep": 0.001,
            "brain.connection_probability": 0.1,
            "plasticity.stdp.enable": True
        }),
        ("Invalid configuration (negative neurons)", {
            "brain.neuron_count": -100,  # Invalid
            "simulation_timestep": 0.001,
            "brain.connection_probability": 0.1
        }),
        ("Invalid configuration (bad timestep)", {
            "brain.neuron_count": 500,
            "simulation_timestep": -0.1,  # Invalid
            "brain.connection_probability": 0.1
        }),
        ("Invalid configuration (invalid probability)", {
            "brain.neuron_count": 500,
            "simulation_timestep": 0.001,
            "brain.connection_probability": 1.5  # Invalid
        })
    ]
    
    for config_name, config_params in test_configs:
        print(f"\nTesting: {config_name}")
        print("-" * 40)
        
        config = pynlm.createDefaultConfig()
        
        # Apply configuration parameters
        for key, value in config_params.items():
            config.set(key, value)
        
        # Validate configuration
        validation_results = pynlm.validateConfiguration(config)
        
        if validation_results["valid"]:
            print("✓ Configuration is valid")
        else:
            print("✗ Configuration has errors:")
            for error in validation_results["errors"]:
                print(f"    - {error}")
        
        if validation_results["warnings"]:
            print("Warnings:")
            for warning in validation_results["warnings"]:
                print(f"    - {warning}")
    
    print("\nConfiguration validation complete!")
    print()

def example_5_performance_benchmarking():
    """Example 5: Performance benchmarking"""
    print("=" * 60)
    print("Example 5: Performance Benchmarking")
    print("=" * 60)
    
    # Create configurations for different benchmarking scenarios
    benchmark_configs = [
        ("Light configuration", {
            "brain.neuron_count": 100,
            "simulation_timestep": 0.001
        }),
        ("Medium configuration", {
            "brain.neuron_count": 500,
            "simulation_timestep": 0.001
        }),
        ("Heavy configuration", {
            "brain.neuron_count": 1000,
            "simulation_timestep": 0.001
        })
    ]
    
    benchmark_steps = 1000
    warmup_steps = 100
    
    print(f"Benchmarking brain performance with {benchmark_steps} steps (warmup: {warmup_steps})")
    print()
    
    for config_name, config_params in benchmark_configs:
        print(f"Testing: {config_name}")
        
        config = pynlm.createDefaultConfig()
        for key, value in config_params.items():
            config.set(key, value)
        
        start_time = time.time()
        benchmark_results = pynlm.benchmarkBrain(config, benchmark_steps, warmup_steps)
        end_time = time.time()
        
        actual_duration = end_time - start_time
        
        print(f"  Configuration: Neurons={config_params['brain.neuron_count']}, "
              f"Steps={benchmark_steps}")
        print(f"  Benchmark results:")
        print(f"    Total duration: {benchmark_results['total_duration_ms']:.2f} ms")
        print(f"    Real duration: {actual_duration:.2f} ms")
        print(f"    Steps per second: {benchmark_results['steps_per_second']:.2f}")
        print(f"    Average step time: {benchmark_results['avg_time_per_step_ms']:.4f} ms")
        print(f"    Final spike count: {benchmark_results['final_spike_count']}")
        print(f"    Final firing neurons: {benchmark_results['final_firing_neurons']}")
        
        # Calculate efficiency
        theoretical_max_steps = 1000000 / benchmark_results['avg_time_per_step_ms']  # Assuming 1 second max per step
        efficiency = benchmark_results['steps_per_second'] / theoretical_max_steps * 100
        print(f"    Efficiency: {efficiency:.2f}%")
        print()
    
    print("Benchmarking complete!")
    print()

def example_6_neural_activity_analysis():
    """Example 6: Neural activity analysis"""
    print("=" * 60)
    print("Example 6: Neural Activity Analysis")
    print("=" * 60)
    
    # Setup simulation
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run for a while to collect data
    print("Running simulation to collect neural activity data...")
    for step in range(500):
        brain.step(step)
    
    # Analyze neural activity
    print("\nNeural Activity Analysis:")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Excitation/inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    # Get neural activity analysis
    activity_analysis = pynlm.analyzeNeuralActivity(brain, 200)
    
    print(f"\nNeural Activity Analysis (200-step window):")
    print(f"Window size: {activity_analysis['window_size']}")
    print(f"Average firing rate: {activity_analysis['avg_firing_rate']:.3f}")
    print(f"Average spike count: {activity_analysis['avg_spike_count']:.1f}")
    print(f"Average E/I ratio: {activity_analysis['avg_ei_ratio']:.3f}")
    print(f"Firing rate variance: {activity_analysis['firing_rate_variance']:.3f}")
    print(f"Activity stability: {activity_analysis['activity_stability']:.3f}")
    print(f"Total neurons in network: {activity_analysis['total_neurons']}")
    print(f"Total synapses in network: {activity_analysis['total_synapses']}")
    
    # Assess brain state
    brain_state = "Normal"
    if activity_analysis['activity_stability'] < 0.5:
        brain_state = "Unstable"
    elif activity_analysis['avg_firing_rate'] < 0.1:
        brain_state = "Quiescent"
    elif activity_analysis['avg_firing_rate'] > 10.0:
        brain_state = "Hyperactive"
    
    print(f"\nBrain State Assessment: {brain_state}")
    
    # Recommendations
    if brain_state == "Unstable":
        print("Recommendations:")
        print("  - Consider increasing simulation timestep")
        print("  - Check for excessive inhibition")
        print("  - Adjust plasticity parameters")
    
    elif brain_state == "Quiescent":
        print("Recommendations:")
        print("  - Increase input excitation")
        print("  - Lower neuron threshold")
        print("  - Add more sensory input")
    
    elif brain_state == "Hyperactive":
        print("Recommendations:")
        print("  - Increase neuron inhibition")
        print("  - Adjust synaptic weights")
        print("  - Reduce input strength")
    
    print()

def main():
    """Run all examples"""
    print("NLM Advanced Usage Examples")
    print("=" * 60)
    print("This script demonstrates advanced usage patterns and best practices")
    print("for the Neural Learning Machine framework with enhanced features.")
    print()
    
    # Check if all required functions are available
    required_functions = [
        'runBatchSimulation',
        'collectTimeSeriesData', 
        'runAdvancedSimulation',
        'validateConfiguration',
        'benchmarkBrain',
        'analyzeNeuralActivity'
    ]
    
    for func_name in required_functions:
        if not hasattr(pynlm, func_name):
            print(f"Warning: Function '{func_name}' is not available.")
            print("Some examples may not work correctly.")
            print()
    
    # Run examples (comment out any you want to skip)
    try:
        example_1_batch_processing()
    except Exception as e:
        print(f"Example 1 failed: {e}")
        print()
    
    try:
        example_2_time_series_collection()
    except Exception as e:
        print(f"Example 2 failed: {e}")
        print()
    
    try:
        example_3_advanced_simulation()
    except Exception as e:
        print(f"Example 3 failed: {e}")
        print()
    
    try:
        example_4_configuration_validation()
    except Exception as e:
        print(f"Example 4 failed: {e}")
        print()
    
    try:
        example_5_performance_benchmarking()
    except Exception as e:
        print(f"Example 5 failed: {e}")
        print()
    
    try:
        example_6_neural_activity_analysis()
    except Exception as e:
        print(f"Example 6 failed: {e}")
        print()
    
    print("=" * 60)
    print("All examples completed!")
    print("=" * 60)
    print("\nFor more information, refer to:")
    print("  - easy_usage.md - Basic usage guide")
    print("  - HOW_TO_USE.md - Comprehensive documentation")
    print("  - docs/ARCHITECTURE.md - System architecture")
    print("  - docs/SCIENCE.md - Scientific background")
    print("  - docs/EXPERIMENTS.md - Experiment descriptions")

if __name__ == "__main__":
    main()