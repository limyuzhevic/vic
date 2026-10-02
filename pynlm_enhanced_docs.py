# NLM Python Enhanced Documentation

This document provides comprehensive documentation for using NLM with Python bindings,
including both basic usage and advanced features.

## Overview

NLM (Neural Learning Machine) is an experimental computational brain simulator that
allows you to create virtual brains with learning capabilities. The Python bindings
provide a user-friendly interface to the NLM C++ framework.

## Installation and Setup

### Building NLM with Python Bindings

1. Clone the repository
2. Create build directory:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

3. The Python module `pynlm` will be built and installed

### Python Package Installation

```bash
# After building the C++ extension
pip install pynlm  # if you have a setup.py file
# OR install the built extension directly
cp python/*.so /usr/local/lib/python3.x/site-packages/pynlm/
```

## Quick Start Examples

### 1. Basic Brain Creation and Simulation

```python
import pynlm
import numpy as np

# Create a basic brain with default configuration
brain = pynlm.create_brain(pynlm.create_default_config())

# Initialize the brain
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)
    print(f"Step {step}: Neurons firing = {brain.get_firing_neuron_count()}")

print(f"Total spikes: {brain.get_total_spike_count()}")
print(f"Average firing rate: {brain.get_average_firing_rate():.2f}")
```

### 2. Brain with World and Agent

```python
import pynlm
import pynlm_helper

# Create an advanced experiment setup
experiment = pynlm_helper.create_experiment_setup(
    world_width=20,
    world_height=20,
    vision_width=8,
    vision_height=8
)

brain = experiment["brain"]
world = experiment["world"]
agent = experiment["agent"]

# Run a learning simulation
results = pynlm_helper.run_learning_simulation(
    experiment,
    steps=500,
    enable_checkpointing=True,
    checkpoint_path="./nlm_checkpoint.bin"
)

print(f"Simulation completed: {results['steps_completed']} steps")
print(f"Total reward: {results['total_reward']:.2f}")
print(f"Final firing rate: {results['avg_firing_rate']:.2f}")
print(f"Memory growth: {results['memory_growth']:.2f}")
```

### 3. Research Configuration Setup

```python
import pynlm

# Create different experiment types
configs = {
    'standard': pynlm.create_research_config("standard", neuron_count=1000),
    'social': pynlm.create_research_config("social", neuron_count=2000),
    'curiosity': pynlm.create_research_config("curiosity", neuron_count=1500),
    'development': pynlm.create_research_config("development", neuron_count=1200)
}

# Use a specific configuration
config = configs['curiosity']
brain = pynlm.create_brain_with_config(config)

# The brain now has curiosity-specific settings
print("Brain initialized with curiosity settings")
```

## Advanced Features

### Brain State Analysis

```python
import pynlm

brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Run a simulation
for step in range(100):
    brain.step(step)

# Analyze brain state
analysis = pynlm.analyze_brain_state(brain)

print("=== Brain State Analysis ===")
print(f"Total neurons: {analysis['neuron_stats']['total']}")
print(f"Active neurons: {analysis['neuron_stats']['active']}")
print(f"Firing neurons: {analysis['neuron_stats']['firing']}")
print(f"Average firing rate: {analysis['neuron_stats']['average_firing_rate']:.2f}")
print(f"Excitatory/inhibitory ratio: {analysis['neuron_stats']['excitatory_ratio']:.2f}")

if analysis['memory_stats']['working_memory_traces'] > 0:
    print(f"Working memory traces: {analysis['memory_stats']['working_memory_traces']}")

if analysis['memory_stats']['episodic_episodes'] > 0:
    print(f"Episodic memory episodes: {analysis['memory_stats']['episodic_episodes']}")

if analysis['neuromodulation']['curiosity_level'] > 0:
    print(f"Curiosity level: {analysis['neuromodulation']['curiosity_level']:.2f}")

if analysis['neuromodulation']['novelty_level'] > 0:
    print(f"Novelty level: {analysis['neuromodulation']['novelty_level']:.2f}")
```

### Performance Benchmarking

