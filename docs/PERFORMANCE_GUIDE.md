# NLM Performance Guide

## Overview

This document provides comprehensive documentation for NLM performance optimization and monitoring. Performance is a critical aspect of NLM that affects simulation speed, memory usage, and overall efficiency. This guide covers performance monitoring, optimization techniques, benchmarking, and performance analysis.

## Performance Monitoring

### Performance Metrics

NLM provides comprehensive performance monitoring with the following key metrics:

#### Core Performance Metrics

| Metric | Description | Unit | Measurement Frequency | Typical Range |
|--------|-------------|------|-----------------------|---------------|
| `simulation_time` | Total simulation time | seconds | Per simulation | 1-1000+ |
| `steps_per_second` | Simulation steps per second | steps/s | Per simulation | 100-100000 |
| `memory_usage` | Memory consumption | MB | Per simulation | 50-5000 |
| `cpu_usage` | CPU utilization | % | Per simulation | 10-100% |
| `firing_rate` | Average firing rate | Hz | Per simulation | 0.1-100 |
| `neuron_utilization` | Neuron activity level | % | Per simulation | 1-100 |
| `plasticity_computation` | Time spent on plasticity | ms | Per simulation | 1-1000 |
| `spike_processing` | Time spent on spike processing | ms | Per simulation | 10-500 |

#### Advanced Performance Metrics

| Metric | Description | Unit | Measurement Frequency | Typical Range |
|--------|-------------|------|-----------------------|---------------|
| `working_memory_usage` | Working memory consumption | MB | Per simulation | 1-100 |
| `episodic_memory_usage` | Episodic memory consumption | MB | Per simulation | 1-100 |
| `communication_overhead` | Communication overhead | ms | Per simulation | 1-50 |
| `synchronization_time` | Synchronization time | ms | Per simulation | 1-100 |
| `load_balance` | Load balance across threads | % | Per simulation | 80-100 |
| `cache_efficiency` | Cache efficiency | % | Per simulation | 60-99 |
| `vectorization_efficiency` | SIMD vectorization efficiency | % | Per simulation | 70-100 |

### Performance Monitoring Commands

#### Basic Performance Monitoring

```bash
# Monitor simulation performance
./nlm performance monitor --duration=60 --interval=5

# Get real-time performance metrics
./nlm performance metrics --realtime --stream

# Set performance alerts
./nlm performance alert --metric=memory_usage --threshold=90
```

#### Advanced Performance Monitoring

```bash
# Profile simulation performance
./nlm performance profile --duration=120 --output=profile.json

# Monitor specific components
./nlm performance monitor --components=plasticity --interval=10

# Analyze performance bottlenecks
./nlm performance analyze --bottleneck=spike_processing

# Generate performance report
./nlm performance report --output=performance_report.html
```

### Performance Monitoring Scripts

#### Python Performance Monitoring

```python
import pynlm
import time
import statistics
from typing import Dict, List, Any
import matplotlib.pyplot as plt

class PerformanceMonitor:
    def __init__(self):
        self.metrics = {
            'simulation_time': [],
            'steps_per_second': [],
            'memory_usage': [],
            'firing_rate': [],
            'neuron_utilization': [],
            'plasticity_computation': [],
            'spike_processing': [],
            'communication_overhead': []
        }
        self.current_metrics = {}
    
    def start_monitoring(self):
        """Start performance monitoring."""
        self.start_time = time.time()
        self.step_count = 0
        
        # Reset metrics
        for metric in self.metrics:
            self.metrics[metric].clear()
        
        self.current_metrics = {
            'start_time': self.start_time,
            'steps_completed': 0,
            'peak_memory': 0,
            'cpu_usage_samples': []
        }
    
    def record_step_metrics(self, brain, world, agent):
        """Record metrics for a single simulation step."""
        current_time = time.time()
        step_time = current_time - self.current_metrics['start_time']
        
        # Record core metrics
        self.metrics['simulation_time'].append(step_time)
        self.metrics['steps_per_second'].append(1.0 / step_time if step_time > 0 else 0)
        
        # Record brain metrics
        self.metrics['firing_rate'].append(brain.getAverageFiringRate())
        self.metrics['neuron_utilization'].append(
            (brain.getFiringNeuronCount() / brain.getTotalNeuronCount()) * 100
        )
        self.metrics['plasticity_computation'].append(
            brain.getTotalPlasticityComputationTime()
        )
        self.metrics['spike_processing'].append(
            brain.getTotalSpikeProcessingTime()
        )
        
        # Record agent metrics
        self.metrics['communication_overhead'].append(
            agent.getCommunicationOverhead()
        )
        
        # Record world metrics
        world_metrics = world.getPerformanceMetrics()
        for key, value in world_metrics.items():
            if key not in self.metrics:
                self.metrics[key] = []
            self.metrics[key].append(value)
        
        self.current_metrics['steps_completed'] += 1
        self.current_metrics['start_time'] = current_time
    
    def get_performance_summary(self):
        """Get performance summary."""
        summary = {
            'total_simulation_time': self.metrics['simulation_time'][-1] if self.metrics['simulation_time'] else 0,
            'total_steps': self.current_metrics['steps_completed'],
            'average_steps_per_second': statistics.mean(self.metrics['steps_per_second']) if self.metrics['steps_per_second'] else 0,
            'median_steps_per_second': statistics.median(self.metrics['steps_per_second']) if self.metrics['steps_per_second'] else 0,
            'min_steps_per_second': min(self.metrics['steps_per_second']) if self.metrics['steps_per_second'] else 0,
            'max_steps_per_second': max(self.metrics['steps_per_second']) if self.metrics['steps_per_second'] else 0,
            'average_memory_usage': statistics.mean(self.metrics['memory_usage']) if self.metrics['memory_usage'] else 0,
            'peak_memory_usage': max(self.metrics['memory_usage']) if self.metrics['memory_usage'] else 0,
            'average_firing_rate': statistics.mean(self.metrics['firing_rate']) if self.metrics['firing_rate'] else 0,
            'average_neuron_utilization': statistics.mean(self.metrics['neuron_utilization']) if self.metrics['neuron_utilization'] else 0,
            'total_plasticity_time': sum(self.metrics['plasticity_computation']) if self.metrics['plasticity_computation'] else 0,
            'total_spike_processing_time': sum(self.metrics['spike_processing']) if self.metrics['spike_processing'] else 0
        }
        
        return summary
    
    def generate_performance_plot(self, plot_type='steps_per_second'):
        """Generate performance plot."""
        if not plt:
            return None
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle('NLM Performance Monitoring', fontsize=16)
        
        # Plot 1: Steps per second
        if plot_type in ['steps_per_second', 'all']:
            ax = axes[0, 0]
            ax.plot(self.metrics['steps_per_second'])
            ax.set_title('Steps per Second')
            ax.set_xlabel('Step')
            ax.set_ylabel('Steps/second')
            ax.grid(True)
        
        # Plot 2: Memory usage
        if plot_type in ['memory_usage', 'all']:
            ax = axes[0, 1]
            ax.plot(self.metrics['memory_usage'])
            ax.set_title('Memory Usage')
            ax.set_xlabel('Step')
            ax.set_ylabel('Memory (MB)')
            ax.grid(True)
        
        # Plot 3: Firing rate
        if plot_type in ['firing_rate', 'all']:
            ax = axes[1, 0]
            ax.plot(self.metrics['firing_rate'])
            ax.set_title('Firing Rate')
            ax.set_xlabel('Step')
            ax.set_ylabel('Hz')
            ax.grid(True)
        
        # Plot 4: Neuron utilization
        if plot_type in ['neuron_utilization', 'all']:
            ax = axes[1, 1]
            ax.plot(self.metrics['neuron_utilization'])
            ax.set_title('Neuron Utilization')
            ax.set_xlabel('Step')
            ax.set_ylabel('Utilization (%)')
            ax.grid(True)
        
        plt.tight_layout()
        return fig
    
    def save_performance_data(self, file_path):
        """Save performance data to file."""
        import json
        
        data = {
            'metrics': self.metrics,
            'summary': self.get_performance_summary(),
            'timestamp': time.time()
        }
        
        with open(file_path, 'w') as f:
            json.dump(data, f, indent=2)

# Example usage
monitor = PerformanceMonitor()

# Start monitoring
monitor.start_monitoring()

# Run simulation with monitoring
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(1000):
    # Record metrics for each step
    monitor.record_step_metrics(brain, world, agent)
    
    # Run simulation
    brain.step(step)

# Get performance summary
summary = monitor.get_performance_summary()
print("Performance Summary:")
for key, value in summary.items():
    print(f"  {key}: {value:.2f}")

# Generate performance plot
fig = monitor.generate_performance_plot('all')
if fig:
    fig.savefig('performance_plot.png')

# Save performance data
monitor.save_performance_data('performance_data.json')
```

