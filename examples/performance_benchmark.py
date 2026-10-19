#!/usr/bin/env python3
"""
Performance Benchmarking Examples
===============================
This example demonstrates NLM performance benchmarking and analysis.
It shows how to measure and optimize brain simulation performance.

Key Features:
- Simulation step timing and throughput measurement
- Memory usage and efficiency analysis
- Scalability testing with different configurations
- Performance comparison and optimization
- Benchmarking against standard metrics
"""

import sys
import os
import time
import psutil
import gc
from collections import defaultdict

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def print_perf_header(title):
    """Print a formatted performance header"""
    print(f"\n{'='*70}")
    print(f"{title}")
    print(f"{'='*70}")

def get_process_memory():
    """Get current process memory usage in MB"""
    process = psutil.Process(os.getpid())
    return process.memory_info().rss / 1024 / 1024

def benchmark_step_timing(brain, steps, warmup=10):
    """Benchmark step timing with warmup"""
    print(f"\n   Benchmarking step timing ({steps} steps)...")
    
    # Warmup
    for i in range(warmup):
        brain.step(i)
    
    # Benchmark
    times = []
    for step in range(steps):
        start = time.perf_counter()
        brain.step(step)
        end = time.perf_counter()
        times.append(end - start)
    
    # Calculate statistics
    avg_time = sum(times) / len(times)
    min_time = min(times)
    max_time = max(times)
    std_time = (sum((t - avg_time) ** 2 for t in times) / len(times)) ** 0.5
    
    # Calculate performance metrics
    steps_per_sec = steps / sum(times) if sum(times) > 0 else 0
    time_per_step_ms = avg_time * 1000
    
    print(f"     Performance metrics:")
    print(f"       Average step time: {avg_time*1000:.2f} ms")
    print(f"       Min step time: {min_time*1000:.2f} ms")
    print(f"       Max step time: {max_time*1000:.2f} ms")
    print(f"       Standard deviation: {std_time*1000:.2f} ms")
    print(f"       Steps per second: {steps_per_sec:.1f}")
    print(f"       Time per step (ms): {time_per_step_ms:.2f}")
    
    return {
        'avg_time': avg_time,
        'min_time': min_time,
        'max_time': max_time,
        'std_time': std_time,
        'steps_per_sec': steps_per_sec,
        'time_per_step_ms': time_per_step_ms,
        'times': times
    }

def benchmark_memory_usage(brain, duration, operations=10):
    """Benchmark memory usage during simulation"""
    print(f"\n   Benchmarking memory usage ({duration} steps, {operations} iterations)...")
    
    # Force garbage collection
    gc.collect()
    initial_memory = get_process_memory()
    
    # Run simulation
    all_times = []
    memory_samples = []
    
    for iteration in range(operations):
        # Measure memory before iteration
        gc.collect()
        memory_before = get_process_memory()
        
        # Run simulation
        times = []
        for step in range(duration):
            start = time.perf_counter()
            brain.step(step)
            end = time.perf_counter()
            times.append(end - start)
            
            # Sample memory usage
            if step % 10 == 0:
                memory_samples.append(get_process_memory())
        
        all_times.extend(times)
        
        # Measure memory after iteration
        gc.collect()
        memory_after = get_process_memory()
        memory_delta = memory_after - memory_before
        
        if iteration % 2 == 0:  # Print every other iteration
            print(f"       Iteration {iteration + 1}: Memory change = {memory_delta:+.1f} MB")
    
    # Calculate overall memory statistics
    avg_memory = sum(memory_samples) / len(memory_samples) if memory_samples else initial_memory
    memory_growth = avg_memory - initial_memory
    
    # Calculate timing statistics
    avg_time = sum(all_times) / len(all_times)
    steps_per_sec = len(all_times) / sum(all_times) if sum(all_times) > 0 else 0
    
    print(f"     Memory usage statistics:")
    print(f"       Initial memory: {initial_memory:.1f} MB")
    print(f"       Average memory: {avg_memory:.1f} MB")
    print(f"       Memory growth: {memory_growth:+.1f} MB")
    print(f"       Memory efficiency: {steps_per_sec/len(memory_samples):.1f} steps/MB" if memory_samples else "N/A")
    print(f"       Average step time: {avg_time*1000:.2f} ms")
    print(f"       Throughput: {steps_per_sec:.1f} steps/second")
    
    return {
        'initial_memory': initial_memory,
        'avg_memory': avg_memory,
        'memory_growth': memory_growth,
        'steps_per_sec': steps_per_sec,
        'memory_samples': memory_samples,
        'total_time': sum(all_times)
    }

