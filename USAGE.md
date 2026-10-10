# NLM Usage Guide

## Overview

This document provides comprehensive usage instructions for the NLM (熙然) Neural Simulation Project. It covers everything from basic neural simulation setup to advanced configuration and system management.

## Quick Start

### Basic Neural Simulation

```bash
# Create a simple simulation with 1000 neurons for 500 steps
./nlm --neuron-count 1000 --max-steps 500

# Save results to file
./nlm --neuron-count 1000 --max-steps 500 --output-file results.json --output-format json

# Run with profiling enabled
./nlm --neuron-count 1000 --max-steps 1000 --enable-profiling --enable-validation --parallel 4
```

### Python API Usage

```python
import pynlm
import time

# Create a simple brain simulation
def run_basic_simulation():
    """Run a basic neural simulation"""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    
    # 2. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Run simulation
    for step in range(1000):
        brain.step(step)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print(f"Simulation completed! Total spikes: {brain.getTotalSpikeCount()}")

# 4. Save results
brain.save("simulation_results.bin")

if __name__ == "__main__":
    run_basic_simulation()
```

### Complete Agent Simulation

```python
import pynlm

def run_complete_agent_simulation():
    """Run a complete agent simulation with brain, world, and agent"""
    
    # 1. Create configuration and brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 4. Initialize agent with world
    agent.initialize(world)
    
    # 5. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 6. Run simulation loop
    num_steps = 1000
    for step in range(num_steps):
        
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log statistics periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    print(f"Agent simulation completed!")

if __name__ == "__main__":
    run_complete_agent_simulation()
```

## Core System Usage

### Brain System

#### Basic Brain Operations

```python
import pynlm

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.v_thresh", -45.0)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(500):
    brain.step(step)

# Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"Development stage: {brain.getDevelopmentalStage()}")

# Save and load state
brain.save("brain_state.bin")
brain.load("brain_state.bin")
```

#### Brain Configuration

```python
# Modify brain configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.05)
config.set("brain.v_thresh", -60.0)
config.set("brain.v_rest", -65.0)
config.set("brain.v_reset", -70.0)

# Get current configuration
print(f"Neuron count: {config.get('brain.neuron_count', 1000)}")
print(f"Synapse density: {config.get('brain.synapse_density', 0.1)}")

# Configuration file I/O
config.saveToFile("my_config.json")
config.loadFromFile("my_config.json")
```

### Agent System

#### Agent Operations

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Initialize with world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
agent.initialize(world)

# Enable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Process sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Get motor command
action = agent.decodeMotorCommand()

# Apply reward modulation
reward = 1.0  # Example reward
agent.applyRewardModulation(reward, 0.0)

# Update development
agent.updateDevelopment(0.1)

# Get agent statistics
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
```

### World System

#### World Operations

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(
    width=50,
    height=50,
    visionWidth=16,
    visionHeight=16,
    maxEnergy=100.0
)

# Control world
world.reset()
world.update(timestep=0.1)
world.setRandomSeed(42)
world.setAgentStart(25.0, 25.0)

# Interact with world
percept = world.getSensoryPercept()
body = world.getAgentBody()

# Add and remove objects
world.addObject(pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(10.0, 10.0)
world.isValidPosition(15.0, 15.0)

# World properties
print(f"Width: {world.getWidth()}")
print(f"Height: {world.getHeight()}")
print(f"Simulation time: {world.getSimulationTime():.3f}")
print(f"Max energy: {world.getMaxEnergy():.1f}")
```

## Advanced Usage

### 1. Memory System Integration