#### Shell Script Performance Monitoring

```bash
#!/bin/bash

# Performance monitoring script for NLM

# Configuration
NLM_EXEC="./nlm"
MONITOR_DURATION=300  # 5 minutes
MONITOR_INTERVAL=10  # 10 seconds
OUTPUT_FILE="performance_data.json"
PLOT_FILE="performance_plot.png"

# Start performance monitoring
./nlm performance monitor --duration=$MONITOR_DURATION --interval=$MONITOR_INTERVAL --output=$OUTPUT_FILE

# Parse performance data
python3 << EOF
import json
import matplotlib.pyplot as plt

# Load performance data
with open('$OUTPUT_FILE', 'r') as f:
    data = json.load(f)

# Extract metrics
steps_per_second = data['metrics']['steps_per_second']
memory_usage = data['metrics']['memory_usage']
firing_rate = data['metrics']['firing_rate']

# Create plots
fig, axes = plt.subplots(2, 2, figsize=(12, 10))
fig.suptitle('NLM Performance Monitoring', fontsize=16)

# Plot 1: Steps per second
axes[0, 0].plot(steps_per_second)
axes[0, 0].set_title('Steps per Second')
axes[0, 0].set_xlabel('Time (samples)')
axes[0, 0].set_ylabel('Steps/second')
axes[0, 0].grid(True)

# Plot 2: Memory usage
axes[0, 1].plot(memory_usage)
axes[0, 1].set_title('Memory Usage')
axes[0, 1].set_xlabel('Time (samples)')
axes[0, 1].set_ylabel('Memory (MB)')
axes[0, 1].grid(True)

# Plot 3: Firing rate
axes[1, 0].plot(firing_rate)
axes[1, 0].set_title('Firing Rate')
axes[1, 0].set_xlabel('Time (samples)')
axes[1, 0].set_ylabel('Hz')
axes[1, 0].grid(True)

# Plot 4: Performance overview
ax2 = axes[1, 1]
ax2.plot(steps_per_second, label='Steps/second')
ax2.set_title('Performance Overview')
ax2.set_xlabel('Time (samples)')
ax2.set_ylabel('Performance')
ax2.legend()
ax2.grid(True)

plt.tight_layout()
plt.savefig('$PLOT_FILE')
print("Performance plot saved to: $PLOT_FILE")

EOF

echo "Performance monitoring completed!"
```

## Performance Optimization

### Optimization Techniques

#### Memory Optimization

```bash
# Enable memory pooling
./nlm config set --key=performance.memory_pooling --value=true

# Configure memory pool size
./nlm config set --key=performance.memory_pool_size --value=1024

# Enable NUMA awareness
./nlm config set --key=performance.enable_numa_awareness --value=true
```

| Optimization Technique | Description | Expected Improvement | Implementation Cost |
|------------------------|-------------|---------------------|-------------------|
| Memory pooling | Pre-allocate memory pools | 20-40% memory reduction | Low |
| NUMA awareness | Optimize memory access patterns | 10-30% performance gain | Medium |
| Cache optimizations | Optimize cache usage | 15-25% performance gain | Low |
| Vectorization | Enable SIMD vectorization | 50-100% performance gain | Low |
| Parallel processing | Enable multi-threading | 200-500% performance gain | Medium |

#### CPU Optimization

```bash
# Enable parallel processing
./nlm config set --key=performance.parallel_processing --value=true

# Configure thread count
./nlm config set --key=performance.max_threads --value=8

# Enable vectorization
./nlm config set --key=performance.enable_vectorization --value=true

# Enable cache optimizations
./nlm config set --key=performance.use_cache_optimizations --value=true
```

#### Network Optimization

```bash
# Configure communication optimization
./nlm config set --key=network.buffer_size --value=1024
./nlm config set --key=network.packet_size --value=1500
./nlm config set --key=network.enable_compression --value=true
```

### Performance Optimization Scripts

#### Python Performance Optimization

