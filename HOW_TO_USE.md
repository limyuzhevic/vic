# NLM (Neural Learning Machine) - Advanced Usage Guide

## Overview

This document covers advanced usage of the NLM Python API, including:

1. **Advanced Simulation Features**
2. **Performance Monitoring and Profiling**
3. **Data Analysis and Visualization**
4. **Advanced Configuration Management**
5. **Resource Management with Context Managers**
6. **Custom Experiment Management**
7. **Troubleshooting and Best Practices**

---

## Part 1: Advanced Simulation Features

### 1.1 Enhanced Simulation Utilities

The NLM Python bindings include advanced simulation utilities that go beyond basic step-by-step execution:

#### run_simulation()

```python
import pynlm

# Create components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run complete simulation with data collection
results = pynlm.run_simulation(
    brain, world, agent,
    steps=1000,          # Number of simulation steps
    timestep=0.1       # Time step in seconds
)
```

**What run_simulation() returns:**

```python
{
    "step": 0,                    # Step number
    "time": 0.0,                  # Simulation time in seconds
    "spike_count": 42,            # Total spikes since start
    "firing_neurons": 5,          # Currently firing neurons
    "average_firing_rate": 4.2,   # Average firing rate (Hz)
    "action_type": 0,             # Numeric action type
    "curiosity_level": 0.1,       # Curiosity (0-1)
    "novelty_level": 0.05,        # Novelty (0-1)
    "prediction_error": 0.02,     # Prediction error
    "neuromodulation_level": 0.1 # Neuromodulation level
}
```

#### run_training_episode()

```python
# Run a complete training episode with all learning enabled
results = pynlm.run_training_episode(
    brain, world, agent,
    steps=500,          # Number of training steps
    timestep=0.1,      # Time step in seconds
    verbose=True       # Print progress information
)

# Enables these learning subsystems automatically:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

### 1.2 Agent Subsystem Management

```python
# Configure agent for different scenarios
agent = pynlm.createAgentBrain(brain)

# Enable specific learning mechanisms based on your goals
agent.enableRewardModulation(True)     # For reinforcement learning
# or
agent.enableRewardModulation(False)    # For developmental learning

agent.enableStructuralPlasticity(True) # For network growth
agent.enableDevelopment(True)          # For maturation
agent.enableCuriosity(True)            # For exploration

# Check current configuration
print("Reward modulation:", agent.isRewardModulationEnabled())
print("Structural plasticity:", agent.isStructuralPlasticityEnabled())
print("Development enabled:", agent.isDevelopmentEnabled())
print("Curiosity enabled:", agent.isCuriosityEnabled())
```

---

## Part 2: Performance Monitoring and Profiling

### 2.1 Profiling Operations

```python
import time

# Profile specific operations
brain_step_time = pynlm.profile_operation(
    lambda: brain.step(100),  # Operation to profile
    "brain_step_100"
)
print(f"Brain step 100 took {brain_step_time[1]} ms")

world_update_time = pynlm.profile_operation(
    lambda: world.update(0.1),
    "world_update_0.1"
)
print(f"World update took {world_update_time[1]} ms")
```

### 2.2 Benchmarking

```python
# Compare performance of different components
benchmark_results = pynlm.benchmark_simulation(
    brain, world, agent,
    num_steps=1000,
    timestep=0.01
)

for component, duration in benchmark_results:
    print(f"{component}: {duration} ms")

# Example output:
# world_update: 45 ms
# brain_processing: 120 ms  
# agent_simulation: 200 ms
```

### 2.3 Performance Optimization Tips

```python
# Use larger timesteps for speed (trade-off accuracy)
pynlm.run_simulation(brain, world, agent, 1000, timestep=0.01)

# Disable unnecessary learning for inference only
agent.enableRewardModulation(False)
agent.enableStructuralPlasticity(False)
agent.enableDevelopment(False)
agent.enableCuriosity(False)
```

---

## Part 3: Data Analysis and Visualization

### 3.1 Analyzing Simulation Results

```python
import matplotlib.pyplot as plt

