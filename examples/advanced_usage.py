#!/usr/bin/env python3
"""
Advanced NLM Usage Examples

This script demonstrates advanced usage patterns for power users of the NLM framework.
It shows how to:
1. Load complex configurations
2. Run multiple simulations with different parameters
3. Analyze results programmatically
4. Use advanced Python features
"""

import pynlm
import json
import time
import os
from pathlib import Path

def advanced_config_example():
    """Example: Create complex configurations with nested settings"""
    print("=== Advanced Configuration Example ===")
    
    # Create a complete configuration
    config = pynlm.createDefaultConfig()
    
    # Set nested brain configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.region_count", 3)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.connection_probability", 0.2)
    
    # Set advanced brain parameters
    config.set("brain.v_thresh", -55.0)
    config.set("brain.v_rest", -70.0)
    config.set("brain.v_reset", -75.0)
    config.set("brain.tau_mem", 20.0)
    config.set("brain.tau_ref", 2.0)
    
    # Set up plasticity systems
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.stdp.tau_plus", 20.0)
    config.set("plasticity.stdp.tau_minus", 20.0)
    
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.01)
    
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.0001)
    config.set("plasticity.structural.pruning_rate", 0.00001)
    
    # Set up neuromodulation
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Set up development
    config.set("development.initial_stage", 0)
    config.set("development.critical_period_duration", 100)
    config.set("development.maturation_rate", 0.01)
    
    # Save configuration to JSON file
    config.saveToFile("advanced_config.json")
    
    print(f"Configuration saved to advanced_config.json")
    print(f"Config has {len(config.getKeys())} settings")
    
    # Show some key settings
    print("Key settings:")
    for key in ["brain.neuron_count", "plasticity.stdp.learning_rate", "neuromod.curiosity.enable"]:
        if config.has(key):
            print(f"  {key}: {config.get(key)}")
    
    return config

def batch_simulation_example():
    """Example: Run multiple simulations with parameter variations"""
    print("\n=== Batch Simulation Example ===")
    
    # Define parameter space to explore
    parameters = [
        {"seed": 42, "neurons": 1000, "steps": 5000},
        {"seed": 123, "neurons": 1000, "steps": 5000},
        {"seed": 456, "neurons": 1000, "steps": 5000},
        {"seed": 789, "neurons": 2000, "steps": 10000},
        {"seed": 999, "neurons": 2000, "steps": 10000},
    ]
    
    results = []
    
    for i, params in enumerate(parameters):
        print(f"Running simulation {i+1}/{len(parameters)}...")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", params["neurons"])
        config.set("random_seed", params["seed"])
        config.set("simulation.timestep", 0.001)
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        start_time = time.time()
        for step in range(params["steps"]):
            brain.step(step)
            
            # Log progress every 1000 steps
            if step % 1000 == 0:
                print(f"  Step {step}/{params['steps']}, Firing: {brain.getFiringNeuronCount()}")
        
        end_time = time.time()
        
        # Collect results
        result = {
            "simulation_id": i + 1,
            "seed": params["seed"],
            "neurons": params["neurons"],
            "steps": params["steps"],
            "final_spikes": brain.getTotalSpikeCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "avg_firing_rate": brain.getAverageFiringRate(),
            "duration": end_time - start_time,
            "e_i_ratio": brain.getExcitationInhibitionRatio()
        }
        
        results.append(result)
        print(f"  Completed. Total spikes: {result['final_spikes']}")
    
    # Save results to JSON file
    with open("batch_results.json", "w") as f:
        json.dump(results, f, indent=2)
    
    print(f"\nBatch simulation completed. Results saved to batch_results.json")
    
    # Show summary statistics
    print("\nBatch Results Summary:")
    print(f"  Total simulations: {len(results)}")
    print(f"  Average spikes per simulation: {sum(r['final_spikes'] for r in results) / len(results):.0f}")
    print(f"  Average duration: {sum(r['duration'] for r in results) / len(results):.1f}s")
    
    return results