```python
import pynlm
import time
from typing import Dict, List, Any
import psutil

class PerformanceOptimizer:
    def __init__(self, config):
        self.config = config
        self.performance_history = []
        self.optimization_suggestions = []
    
    def profile_performance(self, brain, world, agent, num_steps=1000):
        """Profile NLM performance and collect metrics."""
        print("Starting performance profile...")
        
        # Start monitoring
        start_time = time.time()
        memory_before = psutil.virtual_memory().used
        
        # Run simulation
        for step in range(num_steps):
            brain.step(step)
            
            # Record performance metrics
            current_memory = psutil.virtual_memory().used
            memory_delta = (current_memory - memory_before) / (1024 * 1024)  # MB
            
            performance_data = {
                'step': step,
                'time': time.time() - start_time,
                'memory_usage': memory_delta,
                'firing_rate': brain.getAverageFiringRate(),
                'neuron_utilization': (brain.getFiringNeuronCount() / brain.getTotalNeuronCount()) * 100,
                'cpu_usage': psutil.cpu_percent()
            }
            
            self.performance_history.append(performance_data)
        
        end_time = time.time()
        memory_after = psutil.virtual_memory().used
        
        # Calculate performance summary
        total_time = end_time - start_time
        avg_steps_per_second = num_steps / total_time if total_time > 0 else 0
        peak_memory = (memory_after - memory_before) / (1024 * 1024)
        
        summary = {
            'total_time': total_time,
            'avg_steps_per_second': avg_steps_per_second,
            'peak_memory_usage': peak_memory,
            'total_memory_change': (memory_after - memory_before) / (1024 * 1024),
            'avg_firing_rate': sum(d['firing_rate'] for d in self.performance_history) / len(self.performance_history),
            'avg_neuron_utilization': sum(d['neuron_utilization'] for d in self.performance_history) / len(self.performance_history),
            'avg_cpu_usage': sum(d['cpu_usage'] for d in self.performance_history) / len(self.performance_history)
        }
        
        return summary
    
    def optimize_memory_usage(self, brain):
        """Optimize memory usage."""
        suggestions = []
        
        # Check if memory pooling is enabled
        if not self.config.get("performance.memory_pooling", True):
            suggestions.append({
                'type': 'memory_pooling',
                'description': 'Enable memory pooling to reduce memory fragmentation',
                'expected_improvement': '20-40% memory reduction',
                'implementation_cost': 'Low',
                'priority': 'High'
            })
        
        # Check if memory pool size is optimal
        current_pool_size = self.config.get("performance.memory_pool_size", 1024)
        total_neurons = brain.getTotalNeuronCount()
        
        if current_pool_size < total_neurons * 1.5:
            suggestions.append({
                'type': 'increase_memory_pool',
                'description': f'Increase memory pool size from {current_pool_size} to {total_neurons * 1.5}',
                'expected_improvement': 'Reduce memory allocation overhead',
                'implementation_cost': 'Low',
                'priority': 'Medium'
            })
        
        return suggestions
    
    def optimize_cpu_usage(self, brain):
        """Optimize CPU usage."""
        suggestions = []
        
        # Check if parallel processing is enabled
        if not self.config.get("performance.parallel_processing", True):
            suggestions.append({
                'type': 'parallel_processing',
                'description': 'Enable parallel processing for multi-core systems',
                'expected_improvement': '200-500% performance gain',
                'implementation_cost': 'Medium',
                'priority': 'High'
            })
        
        # Check if vectorization is enabled
        if not self.config.get("performance.enable_vectorization", True):
            suggestions.append({
                'type': 'vectorization',
                'description': 'Enable SIMD vectorization for CPU optimization',
                'expected_improvement': '50-100% performance gain',
                'implementation_cost': 'Low',
                'priority': 'High'
            })
        
        # Check if cache optimizations are enabled
        if not self.config.get("performance.use_cache_optimizations", True):
            suggestions.append({
                'type': 'cache_optimizations',
                'description': 'Enable cache optimizations for better cache utilization',
                'expected_improvement': '15-25% performance gain',
                'implementation_cost': 'Low',
                'priority': 'Medium'
            })
        
        return suggestions
    
    def optimize_simulation_parameters(self, brain):
        """Optimize simulation parameters."""
        suggestions = []
        
        # Check neuron count optimization
        current_neurons = self.config.get("brain.neuron_count", 1000)
        firing_rate = brain.getAverageFiringRate()
        
        if firing_rate < 10 and current_neurons > 500:
            suggestions.append({
                'type': 'reduce_neuron_count',
                'description': f'Reduce neuron count from {current_neurons} to {current_neurons // 2}',
                'expected_improvement': 'Faster simulation with minimal performance impact',
                'implementation_cost': 'Low',
                'priority': 'Medium'
            })
        
        # Check plasticity optimization
        current_learning_rate = self.config.get("plasticity.stdp.learning_rate", 0.001)
        
        if current_learning_rate > 0.01:
            suggestions.append({
                'type': 'reduce_learning_rate',
                'description': f'Reduce learning rate from {current_learning_rate} to 0.001',
                'expected_improvement': 'Faster simulation with stable learning',
                'implementation_cost': 'Low',
                'priority': 'Medium'
            })
        
        return suggestions
    
    def apply_optimizations(self, suggestions):
        """Apply optimization suggestions."""
        applied_suggestions = []
        
        for suggestion in suggestions:
            if suggestion['priority'] in ['High', 'Medium']:
                # Apply optimization based on suggestion type
                if suggestion['type'] == 'memory_pooling':
                    self.config.set("performance.memory_pooling", True)
                elif suggestion['type'] == 'parallel_processing':
                    self.config.set("performance.parallel_processing", True)
                elif suggestion['type'] == 'vectorization':
                    self.config.set("performance.enable_vectorization", True)
                elif suggestion['type'] == 'cache_optimizations':
                    self.config.set("performance.use_cache_optimizations", True)
                
                applied_suggestions.append(suggestion)
        
        return applied_suggestions
    
    def get_optimization_report(self):
        """Get optimization report."""
        # Profile current performance
        summary = self.profile_performance(brain, world, agent, 500)
        
        # Get optimization suggestions
        memory_suggestions = self.optimize_memory_usage(brain)
        cpu_suggestions = self.optimize_cpu_usage(brain)
        simulation_suggestions = self.optimize_simulation_parameters(brain)
        
        all_suggestions = memory_suggestions + cpu_suggestions + simulation_suggestions
        
        # Apply high-priority suggestions
        applied_suggestions = self.apply_optimizations(all_suggestions)
        
        # Profile performance after optimizations
        optimized_summary = self.profile_performance(brain, world, agent, 500)
        
        # Calculate improvement
        improvement = {
            'time_improvement': ((summary['total_time'] - optimized_summary['total_time']) / summary['total_time']) * 100 if summary['total_time'] > 0 else 0,
            'memory_improvement': ((summary['peak_memory_usage'] - optimized_summary['peak_memory_usage']) / summary['peak_memory_usage']) * 100 if summary['peak_memory_usage'] > 0 else 0
        }
        
        return {
            'before_optimization': summary,
            'after_optimization': optimized_summary,
            'improvement': improvement,
            'applied_suggestions': applied_suggestions,
            'pending_suggestions': [s for s in all_suggestions if s not in applied_suggestions]
        }

# Example usage
optimizer = PerformanceOptimizer(config)

# Get optimization report
report = optimizer.get_optimization_report()

print("Optimization Report:")
print(f"Time improvement: {report['improvement']['time_improvement']:.1f}%")
print(f"Memory improvement: {report['improvement']['memory_improvement']:.1f}%")
print(f"Applied suggestions: {len(report['applied_suggestions'])}")
print(f"Pending suggestions: {len(report['pending_suggestions'])}")

# Save report to file
import json
with open("optimization_report.json", 'w') as f:
    json.dump(report, f, indent=2)
```

#### Shell Script Optimization

```bash
#!/bin/bash

# Performance optimization script for NLM

# Configuration
NLM_EXEC="./nlm"
OPTIMIZATION_SCRIPT="optimize_nlm.py"
REPORT_FILE="optimization_report.json"

# Create optimization script
python3 << EOF
import pynlm
import time
import json
from typing import Dict, List, Any

class NLMPerformanceOptimizer:
    def __init__(self, config):
        self.config = config
        self.performance_history = []
    
    def profile_performance(self, brain, world, agent, num_steps=500):
        """Profile NLM performance."""
        start_time = time.time()
        
        for step in range(num_steps):
            brain.step(step)
            
            performance_data = {
                'step': step,
                'time': time.time() - start_time,
                'firing_rate': brain.getAverageFiringRate(),
                'neuron_utilization': (brain.getFiringNeuronCount() / brain.getTotalNeuronCount()) * 100
            }
            
            self.performance_history.append(performance_data)
        
        total_time = time.time() - start_time
        return {
            'total_time': total_time,
            'avg_firing_rate': sum(d['firing_rate'] for d in self.performance_history) / len(self.performance_history),
            'avg_neuron_utilization': sum(d['neuron_utilization'] for d in self.performance_history) / len(self.performance_history)
        }
    
    def optimize_memory(self):
        """Optimize memory usage."""
        suggestions = []
        
        if not self.config.get("performance.memory_pooling", True):
            suggestions.append({
                'type': 'memory_pooling',
                'expected_improvement': '20-40% memory reduction',
                'priority': 'High'
            })
        
        return suggestions
    
    def optimize_cpu(self):
        """Optimize CPU usage."""
        suggestions = []
        
        if not self.config.get("performance.parallel_processing", True):
            suggestions.append({
                'type': 'parallel_processing',
                'expected_improvement': '200-500% performance gain',
                'priority': 'High'
            })
        
        if not self.config.get("performance.enable_vectorization", True):
            suggestions.append({
                'type': 'vectorization',
                'expected_improvement': '50-100% performance gain',
                'priority': 'High'
            })
        
        return suggestions

# Load configuration
config = pynlm.createDefaultConfig()

# Create brain with test configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent for testing
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create optimizer
optimizer = NLMPerformanceOptimizer(config)

# Profile current performance
current_performance = optimizer.profile_performance(brain, world, agent)
print("Current Performance:")
for key, value in current_performance.items():
    print(f"  {key}: {value:.2f}")

# Get optimization suggestions
memory_suggestions = optimizer.optimize_memory()
cpu_suggestions = optimizer.optimize_cpu()
all_suggestions = memory_suggestions + cpu_suggestions

print(f"\nOptimization Suggestions ({len(all_suggestions)}):")
for suggestion in all_suggestions:
    print(f"  - {suggestion['type']}: {suggestion['expected_improvement']} (Priority: {suggestion['priority']})")

# Apply high-priority optimizations
if memory_suggestions:
    config.set("performance.memory_pooling", True)
if cpu_suggestions:
    config.set("performance.parallel_processing", True)
    config.set("performance.enable_vectorization", True)

# Create new brain with optimized configuration
optimized_brain = pynlm.createBrain(config)
optimized_brain.initialize()

# Profile optimized performance
optimized_performance = optimizer.profile_performance(optimized_brain, world, agent)
print("\nOptimized Performance:")
for key, value in optimized_performance.items():
    print(f"  {key}: {value:.2f}")

# Calculate improvement
time_improvement = ((current_performance['total_time'] - optimized_performance['total_time']) / current_performance['total_time']) * 100
print(f"\nTime Improvement: {time_improvement:.1f}%")

# Save optimization report
optimization_report = {
    'current_performance': current_performance,
    'optimized_performance': optimized_performance,
    'time_improvement': time_improvement,
    'applied_optimizations': [s['type'] for s in all_suggestions]
}

with open("$REPORT_FILE", 'w') as f:
    json.dump(optimization_report, f, indent=2)

print(f"\nOptimization report saved to: $REPORT_FILE")
EOF

echo "Performance optimization completed!"
```

