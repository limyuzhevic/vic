# NLM Advanced Examples and Documentation

## Overview

This document provides advanced examples, best practices, and detailed documentation for the NLM (Neural Learning Machine) project. It builds upon the easy usage guide and provides more comprehensive examples for advanced users and researchers.

## Advanced Topics

### 1. Advanced Configuration Management

#### Custom Configuration File

Create a custom configuration file for specific experimental setups:

```cpp
// advanced_config.cfg
# NLM Advanced Configuration

# Experimental setup
random_seed = 12345
simulation_timestep = 0.0001
neuron_count = 5000
region_count = 3
connection_probability = 0.08

# Enhanced plasticity settings
plasticity_learning_rate = 0.005
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.02
stdp_time_constant = 25.0

# Advanced development parameters
development_synaptogenesis_rate = 0.002
development_pruning_rate = 0.00005

# Neuromodulation configuration
dopamine_baseline = 0.2
reward_discount_factor = 0.995

# Environment configuration
environment_name = MazeWorld
environment_width = 50
environment_height = 50

# Memory systems
working_memory_capacity = 100
episodic_memory_capacity = 1000
semantic_memory_capacity = 5000

# Logging and monitoring
log_level = DEBUG
log_to_file = true
log_filename = "nlm_advanced.log"
max_simulation_steps = 50000
simulation_time_limit = 600.0

# Visualization
visualization_enabled = true
visualization_update_rate = 30.0
visualization_output_file = "nlm_visualization.html"
```

### 2. Performance Tuning Examples

#### High-Performance Simulation

```python
import pynlm
import time

# High-performance configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 10000)
config.set("simulation_timestep", 0.0005)
config.set("plasticity_enabled", True)
config.set("neuromodulation_enabled", True)
config.set("curiosity_enabled", True)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Performance monitoring
start_time = time.time()
step_count = 0

for step in range(100000):
    brain.step(step)
    step_count += 1
    
    # Performance feedback
    if step % 10000 == 0:
        elapsed = time.time() - start_time
        steps_per_sec = step_count / elapsed
        print(f"Step {step}: {steps_per_sec:.0f} steps/sec")

# Save performance checkpoint
brain.save("performance_checkpoint.bin")
```

#### Memory-Efficient Processing

```python
import pynlm
import numpy as np

# Memory-efficient configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)
config.set("working_memory_capacity", 50)

brain = pynlm.createBrain(config)
brain.initialize()

# Process in chunks to manage memory
chunk_size = 1000
for chunk in range(0, 50000, chunk_size):
    for step in range(chunk, min(chunk + chunk_size, 50000)):
        brain.step(step)
    
    # Periodically save to manage memory
    if (chunk // chunk_size) % 5 == 0:
        brain.save(f"chunk_{chunk//chunk_size}.bin")
        # Clear some memory if needed
        # brain.compactMemory()  # If available
```

### 3. Advanced Agent Behavior

#### Curiosity-Driven Exploration

```python
import pynlm
import random

def run_curiosity_driven_agent(num_steps=5000):
    """Agent that uses curiosity to explore its environment."""
    
    # Configuration with curiosity enabled
    config = pynlm.createDefaultConfig()
    config.set("curiosity_enabled", True)
    config.set("novelty_enabled", True)
    config.set("reward_discount_factor", 0.99)
    
    # Create agent components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent.initialize(world)
    
    # Enable advanced learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Exploration statistics
    exploration_stats = {
        'curiosity_driven': 0,
        'reward_driven': 0,
        'random': 0,
        'total_actions': 0
    }
    
    # Run simulation loop with curiosity monitoring
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command with curiosity consideration
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action with reward feedback
        action_result = world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation with prediction error
        expected_reward = agent.getExpectedReward()
        agent.applyRewardModulation(action_result.reward, expected_reward)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Track exploration behavior
        explore_stats.update_exploration_behavior(motor_cmd, agent)
        
        # Print detailed statistics periodically
        if step % 500 == 0:
            print(f"Step {step}:")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction Error: {agent.getPredictionError():.3f}")
            print(f"  Development Stage: {brain.getDevelopmentalStage()}")
            print(f"  Plasticity Modifier: {agent.getPlasticityModifier():.3f}")
            print(f"  Action Stats: {explore_stats}")
    
    return exploration_stats

# Run curiosity-driven agent
stats = run_curiosity_driven_agent(5000)
print("Final Exploration Stats:", stats)
```

### 4. Debugging and Troubleshooting

#### Common Issues and Solutions

**Issue 1: Brain Not Learning**

```python
import pynlm

# Enable all learning systems
config = pynlm.createDefaultConfig()
config.set("plasticity_enabled", True)
config.set("neuromodulation_enabled", True)
config.set("curiosity_enabled", True)
config.set("development_enabled", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Check if plasticit
```

**Issue 2: Low Performance**

```python
import pynlm

# Performance optimization configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)  # Smaller for testing
config.set("simulation_timestep", 0.001)
config.set("visualization_enabled", False)  # Disable for speed

# Run performance test
brain = pynlm.createBrain(config)
brain.initialize()

import time
start_time = time.time()
for i in range(1000):
    brain.step(i)
elapsed = time.time() - start_time

print(f"Performance: {1000/elapsed:.0f} steps/second")
```

### 5. Advanced Experiment Setup

#### Memory Consolidation Experiment

