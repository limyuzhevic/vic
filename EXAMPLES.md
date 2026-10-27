# NLM Python Bindings - Complete Examples Guide

This document provides comprehensive examples for working with the NLM Python bindings.

## Quick Start

```python
import pynlm as nlm

# Create a default brain configuration
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Create a world and agent
world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

print("NLM brain simulation environment ready!")
```

## Basic Usage Examples

### 1. Creating Brains with Different Templates

```python
import pynlm as nlm

# Small brain for quick testing
small_brain = nlm.createBrainTemplate("small")
small_brain.initialize()

# Medium brain for general use
medium_brain = nlm.createBrainTemplate("medium")
medium_brain.initialize()

# Large brain for complex simulations
large_brain = nlm.createBrainTemplate("large")
large_brain.initialize()

# Check brain status
print(f"Total neurons: {medium_brain.getTotalNeuronCount()}")
print(f"Average firing rate: {medium_brain.getAverageFiringRate():.4f}")
```

### 2. Working with Configuration Files

```python
import pynlm as nlm

# Create brain from configuration file
brain = nlm.createBrainFromConfig("my_config.cfg")

# Or using command line arguments
brain = nlm.createBrainWithArgs(len(sys.argv), sys.argv)

# Validate your configuration
errors = nlm.validateBrainConfig(brain.getConfig())
if errors:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")
else:
    print("Configuration is valid!")
```

### 3. Sensory Processing

```python
import pynlm as nlm

brain = nlm.createBrainTemplate("medium")
world = nlm.createSimpleWorld()
world.configure(128, 128, 64, 64)  # width, height, visionWidth, visionHeight
world.setAgentStart(64, 64)

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Simulate an episode
episodes = 10
for episode in range(episodes):
    total_reward, novelty = nlm.simulateEpisode(brain, world, maxSteps=100, timestep=0.001)
    print(f"Episode {episode + 1}: Reward = {total_reward:.2f}, Novelty = {novelty:.4f}")
```

### 4. Action Creation and Control

```python
import pynlm as nlm

# Create standard actions
action_move = nlm.createStandardAction(nlm.ActionType.MoveForward)
action_look = nlm.createStandardAction(nlm.ActionType.Look, 30.0)  # 30 degrees

action_custom = nlm.createCustomAction(nlm.ActionType.Custom, [0.5, 0.3, 0.8])

print(f"Move action type: {action_move.getType()}")
print(f"Look action name: {action_look.getName()}")
print(f"Custom action parameters: {action_custom.getParameters()}")
```

### 5. Advanced Agent Control

```python
import pynlm as nlm
import time

# Create a complete agent environment
config = nlm.createDefaultConfig()
config.set("neuron_count", 500)
config.set("region_count", 3)

brain = nlm.createBrain(config)
world = nlm.createSimpleWorld()
world.configure(256, 256, 128, 128)
world.setAgentStart(128, 128)

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run a complex simulation
start_time = time.time()

for step in range(1000):
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, step * 0.001)
    
    # Apply reward and update development
    reward = world.computeReward(brain.get())
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.001)
    
    # Log progress
    if step % 100 == 0:
        features = nlm.extractNeuralFeatures(brain)
        print(f"Step {step}: Reward={reward:.3f}, Novelty={agent.getNoveltyLevel():.3f}, "
              f"Curiosity={agent.getCuriosityLevel():.3f}")
        print(f"  Neural features: {features}")

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
```

## Advanced Features

### 1. Neuromodulation and Learning

```python
import pynlm as nlm

brain = nlm.createBrainTemplate("medium")
world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all neuromodulation systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Simulate learning over time
for episode in range(50):
    for step in range(100):
        # Process environment
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step, step * 0.001)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, step * 0.001)
        
        reward = world.computeReward(brain.get())
        predicted_reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted_reward)
        agent.updateDevelopment(0.001)
    
    # Log learning progress
    if episode % 10 == 0:
        print(f"Episode {episode}:")
        print(f"  Dopamine level: {agent.getNeuromodulationLevel():.3f}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
```

### 2. Checkpoint System

```python
import pynlm as nlm
import tempfile

brain = nlm.createBrainTemplate("medium")
brain.initialize()

# Run some steps to create interesting brain state
for step in range(200):
    brain.step(step, step * 0.001)

# Save checkpoint
with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
    checkpoint_path = f.name

brain.save(checkpoint_path)
print(f"Checkpoint saved to: {checkpoint_path}")

# Create new brain and load checkpoint
new_brain = nlm.createBrainTemplate("medium")
new_brain.initialize()

if new_brain.load(checkpoint_path):
    print("Checkpoint loaded successfully!")
    print(f"Restored spike count: {new_brain.getTotalSpikeCount()}")
else:
    print("Failed to load checkpoint")
```

### 3. Neural Feature Analysis

```python
import pynlm as nlm
import matplotlib.pyplot as plt

# Create multiple brains for comparison
brains = []
for template in ["small", "medium", "large"]:
    brain = nlm.createBrainTemplate(template)
    brain.initialize()
    brains.append((template, brain))

# Analyze and compare neural features
print("Neural Feature Comparison:")
print("=" * 50)

for template, brain in brains:
    features = nlm.extractNeuralFeatures(brain)
    print(f"\n{template.upper()} Brain:")
    for feature, value in features.items():
        print(f"  {feature}: {value:.4f}")

# Visualize firing rate distribution
print("\nFiring Rate Distributions:")
for template, brain in brains:
    firing_rate = brain.getAverageFiringRate()
    active_ratio = brain.getActiveNeuronCount() / brain.getTotalNeuronCount()
    print(f"{template}: Avg firing rate = {firing_rate:.4f}, Active neurons = {active_ratio:.2%}")
```