## Benchmarking

### Benchmarking Scripts

#### Python Benchmarking

```python
import pynlm
import time
import statistics
from typing import Dict, List, Any
import json
import os
from datetime import datetime

class NLMBenchmark:
    def __init__(self, benchmark_name):
        self.benchmark_name = benchmark_name
        self.results = []
        self.start_time = None
        self.end_time = None
    
    def run_benchmark(self, config, benchmark_type, num_steps=1000):
        """Run NLM benchmark."""
        print(f"Running {self.benchmark_name} benchmark ({benchmark_type})...")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Create agent
        agent = pynlm.createBrain(brain)  # Simplified for benchmarking
        agent.initialize(world)
        
        # Start benchmark
        self.start_time = time.time()
        
        # Run benchmark
        for step in range(num_steps):
            world.update(0.1)
            
            # Get percept and process
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # End benchmark
        self.end_time = time.time()
        
        # Collect results
        benchmark_result = {
            'benchmark_name': self.benchmark_name,
            'benchmark_type': benchmark_type,
            'timestamp': datetime.now().isoformat(),
            'config': {
                'neuron_count': config.get("brain.neuron_count", 0),
                'synapse_density': config.get("brain.synapse_density", 0),
                'plasticity_stdp_enable': config.get("plasticity.stdp.enable", False),
                'plasticity_hebbian_enable': config.get("plasticity.hebbian.enable", False),
                'performance_optimization_level': config.get("performance.optimization_level", 0),
                'performance_parallel_processing': config.get("performance.parallel_processing", False)
            },
            'performance': {
                'total_time': self.end_time - self.start_time,
                'steps_per_second': num_steps / (self.end_time - self.start_time) if (self.end_time - self.start_time) > 0 else 0,
                'avg_firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'total_neurons': brain.getTotalNeuronCount(),
                'neuron_utilization': (brain.getFiringNeuronCount() / brain.getTotalNeuronCount()) * 100
            },
            'memory': {
                'peak_memory_usage': self.get_peak_memory_usage(),
                'memory_efficiency': self.calculate_memory_efficiency(brain)
            },
            'config_efficiency': self.calculate_config_efficiency(config)
        }
        
        self.results.append(benchmark_result)
        
        return benchmark_result
    
    def get_peak_memory_usage(self):
        """Get peak memory usage."""
        try:
            import psutil
            return psutil.virtual_memory().used / (1024 * 1024)  # MB
        except ImportError:
            return 0
    
    def calculate_memory_efficiency(self, brain):
        """Calculate memory efficiency."""
        neurons = brain.getTotalNeuronCount()
        synapses = brain.getTotalSynapseCount()
        
        if neurons > 0 and synapses > 0:
            return synapses / neurons  # Synapses per neuron
        
        return 0
    
    def calculate_config_efficiency(self, config):
        """Calculate configuration efficiency."""
        efficiency_score = 0
        
        # Neuron efficiency
        neuron_count = config.get("brain.neuron_count", 0)
        if 1000 <= neuron_count <= 5000:
            efficiency_score += 30
        
        # Plasticity efficiency
        if config.get("plasticity.stdp.enable", False) and config.get("plasticity.hebbian.enable", False):
            efficiency_score += 25
        
        # Performance efficiency
        if config.get("performance.optimization_level", 2) >= 2:
            efficiency_score += 25
        
        # Parallel processing efficiency
        if config.get("performance.parallel_processing", False):
            efficiency_score += 20
        
        return efficiency_score
    
    def run_comparison_benchmark(self, configs, benchmark_types, num_steps=1000):
        """Run comparison benchmark."""
        print(f"Running comparison benchmark with {len(configs)} configurations...")
        
        comparison_results = []
        
        for config_name, config in configs.items():
            for benchmark_type in benchmark_types:
                benchmark = NLMBenchmark(f"{config_name}_{benchmark_type}")
                result = benchmark.run_benchmark(config, benchmark_type, num_steps)
                comparison_results.append(result)
        
        return comparison_results
    
    def get_benchmark_summary(self):
        """Get benchmark summary."""
        if not self.results:
            return {}
        
        # Calculate average metrics
        total_time = sum(r['performance']['total_time'] for r in self.results)
        avg_steps_per_second = sum(r['performance']['steps_per_second'] for r in self.results) / len(self.results)
        avg_firing_rate = sum(r['performance']['avg_firing_rate'] for r in self.results) / len(self.results)
        avg_efficiency = sum(r['config_efficiency'] for r in self.results) / len(self.results)
        
        return {
            'benchmark_name': self.benchmark_name,
            'total_benchmarks': len(self.results),
            'total_time': total_time,
            'average_steps_per_second': avg_steps_per_second,
            'average_firing_rate': avg_firing_rate,
            'average_config_efficiency': avg_efficiency,
            'best_configuration': self.get_best_configuration(),
            'results': self.results
        }
    
    def get_best_configuration(self):
        """Get best configuration based on performance."""
        if not self.results:
            return None
        
        best_result = max(self.results, key=lambda x: x['performance']['steps_per_second'])
        
        return {
            'benchmark_name': best_result['benchmark_name'],
            'performance': best_result['performance'],
            'config': best_result['config']
        }
    
    def save_results(self, file_path):
        """Save benchmark results to file."""
        with open(file_path, 'w') as f:
            json.dump({
                'benchmark_name': self.benchmark_name,
                'timestamp': datetime.now().isoformat(),
                'results': self.results,
                'summary': self.get_benchmark_summary()
            }, f, indent=2)

# Example usage
benchmark = NLMBenchmark("Performance_Benchmark")

# Define configurations to test
configs = {
    'small': {
        "brain.neuron_count": 1000,
        "brain.synapse_density": 0.1,
        "performance.optimization_level": 1
    },
    'medium': {
        "brain.neuron_count": 5000,
        "brain.synapse_density": 0.05,
        "performance.optimization_level": 2
    },
    'large': {
        "brain.neuron_count": 10000,
        "brain.synapse_density": 0.02,
        "performance.optimization_level": 3
    }
}

# Run benchmark
results = benchmark.run_comparison_benchmark(
    configs,
    ['simple', 'complex', 'stress'],
    num_steps=500
)

# Get benchmark summary
summary = benchmark.get_benchmark_summary()

print(f"Benchmark completed: {summary['total_benchmarks']} runs")
print(f"Average performance: {summary['average_steps_per_second']:.2f} steps/second")
print(f"Best configuration: {summary['best_configuration']['benchmark_name']}")

# Save results
benchmark.save_results("benchmark_results.json")

# Generate comparison report
with open("benchmark_comparison.html", 'w') as f:
    f.write(generate_html_report(summary))
```

#### Shell Script Benchmarking

