# NLM API Reference Documentation

## Overview

This document provides comprehensive API documentation for NLM (Neural Learning Machine), covering all major classes, functions, and their usage patterns. The NLM Python bindings provide access to the complete C++ neural simulation framework through an intuitive, Pythonic interface.

## Quick Start

### Basic API Usage

```python
import nlm

# Create configuration and brain
config = nlm.ConfigPresets.createDevelopment()
brain = nlm.Brain(config)
brain.initialize()

# Use context manager for safe operations
with brain.context() as ctx:
    for step in range(100):
        ctx.step(step)
        action = ctx.produce_action()
        # Process action...
```

## Core Classes

### Config Class

The `Config` class provides key-value configuration storage for the NLM system.

#### Creation and Basic Usage

```python
# Create empty config
config = nlm.Config()

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("brain.learning_rate", 0.01)
config.set("world.max_energy", 1.0)

# Get configuration values with type conversion
neuron_count = config.get("brain.neuron_count")
learning_rate = config.get("brain.learning_rate")

# Check if configuration key exists
if config.has("brain.neuron_count"):
    print("Neuron count is configured")

# Get all configuration keys
keys = config.get_keys()

# Save configuration to file
config.save_to_file("config.json")

# Load configuration from file
success = config.load_from_file("config.json")
```

#### Configuration Presets

```python
# Create configurations optimized for different use cases
config = nlm.ConfigPresets.createDevelopment()  # Aggressive learning
config = nlm.ConfigPresets.createProduction()  # Stable, conservative
config = nlm.ConfigPresets.createExploration()  # High plasticity

# Or use the create_config helper function
config = nlm.create_config("development")
```

#### Key Configuration Categories

| Category | Key Pattern | Description |
|----------|-------------|-------------|
| `brain.*` | `brain.neuron_count`, `brain.learning_rate` | Brain parameters |
| `plasticity.*` | `plasticity.stdp.enable`, `plasticity.hebbian.learning_rate` | Plasticity settings |
| `neuromod.*` | `neuromod.dopamine.scale`, `neuromod.curiosity.enable` | Neuromodulation |
| `world.*` | `world.width`, `world.max_energy` | World simulation |
| `sensory.*` | `sensory.vision.width`, `sensory.audio.sample_rate` | Sensory processing |
| `development.*` | `development.stage`, `development.plasticity_rate` | Development parameters |

### Brain Class

The `Brain` class represents the central neural simulation engine, implementing spiking neural network (SNN) dynamics.

#### Creation and Initialization

```python
import nlm

# Create brain with default configuration
brain = nlm.Brain(nlm.ConfigPresets.createDefault())

# Or create brain using helper function
brain = nlm.create_brain(nlm.create_default_config())

# Initialize the brain (required before use)
brain.initialize()
```

#### Core Simulation Methods

```python
# Use context manager for safe, automatic reset
with brain.context() as ctx:
    # Perform a single simulation step
    ctx.step(1)
    ctx.step(2, 0.5)  # With timestamp

# Direct step calls (requires initialization)
for step in range(1000):
    brain.step(step)  # Simple version
    brain.step(step, step * 0.1)  # With time

# Reset brain state
brain.reset()
```

#### Sensory Processing

```python
# Inject sensory input into the brain
from nlm import SensoryInput, NeuronType

# Create sensory input
vision_input = SensoryInput([0.5, 0.8, 0.3], 8, 8)  # 8x8 visual field

# Process sensory input
brain.receive_sensory_input(vision_input)

# Inject current into specific neurons
brain.inject_current(0, 0.1)  # Inject 0.1 current to neuron 0
brain.inject_current_to_neurons(NeuronType.Sensory, 0.05)  # All sensory neurons
```

#### Action Production

```python
# Get motor action from brain activity
action = brain.produce_action()
print(f"Action type: {action.type}")
print(f"Action strength: {action.strength}")
```

#### State Management

```python
# Save brain state to file
brain.save("brain_checkpoint.bin")

# Load brain state from file
brain.load("brain_checkpoint.bin")
```

#### Region Management

```python
# Add new neural regions
region_id = brain.add_region("cortex")
region = brain.get_region(region_id)
print(f"Region count: {brain.get_region_count()}")
```

#### Statistics and Monitoring

```python
# Get brain statistics
stats = nlm.brain_statistics(brain)
for stat in stats:
    print(stat)

# Individual statistics
print(f"Total neurons: {brain.get_total_neuron_count()}")
print(f"Total synapses: {brain.get_total_synapse_count()}")
print(f"Active neurons: {brain.get_active_neuron_count()}")
print(f"Firing neurons: {brain.get_firing_neuron_count()}")
print(f"Average firing rate: {brain.get_average_firing_rate()}")
print(f"E/I ratio: {brain.get_excitation_inhibition_ratio()}")
print(f"Total spikes: {brain.get_total_spike_count()}")
```

