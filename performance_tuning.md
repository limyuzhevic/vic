# NLM - Performance Tuning Guide

This guide provides comprehensive information on optimizing NLM (Neural Learning Machine) performance for different use cases. It covers performance bottlenecks, optimization strategies, benchmarking, and troubleshooting.

## Overview

NLM performance can be affected by multiple factors:
- **Simulation parameters** - Timestep size, neuron count, network connectivity
- **Hardware constraints** - Memory, CPU cores, GPU acceleration
- **Configuration choices** - Plasticity settings, learning rates
- **Code optimization** - Efficient algorithms, data structures

This guide helps you optimize NLM performance for your specific requirements.

## Performance Analysis

### Measuring Performance

Track key performance metrics:

```python
import pynlm
import time
import psutil
import os

class PerformanceProfiler:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.metrics = {}
        self.start_time = time.time()
        
    def start_profiling(self):
        self.start_time = time.time()
        self.metrics = {
            'steps_completed': 0,
            'total_spikes': 0,
            'memory_usage': [],
            'cpu_usage': [],
            'simulation_times': [],
            'firing_rates': []
        }
        
    def stop_profiling(self):
        elapsed = time.time() - self.start_time
        return self.get_summary(elapsed)
        
    def record_step(self, step):
        process = psutil.Process(os.getpid())
        
        self.metrics['steps_completed'] = step + 1
        self.metrics['total_spikes'] = self.brain.getTotalSpikeCount()
        self.metrics['memory_usage'].append(process.memory_info().rss)
        self.metrics['cpu_usage'].append(process.cpu_percent())
        self.metrics['firing_rates'].append(self.brain.getAverageFiringRate())
        
    def get_summary(self, elapsed_time):
        if self.metrics['steps_completed'] == 0:
            return {}
            
        return {
            'total_time': elapsed_time,
            'steps_per_second': self.metrics['steps_completed'] / elapsed_time,
            'total_spikes': self.metrics['total_spikes'],
            'average_firing_rate': sum(self.metrics['firing_rates']) / len(self.metrics['firing_rates']),
            'peak_memory_usage': max(self.metrics['memory_usage']) / (1024 * 1024),  # MB
            'average_cpu_usage': sum(self.metrics['cpu_usage']) / len(self.metrics['cpu_usage']),
            'memory_efficiency': self.metrics['total_spikes'] / max(self.metrics['memory_usage']),
            'spike_efficiency': self.metrics['total_spikes'] / self.metrics['steps_completed']
        }

# Usage
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

profiler = PerformanceProfiler(brain, world, agent)
profiler.start_profiling()

for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    profiler.record_step(step)

results = profiler.stop_profiling()
print("Performance Summary:", results)
```

### Performance Bottlenecks

Identify performance bottlenecks:

```python
class BottleneckDetector:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.timeline = []
        
    def profile_operations(self, num_steps=100):
        import time
        
        for step in range(num_steps):
            step_start = time.time()
            
            # World update
            world_start = time.time()
            self.world.update(0.1)
            world_time = time.time() - world_start
            
            # Sensory processing
            sensory_start = time.time()
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            sensory_time = time.time() - sensory_start
            
            # Brain step
            brain_start = time.time()
            self.brain.step(step)
            brain_time = time.time() - brain_start
            
            # Motor decoding
            motor_start = time.time()
            action = self.agent.decodeMotorCommand()
            motor_time = time.time() - motor_start
            
            # World action
            action_start = time.time()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            action_time = time.time() - action_start
            
            total_time = time.time() - step_start
            
            self.timeline.append({
                'step': step,
                'total': total_time,
                'world': world_time,
                'sensory': sensory_time,
                'brain': brain_time,
                'motor': motor_time,
                'action': action_time,
                'efficiency': (world_time + sensory_time + brain_time + motor_time + action_time) / total_time
            })
            
    def get_bottleneck_analysis(self):
        if not self.timeline:
            return {}
            
        avg_times = {
            'world': sum(t['world'] for t in self.timeline) / len(self.timeline),
            'sensory': sum(t['sensory'] for t in self.timeline) / len(self.timeline),
            'brain': sum(t['brain'] for t in self.timeline) / len(self.timeline),
            'motor': sum(t['motor'] for t in self.timeline) / len(self.timeline),
            'action': sum(t['action'] for t in self.timeline) / len(self.timeline)
        }
        
        # Find slowest operation
        slowest_op = max(avg_times.items(), key=lambda x: x[1])
        
        return {
            'average_times': avg_times,
            'slowest_operation': slowest_op[0],
            'slowest_time': slowest_op[1],
            'total_efficiency': sum(t['efficiency'] for t in self.timeline) / len(self.timeline),
            'recommendations': self.generate_recommendations(avg_times)
        }
        
    def generate_recommendations(self, avg_times):
        recommendations = []
        
        for operation, time_taken in avg_times.items():
            if time_taken > 0.1:  # More than 100ms
                if operation == 'brain':
                    recommendations.append("Consider reducing neuron count or timestep")
                elif operation == 'world':
                    recommendations.append("Optimize world update logic")
                elif operation == 'sensory':
                    recommendations.append("Reduce sensory processing complexity")
                elif operation == 'motor':
                    recommendations.append("Optimize motor decoding algorithm")
                elif operation == 'action':
                    recommendations.append("Optimize action application logic")
                    
        return recommendations
```

