"""
NLM Performance Testing Example
===============================

This example demonstrates performance testing and benchmarking
using the enhanced NLM Python bindings.
"""

import pynlm as nlm
import time
import statistics
from typing import List, Dict, Any

def benchmark_simulation(neuron_count: int, num_steps: int) -> Dict[str, Any]:
    """Benchmark simulation performance for given parameters"""
    print(f"Benchmarking: {neuron_count} neurons, {num_steps} steps")
    
    # Create configuration
    config = nlm.create_test_config(neuron_count=neuron_count)
    
    # Measure setup time
    start_time = time.time()
    brain = nlm.create_brain(config)
    brain.initialize()
    setup_time = time.time() - start_time
    
    # Measure simulation time
    start_time = time.time()
    with nlm.BrainSimulator(brain) as simulator:
        firing_rates = simulator.run_steps(num_steps)
    sim_time = time.time() - start_time
    
    # Calculate statistics
    avg_firing_rate = statistics.mean(firing_rates) if firing_rates else 0.0
    max_firing_rate = max(firing_rates) if firing_rates else 0.0
    min_firing_rate = min(firing_rates) if firing_rates else 0.0
    
    result = {
        'neuron_count': neuron_count,
        'num_steps': num_steps,
        'setup_time': setup_time,
        'simulation_time': sim_time,
        'avg_firing_rate': avg_firing_rate,
        'max_firing_rate': max_firing_rate,
        'min_firing_rate': min_firing_rate,
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_rate_std': statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0.0
    }
    
    print(f"   Setup time: {setup_time:.3f}s")
    print(f"   Simulation time: {sim_time:.3f}s")
    print(f"   Average firing rate: {avg_firing_rate:.3f}")
    print(f"   Total spikes: {result['total_spikes']}")
    
    return result

def parameter_sweep_example():
    """Run parameter sweep to test different configurations"""
    print("\n=== Parameter Sweep Example ===\n")
    
    neuron_counts = [50, 100, 200, 500]
    results = []
    
    for neuron_count in neuron_counts:
        result = benchmark_simulation(neuron_count, 100)
        results.append(result)
        print()
    
    # Analyze results
    print("=== Parameter Sweep Results ===")
    print(f"{'Neurons':<8} {'Steps':<6} {'Setup (s)':<10} {'Sim (s)':<10} {'Avg Rate':<12} {'Total Spikes':<12}")
    print("-" * 70)
    
    for result in results:
        print(f"{result['neuron_count']:<8} {result['num_steps']:<6} "
              f"{result['setup_time']:<10.3f} {result['simulation_time']:<10.3f} "
              f"{result['avg_firing_rate']:<12.3f} {result['total_spikes']:<12}")
    
    return results

def condition_based_simulation_example():
    """Example using condition-based simulation"""
    print("\n=== Condition-Based Simulation Example ===\n")
    
    config = nlm.create_test_config(neuron_count=100)
    brain = nlm.create_brain(config)
    brain.initialize()
    
    conditions = [
        ("High activity", lambda b: b.getAverageFiringRate() > 10.0),
        ("Low activity", lambda b: b.getAverageFiringRate() < 1.0),
        ("Many spikes", lambda b: b.getTotalSpikeCount() > 100),
        ("Few spikes", lambda b: b.getTotalSpikeCount() < 10),
    ]
    
    for condition_name, condition in conditions:
        print(f"Running until {condition_name}...")
        brain2 = nlm.create_brain(config)
        brain2.initialize()
        
        with nlm.BrainSimulator(brain2) as sim:
            steps_taken = sim.run_until(condition, max_steps=2000)
            final_brain = sim.get_brain()
            
            print(f"   Condition: {condition_name}")
            print(f"   Steps to meet condition: {steps_taken}")
            print(f"   Final firing rate: {final_brain.getAverageFiringRate():.3f}")
            print(f"   Final spike count: {final_brain.getTotalSpikeCount()}")
            print()