# Analyze simulation results
analysis = pynlm.analyze_simulation_results(results)

print("Simulation Statistics:")
print(f"  Average spikes: {analysis['spike_count_mean']:.1f} ± {analysis['spike_count_std']:.1f}")
print(f"  Max firing rate: {analysis['firing_rate_mean']:.1f} Hz")
print(f"  Max curiosity: {analysis['curiosity_max']:.2f}")
print(f"  Max novelty: {analysis['novelty_max']:.2f}")

# Plot firing rate over time
steps = [r['step'] for r in results]
firing_rates = [r['average_firing_rate'] for r in results]

plt.figure(figsize=(12, 5))
plt.subplot(1, 2, 1)
plt.plot(steps, firing_rates)
plt.xlabel('Step')
plt.ylabel('Firing Rate (Hz)')
plt.title('Firing Rate Over Time')
plt.grid(True)

# Plot action distribution
if 'action_distribution' in analysis:
    plt.subplot(1, 2, 2)
    actions, counts = zip(*analysis['action_distribution'].items())
    plt.bar(actions, counts)
    plt.xlabel('Action Type')
    plt.ylabel('Count')
    plt.title('Action Distribution')

plt.tight_layout()
plt.show()
```

### 3.2 Custom Analysis Functions

```python
def custom_analysis(results):
    """Custom analysis function for specific metrics"""
    analysis = {}
    
    # Calculate learning progress
    initial_curiosity = results[0]['curiosity_level']
    final_curiosity = results[-1]['curiosity_level']
    analysis['curiosity_change'] = final_curiosity - initial_curiosity
    
    # Find peak activity
    peak_spike_step = max(results, key=lambda x: x['spike_count'])
    analysis['peak_spike_step'] = peak_spike_step['step']
    analysis['peak_spike_count'] = peak_spike_step['spike_count']
    
    # Calculate action diversity
    unique_actions = set(r['action_type'] for r in results)
    analysis['action_diversity'] = len(unique_actions) / 6.0  # Normalize to 0-1
    
    return analysis

# Use custom analysis
custom_results = custom_analysis(results)
print("Custom analysis:", custom_results)
```

---

## Part 4: Advanced Configuration Management

### 4.1 Configuration Utilities

```python
from pynlm import createDefaultConfig

# Create configuration with defaults
config = createDefaultConfig()

# Set multiple values efficiently
config.update({
    "brain.neuron_count": 1000,
    "brain.synapse_density": 0.1,
    "plasticity.learning_rate": 0.01,
    "plasticity.stdp.learning_rate": 0.001,
    "neuromod.dopamine.scale": 1.0
})

# Use context manager for configuration
with pynlm.with_config(config) as cfg:
    # Modify configuration within context
    cfg.set("brain.neuron_count", 500)
    cfg["test.parameter"] = 42
    
    # Configuration is automatically saved/rolled back
    print("Modified config in context")

# Validate configuration
with pynlm.with_config(config) as cfg:
    validation_errors = pynlm.validate_config(cfg)
    if validation_errors:
        for error in validation_errors:
            print(f"Validation error: {error}")
```

### 4.2 Configuration Templates

```python
def get_reinforcement_learning_config():
    """Configuration optimized for reinforcement learning"""
    config = pynlm.createDefaultConfig()
    
    # Brain parameters
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.connection_probability", 0.05)
    
    # Plasticity parameters
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.stdp.tau_plus", 20.0)
    config.set("plasticity.stdp.tau_minus", 20.0)
    
    # Neuromodulation
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Environment
    config.set("environment.width", 30)
    config.set("environment.height", 30)
    
    return config

def get_developmental_config():
    """Configuration optimized for developmental learning"""
    config = pynlm.createDefaultConfig()
    
    # Smaller initial brain for development
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.05)
    
    # High plasticity for development
    config.set("plasticity.stdp.learning_rate", 0.01)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.001)
    config.set("plasticity.structural.pruning_rate", 0.0001)
    
    # Development parameters
    config.set("development.synaptogenesis_rate", 0.001)
    config.set("development.pruning_rate", 0.0001)
    config.set("development.maturation_rate", 0.01)
    
    return config