### AgentBrain Class

The `AgentBrain` class connects NLM brains to the world environment, handling sensory transduction and motor decoding.

#### Creation and Initialization

```python
import nlm

# Create agent brain interface
agent = nlm.create_agent_brain(brain)

# Initialize with world
agent.initialize(world)
```

#### Sensory and Motor Processing

```python
# Process sensory input from world
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Decode motor commands from brain activity
motor_cmd = agent.decode_motor_command()

# Apply reward modulation for learning
agent.applyRewardModulation(reward, predicted_reward)

# Update development system
agent.updateDevelopment(timestep)
```

#### Subsystem Control

```python
# Enable/disable subsystems
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)
agent.enable_curiosity(True)

# Check subsystem status
print(f"Reward modulation: {agent.is_reward_modulation_enabled()}")
print(f"Structural plasticity: {agent.is_structural_plasticity_enabled()}")
print(f"Development: {agent.is_development_enabled()}")
print(f"Curiosity: {agent.is_curiosity_enabled()}")
```

#### State and Statistics

```python
# Reset agent for new episode
agent.reset()

# Get agent statistics
print(f"Curiosity level: {agent.get_curiosity_level():.3f}")
print(f"Novelty level: {agent.get_novelty_level():.3f}")
print(f"Prediction error: {agent.get_prediction_error():.3f}")
print(f"Neuromodulation level: {agent.get_neuromodulation_level():.3f}")
print(f"Development stage: {agent.get_developmental_stage()}")
```

### SimpleWorld Class

The `SimpleWorld` class represents the simulated environment in which the NLM agent operates.

#### Creation and Configuration

```python
import nlm

# Create world instance
world = nlm.create_simple_world()

# Configure world parameters
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8,
    visionAngle=180,
    maxEnergy=1000
)
```

#### World Control

```python
# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Advance simulation
world.update(timestep=0.1)

# Apply motor commands
action_result = world.applyMotorCommand(action, world.get_simulation_time())
```

#### State Access

```python
# Get sensory percept (what agent perceives)
percept = world.getSensoryPercept()

# Get agent body state
body = world.getAgentBody()
position = body.getPosition()
orientation = body.getOrientation()

# Access world properties
print(f"World size: {world.getWidth()}x{world.getHeight()}")
print(f"Max energy: {world.getMaxEnergy()}")
print(f"Current time: {world.getSimulationTime()}")

# Get random seed for reproducibility
seed = world.getRandomSeed()
world.setRandomSeed(42)
```

#### Object Management

```python
# Add objects to world
world.addObject(nlm.WorldObjectType.Resource, 5.0, 5.0, value=100.0)
world.addObject(nlm.WorldObjectType.Hazard, 15.0, 15.0)

# Check if position is valid
is_valid = world.isValidPosition(10.0, 10.0)

# Remove objects
world.removeObject(5.0, 5.0)
```

### Enumeration Types

#### NeuronType
```python
# Neuron type classifications
nlm.NeuronType.Excitatory  # Excitatory neurons
nlm.NeuronType.Inhibitory  # Inhibitory neurons
nlm.NeuronType.Sensory     # Sensory neurons
nlm.NeuronType.Motor       # Motor neurons
nlm.NeuronType.Modulatory  # Neuromodulatory neurons
nlm.NeuronType.Internal   # Internal processing neurons
```

#### SynapseType
```python
# Synapse type classifications
nlm.SynapseType.Excitatory    # Excitatory synapses
nlm.SynapseType.Inhibitory    # Inhibitory synapses
nlm.SynapseType.Modulatory    # Neuromodulatory synapses
nlm.SynapseType.Electrical   # Electrical synapses
nlm.SynapseType.GapJunction  # Gap junction synapses
```

#### DevelopmentalStage
```python
# Developmental stages
nlm.DevelopmentalStage.Initial        # Newborn stage
nlm.DevelopmentalStage.CriticalPeriod # Sensitive learning period
nlm.DevelopmentalStage.Maturation     # Growing and maturing
nlm.DevelopmentalStage.Adult         # Fully developed
nlm.DevelopmentalStage.Aging         # Age-related decline
```

#### ActionType
```python
# High-level action types
nlm.ActionType.MoveForward  # Move forward
nlm.ActionType.MoveBackward  # Move backward
nlm.ActionType.TurnLeft    # Turn left
nlm.ActionType.TurnRight   # Turn right
nlm.ActionType.Interact    # Interact with objects
nlm.ActionType.Eat        # Eat/consume
nlm.ActionType.Drink      # Drink
```

