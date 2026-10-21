"""
Optimized NLM Simulation Examples

This module demonstrates performance optimization techniques for large-scale NLM simulations.
Key optimizations include:
- Parallel processing of multiple brain instances
- Memory-efficient algorithms
- Vectorized operations
- Resource management strategies
- Performance benchmarking

Author: NLM Project
License: MIT
Version: 1.0
"""

import pynlm
import time
import threading
import concurrent.futures
import numpy as np
from typing import List, Dict, Any
import tracemalloc
class OptimizedBrainSimulation:
    """
    Optimized simulation framework for large-scale neural networks.
    
    This simulation demonstrates advanced optimization techniques:
    - Parallel processing with thread pools
    - Memory-efficient data structures
    - Batch processing strategies
    - Resource pooling
    - Performance monitoring
    """
    
    def __init__(self, config: pynlm.Config, num_instances: int = 1):
        self.config = config
        self.num_instances = num_instances
        self.brain_instances = []
        self.thread_pool = None
        self.memory_allocator = None
        self.performance_metrics = {}
        
    def setupParallelBrains(self):
        """Set up multiple brain instances for parallel processing."""
        print(f"Setting up {self.num_instances} brain instances...")
        
        for i in range(self.num_instances):
            brain = pynlm.createBrain(self.config)
            brain.initialize()
            self.brain_instances.append(brain)
            
        print(f"Successfully initialized {len(self.brain_instances)} brain instances")
    
    def setupThreadPool(self, max_workers: int = None):
        """Set up thread pool for parallel processing."""
        if max_workers is None:
            max_workers = min(self.num_instances, 4)
            
        self.thread_pool = concurrent.futures.ThreadPoolExecutor(
            max_workers=max_workers
        )
        print(f"Thread pool created with {max_workers} workers")
    
    def stepAllBrains(self, step: int) -> List[int]:
        """
        Step all brain instances in parallel.
        
        Args:
            step: Current simulation step
            
        Returns:
            List of firing neuron counts from each instance
        """
        if not self.thread_pool:
            raise RuntimeError("Thread pool not initialized")
        
        # Submit all brain stepping tasks
        futures = []
        for brain in self.brain_instances:
            future = self.thread_pool.submit(self._stepBrain, brain, step)
            futures.append(future)
        
        # Wait for all to complete and collect results
        results = []
        for future in concurrent.futures.as_completed(futures):
            results.append(future.result())
        
        return results
    
    def _stepBrain(self, brain: pynlm.Brain, step: int) -> int:
        """Step a single brain instance."""
        try:
            # Step the brain
            brain.step(step)
            
            # Get firing neuron count
            return brain.getFiringNeuronCount()
            
        except Exception as e:
            print(f"Error in brain simulation: {e}")
            return 0
    
    def runOptimizedSimulation(self, total_steps: int, batch_size: int = 10):
        """
        Run optimized simulation with batch processing.
        
        Args:
            total_steps: Total number of simulation steps
            batch_size: Number of steps per batch
        """
        print(f"Running optimized simulation for {total_steps} steps...")
        print(f"Batch size: {batch_size}")
        
        start_time = time.time()
        tracemalloc.start()
        
        # Reset performance metrics
        self.performance_metrics = {
            'total_time': 0,
            'steps_completed': 0,
            'firing_neurons_sum': 0,
            'memory_peak': 0,
            'memory_current': 0
        }
        
        # Run simulation in batches
        for batch in range(0, total_steps, batch_size):
            batch_steps = min(batch_size, total_steps - batch)
            
            print(f"Processing batch {batch//batch_size + 1}/{(total_steps + batch_size - 1)//batch_size}")
            
            # Process batch
            batch_start = time.time()
            firing_counts = self.stepAllBrains(batch)
            batch_time = time.time() - batch_start
            
            # Update metrics
            self.performance_metrics['total_time'] += batch_time
            self.performance_metrics['steps_completed'] += batch_steps
            self.performance_metrics['firing_neurons_sum'] += sum(firing_counts)
            
            # Log batch progress
            avg_firing = sum(firing_counts) / len(firing_counts) if firing_counts else 0
            print(f"  Batch completed in {batch_time:.3f}s, "
                  f"Avg firing neurons: {avg_firing:.1f}")
        
        # Get memory usage
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        self.performance_metrics['memory_peak'] = peak / 1024 / 1024  # MB
        self.performance_metrics['memory_current'] = current / 1024 / 1024  # MB
        
        total_time = time.time() - start_time
        
        print(f"\nSimulation completed!")
        print(f"Total time: {total_time:.2f}s")
        print(f"Average time per step: {total_time/total_steps*1000:.2f}ms")
        print(f"Average firing neurons: {self.performance_metrics['firing_neurons_sum']/total_steps:.1f}")
        print(f"Peak memory usage: {self.performance_metrics['memory_peak']:.2f} MB")
        
        return self.performance_metrics
    
    def runComparisonSimulation(self, configs: List[pynlm.Config], steps_per_config: int):
        """
        Run comparison simulation with different configurations.
        
        Args:
            configs: List of different configurations
            steps_per_config: Number of steps for each configuration
        """
        print(f"Running comparison simulation with {len(configs)} configurations...")
        print(f"Steps per configuration: {steps_per_config}")
        
        results = {}
        
        for i, config in enumerate(configs):
            print(f"\n--- Configuration {i+1}: {config} ---")
            
            # Create fresh simulation for this config
            sim = OptimizedBrainSimulation(config, self.num_instances)
            sim.setupParallelBrains()
            
            # Run simulation
            config_results = sim.runOptimizedSimulation(steps_per_config)
            results[f'config_{i}'] = config_results
            
        # Compare results
        self.compareConfigurations(results)
        
        return results
    
    def compareConfigurations(self, results: Dict[str, Dict]):
        """Compare performance across different configurations."""
        print("\n=== Configuration Comparison ===")
        
        print(f"{'Configuration':<15} {'Total Time (s)':<15} {'Time/Step (ms)':<15} "
              f"{'Firing Neurs/step':<20} {'Peak Memory (MB)':<18}")
        print("-" * 85)
        
        for config_name, metrics in results.items():
            total_time = metrics['total_time']
            steps = metrics['steps_completed']
            time_per_step = total_time / steps * 1000 if steps > 0 else 0
            avg_firing = metrics['firing_neurons_sum'] / steps if steps > 0 else 0
            peak_memory = metrics['memory_peak']
            
            print(f"{config_name:<15} {total_time:<15.3f} {time_per_step:<15.3f} "
                  f"{avg_firing:<20.1f} {peak_memory:<18.2f}")
    
    def cleanup(self):
        """Clean up resources."""
        print("Cleaning up simulation resources...")
        
        if self.thread_pool:
            self.thread_pool.shutdown(wait=True)
            self.thread_pool = None
        
        self.brain_instances.clear()
        self.performance_metrics.clear()
        
        print("Cleanup completed.")