```

### 4.3 Configuration Serialization

```python
import json

# Save configuration to file
config.saveToFile("my_config.json")

# Load configuration from file
loaded_config = pynlm.load_config("my_config.json")

# Create from Python dictionary
python_dict_config = {
    "brain.neuron_count": 1000,
    "plasticity.learning_rate": 0.01,
    "neuromod.curiosity.enable": True
}

nlm_config = pynlm.from_dict(python_dict_config)

# Convert to Python dictionary for export
python_export = nlm_config.to_dict()
with open("exported_config.json", "w") as f:
    json.dump(python_export, f, indent=2)
```

---

## Part 5: Resource Management with Context Managers

### 5.1 Brain Context Management

```python
# Create brain with automatic resource management
with pynlm.with_brain(config) as brain:
    brain.initialize()
    
    # Use brain for simulation
    for step in range(100):
        brain.step(step)
    
    # Brain automatically cleaned up (memory freed)
    print(f"Brain had {brain.getTotalNeuronCount()} neurons")
    print(f"Generated {brain.getTotalSpikeCount()} spikes")
```

### 5.2 Advanced Context Management

```python
def complex_simulation_scenario(config):
    """Complex scenario using multiple context managers"""
    
    # Create world with context
    with pynlm.with_config(config) as cfg:
        cfg.set("environment.width", 20)
        cfg.set("environment.height", 20)
        
        # Create brain and world
        with pynlm.with_brain(cfg) as brain:
            world = pynlm.createSimpleWorld()
            world.configure(
                width=cfg.getOr("environment.width", 20),
                height=cfg.getOr("environment.height", 20),
                visionWidth=8, visionHeight=8
            )
            
            # Create agent
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Run complex scenario
            results = pynlm.run_training_episode(
                brain, world, agent,
                steps=200, timestep=0.05,
                verbose=True
            )
            
            return results

# Run complex scenario
scenario_results = complex_simulation_scenario(config)
print(f"Complex scenario completed with {len(scenario_results)} steps")
```

### 5.3 Exception Handling

```python
def safe_simulation(config):
    """Simulation with proper exception handling"""
    try:
        with pynlm.with_brain(config) as brain:
            brain.initialize()
            
            for step in range(100):
                brain.step(step)
                
            return brain.getTotalSpikeCount()
            
    except Exception as e:
        print(f"Simulation failed: {e}")
        return None
    finally:
        print("Resources cleaned up")

# Safe execution
score = safe_simulation(config)
if score is not None:
    print(f"Simulation successful with {score} spikes")
```

---

## Part 6: Custom Experiment Management

### 6.1 Experiment Templates

```python
def run_multiple_trials(config, num_trials=10, steps_per_trial=100):
    """Run multiple experimental trials"""
    all_results = []
    
    for trial in range(num_trials):
        print(f"Running trial {trial + 1}/{num_trials}")
        
        # Reset and create fresh components
        with pynlm.with_brain(config) as brain:
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Run trial
            trial_results = pynlm.run_simulation(
                brain, world, agent,
                steps=steps_per_trial, timestep=0.1
            )
            
            all_results.append(trial_results)
    
    return all_results

def run_parameter_sweep(base_config, parameter_ranges):
    """Run parameter sweep experiment"""
    results = []
    
    # Generate all parameter combinations
    from itertools import product
    
    param_combinations = list(product(*parameter_ranges.values()))
    
    for i, param_values in enumerate(param_combinations):
        print(f"Parameter set {i + 1}/{len(param_combinations)}")
        
        # Create config with current parameters
        config = pynlm.createDefaultConfig()
        config.update(base_config)
        
        # Set current parameter values
        for param_name, value in zip(parameter_ranges.keys(), param_values):
            # Convert parameter names to config keys
            if param_name == "neuron_count":
                config.set("brain.neuron_count", value)
            elif param_name == "learning_rate":
                config.set("plasticity.learning_rate", value)
            elif param_name == "curiosity":
                config.set("neuromod.curiosity.enable", value)
        
        # Run experiment
        experiment_results = run_multiple_trials(
            config, num_trials=5, steps_per_trial=50
        )
        
        results.append({
            "parameters": dict(zip(parameter_ranges.keys(), param_values)),
            "results": experiment_results
        })
    
    return results