```bash
#!/bin/bash

# Benchmarking script for NLM

# Configuration
NLM_EXEC="./nlm"
BENCHMARK_SCRIPT="benchmark_nlm.py"
RESULTS_FILE="benchmark_results.json"
REPORT_FILE="benchmark_report.html"

# Create benchmark script
python3 << EOF
import pynlm
import time
import json
from datetime import datetime

class NLMBenchmark:
    def __init__(self, benchmark_name):
        self.benchmark_name = benchmark_name
        self.results = []
    
    def run_benchmark(self, config, num_steps=500):
        """Run NLM benchmark."""
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20)
        world.reset()
        
        agent = pynlm.createBrain(brain)
        agent.initialize(world)
        
        # Start benchmark
        start_time = time.time()
        
        # Run benchmark
        for step in range(num_steps):
            world.update(0.1)
            
            # Get percept and process
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # End benchmark
        end_time = time.time()
        
        # Calculate performance
        total_time = end_time - start_time
        steps_per_second = num_steps / total_time if total_time > 0 else 0
        
        result = {
            'benchmark_name': self.benchmark_name,
            'timestamp': datetime.now().isoformat(),
            'config': {
                'neuron_count': config.get("brain.neuron_count", 0),
                'synapse_density': config.get("brain.synapse_density", 0),
                'performance_optimization_level': config.get("performance.optimization_level", 0)
            },
            'performance': {
                'total_time': total_time,
                'steps_per_second': steps_per_second,
                'avg_firing_rate': brain.getAverageFiringRate()
            }
        }
        
        self.results.append(result)
        return result

# Define configurations to benchmark
configs = {
    'small': {
        "brain.neuron_count": 1000,
        "brain.synapse_density": 0.1,
        "performance.optimization_level": 1
    },
    'medium': {
        "brain.neuron_count": 5000,
        "brain.synapse_density": 0.05,
        "performance.optimization_level": 2
    },
    'large': {
        "brain.neuron_count": 10000,
        "brain.synapse_density": 0.02,
        "performance.optimization_level": 3
    }
}

# Run benchmarks
benchmark_results = []
for config_name, config in configs.items():
    benchmark = NLMBenchmark(f"{config_name}_benchmark")
    result = benchmark.run_benchmark(config)
    benchmark_results.append(result)
    print(f"{config_name} benchmark: {result['performance']['steps_per_second']:.2f} steps/second")

# Generate comparison report
print(f"\nBenchmark Comparison:")
print(f"{'Configuration':<15} {'Steps/Second':<15} {'Time (s)':<10} {'Firing Rate':<12}")
print("-" * 52)

for result in benchmark_results:
    config = result['config']
    perf = result['performance']
    print(f"{config['neuron_count']:<15} {perf['steps_per_second']:<15.2f} {perf['total_time']:<10.2f} {perf['avg_firing_rate']:<12.2f}")

# Save results
with open("$RESULTS_FILE", 'w') as f:
    json.dump({
        'benchmark_name': 'NLM_Performance_Benchmark',
        'timestamp': datetime.now().isoformat(),
        'results': benchmark_results
    }, f, indent=2)

print(f"\nBenchmark results saved to: $RESULTS_FILE")

EOF

echo "Benchmarking completed!"
```

## Performance Analysis

### Performance Analysis Scripts

#### Python Performance Analysis

```python
import pynlm
import json
import matplotlib.pyplot as plt
import numpy as np
from typing import Dict, List, Any
import seaborn as sns

class PerformanceAnalyzer:
    def __init__(self, benchmark_file):
        self.benchmark_file = benchmark_file
        self.results = self.load_results()
    
    def load_results(self):
        """Load benchmark results from file."""
        with open(self.benchmark_file, 'r') as f:
            return json.load(f)['results']
    
    def analyze_performance_trends(self):
        """Analyze performance trends."""
        trends = {}
        
        # Extract metrics
        for result in self.results:
            config = result['config']
            perf = result['performance']
            
            config_key = f"{config['neuron_count']}_neurons"
            
            if config_key not in trends:
                trends[config_key] = {
                    'steps_per_second': [],
                    'firing_rates': [],
                    'neuron_counts': []
                }
            
            trends[config_key]['steps_per_second'].append(perf['steps_per_second'])
            trends[config_key]['firing_rates'].append(perf['avg_firing_rate'])
            trends[config_key]['neuron_counts'].append(config['neuron_count'])
        
        # Calculate statistics
        analysis = {}
        for config_key, data in trends.items():
            analysis[config_key] = {
                'avg_steps_per_second': np.mean(data['steps_per_second']),
                'std_steps_per_second': np.std(data['steps_per_second']),
                'avg_firing_rate': np.mean(data['firing_rates']),
                'std_firing_rate': np.std(data['firing_rates']),
                'neuron_count': data['neuron_counts'][0]
            }
        
        return analysis
    
    def plot_performance_comparison(self):
        """Plot performance comparison."""
        if not plt:
            return None
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle('NLM Performance Analysis', fontsize=16)
        
        # Prepare data
        config_keys = list(self.results[0]['config'].keys())
        steps_per_second = [r['performance']['steps_per_second'] for r in self.results]
        firing_rates = [r['performance']['avg_firing_rate'] for r in self.results]
        neuron_counts = [r['config']['neuron_count'] for r in self.results]
        
        # Plot 1: Steps per second vs neuron count
        ax1 = axes[0, 0]
        ax1.scatter(neuron_counts, steps_per_second, alpha=0.7)
        ax1.set_xlabel('Neuron Count')
        ax1.set_ylabel('Steps per Second')
        ax1.set_title('Performance vs Neuron Count')
        ax1.grid(True)
        
        # Add trend line
        z = np.polyfit(neuron_counts, steps_per_second, 1)
        p = np.poly1d(z)
        ax1.plot(neuron_counts, p(neuron_counts), "r--", alpha=0.8)
        
        # Plot 2: Firing rate vs neuron count
        ax2 = axes[0, 1]
        ax2.scatter(neuron_counts, firing_rates, alpha=0.7)
        ax2.set_xlabel('Neuron Count')
        ax2.set_ylabel('Firing Rate (Hz)')
        ax2.set_title('Firing Rate vs Neuron Count')
        ax2.grid(True)
        
        # Plot 3: Performance correlation
        ax3 = axes[1, 0]
        correlation_matrix = np.corrcoef([steps_per_second, firing_rates, neuron_counts])
        sns.heatmap(correlation_matrix, annot=True, cmap='coolwarm', center=0,
                   xticklabels=['Steps/s', 'Firing Rate', 'Neuron Count'],
                   yticklabels=['Steps/s', 'Firing Rate', 'Neuron Count'],
                   ax=ax3)
        ax3.set_title('Performance Correlation Matrix')
        
        # Plot 4: Performance distribution
        ax4 = axes[1, 1]
        ax4.hist(steps_per_second, bins=10, alpha=0.7, edgecolor='black')
        ax4.set_xlabel('Steps per Second')
        ax4.set_ylabel('Frequency')
        ax4.set_title('Performance Distribution')
        ax4.grid(True)
        
        plt.tight_layout()
        return fig
    
    def analyze_efficiency(self):
        """Analyze configuration efficiency."""
        efficiency_analysis = {}
        
        for result in self.results:
            config = result['config']
            perf = result['performance']
            
            # Calculate efficiency score
            neuron_efficiency = min(perf['steps_per_second'] / 1000, 10)  # Normalized
            firing_efficiency = min(perf['avg_firing_rate'] / 50, 10)  # Normalized
            
            efficiency_score = (neuron_efficiency + firing_efficiency) / 2
            
            efficiency_analysis[f"{config['neuron_count']}_neurons"] = {
                'efficiency_score': efficiency_score,
                'steps_per_second': perf['steps_per_second'],
                'firing_rate': perf['avg_firing_rate'],
                'neuron_count': config['neuron_count']
            }
        
        return efficiency_analysis
    
    def generate_performance_report(self):
        """Generate performance analysis report."""
        trends = self.analyze_performance_trends()
        efficiency = self.analyze_efficiency()
        
        report = {
            'benchmark_name': 'NLM_Performance_Analysis',
            'timestamp': datetime.now().isoformat(),
            'summary': {
                'total_benchmarks': len(self.results),
                'avg_performance': np.mean([r['performance']['steps_per_second'] for r in self.results]),
                'best_configuration': max(efficiency.items(), key=lambda x: x[1]['efficiency_score'])[0],
                'performance_trend': 'increasing' if len(self.results) > 1 and self.results[-1]['performance']['steps_per_second'] > self.results[0]['performance']['steps_per_second'] else 'decreasing'
            },
            'trends': trends,
            'efficiency': efficiency,
            'recommendations': self.generate_recommendations(trends, efficiency)
        }
        
        return report
    
    def generate_recommendations(self, trends, efficiency):
        """Generate performance optimization recommendations."""
        recommendations = []
        
        # Find best configuration
        best_config = max(efficiency.items(), key=lambda x: x[1]['efficiency_score'])
        best_name, best_data = best_config
        
        # Configuration-specific recommendations
        neuron_count = best_data['neuron_count']
        
        if neuron_count < 2000:
            recommendations.append({
                'type': 'scale_up',
                'description': f'Consider increasing neuron count to {neuron_count + 1000} for better performance',
                'expected_improvement': '10-20% performance gain',
                'priority': 'Medium'
            })
        elif neuron_count > 8000:
            recommendations.append({
                'type': 'scale_down',
                'description': f'Consider decreasing neuron count to {neuron_count - 2000} for better performance',
                'expected_improvement': '15-25% performance gain',
                'priority': 'Medium'
            })
        
        # Firing rate recommendations
        avg_firing_rate = np.mean([d['firing_rate'] for d in efficiency.values()])
        if avg_firing_rate < 20:
            recommendations.append({
                'type': 'increase_plasticity',
                'description': 'Increase plasticity learning rates for better engagement',
                'expected_improvement': '5-15% firing rate increase',
                'priority': 'Low'
            })
        
        # Performance recommendations
        avg_performance = np.mean([d['steps_per_second'] for d in efficiency.values()])
        if avg_performance < 1000:
            recommendations.append({
                'type': 'optimize_performance',
                'description': 'Enable advanced optimizations (vectorization, parallel processing)',
                'expected_improvement': '50-200% performance gain',
                'priority': 'High'
            })
        
        return recommendations

# Example usage
analyzer = PerformanceAnalyzer("benchmark_results.json")

# Generate performance report
report = analyzer.generate_performance_report()

print("Performance Analysis Report:")
print(f"Total benchmarks: {report['summary']['total_benchmarks']}")
print(f"Average performance: {report['summary']['avg_performance']:.2f} steps/second")
print(f"Best configuration: {report['summary']['best_configuration']}")
print(f"Performance trend: {report['summary']['performance_trend']}")

print(f"\nRecommendations ({len(report['recommendations'])}):")
for i, recommendation in enumerate(report['recommendations'], 1):
    print(f"{i}. {recommendation['type'].upper()}: {recommendation['description']}")
    print(f"   Expected: {recommendation['expected_improvement']} (Priority: {recommendation['priority']})")

# Generate performance plots
fig = analyzer.plot_performance_comparison()
if fig:
    fig.savefig("performance_analysis.png")
    print(f"\nPerformance analysis plot saved to: performance_analysis.png")

# Save report
with open("performance_analysis_report.json", 'w') as f:
    json.dump(report, f, indent=2)

print("Performance analysis report saved to: performance_analysis_report.json")
```

