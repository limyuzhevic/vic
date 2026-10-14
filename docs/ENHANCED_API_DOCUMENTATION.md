# NLM Documentation Enhancement - Interactive Tutorials
# 
# This documentation includes interactive tutorials, API references, and best practices
# for using the enhanced NLM (Neural Learning Machine) framework.

## Table of Contents

1. [Quick Start Guide](#quick-start-guide)
2. [Interactive Tutorials](#interactive-tutorials)
   - 2.1 [Creating Your First Brain](#21-creating-your-first-brain)
   - 2.2 [Running a Complete Simulation](#22-running-a-complete-simulation)
   - 2.3 [Working with World Environments](#23-working-with-world-environments)
   - 2.4 [Implementing Agent Behavior](#24-implementing-agent-behavior)
   - 2.5 [Advanced Learning Scenarios](#25-advanced-learning-scenarios)
3. [API Reference](#api-reference)
   - 3.1 [EnhancedBrain Class](#31-enhancedbrain-class)
   - 3.2 [EnhancedWorld Class](#32-enhancedworld-class)
   - 3.3 [EnhancedAgent Class](#33-enhancedagent-class)
   - 3.4 [NLMConfiguration Class](#34-nlmconfiguration-class)
4. [Best Practices](#best-practices)
5. [Troubleshooting](#troubleshooting)
6. [Advanced Features](#advanced-features)
7. [Performance Optimization](#performance-optimization)

---

# Quick Start Guide

This guide provides a quick introduction to using the enhanced NLM framework.

## What You'll Learn

- How to create and initialize a brain
- How to set up a world environment
- How to implement agent behavior
- How to run and monitor simulations
- How to save and load brain states

## Installation

The enhanced NLM framework is already installed. You can start using it immediately:

```python
import nlm_enhanced

# Quick test
brain = nlm_enhanced.create_brain(neuron_count=1000)
brain.initialize()
print(f"Created brain with {brain.get_stats()['firing_neurons']} neurons")
```

---

# Interactive Tutorials

## 21 Creating Your First Brain

### Step 1: Create a Configuration

```python
import nlm_enhanced

# Create a default configuration
brain = nlm_enhanced.create_brain(
    neuron_count=2000,
    timestep=0.01,
    max_steps=5000,
    verbose=True
)

# The configuration automatically validates your parameters
# and provides helpful error messages for invalid values
```

### Step 2: Initialize the Brain

```python
# Initialize the brain with enhanced error handling
try:
    brain.initialize()
    print("Brain initialized successfully!")
except Exception as e:
    print(f"Error initializing brain: {e}")
```

### Step 3: Run a Simple Simulation

```python
# Run a simulation with progress monitoring
brain.run_simulation(
    steps=1000,
    verbose=True,
    callback=lambda step, b: print(f"Progress: {step}/1000")
)

# Check results
stats = brain.get_stats()
print(f"Simulation completed:")
print(f"  - Total spikes: {stats['total_spikes']}")
print(f"  - Firing neurons: {stats['firing_neurons']}")
print(f"  - Average firing rate: {stats['average_firing_rate']:.2f} Hz")
print(f"  - Development stage: {stats['development_stage']}")
```

### Step 4: Save and Load Brain State

```python
# Save the brain state
brain.save_state("my_brain.bin")

# Create a new brain and load the saved state
new_brain = nlm_enhanced.create_brain()
new_brain.load_state("my_brain.bin")

print("Brain state saved and loaded successfully!")
```

## 22 Running a Complete Simulation

This tutorial demonstrates how to run a complete simulation with world interaction.

### Step 1: Create World Environment

```python
# Create a world with custom settings
world = nlm_enhanced.create_world(
    width=30,
    height=30,
    vision_width=8,
    vision_height=8
)

# Add resources and hazards
world.add_resource(5, 5, 1.0)
world.add_resource(15, 15, 2.0)
world.add_hazard(10, 10, -0.5)
world.add_hazard(25, 25, -1.0)

# Set agent starting position
world.set_agent_start(15.0, 15.0)

print("World created with resources and hazards!")
```

### Step 2: Create and Configure Brain

```python
# Create brain with optimal settings for the world
brain = nlm_enhanced.create_brain(
    neuron_count=2000,
    timestep=0.01,
    simulation_timestep=0.01,
    plasticity_stdp_enable=True,
    plasticity_hebbian_enable=True,
    neuromod_dopamine_scale=1.0,
    neuromod_curiosity_enable=True,
    neuromod_novelty_enable=True
)

brain.initialize(verbose=True)
```

### Step 3: Create and Configure Agent

```python
# Create agent for world interaction
agent = nlm_enhanced.create_agent(brain, world)

# Enable all learning subsystems
agent.enable_all_subsystems()

# Configure specific subsystems
agent.enable_subsystem("reward", True)
agent.enable_subsystem("development", True)
agent.enable_subsystem("curiosity", True)
agent.enable_subsystem("structural", True)

print("Agent configured with all learning subsystems!")
```

### Step 4: Run Complete Episode

```python
# Run complete episode with progress monitoring
print("Running complete episode...")
print("=" * 50)

total_reward = world.run_episode(
    brain=brain,
    agent=agent,
    steps=200,
    verbose=True
)

print("=" * 50)
print(f"Episode completed with total reward: {total_reward}")
print(f"Final curiosity level: {agent.get_curiosity_level():.3f}")
print(f"Final novelty level: {agent.get_novelty_level():.3f}")
print(f"Final prediction error: {agent.get_prediction_error():.3f}")

# Get brain statistics
stats = brain.get_stats()
print(f"Brain statistics:")
print(f"  - Neurons firing: {stats['firing_neurons']}")
print(f"  - Total spikes: {stats['total_spikes']}")
print(f"  - Development stage: {stats['development_stage']}")
```

### Step 5: Analyze and Extend

```python
# Save the episode results
brain.save_state("episode_checkpoint.bin")

# Modify and run another episode with different conditions
world.configure(width=40, height=40, vision_width=10, vision_height=10)
world.add_resource(20, 20, 3.0)
world.add_hazard(30, 30, -2.0)

# Run another episode with different parameters
new_episode_reward = world.run_episode(
    brain=agent.get_brain(),
    agent=agent,
    steps=150,
    verbose=False
)

print(f"Second episode reward: {new_episode_reward}")
```

## 23 Working with World Environments

This tutorial covers working with world environments and their features.

### World Configuration

```python
# Create world with custom configuration
world = nlm_enhanced.create_world(
    width=25,           # World width (cells)
    height=25,          # World height (cells)
    vision_width=8,     # Vision width (cells)
    vision_height=8     # Vision height (cells)
)

# Configure world with additional parameters
world.configure(
    width=30,
    height=30,
    vision_width=10,
    vision_height=10,
    setAgentStart=(20.0, 20.0)
)
```

### World Objects

```python
# Add different types of world objects

# Resources (positive reward)
world.add_resource(x=5, y=5, value=1.0)
world.add_resource(x=15, y=15, value=2.5)
world.add_resource(x=25, y=25, value=5.0)

# Hazards (negative reward)
world.add_hazard(x=10, y=10, value=-0.5)
world.add_hazard(x=20, y=20, value=-1.0)

# Walls (boundaries)
# Note: Walls are automatically created at world boundaries
# You can add additional walls using world.addObject()

# Markers (navigation aids)
# Add markers for important locations
world.add_object(pynlm.WorldObject(x=12, y=12, type=pynlm.WorldObjectType.Marker, value=0.5))
```

### World Interaction

```python
# Set agent start position
world.set_agent_start(x=15.0, y=15.0)

# Update world state
world.update(timestep=0.1)

# Get current sensory percept
percept = world.get_sensory_percept()

# Access different sensory modalities
vision = world.get_vision()        # Vision data
touch = percept.get_touch()        # Touch data
internal = percept.get_internal()  # Internal signals (reward)

# Check position validity
if world.is_valid_position(x=5.0, y=5.0):
    print("Position is valid")
else:
    print("Position is invalid")
```

### World Monitoring

```python
# Monitor world properties
print(f"World dimensions: {world.get_width()} x {world.get_height()} cells")
print(f"Vision dimensions: {world.get_vision_width()} x {world.get_vision_height()} cells")
print(f"Agent position: {world.get_agent_body().x}, {world.get_agent_body().y}")
print(f"Agent orientation: {world.get_agent_body().orientation} radians")
print(f"Agent energy: {world.get_agent_body().energy} / 1.0")
print(f"Simulation time: {world.get_simulation_time()} seconds")
```

## 24 Implementing Agent Behavior

This tutorial covers implementing agent behavior with enhanced features.

### Agent Configuration

```python
# Create agent with specific configuration
brain = nlm_enhanced.create_brain(neuron_count=1000)
world = nlm_enhanced.create_world()
agent = nlm_enhanced.create_agent(brain, world)

# Configure learning subsystems
agent.enable_subsystem("reward", True)      # Reward-based learning
agent.enable_subsystem("development", True) # Developmental changes
agent.enable_subsystem("curiosity", True)   # Curiosity-driven exploration
agent.enable_subsystem("structural", True) # Structural plasticity
```

### Sensory Processing

```python
# Process sensory input
percept = world.get_sensory_percept()
agent.process_sensory_input(percept)

# Access different sensory modalities
vision_data = world.get_vision()
touch_data = percept.get_touch()
internal_data = percept.get_internal()
proprioception_data = percept.get_proprioception()
audio_data = percept.get_audio()

print(f"Vision range: [{min(vision_data):.3f}, {max(vision_data):.3f}]")
print(f"Touch signals: {touch_data}")
print(f"Internal signals: {internal_data}")
print(f"Proprioception: {proprioception_data}")
```

### Motor Control

```python
# Get motor command from agent
motor_cmd = agent.decode_motor_command()

# Apply motor command to world
world.apply_motor_command(motor_cmd, world.get_simulation_time())

# Check what action the agent chose
if motor_cmd == pynlm.MotorCommand.MoveForward:
    print("Agent decided to move forward")
elif motor_cmd == pynlm.MotorCommand.TurnLeft:
    print("Agent decided to turn left")
elif motor_cmd == pynlm.MotorCommand.Interact:
    print("Agent decided to interact with environment")
else:
    print("Agent decided to wait")
```

### Learning and Adaptation

```python
# Apply reward modulation based on agent's actions
reward = world.get_internal_signals()[0] if world.get_internal_signals() else 0.0
predicted_reward = 0.0  # In a real scenario, this would be the agent's prediction
agent.apply_reward_modulation(reward, predicted_reward)

# Update development system
agent.update_development(0.1)

# Check agent's cognitive state
print(f"Curiosity level: {agent.get_curiosity_level():.3f}")
print(f"Novelty level: {agent.get_novelty_level():.3f}")
print(f"Prediction error: {agent.get_prediction_error():.3f}")
print(f"Neuromodulation level: {agent.get_neuromodulation_level():.3f}")
print(f"Development stage: {agent.get_developmental_stage()}")
```

### Custom Agent Behavior

```python
# Implement custom agent behavior with callbacks

def custom_behavior_callback(step, agent):
    """Custom behavior based on agent's internal state."""
    curiosity = agent.get_curiosity_level()
    novelty = agent.get_novelty_level()
    
    # Explore when curiosity is high
    if curiosity > 0.7 and novelty > 0.3:
        print(f"Step {step}: Agent exploring due to high curiosity and novelty")
    
    # Learn from prediction errors
    prediction_error = agent.get_prediction_error()
    if abs(prediction_error) > 0.5:
        print(f"Step {step}: Agent learning from large prediction error")

# Create world for this example
world = nlm_enhanced.create_world(width=20, height=20)
world.add_resource(5, 5, 2.0)
world.add_hazard(15, 15, -1.0)

brain = nlm_enhanced.create_brain()
agent = nlm_enhanced.create_agent(brain, world)

# Run episode with custom callback
world.run_episode(brain, agent, steps=100, callback=custom_behavior_callback)
```

## 25 Advanced Learning Scenarios

This tutorial covers advanced learning scenarios and features.

### Multi-Agent Learning

```python
# Create multiple agents for collaborative learning
agents = []
worlds = []

for i in range(3):
    brain = nlm_enhanced.create_brain(neuron_count=500)
    world = nlm_enhanced.create_world(width=15, height=15)
    world.add_resource(i*5, i*5, 1.0 + i*0.5)
    
    agent = nlm_enhanced.create_agent(brain, world)
    agent.enable_subsystem("social", True)  # Enable social learning
    
    agents.append(agent)
    worlds.append(world)

# Run multi-agent simulation
print("Running multi-agent learning scenario...")
for i, (agent, world) in enumerate(zip(agents, worlds)):
    print(f"Agent {i+1}: running episode with {agent.get_curiosity_level():.2f} curiosity")
    world.run_episode(agent.get_brain(), agent, steps=50, verbose=False)
```

### Transfer Learning

```python
# Implement transfer learning across tasks

# Task 1: Navigate to resource
task1_world = nlm_enhanced.create_world(width=20, height=20)
task1_world.add_resource(5, 5, 2.0)
task1_brain = nlm_enhanced.create_brain(neuron_count=1000)
task1_agent = nlm_enhanced.create_agent(task1_brain, task1_world)

# Train on task 1
task1_reward = task1_world.run_episode(task1_brain, task1_agent, steps=100)
print(f"Task 1 completed with reward: {task1_reward}")

# Transfer to Task 2: Navigate to different resource
task2_world = nlm_enhanced.create_world(width=20, height=20)
task2_world.add_resource(15, 15, 2.0)  # Different location
task2_brain = nlm_enhanced.create_brain(neuron_count=1000)
task2_brain.load_state("task1_brain.bin")  # Load learned weights
task2_agent = nlm_enhanced.create_agent(task2_brain, task2_world)

# Run task 2 with transferred knowledge
task2_reward = task2_world.run_episode(task2_brain, task2_agent, steps=100)
print(f"Task 2 completed with reward: {task2_reward} (transferred knowledge)")
```

### Lifelong Learning

```python
# Implement lifelong learning with concept formation

# Create world with multiple resource types
world = nlm_enhanced.create_world(width=30, height=30)

# Add different resources at different locations
resources = [
    (5, 5, 1.0, "food"),
    (15, 15, 2.0, "water"),
    (25, 25, 3.0, "nourishment"),
    (10, 20, 1.5, "energy"),
    (20, 10, 2.5, "sustenance")
]

for x, y, value, resource_type in resources:
    world.add_resource(x, y, value)

# Create brain with concept formation
brain = nlm_enhanced.create_brain(
    neuron_count=2000,
    cognition_concept_formation_enable=True,
    cognition_concept_formation_similarity_threshold=0.7
)

agent = nlm_enhanced.create_agent(brain, world)

# Run multiple episodes to learn different resources
for episode in range(10):
    episode_reward = world.run_episode(brain, agent, steps=50)
    print(f"Episode {episode + 1}: Reward = {episode_reward:.2f}")
    
    # Save learned concepts periodically
    if (episode + 1) % 5 == 0:
        brain.save_state(f"lifelong_learning_checkpoint_{episode + 1}.bin")
        print(f"  -> Concepts saved to checkpoint {episode + 1}")
```

---

# API Reference

## 31 EnhancedBrain Class

### Methods

#### `EnhancedBrain(config=None)`
Create enhanced brain with optional configuration.

```python
brain = EnhancedBrain()  # Default configuration
brain = EnhancedBrain(config)  # Custom configuration
```

#### `initialize(config_dict=None, verbose=False)`
Initialize brain with optional parameter overrides.

```python
brain.initialize()  # Use default configuration
brain.initialize({'brain.neuron_count': 2000})  # Override specific parameter
brain.initialize(verbose=True)  # Enable verbose logging
```

#### `run_simulation(steps, verbose=False, callback=None)`
Run simulation with progress monitoring.

```python
brain.run_simulation(1000)  # Basic simulation
brain.run_simulation(500, verbose=True)  # With verbose logging
brain.run_simulation(200, callback=my_callback)  # With custom callback
```

#### `save_state(filepath)`
Save brain state to file.

```python
brain.save_state("brain_state.bin")
```

#### `load_state(filepath)`
Load brain state from file.

```python
brain.load_state("brain_state.bin")
```

#### `reset()`
Reset brain to initial state.

```python
brain.reset()
```

#### `get_stats()`
Get brain statistics.

```python
stats = brain.get_stats()
print(f"Firing neurons: {stats['firing_neurons']}")
print(f"Total spikes: {stats['total_spikes']}")
```

#### `get_config()`
Get configuration object.

```python
config = brain.get_config()
neuron_count = config.get('brain.neuron_count')
```

---

## 32 EnhancedWorld Class

### Methods

#### `EnhancedWorld(width=20, height=20, vision_width=8, vision_height=8)`
Create enhanced world.

```python
world = EnhancedWorld(30, 30, 10, 10)
```

#### `configure(width=None, height=None, vision_width=None, vision_height=None, **kwargs)`
Configure world parameters.

```python
world.configure(width=40, height=40, vision_width=12, vision_height=12)
world.configure(setAgentStart=(20.0, 20.0))
```

#### `reset()`
Reset world to initial state.

```python
world.reset()
```

#### `update(timestep=0.1)`
Update world state.

```python
world.update(0.1)
```

#### `get_sensory_percept()`
Get current sensory percept.

```python
percept = world.get_sensory_percept()
```

#### `get_agent_body()`
Get agent body state.

```python
body = world.get_agent_body()
```

#### `apply_motor_command(motor_cmd, current_time=0.0)`
Apply motor command to world.

```python
world.apply_motor_command(motor_cmd, current_time)
```

#### `set_agent_start(x, y)`
Set agent starting position.

```python
world.set_agent_start(15.0, 15.0)
```

#### `add_resource(x, y, value=1.0)`
Add resource to world.

```python
world.add_resource(5, 5, 2.0)
```

#### `add_hazard(x, y, value=-1.0)`
Add hazard to world.

```python
world.add_hazard(10, 10, -0.5)
```

#### `is_valid_position(x, y)`
Check if position is valid.

```python
if world.is_valid_position(5.0, 5.0):
    print("Position is valid")
```

---

## 33 EnhancedAgent Class

### Methods

#### `EnhancedAgent(brain, world)`
Create enhanced agent.

```python
agent = EnhancedAgent(brain, world)
```

#### `enable_all_subsystems()`
Enable all agent subsystems.

```python
agent.enable_all_subsystems()
```

#### `enable_subsystem(subsystem, enable=True)`
Enable or disable specific subsystem.

```python
agent.enable_subsystem("reward", True)
agent.enable_subsystem("development", True)
agent.enable_subsystem("curiosity", True)
agent.enable_subsystem("structural", True)
```

#### `process_sensory_input(percept)`
Process sensory input.

```python
agent.process_sensory_input(percept)
```

#### `decode_motor_command()`
Decode motor command from brain.

```python
motor_cmd = agent.decode_motor_command()
```

#### `apply_reward_modulation(reward, predicted_reward=0.0)`
Apply reward modulation.

```python
agent.apply_reward_modulation(reward, predicted_reward)
```

#### `update_development(timestep)`
Update development system.

```python
agent.update_development(0.1)
```

#### `get_curiosity_level()`
Get curiosity level.

```python
curiosity = agent.get_curiosity_level()
```

#### `get_novelty_level()`
Get novelty level.

```python
novelty = agent.get_novelty_level()
```

#### `get_prediction_error()`
Get prediction error.

```python
prediction_error = agent.get_prediction_error()
```

#### `get_neuromodulation_level()`
Get neuromodulation level.

```python
neuromodulation = agent.get_neuromodulation_level()
```

#### `get_developmental_stage()`
Get developmental stage.

```python
stage = agent.get_developmental_stage()
```

#### `reset()`
Reset agent for new episode.

```python
agent.reset()
```

---

## 34 NLMConfiguration Class

### Methods

#### `NLMConfiguration(config=None)`
Initialize configuration with optional existing config.

```python
config = NLMConfiguration()
config = NLMConfiguration(existing_config)
```

#### `set(key, value, section=None)`
Set configuration value with validation.

```python
config.set("brain.neuron_count", 2000)
config.set("simulation.timestep", 0.01, "simulation")
```

#### `get(key, section=None)`
Get configuration value.

```python
value = config.get("brain.neuron_count")
```

#### `has(key, section=None)`
Check if configuration key exists.

```python
if config.has("brain.neuron_count"):
    print("Configuration exists")
```

#### `get_all_keys()`
Get all configuration keys.

```python
keys = config.get_all_keys()
```

#### `summary()`
Get configuration summary.

```python
print(config.summary())
```

---

# Best Practices

## 1. Configuration Validation

Always validate your configuration before running simulations:

```python
import nlm_enhanced

# Create configuration with validation
brain = nlm_enhanced.create_brain(
    neuron_count=1000,
    timestep=0.001,
    max_steps=10000
)

# Configuration automatically validates parameters
# Invalid values will raise ValidationError with helpful messages
```

## 2. Error Handling

Implement proper error handling:

```python
try:
    brain = nlm_enhanced.create_brain(neuron_count=-1000)
    brain.initialize()
except ValueError as e:
    print(f"Configuration error: {e}")
except RuntimeError as e:
    print(f"Runtime error: {e}")
```

## 3. Progress Monitoring

Use callbacks for progress monitoring:

```python
def progress_callback(step, brain):
    if step % 100 == 0:
        stats = brain.get_stats()
        print(f"Step {step}: {stats['firing_neurons']} neurons firing")

brain.run_simulation(1000, callback=progress_callback)
```

## 4. State Management

Save and load brain states regularly:

```python
# Save brain state
brain.save_state("checkpoint1.bin")

# Continue simulation
brain.run_simulation(500)

# Load and continue from saved state
new_brain = nlm_enhanced.create_brain()
new_brain.load_state("checkpoint1.bin")
new_brain.run_simulation(500)
```

## 5. Resource Management

Be mindful of computational resources:

```python
# Use smaller configurations for prototyping
brain = nlm_enhanced.create_brain(neuron_count=100)

# Scale up for final simulations
brain = nlm_enhanced.create_brain(neuron_count=10000)
```

## 6. Logging and Debugging

Use verbose logging for debugging:

```python
brain = nlm_enhanced.create_brain(neuron_count=1000, verbose=True)
brain.initialize(verbose=True)
brain.run_simulation(100, verbose=True)
```

---

# Troubleshooting

## Common Issues and Solutions

### Issue 1: "Brain must be initialized before running simulation"

**Solution:** Call `brain.initialize()` before running simulation.

```python
# Correct
brain = nlm_enhanced.create_brain()
brain.initialize()
brain.run_simulation(100)

# Incorrect (causes error)
brain = nlm_enhanced.create_brain()
brain.run_simulation(100)  # Error!
```

### Issue 2: Invalid configuration values

**Solution:** Use valid configuration values.

```python
# Valid configuration
brain = nlm_enhanced.create_brain(neuron_count=1000)

# Invalid configuration (causes error)
brain = nlm_enhanced.create_brain(neuron_count=-1000)  # Error!
```

### Issue 3: Simulation crashes

**Solution:** Check for resource limits and configuration issues.

```python
# Check system resources
import psutil
memory = psutil.virtual_memory()
print(f"Memory usage: {memory.percent}%")

# Use smaller configuration if memory is low
brain = nlm_enhanced.create_brain(neuron_count=100)
```

### Issue 4: Agent not moving

**Solution:** Check agent configuration and world setup.

```python
# Enable motor and curiosity subsystems
agent.enable_subsystem("curiosity", True)

# Check if agent is getting sensory input
percept = world.get_sensory_percept()
agent.process_sensory_input(percept)

# Decode and apply motor command
motor_cmd = agent.decode_motor_command()
world.apply_motor_command(motor_cmd, world.get_simulation_time())
```

### Issue 5: Poor learning performance

**Solution:** Adjust learning parameters.

```python
# Adjust learning rates
brain = nlm_enhanced.create_brain(
    plasticity_stdp_learning_rate=0.001,
    plasticity_hebbian_learning_rate=0.01,
    neuromod_dopamine_scale=1.0,
    neuromod_curiosity_threshold=0.3
)
```

---

# Advanced Features

## 1. Plugin Architecture

The enhanced NLM supports plugin architecture for custom extensions.

```python
class CustomPlugin:
    def initialize(self, brain, world):
        self.brain = brain
        self.world = world
    
    def update(self, timestep):
        # Custom plugin logic
        pass

# Use plugin in simulation
plugin = CustomPlugin()
plugin.initialize(brain, world)
brain.run_simulation(100, callback=plugin.update)
```

## 2. Performance Profiling

Profile simulation performance:

```python
import time
import cProfile
import pstats

# Profile simulation
profiler = cProfile.Profile()
profiler.enable()

brain.run_simulation(1000)

profiler.disable()

# Generate statistics
stats = pstats.Stats(profiler)
stats.sort_stats('cumulative')
stats.print_stats(20)  # Top 20 time-consuming functions
```

## 3. Visualization

Integrate with visualization libraries:

```python
import matplotlib.pyplot as plt
import numpy as np

# Collect simulation statistics
stats = []
for step in range(100):
    brain.run_simulation(1)
    firing_rate = brain.get_stats()['average_firing_rate']
    stats.append(firing_rate)

# Plot firing rate over time
plt.plot(stats)
plt.xlabel('Step')
plt.ylabel('Firing Rate (Hz)')
plt.title('Brain Activity Over Time')
plt.show()
```

## 4. Distributed Computing

Run simulations across multiple processes:

```python
from multiprocessing import Pool
import nlm_enhanced

def run_simulation(config):
    brain = nlm_enhanced.create_brain(**config)
    brain.initialize()
    brain.run_simulation(100)
    return brain.get_stats()

# Configuration for parallel simulation
configs = [
    {'neuron_count': 1000},
    {'neuron_count': 2000},
    {'neuron_count': 3000}
]

# Run in parallel
with Pool(3) as pool:
    results = pool.map(run_simulation, configs)

# Process results
for result in results:
    print(f"Simulation completed: {result['simulation_steps']} steps")
```

---

# Performance Optimization

## 1. Configuration Optimization

Use optimal configurations for different use cases:

```python
# For rapid prototyping
brain = nlm_enhanced.create_brain(neuron_count=100)

# For performance testing
brain = nlm_enhanced.create_brain(
    neuron_count=5000,
    timestep=0.001,
    max_steps=10000
)

# For research experiments
brain = nlm_enhanced.create_brain(
    neuron_count=20000,
    timestep=0.0001,
    max_steps=50000
)
```

## 2. Memory Optimization

Optimize memory usage:

```python
# Clear unused data
brain = nlm_enhanced.create_brain()
brain.initialize()

# Run simulation
brain.run_simulation(100)

# Clear internal caches
brain.reset()

# Force garbage collection
import gc
gc.collect()
```

## 3. CPU Optimization

Optimize CPU usage:

```python
import os

# Set CPU affinity for better performance
os.sched_setaffinity(0, [0])  # Use only CPU 0

# Run simulation with optimized settings
brain = nlm_enhanced.create_brain(
    neuron_count=2000,
    timestep=0.001,
    max_steps=5000
)

brain.initialize()
brain.run_simulation(500)
```

## 4. Disk I/O Optimization

Optimize disk I/O operations:

```python
# Use memory-mapped files for large checkpoints
import mmap
import os

# Save checkpoint
brain.save_state("checkpoint.bin")

# Load checkpoint with memory mapping
with open("checkpoint.bin", "r+b") as f:
    data = mmap.mmap(f.fileno(), 0)
    # Process data...
    data.close()
```

---

# API Reference Summary

## Quick API Reference

```python
# Configuration
config = nlm_enhanced.create_default_config()
config.set("brain.neuron_count", 1000)

# Brain
brain = nlm_enhanced.create_brain()
brain.initialize()
brain.run_simulation(100)

# World
world = nlm_enhanced.create_world(20, 20, 8, 8)
world.add_resource(5, 5, 1.0)

# Agent
agent = nlm_enhanced.create_agent(brain, world)
agent.enable_all_subsystems()

# Run episode
reward = world.run_episode(brain, agent, 200)
```

## Supported Configuration Parameters

- **brain.neuron_count**: Number of neurons (100-100000)
- **brain.synapse_density**: Synapse connectivity density (0.0-1.0)
- **brain.connection_probability**: Connection probability (0.0-1.0)
- **simulation.timestep**: Simulation timestep (0.0001-0.1)
- **simulation.max_steps**: Maximum simulation steps (1-1000000)
- **plasticity.stdp.enable**: STDP plasticity (boolean)
- **plasticity.stdp.learning_rate**: STDP learning rate (0.0-0.1)
- **plasticity.hebbian.enable**: Hebbian plasticity (boolean)
- **neuromod.dopamine.scale**: Dopamine scale (0.0-2.0)
- **neuromod.curiosity.enable**: Curiosity exploration (boolean)

## Method Chaining

All major methods support chaining for fluent API:

```python
brain.initialize() \
    .run_simulation(100, verbose=False) \
    .save_state("checkpoint.bin")

world.configure(width=30, height=30) \
    .set_agent_start(15.0, 15.0) \
    .add_resource(5, 5, 1.0)

agent.enable_all_subsystems() \
    .enable_subsystem("reward", True) \
    .enable_subsystem("curiosity", True)
```

---

# Conclusion

The enhanced NLM framework provides:

1. **Robust error handling** with comprehensive validation
2. **Intuitive API** with high-level convenience methods
3. **Flexible configuration** with automatic validation
4. **Performance optimization** with benchmarking support
5. **Extensible architecture** with plugin support
6. **Comprehensive testing** with integration test suites
7. **Detailed documentation** with interactive tutorials

Start using the enhanced NLM framework today for your neural simulation needs!

---

*This documentation is continuously updated. Check for the latest version at the NLM documentation portal.*