def benchmark_scalability(brain_sizes, duration_per_size):
    """Benchmark scalability with different brain sizes"""
    print(f"\n   Benchmarking scalability ({brain_sizes} sizes, {duration_per_size} steps each)...")
    
    scalability_results = {}
    
    for size in brain_sizes:
        print(f"\n     Testing brain size: {size} neurons...")
        
        # Create brain with specific size
        config = pynlm.createDefaultConfig()
        config.set("brain.neuronCount", size)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Measure timing
        times = []
        for step in range(duration_per_size):
            start = time.perf_counter()
            brain.step(step)
            end = time.perf_counter()
            times.append(end - start)
        
        # Calculate metrics
        avg_time = sum(times) / len(times)
        steps_per_sec = duration_per_size / sum(times)
        memory_usage = get_process_memory()
        
        scalability_results[size] = {
            'avg_time': avg_time,
            'steps_per_sec': steps_per_sec,
            'memory_usage': memory_usage,
            'neurons_per_sec': size * steps_per_sec / duration_per_size
        }
        
        print(f"       Average step time: {avg_time*1000:.2f} ms")
        print(f"       Throughput: {steps_per_sec:.1f} steps/second")
        print(f"       Memory usage: {memory_usage:.1f} MB")
        print(f"       Neurons processed per second: {size * steps_per_sec / duration_per_size:.0f}")
    
    # Analyze scalability
    print(f"\n     Scalability analysis:")
    if len(brain_sizes) >= 2:
        sizes = sorted(brain_sizes)
        times = [scalability_results[size]['avg_time'] for size in sizes]
        throughputs = [scalability_results[size]['steps_per_sec'] for size in sizes]
        neurons_per_sec = [scalability_results[size]['neurons_per_sec'] for size in sizes]
        
        print(f"       Time scaling: {times[0]:.4f}s → {times[-1]:.4f}s ({times[-1]/times[0]:.2f}x)")
        print(f"       Throughput scaling: {throughputs[0]:.1f} → {throughputs[-1]:.1f} steps/sec ({throughputs[-1]/throughputs[0]:.2f}x)")
        print(f"       Neuron throughput: {neurons_per_sec[0]:.0f} → {neurons_per_sec[-1]:.0f} neurons/sec")
        
        # Check for sublinear scaling (good)
        if throughputs[-1]/throughputs[0] < sizes[-1]/sizes[0]:
            print(f"       ✓ Sublinear scaling detected (performance per neuron decreasing)")
        else:
            print(f"       ⚠ Linear or super-linear scaling")
    
    return scalability_results

def benchmark_neuromodulation_overhead(brain, agent, world, duration):
    """Benchmark performance overhead of neuromodulation"""
    print(f"\n   Benchmarking neuromodulation overhead...")
    
    # Run without neuromodulation
    print("     Testing baseline (no neuromodulation)...")
    
    # Disable neuromodulation
    if hasattr(agent, 'enableRewardModulation'):
        agent.enableRewardModulation(False)
    if hasattr(agent, 'enableCuriosity'):
        agent.enableCuriosity(False)
    
    times_no_neuro = []
    for step in range(duration):
        start = time.perf_counter()
        brain.step(step)
        end = time.perf_counter()
        times_no_neuro.append(end - start)
    
    avg_time_no_neuro = sum(times_no_neuro) / len(times_no_neuro)
    
    # Enable neuromodulation
    print("     Testing with neuromodulation enabled...")
    
    if hasattr(agent, 'enableRewardModulation'):
        agent.enableRewardModulation(True)
    if hasattr(agent, 'enableCuriosity'):
        agent.enableCuriosity(True)
    
    times_with_neuro = []
    for step in range(duration):
        start = time.perf_counter()
        brain.step(step)
        end = time.perf_counter()
        times_with_neuro.append(end - start)
    
    avg_time_with_neuro = sum(times_with_neuro) / len(times_with_neuro)
    
    # Calculate overhead
    overhead_ratio = avg_time_with_neuro / avg_time_no_neuro
    overhead_percent = (overhead_ratio - 1.0) * 100
    
    print(f"     Performance comparison:")
    print(f"       Without neuromodulation: {avg_time_no_neuro*1000:.2f} ms/step")
    print(f"       With neuromodulation: {avg_time_with_neuro*1000:.2f} ms/step")
    print(f"       Overhead: {overhead_percent:+.1f}%")
    
    if overhead_percent < 10:
        print(f"       ✓ Minimal neuromodulation overhead")
    elif overhead_percent < 25:
        print(f"       ⚠ Moderate neuromodulation overhead")
    else:
        print(f"       ⚠ High neuromodulation overhead")
    
    return {
        'avg_time_no_neuro': avg_time_no_neuro,
        'avg_time_with_neuro': avg_time_with_neuro,
        'overhead_ratio': overhead_ratio,
        'overhead_percent': overhead_percent
    }