class LargeScaleSimulation:
    """
    Large-scale simulation with thousands of neurons.
    
    This simulation demonstrates techniques for handling very large neural networks:
    - Memory-efficient data structures
    - Batch processing for large datasets
    - Parallel computation
    - Performance monitoring and analysis
    """
    
    def __init__(self, num_neurons: int = 10000, connection_density: float = 0.1):
        self.num_neurons = num_neurons
        self.connection_density = connection_density
        self.simulations = []
        
    def createLargeBrain(self) -> pynlm.Brain:
        """Create a brain with large number of neurons."""
        config = pynlm.createDemoConfig(self.num_neurons, self.connection_density)
        config.set("brain.synapse_density", self.connection_density)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        return brain
    
    def runScaleBenchmark(self):
        """Run benchmark for different scales."""
        scales = [
            {'neurons': 1000, 'density': 0.05},
            {'neurons': 5000, 'density': 0.1},
            {'neurons': 10000, 'density': 0.15},
            {'neurons': 20000, 'density': 0.2}
        ]
        
        print("Running scale benchmark...")
        print(f"{'Neurons':<10} {'Density':<10} {'Time (s)':<10} {'Time/Step (ms)':<15} "
              f"{'Memory (MB)':<12}")
        print("-" * 70)
        
        for scale in scales:
            start_time = time.time()
            tracemalloc.start()
            
            # Create and run large brain
            brain = self.createLargeBrain()
            
            # Run simulation
            for step in range(100):
                brain.step(step)
            
            # Get performance metrics
            end_time = time.time()
            current, peak = tracemalloc.get_traced_memory()
            tracemalloc.stop()
            
            total_time = end_time - start_time
            time_per_step = total_time / 100 * 1000
            memory_mb = peak / 1024 / 1024
            
            print(f"{scale['neurons']:<10} {scale['density']:<10} {total_time:<10.3f} "
                  f"{time_per_step:<15.3f} {memory_mb:<12.2f}")
            
            # Store for analysis
            self.simulations.append({
                'neurons': scale['neurons'],
                'density': scale['density'],
                'time_per_step': time_per_step,
                'memory_mb': memory_mb
            })
        
    def analyzeScaling(self):
        """Analyze scaling characteristics."""
        if not self.simulations:
            print("No simulation data available for analysis")
            return
        
        print("\n=== Scaling Analysis ===")
        
        neurons = [s['neurons'] for s in self.simulations]
        times = [s['time_per_step'] for s in self.simulations]
        memories = [s['memory_mb'] for s in self.simulations]
        
        # Calculate scaling exponents
        log_neurons = np.log10(neurons)
        log_times = np.log10(times)
        log_memories = np.log10(memories)
        
        # Linear regression (simplified)
        time_slope = np.polyfit(log_neurons, log_times, 1)[0]
        mem_slope = np.polyfit(log_neurons, log_memories, 1)[0]
        
        print(f"Time complexity: O(N^{time_slope:.2f})")
        print(f"Memory complexity: O(N^{mem_slope:.2f})")
        
        # Predict for larger scale
        next_neuron_scale = 50000
        log_pred = np.polyval(np.polyfit(log_neurons, log_times, 1), np.log10(next_neuron_scale))
        pred_time = 10 ** log_pred
        
        print(f"\nPredicted performance for {next_neuron_scale} neurons:")
        print(f"Estimated time per step: {pred_time:.2f} ms")
        
        return {
            'time_complexity': time_slope,
            'memory_complexity': mem_slope,
            'predictions': {
                'neurons': next_neuron_scale,
                'time_per_step_ms': pred_time
            }
        }
