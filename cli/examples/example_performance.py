#!/usr/bin/env python3
"""
NLM Performance Testing Example

This script demonstrates performance monitoring and benchmarking for NLM.
"""

import sys
sys.path.insert(0, "../src")

from core.Config.Config import Config
from brain.Brain import Brain
from brain.NeuronType import NeuronType
import time
import statistics
import json
from pathlib import Path

def benchmark_simulation(config_file: str, steps: int = 1000):
    """Benchmark simulation performance."""
    
    print("=== NLM Performance Benchmark ===\n")
    
    # Load configuration
    config = Config()
    config.load_from_file(config_file)
    
    print(f"Configuration loaded from {config_file}")
    print(f"Simulation steps: {steps}")
    
    # Create brain
    brain = Brain()
    brain.set_max_steps(steps)
    brain.set_time_step(config.get_or("time_time", 0.001))
    brain.set_leak_conductance(config.get_or("leak_conductance", 10.0))
    
    # Setup neural network
    brain.create_region("sensory", config.get_or("sensory_neurons", 10), NeuronType.EXCITATORY)
    brain.create_region("hidden", config.get_or("hidden_neurons", 20), NeuronType.EXCITATORY)
    brain.create_region("motor", config.get_or("motor_neurons", 5), NeuronType.EXCITATORY)
    brain.create_region("associative", config.get_or("associative_neurons", 15), NeuronType.EXCITATORY)
    
    # Setup memory systems
    brain.create_working_memory(capacity=config.get_or("working_memory_capacity", 100))
    brain.create_episodic_memory(max_episodes=config.get_or("episodic_memory_capacity", 1000))
    brain.create_semantic_memory()
    brain.create_procedural_memory()
    brain.create_associative_memory()
    
    # Setup connections
    input_neurons = brain.get_region("sensory").get_all_neurons()
    hidden_neurons = brain.get_region("hidden").get_all_neurons()
    output_neurons = brain.get_region("motor").get_all_neurons()
    
    # Connect input to hidden
    for input_neuron in input_neurons:
        for hidden_neuron in hidden_neurons:
            brain.create_synapse(input_neuron, hidden_neuron, 
                               weight=config.get_or("default_synaptic_weight", 0.5))
    
    # Connect hidden to output
    for hidden_neuron in hidden_neurons:
        for output_neuron in output_neurons:
            brain.create_synapse(hidden_neuron, output_neuron,
                               weight=config.get_or("hidden_output_weight", 0.3))
    
    # Run performance benchmark
    print(f"\nStarting performance benchmark...")
    
    step_times = []
    spike_rates = []
    memory_usages = []
    
    start_time = time.time()
    
    for step in range(steps):
        step_start = time.time()
        
        # Apply dynamic input
        import math
        input_strength = 0.5 * (1 + math.sin(2 * math.pi * step / 100)) / 2
        
        for neuron in input_neurons:
            neuron.injectCurrent(input_strength * 2.0)
        
        # Step brain
        brain.step()
        
        step_end = time.time()
        step_time = (step_end - step_start) * 1000  # Convert to ms
        step_times.append(step_time)
        
        # Collect statistics
        stats = brain.get_statistics()
        spike_rates.append(stats.average_firing_rate)
        
        mem_stats = brain.get_memory_statistics()
        memory_usages.append(mem_stats.working_memory_usage / mem_stats.working_memory_capacity)
        
        # Store episodic memory every 100 steps
        if step % 100 == 0:
            from motor.Action import ActionType
            from motor.Action import ActionType
            from motor.Action import ActionType
            from motor.Action import ActionType
            
            episode = EpisodicMemoryItem(
                timestamp=step,
                neurons=[n.get_id() for n in input_neurons],
                values=[input_strength] * len(input_neurons),
                metadata="performance_test",
                action=ActionType.WAIT,
                reward=stats.average_firing_rate * 0.1
            )
            brain.get_episodic_memory().store_episode(episode)
    
    end_time = time.time()
    total_time = end_time - start_time
    
    # Calculate performance metrics
    print(f"\nPerformance Benchmark Results:")
    print(f"  Total simulation time: {total_time:.2f} seconds")
    print(f"  Average step time: {statistics.mean(step_times):.4f} ms")
    print(f"  Min step time: {min(step_times):.4f} ms")
    print(f"  Max step time: {max(step_times):.4f} ms")
    print(f"  Step time standard deviation: {statistics.stdev(step_times):.4f} ms")
    
    print(f"\nSpike Rate Statistics:")
    print(f"  Average: {statistics.mean(spike_rates):.2f} Hz")
    print(f"  Maximum: {max(spike_rates):.2f} Hz")
    print(f"  Minimum: {min(spike_rates):.2f} Hz")
    print(f"  Standard deviation: {statistics.stdev(spike_rates):.2f} Hz")
    
    print(f"\nMemory Usage Statistics:")
    print(f"  Average utilization: {statistics.mean(memory_usages)*100:.1f}%")
    print(f"  Peak utilization: {max(memory_usages)*100:.1f}%")
    print(f"  Memory efficiency: {(statistics.mean(memory_usages) / max(memory_usages))*100:.1f}%")
    
    # Calculate steps per second
    steps_per_second = steps / total_time
    print(f"\nPerformance Metrics:")
    print(f"  Steps per second: {steps_per_second:.0f}")
    print(f"  Milliseconds per step: {1000/steps_per_second:.2f} ms")
    print(f"  Neural throughput: {steps_per_second * 1000:.0f} steps/second")
    
    # Save detailed results
    results = {
        "benchmark_parameters": {
            "config_file": config_file,
            "simulation_steps": steps,
            "total_time_seconds": total_time,
            "steps_per_second": steps_per_second
        },
        "performance_metrics": {
            "step_time_mean": statistics.mean(step_times),
            "step_time_std": statistics.stdev(step_times),
            "step_time_min": min(step_times),
            "step_time_max": max(step_times),
            "spike_rate_mean": statistics.mean(spike_rates),
            "spike_rate_std": statistics.stdev(spike_rates),
            "spike_rate_max": max(spike_rates),
            "spike_rate_min": min(spike_rates),
            "memory_usage_mean": statistics.mean(memory_usages),
            "memory_usage_max": max(memory_usages)
        },
        "network_architecture": {
            "input_neurons": len(input_neurons),
            "hidden_neurons": len(hidden_neurons),
            "output_neurons": len(output_neurons),
            "total_synapses": len(brain.get_all_synapses())
        },
        "memory_system": {
            "working_memory_capacity": brain.get_working_memory().getCapacity(),
            "working_memory_usage": sum(1 for n in brain.get_all_neurons() if brain.get_working_memory().contains(n.get_id())),
            "episodic_memory_episodes": brain.get_episodic_memory().getEpisodeCount(),
            "semantic_memory_facts": len(brain.get_semantic_memory().getAllFacts()),
            "procedural_memory_skills": len(brain.get_procedural_memory().getAllSkills()),
            "associative_memory_associations": len(brain.get_associative_memory().getAllAssociations())
        }
    }
    
    # Save results to JSON file
    results_file = "performance_benchmark_results.json"
    with open(results_file, "w") as f:
        json.dump(results, f, indent=2)
    
    print(f"\nDetailed results saved to {results_file}")
    
    # Save summary to text file
    summary_file = "performance_benchmark_summary.txt"
    with open(summary_file, "w") as f:
        f.write("NLM Performance Benchmark Summary\n")
        f.write("=" * 50 + "\n\n")
        f.write(f"Configuration: {config_file}\n")
        f.write(f"Simulation steps: {steps}\n")
        f.write(f"Total time: {total_time:.2f} seconds\n")
        f.write(f"Steps per second: {steps_per_second:.0f}\n\n")
        
        f.write("Performance Metrics:\n")
        f.write(f"  Average step time: {statistics.mean(step_times):.4f} ms\n")
        f.write(f"  Step time standard deviation: {statistics.stdev(step_times):.4f} ms\n")
        f.write(f"  Average spike rate: {statistics.mean(spike_rates):.2f} Hz\n")
        f.write(f"  Spike rate standard deviation: {statistics.stdev(spike_rates):.2f} Hz\n\n")
        
        f.write("Network Architecture:\n")
        f.write(f"  Input neurons: {len(input_neurons)}\n")
        f.write(f"  Hidden neurons: {len(hidden_neurons)}\n")
        f.write(f"  Output neurons: {len(output_neurons)}\n")
        f.write(f"  Total synapses: {len(brain.get_all_synapses())}\n\n")
        
        f.write("Memory System:\n")
        mem_stats = brain.get_memory_statistics()
        f.write(f"  Working memory: {mem_stats.working_memory_usage}/{mem_stats.working_memory_capacity}\n")
        f.write(f"  Episodic memory: {mem_stats.episodic_memory_episodes} episodes\n")
        f.write(f"  Semantic memory: {len(brain.get_semantic_memory().getAllFacts())} facts\n")
        f.write(f"  Procedural memory: {len(brain.get_procedural_memory().getAllSkills())} skills\n")
        f.write(f"  Associative memory: {mem_stats.associative_memory_associations} associations\n")
    
    print(f"Summary saved to {summary_file}")
    
    print(f"\n=== Benchmark Complete ===")

class EpisodicMemoryItem:
    """Simple implementation for benchmarking example."""
    def __init__(self, timestamp, neurons, values, metadata, action, reward):
        self.timestamp = timestamp
        self.neurons = neurons
        self.values = values
        self.metadata = metadata
        self.action = action
        self.reward = reward

if __name__ == "__main__":
    # Run benchmark with default configuration
    import sys
    
    config_file = sys.argv[1] if len(sys.argv) > 1 else "configs/basic_config.yaml"
    
    if not sys.path.exists(config_file):
        print(f"Configuration file not found: {config_file}")
        print("Available configurations:")
        for f in Path("configs").glob("*.yaml"):
            print(f"  {f.name}")
        sys.exit(1)
    
    steps = 1000  # Default benchmark steps
    
    print(f"Running performance benchmark with {config_file}")
    print(f"Using {steps} simulation steps")
    
    benchmark_simulation(config_file, steps)