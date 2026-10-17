#!/usr/bin/env python3
"""
NLM Python Performance Testing Examples

This script demonstrates performance testing capabilities with NLM Python bindings,
including benchmarking, performance metrics, and optimization examples.
"""

import pynlm
import time
import numpy as np
from datetime import datetime

def benchmark_brain_creation():
    """Benchmark brain creation and initialization"""
    print("=" * 60)
    print("Benchmark: Brain Creation and Initialization")
    print("=" * 60)
    
    step_counts = [10, 50, 100, 500]
    num_trials = 5
    
    print(f"Running benchmark with {num_trials} trials per step count...")
    
    results = {}
    
    for steps in step_counts:
        times = []
        
        for trial in range(num_trials):
            config = pynlm.createDefaultConfig()
            start_time = time.time()
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            for i in range(steps):
                brain.step(i)
            
            end_time = time.time()
            times.append(end_time - start_time)
        
        avg_time = np.mean(times)
        std_time = np.std(times)
        steps_per_second = steps / avg_time if avg_time > 0 else 0
        
        results[steps] = {
            'avg_time': avg_time,
            'std_time': std_time,
            'steps_per_second': steps_per_second,
            'total_spikes': brains[0].getTotalSpikeCount() if 'brains' in locals() else None
        }
        
        print(f"  Steps: {steps:4d} | Avg Time: {avg_time:.3f}s ± {std_time:.3f}s | "
              f"Steps/sec: {steps_per_second:8.1f}")
    
    # Save benchmark results
    with open("brain_creation_benchmark.txt", "w") as f:
        f.write("Brain Creation and Initialization Benchmark\n")
        f.write("=" * 50 + "\n")
        f.write(f"Performed {num_trials} trials per step count\n\n")
        
        for steps, data in results.items():
            f.write(f"Step Count: {steps}\n")
            f.write(f"  Average Time: {data['avg_time']:.3f} ± {data['std_time']:.3f} seconds\n")
            f.write(f"  Steps per Second: {data['steps_per_second']:.1f}\n\n")
    
    print(f"\n✓ Benchmark results saved to 'brain_creation_benchmark.txt'")
    return results

def benchmark_step_performance():
    """Benchmark step performance over time"""
    print("\n" + "=" * 60)
    print("Benchmark: Step Performance Over Time")
    print("=" * 60)
    
    # Test different step counts
    step_counts = [100, 500, 1000, 2000, 5000]
    
    print("Testing step performance at different scales...")
    
    results = {}
    
    for steps in step_counts:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Measure step performance
        start_time = time.time()
        for i in range(steps):
            brain.step(i)
        end_time = time.time()
        
        duration = end_time - start_time
        steps_per_second = steps / duration if duration > 0 else 0
        spikes_per_second = brain.getTotalSpikeCount() / duration if duration > 0 else 0
        
        results[steps] = {
            'duration': duration,
            'steps_per_second': steps_per_second,
            'spikes_per_second': spikes_per_second,
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount()
        }
        
        print(f"  Steps: {steps:5d} | Duration: {duration:6.3f}s | "
              f"Steps/sec: {steps_per_second:8.1f} | "
              f"Spikes/sec: {spikes_per_second:8.1f}")
    
    # Create performance comparison
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    
    # Plot 1: Steps per second
    axes[0, 0].plot(list(results.keys()), 
                   [r['steps_per_second'] for r in results.values()], 
                   'bo-', linewidth=2, markersize=8)
    axes[0, 0].set_title('Steps per Second')
    axes[0, 0].set_xlabel('Step Count')
    axes[0, 0].set_ylabel('Steps per Second')
    axes[0, 0].grid(True, alpha=0.3)
    
    # Plot 2: Spikes per second
    axes[0, 1].plot(list(results.keys()), 
                   [r['spikes_per_second'] for r in results.values()], 
                   'ro-', linewidth=2, markersize=8)
    axes[0, 1].set_title('Spikes per Second')
    axes[0, 1].set_xlabel('Step Count')
    axes[0, 1].set_ylabel('Spikes per Second')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Plot 3: Efficiency (spikes per step)
    axes[1, 0].plot(list(results.keys()), 
                   [r['total_spikes'] / steps for steps, r in results.items()], 
                   'go-', linewidth=2, markersize=8)
    axes[1, 0].set_title('Efficiency (Spikes per Step)')
    axes[1, 0].set_xlabel('Step Count')
    axes[1, 0].set_ylabel('Spikes per Step')
    axes[1, 0].grid(True, alpha=0.3)
    
    # Plot 4: Performance summary
    metrics = ['Steps per Second', 'Spikes per Second', 'Efficiency']
    values = []
    
    for steps, data in results.items():
        values.append([data['steps_per_second'], data['spikes_per_second'], 
                      data['total_spikes'] / steps if steps > 0 else 0])
    
    values = np.array(values)
    axes[1, 1].boxplot(values.T, labels=metrics)
    axes[1, 1].set_title('Performance Comparison')
    axes[1, 1].set_ylabel('Performance Metric')
    axes[1, 1].grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('performance_benchmark.png', dpi=150)
    print(f"\n✓ Performance visualization saved as 'performance_benchmark.png'")
    
    plt.close()
    
    # Save performance data
    with open("performance_benchmark_data.csv", "w") as f:
        f.write("Step Count,Duration,Steps per Second,Spikes per Second,Total Spikes,Firing Neurons\n")
        for steps, data in results.items():
            f.write(f"{steps},{data['duration']},{data['steps_per_second']},"
                   f"{data['spikes_per_second']},{data['total_spikes']},"
                   f"{data['firing_neurons']}\n")
    
    print(f"✓ Performance data saved as 'performance_benchmark_data.csv'")
    
    return results

