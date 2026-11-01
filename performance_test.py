#!/usr/bin/env python3
"""
Performance test and optimization for NLM
"""

import pynlm
import time
import json
from pynlm.enhanced import ConfigPresets

def run_performance_test(config, steps=1000, description=""):
    """Run a performance test with given configuration"""
    print(f"\nRunning performance test: {description}")
    print(f"  Neurons: {config.get('brain.neuron_count', 'N/A')}")
    print(f"  Steps: {steps}")
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Collect baseline metrics
    start_time = time.time()
    start_spikes = brain.getTotalSpikeCount()
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
    
    end_time = time.time()
    end_spikes = brain.getTotalSpikeCount()
    
    # Calculate metrics
    simulation_time = end_time - start_time
    spikes_generated = end_spikes - start_spikes
    spikes_per_second = spikes_generated / simulation_time if simulation_time > 0 else 0
    firing_rate_per_neuron = spikes_per_second / config.get("brain.neuron_count", 1) if config.get("brain.neuron_count", 1) > 0 else 0
    
    print(f"  Simulation time: {simulation_time:.3f}s")
    print(f"  Spikes generated: {spikes_generated}")
    print(f"  Spikes per second: {spikes_per_second:.1f}")
    print(f"  Firing rate per neuron: {firing_rate_per_neuron:.3f} Hz")
    print(f"  Firing rate per step: {spikes_generated / steps:.1f} spikes/step")
    
    return {
        'simulation_time': simulation_time,
        'spikes_generated': spikes_generated,
        'spikes_per_second': spikes_per_second,
        'firing_rate_per_neuron': firing_rate_per_neuron,
        'firing_rate_per_step': spikes_generated / steps,
        'description': description,
    }

def optimize_for_low_activity(config, target_spikes_per_step=10):
    """Optimize configuration for lower spike activity"""
    print("\nOptimizing for lower spike activity...")
    
    # Start with current config
    optimized = config.copy()
    
    # Adjust neuron parameters for lower activity
    if optimized.get("brain.v_thresh", -50.0) > -60.0:
        optimized["brain.v_thresh"] = -60.0
        print(f"  Lowered threshold to {optimized['brain.v_thresh']} mV")
    
    # Adjust synaptic weights for weaker connections
    if optimized.get("brain.initial_weight_mean", 0.5) > 0.3:
        optimized["brain.initial_weight_mean"] = 0.3
        optimized["brain.initial_weight_std"] = 0.05
        print(f"  Reduced initial weights to mean={optimized['brain.initial_weight_mean']}, std={optimized['brain.initial_weight_std']}")
    
    # Reduce connection probability
    if optimized.get("brain.connection_probability", 0.1) > 0.05:
        optimized["brain.connection_probability"] = 0.05
        print(f"  Reduced connection probability to {optimized['brain.connection_probability']}")
    
    # Increase leak conductance
    if optimized.get("brain.tau_mem", 20.0) > 15.0:
        optimized["brain.tau_mem"] = 15.0
        print(f"  Increased membrane time constant to {optimized['brain.tau_mem']} ms")
    
    return optimized

def run_configuration_comparison():
    """Compare different configurations for performance"""
    print("=" * 60)
    print("NLM PERFORMANCE OPTIMIZATION TEST")
    print("=" * 60)
    
    # Test configurations
    configs = [
        {"brain.neuron_count": 500, "description": "Small network"},
        {"brain.neuron_count": 1000, "description": "Medium network"},
        {"brain.neuron_count": 2000, "description": "Large network"},
    ]
    
    # Add connectivity variations
    for config in configs[:]:
        modified_config = config.copy()
        modified_config["brain.connection_probability"] = 0.02
        modified_config["description"] = f"{config['description']}, sparse"
        configs.append(modified_config)
    
    results = []
    
    for config_dict in configs:
        # Create config
        config = pynlm.createDefaultConfig()
        for key, value in config_dict.items():
            if key != 'description':
                config.set(key, value)
        
        # Run test
        result = run_performance_test(config, steps=500, description=config_dict['description'])
        results.append(result)
    
    # Print summary
    print("\n" + "=" * 60)
    print("PERFORMANCE SUMMARY")
    print("=" * 60)
    
    print(f"{'Description':<25} {'Spikes/Step':<12} {'Firing Rate/Step':<18} {'Spikes/sec':<12} {'Time (s)':<10}")
    print("-" * 80)
    
    for result in results:
        print(f"{result['description']:<25} {result['firing_rate_per_step']:<12.1f} "
              f"{result['firing_rate_per_neuron']*1000:<18.1f} {result['spikes_per_second']:<12.1f} "
              f"{result['simulation_time']:<10.3f}")
    
    # Find best configuration for low activity
    best_result = min(results, key=lambda x: x['firing_rate_per_step'])
    print(f"\nBest configuration for low activity: {best_result['description']}")
    print(f"  Spikes per step: {best_result['firing_rate_per_step']:.1f}")
    
    return results

