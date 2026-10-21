"""
Benchmark utilities for NLM.

This module provides benchmarking and performance testing utilities
for NLM simulations.
"""

import sys
import time
import json
import argparse
from typing import Dict, Any, Optional
import pynlm

def benchmark_brain_creation(steps: int = 50) -> Dict[str, Any]:
    """
    Benchmark brain creation and initialization.
    
    Args:
        steps: Number of brains to benchmark
        
    Returns:
        Dictionary containing benchmark results
    """
    times = []
    
    for i in range(steps):
        start_time = time.time()
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        for j in range(10):
            brain.step(j)
        end_time = time.time()
        
        times.append(end_time - start_time)
    
    return {
        'avg_time_per_brain': sum(times) / len(times) if times else 0,
        'std_time_per_brain': (sum((t - sum(times)/len(times))**2 for t in times) / len(times))**0.5 if len(times) > 1 else 0,
        'min_time': min(times) if times else 0,
        'max_time': max(times) if times else 0,
        'total_time': sum(times),
        'brains_per_second': steps / sum(times) if sum(times) > 0 else 0
    }

def benchmark_simulation_speed(steps: int = 1000) -> Dict[str, Any]:
    """
    Benchmark simulation speed.
    
    Args:
        steps: Number of simulation steps
        
    Returns:
        Dictionary containing simulation benchmark results
    """
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    start_time = time.time()
    for step in range(steps):
        brain.step(step)
    end_time = time.time()
    
    total_time = end_time - start_time
    
    return {
        'total_steps': steps,
        'total_time': total_time,
        'steps_per_second': steps / total_time if total_time > 0 else 0,
        'time_per_step': total_time / steps if steps > 0 else 0,
        'spikes_per_second': brain.getTotalSpikeCount() / total_time if total_time > 0 else 0
    }

def benchmark_world_interaction(steps: int = 500) -> Dict[str, Any]:
    """
    Benchmark world interaction performance.
    
    Args:
        steps: Number of simulation steps
        
    Returns:
        Dictionary containing world interaction benchmark results
    """
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    start_time = time.time()
    
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    end_time = time.time()
    total_time = end_time - start_time
    
    return {
        'total_steps': steps,
        'total_time': total_time,
        'steps_per_second': steps / total_time if total_time > 0 else 0,
        'time_per_step': total_time / steps if steps > 0 else 0,
        'final_brain_spikes': brain.getTotalSpikeCount(),
        'agent_energy': world.getAgentBody().energy,
        'agent_health': world.getAgentBody().health
    }

def save_benchmark_results(results: Dict[str, Any], filename: str):
    """
    Save benchmark results to file.
    
    Args:
        results: Benchmark results to save
        filename: Output filename
    """
    with open(filename, 'w') as f:
        json.dump(results, f, indent=2)
    print(f"Benchmark results saved to {filename}")

def main():
    """Main benchmark function."""
    parser = argparse.ArgumentParser(description="NLM Benchmark Suite")
    parser.add_argument("--brain", action="store_true", help="Benchmark brain creation")
    parser.add_argument("--simulation", action="store_true", help="Benchmark simulation speed")
    parser.add_argument("--world", action="store_true", help="Benchmark world interaction")
    parser.add_argument("--steps", type=int, default=100, help="Number of steps for benchmarks")
    parser.add_argument("--output", type=str, default="benchmark_results.json", help="Output file for results")
    
    args = parser.parse_args()
    
    print("=== NLM Benchmark Suite ===")
    
    all_results = {}
    
    if args.brain or not (args.simulation or args.world):
        print("Benchmarking brain creation...")
        brain_results = benchmark_brain_creation(args.steps)
        all_results['brain_creation'] = brain_results
        print(f"Average brain creation time: {brain_results['avg_time_per_brain']:.4f}s")
        print(f"Brains per second: {brain_results['brains_per_second']:.2f}")
    
    if args.simulation or not (args.simulation or args.world):
        print("Benchmarking simulation speed...")
        sim_results = benchmark_simulation_speed(args.steps)
        all_results['simulation'] = sim_results
        print(f"Steps per second: {sim_results['steps_per_second']:.2f}")
        print(f"Time per step: {sim_results['time_per_step']*1000:.2f}ms")
    
    if args.world or not (args.simulation or args.world):
        print("Benchmarking world interaction...")
        world_results = benchmark_world_interaction(args.steps)
        all_results['world_interaction'] = world_results
        print(f"Steps per second: {world_results['steps_per_second']:.2f}")
        print(f"Final brain spikes: {world_results['final_brain_spikes']}")
    
    # Save results
    save_benchmark_results(all_results, args.output)
    
    print("\n=== Benchmark Complete ===")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)
