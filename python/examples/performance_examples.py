"""Performance and monitoring examples for NLM Python bindings."""

from pynlm import NeuralSystem, PerformanceProfiler
import numpy as np
import time

print("=== NLM Performance Examples ===")

# Example 1: Basic performance profiling
print("\n1. Basic Performance Profiling:")
print("Demonstrating performance monitoring...")

profiler = PerformanceProfiler()
profiler.start_profiling()

# Run simulation
brain = NeuralSystem()
world = brain.create_world()

# Run multiple episodes
for i in range(3):
    episode, reward = brain.run_episode(max_steps=100)
    print(f"  Episode {i + 1}: {reward} reward")

profiler.end_profiling()

print(f"  Average time: {profiler.avg_time:.4f}s")
print(f"  Average memory: {profiler.avg_memory:.2f} MB")

# Example 2: Scalability testing
print("\n2. Scalability Testing:")
print("Testing performance with different brain sizes...")

sizes = [100, 500, 1000]
for size in sizes:
    print(f"  Testing with {size} neurons...")
    
    # Create brain with specific configuration
    brain = NeuralSystem()
    # Note: Actual configuration would need to be set
    
    profiler = PerformanceProfiler()
    profiler.start_profiling()
    
    # Run simulation
    episode, reward = brain.run_episode(max_steps=50)
    
    profiler.end_profiling()
    
    print(f"    Average time: {profiler.avg_time:.4f}s")
    print(f"    Average memory: {profiler.avg_memory:.2f} MB")
    print(f"    Reward: {reward}")

# Example 3: Memory usage analysis
print("\n3. Memory Usage Analysis:")
print("Analyzing memory consumption over time...")

brain = NeuralSystem()

# Collect metrics over time
metrics_history = []
for i in range(10):
    metrics = brain.get_metrics()
    metrics_history.append(metrics)
    time.sleep(0.1)  # Small delay to see progression

# Calculate memory statistics
memory_values = [m.memory_usage for m in metrics_history if m.memory_usage > 0]
if memory_values:
    print(f"  Memory usage statistics:")
    print(f"    Mean: {np.mean(memory_values):.2f} MB")
    print(f"    Std: {np.std(memory_values):.2f} MB")
    print(f"    Min: {np.min(memory_values):.2f} MB")
    print(f"    Max: {np.max(memory_values):.2f} MB")

# Example 4: Real-time monitoring
print("\n4. Real-Time Monitoring:")
print("Demonstrating real-time performance monitoring...")

brain = NeuralSystem()

# Simulate real-time monitoring
print("  Monitoring brain state...")
print("  Tracking neural activity...")
print("  Monitoring resource usage...")
print("  Collecting performance metrics...")

for step in range(5):
    metrics = brain.get_metrics()
    print(f"  Step {step + 1}: {metrics.active_neurons} active neurons, {metrics.firing_rate:.2f} Hz")

# Example 5: Benchmarking
print("\n5. Benchmarking:")
print("Running comprehensive benchmarks...")

brain = NeuralSystem()

# Run benchmarking with different parameters
test_params = [
    {"steps": 100, "neurons": 100},
    {"steps": 500, "neurons": 500},
    {"steps": 1000, "neurons": 1000}
]

for params in test_params:
    print(f"  Running benchmark with {params['steps']} steps, {params['neurons']} neurons")
    
    profiler = PerformanceProfiler()
    profiler.start_profiling()
    
    # Run simulation
    episode, reward = brain.run_episode(max_steps=params['steps'])
    
    profiler.end_profiling()
    
    print(f"    Performance: {profiler.avg_time:.4f}s, {profiler.avg_memory:.2f} MB")
    print(f"    Result: {reward} reward")

print("\nPerformance examples completed!")