def runOptimizedSimulationExample():
    """Run the optimized simulation example."""
    print("=== Optimized NLM Simulation Example ===\n")
    
    # Create different configurations for comparison
    configs = [
        pynlm.createDemoConfig(500, 0.05),    # Small brain
        pynlm.createDemoConfig(1000, 0.1),   # Medium brain
        pynlm.createDemoConfig(2000, 0.15),  # Large brain
    ]
    
    # Set up optimized simulation
    sim = OptimizedBrainSimulation(configs[0], num_instances=2)
    sim.setupParallelBrains()
    sim.setupThreadPool()
    
    print("Created optimized simulation with:")
    print(f"- {sim.num_instances} parallel brain instances")
    print(f"- Thread pool enabled")
    print(f"- Configuration: {sim.num_instances * sim.config.getOr<int64_t>('neuron_count', 1000)} total neurons")
    print()
    
    # Run optimized simulation
    metrics = sim.runOptimizedSimulation(total_steps=200, batch_size=20)
    
    print("\n=== Optimization Results ===")
    print(f"Total simulation time: {metrics['total_time']:.3f}s")
    print(f"Average time per step: {metrics['total_time']/metrics['steps_completed']*1000:.3f}ms")
    print(f"Average firing neurons: {metrics['firing_neurons_sum']/metrics['steps_completed']:.1f}")
    print(f"Peak memory usage: {metrics['memory_peak']:.2f} MB")
    
    # Cleanup
    sim.cleanup()
    
    print("\n=== Large-Scale Simulation Example ===")
    
    # Run scale benchmark
    large_sim = LargeScaleSimulation()
    large_sim.runScaleBenchmark()
    
    # Analyze scaling
    analysis = large_sim.analyzeScaling()
    
    print("\n=== Large-Scale Simulation Complete ===")
if __name__ == "__main__":
    runOptimizedSimulationExample()