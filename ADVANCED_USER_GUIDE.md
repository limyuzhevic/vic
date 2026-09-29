# NLM - Advanced User Documentation

This document provides comprehensive examples and best practices for using NLM (Neural Learning Machine) at advanced levels.

## Table of Contents

1. [Advanced Python API Usage](#advanced-python-api-usage)
2. [Command Line Interface](#command-line-interface)
3. [Configuration Management](#configuration-management)
4. [Experiment Design Patterns](#experiment-design-patterns)
5. [Performance Optimization](#performance-optimization)
6. [Debugging and Monitoring](#debugging-and-monitoring)
7. [Common Patterns and Anti-patterns](#common-patterns-and-anti-patterns)

---

## Advanced Python API Usage

### Complete Agent Simulation Setup

This example shows how to set up a complete agent simulation with Phase 6 features:

```python
import pynlm
import time

# Create advanced configuration with Phase 6 defaults
config = pynlm.createAdvancedConfig()

# Create brain with advanced settings
brain = pynlm.createBrain(config)
brain.initialize()

# Create world with custom settings
world = pynlm.createGridWorld(width=30, height=30, 
                               resourceDensity=0.15, 
                               hazardDensity=0.05)
world.reset()

# Create agent with all advanced features enabled
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Configure for a specific experiment type
pynlm.configureBrainForExperiment(brain, "memory", neurons=2000, steps=8000)

print("✓ Advanced agent simulation setup complete")
print(f"  Brain: {brain.getTotalNeuronCount()} neurons")
print(f"  World: {world.getWidth()}x{world.getHeight()} grid")
```

### Experiment-Specific Configurations

Different experiment types benefit from specific configurations:

```python
# Memory experiment configuration
memory_config = pynlm.createAdvancedConfig()
memory_config.set("neuron_count", 3000)
memory_config.set("region_count", 8)
memory_config.set("connection_probability", 0.12)
memory_config.set("enable_reward_modulation", True)
memory_config.set("enable_structural_plasticity", True)

# Prediction experiment configuration  
prediction_config = pynlm.createAdvancedConfig()
prediction_config.set("neuron_count", 2000)
prediction_config.set("region_count", 3)
prediction_config.set("connection_probability", 0.18)
prediction_config.set("enable_prediction", True)
prediction_config.set("enable_curiosity", True)

# Development experiment configuration
development_config = pynlm.createAdvancedConfig()
development_config.set("neuron_count", 1500)
development_config.set("region_count", 6)
development_config.set("connection_probability", 0.1)
development_config.set("enable_structural_plasticity", True)
development_config.set("enable_development", True)
development_config.set("development_stage_length", 2000)
```

### Monitoring and Logging

Set up comprehensive monitoring for your simulation:

```python
import time
from datetime import datetime

# Create monitoring callback for key metrics
def create_metrics_monitor():
    metrics = {
        'start_time': time.time(),
        'steps_completed': 0,
        'total_reward': 0.0,
        'max_firing_rate': 0.0,
        'min_firing_rate': float('inf'),
        'spike_counts': [],
        'rewards': []
    }
    
    def monitor_callback(step_info):
        metrics['steps_completed'] += 1
        metrics['total_reward'] += step_info['reward']
        
        firing_rate = step_info['firing_rate']
        metrics['spike_counts'].append(step_info['spikes'])
        metrics['rewards'].append(step_info['reward'])
        
        metrics['max_firing_rate'] = max(metrics['max_firing_rate'], firing_rate)
        metrics['min_firing_rate'] = min(metrics['min_firing_rate'], firing_rate)
        
        if step_info['steps_completed'] % 100 == 0:
            print(f"Step {step_info['steps_completed']}: "
                  f"Reward: {metrics['total_reward']/step_info['steps_completed']:.4f}, "
                  f"Firing: {firing_rate:.2f}")
    
    return metrics, monitor_callback

# Usage in simulation
metrics, monitor = create_metrics_monitor()

# Your simulation loop would call monitor()
for step in range(total_steps):
    # ... simulation code ...
    
    monitor({
        'step': step,
        'reward': reward,
        'firing_rate': firing_rate,
        'spikes': spike_count,
        'steps_completed': step + 1
    })

# Analyze results
print(f"Simulation completed in {time.time() - metrics['start_time']:.2f}s")
print(f"Average reward: {metrics['total_reward']/metrics['steps_completed']:.4f}")
print(f"Max firing rate: {metrics['max_firing_rate']:.2f}")
print(f"Min firing rate: {metrics['min_firing_rate']:.2f}")
```

### Batch Experiment Runner

Run multiple experiments efficiently:

```python
import threading
import concurrent.futures

class ExperimentRunner:
    def __init__(self):
        self.results = []
        
    def run_experiment(self, experiment_config, experiment_name):
        """Run a single experiment with given configuration"""
        print(f"Starting {experiment_name}...")
        
        # Create simulation
        brain, world, agent = pynlm.createAgentSimulation(
            configFile=experiment_config.get('config_file', ''),
            worldWidth=experiment_config.get('width', 20),
            worldHeight=experiment_config.get('height', 20),
            visionWidth=experiment_config.get('vision_width', 8),
            visionHeight=experiment_config.get('vision_height', 8)
        )
        
        # Configure based on experiment type
        if experiment_config.get('type') == 'memory':
            pynlm.configureBrainForExperiment(brain, 'memory')
        elif experiment_config.get('type') == 'prediction':
            pynlm.configureBrainForExperiment(brain, 'prediction')
            
        # Run simulation
        steps = experiment_config.get('steps', 2000)
        avg_reward, avg_firing = pynlm.runQuickTest(
            neurons=experiment_config.get('neurons', 500),
            steps=steps
        )
        
        result = {
            'name': experiment_name,
            'avg_reward': avg_reward,
            'avg_firing': avg_firing,
            'steps': steps,
            'neurons': experiment_config.get('neurons', 500)
        }
        
        self.results.append(result)
        print(f"✓ {experiment_name} completed: Reward={avg_reward:.4f}, Firing={avg_firing:.4f}")
        
        return result
    
    def run_experiments_parallel(self, experiment_configs):
        """Run multiple experiments in parallel"""
        with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
            futures = []
            for i, config in enumerate(experiment_configs):
                future = executor.submit(self.run_experiment, config, f"Exp_{i}")
                futures.append(future)
                
            results = [future.result() for future in concurrent.futures.as_completed(futures)]
            return self.results

# Example usage
runner = ExperimentRunner()

experiments = [
    {'type': 'memory', 'neurons': 1000, 'steps': 3000, 'width': 20, 'height': 20},
    {'type': 'prediction', 'neurons': 800, 'steps': 4000, 'width': 25, 'height': 25},
    {'type': 'development', 'neurons': 1200, 'steps': 5000, 'width': 30, 'height': 30},
]

results = runner.run_experiments_parallel(experiments)

# Print summary
print("\n=== EXPERIMENT SUMMARY ===")
for result in results:
    print(f"{result['name']}: Reward={result['avg_reward']:.4f}, "
          f"Firing={result['avg_firing']:.4f}, "
          f"Neurons={result['neurons']}, Steps={result['steps']}")
```

---

## Command Line Interface

The NLM CLI provides a powerful command-line interface for running simulations and managing configurations.

### Installation and Usage

```bash
# Make the CLI executable
chmod +x nlm_cli.py

# Run help
./nlm_cli.py --help

# Create advanced brain
./nlm_cli.py create-advanced-brain 2000

# Create complete agent simulation
./nlm_cli.py create-agent 30 30 8 8

# Run simulation
./nlm_cli.py run-sim 5000

# Run quick test
./nlm_cli.py quick-test

# Configure for memory experiment
./nlm_cli.py experiment memory

# Create custom world
./nlm_cli.py create-world 25 25 0.2 0.1

# Save brain state
./nlm_cli.py save my_brain.bin

# Load brain state
./nlm_cli.py load my_brain.bin

# Display status
./nlm_cli.py status
```

### CLI Example Scripts

Here are useful CLI example scripts for common tasks:

#### quick_run.sh (Run a quick test)
```bash
#!/bin/bash
# Quick run script for NLM testing

echo "Running NLM quick test..."
./nlm_cli.py quick-test --neurons 500 --steps 1000

echo "Quick test completed."
```

#### experiment_memory.sh (Memory experiment)
```bash
#!/bin/bash
# Memory experiment setup and run

echo "Setting up memory experiment..."
./nlm_cli.py create-advanced-brain 3000
./nlm_cli.py experiment memory
./nlm_cli.py run-sim 8000

# Save results
./nlm_cli.py save memory_experiment.bin

echo "Memory experiment completed and saved."
```

#### training_loop.sh (Continuous training)
```bash
#!/bin/bash
# Continuous training loop for RL-style learning

EXPERIMENT_NAME="nlm_training_$(date +%Y%m%d_%H%M%S)"
CONFIG_FILE="${EXPERIMENT_NAME}.cfg"
BRAIN_FILE="${EXPERIMENT_NAME}.bin"

# Create initial brain
./nlm_cli.py create-advanced-brain 2000 > "${CONFIG_FILE}"
./nlm_cli.py create-agent 20 20 8 8

# Training loop
for episode in {1..10}; do
    echo "Episode $episode/10"
    
    ./nlm_cli.py run-sim 2000
    
    # Save after each episode
    ./nlm_cli.py save "${BRAIN_FILE}_episode$episode.bin"
    
    # Use last episode's brain for next episode
    ./nlm_cli.py load "${BRAIN_FILE}_episode$episode.bin"
    
    echo "Episode $episode completed and saved."
done

echo "Training completed!"
echo "Results saved as ${EXPERIMENT_NAME}_episode*.bin"
```

---

## Configuration Management

### Advanced Configuration Patterns

#### 1. Configuration Templates

```python
# Template system for common configuration patterns
class NLMConfigTemplates:
    @staticmethod
    def get_phase6_defaults():
        """Phase 6 integration defaults"""
        config = pynlm.createAdvancedConfig()
        return config
    
    @staticmethod
    def get_memory_experiment():
        """Optimized for memory studies"""
        config = pynlm.createAdvancedConfig()
        config.set("neuron_count", 3000)
        config.set("region_count", 8)
        config.set("connection_probability", 0.12)
        config.set("enable_reward_modulation", True)
        config.set("enable_structural_plasticity", True)
        return config
    
    @staticmethod
    def get_prediction_experiment():
        """Optimized for prediction studies"""
        config = pynlm.createAdvancedConfig()
        config.set("neuron_count", 2000)
        config.set("region_count", 3)
        config.set("connection_probability", 0.18)
        config.set("enable_prediction", True)
        config.set("enable_curiosity", True)
        return config
    
    @staticmethod
    def get_development_experiment():
        """Optimized for development studies"""
        config = pynlm.createAdvancedConfig()
        config.set("neuron_count", 1500)
        config.set("region_count", 6)
        config.set("connection_probability", 0.1)
        config.set("enable_structural_plasticity", True)
        config.set("enable_development", True)
        config.set("development_stage_length", 2000)
        return config

# Usage
memory_config = NLMConfigTemplates.get_memory_experiment()
brain = pynlm.createBrain(memory_config)
```

#### 2. Configuration Comparison and Validation

```python
def validate_config(config, min_neurons=100, max_neurons=10000):
    """Validate configuration parameters"""
    errors = []
    
    # Check neuron count
    neuron_count = config.getOr("neuron_count", 1000)
    if neuron_count < min_neurons:
        errors.append(f"Neuron count {neuron_count} below minimum {min_neurons}")
    elif neuron_count > max_neurons:
        errors.append(f"Neuron count {neuron_count} above maximum {max_neurons}")
    
    # Check region count
    region_count = config.getOr("region_count", 4)
    if region_count < 1:
        errors.append("Region count must be at least 1")
    elif region_count > 20:
        errors.append("Region count cannot exceed 20")
    
    # Check connection probability
    conn_prob = config.getOr("connection_probability", 0.15)
    if conn_prob < 0.0 or conn_prob > 1.0:
        errors.append(f"Connection probability {conn_prob} must be between 0 and 1")
    
    # Check plasticity parameters
    stdp_ltp = config.getOr("stdp_ltp_weight", 0.02)
    stdp_ltd = config.getOr("stdp_ltd_weight", 0.018)
    
    if stdp_ltp < 0.0 or stdp_ltd < 0.0:
        errors.append("STDP weights must be positive")
    
    if stdp_ltp >= stdp_ltd:
        errors.append("STDP LTP weight should be greater than LTD weight")
    
    return errors

# Example validation
config = pynlm.createAdvancedConfig()
config.set("neuron_count", 500)
config.set("region_count", 3)

errors = validate_config(config)
if errors:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")
else:
    print("✓ Configuration is valid")
```

#### 3. Configuration Serialization

```python
class ConfigSerializer:
    @staticmethod
    def to_dict(config):
        """Convert configuration to dictionary"""
        result = {}
        for key in config.getKeys():
            value = config.getOr(key)
            result[key] = value
        return result
    
    @staticmethod
    def from_dict(config_dict):
        """Create configuration from dictionary"""
        config = pynlm.createAdvancedConfig()
        for key, value in config_dict.items():
            config.set(key, value)
        return config
    
    @staticmethod
    def save_json(config, filepath):
        """Save configuration as JSON"""
        data = ConfigSerializer.to_dict(config)
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
    
    @staticmethod
    def load_json(filepath):
        """Load configuration from JSON"""
        with open(filepath, 'r') as f:
            data = json.load(f)
        return ConfigSerializer.from_dict(data)

# Usage
config = pynlm.createAdvancedConfig()
config.set("neuron_count", 1000)
config.set("region_count", 4)

# Save to JSON
ConfigSerializer.save_json(config, "my_config.json")

# Load from JSON
loaded_config = ConfigSerializer.load_json("my_config.json")
```

---

## Experiment Design Patterns

### 1. Controlled Experiment Pattern

```python
def run_controlled_experiment(base_config, experiment_id, num_repeats=5):
    """
    Run a controlled experiment with multiple repetitions
    for statistical significance.
    """
    results = []
    
    for repeat in range(num_repeats):
        print(f"Running repeat {repeat + 1}/{num_repeats}")
        
        # Create fresh configuration for each repeat
        config = pynlm.createAdvancedConfig()
        
        # Apply base configuration
        for key in base_config.getKeys():
            config.set(key, base_config.getOr(key))
        
        # Add repeat-specific variations
        neuron_count = base_config.getOr("neuron_count", 1000)
        config.set("random_seed", repeat * 1000)  # Different seed for each repeat
        
        # Run experiment
        brain, world, agent = pynlm.createAgentSimulation()
        pynlm.configureBrainForExperiment(brain, "memory")
        
        avg_reward, avg_firing = pynlm.runQuickTest(
            neurons=neuron_count, 
            steps=base_config.getOr("steps", 2000)
        )
        
        results.append({
            'repeat': repeat,
            'avg_reward': avg_reward,
            'avg_firing': avg_firing,
            'neuron_count': neuron_count
        })
    
    return results

# Usage
base_config = pynlm.createAdvancedConfig()
base_config.set("neuron_count", 800)
base_config.set("steps", 3000)

experiment_results = run_controlled_experiment(base_config, "memory_study")
```

### 2. Parameter Sweep Pattern

```python
def parameter_sweep(param_ranges, fixed_config=None):
    """
    Run parameter sweep over multiple values
    """
    if fixed_config is None:
        fixed_config = pynlm.createAdvancedConfig()
    
    all_results = []
    
    # Generate all parameter combinations
    from itertools import product
    
    neuron_counts = param_ranges.get('neuron_count', [100, 500, 1000, 2000])
    region_counts = param_ranges.get('region_count', [1, 2, 4, 8])
    connection_probs = param_ranges.get('connection_probability', [0.1, 0.15, 0.2])
    
    param_combinations = list(product(neuron_counts, region_counts, connection_probs))
    
    for i, (neurons, regions, conn_prob) in enumerate(param_combinations):
        print(f"Running parameter set {i + 1}/{len(param_combinations)}")
        print(f"  Neurons: {neurons}, Regions: {regions}, Conn Prob: {conn_prob}")
        
        # Create configuration
        config = pynlm.createAdvancedConfig()
        config.set("neuron_count", neurons)
        config.set("region_count", regions)
        config.set("connection_probability", conn_prob)
        
        # Apply fixed parameters
        if fixed_config:
            for key in fixed_config.getKeys():
                if key not in ['neuron_count', 'region_count', 'connection_probability']:
                    config.set(key, fixed_config.getOr(key))
        
        # Run experiment
        avg_reward, avg_firing = pynlm.runQuickTest(neurons=neurons, steps=1000)
        
        result = {
            'neurons': neurons,
            'regions': regions,
            'connection_probability': conn_prob,
            'avg_reward': avg_reward,
            'avg_firing': avg_firing
        }
        
        all_results.append(result)
        
        # Save intermediate results
        if (i + 1) % 10 == 0:
            filename = f"sweep_results_{i + 1}.json"
            with open(filename, 'w') as f:
                json.dump(all_results, f, indent=2)
            print(f"  Saved results to {filename}")
    
    return all_results

# Example parameter sweep
param_ranges = {
    'neuron_count': [100, 500, 1000, 2000],
    'region_count': [1, 2, 4],
    'connection_probability': [0.1, 0.15, 0.2]
}

all_results = parameter_sweep(param_ranges)
```

### 3. Longitudinal Study Pattern

```python
def run_longitudinal_study(num_timepoints, base_config, checkpoint_interval=5):
    """
    Run a longitudinal study tracking brain development over time
    """
    longitudinal_data = []
    
    for timepoint in range(num_timepoints):
        print(f"Timepoint {timepoint + 1}/{num_timepoints}")
        
        # Create brain configuration
        config = pynlm.createAdvancedConfig()
        
        # Age-dependent parameters
        config.set("neuron_count", base_config.getOr("neuron_count", 1000))
        config.set("region_count", base_config.getOr("region_count", 4))
        
        # Apply development stage parameters based on age
        if timepoint < 10:
            # Early development
            config.set("development_stage_length", 500)
            config.set("enable_structural_plasticity", True)
            config.set("enable_development", True)
        elif timepoint < 20:
            # Maturation phase
            config.set("development_stage_length", 1000)
            config.set("enable_structural_plasticity", True)
            config.set("enable_development", True)
        else:
            # Adult stage
            config.set("development_stage_length", 0)
            config.set("enable_structural_plasticity", False)
            config.set("enable_development", False)
        
        # Create and run simulation
        brain, world, agent = pynlm.createAgentSimulation()
        pynlm.configureBrainForExperiment(brain, "development")
        
        avg_reward, avg_firing = pynlm.runQuickTest(
            neurons=config.getOr("neuron_count", 1000),
            steps=2000
        )
        
        # Store timepoint data
        timepoint_data = {
            'timepoint': timepoint,
            'age_days': timepoint * 10,  # Assuming 10-day steps
            'avg_reward': avg_reward,
            'avg_firing': avg_firing,
            'neuron_count': config.getOr("neuron_count", 1000),
            'region_count': config.getOr("region_count", 4),
            'structural_plasticity_enabled': config.getOr("enable_structural_plasticity", False),
            'development_enabled': config.getOr("enable_development", False)
        }
        
        longitudinal_data.append(timepoint_data)
        
        # Save checkpoint
        if (timepoint + 1) % checkpoint_interval == 0:
            filename = f"longitudinal_checkpoint_{timepoint + 1}.json"
            with open(filename, 'w') as f:
                json.dump(longitudinal_data, f, indent=2)
            print(f"  Checkpoint saved to {filename}")
        
        print(f"  ✓ Timepoint {timepoint + 1} completed")
    
    return longitudinal_data

# Usage
base_config = pynlm.createAdvancedConfig()
longitudinal_results = run_longitudinal_study(
    num_timepoints=30,
    base_config=base_config,
    checkpoint_interval=5
)
```

---

## Performance Optimization

### 1. Memory Management

```python
def optimize_memory_usage(brain, world, agent, target_memory_mb=500):
    """
    Optimize memory usage for large simulations
    """
    import psutil
    import os
    
    process = psutil.Process(os.getpid())
    
    def get_memory_usage():
        return process.memory_info().rss / (1024 * 1024)  # MB
    
    initial_memory = get_memory_usage()
    print(f"Initial memory usage: {initial_memory:.2f} MB")
    
    # Force garbage collection
    import gc
    gc.collect()
    
    # Reset brain to free memory
    brain.reset()
    world.reset()
    agent.reset()
    
    gc.collect()
    after_reset_memory = get_memory_usage()
    print(f"Memory after reset: {after_reset_memory:.2f} MB")
    
    memory_saved = initial_memory - after_reset_memory
    print(f"Memory saved by reset: {memory_saved:.2f} MB")
    
    # Check if target memory is reached
    if after_reset_memory < target_memory_mb:
        print(f"✓ Memory usage optimized ({after_reset_memory:.2f} MB < {target_memory_mb} MB)")
        return True
    else:
        print(f"⚠ Memory usage still high ({after_reset_memory:.2f} MB)")
        return False

# Usage
brain, world, agent = pynlm.createAgentSimulation()
optimize_memory_usage(brain, world, agent, target_memory_mb=500)
```

### 2. Performance Monitoring

```python
class PerformanceMonitor:
    def __init__(self):
        self.timings = {}
        self.memory_usage = []
        self.start_time = None
        
    def start_timing(self, operation_name):
        """Start timing a specific operation"""
        self.timings[operation_name] = {
            'start_time': time.time(),
            'end_time': None,
            'duration': None
        }
    
    def end_timing(self, operation_name):
        """End timing for a specific operation"""
        if operation_name in self.timings:
            self.timings[operation_name]['end_time'] = time.time()
            self.timings[operation_name]['duration'] = (
                self.timings[operation_name]['end_time'] - 
                self.timings[operation_name]['start_time']
            )
    
    def record_memory_usage(self):
        """Record current memory usage"""
        import psutil
        process = psutil.Process()
        memory_mb = process.memory_info().rss / (1024 * 1024)
        self.memory_usage.append(memory_mb)
    
    def print_performance_report(self):
        """Print performance analysis report"""
        print("=== PERFORMANCE REPORT ===")
        
        if self.start_time:
            total_time = time.time() - self.start_time
            print(f"Total execution time: {total_time:.2f}s")
        
        print("\nOperation Timings:")
        for op_name, timing_data in self.timings.items():
            if timing_data['duration']:
                print(f"  {op_name}: {timing_data['duration']:.4f}s")
        
        if self.memory_usage:
            print(f"\nMemory Usage:")
            print(f"  Initial: {self.memory_usage[0]:.2f} MB")
            print(f"  Peak: {max(self.memory_usage):.2f} MB")
            print(f"  Final: {self.memory_usage[-1]:.2f} MB")
            print(f"  Average: {sum(self.memory_usage)/len(self.memory_usage):.2f} MB")

# Usage in simulation
monitor = PerformanceMonitor()

# Start overall timing
monitor.start_timing('total_simulation')
monitor.start_timing('brain_initialization')

brain = pynlm.createBrain(config)
brain.initialize()

monitor.end_timing('brain_initialization')

# Run simulation with monitoring
monitor.start_timing('simulation_steps')

for step in range(total_steps):
    # ... simulation code ...
    
    # Record memory periodically
    if step % 100 == 0:
        monitor.record_memory_usage()

monitor.end_timing('simulation_steps')
monitor.end_timing('total_simulation')

# Print performance report
monitor.print_performance_report()
```

### 3. Scalable Simulation Strategy

```python
def run_scalable_simulation(base_neuron_count=1000, scaling_factor=2):
    """
    Run simulations at different scales to analyze performance characteristics
    """
    scalability_results = []
    
    # Test different simulation scales
    neuron_counts = [base_neuron_count // scaling_factor, 
                     base_neuron_count, 
                     base_neuron_count * scaling_factor]
    
    for neurons in neuron_counts:
        print(f"\nRunning scalable test with {neurons} neurons...")
        
        config = pynlm.createAdvancedConfig()
        config.set("neuron_count", neurons)
        
        # Start timing
        start_time = time.time()
        
        # Run quick test
        avg_reward, avg_firing = pynlm.runQuickTest(
            neurons=neurons,
            steps=1000
        )
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Calculate performance metrics
        neurons_per_second = neurons / duration
        rewards_per_second = 1.0 / duration  # One quick test per duration
        
        result = {
            'neurons': neurons,
            'duration': duration,
            'avg_reward': avg_reward,
            'avg_firing': avg_firing,
            'neurons_per_second': neurons_per_second,
            'rewards_per_second': rewards_per_second
        }
        
        scalability_results.append(result)
        
        print(f"  Duration: {duration:.2f}s")
        print(f"  Neurons/sec: {neurons_per_second:,.0f}")
        print(f"  Reward/sec: {rewards_per_second:.4f}")
        print(f"  Final reward: {avg_reward:.4f}")
        print(f"  Final firing: {avg_firing:.4f}")
    
    # Analyze scalability
    print("\n=== SCALABILITY ANALYSIS ===")
    print("Neuron Count | Duration | Neurons/sec | Reward/sec")
    print("-------------|----------|-------------|-----------")
    
    for result in scalability_results:
        print(f"{result['neurons']:11d} | {result['duration']:8.2f}s | "
              f"{result['neurons_per_second']:11,.0f} | "
              f"{result['rewards_per_second']:8.4f}")
    
    return scalability_results

# Usage
scalability_data = run_scalable_simulation(base_neuron_count=1000, scaling_factor=2)
```

---

## Debugging and Monitoring

### 1. Error Handling and Recovery

```python
def safe_simulation_run(brain, world, agent, max_retries=3):
    """
    Run simulation with error handling and recovery
    """
    for attempt in range(max_retries):
        try:
            print(f"Attempt {attempt + 1}/{max_retries}")
            
            # Validate inputs
            if not brain or not world or not agent:
                raise ValueError("Invalid brain, world, or agent")
            
            # Run simulation
            for step in range(total_steps):
                # Validate step count
                if step < 0:
                    raise ValueError(f"Invalid step count: {step}")
                
                # Get sensory input
                percept = world.getSensoryPercept()
                if not percept:
                    print(f"Warning: Empty percept at step {step}")
                    continue
                
                # Process sensory input
                agent.processSensoryInput(percept)
                
                # Brain step
                brain.step(step, step * 0.001)
                
                # Get motor command
                action = agent.decodeMotorCommand()
                
                # Apply action
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Validate action
                if not action:
                    print(f"Warning: No action generated at step {step}")
                    continue
                
            print(f"✓ Simulation completed successfully on attempt {attempt + 1}")
            return True
            
        except Exception as e:
            print(f"✗ Error on attempt {attempt + 1}: {str(e)}")
            
            if attempt < max_retries - 1:
                print(f"Retrying in 5 seconds...")
                time.sleep(5)
                
                # Reset simulation state for retry
                brain.reset()
                world.reset()
                agent.reset()
            else:
                print(f"✗ All {max_retries} attempts failed")
                raise e
    
    return False

# Usage with error handling
brain, world, agent = pynlm.createAgentSimulation()
if safe_simulation_run(brain, world, agent, max_retries=3):
    print("Simulation completed successfully!")
else:
    print("Simulation failed after all retries")
```

### 2. Detailed Logging

```python
import logging
from datetime import datetime

class NLMLoggingConfig:
    def __init__(self, log_file=None, log_level=logging.INFO):
        self.log_file = log_file
        self.log_level = log_level
        self.setup_logging()
    
    def setup_logging(self):
        """Setup detailed logging configuration"""
        # Create logger
        self.logger = logging.getLogger('nlm_simulation')
        self.logger.setLevel(self.log_level)
        
        # Create formatter
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s',
            datefmt='%Y-%m-%d %H:%M:%S'
        )
        
        # Console handler
        console_handler = logging.StreamHandler()
        console_handler.setFormatter(formatter)
        self.logger.addHandler(console_handler)
        
        # File handler
        if self.log_file:
            file_handler = logging.FileHandler(self.log_file)
            file_handler.setFormatter(formatter)
            self.logger.addHandler(file_handler)
        
        # Performance logger
        self.perf_logger = logging.getLogger('nlm_performance')
        self.perf_logger.setLevel(logging.DEBUG)
        
        # Create performance log file
        perf_log_file = self.log_file.replace('.log', '_perf.log') if self.log_file else None
        if perf_log_file:
            perf_file_handler = logging.FileHandler(perf_log_file)
            perf_file_handler.setFormatter(formatter)
            self.perf_logger.addHandler(perf_file_handler)
    
    def log_simulation_step(self, step, reward, firing_rate, action_type):
        """Log detailed simulation step information"""
        self.logger.info(f"Step {step}: Reward={reward:.4f}, "
                        f"Firing={firing_rate:.2f}, Action={action_type}")
        
        self.perf_logger.debug(f"Step {step}: "
                               f"Reward={reward:.4f}, "
                               f"Firing={firing_rate:.2f}, "
                               f"Action={action_type}")
    
    def log_experiment_summary(self, experiment_name, results):
        """Log experiment summary with statistics"""
        self.logger.info(f"=== {experiment_name} SUMMARY ===")
        self.logger.info(f"Total Steps: {results['total_steps']}")
        self.logger.info(f"Average Reward: {results['avg_reward']:.4f}")
        self.logger.info(f"Average Firing Rate: {results['avg_firing_rate']:.2f}")
        self.logger.info(f"Total Spikes: {results['total_spikes']}")
        
        # Log detailed statistics
        self.logger.info("Per-step statistics:")
        for step_range, stats in results['step_stats'].items():
            self.logger.info(f"  {step_range}: Avg Reward={stats['avg_reward']:.4f}, "
                           f"Avg Firing={stats['avg_firing_rate']:.2f}")

# Usage
logging_config = NLMLoggingConfig(
    log_file="nlm_simulation.log",
    log_level=logging.INFO
)

# In simulation loop
logging_config.log_simulation_step(
    step=100,
    reward=0.5,
    firing_rate=3.2,
    action_type="MoveForward"
)

# After simulation
experiment_results = {
    'total_steps': 1000,
    'avg_reward': 0.45,
    'avg_firing_rate': 2.8,
    'total_spikes': 1500,
    'step_stats': {
        '0-99': {'avg_reward': 0.3, 'avg_firing_rate': 2.5},
        '100-199': {'avg_reward': 0.5, 'avg_firing_rate': 3.1},
        '200-999': {'avg_reward': 0.5, 'avg_firing_rate': 3.0}
    }
}

logging_config.log_experiment_summary("Memory Experiment", experiment_results)
```

### 3. Real-time Monitoring

```python
import threading
import time
from collections import deque

class RealTimeMonitor:
    def __init__(self, update_interval=1.0):
        self.update_interval = update_interval
        self.metrics = {
            'step': 0,
            'reward': 0.0,
            'firing_rate': 0.0,
            'spikes': 0,
            'action_counts': {},
            'start_time': time.time(),
            'metrics_history': deque(maxlen=1000)
        }
        self.running = False
        self.monitor_thread = None
    
    def update_metrics(self, step, reward, firing_rate, spikes, action_type):
        """Update monitoring metrics"""
        self.metrics['step'] = step
        self.metrics['reward'] = reward
        self.metrics['firing_rate'] = firing_rate
        self.metrics['spikes'] = spikes
        
        if action_type:
            if action_type not in self.metrics['action_counts']:
                self.metrics['action_counts'][action_type] = 0
            self.metrics['action_counts'][action_type] += 1
        
        # Add to history
        self.metrics['metrics_history'].append({
            'timestamp': time.time(),
            'step': step,
            'reward': reward,
            'firing_rate': firing_rate,
            'spikes': spikes,
            'action': action_type
        })
    
    def start_monitoring(self):
        """Start real-time monitoring in a separate thread"""
        self.running = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
    
    def _monitor_loop(self):
        """Main monitoring loop"""
        while self.running:
            self._display_status()
            time.sleep(self.update_interval)
    
    def _display_status(self):
        """Display current status"""
        clear = '\033[2J\033[H'  # Clear screen (ANSI escape codes)
        print(clear)
        
        print("=== NLM REAL-TIME MONITOR ===")
        print(f"Simulation Time: {time.time() - self.metrics['start_time']:.1f}s")
        print(f"Current Step: {self.metrics['step']}")
        print(f"Current Reward: {self.metrics['reward']:.4f}")
        print(f"Current Firing Rate: {self.metrics['firing_rate']:.2f}")
        print(f"Total Spikes: {self.metrics['spikes']}")
        
        print("\nAction Distribution:")
        for action, count in self.metrics['action_counts'].items():
            print(f"  {action}: {count}")
        
        # Calculate recent statistics
        if len(self.metrics['metrics_history']) >= 10:
            recent_history = list(self.metrics['metrics_history'])[-10:]
            avg_reward = sum(h['reward'] for h in recent_history) / len(recent_history)
            avg_firing = sum(h['firing_rate'] for h in recent_history) / len(recent_history)
            
            print(f"\nRecent Statistics (last 10 steps):")
            print(f"  Average Reward: {avg_reward:.4f}")
            print(f"  Average Firing Rate: {avg_firing:.2f}")
        
        print(f"\nTotal Metrics Recorded: {len(self.metrics['metrics_history'])}")
        print(f"Press Ctrl+C to stop monitoring")
    
    def stop_monitoring(self):
        """Stop monitoring"""
        self.running = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)

# Usage
monitor = RealTimeMonitor(update_interval=2.0)

# Start monitoring in background
monitor.start_monitoring()

# Run simulation with metric updates
brain, world, agent = pynlm.createAgentSimulation()

for step in range(total_steps):
    # ... simulation code ...
    
    # Update metrics for monitoring
    monitor.update_metrics(
        step=step,
        reward=reward,
        firing_rate=firing_rate,
        spikes=spike_count,
        action_type=action_type
    )
    
    # Your simulation logic here...

# Stop monitoring when done
monitor.stop_monitoring()
```

---

## Common Patterns and Anti-patterns

### Recommended Patterns

#### 1. Gradual Development Pattern

```python
def gradual_development_simulation(brain, world, agent, developmental_stages):
    """
    Implement gradual development with different phases
    """
    for stage, params in developmental_stages.items():
        print(f"Entering {stage} phase...")
        
        # Configure brain for developmental stage
        brain.getConfig().set("enable_structural_plasticity", params.get('plasticity', True))
        brain.getConfig().set("enable_development", params.get('development', True))
        brain.getConfig().set("enable_curiosity", params.get('curiosity', True))
        brain.getConfig().set("enable_prediction", params.get('prediction', False))
        
        # Run simulation for this stage duration
        stage_duration = params.get('duration', 100)
        for step in range(stage_duration):
            # ... simulation code ...
            
            # Apply stage-specific learning rules
            if stage == 'early':
                # Rapid learning in early stages
                agent.enableRewardModulation(True)
                agent.enableStructuralPlasticity(True)
            elif stage == 'mature':
                # Stabilized learning in mature stages
                agent.enableRewardModulation(True)
                agent.enableStructuralPlasticity(False)
        
        print(f"✓ Completed {stage} phase")
    
    print("✓ Gradual development simulation completed")
```

#### 2. Multi-objective Optimization Pattern

```python
def multi_objective_optimization(brain, world, agent, objectives):
    """
    Optimize for multiple objectives simultaneously
    """
    optimization_weights = {
        'reward': 0.4,           # Primary objective
        'firing_stability': 0.3, # Secondary objective
        'novelty': 0.2,          # Tertiary objective
        'development': 0.1       # Bonus objective
    }
    
    # Run simulation while tracking all objectives
    objective_values = {obj: 0.0 for obj in objectives}
    objective_history = {obj: [] for obj in objectives}
    
    for step in range(total_steps):
        # ... simulation code ...
        
        # Calculate objective values
        for objective in objectives:
            if objective == 'reward':
                objective_values[objective] += reward
            elif objective == 'firing_stability':
                # Calculate firing rate stability
                firing_stability = 1.0 / (1.0 + abs(firing_rate - target_firing_rate))
                objective_values[objective] += firing_stability
            elif objective == 'novelty':
                # Calculate novelty based on curiosity
                novelty = agent.getNoveltyLevel()
                objective_values[objective] += novelty
            elif objective == 'development':
                # Calculate development progress
                development_stage = agent.getDevelopmentalStage()
                objective_values[objective] += development_stage
        
        # Store history for analysis
        for objective in objectives:
            objective_history[objective].append(objective_values[objective])
        
        # Check for Pareto improvements (conceptual)
        # This would require maintaining a Pareto front
    
    # Calculate weighted sum of objectives
    weighted_objective = 0.0
    for objective, weight in optimization_weights.items():
        weighted_objective += objective_values[objective] * weight
    
    print(f"Multi-objective optimization result: {weighted_objective:.4f}")
    for objective in objectives:
        print(f"  {objective}: {objective_values[objective]:.4f}")
    
    return weighted_objective, objective_values, objective_history
```

### Anti-patterns to Avoid

#### 1. Magic Numbers Pattern (Anti-pattern)

```python
# BAD: Magic numbers scattered throughout
for i in range(50):  # Magic number 50
    brain->injectCurrent(neurons[i], 60.0f);  # Magic number 60.0
    brain->step(step, step * 0.001);  # Magic number 0.001
    
# GOOD: Use named constants
const int MAX_EXPERIMENT_STEPS = 10000;
const float DEFAULT_TIMESTEP = 0.001f;
const float EXCITATORY_INPUT = 50.0f;

for (int i = 0; i < MAX_EXPERIMENT_STEPS; ++i) {
    brain->injectCurrent(neurons[i], EXCITATORY_INPUT);
    brain->step(step, step * DEFAULT_TIMESTEP);
}
```

#### 2. Deep Nesting Pattern (Anti-pattern)

```python
# BAD: Deep nesting makes code hard to read
if (condition1) {
    if (condition2) {
        if (condition3) {
            if (condition4) {
                if (condition5) {
                    // Complex logic here
                }
            }
        }
    }
}

# GOOD: Use early returns and helper functions
if (!condition1) return false;
if (!condition2) return false;
if (!condition3) return false;
if (!condition4) return false;
if (!condition5) return false;

// Execute complex logic here
```

#### 3. Repeated Code Pattern (Anti-pattern)

```python
// BAD: Repeated code in multiple places
void run_memory_experiment_1() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 4);
    // ... more config ...
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    // ... setup and run ...
}

void run_memory_experiment_2() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 4);
    // ... same config as above ...
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    // ... same setup and run ...
}

// GOOD: Use helper functions to reduce duplication
void run_memory_experiment(int neuron_count, int region_count, int steps) {
    auto config = create_memory_experiment_config(neuron_count, region_count);
    auto brain = create_and_initialize_brain(config);
    run_simulation(brain, steps);
}

std::shared_ptr<Config> create_memory_experiment_config(int neurons, int regions) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", neurons);
    config->set("region_count", regions);
    // ... additional common config ...
    return config;
}
```

---

## Conclusion

This documentation provides comprehensive guidance for using NLM at advanced levels. The key improvements include:

1. **Enhanced Python API** with advanced convenience functions
2. **Comprehensive Command Line Interface** for easy automation
3. **Configuration Management** tools for complex experiments
4. **Experiment Design Patterns** for different research scenarios
5. **Performance Optimization** techniques for efficient simulations
6. **Debugging and Monitoring** tools for better control and analysis
7. **Common Patterns** with clear recommendations for best practices

The improvements make NLM more accessible to advanced users while maintaining its powerful neural simulation capabilities.

---

## Further Reading

- [NLM Architecture Documentation](docs/ARCHITECTURE.md)
- [Phase 6 Integration Details](docs/PHASE6_ARCHITECTURE.md)
- [Performance Optimization Guide](docs/PERFORMANCE.md)
- [Scientific Background](docs/SCIENCE.md)

*Last updated: $(date +%Y-%m-%d)*