```

### 6.2 Data Collection and Logging

```python
import json
import datetime
from pathlib import Path

def collect_experiment_data(config, experiment_name):
    """Collect experiment data with logging"""
    
    # Create results directory
    results_dir = Path("experiments") / experiment_name
    results_dir.mkdir(parents=True, exist_ok=True)
    
    # Setup experiment
    with pynlm.with_brain(config) as brain:
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run experiment
        results = pynlm.run_training_episode(
            brain, world, agent,
            steps=300, timestep=0.1, verbose=False
        )
        
        # Analyze results
        analysis = pynlm.analyze_simulation_results(results)
        
        # Create metadata
        metadata = {
            "experiment_name": experiment_name,
            "timestamp": datetime.datetime.now().isoformat(),
            "config": config.to_dict(),
            "parameters": {
                "neuron_count": config.getOr("brain.neuron_count", 1000),
                "learning_rate": config.getOr("plasticity.learning_rate", 0.01),
                "curiosity_enabled": config.getOr("neuromod.curiosity.enable", True)
            }
        }
        
        # Save results
        results_file = results_dir / "results.json"
        with open(results_file, 'w') as f:
            json.dump({
                "metadata": metadata,
                "analysis": analysis,
                "step_results": results
            }, f, indent=2)
        
        # Create summary
        summary = {
            "experiment_name": experiment_name,
            "steps_completed": len(results),
            "final_spike_count": analysis.get("spike_count_mean", 0),
            "average_firing_rate": analysis.get("firing_rate_mean", 0),
            "max_curiosity": analysis.get("curiosity_max", 0),
            "max_novelty": analysis.get("novelty_max", 0),
            "action_distribution": analysis.get("action_distribution", {})
        }
        
        summary_file = results_dir / "summary.json"
        with open(summary_file, 'w') as f:
            json.dump(summary, f, indent=2)
        
        print(f"Experiment '{experiment_name}' completed and saved")
        print(f"  Results: {len(results)} steps")
        print(f"  Average spikes: {analysis.get('spike_count_mean', 0):.1f}")
        
        return summary

# Run experiment
experiment_summary = collect_experiment_data(
    config, "reinforcement_learning_test"
)
```

---

## Part 7: Troubleshooting and Best Practices

### 7.1 Common Issues and Solutions

#### Issue 1: "Brain is not responding"

**Problem:** Brain seems stuck or not processing.

**Solution:**
```python
# Check if brain was initialized
if not brain.initialize():
    print("Brain initialization failed")
    # Try with different configuration
    config.set("brain.neuron_count", 100)
    brain = pynlm.createBrain(config)
    brain.initialize()
```

#### Issue 2: "Memory usage too high"

**Problem:** Simulation consuming excessive memory.

**Solution:**
```python
# Reduce brain size
config.set("brain.neuron_count", 100)  # Instead of 1000
config.set("brain.synapse_density", 0.05)  # Instead of 0.1

# Use context managers for automatic cleanup
with pynlm.with_brain(config) as brain:
    # Simulation runs and automatically cleans up
    pass
```

#### Issue 3: "Learning not working"

**Problem:** Agent not learning from experience.

**Solution:**
```python
# Check if learning subsystems are enabled
if not agent.isRewardModulationEnabled():
    agent.enableRewardModulation(True)

if not agent.isCuriosityEnabled():
    agent.enableCuriosity(True)

# Verify configuration
print("Reward modulation:", agent.isRewardModulationEnabled())
print("Curiosity enabled:", agent.isCuriosityEnabled())
print("Development enabled:", agent.isDevelopmentEnabled())
```

### 7.2 Performance Optimization

```python
def optimize_for_inference(config):
    """Optimize configuration for inference (no learning)"""
    config.set("plasticity.stdp.enable", False)
    config.set("plasticity.hebbian.enable", False)
    config.set("plasticity.structural.enable", False)
    
    agent = pynlm.createAgentBrain(pynlm.createBrain(config))
    agent.initialize(world)
    
    # Disable all learning subsystems
    agent.enableRewardModulation(False)
    agent.enableStructuralPlasticity(False)
    agent.enableDevelopment(False)
    agent.enableCuriosity(False)
    
    return agent