## Performance and Debugging

### 1. Performance Monitoring

```python
import pynlm as nlm
import time
import psutil
import os

def monitor_performance(brain, world, steps):
    process = psutil.Process(os.getpid())
    memory_before = process.memory_info().rss / 1024 / 1024  # MB
    
    start_time = time.time()
    start_memory = memory_before
    
    for step in range(steps):
        # Run simulation step
        brain.step(step, step * 0.001)
        
        if step % 100 == 0:
            memory_current = process.memory_info().rss / 1024 / 1024
            cpu_percent = process.cpu_percent()
            
            print(f"Step {step}: "
                  f"Memory={memory_current - start_memory:.1f}MB, "
                  f"CPU={cpu_percent}%, "
                  f"Firing rate={brain.getAverageFiringRate():.4f}")
    
    end_time = time.time()
    memory_after = process.memory_info().rss / 1024 / 1024
    
    print(f"\nPerformance Summary:")
    print(f"  Total time: {end_time - start_time:.2f} seconds")
    print(f"  Total memory change: {memory_after - start_memory:.1f} MB")
    print(f"  Steps per second: {steps / (end_time - start_time):.1f}")
    print(f"  Memory per step: {(memory_after - start_memory) / steps:.2f} MB")

# Run performance test
brain = nlm.createBrainTemplate("medium")
world = nlm.createSimpleWorld()
brain.initialize()
monitor_performance(brain, world, 500)
```

### 2. Debug Mode

```python
import pynlm as nlm

# Enable debug logging
nlm.set_log_level(nlm.LogLevel.Debug)

brain = nlm.createBrainTemplate("medium")
brain.initialize()

# Run a debug simulation
print("Starting debug simulation...")
for step in range(50):
    brain.step(step, step * 0.001)
    
    # Log neural activity
    if brain.getFiringNeuronCount() > 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
              f"Total spikes: {brain.getTotalSpikeCount()}")

print("Debug simulation complete.")
```

## Error Handling and Best Practices

### 1. Robust Configuration Loading

```python
import pynlm as nlm
import sys

def safe_create_brain(config_file=None):
    """Safely create a brain with fallback configuration."""
    try:
        if config_file:
            brain = nlm.createBrainFromConfig(config_file)
        else:
            brain = nlm.createBrainWithArgs(len(sys.argv), sys.argv)
        
        # Validate configuration
        errors = nlm.validateBrainConfig(brain.getConfig())
        if errors:
            print(f"Warning: Configuration issues found:")
            for error in errors:
                print(f"  - {error}")
        
        brain.initialize()
        return brain
    
    except Exception as e:
        print(f"Error creating brain: {e}")
        print("Using default configuration...")
        return nlm.createBrainTemplate("medium")

# Usage
brain = safe_create_brain("my_config.cfg")
```

### 2. Resource Management

```python
import pynlm as nlm

# Use context managers for resource cleanup
class NLMEnvironment:
    def __init__(self, brain_template="medium"):
        self.brain = nlm.createBrainTemplate(brain_template)
        self.world = nlm.createSimpleWorld()
        self.agent = None
        
    def initialize(self):
        self.brain.initialize()
        self.agent = nlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
    def run_episode(self, max_steps=100):
        """Run a single episode and return results."""
        total_reward = 0.0
        
        for step in range(max_steps):
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step, step * 0.001)
            
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, step * 0.001)
            
            reward = self.world.computeReward(self.brain.get())
            total_reward += reward
            
            self.agent.applyRewardModulation(reward, 0.0)
            self.agent.updateDevelopment(0.001)
        
        return total_reward
    
    def cleanup(self):
        """Cleanup resources."""
        if self.brain:
            # Save final state if needed
            pass

# Usage with context management
env = NLMEnvironment("medium")
env.initialize()

for episode in range(10):
    reward = env.run_episode()
    print(f"Episode {episode + 1}: Reward = {reward:.2f}")

env.cleanup()
print("Environment cleaned up successfully.")
```

## API Reference Summary

### Core Classes

- `Brain` - Main neural simulation engine
- `Config` - Configuration management
- `AgentBrain` - Brain-world interface
- `SimpleWorld` - Simulation environment

### Key Methods

- `createDefaultConfig()` - Create default configuration
- `createBrain(config)` - Create brain from config
- `createBrainFromConfig(filepath)` - Load from file
- `createBrainWithArgs()` - Load from command line
- `createBrainTemplate(name)` - Predefined templates
- `createAgentBrain(brain)` - Create agent interface
- `createSimpleWorld()` - Create simulation world

### Advanced Functions

- `validateBrainConfig(config)` - Validate configuration
- `simulateEpisode(brain, world, maxSteps, timestep)` - Run complete episode
- `extractNeuralFeatures(brain)` - Get neural analysis data
- `createStandardAction(type, params)` - Create actions
- `createCustomAction(type, params)` - Create custom actions

## Troubleshooting

### Common Issues and Solutions

**Q: Configuration file not found**
```python
# Solution: Create default file or use template
brain = nlm.createBrainTemplate("medium")
```

**Q: Memory issues with large brains**
```python
# Solution: Use smaller templates or run in episodes
brain = nlm.createBrainTemplate("small")
```

**Q: Invalid action type**
```python
# Solution: Check available action types
import pynlm as nlm
valid_actions = [nlm.ActionType.MoveForward, nlm.ActionType.TurnLeft, ...]
```

**Q: Performance problems**
```python
# Solution: Monitor performance and optimize
summary = nlm.monitor_performance(brain, world, 500)
```

This comprehensive guide should help you get started with NLM Python bindings and explore the full range of features available for building neural simulation environments.