```python
import pynlm
import time

# Create experiment setup
experiment = pynlm.create_advanced_experiment_setup()

# Run performance benchmark
benchmark = pynlm.benchmark_performance(
    experiment,
    durations=[0.1, 0.5, 1.0, 2.0]  # Test different durations
)

print("=== Performance Benchmark ===")
for bench in benchmark['benchmarks']:
    print(f"Duration: {bench['duration_sec']}s")
    print(f"  Steps per second: {bench['steps_per_second']}")
    print(f"  Average firing rate: {bench['firing_rate_avg']:.2f}")
    print(f"  Neurons active: {bench['neurons_active']}")
    print()

print("=== Performance Scaling ===")
scaling = benchmark['performance_scaling']
print(f"Time scaling: {scaling['time_scaling']:.2f}")
print(f"Memory scaling: {scaling['memory_scaling']:.2f}")
print(f"Efficiency scaling: {scaling['efficiency_scaling']:.2f}")

print("\n=== Bottleneck Analysis ===")
bottleneck = benchmark['bottleneck_analysis']
print(f"Max time per step: {bottleneck['max_time_per_step_ms']:.2f} ms")
print(f"Recommended max steps: {bottleneck['recommended_max_steps']}")
print(f"Performance rating: {bottleneck['performance_rating']}")
```

### Learning Experiment with Visualization

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np

# Create experiment setup
experiment = pynlm.create_advanced_experiment_setup()
brain = experiment["brain"]
world = experiment["world"]
agent = experiment["agent"]

# Run learning simulation with data collection
steps = 1000
firing_rates = []
rewards = []
learning_events = []

for step in range(steps):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Collect metrics
    firing_rates.append(brain.getAverageFiringRate())
    rewards.append(0.0)  # Simplified reward collection
    
    # Check for learning events
    try:
        learning_events.append(agent.getPredictionError())
    except:
        learning_events.append(0.0)

# Create visualization
fig, axes = plt.subplots(2, 2, figsize=(12, 10))

# Plot 1: Firing rate over time
axes[0, 0].plot(firing_rates)
axes[0, 0].set_title('Average Firing Rate Over Time')
axes[0, 0].set_xlabel('Step')
axes[0, 0].set_ylabel('Firing Rate')
axes[0, 0].grid(True, alpha=0.3)

# Plot 2: Learning events (prediction error)
axes[0, 1].plot(learning_events)
axes[0, 1].set_title('Learning Events (Prediction Error)')
axes[0, 1].set_xlabel('Step')
axes[0, 1].set_ylabel('Prediction Error')
axes[0, 1].grid(True, alpha=0.3)

# Plot 3: Reward over time
axes[1, 0].plot(rewards)
axes[1, 0].set_title('Reward Over Time')
axes[1, 0].set_xlabel('Step')
axes[1, 0].set_ylabel('Reward')
axes[1, 0].grid(True, alpha=0.3)

# Plot 4: Brain state analysis
analysis = pynlm.analyze_brain_state(brain)
if analysis['memory_stats']['episodic_episodes'] > 0:
    axes[1, 1].bar(['Working Memory', 'Episodic Memory'], 
                   [analysis['memory_stats']['working_memory_traces'],
                    analysis['memory_stats']['episodic_episodes']])
    axes[1, 1].set_title('Memory Statistics')
    axes[1, 1].set_ylabel('Count')

plt.tight_layout()
plt.savefig('nlm_learning_results.png', dpi=150)
print("Learning visualization saved as 'nlm_learning_results.png'")

# Print final analysis
final_analysis = pynlm.analyze_brain_state(brain)
print("\n=== Final Analysis ===")
print(f"Total neurons: {final_analysis['neuron_stats']['total']}")
print(f"Firing neurons: {final_analysis['neuron_stats']['firing']}")
print(f"Total spikes: {final_analysis['neuron_stats']['total_spikes']}")
print(f"Average firing rate: {final_analysis['neuron_stats']['average_firing_rate']:.2f}")
```

## Configuration Management

### Working with Configuration Files

```python
import pynlm
import json
import os

# Create a configuration
config = pynlm.create_default_config()

# Modify configuration
config.set_value("neuron_count", 1500)
config.set_value("region_count", 2)
config.set_value("connection_probability", 0.08)
config.set_value("stdp_ltp_weight", 0.025)

# Save configuration to file
config.save_to_file("my_research_config.cfg")

# Load configuration from file
config2 = pynlm.create_default_config()
config2.load_from_file("my_research_config.cfg")

# Get configuration summary
print(config2.summary())
```

### Advanced Configuration Examples

```python
def create_social_research_config(neuron_count=2000, empathy_rate=0.3):
    """Create a configuration optimized for social learning research."""
    config = pynlm.create_research_config("social", neuron_count=neuron_count)
    config.set_value("social_coefficient", empathy_rate)
    config.set_value("empathy_rate", empathy_rate)
    config.set_value("imitation_probability", 0.4)
    config.set_value("mirror_neuron_activation", True)
    return config

def create_curiosity_research_config(neuron_count=1500):
    """Create a configuration optimized for curiosity-driven learning."""
    config = pynlm.create_research_config("curiosity", neuron_count=neuron_count)
    config.set_value("curiosity_threshold", 0.5)
    config.set_value("exploration_bonus", 0.3)
    config.set_value("intrinsic_reward_scale", 0.2)
    return config