def main():
    print_perf_header("NLM Performance Benchmarking Examples")
    
    # 1. Setup benchmark environment
    print("\n1. Setting up performance benchmarking...")
    
    # Create configuration for benchmarking
    config = pynlm.createDefaultConfig()
    config.set("brain.neuronCount", 100)
    config.set("plasticity.enable", True)
    
    print("   Configuration created for benchmarking:")
    print("     - Neuron count: 100")
    print("     - Plasticity enabled")
    print("     - Neuromodulation ready")
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    print("   Benchmark environment ready")
    
    # 2. Run step timing benchmark
    print("\n2. Running step timing benchmark...")
    
    step_timing = benchmark_step_timing(brain, 200)
    
    # 3. Run memory usage benchmark
    print("\n3. Running memory usage benchmark...")
    
    memory_usage = benchmark_memory_usage(brain, 100, 5)
    
    # 4. Run scalability benchmark
    print("\n4. Running scalability benchmark...")
    
    scalability = benchmark_scalability([50, 100, 200], 50)
    
    # 5. Run neuromodulation overhead benchmark
    print("\n5. Running neuromodulation overhead benchmark...")
    
    neuromod_overhead = benchmark_neuromodulation_overhead(brain, agent, world, 100)
    
    # 6. Performance analysis
    print("\n6. Performance analysis...")
    
    print("\n   Overall performance metrics:")
    print(f"     - Average step time: {step_timing['avg_time']*1000:.2f} ms")
    print(f"     - Throughput: {step_timing['steps_per_sec']:.1f} steps/second")
    print(f"     - Memory growth: {memory_usage['memory_growth']:+.1f} MB")
    print(f"     - Neuromodulation overhead: {neuromod_overhead['overhead_percent']:+.1f}%")
    
    # 7. Performance recommendations
    print("\n7. Performance recommendations...")
    
    print("\n   Optimization opportunities:")
    
    if step_timing['std_time'] / step_timing['avg_time'] > 0.5:
        print("     - High step time variability detected")
        print("       Recommendation: Check for performance bottlenecks")
    
    if memory_usage['memory_growth'] > 50:
        print("     - High memory growth detected")
        print("       Recommendation: Check for memory leaks")
    
    if neuromod_overhead['overhead_percent'] > 25:
        print("     - High neuromodulation overhead detected")
        print("       Recommendation: Optimize neuromodulator calculations")
    
    # Check scalability
    if len(scalability) >= 2:
        sizes = sorted(scalability.keys())
        throughputs = [scalability[size]['steps_per_sec'] for size in sizes]
        
        if throughputs[-1]/throughputs[0] < sizes[-1]/sizes[0]:
            print("     - Sublinear scaling detected")
            print("       Recommendation: Consider parallelization for larger brains")
    
    # 8. Practical performance insights
    print("\n8. Practical performance insights...")
    
    print("\n   Benchmarking insights:")
    print(f"     - Real-time capability: {'YES' if step_timing['avg_time'] < 0.01 else 'NO'} (need <10ms/step)")
    print(f"     - Memory efficiency: {'GOOD' if memory_usage['steps_per_sec'] > 100 else 'NEEDS IMPROVEMENT'}")
    print(f"     - Neuromodulation overhead: {'ACCEPTABLE' if neuromod_overhead['overhead_percent'] < 10 else 'HIGH'}")
    print(f"     - Scalability: {'EXCELLENT' if len(scalability) >= 3 else 'MODERATE'}")
    
    # 9. Summary
    print("\n9. Performance benchmarking summary...")
    
    print("\n   Key performance metrics:")
    print(f"     - Steps per second: {step_timing['steps_per_sec']:.1f}")
    print(f"     - Time per step: {step_timing['time_per_step_ms']:.2f} ms")
    print(f"     - Memory growth: {memory_usage['memory_growth']:+.1f} MB")
    print(f"     - Neuromodulation overhead: {neuromod_overhead['overhead_percent']:+.1f}%")
    
    print("\n   Performance recommendations:")
    print("     - Optimize for real-time applications (<10ms/step)")
    print("     - Monitor memory usage for long-running simulations")
    print("     - Consider neuromodulation overhead in performance-critical applications")
    print("     - Plan for scalability when increasing brain size")
    print("     - Use sub-threshold optimization for large-scale simulations")
    
    print_perf_header("Performance Benchmarking Complete")
    
    print("\nNext Steps:")
    print("  - Try simple_brain.py for basic examples")
    print("  - Try agent_world.py for environment interaction")
    print("  - Try advanced_config.py for configuration examples")
    print("  - Try plasticity_experiment.py for learning experiments")
    print("  - Try neuromodulation_study.py for neuromodulation analysis")
    print("  - Try memory_system_test.py for memory system testing")
    print("\nFor detailed performance optimization, see ADVANCED_FEATURES_IMPLEMENTATION.md")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"\nError: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)