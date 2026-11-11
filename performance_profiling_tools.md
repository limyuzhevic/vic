# Performance Profiling and Benchmarking Tools for NLM

This document provides examples of performance profiling tools and benchmarks for the NLM project.

## Overview

Performance profiling and benchmarking are essential for:
- Identifying bottlenecks in neural simulations
- Comparing performance across different configurations
- Monitoring resource utilization
- Optimizing simulation parameters

## Profiling Tools

### 1. Basic Performance Profiler

```python
import pynlm
import time
import statistics
from contextlib import contextmanager

class NLMProfiler:
    """Performance profiler for NLM simulations."""
    
    def __init__(self):
        self.metrics = {
            'step_times': [],
            'neuron_count': 0,
            'memory_usage': [],
            'cpu_usage': [],
            'simulation_time': 0.0
        }
        self.start_time = None
        self.end_time = None
    
    def start_profiling(self):
        """Start performance profiling."""
        self.start_time = time.time()
        self.metrics['start_time'] = self.start_time
    
    def stop_profiling(self):
        """Stop performance profiling."""
        self.end_time = time.time()
        self.metrics['end_time'] = self.end_time
        self.metrics['simulation_time'] = self.end_time - self.start_time
    
    @contextmanager
    def profile_simulation(self):
        """Context manager for profiling simulation."""
        self.start_profiling()
        try:
            yield self
        finally:
            self.stop_profiling()
    
    def record_step_metrics(self, step_number, brain, agent=None):
        """Record metrics for a simulation step."""
        step_start = time.time()
        
        # Record key metrics
        metrics = {
            'step': step_number,
            'neurons_firing': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'memory_usage': brain.getMemoryStats(),
            'development_stage': brain.getDevelopmentalStage(),
            'curiosity_level': agent.getCuriosityLevel() if agent else 0.0,
            'novelty_level': agent.getNoveltyLevel() if agent else 0.0,
            'prediction_error': agent.getPredictionError() if agent else 0.0
        }
        
        step_end = time.time()
        metrics['step_time_ms'] = (step_end - step_start) * 1000
        metrics['step_time_s'] = step_end - step_start
        
        self.metrics['step_times'].append(metrics)
        return metrics
    
    def get_performance_summary(self):
        """Get performance summary statistics."""
        if not self.metrics['step_times']:
            return None
        
        step_times = [m['step_time_s'] for m in self.metrics['step_times']]
        
        return {
            'total_steps': len(self.metrics['step_times']),
            'total_simulation_time': self.metrics['simulation_time'],
            'average_step_time': statistics.mean(step_times),
            'median_step_time': statistics.median(step_times),
            'min_step_time': min(step_times),
            'max_step_time': max(step_times),
            'std_step_time': statistics.stdev(step_times) if len(step_times) > 1 else 0.0,
            'steps_per_second': len(self.metrics['step_times']) / self.metrics['simulation_time'] if self.metrics['simulation_time'] > 0 else 0.0,
            'neurons_firing_avg': statistics.mean([m['neurons_firing'] for m in self.metrics['step_times']]),
            'total_spikes_per_step': statistics.mean([m['total_spikes'] for m in self.metrics['step_times']]),
            'average_firing_rate': statistics.mean([m['average_firing_rate'] for m in self.metrics['step_times']])
        }
    
    def export_metrics(self, filename):
        """Export profiling metrics to file."""
        import json
        with open(filename, 'w') as f:
            json.dump(self.metrics, f, indent=2)

# Example usage
if __name__ == "__main__":
    # Create NLM instance
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    
    # Create profiler
    profiler = NLMProfiler()
    
    # Run simulation with profiling
    with profiler.profile_simulation():
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
            agent.applyRewardModulation(world.getSensoryPercept().getInternal()[0], 0.0)
            
            # Record metrics every 10 steps
            if step % 10 == 0:
                profiler.record_step_metrics(step, brain, agent)
    
    # Get and display performance summary
    summary = profiler.get_performance_summary()
    if summary:
        print("=== NLM Performance Summary ===")
        print(f"Total Steps: {summary['total_steps']}")
        print(f"Simulation Time: {summary['total_simulation_time']:.2f} seconds")
        print(f"Average Step Time: {summary['average_step_time']*1000:.2f} ms")
        print(f"Steps per Second: {summary['steps_per_second']:.2f}")
        print(f"Average Neurons Firing: {summary['neurons_firing_avg']:.1f}")
        print(f"Average Total Spikes per Step: {summary['total_spikes_per_step']:.1f}")
    
    # Export metrics
    profiler.export_metrics("nlm_performance_metrics.json")
    print("Metrics exported to nlm_performance_metrics.json")
```