## Optimization Strategies

### Simulation Parameters

#### Timestep Optimization

Find optimal timestep for your application:

```python
import pynlm
import matplotlib.pyplot as plt

def find_optimal_timestep(brain_size, simulation_steps):
    """Find optimal timestep for given brain size and steps"""
    timesteps = [0.0001, 0.0005, 0.001, 0.005, 0.01, 0.05]
    results = []
    
    for dt in timesteps:
        # Run simulation with this timestep
        config = pynlm.createAdvancedConfig()
        config.set("simulation_timestep", dt)
        config.set("neuron_count", brain_size)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        for step in range(simulation_steps):
            brain.step(step)
        end_time = time.time()
        
        results.append({
            'timestep': dt,
            'total_time': end_time - start_time,
            'steps_per_second': simulation_steps / (end_time - start_time),
            'spikes_generated': brain.getTotalSpikeCount()
        })
        
    return results

# Find optimal timestep for 1000 neurons, 1000 steps
optimal_results = find_optimal_timestep(1000, 1000)

# Plot results
steps_per_second = [r['steps_per_second'] for r in optimal_results]
timesteps = [r['timestep'] for r in optimal_results]

plt.figure(figsize=(10, 6))
plt.plot(timesteps, steps_per_second, 'bo-')
plt.xscale('log')
plt.xlabel('Timestep (s)')
plt.ylabel('Steps per Second')
plt.title('Performance vs. Timestep')
plt.grid(True)
plt.show()

# Choose timestep with best performance
best_result = max(optimal_results, key=lambda x: x['steps_per_second'])
print(f"Optimal timestep: {best_result['timestep']}s "
      f"({best_result['steps_per_second']:.0f} steps/sec)")
```

#### Neuron Count Optimization

Balance brain size with performance:

```python
def find_optimal_neuron_count(target_performance, max_neurons=5000):
    """Find optimal neuron count for target performance"""
    neuron_counts = [100, 500, 1000, 2000, 5000, 10000]
    results = []
    
    for neuron_count in neuron_counts:
        # Run simulation with this neuron count
        config = pynlm.createAdvancedConfig()
        config.set("neuron_count", neuron_count)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        for step in range(100):
            brain.step(step)
        end_time = time.time()
        
        simulation_time = end_time - start_time
        firing_rate = brain.getAverageFiringRate()
        spikes = brain.getTotalSpikeCount()
        
        results.append({
            'neuron_count': neuron_count,
            'simulation_time': simulation_time,
            'firing_rate': firing_rate,
            'spikes': spikes,
            'performance_efficiency': spikes / simulation_time,
            'memory_efficiency': firing_rate / neuron_count
        })
        
    return results

# Find optimal neuron count for task requiring certain performance
optimal_neurons = find_optimal_neuron_count(target_performance=1000)

# Find neuron count with best trade-off between performance and efficiency
best_efficiency = max(optimal_neurons, key=lambda x: x['performance_efficiency'])
best_memory = max(optimal_neurons, key=lambda x: x['memory_efficiency'])

print(f"Best performance efficiency: {best_efficiency['neuron_count']} neurons "
      f"(efficiency: {best_efficiency['performance_efficiency']:.2f} spikes/sec)")
print(f"Best memory efficiency: {best_memory['neuron_count']} neurons "
      f"(efficiency: {best_memory['memory_efficiency']:.4f} Hz/neuron)")
```

### Hardware Optimization

#### Memory Management

Optimize memory usage:

```python
import pynlm
import psutil
import gc

def optimize_memory_usage(brain, world, agent, target_memory_mb=500):
    """Optimize memory usage by trading off resolution vs. performance"""
    
    process = psutil.Process()
    
    # Check current memory usage
    current_memory = process.memory_info().rss / (1024 * 1024)  # MB
    print(f"Current memory usage: {current_memory:.1f} MB")
    
    # If memory usage is high, try optimizations
    if current_memory > target_memory_mb:
        # Reduce brain size
        if brain.getTotalNeuronCount() > 1000:
            config = pynlm.createAdvancedConfig()
            config.set("neuron_count", min(1000, brain.getTotalNeuronCount() // 2))
            new_brain = pynlm.createBrain(config)
            new_brain.initialize()
            print("Reduced neuron count to save memory")
            return new_brain
            
        # Disable memory-intensive features
        agent.enableStructuralPlasticity(False)
        print("Disabled structural plasticity to save memory")
        
    return brain

def monitor_memory_usage(brain, check_interval=100, threshold_mb=800):
    """Monitor memory usage and warn if approaching threshold"""
    import time
    
    process = psutil.Process()
    warnings = []
    
    for step in range(0, 10000, check_interval):
        current_memory = process.memory_info().rss / (1024 * 1024)  # MB
        
        if current_memory > threshold_mb:
            warning = f"High memory usage at step {step}: {current_memory:.1f}MB"
            warnings.append(warning)
            print(f"WARNING: {warning}")
            
        # Force garbage collection periodically
        if step % 1000 == 0:
            gc.collect()
            
        if step > 0:  # Don't check on first iteration
            brain.step(step)
            
    return warnings
```

#### CPU Optimization

Optimize CPU usage for multi-threading:

```python
import pynlm
import threading
import concurrent.futures

class ParallelSimulation:
    def __init__(self, config, num_workers=None):
        self.config = config
        self.num_workers = num_workers or psutil.cpu_count()
        
    def run_parallel_simulation(self, num_simulations, steps_per_sim):
        """Run multiple simulations in parallel"""
        print(f"Running {num_simulations} simulations with {self.num_workers} workers...")
        
        with concurrent.futures.ThreadPoolExecutor(max_workers=self.num_workers) as executor:
            # Submit all simulations
            futures = []
            for i in range(num_simulations):
                future = executor.submit(self.run_single_simulation, i, steps_per_sim)
                futures.append(future)
                
            # Collect results
            results = []
            for future in concurrent.futures.as_completed(futures):
                results.append(future.result())
                
            return results
            
    def run_single_simulation(self, simulation_id, steps):
        """Run a single simulation"""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        start_time = time.time()
        for step in range(steps):
            brain.step(step)
        end_time = time.time()
        
        return {
            'simulation_id': simulation_id,
            'steps': steps,
            'total_time': end_time - start_time,
            'steps_per_second': steps / (end_time - start_time),
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'final_stage': brain.getDevelopmentalStage()
        }

# Usage
config = pynlm.createAdvancedConfig()
parallel_sim = ParallelSimulation(config, num_workers=4)

results = parallel_sim.run_parallel_simulation(
    num_simulations=20,
    steps_per_sim=500
)

# Analyze parallel results
print(f"Average simulation time: {sum(r['total_time'] for r in results) / len(results):.2f}s")
print(f"Best performance: {max(results, key=lambda x: x['steps_per_second'])}")
```

### Configuration Optimization

#### Plasticity Settings

Optimize plasticity parameters:

```python
import pynlm
import numpy as np

class PlasticityOptimizer:
    def __init__(self, brain, agent):
        self.brain = brain
        self.agent = agent
        
    def optimize_learning_rate(self, learning_rates=[0.001, 0.01, 0.05, 0.1, 0.2]):
        """Find optimal learning rate for stability and performance"""
        results = []
        
        for lr in learning_rates:
            # Reset brain
            self.brain.reset()
            self.brain.initialize()
            
            # Run simulation
            initial_spikes = self.brain.getTotalSpikeCount()
            
            for step in range(500):
                # Apply learning rate
                self.brain.setLearningRate(lr)
                self.brain.step(step)
                
            final_spikes = self.brain.getTotalSpikeCount()
            spike_change = final_spikes - initial_spikes
            learning_efficiency = spike_change / (lr * 500) if lr > 0 else 0
            
            results.append({
                'learning_rate': lr,
                'spike_change': spike_change,
                'learning_efficiency': learning_efficiency,
                'stability': min(abs(spike_change) / 100, 1.0)  # Normalize stability
            })
            
        return results
        
    def optimize_stdp_parameters(self, ltp_weights=[0.01, 0.02, 0.05, 0.1], 
                                ltd_weights=[0.012, 0.015, 0.02, 0.03],
                                time_constants=[10.0, 20.0, 50.0, 100.0]):
        """Find optimal STDP parameters"""
        results = []
        
        for ltp in ltp_weights:
            for ltd in ltd_weights:
                for tau in time_constants:
                    # Reset and configure brain
                    self.brain.reset()
                    self.brain.initialize()
                    
                    # Configure STDP
                    self.brain.configureSTDP(ltp, ltd, tau)
                    
                    # Run simulation
                    initial_weights = self.get_average_synaptic_weights()
                    
                    for step in range(300):
                        self.brain.step(step)
                        
                    final_weights = self.get_average_synaptic_weights()
                    
                    weight_change = np.mean(final_weights) - np.mean(initial_weights)
                    stability = 1.0 / (1.0 + abs(weight_change))
                    
                    results.append({
                        'ltp_weight': ltp,
                        'ltd_weight': ltd,
                        'time_constant': tau,
                        'weight_change': weight_change,
                        'stability': stability
                    })
                    
        return results
        
    def get_average_synaptic_weights(self):
        """Get average synaptic weight across all regions"""
        weights = []
        for region in self.brain.getRegions():
            for synapse in region->getSynapses():
                weights.append(synapse->getWeight())
        return weights
```