def optimize_for_training(config):
    """Optimize configuration for training (max learning)"""
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    agent = pynlm.createAgentBrain(pynlm.createBrain(config))
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    return agent
```

### 7.3 Best Practices

1. **Always use context managers** for resource management:
   ```python
   with pynlm.with_brain(config) as brain:
       # Your simulation code here
   ```

2. **Validate configurations** before running simulations:
   ```python
   if not pynlm.validate_config(config):
       print("Configuration invalid")
       return
   ```

3. **Profile performance** of critical operations:
   ```python
   time_info = pynlm.profile_operation(
       lambda: brain.step(100),
       "brain_step_100"
   )
   ```

4. **Collect and analyze data** systematically:
   ```python
   results = pynlm.run_simulation(brain, world, agent, 1000)
   analysis = pynlm.analyze_simulation_results(results)
   ```

5. **Use context managers** for complex scenarios:
   ```python
   with pynlm.with_config(config) as cfg:
       with pynlm.with_brain(cfg) as brain:
           # Complex simulation with automatic cleanup
   ```

---

## Part 8: Quick Reference

### 8.1 Core Functions

| Function | Description | Example |
|----------|-------------|---------|
| `pynlm.createDefaultConfig()` | Create default configuration | `config = pynlm.createDefaultConfig()` |
| `pynlm.createBrain(config)` | Create brain with configuration | `brain = pynlm.createBrain(config)` |
| `pynlm.createSimpleWorld()` | Create simple environment | `world = pynlm.createSimpleWorld()` |
| `pynlm.createAgentBrain(brain)` | Create agent interface | `agent = pynlm.createAgentBrain(brain)` |

### 8.2 Advanced Functions

| Function | Description | Parameters | Example |
|----------|-------------|------------|---------|
| `run_simulation()` | Run complete simulation | `brain, world, agent, steps, timestep` | `results = pynlm.run_simulation(brain, world, agent, 1000)` |
| `run_training_episode()` | Run training with all learning | `brain, world, agent, steps, timestep, verbose` | `results = pynlm.run_training_episode(brain, world, agent, 500)` |
| `profile_operation()` | Profile operation timing | `operation, name` | `time_info = pynlm.profile_operation(lambda: ..., "name")` |
| `analyze_simulation_results()` | Analyze simulation data | `results` | `analysis = pynlm.analyze_simulation_results(results)` |
| `with_brain()` | Context manager for brain | `config, func` | `with pynlm.with_brain(config) as brain: ...` |
| `with_config()` | Context manager for config | `config, func` | `with pynlm.with_config(config) as cfg: ...` |

### 8.3 Configuration Keys

| Category | Key | Default | Description |
|----------|-----|---------|-------------|
| Brain | `brain.neuron_count` | 1000 | Number of neurons |
| Brain | `brain.synapse_density` | 0.1 | Synapse connectivity density |
| Plasticity | `plasticity.stdp.enable` | true | Enable STDP learning |
| Plasticity | `plasticity.stdp.learning_rate` | 0.001 | STDP learning rate |
| Neuromod | `neuromod.curiosity.enable` | true | Enable curiosity-driven exploration |
| World | `environment.width` | 10 | World width |
| World | `environment.height` | 10 | World height |

### 8.4 Agent Methods

| Method | Description |
|--------|-------------|
| `enableRewardModulation(bool)` | Enable/disable reward learning |
| `enableStructuralPlasticity(bool)` | Enable/disable structural learning |
| `enableDevelopment(bool)` | Enable/disable development |
| `enableCuriosity(bool)` | Enable/disable curiosity-driven exploration |
| `isRewardModulationEnabled()` | Check if reward learning enabled |
| `isStructuralPlasticityEnabled()` | Check if structural learning enabled |
| `isDevelopmentEnabled()` | Check if development enabled |
| `isCuriosityEnabled()` | Check if curiosity enabled |
| `processSensoryInput(percept)` | Process sensory data |
| `decodeMotorCommand()` | Get action from brain |
| `applyRewardModulation(reward, predicted)` | Apply reward signal |
| `updateDevelopment(timestep)` | Update developmental stage |

---

## Part 9: Example Scripts

### 9.1 Quick Start Script

```python
#!/usr/bin/env python3
"""
Quick start script demonstrating NLM advanced features
"""