#### MotorCommand
```python
# Low-level motor commands
nlm.MotorCommand.MoveForward  # Move forward command
nlm.MotorCommand.MoveBackward  # Move backward command
nlm.MotorCommand.TurnLeft    # Turn left command
nlm.MotorCommand.TurnRight   # Turn right command
nlm.MotorCommand.Interact    # Interaction command
nlm.MotorCommand.Wait       # Wait/idle command
```

#### WorldObjectType
```python
# World object types
nlm.WorldObjectType.Empty     # Empty space
nlm.WorldObjectType.Resource  # Valuable resource
nlm.WorldObjectType.Hazard    # Dangerous object
nlm.WorldObjectType.Wall      # Obstacle
nlm.WorldObjectType.Marker    # Special marker
```

### Context Managers

#### BrainContext
```python
import nlm

brain = nlm.Brain(nlm.create_default_config())
brain.initialize()

# Use context manager for safe operations with automatic reset
with brain.context() as ctx:
    ctx.step(1)
    ctx.inject(0, 0.1)
    action = ctx.produce_action()
    # Brain state automatically reset on exit
```

#### WorldContext
```python
import nlm

world = nlm.create_simple_world()
world.configure(width=10, height=10)

# Use context manager for safe world operations
with world.context() as ctx:
    ctx.update(0.1)
    action = ctx.apply(nlm.MotorCommand.MoveForward)
    # World state automatically reset on exit
```

### Utility Functions

#### Progress Tracking
```python
import nlm

# Create progress tracker with callback
def progress_callback(progress, message):
    print(f"Progress: {progress:.1%}, {message}")

tracker = nlm.ProgressTracker(progress_callback, 0.1)

tracker.set(0.5, "Processing steps")
tracker.increment(0.2, "Loading neurons")
print(f"Current progress: {tracker.get_progress():.1%}")
```

#### Batch Operations
```python
import nlm

# Batch inject current into multiple neurons
injections = [(0, 0.1), (1, 0.2), (2, 0.15)]
nlm.batch_inject_current(brain, injections)

# Batch process sensory inputs
inputs = [vision1, audio1, proprioceptive1]
nlm.batch_process_sensory(agent, inputs)
```

#### Type Conversion
```python
import nlm

# Convert between types
value_int = nlm.convert_to_int(3.14)
value_float = nlm.convert_to_float(42)
value_str = nlm.convert_to_string(123)
```

#### Statistical Utilities
```python
import nlm

# Get brain statistics as formatted strings
stats = nlm.brain_statistics(brain)
for stat in stats:
    print(stat)

# Get world information
world_info = nlm.world_info(world)
for info in world_info:
    print(info)
```

#### Mathematical Utilities
```python
import nlm
import numpy as np

# Normalize data values
values = [1.0, 2.0, 3.0, 4.0, 5.0]
normalized = nlm.normalize(values, 0.0, 1.0)

# Create linearly spaced values
linspace_vals = nlm.linspace(0.0, 1.0, 11)  # [0.0, 0.1, 0.2, ..., 1.0]

# Format floating point numbers
formatted = nlm.format_float(3.14159265, 3)  # "3.142"
```

### Error Handling

NLM provides custom exceptions for better error handling:

```python
import nlm

try:
    brain = nlm.Brain(config)
    brain.initialize()
    brain.step(1)
except nlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except nlm.ValidationError as e:
    print(f"Validation error: {e}")
except nlm.ResourceError as e:
    print(f"Resource error: {e}")
except nlm.InvalidOperationError as e:
    print(f"Invalid operation: {e}")
```

## Advanced API Usage

### Custom Configuration

```python
import nlm

# Create custom configuration
config = nlm.Config()

# Set complex parameters
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("development.stage", nlm.DevelopmentalStage.CriticalPeriod)

# Create brain with custom config
brain = nlm.Brain(config)
brain.initialize()
```

### Performance Optimization

```python
import nlm
import time

# Configuration optimized for performance
config = nlm.ConfigPresets.createProduction()
config.set("brain.neuron_count", 2000)  # Balance performance and complexity

# Create brain and timing test
brain = nlm.Brain(config)
brain.initialize()

# Measure simulation performance
start_time = time.time()
steps_completed = 0

for step in range(1000):
    brain.step(step)
    steps_completed += 1

total_time = time.time() - start_time
steps_per_second = steps_completed / total_time
print(f"Performance: {steps_per_second:.1f} steps/second")
```

### Complex Agent Behavior