def stress_test_example():
    """Stress test with large simulations"""
    print("\n=== Stress Test Example ===\n")
    
    configs = [
        ("Small", 50, 1000),
        ("Medium", 200, 500),
        ("Large", 500, 200),
    ]
    
    for name, neuron_count, steps in configs:
        print(f"Running {name} stress test...")
        result = benchmark_simulation(neuron_count, steps)
        
        # Calculate performance metrics
        sim_time_per_step = result['simulation_time'] / steps
        spikes_per_second = result['total_spikes'] / result['simulation_time']
        
        print(f"   Performance: {sim_time_per_step*1000:.2f}ms/step")
        print(f"   Spiking rate: {spikes_per_second:.1f} spikes/second")
        print()

def batch_simulation_example():
    """Example running multiple simulations in batch"""
    print("\n=== Batch Simulation Example ===\n")
    
    # Create multiple brains with different configurations
    configs = []
    for i in range(5):
        config = (nlm.Config()
                 .set_int("neuron_count", 100 + i * 50)
                 .set_float("connection_probability", 0.1 + i * 0.01))
        configs.append(config)
    
    # Run all simulations
    results = []
    for i, config in enumerate(configs):
        print(f"Running simulation {i+1}/5...")
        brain = nlm.create_brain(config)
        brain.initialize()
        
        with nlm.BrainSimulator(brain) as sim:
            firing_rates = sim.run_steps(50)
            
            result = {
                'simulation': i + 1,
                'neuron_count': config.get_int("neuron_count"),
                'connection_prob': config.get_float("connection_probability"),
                'avg_firing_rate': sum(firing_rates) / len(firing_rates),
                'total_spikes': brain.getTotalSpikeCount()
            }
            results.append(result)
    
    # Analyze batch results
    print("\n=== Batch Simulation Results ===")
    print(f"{'Sim':<5} {'Neurons':<8} {'Conn Prob':<10} {'Avg Rate':<12} {'Total Spikes':<12}")
    print("-" * 55)
    
    for result in results:
        print(f"{result['simulation']:<5} {result['neuron_count']:<8} "
              f"{result['connection_prob']:<10.3f} {result['avg_firing_rate']:<12.3f} "
              f"{result['total_spikes']:<12}")
    
    # Calculate batch statistics
    avg_firing = sum(r['avg_firing_rate'] for r in results) / len(results)
    total_spikes = sum(r['total_spikes'] for r in results)
    
    print(f"\nBatch Statistics:")
    print(f"   Average firing rate: {avg_firing:.3f}")
    print(f"   Total spikes across all: {total_spikes}")
    
    return results

def memory_usage_example():
    """Example monitoring memory usage (simplified)"""
    print("\n=== Memory Usage Example ===\n")
    
    config = nlm.create_test_config(neuron_count=300)
    brain = nlm.create_brain(config)
    brain.initialize()
    
    print("Monitoring memory usage during simulation...")
    
    with nlm.BrainSimulator(brain) as sim:
        for step in range(100):
            sim.step(step)
            
            if step % 20 == 0:
                print(f"   Step {step}: "
                      f"Neurons={brain.getTotalNeuronCount()}, "
                      f"Synapses={brain.getTotalSynapseCount()}, "
                      f"Active={brain.getActiveNeuronCount()}, "
                      f"Spikes={brain.getTotalSpikeCount()}")
    
    print(f"\nFinal statistics:")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   Active neurons: {brain.getActiveNeuronCount()}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.3f}")

def main():
    """Run all performance examples"""
    print("NLM Enhanced Python Bindings - Performance Testing")
    print("=" * 60)
    
    try:
        # Run parameter sweep
        parameter_sweep_example()
        
        # Run condition-based simulation
        condition_based_simulation_example()
        
        # Run stress test
        stress_test_example()
        
        # Run batch simulation
        batch_simulation_example()
        
        # Run memory usage example
        memory_usage_example()
        
        print("\n\n" + "=" * 60)
        print("All performance examples completed successfully!")
        print("=" * 60)
        
    except Exception as e:
        print(f"\n❌ Error running performance examples: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()
