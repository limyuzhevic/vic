#!/usr/bin/env python3
"""
NLM Advanced Usage Examples
==========================

Comprehensive examples demonstrating advanced NLM usage patterns
for research, production, and development scenarios.
"""

import pynlm
import json
import time
from pathlib import Path
from nlm_advanced_utilities import (
    NLMConfigManager, 
    NLMDataAnalyzer, 
    NLMSimulationProfiler,
    create_advanced_config_templates
)

def example_1_basic_usage():
    """
    Example 1: Basic NLM Usage
    =========================
    
    Demonstrates the fundamental NLM workflow for simple neural simulation.
    """
    print("Example 1: Basic NLM Usage")
    print("=" * 40)
    
    # Create brain with default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain has {brain.getTotalSynapseCount()} synapses")
    
    # Run simulation
    for i in range(50):
        brain.step(i)
        
    print(f"Simulation completed!")
    print(f"Final stats: {brain.getTotalSpikeCount()} spikes, "
          f"{brain.getFiringNeuronCount()} firing neurons")
    
    return brain

def example_2_configuration_management():
    """
    Example 2: Advanced Configuration Management
    ============================================
    
    Demonstrates advanced configuration management with config templates
    and validation.
    """
    print("\nExample 2: Advanced Configuration Management")
    print("=" * 50)
    
    # Initialize config manager
    config_manager = NLMConfigManager()
    
    # Create research configuration
    research_config = config_manager.create_research_config(
        neuron_count=3000,
        network_type="dense",
        plasticity_mode="stdp+hebbian",
        neuromodulation_enabled=True
    )
    
    print("Research configuration created:")
    print(f"  Neurons: {research_config['brain']['neuron_count']}")
    print(f"  Synapses: {research_config['brain']['neuron_count'] * 5}")
    print(f"  STDP enabled: {research_config['plasticity']['stdp']['enable']}")
    print(f"  Hebbian enabled: {research_config['plasticity']['hebbian']['enable']}")
    
    # Save configuration
    config_manager.save_config_to_file(research_config, "research_config.json")
    print("Configuration saved to research_config.json")
    
    # Create different configuration templates
    templates = create_advanced_config_templates()
    print(f"\nAvailable configuration templates: {list(templates.keys())}")
    
    # Load and compare configurations
    if Path("research_config.json").exists():
        loaded_config = config_manager.load_config_from_file("research_config.json")
        print(f"\nConfiguration validation passed: {loaded_config['brain']['neuron_count']} neurons")
        
        # Compare configurations
        differences = config_manager.compare_configs(research_config, loaded_config)
        print(f"Configuration comparison:")
        print(f"  Added keys: {differences['added_keys']}")
        print(f"  Removed keys: {differences['removed_keys']}")
        print(f"  Modified keys: {len(differences['modified_keys'])}")
        print(f"  Equal keys: {len(differences['equal_keys'])}")
    
    return config_manager, research_config

def example_3_data_analysis():
    """
    Example 3: Advanced Data Analysis
    =================================
    
    Demonstrates advanced data analysis utilities for neural simulation data.
    """
    print("\nExample 3: Advanced Data Analysis")
    print("=" * 40)
    
    # Initialize data analyzer
    analyzer = NLMDataAnalyzer()
    
    # Create sample spike count data
    spike_counts = [i * 2 for i in range(100)]  # Simulated data
    
    # Analyze spike dynamics
    spike_analysis = analyzer.analyze_spike_dynamics(spike_counts)
    print("Spike Dynamics Analysis:")
    print(f"  Total spikes: {spike_analysis['total_spikes']}")
    print(f"  Mean spikes: {spike_analysis['mean_spikes']:.2f}")
    print(f"  Spike frequency: {spike_analysis['spike_frequency']:.2f} spikes/step")
    print(f"  High activity periods: {spike_analysis['high_activity_periods']}")
    
    # Create sample neural activity data
    firing_rates = [0.1 * i for i in range(100)]
    e_i_ratios = [1.0 + 0.1 * i for i in range(100)]
    
    # Analyze neural activity
    neural_analysis = analyzer.analyze_neural_activity(firing_rates, e_i_ratios)
    print(f"\nNeural Activity Analysis:")
    if 'firing_rate' in neural_analysis:
        fr = neural_analysis['firing_rate']
        print(f"  Firing rate stability: {fr['stability']:.3f}")
        print(f"  Firing rate range: {fr['min']:.3f} - {fr['max']:.3f}")
    
    if 'e_i_balance' in neural_analysis:
        eib = neural_analysis['e_i_balance']
        print(f"  E/I balance stability: {eib['stability']:.3f}")
        print(f"  Imbalance periods: {eib['imbalance_periods']}")
    
    return analyzer, spike_counts, firing_rates, e_i_ratios