```python
import pynlm

def run_memory_consolidation_experiment():
    """Experiment to study memory consolidation during rest."""
    
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 3000)
    config.set("episodic_memory_capacity", 2000)
    config.set("working_memory_capacity", 100)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with episodic memory
    agent = pynlm.createAgentBrain(brain)
    
    # Run learning phase
    for step in range(2000):
        # Simulate experiences
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20)
        world.reset()
        agent.initialize(world)
        
        # Agent learns from experiences
        for t in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step * 100 + t)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    # Run consolidation phase (rest period)
    NLM_LOG_INFO("=== Memory Consolidation Phase ===")
    for step in range(1000):
        brain.step(step + 2000)
        # No new input - consolidation during rest
        
        # Check memory retention
        if step % 100 == 0:
            retention = agent.getEpisodicMemory().getRetentionRate()
            NLM_LOG_INFO(f"Consolidation step {step}: Memory retention = {retention:.3f}")
    
    return brain

# Run memory consolidation experiment
consolidation_brain = run_memory_consolidation_experiment()
```

### 6. API Reference

#### Advanced Configuration Methods

```python
# Set multiple configuration values at once
config.setMultiple({
    "brain.neuron_count": 5000,
    "brain.synapse_density": 0.12,
    "plasticity.stdp.enable": True,
    "neuromodulation.curiosity.enable": True,
    "environment.width": 100,
    "environment.height": 100
})

# Get configuration with validation
def getValidatedConfig():
    config = pynlm.createDefaultConfig()
    
    # Validate configuration
    if not config.has("brain.neuron_count"):
        raise ValueError("neuron_count is required")
    
    if config.get("brain.neuron_count") <= 0:
        raise ValueError("neuron_count must be positive")
    
    if config.get("simulation_timestep") <= 0:
        raise ValueError("simulation_timestep must be positive")
    
    return config

# Configuration templates
def createDevelopmentConfig():
    """Configuration for developmental studies."""
    config = pynlm.createDefaultConfig()
    config.set("development_enabled", True)
    config.set("plasticity_learning_rate", 0.01)
    config.set("development_synaptogenesis_rate", 0.005)
    config.set("development_pruning_rate", 0.001)
    return config

def createLearningConfig():
    """Configuration for learning studies."""
    config = pynlm.createDefaultConfig()
    config.set("neuromodulation_enabled", True)
    config.set("curiosity_enabled", True)
    config.set("reward_discount_factor", 0.95)
    config.set("stdp_ltp_weight", 0.03)
    config.set("stdp_ltd_weight", 0.025)
    return config
```

### 7. Performance Monitoring

#### System Monitoring

```python
import pynlm
import psutil
import time

class PerformanceMonitor:
    def __init__(self, brain, interval=1000):
        self.brain = brain
        self.interval = interval
        self.metrics = []
        
    def start_monitoring(self, duration):
        """Monitor performance for specified duration."""
        start_time = time.time()
        step_count = 0
        
        while time.time() - start_time < duration:
            step_start = time.time()
            self.brain.step(step_count)
            step_count += 1
            
            if step_count % self.interval == 0:
                self.collect_metrics(step_count, time.time() - step_start)
                
    def collect_metrics(self, step_count, step_time):
        """Collect performance metrics."""
        process = psutil.Process()
        memory_info = process.memory_info()
        
        metrics = {
            'step': step_count,
            'step_time': step_time,
            'steps_per_second': 1.0 / step_time if step_time > 0 else 0,
            'memory_rss': memory_info.rss,
            'memory_vms': memory_info.vms,
            'cpu_percent': process.cpu_percent(),
            'neuron_count': self.brain.getTotalNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitement_inhibition_ratio': self.brain.getExcitationInhibitionRatio()
        }
        
        self.metrics.append(metrics)
        
    def get_summary(self):
        """Get performance summary."""
        if not self.metrics:
            return None
            
        total_steps = self.metrics[-1]['step']
        total_time = sum(m['step_time'] for m in self.metrics)
        
        return {
            'total_steps': total_steps,
            'total_time': total_time,
            'average_steps_per_second': total_steps / total_time,
            'peak_memory_mb': max(m['memory_rss'] for m in self.metrics) / (1024 * 1024),
            'average_cpu_percent': sum(m['cpu_percent'] for m in self.metrics) / len(self.metrics),
            'final_neuron_count': self.metrics[-1]['neuron_count'],
            'final_firing_rate': self.metrics[-1]['average_firing_rate']
        }

# Example usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

monitor = PerformanceMonitor(brain, interval=100)
monitor.start_monitoring(duration=60.0)  # Monitor for 60 seconds

summary = monitor.get_summary()
print("Performance Summary:", summary)
```

## Conclusion

The NLM project has been significantly improved with:

1. **Fixed configuration issues**: Resolved duplicate keys and improved parsing
2. **Enhanced error handling**: Better bounds checking and informative error messages
3. **Eliminated magic numbers**: Comprehensive constants system for maintainability
4. **Advanced command-line interface**: Rich set of options for flexible control
5. **Improved neuron distribution**: Optimized algorithms for better performance
6. **Configuration validation**: Robust validation and helpful error messages
7. **Performance improvements**: Memory management and optimized data structures
8. **Advanced examples and documentation**: Comprehensive guide for users at all levels

These improvements make NLM more robust, maintainable, and accessible to users ranging from beginners to advanced researchers.