```python
import pynlm

# Create brain with memory systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with memory systems
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Enable all memory subsystems
agent.enableWorkingMemory(True)
agent.enableEpisodicMemory(True)
agent.enableAssociativeMemory(True)

# Run simulation with memory usage
for step in range(1000):
    world.update(0.1)
    
    # Memory operations
    agent.storeExperience(step, "goal_reached", 1.0)
    
    # Retrieve past experiences
    past_experience = agent.retrieveExperience("goal_reached")
    
    # Consolidate memory
    agent.consolidateMemory(step, 0.1)
    
    # Get memory statistics
    memory_stats = agent.getMemoryStatistics()
    print(f"Memory usage: {memory_stats.used_memory}/{memory_stats.total_memory}")
```

### 2. Neuromodulation System

```python
import pynlm

# Create simulation with neuromodulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Enable neuromodulation
agent.enableNeuromodulation({
    "dopamine": True,
    "serotonin": True,
    "noradrenaline": True
})

# Run simulation with neuromodulation
for step in range(500):
    world.update(0.1)
    
    # Get neuromodulation levels
    neuromod_levels = agent.getNeuromodulationLevels()
    print(f"Dopamine: {neuromod_levels.dopamine:.3f}")
    print(f"Serotonin: {neuromod_levels.serotonin:.3f}")
    print(f"Curiosity: {neuromod_levels.curiosity:.3f}")
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Run brain
    brain.step(step)
    
    # Get motor command
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### 3. Development System

```python
import pynlm

# Create developing brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with development
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Enable development
agent.enableDevelopment(True)

# Run developmental simulation
for step in range(2000):
    world.update(0.1)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Get developmental stage
    stage = brain.getDevelopmentalStage()
    print(f"Developmental stage: {stage}")
    
    # Adjust learning rates based on development
    if stage == pynlm.DevelopmentalStage.CriticalPeriod:
        # Higher learning during critical period
        agent.setLearningRate(0.1)
    elif stage == pynlm.DevelopmentalStage.Maturation:
        # Lower learning during maturation
        agent.setLearningRate(0.01)
```

### 4. Prediction System

```python
import pynlm

# Create brain with prediction capabilities
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Enable prediction
agent.enablePrediction(True)

# Run prediction-based simulation
for step in range(1000):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Run brain step
    brain.step(step)
    
    # Get prediction
    prediction = agent.getPrediction()
    
    # Get prediction error
    error = agent.getPredictionError()
    
    # Apply reward based on prediction error
    reward = 1.0 - abs(error)
    agent.applyRewardModulation(reward, prediction)
```

## Configuration Management

### Configuration File Examples

#### Basic Configuration

```ini
# configs/basic.cfg
[brain]
neuron_count = 1000
synapse_density = 0.1
v_thresh = -50.0
v_rest = -70.0
v_reset = -75.0
tau_mem = 20.0
tau_ref = 2.0

[plasticity]
stdp_enable = true
stdp_learning_rate = 0.001
stdp_tau_plus = 20.0
stdp_tau_minus = 20.0

[neuromod]
dopamine_scale = 1.0
curiosity_enable = true
novelty_enable = true
```

#### Advanced Configuration

```json
// configs/advanced.json
{
    "brain": {
        "neuron_count": 5000,
        "synapse_density": 0.05,
        "connection_probability": 0.02,
        "initial_weight_mean": 0.5,
        "initial_weight_std": 0.1,
        "v_thresh": -60.0,
        "v_rest": -65.0,
        "v_reset": -70.0,
        "tau_mem": 25.0,
        "tau_ref": 3.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.0005
        },
        "structural": {
            "enable": true,
            "growth_rate": 0.001,
            "pruning_rate": 0.0005
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0,
            "baseline": 0.1
        },
        "curiosity": {
            "enable": true,
            "scale": 0.5,
            "threshold": 0.05
        }
    },
    "memory": {
        "working_memory": {
            "capacity": 1000,
            "decay_rate": 0.01
        },
        "episodic_memory": {
            "capacity": 5000,
            "replay_frequency": 0.1
        }
    }
}
```

### Using Configuration Files

```python
import pynlm

# Load configuration from file
config = pynlm.createDefaultConfig()
config.loadFromFile("configs/my_config.json")