def advanced_analysis_example():
    """Example: Advanced analysis of brain simulation results"""
    print("\n=== Advanced Analysis Example ===")
    
    # Create a brain with some activity
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("random_seed", 42)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation and record data
    print("Running simulation and recording detailed metrics...")
    
    spike_history = []
    firing_rate_history = []
    time_stamps = []
    
    for step in range(2000):
        brain.step(step)
        
        if step % 100 == 0:
            spike_count = brain.getTotalSpikeCount()
            firing_count = brain.getFiringNeuronCount()
            avg_rate = brain.getAverageFiringRate()
            
            spike_history.append(spike_count)
            firing_rate_history.append(firing_count)
            time_stamps.append(step)
            
            print(f"  Step {step}: Spikes={spike_count}, Firing={firing_count}, Rate={avg_rate:.2f}Hz")
    
    # Advanced analysis
    print("\n--- Advanced Analysis ---")
    
    # Calculate statistics
    total_steps = len(spike_history)
    avg_spikes_per_step = sum(spike_history) / total_steps if total_steps > 0 else 0
    max_spikes = max(spike_history)
    min_spikes = min(spike_history)
    spike_std = (sum((x - avg_spikes_per_step) ** 2 for x in spike_history) / total_steps) ** 0.5 if total_steps > 0 else 0
    
    # Calculate firing rate statistics
    avg_firing = sum(firing_rate_history) / total_steps if total_steps > 0 else 0
    peak_firing = max(firing_rate_history)
    
    print(f"Spike Statistics:")
    print(f"  Average spikes per step: {avg_spikes_per_step:.1f}")
    print(f"  Spike standard deviation: {spike_std:.1f}")
    print(f"  Peak spikes: {max_spikes}")
    print(f"  Minimum spikes: {min_spikes}")
    
    print(f"\nFiring Rate Statistics:")
    print(f"  Average firing neurons: {avg_firing:.0f}")
    print(f"  Peak firing neurons: {peak_firing}")
    print(f"  Firing efficiency: {avg_firing/total_steps:.2f} neurons per step")
    
    # Detect patterns
    print(f"\nPattern Analysis:")
    
    # Check for periodic firing patterns
    if len(spike_history) >= 4:
        # Simple pattern detection: look for increasing/decreasing trends
        increasing = all(spike_history[i] <= spike_history[i+1] for i in range(len(spike_history)-1))
        decreasing = all(spike_history[i] >= spike_history[i+1] for i in range(len(spike_history)-1))
        
        if increasing:
            print("  Pattern detected: Increasing spike activity over time")
        elif decreasing:
            print("  Pattern detected: Decreasing spike activity over time")
        else:
            print("  Pattern detected: Non-monotonic spike activity")
    
    # Calculate learning metrics if available
    if hasattr(brain, 'getDevelopmentalStage'):
        stage = brain.getDevelopmentalStage()
        print(f"Development stage: {stage}")
    
    # Save analysis results
    analysis_data = {
        "spike_history": spike_history,
        "firing_rate_history": firing_rate_history,
        "timestamps": time_stamps,
        "statistics": {
            "avg_spikes_per_step": avg_spikes_per_step,
            "max_spikes": max_spikes,
            "min_spikes": min_spikes,
            "spike_std": spike_std,
            "avg_firing": avg_firing,
            "peak_firing": peak_firing
        }
    }
    
    with open("analysis_results.json", "w") as f:
        json.dump(analysis_data, f, indent=2)
    
    print(f"\nAnalysis saved to analysis_results.json")
    
    return analysis_data

def config_benchmarking_example():
    """Example: Benchmark different configuration settings"""
    print("\n=== Configuration Benchmarking Example ===")
    
    # Define different configurations to benchmark
    configs = [
        {"name": "Minimal", "neurons": 500, "steps": 1000},
        {"name": "Small", "neurons": 1000, "steps": 2000},
        {"name": "Medium", "neurons": 2000, "steps": 5000},
        {"name": "Large", "neurons": 5000, "steps": 10000},
    ]
    
    benchmarks = []
    
    for config_def in configs:
        print(f"\nBenchmarking {config_def['name']} configuration...")
        print(f"  Neurons: {config_def['neurons']}, Steps: {config_def['steps']}")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", config_def['neurons'])
        config.set("simulation.timestep", 0.001)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Time the simulation
        start_time = time.time()
        
        for step in range(config_def['steps']):
            brain.step(step)
            
            # Simulate some work to make timing meaningful
            if step % 500 == 0:
                pass  # Brain computation
        
        end_time = time.time()
        
        # Calculate metrics
        duration = end_time - start_time
        steps_per_second = config_def['steps'] / duration
        neurons_per_second = config_def['neurons'] * steps_per_second
        spikes_per_second = brain.getTotalSpikeCount() / duration
        
        benchmark_result = {
            "config_name": config_def['name'],
            "neuron_count": config_def['neurons'],
            "steps": config_def['steps'],
            "duration_seconds": duration,
            "steps_per_second": steps_per_second,
            "neurons_per_second": neurons_per_second,
            "total_spikes": brain.getTotalSpikeCount(),
            "spikes_per_second": spikes_per_second,
            "final_firing_rate": brain.getAverageFiringRate()
        }
        
        benchmarks.append(benchmark_result)
        
        print(f"  Duration: {duration:.2f}s")
        print(f"  Speed: {steps_per_second:.0f} steps/s")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Final firing rate: {brain.getAverageFiringRate():.2f}Hz")
    
    # Save benchmarks
    with open("benchmark_results.json", "w") as f:
        json.dump(benchmarks, f, indent=2)
    
    print(f"\nBenchmarking completed. Results saved to benchmark_results.json")
    
    # Show summary
    print(f"\nBenchmark Summary:")
    print(f"  Configurations tested: {len(benchmarks)}")
    
    fastest = max(benchmarks, key=lambda x: x['steps_per_second'])
    most_spikes = max(benchmarks, key=lambda x: x['total_spikes'])
    
    print(f"  Fastest (steps/s): {fastest['config_name']} ({fastest['steps_per_second']:.0f} steps/s)")
    print(f"  Most spikes: {most_spikes['config_name']} ({most_spikes['total_spikes']} spikes)")
    
    return benchmarks

def main():
    """Run all advanced examples"""
    print("Advanced NLM Usage Examples")
    print("=" * 50)
    
    try:
        # Example 1: Advanced configuration
        config = advanced_config_example()
        
        # Example 2: Batch simulation
        batch_results = batch_simulation_example()
        
        # Example 3: Advanced analysis
        analysis_data = advanced_analysis_example()
        
        # Example 4: Configuration benchmarking
        benchmark_data = config_benchmarking_example()
        
        print("\n" + "=" * 50)
        print("All advanced examples completed successfully!")
        print("\nGenerated files:")
        print("  - advanced_config.json")
        print("  - batch_results.json")
        print("  - analysis_results.json")
        print("  - benchmark_results.json")
        
    except Exception as e:
        print(f"\nError running examples: {e}")
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())