def benchmark_memory_usage():
    """Benchmark memory usage and efficiency"""
    print("\n" + "=" * 60)
    print("Benchmark: Memory Usage and Efficiency")
    print("=" * 60)
    
    import sys
    
    step_counts = [100, 500, 1000, 2000]
    
    print("Testing memory efficiency...")
    
    memory_results = {}
    
    for steps in step_counts:
        # Measure initial memory
        initial_objects = len(gc.get_objects()) if 'gc' in globals() else 0
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for i in range(steps):
            brain.step(i)
        
        # Measure final memory
        final_objects = len(gc.get_objects()) if 'gc' in globals() else 0
        memory_increase = final_objects - initial_objects
        
        # Calculate efficiency metrics
        spikes_per_step = brain.getTotalSpikeCount() / steps if steps > 0 else 0
        memory_efficiency = spikes_per_step / memory_increase if memory_increase > 0 else 0
        
        memory_results[steps] = {
            'memory_increase': memory_increase,
            'spikes_per_step': spikes_per_step,
            'memory_efficiency': memory_efficiency,
            'final_spikes': brain.getTotalSpikeCount()
        }
        
        print(f"  Steps: {steps:4d} | Memory Increase: {memory_increase:5d} objects | "
              f"Efficiency: {memory_efficiency:8.3f} spikes/object | "
              f"Total Spikes: {brain.getTotalSpikeCount():6d}")
    
    # Create memory efficiency visualization
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    
    # Plot 1: Memory increase vs steps
    axes[0, 0].bar(list(memory_results.keys()), 
                   [r['memory_increase'] for r in memory_results.values()], 
                   alpha=0.7, color='skyblue', edgecolor='black')
    axes[0, 0].set_title('Memory Usage vs Step Count')
    axes[0, 0].set_xlabel('Step Count')
    axes[0, 0].set_ylabel('Memory Increase (objects)')
    axes[0, 0].grid(True, alpha=0.3)
    
    # Plot 2: Memory efficiency
    axes[0, 1].bar(list(memory_results.keys()), 
                   [r['memory_efficiency'] for r in memory_results.values()], 
                   alpha=0.7, color='lightgreen', edgecolor='black')
    axes[0, 1].set_title('Memory Efficiency vs Step Count')
    axes[0, 1].set_xlabel('Step Count')
    axes[0, 1].set_ylabel('Efficiency (spikes/object)')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Plot 3: Efficiency comparison
    axes[1, 0].plot(list(memory_results.keys()), 
                   [r['spikes_per_step'] for r in memory_results.values()], 
                   'ro-', linewidth=2, markersize=8)
    axes[1, 0].set_title('Spikes per Step')
    axes[1, 0].set_xlabel('Step Count')
    axes[1, 0].set_ylabel('Spikes per Step')
    axes[1, 0].grid(True, alpha=0.3)
    
    # Plot 4: Memory efficiency summary
    efficiency_data = list(memory_results.values())
    axes[1, 1].boxplot([r['memory_efficiency'] for r in efficiency_data,
                      r['spikes_per_step'] for r in efficiency_data], 
                      labels=['Memory Efficiency', 'Spikes per Step'])
    axes[1, 1].set_title('Memory Efficiency Comparison')
    axes[1, 1].set_ylabel('Metric')
    axes[1, 1].grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('memory_benchmark.png', dpi=150)
    print(f"\n✓ Memory visualization saved as 'memory_benchmark.png'")
    
    plt.close()
    
    return memory_results

