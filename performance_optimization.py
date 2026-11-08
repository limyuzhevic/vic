#!/usr/bin/env python3
"""
Performance Optimization Guide for NLM Python API

This document provides performance optimization techniques for the NLM Python API,
including best practices, common bottlenecks, and optimization strategies.
"""

import pynlm
import time
import psutil
import os
from memory_profiler import profile
import numpy as np

class PerformanceOptimizer:
    """Performance optimization utilities for NLM Python API."""
    
    @staticmethod
    def profile_simulation(brain_config, duration_steps=100):
        """
        Profile simulation performance and identify bottlenecks.
        
        Args:
            brain_config: Dictionary of brain configuration settings
            duration_steps: Number of steps to profile
            
        Returns:
            Dictionary with performance metrics
        """
        print("=== NLM Performance Profiling ===")
        
        # Get initial memory
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Create and initialize brain
        config = pynlm.createDefaultConfig()
        for key, value in brain_config.items():
            config.set(key, value)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get memory after brain creation
        after_brain_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Get memory after agent creation
        after_agent_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Run simulation with profiling
        start_time = time.time()
        initial_spikes = brain.getTotalSpikeCount()
        
        for step in range(duration_steps):
            # Update world
            world.update(0.1)
            
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Log progress
            if step % 50 == 0:
                current_memory = process.memory_info().rss / 1024 / 1024
                print(f"Step {step}: Memory={current_memory:.1f}MB, "
                      f"Spikes={brain.getTotalSpikeCount()}")
        
        end_time = time.time()
        
        # Get final memory usage
        final_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Calculate metrics
        total_time = end_time - start_time
        steps_per_second = duration_steps / total_time
        final_spikes = brain.getTotalSpikeCount()
        spikes_completed = final_spikes - initial_spikes
        
        metrics = {
            "brain_config": brain_config,
            "duration_steps": duration_steps,
            "total_time_seconds": total_time,
            "steps_per_second": steps_per_second,
            "spikes_completed": spikes_completed,
            "spikes_per_second": spikes_completed / total_time,
            "memory_initial_mb": initial_memory,
            "memory_after_brain_mb": after_brain_memory,
            "memory_after_agent_mb": after_agent_memory,
            "memory_final_mb": final_memory,
            "memory_growth_mb": final_memory - initial_memory,
            "memory_efficiency_spikes_per_mb": spikes_completed / (final_memory - initial_memory) if (final_memory - initial_memory) > 0 else 0,
            "firing_rate": brain.getAverageFiringRate(),
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount()
        }
        
        print("\n=== Performance Summary ===")
        print(f"Total time: {total_time:.2f} seconds")
        print(f"Steps per second: {steps_per_second:.1f}")
        print(f"Spikes per second: {spikes_completed/total_time:.1f}")
        print(f"Memory growth: {metrics['memory_growth_mb']:.1f} MB")
        print(f"Spikes per MB memory: {metrics['memory_efficiency_spikes_per_mb']:.1f}")
        print(f"Firing rate: {metrics['firing_rate']:.2f} Hz")
        
        return metrics
    
    @staticmethod
    def optimize_configuration(brain_config, target_performance=None):
        """
        Optimize configuration for target performance.
        
        Args:
            brain_config: Initial brain configuration
            target_performance: Target performance score (optional)
            
        Returns:
            Optimized configuration
        """
        print("=== Configuration Optimization ===")
        
        if target_performance is None:
            target_performance = 1000  # Default target
        
        # Define parameter ranges for optimization
        parameter_ranges = {
            "brain.neuron_count": list(range(500, 2001, 200)),  # 500 to 2000 neurons
            "brain.synapse_density": [0.05, 0.1, 0.15, 0.2],  # Synapse density
            "plasticity.stdp.learning_rate": [0.001, 0.01, 0.1],  # STDP learning rate
            "plasticity.hebbian.learning_rate": [0.001, 0.01, 0.1],  # Hebbian learning rate
        }
        
        best_config = None
        best_score = 0
        
        print("Testing configuration combinations...")
        
        # Test different configurations
        for neuron_count in parameter_ranges["brain.neuron_count"]:
            for synapse_density in parameter_ranges["brain.synapse_density"]:
                for stdp_lr in parameter_ranges["plasticity.stdp.learning_rate"]:
                    for hebbian_lr in parameter_ranges["plasticity.hebbian.learning_rate"]:
                        
                        # Create test configuration
                        test_config = brain_config.copy()
                        test_config["brain.neuron_count"] = neuron_count
                        test_config["brain.synapse_density"] = synapse_density
                        test_config["plasticity.stdp.learning_rate"] = stdp_lr
                        test_config["plasticity.hebbian.learning_rate"] = hebbian_lr
                        
                        # Run quick performance test
                        metrics = PerformanceOptimizer.profile_simulation(test_config, duration_steps=50)
                        
                        # Calculate performance score
                        # Combine spikes per second and memory efficiency
                        performance_score = (metrics["spikes_per_second"] * 1000) + (metrics["memory_efficiency_spikes_per_mb"] * 100)
                        
                        print(f"Config: {neuron_count} neurons, density={synapse_density}, "
                              f"stdp_lr={stdp_lr}, hebbian_lr={hebbian_lr}")
                        print(f"  Performance score: {performance_score:.1f}, "
                              f"Spikes/s: {metrics['spikes_per_second']:.1f}, "
                              f"Memory efficiency: {metrics['memory_efficiency_spikes_per_mb']:.1f}")
                        
                        # Update best configuration if better
                        if performance_score > best_score:
                            best_score = performance_score
                            best_config = test_config.copy()
                            
                            print(f"  -> New best configuration!")
        
        print(f"\n=== Optimization Results ===")
        print(f"Best configuration found:")
        for key, value in best_config.items():
            print(f"  {key}: {value}")
        print(f"Best performance score: {best_score:.1f}")
        
        return best_config
    
    @staticmethod
    def batch_performance_test(configs, iterations=3):
        """
        Run batch performance tests across multiple configurations.
        
        Args:
            configs: List of configuration dictionaries
            iterations: Number of iterations per configuration
            
        Returns:
            Dictionary with batch test results
        """
        print("=== Batch Performance Testing ===")
        
        results = {}
        
        for i, config in enumerate(configs):
            print(f"\nTesting configuration {i+1}/{len(configs)}: {config.get('brain.neuron_count', 'default')} neurons")
            
            config_results = []
            
            for iter in range(iterations):
                print(f"  Iteration {iter+1}/{iterations}")
                metrics = PerformanceOptimizer.profile_simulation(config, duration_steps=100)
                config_results.append(metrics)
            
            # Calculate average metrics for this configuration
            avg_metrics = {}
            for key in config_results[0].keys():
                if isinstance(config_results[0][key], (int, float)):
                    avg_metrics[key] = np.mean([r[key] for r in config_results])
                else:
                    avg_metrics[key] = config_results[0][key]  # Non-numeric values
            
            results[f"config_{i}"] = {
                "config": config,
                "average_metrics": avg_metrics,
                "individual_runs": config_results,
                "consistency": _calculate_consistency(config_results)
            }
        
        return results
    
    @staticmethod
    def memory_profile_simulation(brain_config, max_steps=200):
        """
        Detailed memory profiling of simulation.
        
        Args:
            brain_config: Brain configuration
            max_steps: Maximum number of steps to simulate
            
        Returns:
            Memory profiling data
        """
        print("=== Detailed Memory Profiling ===")
        
        process = psutil.Process(os.getpid())
        
        # Create brain
        config = pynlm.createDefaultConfig()
        for key, value in brain_config.items():
            config.set(key, value)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(8, 8, 6, 6)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        agent.enableRewardModulation(True)
        
        # Memory tracking data
        memory_data = {
            "timestamps": [],
            "memory_mb": [],
            "spikes": [],
            "steps": [],
            "neuron_count": brain.getTotalNeuronCount()
        }
        
        print("Starting memory profiling...")
        
        for step in range(max_steps):
            # Record memory before step
            if step % 5 == 0:
                memory_mb = process.memory_info().rss / 1024 / 1024
                memory_data["timestamps"].append(step)
                memory_data["memory_mb"].append(memory_mb)
                memory_data["spikes"].append(brain.getTotalSpikeCount())
                memory_data["steps"].append(step)
                
                print(f"Step {step}: Memory={memory_mb:.1f}MB, "
                      f"Spikes={memory_data['spikes'][-1]}")
            
            # Run simulation step
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Analyze memory data
        print(f"\n=== Memory Analysis ===")
        print(f"Final memory usage: {memory_data['memory_mb'][-1]:.1f} MB")
        print(f"Peak memory: {max(memory_data['memory_mb']):.1f} MB")
        print(f"Memory growth: {memory_data['memory_mb'][-1] - memory_data['memory_mb'][0]:.1f} MB")
        
        # Calculate memory efficiency
        final_spikes = memory_data['spikes'][-1]
        peak_memory = max(memory_data['memory_mb'])
        initial_memory = memory_data['memory_mb'][0]
        memory_efficiency = final_spikes / (peak_memory - initial_memory) if (peak_memory - initial_memory) > 0 else 0
        
        print(f"Memory efficiency: {memory_efficiency:.1f} spikes per MB")
        
        return memory_data