```

## Error Handling and Debugging

### Exception Handling

```python
import pynlm

try:
    # Create brain with invalid configuration
    config = pynlm.create_default_config()
    config.set_value("invalid_key", "invalid_value")  # This might cause issues
    
    brain = pynlm.create_brain(config)
    brain.initialize()
    
except pynlm.RuntimeError as e:
    print(f"NLM Runtime Error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")

# Use helper functions for error recovery
import pynlm_helper

try:
    experiment = pynlm_helper.create_experiment_setup()
    results = pynlm_helper.run_learning_simulation(experiment, steps=100)
    
    if "error" in results:
        print(f"Simulation failed: {results['error']}")
        # Recovery strategy
        experiment = pynlm_helper.create_experiment_setup(neuron_count=500)
        results = pynlm_helper.run_learning_simulation(experiment, steps=50)
        
except Exception as e:
    print(f"Setup failed: {e}")
    print("Consider using simpler configuration or smaller neuron count.")
```

## Performance Optimization

### Efficient Simulation Patterns

```python
import pynlm
import pynlm_helper

def optimize_simulation_for_performance(experiment, target_steps=1000):
    """Optimize simulation for better performance."""
    
    # Benchmark to understand performance characteristics
    benchmark = pynlm.benchmark_performance(experiment, durations=[0.5])
    
    max_time_per_step = benchmark['bottleneck_analysis']['max_time_per_step_ms'] / 1000
    recommended_steps = benchmark['bottleneck_analysis']['recommended_max_steps']
    
    print(f"Performance: {max_time_per_step*1000:.2f}ms per step")
    print(f"Recommended max steps: {recommended_steps}")
    
    # Adjust simulation parameters based on performance
    if max_time_per_step > 0.001:  # If slower than 1ms per step
        print("Warning: Simulation slower than optimal. Consider:")
        print("  1. Reducing neuron count")
        print("  2. Disabling complex learning mechanisms")
        print("  3. Reducing simulation duration")
        
        # Create simpler experiment
        experiment = pynlm_helper.create_experiment_setup(neuron_count=500)
    
    return experiment

# Use optimized simulation
experiment = pynlm.create_advanced_experiment_setup()
optimized_experiment = optimize_simulation_for_performance(experiment, target_steps=500)
```

### Memory Management

```python
import pynlm
import gc
import psutil
import os

def monitor_memory_usage(experiment, steps=100):
    """Monitor memory usage during simulation."""
    process = psutil.Process(os.getpid())
    memory_samples = []
    
    brain = experiment["brain"]
    world = experiment["world"]
    agent = experiment["agent"]
    
    for step in range(steps):
        # Get memory before step
        memory_samples.append(process.memory_info().rss / 1024 / 1024)  # MB
        
        # Run simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Force garbage collection periodically
        if step % 50 == 0:
            gc.collect()
    
    return memory_samples

# Run memory monitoring
experiment = pynlm.create_advanced_experiment_setup(neuron_count=800)
memory_samples = monitor_memory_usage(experiment, steps=200)

print(f"Memory usage over time (MB): {memory_samples}")
print(f"Memory growth: {memory_samples[-1] - memory_samples[0]:.2f} MB")
```

## Advanced Python Features

### Parallel Processing

```python
import pynlm
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
import time

def run_independent_simulation(config_id, neuron_count=500):
    """Run an independent simulation for parallel processing."""
    config = pynlm.create_research_config("standard", neuron_count=neuron_count)
    brain = pynlm.create_brain(config)
    
    for step in range(100):
        brain.step(step)
    
    return {
        'config_id': config_id,
        'total_spikes': brain.get_total_spike_count(),
        'avg_firing_rate': brain.get_average_firing_rate(),
        'final_neurons': brain.get_firing_neuron_count()
    }

# Run simulations in parallel
if __name__ == "__main__":
    configs = list(range(10))  # Run 10 simulations
    
    # Use ThreadPoolExecutor for lightweight parallel execution
    with ThreadPoolExecutor(max_workers=4) as executor:
        results = list(executor.map(run_independent_simulation, configs))
    
    # Analyze results
    total_spikes = sum(r['total_spikes'] for r in results)
    avg_firing = sum(r['avg_firing_rate'] for r in results) / len(results)
    
    print(f"Parallel simulation completed:")
    print(f"  Total configurations: {len(results)}")
    print(f"  Total spikes across all runs: {total_spikes}")
    print(f"Average firing rate: {avg_firing:.2f}")
```

### Real-time Monitoring

```python
import pynlm
import time
import threading
from datetime import datetime