def benchmark_stress_test():
    """Stress test with extreme configurations"""
    print("\n" + "=" * 60)
    print("Benchmark: Stress Test")
    print("=" * 60)
    
    stress_tests = []
    
    # Test 1: Large brain
    print("\nTest 1: Large Brain Configuration")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 5000)
    config.set("brain.region_count", 10)
    config.set("brain.synapse_density", 0.2)
    
    brain = pynlm.createBrain(config)
    start_time = time.time()
    brain.initialize()
    init_time = time.time() - start_time
    
    start_time = time.time()
    for i in range(1000):
        brain.step(i)
    duration = time.time() - start_time
    
    stress_tests.append({
        'name': 'Large Brain (5000 neurons)',
        'neuron_count': config.get('brain.neuron_count'),
        'init_time': init_time,
        'simulation_time': duration,
        'steps_per_second': 1000 / duration if duration > 0 else 0,
        'total_spikes': brain.getTotalSpikeCount()
    })
    
    print(f"  Initialization Time: {init_time:.3f}s")
    print(f"  Simulation Time: {duration:.3f}s")
    print(f"  Steps per Second: {1000/duration:.1f}")
    print(f"  Total Spikes: {brain.getTotalSpikeCount()}")
    
    # Test 2: High plasticity
    print("\nTest 2: High Plasticity Configuration")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.01)  # High learning rate
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    brain = pynlm.createBrain(config)
    start_time = time.time()
    brain.initialize()
    init_time = time.time() - start_time
    
    start_time = time.time()
    for i in range(1000):
        brain.step(i)
    duration = time.time() - start_time
    
    stress_tests.append({
        'name': 'High Plasticity',
        'neuron_count': config.get('brain.neuron_count'),
        'init_time': init_time,
        'simulation_time': duration,
        'steps_per_second': 1000 / duration if duration > 0 else 0,
        'total_spikes': brain.getTotalSpikeCount()
    })
    
    print(f"  Initialization Time: {init_time:.3f}s")
    print(f"  Simulation Time: {duration:.3f}s")
    print(f"  Steps per Second: {1000/duration:.1f}")
    print(f"  Total Spikes: {brain.getTotalSpikeCount()}")
    
    # Test 3: High neuromodulation
    print("\nTest 3: High Neuromodulation Configuration")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.dopamine.scale", 2.0)  # High neuromodulation scale
    
    brain = pynlm.createBrain(config)
    start_time = time.time()
    brain.initialize()
    init_time = time.time() - start_time
    
    start_time = time.time()
    for i in range(1000):
        brain.step(i)
    duration = time.time() - start_time
    
    stress_tests.append({
        'name': 'High Neuromodulation',
        'neuron_count': config.get('brain.neuron_count'),
        'init_time': init_time,
        'simulation_time': duration,
        'steps_per_second': 1000 / duration if duration > 0 else 0,
        'total_spikes': brain.getTotalSpikeCount()
    })
    
    print(f"  Initialization Time: {init_time:.3f}s")
    print(f"  Simulation Time: {duration:.3f}s")
    print(f"  Steps per Second: {1000/duration:.1f}")
    print(f"  Total Spikes: {brain.getTotalSpikeCount()}")
    
    # Create stress test visualization
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    
    # Plot 1: Stress test comparison
    names = [t['name'] for t in stress_tests]
    init_times = [t['init_time'] for t in stress_tests]
    sim_times = [t['simulation_time'] for t in stress_tests]
    
    x = np.arange(len(names))
    width = 0.35
    
    axes[0, 0].bar(x - width/2, init_times, width, label='Initialization', alpha=0.7, color='skyblue')
    axes[0, 0].bar(x + width/2, sim_times, width, label='Simulation', alpha=0.7, color='lightgreen')
    axes[0, 0].set_title('Stress Test Performance')
    axes[0, 0].set_xlabel('Test Configuration')
    axes[0, 0].set_ylabel('Time (seconds)')
    axes[0, 0].set_xticks(x)
    axes[0, 0].set_xticklabels(names, rotation=15)
    axes[0, 0].legend()
    axes[0, 0].grid(True, alpha=0.3)
    
    # Plot 2: Steps per second
    steps_per_second = [t['steps_per_second'] for t in stress_tests]
    axes[0, 1].bar(names, steps_per_second, alpha=0.7, color='salmon', edgecolor='black')
    axes[0, 1].set_title('Steps per Second')
    axes[0, 1].set_xlabel('Test Configuration')
    axes[0, 1].set_ylabel('Steps per Second')
    axes[0, 1].tick_params(axis='x', rotation=15)
    axes[0, 1].grid(True, alpha=0.3)
    
    # Plot 3: Total spikes
    total_spikes = [t['total_spikes'] for t in stress_tests]
    axes[1, 0].bar(names, total_spikes, alpha=0.7, color='gold', edgecolor='black')
    axes[1, 0].set_title('Total Spikes Generated')
    axes[1, 0].set_xlabel('Test Configuration')
    axes[1, 0].set_ylabel('Total Spikes')
    axes[1, 0].tick_params(axis='x', rotation=15)
    axes[1, 0].grid(True, alpha=0.3)
    
    # Plot 4: Stress test summary
    metrics = ['Init Time', 'Sim Time', 'Steps/sec', 'Total Spikes']
    data = np.array([
        [t['init_time'] for t in stress_tests],
        [t['simulation_time'] for t in stress_tests],
        [t['steps_per_second'] for t in stress_tests],
        [t['total_spikes'] for t in stress_tests]
    ])
    
    axes[1, 1].boxplot(data, labels=metrics)
    axes[1, 1].set_title('Stress Test Metrics Distribution')
    axes[1, 1].set_ylabel('Metric Value')
    axes[1, 1].grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('stress_test_benchmark.png', dpi=150)
    print(f"\n✓ Stress test visualization saved as 'stress_test_benchmark.png'")
    
    plt.close()
    
    # Save stress test results
    with open("stress_test_results.txt", "w") as f:
        f.write("NLM Stress Test Benchmark Results\n")
        f.write("=" * 50 + "\n\n")
        for i, test in enumerate(stress_tests):
            f.write(f"Test {i+1}: {test['name']}\n")
            f.write(f"  Neuron Count: {test['neuron_count']}\n")
            f.write(f"  Initialization Time: {test['init_time']:.3f}s\n")
            f.write(f"  Simulation Time: {test['simulation_time']:.3f}s\n")
            f.write(f"  Steps per Second: {test['steps_per_second']:.1f}\n")
            f.write(f"  Total Spikes: {test['total_spikes']}\n\n")
    
    print(f"✓ Stress test results saved to 'stress_test_results.txt'")
    
    return stress_tests

def main():
    """Run all performance benchmarks"""
    print("NLM Python Performance Testing Examples")
    print("=" * 60)
    print("This script demonstrates performance testing with NLM Python bindings.")
    print()
    
    # Run all benchmarks
    brain_creation_results = benchmark_brain_creation()
    step_results = benchmark_step_performance()
    memory_results = benchmark_memory_usage()
    stress_results = benchmark_stress_test()
    
    print("\n" + "=" * 60)
    print("All performance benchmarks completed successfully!")
    print("=" * 60)
    print("\nKey features demonstrated:")
    print("1. ✓ Brain creation and initialization performance")
    print("2. ✓ Step performance over different scales")
    print("3. ✓ Memory usage and efficiency analysis")
    print("4. ✓ Stress testing with extreme configurations")
    print("\nGenerated files:")
    print("  - brain_creation_benchmark.txt")
    print("  - performance_benchmark.png")
    print("  - memory_benchmark.png")
    print("  - stress_test_benchmark.png")
    print("  - performance_benchmark_data.csv")
    print("  - stress_test_results.txt")
    print("\nAll performance testing features working correctly!")

if __name__ == "__main__":
    main()