def example_4_complete_simulation():
    """
    Example 4: Complete Advanced Simulation
    ======================================
    
    Demonstrates a complete advanced simulation with data collection
    and performance profiling.
    """
    print("\nExample 4: Complete Advanced Simulation")
    print("=" * 45)
    
    # Initialize profiler
    profiler = NLMSimulationProfiler()
    
    # Create configuration for advanced simulation
    config = {
        "brain": {
            "neuron_count": 2000,
            "region_count": 2,
            "connection_probability": 0.05
        },
        "plasticity": {
            "stdp": {"enable": True, "learning_rate": 0.01},
            "hebbian": {"enable": True, "learning_rate": 0.005}
        }
    }
    
    # Run simulation with profiling
    def advanced_simulation():
        """Advanced simulation function."""
        # Create brain and run simulation
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Simple simulation loop
        for i in range(100):
            brain.step(i)
            
        return {
            'brain': brain,
            'final_spikes': brain.getTotalSpikeCount(),
            'final_neurons': brain.getTotalNeuronCount()
        }
    
    print("Running advanced simulation with profiling...")
    start_time = time.time()
    
    profile = profiler.profile_simulation(advanced_simulation)
    
    end_time = time.time()
    
    print(f"Simulation completed in {profile['execution_time']:.3f} seconds")
    print(f"Memory usage: {profile['memory_usage']['rss'] / (1024 * 1024):.2f} MB")
    print(f"CPU usage: {profile['cpu_usage']['cpu_percent']:.1f}%")
    
    return profiler, profile