# Modify configuration
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.002)

# Save configuration
config.saveToFile("configs/modified_config.json")

# Get configuration parameters
neuron_count = config.get("brain.neuron_count", 1000)
learning_rate = config.get("plasticity.stdp.learning_rate", 0.001)
```

## Performance Optimization

### 1. Parallel Processing

```bash
# Enable parallel processing in CMake
cmake .. -DNLM_ENABLE_MPI=ON -DNLM_PARALLEL_JOBS=4

# Run with parallel processing
./nlm --neuron-count 2000 --max-steps 1000 --parallel 4
```

### 2. Memory Management

```python
import pynlm
import gc

# Enable memory optimization
config = pynlm.createDefaultConfig()
config.set("memory.enable_pooling", True)
config.set("memory.pool_size", 1000)

brain = pynlm.createBrain(config)
brain.initialize()

# Monitor memory usage
import psutil
process = psutil.Process()

for step in range(1000):
    brain.step(step)
    
    # Print memory usage every 100 steps
    if step % 100 == 0:
        memory_mb = process.memory_info().rss / 1024 / 1024
        print(f"Step {step}, Memory: {memory_mb:.1f} MB")
    
    # Force garbage collection periodically
    if step % 500 == 0:
        gc.collect()
```

### 3. Profiling and Benchmarking

```bash
# Run with performance profiling
./nlm --neuron-count 1000 --max-steps 500 --enable-profiling

# Generate performance report
./nlm --benchmark --output-file perf_report.json

# Profile memory usage
valgrind --leak-check=full ./nlm --neuron-count 500 --max-steps 200
```

```python
import pynlm

# Performance profiling in Python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

brain = pynlm.createBrain(config)
brain.initialize()

# Profile brain step performance
import time

start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()

print(f"Average step time: {(end_time - start_time) / 1000 * 1000:.3f} ms")
print(f"Steps per second: {1000 / (end_time - start_time):.1f}")
```

## Examples and Templates

### Template: Simple Brain Simulation

```python
import pynlm

