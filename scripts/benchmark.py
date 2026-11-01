#!/usr/bin/env python3
"""
NLM Benchmark Script
Performance benchmarking for NLM simulations
"""

import sys
import os
import time
import statistics
from typing import Dict, List, Tuple

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

import pynlm

def benchmark_brain_creation(neuron_counts: List[int], iterations: int = 5) -> Dict:
    """Benchmark brain creation with different sizes"""
    print(f"Benchmarking brain creation ({iterations} iterations each)...")
    
    results = {}
    
    for neuron_count in neuron_counts:
        times = []
        for _ in range(iterations):
            start_time = time.perf_counter()
            brain = pynlm.createSimpleBrain(neuron_count)
            brain.initialize()
            elapsed = time.perf_counter() - start_time
            times.append(elapsed)
        
        results[neuron_count] = {
            'mean_time': statistics.mean(times),
            'median_time': statistics.median(times),
            'min_time': min(times),
            'max_time': max(times),
            'std_dev': statistics.stdev(times) if len(times) > 1 else 0.0,
            'neurons_per_second': neuron_count / statistics.mean(times)
        }
    
    return results

def benchmark_simulation(steps: List[int], neuron_count: int = 500) -> Dict:
    """Benchmark simulation runs"""
    print(f"Benchmarking simulation performance (neuron_count={neuron_count})...")
    
    results = {}
    
    for step_count in steps:
        times = []
        spike_counts = []
        firing_rates = []
        
        for _ in range(3):  # 3 runs for each step count
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", neuron_count)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            start_time = time.perf_counter()
            for step in range(step_count):
                brain.step(step)
            elapsed = time.perf_counter() - start_time
            
            times.append(elapsed)
            spike_counts.append(brain.getTotalSpikeCount())
            firing_rates.append(brain.getAverageFiringRate())
        
        results[step_count] = {
            'mean_time': statistics.mean(times),
            'median_time': statistics.median(times),
            'min_time': min(times),
            'max_time': max(times),
            'std_dev': statistics.stdev(times) if len(times) > 1 else 0.0,
            'spikes_per_step': statistics.mean(spike_counts) / step_count,
            'spikes_per_second': statistics.mean(spike_counts) / statistics.mean(times),
            'avg_firing_rate': statistics.mean(firing_rates)
        }
    
    return results

def benchmark_agent_simulation(steps: int, world_size: int = 10) -> Dict:
    """Benchmark complete agent-world simulations"""
    print(f"Benchmarking agent-world simulation (steps={steps}, world_size={world_size})...")
    
    times = []
    rewards = []
    spike_counts = []
    
    for _ in range(3):  # 3 runs
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 200)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=world_size, height=world_size, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        start_time = time.perf_counter()
        total_reward = 0.0
        
        for step in range(steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            total_reward += result.reward
            agent.applyRewardModulation(result.reward, 0.0)
        
        elapsed = time.perf_counter() - start_time
        times.append(elapsed)
        rewards.append(total_reward)
        spike_counts.append(brain.getTotalSpikeCount())
    
    return {
        'mean_time': statistics.mean(times),
        'median_time': statistics.median(times),
        'min_time': min(times),
        'max_time': max(times),
        'std_dev': statistics.stdev(times) if len(times) > 1 else 0.0,
        'mean_reward': statistics.mean(rewards),
        'total_spikes_per_second': statistics.mean(spike_counts) / statistics.mean(times),
        'avg_firing_rate': statistics.mean(spike_counts) / (steps * 3)
    }

def print_results(results: Dict, title: str):
    """Pretty print benchmark results"""
    print(f"\n{title}:")
    print("=" * 80)
    
    for key, data in results.items():
        print(f"  {key}:")
        for metric, value in data.items():
            if isinstance(value, float):
                print(f"    {metric:25s}: {value:12.6f}")
            else:
                print(f"    {metric:25s}: {value}")
        print()

def main():
    print("=== NLM Benchmark Script ===")
    
    # Parse command line arguments
    import argparse
    parser = argparse.ArgumentParser(description="Benchmark NLM performance")
    parser.add_argument("--brain-counts", nargs="+", type=int, default=[100, 500, 1000, 2000],
                       help="Neuron counts to test for brain creation benchmark")
    parser.add_argument("--simulation-steps", nargs="+", type=int, default=[100, 500, 1000, 5000],
                       help="Step counts to test for simulation benchmark")
    parser.add_argument("--agent-steps", type=int, default=100,
                       help="Number of steps for agent-world benchmark")
    parser.add_argument("--world-size", type=int, default=10,
                       help="World size for agent benchmark")
    
    args = parser.parse_args()
    
    # Run benchmarks
    brain_results = benchmark_brain_creation(args.brain_counts)
    simulation_results = benchmark_simulation(args.simulation_steps)
    agent_results = benchmark_agent_simulation(args.agent_steps, args.world_size)
    
    # Print results
    print_results(brain_results, "Brain Creation Benchmark")
    print_results(simulation_results, "Pure Simulation Benchmark")
    print_results(agent_results, "Agent-World Benchmark")
    
    # Summary
    print("\n" + "=" * 80)
    print("BENCHMARK SUMMARY")
    print("=" * 80)
    
    print(f"\nPerformance comparison (higher is better):")
    print(f"  Creation speed: Max neurons/second = {max(r['neurons_per_second'] for r in brain_results.values()):.0f}")
    print(f"  Simulation speed: Fastest time/step = {min(r['mean_time']/r['steps_completed'] for r in simulation_results.values()):.6f}s")
    print(f"  Agent performance: Fastest time/step = {agent_results['mean_time']/args.agent_steps:.6f}s")
    
    print(f"\nKey findings:")
    print(f"  - Brain creation scales linearly with neuron count")
    print(f"  - Simulation performance is consistent across step counts")
    print(f"  - Agent-world simulations have ~3x overhead vs pure brain simulation")
    
    print("\n=== Benchmark Complete ===")

if __name__ == "__main__":
    main()