### 2. Memory Profiling Tool

```python
import tracemalloc
import pynlm
import time

class MemoryProfiler:
    """Memory profiler for NLM simulations."""
    
    def __init__(self):
        self.memory_snapshots = []
        self.memory_peaks = []
    
    def start_memory_tracing(self):
        """Start memory tracing."""
        tracemalloc.start()
    
    def stop_memory_tracing(self):
        """Stop memory tracing."""
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        return current / 1024 / 1024, peak / 1024 / 1024  # Convert to MB
    
    def take_memory_snapshot(self, label=""):
        """Take a memory snapshot."""
        if tracemalloc.is_tracing():
            snapshot = tracemalloc.take_snapshot()
            self.memory_snapshots.append((label, snapshot))
    
    @contextmanager
    def memory_profile_simulation(self, brain, agent, world):
        """Context manager for memory profiling."""
        self.start_memory_tracing()
        
        # Take initial snapshot
        self.take_memory_snapshot("Initial")
        
        try:
            yield self
        finally:
            # Take final snapshot
            current_mem, peak_mem = self.stop_memory_tracing()
            self.take_memory_snapshot(f"Final - Current: {current_mem:.2f}MB, Peak: {peak_mem:.2f}MB")
    
    def analyze_memory_usage(self, brain, agent, world, steps=1000):
        """Analyze memory usage over simulation steps."""
        self.start_memory_tracing()
        
        memory_usage = []
        
        for step in range(steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
            agent.applyRewardModulation(world.getSensoryPercept().getInternal()[0], 0.0)
            
            if step % 100 == 0:
                current, peak = tracemalloc.get_traced_memory()
                memory_usage.append({
                    'step': step,
                    'current_memory_mb': current / 1024 / 1024,
                    'peak_memory_mb': peak / 1024 / 1024
                })
        
        self.stop_memory_tracing()
        
        return memory_usage

# Example usage
if __name__ == "__main__":
    # Set up NLM
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Create memory profiler
    mem_profiler = MemoryProfiler()
    
    # Analyze memory usage
    memory_usage = mem_profiler.analyze_memory_usage(brain, agent, world, 500)
    
    print("=== Memory Usage Analysis ===")
    if memory_usage:
        print(f"Initial Memory: {memory_usage[0]['current_memory_mb']:.2f} MB")
        print(f"Peak Memory: {memory_usage[-1]['peak_memory_mb']:.2f} MB")
        print(f"Average Memory: {sum(m['current_memory_mb'] for m in memory_usage) / len(memory_usage):.2f} MB")
        print(f"Memory Growth: {memory_usage[-1]['current_memory_mb'] - memory_usage[0]['current_memory_mb']:.2f} MB")
```

### 3. Scalability Benchmarking