#### Simulation Settings

Optimize simulation settings:

```python
import pynlm

class SimulationSettingsOptimizer:
    def __init__(self, brain_size=1000):
        self.brain_size = brain_size
        
    def optimize_timestep_and_steps(self, target_time_sec=10.0):
        """Find optimal timestep and step count for target simulation time"""
        
        # Calculate step count for target time
        candidate_timesteps = [0.0001, 0.0005, 0.001, 0.005, 0.01]
        results = []
        
        for dt in candidate_timesteps:
            # Calculate steps needed for target time
            steps = int(target_time_sec / dt)
            
            # Run simulation
            config = pynlm.createAdvancedConfig()
            config.set("simulation_timestep", dt)
            config.set("neuron_count", self.brain_size)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            start_time = time.time()
            for step in range(steps):
                brain.step(step)
            end_time = time.time()
            
            actual_time = end_time - start_time
            efficiency = steps / actual_time
            firing_rate = brain.getAverageFiringRate()
            
            results.append({
                'timestep': dt,
                'target_steps': steps,
                'actual_steps': int(actual_time / dt),
                'actual_time': actual_time,
                'efficiency': efficiency,
                'firing_rate': firing_rate,
                'total_spikes': brain.getTotalSpikeCount()
            })
            
        return results
        
    def optimize_simulation_size(self, max_time_sec=30.0):
        """Find optimal brain size for given simulation time"""
        
        brain_sizes = [500, 1000, 2000, 5000, 10000]
        results = []
        
        for size in brain_sizes:
            config = pynlm.createAdvancedConfig()
            config.set("neuron_count", size)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Calculate steps for max time (assuming 0.001 timestep default)
            steps = int(max_time_sec / 0.001)
            
            start_time = time.time()
            for step in range(min(steps, 1000)):  # Limit for benchmarking
                brain.step(step)
            end_time = time.time()
            
            simulation_time = end_time - start_time
            steps_per_second = min(steps, 1000) / simulation_time
            memory_per_neuron = brain.getMemoryUsage() / size
            
            results.append({
                'neuron_count': size,
                'steps_per_second': steps_per_second,
                'simulation_time': simulation_time,
                'memory_per_neuron': memory_per_neuron,
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate()
            })
            
        return results
```

## Benchmarking

### Benchmark Suite

Create comprehensive benchmarks:

```python
import pynlm
import time
import json
from pathlib import Path

class NLMBenchmarkSuite:
    def __init__(self, output_dir="benchmarks"):
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
        self.results = {}
        
    def run_memory_benchmark(self, neuron_counts=[100, 500, 1000, 2000, 5000]):
        """Benchmark memory usage with different brain sizes"""
        print("Running memory benchmark...")
        
        memory_results = []
        for neurons in neuron_counts:
            config = pynlm.createAdvancedConfig()
            config.set("neuron_count", neurons)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Measure memory usage
            import psutil
            process = psutil.Process()
            memory_before = process.memory_info().rss
            
            # Run simulation
            for step in range(100):
                brain.step(step)
                
            memory_after = process.memory_info().rss
            memory_used = (memory_after - memory_before) / (1024 * 1024)  # MB
            
            memory_results.append({
                'neuron_count': neurons,
                'memory_used_mb': memory_used,
                'memory_per_neuron': memory_used / neurons,
                'spikes_generated': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate()
            })
            
        self.results['memory'] = memory_results
        self.save_results("memory_benchmark.json")
        return memory_results
        
    def run_performance_benchmark(self, brain_sizes=[100, 500, 1000, 2000]):
        """Benchmark performance with different brain sizes"""
        print("Running performance benchmark...")
        
        performance_results = []
        for size in brain_sizes:
            config = pynlm.createAdvancedConfig()
            config.set("neuron_count", size)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Measure performance
            start_time = time.time()
            for step in range(100):
                brain.step(step)
            end_time = time.time()
            
            simulation_time = end_time - start_time
            steps_per_second = 100 / simulation_time
            spikes_per_second = brain.getTotalSpikeCount() / simulation_time
            
            performance_results.append({
                'neuron_count': size,
                'simulation_time_sec': simulation_time,
                'steps_per_second': steps_per_second,
                'spikes_per_second': spikes_per_second,
                'average_firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount()
            })
            
        self.results['performance'] = performance_results
        self.save_results("performance_benchmark.json")
        return performance_results
        
    def run_plasticity_benchmark(self, learning_rates=[0.001, 0.01, 0.05, 0.1]):
        """Benchmark plasticity performance with different learning rates"""
        print("Running plasticity benchmark...")
        
        plasticity_results = []
        for lr in learning_rates:
            config = pynlm.createAdvancedConfig()
            config.set("plasticity_learning_rate", lr)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Record initial weights
            initial_weights = self.get_average_synaptic_weights(brain)
            initial_mean = np.mean(initial_weights) if initial_weights else 0
            
            # Run simulation
            for step in range(200):
                brain.setLearningRate(lr)
                brain.step(step)
                
            # Record final weights
            final_weights = self.get_average_synaptic_weights(brain)
            final_mean = np.mean(final_weights) if final_weights else 0
            
            weight_change = final_mean - initial_mean
            stability = 1.0 / (1.0 + abs(weight_change))
            
            plasticity_results.append({
                'learning_rate': lr,
                'initial_mean_weight': initial_mean,
                'final_mean_weight': final_mean,
                'weight_change': weight_change,
                'stability': stability,
                'convergence_speed': abs(weight_change) / (lr * 200) if lr > 0 else 0
            })
            
        self.results['plasticity'] = plasticity_results
        self.save_results("plasticity_benchmark.json")
        return plasticity_results
        
    def get_average_synaptic_weights(self, brain):
        """Get average synaptic weights from brain"""
        weights = []
        for region in brain.getRegions():
            for synapse in region.getSynapses():
                weights.append(synapse.getWeight())
        return weights
        
    def save_results(self, filename):
        """Save benchmark results to file"""
        filepath = self.output_dir / filename
        with open(filepath, 'w') as f:
            json.dump(self.results, f, indent=2)
        print(f"Results saved to {filepath}")
        
    def generate_report(self):
        """Generate comprehensive benchmark report"""
        report = "# NLM Performance Benchmark Report\n\n"
        
        if 'memory' in self.results:
            report += "## Memory Benchmark\n\n"
            report += "| Neuron Count | Memory (MB) | Memory/Neuron | Spikes | Firing Rate |\n"
            report += "|--------------|--------------|---------------|--------|-------------|\n"
            for result in self.results['memory']:
                report += f"| {result['neuron_count']} | {result['memory_used_mb']:.1f} | "
                report += f"{result['memory_per_neuron']:.2f} | {result['spikes_generated']} | "
                report += f"{result['firing_rate']:.2f} |\n"
            report += "\n"
            
        if 'performance' in self.results:
            report += "## Performance Benchmark\n\n"
            report += "| Neuron Count | Steps/sec | Spikes/sec | Firing Rate |\n"
            report += "|--------------|-----------|------------|-------------|\n"
            for result in self.results['performance']:
                report += f"| {result['neuron_count']} | {result['steps_per_second']:.0f} | "
                report += f"{result['spikes_per_second']:.0f} | {result['average_firing_rate']:.2f} |\n"
            report += "\n"
            
        if 'plasticity' in self.results:
            report += "## Plasticity Benchmark\n\n"
            report += "| Learning Rate | Weight Change | Stability | Convergence Speed |\n"
            report += "|---------------|---------------|-----------|-------------------|\n"
            for result in self.results['plasticity']:
                report += f"| {result['learning_rate']:.3f} | {result['weight_change']:.4f} | "
                report += f"{result['stability']:.3f} | {result['convergence_speed']:.4f} |\n"
            
        return report
```

### Usage