def create_benchmark_config():
    """Create optimized benchmark configurations"""
    print("\n" + "=" * 60)
    print("BENCHMARK CONFIGURATION CREATION")
    print("=" * 60)
    
    # Create different benchmark configurations
    benchmarks = {
        "quick_test": {
            "brain.neuron_count": 100,
            "brain.connection_probability": 0.02,
            "description": "Quick test - minimal load"
        },
        "standard_test": {
            "brain.neuron_count": 500,
            "brain.connection_probability": 0.05,
            "description": "Standard test - moderate load"
        },
        "stress_test": {
            "brain.neuron_count": 1000,
            "brain.connection_probability": 0.08,
            "description": "Stress test - high load"
        },
    }
    
    # Create enhanced configurations for testing
    enhanced_benchmarks = {}
    
    for name, config_dict in benchmarks.items():
        config = pynlm.createDefaultConfig()
        
        # Set base parameters
        config.set("brain.neuron_count", config_dict["brain.neuron_count"])
        config.set("brain.connection_probability", config_dict["brain.connection_probability"])
        
        # Add advanced features
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("neuromod.dopamine.scale", 0.5)
        config.set("neuromod.curiosity.enable", False)
        
        enhanced_benchmarks[name] = {
            "config": config,
            "description": config_dict["description"]
        }
    
    return enhanced_benchmarks

def run_advanced_tests():
    """Run advanced tests with different configurations"""
    print("\n" + "=" * 60)
    print("ADVANCED PERFORMANCE TESTS")
    print("=" * 60)
    
    benchmarks = create_benchmark_config()
    
    for name, benchmark in benchmarks.items():
        config = benchmark["config"]
        description = benchmark["description"]
        
        print(f"\n--- {name.upper()} ---")
        print(f"Description: {description}")
        
        # Run performance test
        result = run_performance_test(config, steps=1000, description=name)
        
        # Print advanced metrics
        print(f"  Neurons: {config.get('brain.neuron_count', 'N/A')}")
        print(f"  Connections: {config.get('brain.connection_probability', 'N/A')}")
        print(f"  STDP enabled: {config.get('plasticity.stdp.enable', False)}")
        print(f"  Hebbian enabled: {config.get('plasticity.hebbian.enable', False)}")
        
        # Save results
        with open(f"benchmark_{name}.json", "w") as f:
            json.dump({
                'name': name,
                'description': description,
                'result': result,
                'config': {
                    'brain.neuron_count': config.get('brain.neuron_count', 0),
                    'brain.connection_probability': config.get('brain.connection_probability', 0),
                    'plasticity.stdp.enable': config.get('plasticity.stdp.enable', False),
                    'plasticity.hebbian.enable': config.get('plasticity.hebbian.enable', False),
                }
            }, f, indent=2)
        
        print(f"  Results saved to benchmark_{name}.json")

def main():
    """Main entry point"""
    print("NLM Performance Optimization and Testing")
    print("=" * 60)
    
    # Run configuration comparison
    comparison_results = run_configuration_comparison()
    
    # Run advanced tests
    run_advanced_tests()
    
    print("\n" + "=" * 60)
    print("OPTIMIZATION RECOMMENDATIONS")
    print("=" * 60)
    
    # Find optimal configuration
    best_for_activity = min(comparison_results, key=lambda x: x['firing_rate_per_step'])
    
    print(f"For low spike activity (< 10 spikes/step):")
    print(f"  Use: {best_for_activity['description']}")
    print(f"  Target: ~{best_for_activity['firing_rate_per_step']:.1f} spikes per step")
    
    best_for_throughput = max(comparison_results, key=lambda x: x['spikes_per_second'])
    
    print(f"\nFor high throughput:")
    print(f"  Use: {best_for_throughput['description']}")
    print(f"  Throughput: {best_for_throughput['spikes_per_second']:.1f} spikes per second")
    
    print(f"\nBenchmark results saved to benchmark_*.json files")
    print("\nOptimization complete!")

if __name__ == "__main__":
    main()