class NLMRealTimeMonitor:
    """Real-time monitor for NLM simulation."""
    
    def __init__(self, experiment):
        self.experiment = experiment
        self.brain = experiment["brain"]
        self.world = experiment["world"]
        self.agent = experiment["agent"]
        self.monitoring = False
        self.metrics = []
    
    def start_monitoring(self, interval=1.0, duration=60.0):
        """Start real-time monitoring."""
        self.monitoring = True
        start_time = time.time()
        
        while self.monitoring and (time.time() - start_time) < duration:
            metric = {
                'timestamp': datetime.now(),
                'neurons_firing': self.brain.get_firing_neuron_count(),
                'total_spikes': self.brain.get_total_spike_count(),
                'avg_firing_rate': self.brain.get_average_firing_rate(),
                'world_time': self.world.getSimulationTime(),
                'curiosity_level': 0.0,
                'prediction_error': 0.0
            }
            
            try:
                metric['curiosity_level'] = self.agent.getCuriosityLevel()
                metric['prediction_error'] = self.agent.getPredictionError()
            except:
                pass
            
            self.metrics.append(metric)
            time.sleep(interval)
    
    def stop_monitoring(self):
        """Stop monitoring."""
        self.monitoring = False
    
    def get_summary(self):
        """Get monitoring summary."""
        if not self.metrics:
            return {}
        
        return {
            'total_monitoring_time': len(self.metrics),
            'avg_firing_rate': sum(m['avg_firing_rate'] for m in self.metrics) / len(self.metrics),
            'max_neurons_firing': max(m['neurons_firing'] for m in self.metrics),
            'total_spikes_generated': sum(m['total_spikes'] for m in self.metrics),
            'avg_curiosity_level': sum(m['curiosity_level'] for m in self.metrics) / len(self.metrics),
            'avg_prediction_error': sum(m['prediction_error'] for m in self.metrics) / len(self.metrics)
        }

# Use real-time monitor
experiment = pynlm.create_advanced_experiment_setup()
monitor = NLMRealTimeMonitor(experiment)

print("Starting real-time monitoring (5 seconds)...")
monitor.start_monitoring(interval=0.5, duration=5.0)

summary = monitor.get_summary()
print(f"Monitoring summary: {summary}")
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: "Brain initialization failed"
**Solution:** Check configuration parameters and ensure they are valid. Try using default configuration:

```python
import pynlm

# Use robust configuration creation
config = pynlm.create_default_config()
config.set_value("neuron_count", 500)
config.set_value("region_count", 1)

brain = pynlm.create_brain(config)
if not brain.initialize():
    print("Brain initialization failed. Using default settings...")
    config = pynlm.create_default_config()  # Reset to defaults
    brain = pynlm.create_brain(config)
    brain.initialize()
```

#### Issue 2: High memory usage
**Solution:** Monitor memory and optimize neuron count:

```python
import pynlm_helper
import psutil
import os

# Monitor before creating experiment
process = psutil.Process(os.getpid())
print(f"Initial memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")

# Create experiment with memory monitoring
experiment = pynlm_helper.create_experiment_setup(neuron_count=500)
results = pynlm_helper.run_learning_simulation(experiment, steps=200)

print(f"Memory usage during simulation: {process.memory_info().rss / 1024 / 1024:.2f} MB")
```

#### Issue 3: Slow simulation performance
**Solution:** Benchmark and optimize simulation parameters:

```python
import pynlm

# Create experiment and benchmark
experiment = pynlm.create_advanced_experiment_setup()
benchmark = pynlm.benchmark_performance(experiment, durations=[0.1])

print("Performance analysis:")
print(f"Time per step: {benchmark['bottleneck_analysis']['max_time_per_step_ms']:.2f} ms")
print(f"Performance rating: {benchmark['bottleneck_analysis']['performance_rating']}")

if benchmark['bottleneck_analysis']['performance_rating'] == "NEEDS_OPTIMIZATION":
    print("Consider reducing neuron count or disabling complex features.")
```

### Getting Help

1. **Documentation:** Check this file for examples and advanced usage patterns
2. **Error messages:** NLM provides detailed error messages in English
3. **Community:** For complex issues, consider posting to the NLM research forum
4. **Debugging:** Enable verbose logging in your configuration

## Conclusion

This enhanced Python interface provides a comprehensive toolkit for working with NLM:

- **User-friendly:** Easy to get started with basic examples
- **Advanced features:** Sophisticated tools for research and development
- **Error resilient:** Robust error handling and recovery mechanisms
- **Performance optimized:** Tools to benchmark and optimize simulations
- **Well documented:** Comprehensive examples and troubleshooting guide

The enhanced Python bindings make NLM accessible to beginners while providing powerful tools for advanced researchers and developers.
