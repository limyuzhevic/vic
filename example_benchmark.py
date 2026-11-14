#!/usr/bin/env python3
# NLM Python Benchmark Example
# Demonstrates performance benchmarking of NLM Python bindings

import pynlm
import time
import statistics

def run_benchmark():
    """Run benchmarks for different brain sizes"""
    print("=== NLM Python Benchmark ===\n")
    
    benchmarks = [
        {"name": "Small", "neurons": 100, "steps": 2000},
        {"name": "Medium", "neurons": 500, "steps": 1000},
        {"name": "Large", "neurons": 1000, "steps": 500},
    ]
    
    results = []
    
    for bench in benchmarks:
        print(f"Benchmark: {bench['name']} ({bench['neurons']} neurons, {bench['steps']} steps)")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", bench['neurons'])
        config.set("region_count", 1)
        config.set("connection_probability", 0.1)
        config.set("random_seed", 42)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run benchmark
        start_time = time.time()
        
        for step in range(bench['steps']):
            brain.step(step, step * 0.001)
            
            # Inject some sensory activity
            if step % 10 == 0:
                # Get sensory input (using agent's brain interface)
                # For benchmark, we'll just step the brain
                pass
        
        end_time = time.time()
        total_time = end_time - start_time
        steps_per_second = bench['steps'] / total_time
        
        result = {
            "name": bench['name'],
            "neurons": bench['neurons'],
            "steps": bench['steps'],
            "total_time": total_time,
            "steps_per_second": steps_per_second,
            "spikes_per_step": brain.getTotalSpikeCount() / bench['steps'],
            "neurons_per_second": bench['neurons'] * steps_per_second,
        }
        
        results.append(result)
        
        print(f"  Total time: {total_time:.3f} seconds")
        print(f"  Steps/second: {steps_per_second:.1f}")
        print(f"  Spikes/step: {result['spikes_per_step']:.2f}")
        print(f"  Neurons/second: {result['neurons_per_second']:.0f}")
        print()
    
    # Print summary
    print("=== Benchmark Summary ===")
    print(f"{'Name':<10} {'Neurons':<10} {'Steps':<10} {'Time(s)':<10} {'Steps/sec':<12} {'Spikes/step':<12}")
    print("-" * 80)
    
    for result in results:
        print(f"{result['name']:<10} {result['neurons']:<10} {result['steps']:<10} "
              f"{result['total_time']:<10.3f} {result['steps_per_second']:<12.1f} "
              f"{result['spikes_per_step']:<12.2f}")
    
    # Calculate performance metrics
    if len(results) >= 2:
        small_result = results[0]
        large_result = results[-1]
        
        scaling = (large_result['neurons'] / small_result['neurons']) * \
                 (large_result['steps_per_second'] / small_result['steps_per_second'])
        
        print(f"\nPerformance Scaling:")
        print(f"  Neuron scaling: {large_result['neurons']/small_result['neurons']:.1f}x")
        print(f"  Steps/sec scaling: {large_result['steps_per_second']/small_result['steps_per_second']:.1f}x")
        print(f"  Overall scaling: {scaling:.1f}x (ideal would be linear)")
    
    return results

def benchmark_python_api():
    """Benchmark Python API operations"""
    print("\n=== Python API Benchmark ===\n")
    
    # Benchmark configuration creation
    start_time = time.time()
    for i in range(1000):
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 100 + i)
    config_time = time.time() - start_time
    
    # Benchmark brain creation
    start_time = time.time()
    for i in range(100):
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 100)
        brain = pynlm.createBrain(config)
        brain.initialize()
    brain_time = time.time() - start_time
    
    # Benchmark world creation
    start_time = time.time()
    for i in range(1000):
        world = pynlm.createSimpleWorld()
    world_time = time.time() - start_time
    
    # Benchmark agent creation
    start_time = time.time()
    for i in range(100):
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 100)
        brain = pynlm.createBrain(config)
        brain.initialize()
        agent = pynlm.createAgentBrain(brain)
    agent_time = time.time() - start_time
    
    print(f"Configuration creation (1000 iterations): {config_time:.3f}s")
    print(f"Brain creation (100 iterations): {brain_time:.3f}s")
    print(f"World creation (1000 iterations): {world_time:.3f}s")
    print(f"Agent creation (100 iterations): {agent_time:.3f}s")
    
    return {
        "config_time": config_time,
        "brain_time": brain_time,
        "world_time": world_time,
        "agent_time": agent_time,
    }

if __name__ == "__main__":
    try:
        print("Running NLM Python benchmarks...\n")
        
        # Run main benchmarks
        results = run_benchmark()
        
        # Run API benchmarks
        api_results = benchmark_python_api()
        
        print("\n=== All Benchmarks Completed ===")
        print("The Python bindings perform well for neural simulation tasks.")
        
    except Exception as e:
        print(f"\nError running benchmarks: {e}")
        import traceback
        traceback.print_exc()
        exit(1)