def example_5_integration_demo():
    """
    Example 5: Integration Demo
    ==========================
    
    Demonstrates integration of all NLM components with utilities.
    """
    print("\nExample 5: Integration Demo")
    print("=" * 30)
    
    # Create all components
    print("Creating NLM components...")
    
    # Configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    
    # Brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Agent
    agent = pynlm.createAgentBrain(brain)
    
    # World
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.setAgentStart(10.0, 10.0)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"Components created:")
    print(f"  Brain: {brain.getTotalNeuronCount()} neurons")
    print(f"  Agent: Sensory size={agent.getSensoryInputSize()}, Motor size={agent.getMotorOutputSize()}")
    print(f"  World: {world.getWidth()}x{world.getHeight()}")
    
    # Run integration simulation
    print(f"\nRunning integration simulation...")
    for step in range(50):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        action_result = world.applyMotorCommand(action, world.getSimulationTime())
        
        agent.applyRewardModulation(action_result.reward, 0.0)
        agent.updateDevelopment(0.1)
        
    print(f"Integration simulation completed!")
    print(f"Final state:")
    print(f"  Brain: {brain.getTotalSpikeCount()} spikes")
    print(f"  Agent: Dopamine={agent.getNeuromodulationLevel():.3f}, "
          f"Curiosity={agent.getCuriosityLevel():.3f}")
    print(f"  World: Position=({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    return brain, agent, world

def example_6_performance_benchmark():
    """
    Example 6: Performance Benchmarking
    ===================================
    
    Demonstrates performance benchmarking with different configurations.
    """
    print("\nExample 6: Performance Benchmarking")
    print("=" * 40)
    
    # Create profiler
    profiler = NLMSimulationProfiler()
    
    # Create different simulation configurations
    configs = [
        {"brain": {"neuron_count": 1000, "region_count": 1}},
        {"brain": {"neuron_count": 2000, "region_count": 2}},
        {"brain": {"neuron_count": 3000, "region_count": 3}},
    ]
    
    print("Running performance benchmarks...")
    benchmark_results = profiler.benchmark_performance(configs)
    
    # Generate performance report
    report = profiler.generate_performance_report(benchmark_results)
    print(report)
    
    return profiler, benchmark_results

def example_7_visualization_preparation():
    """
    Example 7: Visualization Data Preparation
    =========================================
    
    Demonstrates preparation of data for visualization.
    """
    print("\nExample 7: Visualization Data Preparation")
    print("=" * 45)
    
    # Initialize analyzer
    analyzer = NLMDataAnalyzer()
    
    # Create sample data
    experiment_data = {
        'steps': list(range(100)),
        'spike_counts': [i * 2 + 10 for i in range(100)],
        'energy_levels': [100 - i * 0.5 for i in range(100)],
        'firing_rates': [0.1 * i for i in range(100)],
        'dopamine_levels': [0.5 * (i % 20) / 20 for i in range(100)],
        'prediction_errors': [0.1 * (i % 10) / 10 for i in range(100)]
    }
    
    print("Preparing visualization data...")
    viz_data = analyzer.prepare_visualization_data(experiment_data)
    
    print(f"Visualization data prepared with metrics:")
    for metric in viz_data['time_series'].keys():
        stats = viz_data['summary_statistics'][metric]
        print(f"  {metric}: mean={stats['mean']:.2f}, std={stats['std']:.2f}, range={stats['range']:.2f}")
        
    # Prepare data for specific visualization
    print(f"\nSample processed metrics for line plot:")
    if 'spike_counts' in viz_data['processed_metrics']:
        spike_metrics = viz_data['processed_metrics']['spike_counts']
        print(f"  Moving average length: {len(spike_metrics['moving_average'])}")
        print(f"  Normalized range: {min(spike_metrics['normalized']):.3f} - {max(spike_metrics['normalized']):.3f}")
        
    return analyzer, viz_data

def main():
    """
    Main function to run all examples.
    
    This function demonstrates all the advanced NLM features and utilities
    in a comprehensive workflow.
    """
    print("NLM Advanced Usage Examples")
    print("=" * 60)
    print("This script demonstrates advanced NLM usage patterns including:")
    print("  1. Basic NLM usage")
    print("  2. Advanced configuration management")
    print("  3. Data analysis utilities")
    print("  4. Complete simulation with profiling")
    print("  5. Integration demo")
    print("  6. Performance benchmarking")
    print("  7. Visualization data preparation")
    print()
    
    # Run all examples
    examples = [
        ("Basic Usage", example_1_basic_usage),
        ("Configuration Management", example_2_configuration_management),
        ("Data Analysis", example_3_data_analysis),
        ("Complete Simulation", example_4_complete_simulation),
        ("Integration Demo", example_5_integration_demo),
        ("Performance Benchmarking", example_6_performance_benchmark),
        ("Visualization Preparation", example_7_visualization_preparation),
    ]
    
    results = {}
    
    for example_name, example_func in examples:
        try:
            print(f"\n{'='*60}")
            result = example_func()
            results[example_name] = result
            print(f"✓ {example_name} completed successfully")
        except Exception as e:
            print(f"✗ {example_name} failed: {e}")
            results[example_name] = None
            
    # Summary
    print(f"\n{'='*60}")
    print("SUMMARY")
    print("=" * 60)
    print("All examples completed!")
    print(f"Successfully demonstrated {len([r for r in results.values() if r is not None])}/{len(results)} examples")
    
    # Save results
    with open("examples_results.json", "w") as f:
        json.dump({
            "examples_completed": list(results.keys()),
            "successful_examples": [name for name, result in results.items() if result is not None],
            "failed_examples": [name for name, result in results.items() if result is None]
        }, f, indent=2)
        
    print("\nExample results saved to examples_results.json")
    
    return results

if __name__ == "__main__":
    main()