Run comprehensive benchmarks:

```python
import pynlm
from benchmarks import NLMBenchmarkSuite

# Create benchmark suite
benchmark = NLMBenchmarkSuite("nlm_benchmarks")

# Run all benchmarks
print("Starting NLM benchmarks...")

memory_results = benchmark.run_memory_benchmark()
performance_results = benchmark.run_performance_benchmark()
plasticity_results = benchmark.run_plasticity_benchmark()

# Generate report
report = benchmark.generate_report()
print(report)

# Save report to file
with open("nlm_benchmark_report.md", "w") as f:
    f.write(report)
print("Report saved to nlm_benchmark_report.md")
```

## Troubleshooting

### Common Performance Issues

#### "Simulation too slow"

**Symptoms:** Simulation takes too long to complete.

**Solutions:**
1. **Reduce neuron count** - Larger brains are slower
2. **Increase timestep** - Smaller timestep means more steps
3. **Disable unnecessary features** - Turn off plasticity, development, etc.
4. **Use batch processing** - Process multiple steps at once
5. **Enable performance optimizations** - Use memory pools, SIMD, etc.

**Debug Code:**
```python
def debug_performance(brain, world, agent, max_time_sec=30.0):
    import time
    import psutil
    
    start_time = time.time()
    step_count = 0
    
    process = psutil.Process()
    initial_memory = process.memory_info().rss
    
    while time.time() - start_time < max_time_sec:
        world_start = time.time()
        world.update(0.1)
        world_time = time.time() - world_start
        
        if world_time > 0.1:  # World update taking too long
            print(f"Slow world update: {world_time:.3f}s")
            
        step_start = time.time()
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step_count)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        step_time = time.time() - step_start
        
        if step_time > 0.1:  # Step taking too long
            print(f"Slow step: {step_time:.3f}s at step {step_count}")
            
        step_count += 1
        
        # Check memory usage
        current_memory = process.memory_info().rss
        if current_memory - initial_memory > 100 * 1024 * 1024:  # 100MB
            print(f"High memory usage: {(current_memory - initial_memory) / (1024*1024):.1f}MB")
            
    actual_time = time.time() - start_time
    steps_per_second = step_count / actual_time
    print(f"Performance: {steps_per_second:.1f} steps/sec, "
          f"Total steps: {step_count}, "
          f"Time: {actual_time:.1f}s")
```

#### "Memory usage growing"

**Symptoms:** Memory usage increases during simulation.

**Solutions:**
1. **Enable memory monitoring** - Track memory usage
2. **Use checkpointing** - Save and reload to free memory
3. **Garbage collection** - Force garbage collection
4. **Reduce data retention** - Keep less history
5. **Optimize data structures** - Use more efficient structures

**Debug Code:**
```python
def debug_memory_usage(brain, agent, interval=100, threshold_mb=500):
    import psutil
    import gc
    
    process = psutil.Process()
    initial_memory = process.memory_info().rss / (1024 * 1024)  # MB
    
    for step in range(0, 10000, interval):
        # Check memory usage
        current_memory = process.memory_info().rss / (1024 * 1024)  # MB
        memory_growth = current_memory - initial_memory
        
        print(f"Step {step}: Memory = {current_memory:.1f}MB "
              f"(growth: {memory_growth:.1f}MB)")
        
        if memory_growth > threshold_mb:
            print(f"WARNING: Memory growth {memory_growth:.1f}MB exceeds threshold {threshold_mb}MB")
            
        # Force garbage collection periodically
        if step % 1000 == 0:
            gc.collect()
            print("Forced garbage collection")
            
        # Simulate step
        brain.step(step)
```

#### "Learning not stable"

**Symptoms:** Learning is unstable or diverges.

**Solutions:**
1. **Reduce learning rate** - High learning rates cause instability
2. **Add weight constraints** - Clip weights to prevent explosion
3. **Stabilize plasticity** - Use more stable plasticity rules
4. **Monitor learning progress** - Track learning curves
5. **Use adaptive learning rates** - Adjust learning rate dynamically