# Helper function for calculating consistency
def _calculate_consistency(runs):
    """Calculate consistency metrics across runs."""
    if len(runs) < 2:
        return {"consistency_score": 0, "std_dev": 0}
    
    # Calculate coefficient of variation for key metrics
    spikes_per_second = [r["spikes_per_second"] for r in runs]
    memory_efficiency = [r["memory_efficiency_spikes_per_mb"] for r in runs]
    
    spikes_cv = np.std(spikes_per_second) / np.mean(spikes_per_second) if np.mean(spikes_per_second) > 0 else 0
    memory_cv = np.std(memory_efficiency) / np.mean(memory_efficiency) if np.mean(memory_efficiency) > 0 else 0
    
    # Combined consistency score (lower is better)
    consistency_score = (spikes_cv + memory_cv) / 2
    
    return {
        "consistency_score": consistency_score,
        "spikes_cv": spikes_cv,
        "memory_cv": memory_cv,
        "coefficient_of_variation": consistency_score
    }

# Example usage functions
def run_performance_examples():
    """Run performance optimization examples."""
    print("=" * 80)
    print("NLM PERFORMANCE OPTIMIZATION EXAMPLES")
    print("=" * 80)
    
    # Example 1: Basic profiling
    print("\n1. Basic Performance Profiling")
    config1 = {"brain.neuron_count": 1000, "brain.synapse_density": 0.1}
    metrics1 = PerformanceOptimizer.profile_simulation(config1, duration_steps=100)
    
    # Example 2: Configuration optimization
    print("\n2. Configuration Optimization")
    base_config = {"brain.neuron_count": 1000, "brain.synapse_density": 0.1}
    optimized_config = PerformanceOptimizer.optimize_configuration(base_config)
    
    # Example 3: Memory profiling
    print("\n3. Memory Profiling")
    config2 = {"brain.neuron_count": 1500}
    memory_data = PerformanceOptimizer.memory_profile_simulation(config2, max_steps=100)
    
    # Example 4: Batch performance testing
    print("\n4. Batch Performance Testing")
    configs = [
        {"brain.neuron_count": 500},
        {"brain.neuron_count": 1000},
        {"brain.neuron_count": 1500},
        {"brain.neuron_count": 2000}
    ]
    batch_results = PerformanceOptimizer.batch_performance_test(configs, iterations=2)
    
    print(f"\n{'='*80}")
    print("PERFORMANCE OPTIMIZATION COMPLETE")
    print(f"{'='*80}")
    
    return {
        "basic_metrics": metrics1,
        "optimized_config": optimized_config,
        "memory_data": memory_data,
        "batch_results": batch_results
    }

if __name__ == "__main__":
    # Run performance optimization examples
    results = run_performance_examples()
    
    print("\nPerformance optimization completed successfully!")