import pynlm

def main():
    print("=== NLM Advanced Features Demo ===\n")
    
    # 1. Create enhanced configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("plasticity.learning_rate", 0.01)
    
    # 2. Create brain with context management
    with pynlm.with_brain(config) as brain:
        print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
        
        # 3. Create environment
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        # 4. Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # 5. Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # 6. Run advanced simulation
        print("Running simulation...")
        results = pynlm.run_training_episode(
            brain, world, agent,
            steps=100, timestep=0.1, verbose=True
        )
        
        # 7. Analyze results
        analysis = pynlm.analyze_simulation_results(results)
        
        print("\n=== Simulation Results ===")
        print(f"Steps completed: {len(results)}")
        print(f"Average firing rate: {analysis.get('firing_rate_mean', 0):.1f} Hz")
        print(f"Max curiosity: {analysis.get('curiosity_max', 0):.2f}")
        print(f"Max novelty: {analysis.get('novelty_max', 0):.2f}")
        
        # 8. Profile performance
        print("\n=== Performance Profiling ===")
        time_info = pynlm.profile_operation(
            lambda: brain.step(50),
            "brain_step_50"
        )
        print(f"Brain step 50 took {time_info[1]} ms")

if __name__ == "__main__":
    main()
```

### 9.2 Data Collection Script

```python
#!/usr/bin/env python3
"""
Data collection script for systematic experiments
"""

import pynlm
import json
import datetime
from pathlib import Path

def collect_experiment_data(config, experiment_name):
    """Collect comprehensive experiment data"""
    
    # Create results directory
    results_dir = Path("experiments") / experiment_name
    results_dir.mkdir(parents=True, exist_ok=True)
    
    # Record experiment start time
    start_time = datetime.datetime.now()
    
    print(f"Starting experiment: {experiment_name}")
    print(f"Configuration: {config.to_dict()}")
    
    # Run experiment
    with pynlm.with_brain(config) as brain:
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation with data collection
        results = pynlm.run_training_episode(
            brain, world, agent,
            steps=200, timestep=0.1, verbose=False
        )
        
        # Analyze results
        analysis = pynlm.analyze_simulation_results(results)
        
        # End time and duration
        end_time = datetime.datetime.now()
        duration = (end_time - start_time).total_seconds()
        
        # Create comprehensive results package
        experiment_data = {
            "metadata": {
                "experiment_name": experiment_name,
                "start_time": start_time.isoformat(),
                "end_time": end_time.isoformat(),
                "duration_seconds": duration,
                "config": config.to_dict(),
                "parameters": {
                    "neuron_count": config.getOr("brain.neuron_count", 1000),
                    "learning_rate": config.getOr("plasticity.learning_rate", 0.01),
                    "curiosity_enabled": config.getOr("neuromod.curiosity.enable", True)
                }
            },
            "simulation_results": results,
            "analysis": analysis,
            "performance_metrics": {
                "total_steps": len(results),
                "final_spike_count": analysis.get("spike_count_mean", 0),
                "average_firing_rate": analysis.get("firing_rate_mean", 0),
                "max_curiosity": analysis.get("curiosity_max", 0),
                "max_novelty": analysis.get("novelty_max", 0),
                "average_action_diversity": analysis.get("action_diversity", 0)
            }
        }
        
        # Save results
        results_file = results_dir / "experiment_data.json"
        with open(results_file, 'w') as f:
            json.dump(experiment_data, f, indent=2)
        
        # Create summary
        summary = {
            "experiment_name": experiment_name,
            "duration_seconds": duration,
            "steps_completed": len(results),
            "final_spike_count": analysis.get("spike_count_mean", 0),
            "average_firing_rate": analysis.get("firing_rate_mean", 0),
            "max_curiosity": analysis.get("curiosity_max", 0),
            "max_novelty": analysis.get("novelty_max", 0),
            "parameters_used": experiment_data["metadata"]["parameters"],
            "files_created": [str(f) for f in results_dir.glob("*")]
        }
        
        summary_file = results_dir / "summary.json"
        with open(summary_file, 'w') as f:
            json.dump(summary, f, indent=2)
        
        print(f"\n=== Experiment Complete ===")
        print(f"Duration: {duration:.1f} seconds")
        print(f"Steps completed: {len(results)}")
        print(f"Average firing rate: {analysis.get('firing_rate_mean', 0):.1f} Hz")
        print(f"Max curiosity: {analysis.get('curiosity_max', 0):.2f}")
        print(f"Results saved to: {results_dir}")
        
        return summary