```python
import nlm

class AdvancedAgent:
    def __init__(self, brain, world):
        self.brain = brain
        self.agent = nlm.create_agent_brain(brain)
        self.world = world
        
        # Initialize agent
        self.agent.initialize(world)
        
        # Enable all subsystems
        self.setup_all_systems()
    
    def setup_all_systems(self):
        """Enable all learning subsystems"""
        self.agent.enable_reward_modulation(True)
        self.agent.enable_structural_plasticity(True)
        self.agent.enable_development(True)
        self.agent.enable_curiosity(True)
    
    def run_episode(self, max_steps):
        """Run complete agent episode"""
        for step in range(max_steps):
            # World dynamics
            self.world.update(0.1)
            
            # Perception
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain processing
            self.brain.step(step)
            
            # Action
            action = self.agent.decode_motor_command()
            self.world.applyMotorCommand(action, self.world.get_simulation_time())
            
            # Learning
            reward = self.calculate_reward(percept)
            self.agent.apply_reward_modulation(reward, 0.0)
            
            # Development
            self.agent.update_development(0.1)
            
            # Check termination
            if self.check_termination():
                break
        
        return self.get_episode_stats()
    
    def calculate_reward(self, percept):
        """Calculate reward based on agent's performance"""
        # Custom reward logic
        return 0.1  # Small exploration bonus
    
    def check_termination(self):
        """Check if episode should end"""
        return False  # Continue until max_steps
    
    def get_episode_stats(self):
        """Get episode statistics"""
        return {
            'firing_rate': self.brain.get_average_firing_rate(),
            'curiosity': self.agent.get_curiosity_level(),
            'novelty': self.agent.get_novelty_level(),
            'development_stage': self.agent.get_developmental_stage()
        }

# Usage
config = nlm.ConfigPresets.createDevelopment()
brain = nlm.Brain(config)
brain.initialize()

world = nlm.create_simple_world()
world.configure(width=20, height=20)

agent = AdvancedAgent(brain, world)
episode_stats = agent.run_episode(500)
```

### Debugging and Monitoring

```python
import nlm
import time

# Create brain with debugging enabled
config = nlm.ConfigPresets.createDevelopment()
brain = nlm.Brain(config)
brain.initialize()

# Create agent and world for debugging
agent = nlm.create_agent_brain(brain)
world = nlm.create_simple_world()

# Monitoring loop
for step in range(100):
    # Track performance
    step_start = time.time()
    
    # Run simulation
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decode_motor_command()
    world.applyMotorCommand(action, world.get_simulation_time())
    
    # Measure performance
    step_time = time.time() - step_start
    
    # Check for issues
    firing_rate = brain.get_average_firing_rate()
    if firing_rate > 100:  # Suspiciously high firing rate
        print(f"Warning: High firing rate at step {step}: {firing_rate}")
    
    if step % 10 == 0:
        print(f"Step {step}: {1000/step_time:.1f} Hz, "
              f"firing: {firing_rate:.1f}")
```

## Best Practices

### 1. Use Context Managers

```python
# Always use context managers for safe resource management
with brain.context() as ctx:
    ctx.step(1)
    action = ctx.produce_action()
```

### 2. Initialize Before Use

```python
brain = nlm.Brain(config)
brain.initialize()  # Required before first step
```

### 3. Handle Errors Gracefully

```python
try:
    brain.step(1)
except nlm.RuntimeError as e:
    print(f"Error: {e}")
    brain.reset()
```

### 4. Monitor Performance

```python
# Periodically check performance metrics
if brain.get_average_firing_rate() > 50:
    print("High firing rate detected - consider adjusting configuration")
```

### 5. Use Appropriate Presets

```python
# Choose preset based on use case
if learning_needed:
    config = nlm.ConfigPresets.createDevelopment()
elif stability_needed:
    config = nlm.ConfigPresets.createProduction()
else:
    config = nlm.ConfigPresets.createExploration()
```

## Migration Guide

### From Version 0.x to 1.x

1. **Configuration Changes**
   - Old: `config.set("neurons", 1000)`
   - New: `config.set("brain.neuron_count", 1000)`

2. **Class Names**
   - Old: `nlm.Brain` → New: `nlm.Brain` (unchanged)
   - Old: `nlm.Agent` → New: `nlm.create_agent_brain(brain)`

3. **Method Names**
   - Old: `brain.initializeBrain()` → New: `brain.initialize()`
   - Old: `agent.processInput()` → New: `agent.processSensoryInput()`

4. **New Features**
   - Context managers added for safer operation
   - Progress tracking for long-running operations
   - Enhanced error handling with custom exceptions

## Summary

The NLM API provides comprehensive access to the neural simulation framework through:

- **Intuitive, Pythonic interfaces** with context managers and helper functions
- **Comprehensive configuration system** with presets and type conversion
- **Advanced monitoring and debugging tools** for production use
- **Flexible error handling** with custom exception types
- **Performance optimization** utilities for large-scale simulations

This API reference should provide everything needed to work effectively with NLM, from basic examples to advanced implementations.