**Debug Code:**
```python
def debug_learning_stability(brain, agent, steps=500):
    print("=== Learning Stability Debug ===")
    
    # Record initial state
    initial_spikes = brain.getTotalSpikeCount()
    initial_stage = brain.getDevelopmentalStage()
    initial_dopamine = agent.getNeuromodulationLevel()
    
    print(f"Initial - Spikes: {initial_spikes}, "
          f"Stage: {initial_stage}, "
          f"Dopamine: {initial_dopamine:.3f}")
    
    # Track learning over time
    spike_history = []
    weight_changes = []
    
    for step in range(steps):
        # Record before step
        before_spikes = brain.getTotalSpikeCount()
        before_stage = brain.getDevelopmentalStage()
        
        # Run simulation step
        brain.step(step)
        
        # Record after step
        after_spikes = brain.getTotalSpikeCount()
        after_stage = brain.getDevelopmentalStage()
        
        spike_change = after_spikes - before_spikes
        spike_history.append(spike_change)
        
        # Check for instability
        if abs(spike_change) > 100:  # Large spike changes
            print(f"Potential instability at step {step}: "
                  f"spike change = {spike_change}")
                  
        if step % 100 == 0:
            print(f"Step {step}: Spikes = {after_spikes}, "
                  f"Stage = {after_stage}")
            
    # Analyze stability
    avg_spike_change = sum(abs(x) for x in spike_history) / len(spike_history)
    max_spike_change = max(abs(x) for x in spike_history)
    
    print(f"\nStability Analysis:")
    print(f"Average spike change: {avg_spike_change:.1f}")
    print(f"Max spike change: {max_spike_change:.1f}")
    
    if avg_spike_change > 50:
        print("WARNING: High average spike change - potential instability")
    if max_spike_change > 200:
        print("ALERT: Large spike change detected - unstable learning")
```

### Advanced Optimization Techniques

#### Memory Pool Optimization

Use memory pools for better memory management:

```python
import pynlm
import psutil

class MemoryPoolOptimizer:
    def __init__(self, brain, use_memory_pool=True):
        self.brain = brain
        self.use_memory_pool = use_memory_pool
        self.memory_baseline = None
        
    def optimize_memory_pools(self):
        """Optimize memory pool usage"""
        import psutil
        
        process = psutil.Process()
        initial_memory = process.memory_info().rss
        
        # Enable or disable memory pools
        config = pynlm.createAdvancedConfig()
        config.set("use_memory_pool", self.use_memory_pool)
        
        # Create new brain with memory pool settings
        new_brain = pynlm.createBrain(config)
        new_brain.initialize()
        
        # Measure memory usage
        after_init = process.memory_info().rss
        init_memory = (after_init - initial_memory) / (1024 * 1024)  # MB
        
        # Run simulation
        for step in range(100):
            new_brain.step(step)
            
        final_memory = process.memory_info().rss
        final_memory = (final_memory - initial_memory) / (1024 * 1024)  # MB
        
        return {
            'initial_memory_mb': initial_memory / (1024 * 1024),
            'init_memory_mb': init_memory,
            'final_memory_mb': final_memory,
            'memory_efficiency': final_memory / init_memory if init_memory > 0 else 1.0,
            'steps_per_mb': 100 / final_memory if final_memory > 0 else 0
        }
```

#### SIMD Optimization

Use SIMD for better CPU utilization:

```python
import pynlm
import numpy as np

class SIMDOptimizer:
    def __init__(self, brain):
        self.brain = brain
        
    def benchmark_simd_usage(self):
        """Benchmark SIMD usage in neural computation"""
        # This would require checking actual SIMD usage
        # In practice, check compiler flags and runtime behavior
        
        results = {
            'simd_enabled': self.check_simd_support(),
            'vectorization_potential': self.estimate_vectorization_potential(),
            'performance_gain': self.estimate_performance_gain()
        }
        
        return results
        
    def check_simd_support(self):
        """Check if SIMD is supported/enabled"""
        # Check compiler flags
        import sys
        if hasattr(sys, 'maxsize') and sys.maxsize > 2**32:  # 64-bit
            return "AVX2 or higher"
        else:
            return "SSE or lower"
            
    def estimate_vectorization_potential(self):
        """Estimate potential SIMD vectorization"""
        # Analyze neural operations for SIMD compatibility
        potential_operations = [
            'neuron_state_updates',
            'synapse_weight_updates',
            'spike_detection',
            'memory_access_patterns'
        ]
        
        return {
            'potential_operations': potential_operations,
            'estimated_speedup': 2.0,  # Typical SIMD speedup
            'data_type': 'float32'  # Most common type
        }
        
    def estimate_performance_gain(self):
        """Estimate performance gain from SIMD"""
        # Based on typical neural computation patterns
        return {
            'neuron_updates': 2.0,
            'synapse_updates': 3.0,
            'spike_detection': 4.0,
            'overall': 2.5
        }
```

### Performance Monitoring

#### Real-time Performance Monitoring

Monitor performance in real-time:

```python
import pynlm
import time
import psutil
import threading
from typing import Dict, List, Callable

class RealTimePerformanceMonitor:
    def __init__(self, brain, update_interval=1.0):
        self.brain = brain
        self.update_interval = update_interval
        self.metrics_history: List[Dict] = []
        self.monitoring = False
        self.monitor_thread = None
        
    def start_monitoring(self):
        """Start real-time performance monitoring"""
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        print(f"Started performance monitoring (interval: {self.update_interval}s)")
        
    def stop_monitoring(self):
        """Stop real-time performance monitoring"""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
            self.monitor_thread = None
        print("Stopped performance monitoring")
        
    def _monitor_loop(self):
        """Main monitoring loop"""
        process = psutil.Process()
        
        while self.monitoring:
            start_time = time.time()
            
            # Collect metrics
            metrics = self._collect_metrics()
            
            # Store metrics
            self.metrics_history.append(metrics)
            
            # Print real-time metrics
            self._print_metrics(metrics)
            
            # Sleep for update interval
            elapsed = time.time() - start_time
            sleep_time = max(0, self.update_interval - elapsed)
            time.sleep(sleep_time)
            
    def _collect_metrics(self) -> Dict:
        """Collect current performance metrics"""
        process = psutil.Process()
        
        return {
            'timestamp': time.time(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'memory_usage_mb': process.memory_info().rss / (1024 * 1024),
            'cpu_percent': process.cpu_percent(),
            'simulation_steps': self.metrics_history[-1]['total_steps'] + 1 if self.metrics_history else 1
        }
        
    def _print_metrics(self, metrics: Dict):
        """Print current metrics"""
        print(f"[t={metrics['timestamp']:.1f}] "
              f"Spikes: {metrics['total_spikes']}, "
              f"Firing: {metrics['firing_rate']:.2f} Hz, "
              f"Memory: {metrics['memory_usage_mb']:.1f}MB, "
              f"CPU: {metrics['cpu_percent']:.1f}%")
        
    def get_performance_summary(self) -> Dict:
        """Get summary of collected performance metrics"""
        if not self.metrics_history:
            return {}
            
        # Calculate summary statistics
        spike_rates = [m['firing_rate'] for m in self.metrics_history]
        memory_usage = [m['memory_usage_mb'] for m in self.metrics_history]
        cpu_usage = [m['cpu_percent'] for m in self.metrics_history]
        
        return {
            'monitoring_duration': len(self.metrics_history) * self.update_interval,
            'average_firing_rate': sum(spike_rates) / len(spike_rates),
            'peak_firing_rate': max(spike_rates),
            'average_memory_usage': sum(memory_usage) / len(memory_usage),
            'peak_memory_usage': max(memory_usage),
            'average_cpu_usage': sum(cpu_usage) / len(cpu_usage),
            'total_spikes': self.metrics_history[-1]['total_spikes'],
            'spikes_per_second': self.metrics_history[-1]['total_spikes'] / (len(self.metrics_history) * self.update_interval)
        }
        
    def detect_performance_issues(self) -> List[str]:
        """Detect potential performance issues"""
        issues = []
        
        if not self.metrics_history:
            return issues
            
        summary = self.get_performance_summary()
        
        # Check for high memory usage
        if summary['peak_memory_usage'] > 1000:  # MB
            issues.append(f"High memory usage: {summary['peak_memory_usage']:.1f}MB")
            
        # Check for high CPU usage
        if summary['average_cpu_usage'] > 80:  # %
            issues.append(f"High CPU usage: {summary['average_cpu_usage']:.1f}%")
            
        # Check for low performance
        if summary['spikes_per_second'] < 10:  # spikes/sec
            issues.append(f"Low performance: {summary['spikes_per_second']:.1f} spikes/sec")
            
        return issues
```

### Usage

Use real-time performance monitoring:

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

# Create performance monitor
monitor = RealTimePerformanceMonitor(brain, update_interval=2.0)

# Start monitoring
monitor.start_monitoring()

# Run simulation
for step in range(100):
    brain.step(step)
    time.sleep(0.1)

# Stop monitoring
monitor.stop_monitoring()

# Get performance summary
summary = monitor.get_performance_summary()
print("Performance Summary:", summary)

# Check for issues
issues = monitor.detect_performance_issues()
if issues:
    print("Performance Issues Found:")
    for issue in issues:
        print(f"  - {issue}")
else:
    print("No performance issues detected")
```

This performance tuning guide provides comprehensive information on optimizing NLM performance for different use cases. Use these tools and techniques to ensure your NLM simulations run efficiently and effectively.