if __name__ == "__main__":
    # Example usage
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("plasticity.learning_rate", 0.01)
    
    experiment_summary = collect_experiment_data(
        config, "reinforcement_learning_test_001"
    )
```

---

## Part 10: Migration Guide

### 10.1 From Old API to New API

#### Old Way (Basic):

```python
import pynlm

# Basic usage
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
```

#### New Way (Enhanced):

```python
import pynlm

# Enhanced usage with context managers
config = pynlm.createDefaultConfig()

with pynlm.with_brain(config) as brain:
    for step in range(100):
        brain.step(step)
```

### 10.2 Migrating Complex Scripts

#### Old Complex Script:

```python
def old_simulation_script():
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    return brain.getTotalSpikeCount()
```

#### New Enhanced Script:

```python
def new_simulation_script():
    config = pynlm.createDefaultConfig()
    
    with pynlm.with_brain(config) as brain:
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enhanced simulation with data collection
        results = pynlm.run_simulation(
            brain, world, agent,
            steps=100, timestep=0.1
        )
        
        # Analyze results
        analysis = pynlm.analyze_simulation_results(results)
        
        return {
            "spike_count": brain.getTotalSpikeCount(),
            "firing_rate": analysis.get("firing_rate_mean", 0),
            "steps_completed": len(results),
            "results": results
        }
```

### 10.3 Feature Comparisons

| Feature | Old API | New API |
|---------|---------|---------|
| Resource Management | Manual | Automatic (context managers) |
| Data Collection | Manual | Built-in (`run_simulation()`) |
| Performance Profiling | Manual | Built-in (`profile_operation()`) |
| Configuration Validation | Manual | Built-in (`validate_config()`) |
| Context Management | None | Available (`with_brain()`, `with_config()`) |
| Data Analysis | Manual | Built-in (`analyze_simulation_results()`) |
| Error Handling | Basic | Comprehensive |
| Documentation | Limited | Extensive |

---

## Conclusion

The enhanced NLM Python API provides powerful tools for:

1. **Efficient Simulation**: Use `run_simulation()` and `run_training_episode()` for complete control loops
2. **Resource Management**: Use context managers (`with_brain()`, `with_config()`) for automatic cleanup
3. **Performance Monitoring**: Use `profile_operation()` to identify bottlenecks
4. **Data Analysis**: Use `analyze_simulation_results()` for comprehensive analysis
5. **Advanced Configuration**: Use enhanced configuration utilities with Pythonic interfaces
6. **Systematic Experimentation**: Use built-in tools for parameter sweeps and multiple trials

The new API maintains full backward compatibility while adding powerful new features that make NLM easier to use, more powerful, and more maintainable.

Start with the examples and gradually adopt new features as you become more familiar with the enhanced capabilities.

For the most up-to-date documentation and examples, visit the NLM documentation website.

---

*Last updated: $(date)
*Version: 2.0 (Enhanced Python Bindings)*
*Copyright: NLM Project Team*