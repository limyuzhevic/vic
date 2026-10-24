"""
Benchmark script for NLM Python bindings.

This script runs performance benchmarks on the NLM Python API.
"""

import time
import sys
from pathlib import Path
import json

# Add the current directory to the path so we can import pynlm
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
    HAS_PYNLM = True
except ImportError:
    HAS_PYNLM = False
    print("ERROR: pynlm module not available")
    sys.exit(1)
def benchmark_brain_creation(num_iterations=100):
    """Benchmark brain creation performance."""
    print(f"Benchmarking brain creation ({num_iterations} iterations)...")
    
    times = []
    for i in range(num_iterations):
        start_time = time.perf_counter()
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        end_time = time.perf_counter()
        times.append(end_time - start_time)
    
    avg_time = sum(times) / len(times)
    min_time = min(times)
    max_time = max(times)
    
    print(f"  Average time: {avg_time*1000:.2f} ms")
    print(f"  Min time: {min_time*1000:.2f} ms")
    print(f"  Max time: {max_time*1000:.2f} ms")
    print(f"  Total neurons created: {brain.getTotalNeuronCount()}")
    
    return {
        "avg_time_ms": avg_time * 1000,
        "min_time_ms": min_time * 1000,
        "max_time_ms": max_time * 1000,
        "total_neurons": brain.getTotalNeuronCount(),
    }
def benchmark_simulation_steps(num_steps=1000, num_iterations=10):
    """Benchmark simulation step performance."""
    print(f"Benchmarking simulation steps ({num_steps} steps, {num_iterations} iterations)...")
    
    times = []
    for iteration in range(num_iterations):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.perf_counter()
        for step in range(num_steps):
            brain.step(step)
        end_time = time.perf_counter()
        
        elapsed = end_time - start_time
        times.append(elapsed)
        
        if iteration % 5 == 0:
            print(f"  Iteration {iteration + 1}: {elapsed:.3f} seconds")
    
    avg_time = sum(times) / len(times)
    min_time = min(times)
    max_time = max(times)
    
    steps_per_second = num_steps / avg_time
    
    print(f"  Average time: {avg_time:.3f} seconds")
    print(f"  Min time: {min_time:.3f} seconds")
    print(f"  Max time: {max_time:.3f} seconds")
    print(f"  Steps per second: {steps_per_second:.0f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    return {
        "avg_time_seconds": avg_time,
        "min_time_seconds": min_time,
        "max_time_seconds": max_time,
        "steps_per_second": steps_per_second,
        "total_spikes": brain.getTotalSpikeCount(),
    }
def benchmark_agent_simulation(num_steps=500, num_iterations=5):
    """Benchmark complete agent simulation (brain + world + agent)."""
    print(f"Benchmarking agent simulation ({num_steps} steps, {num_iterations} iterations)...")
    
    times = []
    for iteration in range(num_iterations):
        # Setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        start_time = time.perf_counter()
        for step in range(num_steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        end_time = time.perf_counter()
        elapsed = end_time - start_time
        times.append(elapsed)
        
        print(f"  Iteration {iteration + 1}: {elapsed:.3f} seconds")
    
    avg_time = sum(times) / len(times)
    min_time = min(times)
    max_time = max(times)
    
    steps_per_second = num_steps / avg_time
    
    print(f"  Average time: {avg_time:.3f} seconds")
    print(f"  Min time: {min_time:.3f} seconds")
    print(f"  Max time: {max_time:.3f} seconds")
    print(f"  Steps per second: {steps_per_second:.0f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Curiosiy level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    
    return {
        "avg_time_seconds": avg_time,
        "min_time_seconds": min_time,
        "max_time_seconds": max_time,
        "steps_per_second": steps_per_second,
        "total_spikes": brain.getTotalSpikeCount(),
        "curiosity_level": agent.getCuriosityLevel(),
        "novelty_level": agent.getNoveltyLevel(),
    }
def benchmark_memory_usage():
    """Benchmark memory usage by creating large brains."""
    print("Benchmarking memory usage with large brain...")
    
    try:
        import psutil
        import os
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Create large brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 5000)
        config.set("brain.synapse_density", 0.1)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some simulation
        for i in range(100):
            brain.step(i)
        
        final_memory = process.memory_info().rss / 1024 / 1024  # MB
        memory_increase = final_memory - initial_memory
        
        print(f"  Initial memory: {initial_memory:.1f} MB")
        print(f"  Final memory: {final_memory:.1f} MB")
        print(f"  Memory increase: {memory_increase:.1f} MB")
        print(f"  Neurons: {brain.getTotalNeuronCount()}")
        print(f"  Synapses: {brain.getTotalSynapseCount()}")
        
        return {
            "initial_memory_mb": initial_memory,
            "final_memory_mb": final_memory,
            "memory_increase_mb": memory_increase,
            "neurons": brain.getTotalNeuronCount(),
            "synapses": brain.getTotalSynapseCount(),
        }
        
    except ImportError:
        print("  psutil not available, skipping memory benchmark")
        return None
def main():
    """Run all benchmarks."""
    print("=== NLM Python Benchmark Suite ===")
    print()
    
    all_results = {}
    
    # Benchmark 1: Brain creation
    all_results["brain_creation"] = benchmark_brain_creation(50)
    print()
    
    # Benchmark 2: Simulation steps
    all_results["simulation_steps"] = benchmark_simulation_steps(500, 5)
    print()
    
    # Benchmark 3: Agent simulation
    all_results["agent_simulation"] = benchmark_agent_simulation(200, 3)
    print()
    
    # Benchmark 4: Memory usage (if psutil available)
    all_results["memory_usage"] = benchmark_memory_usage()
    print()
    
    # Save results to JSON file
    output_file = Path("benchmark_results.json")
    with open(output_file, 'w') as f:
        json.dump(all_results, f, indent=2)
    
    print(f"Benchmark results saved to: {output_file}")
    print()
    print("=== Summary ===")
    for benchmark_name, results in all_results.items():
        if results is not None:
            print(f"{benchmark_name}:")
            for key, value in results.items():
                if isinstance(value, float):
                    print(f"  {key}: {value:.2f}")
                else:
                    print(f"  {key}: {value}")
            print()
if __name__ == "__main__":
    main()