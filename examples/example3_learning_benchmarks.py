#!/usr/bin/env python3
"""
Example 3: Learning Benchmarks

This example demonstrates how to run comparative experiments and benchmark
different configurations for learning performance.
"""

import sys
import os
import json
import time

# Add the parent directory to path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import pynlm

def run_learning_benchmarks():
    print("\n=== Example 3: Learning Benchmarks ===")
    
    configurations = [
        {'name': 'Small Brain', 'neurons': 500, 'density': 0.05, 'learning_rate': 0.01},
        {'name': 'Medium Brain', 'neurons': 1000, 'density': 0.1, 'learning_rate': 0.01},
        {'name': 'Large Brain', 'neurons': 2000, 'density': 0.15, 'learning_rate': 0.005},
        {'name': 'High Plasticity', 'neurons': 1000, 'density': 0.2, 'learning_rate': 0.02},
    ]
    
    results = {}
    
    for config in configurations:
        print(f"\n--- Testing {config['name']} ---")
        
        # Create configuration
        cfg = pynlm.createDefaultConfig()
        cfg.set("brain.neuron_count", config['neurons'])
        cfg.set("brain.synapse_density", config['density'])
        cfg.set("plasticity.stdp.learning_rate", config['learning_rate'])
        cfg.set("plasticity.hebbian.learning_rate", config['learning_rate'])
        
        # Create brain and agent
        brain = pynlm.createBrain(cfg)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Setup world
        world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        world.reset()
        world.setAgentStart(5, 5)
        agent.initialize(world)
        
        # Enable all learning features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run experiment
        steps = 500
        start_time = time.time()
        
        for step in range(steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Simple reward function
            internal = percept.getInternal()
            reward = internal[0] if internal else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            if step % 100 == 0:
                agent.updateDevelopment(0.1)
        
        end_time = time.time()
        
        # Collect metrics
        result = {
            'name': config['name'],
            'final_firing': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'dopamine': agent.getNeuromodulationLevel(),
            'development_stage': agent.getDevelopmentalStage(),
            'steps_per_second': steps / (end_time - start_time),
            'learning_rate': config['learning_rate']
        }
        
        results[config['name']] = result
        
        print(f"  Final firing: {result['final_firing']} neurons")
        print(f"  Average firing rate: {result['average_firing_rate']:.2f} Hz")
        print(f"  Performance: {result['steps_per_second']:.1f} steps/sec")
        print(f"  Time: {end_time - start_time:.2f} seconds")
    
    # Compare results
    print("\n=== Benchmark Comparison ===")
    print(f"{'Configuration':<15} {'Firing':<10} {'Avg Rate':<12} {'Curiosity':<10} {'Performance':<12}")
    print("-" * 70)
    
    for name, result in results.items():
        print(f"{name:<15} {result['final_firing']:<10} {result['average_firing_rate']:<12.2f} "
              f"{result['curiosity']:<10.3f} {result['steps_per_second']:<12.1f}")
    
    # Save results
    with open('benchmark_results.json', 'w') as f:
        json.dump(results, f, indent=2)
    
    print("\nBenchmark results saved to benchmark_results.json")
    return results

if __name__ == "__main__":
    run_learning_benchmarks()