#### Shell Script Performance Analysis

```bash
#!/bin/bash

# Performance analysis script for NLM

# Configuration
NLM_EXEC="./nlm"
ANALYSIS_SCRIPT="analyze_performance.py"
RESULTS_FILE="benchmark_results.json"
REPORT_FILE="performance_analysis_report.json"
PLOT_FILE="performance_analysis.png"

# Create analysis script
python3 << EOF
import json
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
from datetime import datetime

class NLMPerformanceAnalyzer:
    def __init__(self, benchmark_file):
        self.benchmark_file = benchmark_file
        self.results = self.load_results()
    
    def load_results(self):
        """Load benchmark results from file."""
        with open(self.benchmark_file, 'r') as f:
            return json.load(f)['results']
    
    def analyze_performance_trends(self):
        """Analyze performance trends."""
        trends = {}
        
        # Extract metrics
        for result in self.results:
            config = result['config']
            perf = result['performance']
            
            config_key = f"{config['neuron_count']}_neurons"
            
            if config_key not in trends:
                trends[config_key] = {
                    'steps_per_second': [],
                    'firing_rates': [],
                    'neuron_counts': []
                }
            
            trends[config_key]['steps_per_second'].append(perf['steps_per_second'])
            trends[config_key]['firing_rates'].append(perf['avg_firing_rate'])
            trends[config_key]['neuron_counts'].append(config['neuron_count'])
        
        # Calculate statistics
        analysis = {}
        for config_key, data in trends.items():
            analysis[config_key] = {
                'avg_steps_per_second': np.mean(data['steps_per_second']),
                'std_steps_per_second': np.std(data['steps_per_second']),
                'avg_firing_rate': np.mean(data['firing_rates']),
                'std_firing_rate': np.std(data['firing_rates']),
                'neuron_count': data['neuron_counts'][0]
            }
        
        return analysis
    
    def plot_performance_comparison(self):
        """Plot performance comparison."""
        if not plt:
            return None
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle('NLM Performance Analysis', fontsize=16)
        
        # Prepare data
        neuron_counts = [r['config']['neuron_count'] for r in self.results]
        steps_per_second = [r['performance']['steps_per_second'] for r in self.results]
        firing_rates = [r['performance']['avg_firing_rate'] for r in self.results]
        
        # Plot 1: Steps per second vs neuron count
        axes[0, 0].scatter(neuron_counts, steps_per_second, alpha=0.7)
        axes[0, 0].set_xlabel('Neuron Count')
        axes[0, 0].set_ylabel('Steps per Second')
        axes[0, 0].set_title('Performance vs Neuron Count')
        axes[0, 0].grid(True)
        
        # Plot 2: Firing rate vs neuron count
        axes[0, 1].scatter(neuron_counts, firing_rates, alpha=0.7)
        axes[0, 1].set_xlabel('Neuron Count')
        axes[0, 1].set_ylabel('Firing Rate (Hz)')
        axes[0, 1].set_title('Firing Rate vs Neuron Count')
        axes[0, 1].grid(True)
        
        # Plot 3: Performance correlation matrix
        correlation_matrix = np.corrcoef([steps_per_second, firing_rates, neuron_counts])
        sns.heatmap(correlation_matrix, annot=True, cmap='coolwarm', center=0,
                   xticklabels=['Steps/s', 'Firing Rate', 'Neuron Count'],
                   yticklabels=['Steps/s', 'Firing Rate', 'Neuron Count'],
                   ax=axes[1, 0])
        axes[1, 0].set_title('Performance Correlation Matrix')
        
        # Plot 4: Performance distribution
        axes[1, 1].hist(steps_per_second, bins=10, alpha=0.7, edgecolor='black')
        axes[1, 1].set_xlabel('Steps per Second')
        axes[1, 1].set_ylabel('Frequency')
        axes[1, 1].set_title('Performance Distribution')
        axes[1, 1].grid(True)
        
        plt.tight_layout()
        return fig
    
    def generate_analysis_report(self):
        """Generate performance analysis report."""
        trends = self.analyze_performance_trends()
        
        report = {
            'benchmark_name': 'NLM_Performance_Analysis',
            'timestamp': datetime.now().isoformat(),
            'summary': {
                'total_benchmarks': len(self.results),
                'avg_performance': np.mean([r['performance']['steps_per_second'] for r in self.results]),
                'performance_trend': 'increasing' if len(self.results) > 1 and self.results[-1]['performance']['steps_per_second'] > self.results[0]['performance']['steps_per_second'] else 'decreasing'
            },
            'trends': trends,
            'recommendations': self.generate_recommendations(trends)
        }
        
        return report
    
    def generate_recommendations(self, trends):
        """Generate performance optimization recommendations."""
        recommendations = []
        
        # Find configuration with best performance
        best_config = max(trends.items(), key=lambda x: x[1]['avg_steps_per_second'])
        best_name, best_data = best_config
        
        # Extract neuron count from config key
        neuron_count = int(best_name.replace('_neurons', ''))
        
        if neuron_count < 2000:
            recommendations.append({
                'type': 'scale_up',
                'description': f'Consider increasing neuron count to {neuron_count + 1000} for better performance',
                'expected_improvement': '10-20% performance gain',
                'priority': 'Medium'
            })
        elif neuron_count > 8000:
            recommendations.append({
                'type': 'scale_down',
                'description': f'Consider decreasing neuron count to {neuron_count - 2000} for better performance',
                'expected_improvement': '15-25% performance gain',
                'priority': 'Medium'
            })
        
        # Performance recommendations
        avg_performance = np.mean([data['avg_steps_per_second'] for data in trends.values()])
        if avg_performance < 1000:
            recommendations.append({
                'type': 'optimize_performance',
                'description': 'Enable advanced optimizations (vectorization, parallel processing)',
                'expected_improvement': '50-200% performance gain',
                'priority': 'High'
            })
        
        return recommendations

# Generate analysis report
analyzer = NLMPerformanceAnalyzer("$RESULTS_FILE")
report = analyzer.generate_analysis_report()

print("Performance Analysis Report:")
print(f"Total benchmarks: {report['summary']['total_benchmarks']}")
print(f"Average performance: {report['summary']['avg_performance']:.2f} steps/second")
print(f"Performance trend: {report['summary']['performance_trend']}")

print(f"\nRecommendations ({len(report['recommendations'])}):")
for i, recommendation in enumerate(report['recommendations'], 1):
    print(f"{i}. {recommendation['type'].upper()}: {recommendation['description']}")
    print(f"   Expected: {recommendation['expected_improvement']} (Priority: {recommendation['priority']})")

# Generate performance plots
fig = analyzer.plot_performance_comparison()
if fig:
    fig.savefig("$PLOT_FILE")
    print(f"\nPerformance analysis plot saved to: $PLOT_FILE")

# Save report
with open("$REPORT_FILE", 'w') as f:
    json.dump(report, f, indent=2)

print("Performance analysis report saved to: $REPORT_FILE")
EOF

echo "Performance analysis completed!"
```