def run_simple_simulation(num_steps=1000, neuron_count=1000):
    """Simple brain simulation template"""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(num_steps):
        brain.step(step)
    
    # Print results
    print(f"Simulation completed:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    return brain

# Usage
brain = run_simple_simulation()
brain.save("results.bin")
```

### Template: Complete Agent Simulation

```python
import pynlm

def run_complete_agent_simulation(world_config, brain_config, num_steps=1000):
    """Complete agent simulation template"""
    
    # Create configurations
    config = pynlm.createDefaultConfig()
    
    # Apply brain configuration
    for key, value in brain_config.items():
        config.set(key, value)
    
    # Create and initialize systems
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # Configure world
    for key, value in world_config.items():
        if hasattr(world, f'set{key.title()}'):
            getattr(world, f'set{key.title()}')(value)
    
    world.reset()
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(num_steps):
        world.update(0.1)
        
        # Agent loop
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
    
    return brain, agent, world

# Usage example
world_config = {
    "width": 30,
    "height": 30,
    "visionWidth": 12,
    "visionHeight": 12,
    "maxEnergy": 100.0
}

brain_config = {
    "brain.neuron_count": 2000,
    "brain.synapse_density": 0.1
}

brain, agent, world = run_complete_agent_simulation(world_config, brain_config)
```

## Common Patterns and Best Practices

### Pattern 1: Repeated Simulation with Different Configurations

```python
import pynlm

def run_simulation_series(configs):
    """Run multiple simulations with different configurations"""
    
    results = []
    
    for config_name, config_params in configs.items():
        print(f"Running simulation: {config_name}")
        
        # Create and configure brain
        config = pynlm.createDefaultConfig()
        for key, value in config_params.items():
            config.set(key, value)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(config_params.get("steps", 1000)):
            brain.step(step)
        
        # Collect results
        result = {
            "config": config_name,
            "total_neurons": brain.getTotalNeuronCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "final_stage": brain.getDevelopmentalStage()
        }
        
        results.append(result)
        print(f"  Completed: {result}")
    
    return results
```

### Pattern 2: Parameter Sweep

```python
import pynlm
import numpy as np

def parameter_sweep(base_config, param_ranges):
    """Run parameter sweep for key configuration parameters"""
    
    results = []
    
    # Generate parameter combinations
    param_combinations = np.meshgrid(
        param_ranges["neuron_count"],
        param_ranges["synapse_density"],
        param_ranges["learning_rate"]
    )
    
    for i in range(len(param_combinations[0])):
        config = pynlm.createDefaultConfig()
        
        # Apply base configuration
        for key, value in base_config.items():
            config.set(key, value)
        
        # Apply parameter combination
        config.set("brain.neuron_count", param_combinations[0][i])
        config.set("brain.synapse_density", param_combinations[1][i])
        config.set("plasticity.stdp.learning_rate", param_combinations[2][i])
        
        # Run simulation
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(500):
            brain.step(step)
        
        # Collect results
        result = {
            "neuron_count": param_combinations[0][i],
            "synapse_density": param_combinations[1][i],
            "learning_rate": param_combinations[2][i],
            "total_spikes": brain.getTotalSpikeCount(),
            "average_firing_rate": brain.getAverageFiringRate()
        }
        
        results.append(result)
        print(f"Completed parameter set {i+1}/{len(param_combinations[0])}")
    
    return results
```

### Pattern 3: Learning Progress Tracking

```python
import pynlm

def track_learning_progress(config, num_steps=1000):
    """Track learning progress over time"""
    
    # Create and initialize systems
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    
    # Track learning progress
    progress_data = {
        "steps": [],
        "firing_rate": [],
        "curiosity": [],
        "novelty": [],
        "prediction_error": [],
        "development_stage": []
    }
    
    # Run simulation
    for step in range(num_steps):
        world.update(0.1)
        
        # Agent loop
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Track progress
        progress_data["steps"].append(step)
        progress_data["firing_rate"].append(brain.getAverageFiringRate())
        progress_data["curiosity"].append(agent.getCuriosityLevel())
        progress_data["novelty"].append(agent.getNoveltyLevel())
        progress_data["prediction_error"].append(agent.getPredictionError())
        progress_data["development_stage"].append(brain.getDevelopmentalStage())
    
    return progress_data
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: Brain Not Firing

**Problem**: Brain neurons are not firing during simulation.

**Solutions**:
```python
# 1. Check brain initialization
brain = pynlm.createBrain(config)
brain.initialize()  # Always call this first

# 2. Ensure proper configuration
config.set("brain.v_thresh", -50.0)  # Higher threshold
config.set("brain.synapse_density", 0.1)  # More connections

# 3. Add sensory input to neurons
brain.receiveSensoryInput(some_input)

# 4. Run more steps for warmup
for step in range(100):
    brain.step(step)
```

#### Issue 2: Agent Not Taking Actions

**Problem**: Agent is not generating motor commands.

**Solutions**:
```python
# 1. Check agent initialization
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# 2. Ensure motor neurons are active
# (May require sufficient sensory input)

# 3. Check motor command decoding
action = agent.decodeMotorCommand()
print(f"Action: {action}")

# 4. Enable action system
agent.enableActionSelection(True)
```

#### Issue 3: Memory System Not Working

**Problem**: Memory systems are not storing or retrieving information.

**Solutions**:
```python
# 1. Enable memory subsystems
agent.enableWorkingMemory(True)
agent.enableEpisodicMemory(True)
agent.enableAssociativeMemory(True)

# 2. Store experience
agent.storeExperience(step, "goal_reached", 1.0)

# 3. Retrieve experience
experience = agent.retrieveExperience("goal_reached")

# 4. Consolidate memory
agent.consolidateMemory(step, 0.1)
```

#### Issue 4: Performance Issues

**Problem**: Simulation is too slow.

**Solutions**:
```bash
# 1. Use parallel processing
./nlm --neuron-count 1000 --max-steps 500 --parallel 4

# 2. Reduce simulation complexity
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.05)

# 3. Enable memory pooling
config.set("memory.enable_pooling", True)

# 4. Profile performance
./nlm --benchmark --output-file perf_report.json
```

#### Issue 5: Configuration Errors

**Problem**: Configuration file is invalid or missing parameters.

**Solutions**:
```python
# 1. Validate configuration
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")

# 2. Check for missing parameters
required_params = ["brain.neuron_count", "brain.synapse_density"]
for param in required_params:
    if not config.has(param):
        print(f"Missing parameter: {param}")
        config.set(param, get_default_value(param))

# 3. Save corrected configuration
config.saveToFile("config_corrected.json")
```

## Advanced Features

### 1. Custom Neuron Models

```cpp
// Custom neuron plugin example
#include "src/plugin/neuron/INeuronPlugin.hpp"

class CustomNeuronPlugin : public INeuronPlugin {
public:
    std::string getName() const override {
        return "CustomNeuron";
    }
    
    double calculateDynamics(double v, double I, double dt) override {
        // Custom neuron dynamics
        double dv = (I - v + v*v*v/3.0 - v) * dt;
        return v + dv;
    }
    
    // Implement other required methods...
};
```

### 2. Custom Plasticity Rules

```python
import pynlm

# Custom plasticity rule
class CustomPlasticityRule:
    def __init__(self, learning_rate=0.001):
        self.learning_rate = learning_rate
    
    def learn(self, pre_spike_time, post_spike_time, weight):
        # Custom STDP-like rule
        dt = post_spike_time - pre_spike_time
        if dt > 0:
            # Post before pre (LTP)
            weight += self.learning_rate * np.exp(-dt / 20.0)
        else:
            # Pre before post (LTD)
            weight -= self.learning_rate * np.exp(dt / 20.0)
        
        return max(0.0, min(1.0, weight))

# Use custom plasticity
brain.enablePlasticityRule(CustomPlasticityRule(0.001))
```

### 3. Environment Customization

```python
import pynlm

# Custom environment
class CustomEnvironment(pynlm.SimpleWorld):
    def __init__(self):
        super().__init__()
        self.reward_function = self.exponential_reward
    
    def exponential_reward(self, position, goal):
        distance = np.sqrt((position.x - goal.x)**2 + (position.y - goal.y)**2)
        return np.exp(-distance / 10.0)
    
    def get_reward(self, agent_position, goal_position):
        return self.reward_function(agent_position, goal_position)

# Use custom environment
world = CustomEnvironment()
world.configure(width=50, height=50)
```

## Conclusion

The NLM neural simulation project provides a comprehensive framework for creating and studying artificial developmental brains. This guide has covered the essential usage patterns, from basic neural simulations to complex agent-based learning systems.

Key takeaways:

1. **Start Simple**: Begin with basic neural simulations to understand the core concepts
2. **Incremental Development**: Gradually add complexity through agent systems, memory, and neuromodulation
3. **Configuration Management**: Use configuration files for reproducible experiments
4. **Performance Optimization**: Monitor and optimize performance for large-scale simulations
5. **Documentation**: Keep track of experiments and results for analysis

For more advanced topics and specialized use cases, refer to the following resources:

- **DEVELOPING.md** - Developer guide for extending the system
- **ARCHITECTURE.md** - Detailed system architecture documentation
- **CLI.md** - Command-line interface documentation
- **PROFILING.md** - Performance profiling documentation
- **PLUGINS.md** - Plugin development guide
- **TUTORIALS.md** - Step-by-step tutorials

The NLM project continues to evolve, so check for updates and new features regularly. The combination of biological inspiration and computational power makes NLM a unique platform for studying artificial developmental intelligence.