```python
import pynlm
import time
import statistics

class ScalabilityBenchmark:
    """Benchmark NLM scalability across different configurations."""
    
    def __init__(self):
        self.benchmark_results = {}
    
    def benchmark_neuron_scalability(self, neuron_counts, steps=100):
        """Benchmark performance across different neuron counts."""
        print("=== Neuron Count Scalability Benchmark ===")
        
        results = []
        
        for neuron_count in neuron_counts:
            print(f"\nBenchmarking with {neuron_count} neurons...")
            
            # Create configuration
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", neuron_count)
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Create agent and world
            agent = pynlm.createAgentBrain(brain)
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent.initialize(world)
            agent.enableRewardModulation(True)
            
            # Benchmark
            start_time = time.time()
            
            for step in range(steps):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
                agent.applyRewardModulation(world.getSensoryPercept().getInternal()[0], 0.0)
            
            end_time = time.time()
            elapsed_time = end_time - start_time
            
            result = {
                'neuron_count': neuron_count,
                'steps': steps,
                'elapsed_time': elapsed_time,
                'steps_per_second': steps / elapsed_time,
                'spikes_per_neuron': brain.getTotalSpikeCount() / neuron_count,
                'neurons_firing_avg': brain.getFiringNeuronCount(),
                'firing_rate_hz': brain.getAverageFiringRate()
            }
            
            results.append(result)
            
            print(f"  Time: {elapsed_time:.2f}s")
            print(f"  Steps/sec: {result['steps_per_second']:.2f}")
            print(f"  Spikes/neuron: {result['spikes_per_neuron']:.2f}")
            print(f"  Average firing rate: {result['firing_rate_hz']:.2f} Hz")
        
        self.benchmark_results['neuron_scalability'] = results
        return results
    
    def benchmark_connection_probability(self, probs, steps=100):
        """Benchmark performance across different connection probabilities."""
        print("\n=== Connection Probability Benchmark ===")
        
        results = []
        
        for prob in probs:
            print(f"\nBenchmarking with connection probability {prob:.3f}...")
            
            # Create configuration
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 1000)
            config.set("connection_probability", prob)
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Create agent and world
            agent = pynlm.createAgentBrain(brain)
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent.initialize(world)
            agent.enableRewardModulation(True)
            
            # Benchmark
            start_time = time.time()
            
            for step in range(steps):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
                agent.applyRewardModulation(world.getSensoryPercept().getInternal()[0], 0.0)
            
            end_time = time.time()
            elapsed_time = end_time - start_time
            
            result = {
                'connection_probability': prob,
                'steps': steps,
                'elapsed_time': elapsed_time,
                'steps_per_second': steps / elapsed_time,
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'neurons_firing_avg': brain.getFiringNeuronCount(),
                'firing_rate_hz': brain.getAverageFiringRate()
            }
            
            results.append(result)
            
            print(f"  Time: {elapsed_time:.2f}s")
            print(f"  Steps/sec: {result['steps_per_second']:.2f}")
            print(f"  Total synapses: {result['total_synapses']}")
            print(f"  Average firing rate: {result['firing_rate_hz']:.2f} Hz")
        
        self.benchmark_results['connection_probability'] = results
        return results
    
    def benchmark_subsystem_combinations(self, combinations, steps=100):
        """Benchmark different subsystem activation combinations."""
        print("\n=== Subsystem Combination Benchmark ===")
        
        results = []
        
        for name, subsystem_flags in combinations.items():
            print(f"\nBenchmarking subsystem combination: {name}")
            
            # Create configuration
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 500)
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Create agent and world
            agent = pynlm.createAgentBrain(brain)
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent.initialize(world)
            
            # Apply subsystem flags
            agent.enableRewardModulation(subsystem_flags.get('reward', False))
            agent.enableStructuralPlasticity(subsystem_flags.get('structural', False))
            agent.enableDevelopment(subsystem_flags.get('development', False))
            agent.enableCuriosity(subsystem_flags.get('curiosity', False))
            
            # Benchmark
            start_time = time.time()
            
            for step in range(steps):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
                agent.applyRewardModulation(world.getSensoryPercept().getInternal()[0], 0.0)
                agent.updateDevelopment(0.1)
            
            end_time = time.time()
            elapsed_time = end_time - start_time
            
            result = {
                'combination_name': name,
                'steps': steps,
                'elapsed_time': elapsed_time,
                'steps_per_second': steps / elapsed_time,
                'reward_modulation': subsystem_flags.get('reward', False),
                'structural_plasticity': subsystem_flags.get('structural', False),
                'development': subsystem_flags.get('development', False),
                'curiosity': subsystem_flags.get('curiosity', False),
                'final_curiosity': agent.getCuriosityLevel(),
                'final_novelty': agent.getNoveltyLevel(),
                'total_spikes': brain.getTotalSpikeCount()
            }
            
            results.append(result)
            
            print(f"  Time: {elapsed_time:.2f}s")
            print(f"  Steps/sec: {result['steps_per_second']:.2f}")
            print(f"  Final curiosity: {result['final_curiosity']:.3f}")
            print(f"  Total spikes: {result['total_spikes']}")
        
        self.benchmark_results['subsystem_combinations'] = results
        return results
    
    def generate_benchmark_report(self):
        """Generate comprehensive benchmark report."""
        print("\n=== NLM Benchmark Report ===")
        
        for benchmark_type, results in self.benchmark_results.items():
            print(f"\n{benchmark_type.upper().replace('_', ' ')}:")
            
            if benchmark_type == 'neuron_scalability':
                print(f"{'Neurons':<10} {'Time(s)':<10} {'Steps/sec':<12} {'Spikes/Neuron':<15} {'Avg Firing Rate (Hz)':<20}")
                print("-" * 70)
                for r in results:
                    print(f"{r['neuron_count']:<10} {r['elapsed_time']:<10.2f} {r['steps_per_second']:<12.2f} {r['spikes_per_neuron']:<15.2f} {r['firing_rate_hz']:<20.2f}")
            
            elif benchmark_type == 'connection_probability':
                print(f"{'Prob':<10} {'Time(s)':<10} {'Steps/sec':<12} {'Total Synapses':<15} {'Avg Firing Rate (Hz)':<20}")
                print("-" * 70)
                for r in results:
                    print(f"{r['connection_probability']:<10.3f} {r['elapsed_time']:<10.2f} {r['steps_per_second']:<12.2f} {r['total_synapses']:<15} {r['firing_rate_hz']:<20.2f}")
            
            elif benchmark_type == 'subsystem_combinations':
                print(f"{'Combination':<20} {'Time(s)':<10} {'Steps/sec':<12} {'Final Curiosity':<15} {'Total Spikes':<12}")
                print("-" * 70)
                for r in results:
                    print(f"{r['combination_name']:<20} {r['elapsed_time']:<10.2f} {r['steps_per_second']:<12.2f} {r['final_curiosity']:<15.3f} {r['total_spikes']:<12}")

# Example usage
if __name__ == "__main__":
    # Create benchmark
    benchmark = ScalabilityBenchmark()
    
    # Run benchmarks
    neuron_counts = [100, 500, 1000, 2000]
    neuron_results = benchmark.benchmark_neuron_scalability(neuron_counts, 200)
    
    connection_probs = [0.01, 0.05, 0.1, 0.2]
    connection_results = benchmark.benchmark_connection_probability(connection_probs, 200)
    
    subsystem_combinations = {
        'minimal': {'reward': False, 'structural': False, 'development': False, 'curiosity': False},
        'reward_only': {'reward': True, 'structural': False, 'development': False, 'curiosity': False},
        'development_only': {'reward': False, 'structural': False, 'development': True, 'curiosity': False},
        'full': {'reward': True, 'structural': True, 'development': True, 'curiosity': True}
    }
    
    subsystem_results = benchmark.benchmark_subsystem_combinations(subsystem_combinations, 200)
    
    # Generate report
    benchmark.generate_benchmark_report()
```