## Performance Best Practices

### Performance Optimization Checklist

#### Configuration Optimization
- [ ] Use optimal neuron count for your use case (1000-5000 for most simulations)
- [ ] Enable all performance optimizations (vectorization, parallel processing)
- [ ] Configure memory pool size appropriately
- [ ] Use efficient plasticity settings
- [ ] Optimize simulation parameters

#### Runtime Optimization
- [ ] Monitor simulation performance continuously
- [ ] Use appropriate timestep for your simulation
- [ ] Enable world optimization where applicable
- [ ] Monitor and optimize memory usage
- [ ] Use efficient agent configuration

#### Analysis Optimization
- [ ] Use appropriate benchmarking techniques
- [ ] Compare configurations systematically
- [ ] Analyze performance trends over time
- [ ] Generate performance reports regularly
- [ ] Set up performance alerts and warnings

### Performance Monitoring Best Practices

1. **Start monitoring early**: Begin monitoring before running simulations
2. **Monitor continuously**: Monitor throughout the entire simulation
3. **Record baseline**: Record baseline performance before optimization
4. **Document changes**: Document all performance changes
5. **Set alerts**: Set up alerts for performance issues
6. **Regular analysis**: Analyze performance data regularly
7. **Benchmark regularly**: Run benchmarks regularly

### Performance Optimization Best Practices

1. **Start with basic optimizations**: Begin with basic optimizations
2. **Measure before optimizing**: Measure performance before optimizing
3. **Optimize one at a time**: Optimize one aspect at a time
4. **Test thoroughly**: Test optimizations thoroughly
5. **Document optimizations**: Document all optimizations
6. **Set performance targets**: Set clear performance targets
7. **Monitor impact**: Monitor impact of optimizations

## Performance Troubleshooting

### Common Performance Issues

#### "Simulation is too slow"

**Symptoms:**
- Low steps per second (< 1000)
- High memory usage
- Long simulation times

**Solutions:**
```bash
# Enable performance optimizations
./nlm config set --key=performance.optimization_level --value=3
./nlm config set --key=performance.parallel_processing --value=true
./nlm config set --key=performance.enable_vectorization --value=true

# Reduce complexity
./nlm config set --key=performance.max_steps --value=5000
./nlm config set --key=plasticity.stdp.learning_rate --value=0.0005
```

#### "Memory usage is too high"

**Symptoms:**
- High memory usage (> 2 GB)
- Memory allocation failures
- System slowdown

**Solutions:**
```bash
# Enable memory pooling
./nlm config set --key=performance.memory_pooling --value=true

# Configure memory pool size
./nlm config set --key=performance.memory_pool_size --value=2048

# Reduce simulation complexity
./nlm config set --key=performance.max_steps --value=2000
```

#### "Firing rate is too low"

**Symptoms:**
- Low firing rates (< 10 Hz)
- Neurons not firing
- Simulation appears stuck

**Solutions:**
```bash
# Increase excitability
./nlm config set --key=brain.v_thresh --value=-40.0
./nlm config set --key=brain.initial_weight_mean --value=0.7

# Increase plasticity
./nlm config set --key=performance.optimization_level --value=3
```

#### "Performance is inconsistent"

**Symptoms:**
- Variable steps per second
- Inconsistent firing rates
- Unpredictable simulation times

**Solutions:**
```bash
# Enable deterministic simulation
./nlm config set --key=simulation.random_seed --value=42
./nlm config set --key=performance.optimization_level --value=3

# Monitor performance continuously
./nlm performance monitor --duration=60 --interval=5
```

### Performance Troubleshooting Scripts

#### Python Performance Troubleshooting

```python
import pynlm
import json
from typing import Dict, List, Any

class PerformanceTroubleshooter:
    def __init__(self):
        self.common_issues = {
            'slow_simulation': {
                'symptoms': ['low_steps_per_second', 'high_memory_usage', 'long_simulation_time'],
                'solutions': [
                    'Enable parallel processing',
                    'Enable vectorization',
                    'Reduce simulation complexity',
                    'Optimize memory usage'
                ]
            },
            'high_memory': {
                'symptoms': ['high_memory_usage', 'memory_allocation_failures', 'system_slowdown'],
                'solutions': [
                    'Enable memory pooling',
                    'Configure optimal memory pool size',
                    'Reduce simulation complexity',
                    'Optimize plasticity settings'
                ]
            },
            'low_firing_rate': {
                'symptoms': ['low_firing_rate', 'neurons_not_firing', 'simulation_stuck'],
                'solutions': [
                    'Increase neuronal excitability',
                    'Increase plasticity',
                    'Optimize synaptic weights'
                ]
            }
        }
    
    def diagnose_performance_issue(self, performance_data):
        """Diagnose performance issue based on performance data."""
        issues = []
        
        # Check for slow simulation
        if performance_data.get('steps_per_second', 0) < 1000:
            issues.append('slow_simulation')
        
        # Check for high memory usage
        if performance_data.get('memory_usage', 0) > 2000:  # MB
            issues.append('high_memory')
        
        # Check for low firing rate
        if performance_data.get('firing_rate', 0) < 10:  # Hz
            issues.append('low_firing_rate')
        
        return issues
    
    def get_troubleshooting_guide(self, issue):
        """Get troubleshooting guide for specific issue."""
        if issue in self.common_issues:
            return self.common_issues[issue]
        
        return None
    
    def troubleshoot_performance(self, performance_data):
        """Troubleshoot performance issues."""
        # Diagnose issues
        issues = self.diagnose_performance_issue(performance_data)
        
        if not issues:
            return {
                'status': 'healthy',
                'issues': [],
                'recommendations': []
            }
        
        # Get troubleshooting guides
        troubleshooting_guides = [self.get_troubleshooting_guide(issue) for issue in issues]
        
        # Collect recommendations
        recommendations = []
        for guide in troubleshooting_guides:
            if guide:
                recommendations.extend(guide['solutions'])
        
        # Remove duplicates
        recommendations = list(set(recommendations))
        
        return {
            'status': 'needs_attention',
            'issues': issues,
            'recommendations': recommendations
        }

# Example usage
troubleshooter = PerformanceTroubleshooter()

# Example performance data
performance_data = {
    'steps_per_second': 500,
    'memory_usage': 3000,  # MB
    'firing_rate': 5  # Hz
}

# Troubleshoot performance
result = troubleshooter.troubleshoot_performance(performance_data)

print(f"Performance Status: {result['status']}")
print(f"Issues: {result['issues']}")
print(f"Recommendations: {result['recommendations']}")
```

#### Shell Script Troubleshooting

```bash
#!/bin/bash

# Performance troubleshooting script for NLM

# Configuration
NLM_EXEC="./nlm"
PERF_DATA_FILE="performance_data.json"
TROUBLESHOOT_SCRIPT="troubleshoot_performance.py"

# Create troubleshooting script
python3 << EOF
import json
from typing import Dict, List, Any

class NLMTroubleshooter:
    def __init__(self):
        self.common_issues = {
            'slow_simulation': {
                'symptoms': ['low_steps_per_second', 'high_memory_usage', 'long_simulation_time'],
                'solutions': [
                    'Enable parallel processing: ./nlm config set --key=performance.parallel_processing --value=true',
                    'Enable vectorization: ./nlm config set --key=performance.enable_vectorization --value=true',
                    'Reduce simulation complexity: ./nlm config set --key=performance.max_steps --value=2000',
                    'Optimize memory usage: ./nlm config set --key=performance.memory_pooling --value=true'
                ]
            },
            'high_memory': {
                'symptoms': ['high_memory_usage', 'memory_allocation_failures', 'system_slowdown'],
                'solutions': [
                    'Enable memory pooling: ./nlm config set --key=performance.memory_pooling --value=true',
                    'Configure memory pool: ./nlm config set --key=performance.memory_pool_size --value=2048',
                    'Reduce simulation complexity: ./nlm config set --key=performance.max_steps --value=1000',
                    'Optimize plasticity: ./nlm config set --key=plasticity.stdp.learning_rate --value=0.0001'
                ]
            },
            'low_firing_rate': {
                'symptoms': ['low_firing_rate', 'neurons_not_firing', 'simulation_stuck'],
                'solutions': [
                    'Increase excitability: ./nlm config set --key=brain.v_thresh --value=-40.0',
                    'Increase plasticity: ./nlm config set --key=performance.optimization_level --value=3',
                    'Optimize synaptic weights: ./nlm config set --key=brain.initial_weight_mean --value=0.7'
                ]
            }
        }
    
    def diagnose_performance_issue(self, performance_data):
        """Diagnose performance issue based on performance data."""
        issues = []
        
        # Check for slow simulation
        if performance_data.get('steps_per_second', 0) < 1000:
            issues.append('slow_simulation')
        
        # Check for high memory usage
        if performance_data.get('memory_usage', 0) > 2000:  # MB
            issues.append('high_memory')
        
        # Check for low firing rate
        if performance_data.get('firing_rate', 0) < 10:  # Hz
            issues.append('low_firing_rate')
        
        return issues
    
    def get_troubleshooting_guide(self, issue):
        """Get troubleshooting guide for specific issue."""
        if issue in self.common_issues:
            return self.common_issues[issue]
        
        return None
    
    def troubleshoot_performance(self, performance_data):
        """Troubleshoot performance issues."""
        # Diagnose issues
        issues = self.diagnose_performance_issue(performance_data)
        
        if not issues:
            return {
                'status': 'healthy',
                'issues': [],
                'recommendations': []
            }
        
        # Get troubleshooting guides
        troubleshooting_guides = [self.get_troubleshooting_guide(issue) for issue in issues]
        
        # Collect recommendations
        recommendations = []
        for guide in troubleshooting_guides:
            if guide:
                recommendations.extend(guide['solutions'])
        
        # Remove duplicates
        recommendations = list(set(recommendations))
        
        return {
            'status': 'needs_attention',
            'issues': issues,
            'recommendations': recommendations
        }

# Load performance data
with open("$PERF_DATA_FILE", 'r') as f:
    perf_data = json.load(f)

# Troubleshoot performance
troubleshooter = NLMTroubleshooter()
result = troubleshooter.troubleshoot_performance(perf_data)

print("Performance Troubleshooting Report:")
print(f"Status: {result['status']}")
print(f"Issues: {result['issues']}")

if result['recommendations']:
    print(f"\nRecommendations ({len(result['recommendations'])}):")
    for i, recommendation in enumerate(result['recommendations'], 1):
        print(f"{i}. {recommendation}")

# Generate troubleshooting report
troubleshooting_report = {
    'benchmark_name': 'NLM_Performance_Troubleshooting',
    'timestamp': datetime.now().isoformat(),
    'status': result['status'],
    'issues': result['issues'],
    'recommendations': result['recommendations']
}

# Save troubleshooting report
with open("performance_troubleshooting_report.json", 'w') as f:
    json.dump(troubleshooting_report, f, indent=2)

print(f"\nTroubleshooting report saved to: performance_troubleshooting_report.json")
EOF

echo "Performance troubleshooting completed!"
```

## Performance Support and Resources

### Getting Help

- **Documentation**: `docs/PERFORMANCE_GUIDE.md`
- **API Reference**: `docs/API_REFERENCE.md`
- **Command Line Guide**: `docs/COMMAND_LINE_GUIDE.md`
- **Configuration Guide**: `docs/CONFIGURATION_GUIDE.md`
- **Experiment Guide**: `docs/EXPERIMENT_GUIDE.md`

### Additional Resources

- **Performance Monitoring**: `performance/` directory
- **Performance Optimization**: `optimization/` directory
- **Benchmarking**: `benchmarking/` directory
- **Performance Analysis**: `analysis/` directory
- **Troubleshooting**: `troubleshooting/` directory

## Performance Best Practices Summary

### Configuration Best Practices

1. **Use optimal neuron count**: 1000-5000 neurons for most simulations
2. **Enable all optimizations**: Use parallel processing, vectorization, and memory pooling
3. **Configure properly**: Set appropriate memory pool size and thread count
4. **Monitor performance**: Monitor performance continuously during simulations

### Runtime Best Practices

1. **Start monitoring early**: Begin monitoring before running simulations
2. **Record baseline**: Record baseline performance before optimization
3. **Document changes**: Document all performance changes
4. **Set alerts**: Set up alerts for performance issues
5. **Benchmark regularly**: Run benchmarks regularly

### Analysis Best Practices

1. **Use appropriate techniques**: Use appropriate benchmarking and analysis techniques
2. **Compare configurations**: Compare configurations systematically
3. **Analyze trends**: Analyze performance trends over time
4. **Generate reports**: Generate performance reports regularly
5. **Set targets**: Set clear performance targets

## Performance Optimization Workflow

### Phase 1: Baseline Measurement

1. Configure NLM with basic settings
2. Run baseline simulation
3. Record performance metrics
4. Document baseline performance

### Phase 2: Optimization

1. Identify performance bottlenecks
2. Apply optimizations
3. Test performance improvements
4. Document optimizations

### Phase 3: Monitoring

1. Set up continuous monitoring
2. Monitor performance during simulations
3. Set up alerts for performance issues
4. Analyze performance data

### Phase 4: Analysis

1. Analyze performance trends
2. Compare different configurations
3. Generate performance reports
4. Set up performance optimization recommendations

## Conclusion

NLM performance optimization is a critical aspect of NLM that affects simulation speed, memory usage, and overall efficiency. This guide provides comprehensive documentation for NLM performance optimization, including performance monitoring, optimization techniques, benchmarking, and performance analysis.

Effective performance management is essential for:

1. **Research and experimentation**: Optimize NLM performance for different research scenarios
2. **Performance optimization**: Optimize NLM performance through configuration and techniques
3. **Reproducibility**: Ensure reproducible experiments through performance monitoring
4. **Automation**: Automate performance optimization through scripts
5. **Testing**: Test performance through benchmarking

This guide provides the knowledge and tools needed to effectively manage NLM performance. For more advanced performance tasks, refer to the API reference, command-line guide, and other documentation files.