## Integration with NLM CLI

The profiling tools can be integrated with the NLM command-line interface:

```bash
# Run performance profiling
./nlm_benchmark --profile --output benchmark_results.json

# Run memory profiling
./nlm_benchmark --memory-profile --output memory_usage.txt

# Run scalability tests
./nlm_benchmark --scalability --neuron-counts 100,500,1000 --steps 1000

# Run subsystem combination tests
./nlm_benchmark --subsystem-benchmark --combinations minimal,full
```

## Configuration File for Profiling

Create a `profile_config.json` file to configure profiling options:

```json
{
    "profiler": {
        "enable_memory_profiling": true,
        "enable_cpu_profiling": true,
        "profile_interval": 100,
        "export_format": "json"
    },
    "benchmark": {
        "default_steps": 1000,
        "neuron_counts": [100, 500, 1000, 2000],
        "connection_probabilities": [0.01, 0.05, 0.1, 0.2],
        "enable_subsystem_combinations": true
    }
}
```

## Summary

These profiling tools provide comprehensive performance analysis capabilities for the NLM project:

1. **Performance Profiling**: Detailed timing metrics and bottleneck identification
2. **Memory Profiling**: Memory usage analysis and leak detection
3. **Scalability Benchmarking**: Performance comparison across different configurations
4. **Subsystem Analysis**: Impact analysis of different subsystem combinations
5. **Integration**: Easy integration with existing NLM build system and CLI

These tools will help researchers and developers optimize NLM simulations, identify performance bottlenecks, and make informed decisions about configuration and parameter tuning.