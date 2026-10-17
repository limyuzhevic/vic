# Python Bindings Documentation

## Overview

This document provides comprehensive documentation for the NLM Python bindings. It includes:

- **Module structure**: Overview of the Python bindings package
- **Core functionality**: Documentation for major classes and functions
- **Usage examples**: Step-by-step examples for common tasks
- **Advanced features**: Documentation for advanced users
- **Error handling**: Complete error reference
- **Performance considerations**: Tips for optimizing simulations
- **Migration guide**: Guidance for migrating from C++ API

## Package Structure

### Directory Layout

```
python/bindings/
├── __init__.py              # Main package initialization
├── types.py                 # Type definitions and enumerations
├── config.py                # Configuration system
├── neural.py                # Neural components
├── brain.py                 # Brain system integration
├── agent.py                 # Agent interface
├── world.py                 # Environment and world simulation
├── memory.py                # Memory systems
├── neuromodulation.py        # Neuromodulation systems
├── cognition.py             # Cognitive systems
├── prediction.py            # Prediction systems
├── development.py           # Development systems
├── plasticity.py            # Plasticity systems
├── exceptions.py            # Custom exception classes
├── utils.py                 # Utility functions
├── examples/               # Example scripts
│   ├── basic_simulation.py
│   ├── advanced_learning.py
│   ├── multi_agent.py
│   ├── performance_benchmark.py
│   └── visualization.py
├── tests/                   # Python test suite
│   ├── test_basic.py
│   ├── test_neural.py
│   ├── test_memory.py
│   ├── test_plasticity.py
│   └── test_integration.py
├── README.md                # Package documentation
└── pyproject.toml          # Build configuration
```

### Module Dependencies

```python
# Import core modules
from nlm import (
    Config, Brain, AgentBrain, SimpleWorld,
    Neuron, Synapse, NeuralRegion, NeuralPopulation,
    create_default_config, create_brain, create_simple_world
)

# Import type definitions
from nlm.types import (
    NeuronType, SynapseType, DevelopmentalStage,
    FiringState, ActionType, NeuronId, SynapseId,
    RegionId, PopulationId
)

# Import memory systems
from nlm.memory import WorkingMemory, EpisodicMemory, AssociativeMemory

# Import neuromodulation systems
from nlm.neuromodulation import Dopamine, Curiosity, Novelty, PredictionError

# Import cognition systems
from nlm.cognition import NeuralPlanner, ConceptFormation, AttentionalSelection

# Import prediction systems
from nlm.prediction import PredictionSystem

# Import development systems
from nlm.development import DevelopmentSystem

# Import plasticity systems
from nlm.plasticity import STDP, Hebbian, StructuralPlasticity
```

## Core Functionality

### Configuration System

#### Config Class

The Config class manages configuration parameters for NLM simulations.

**Key Features:**
- **Flexible configuration**: Support for various parameter types (int, float, string, bool, lists, dictionaries)
- **Validation**: Automatic validation of configuration values
- **Persistence**: Save and load configuration from files
- **Defaults**: Sensible default values for most parameters

**Common Configuration Parameters:**

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | int | 1000 | Number of neurons in the brain |
| `region_count` | int | 1 | Number of brain regions |
| `connection_probability` | float | 0.1 | Probability of creating connections |
| `simulation_timestep` | float | 0.001 | Time step for simulation (seconds) |
| `stdp_ltp_weight` | float | 0.01 | STDP Long-Term Potentiation weight |
| `stdp_ltd_weight` | float | 0.012 | STDP Long-Term Depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant |
| `synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `pruning_rate` | float | 0.00001 | Rate of synapse elimination |
| `working_memory_capacity` | int | 100 | Capacity of working memory |
| `episodic_memory_max_episodes` | int | 1000 | Maximum number of episodes |
| `reward_modulation` | bool | true | Enable reward modulation |
| `curiosity_enabled` | bool | true | Enable curiosity system |
| `prediction_system` | bool | true | Enable prediction system |

**Example Usage:**

```python
import nlm

# Create default configuration
config = nlm.create_default_config()

# Set configuration values
config.set("neuron_count", 2000)
config.set("region_count", 5)
config.set("connection_probability", 0.15)
config.set("simulation_timestep", 0.0005)
config.set("stdp_ltp_weight", 0.02)

# Check configuration has a key
has_neuron_count = config.has("neuron_count")

# Get all configuration keys
keys = config.get_keys()

# Save configuration to file
config.save_to_file("my_config.json")

# Load configuration from file
new_config = nlm.Config()
new_config.load_from_file("my_config.json")

# Get configuration summary
summary = config.summary()
print(summary)
```

#### Configuration Validation

The configuration system validates parameter values and provides helpful error messages.

```python
try:
    config = nlm.Config()
    config.set("neuron_count", -100)  # Invalid: negative neurons
    config.set("region_count", 0)     # Invalid: no regions
    config.set("connection_probability", 1.5)  # Invalid: > 1.0
    
    brain = nlm.create_brain(config)
    brain.initialize()
    
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
    # Output: "Configuration error: Invalid neuron_count value: -100"
```

### Brain Class

#### Basic Brain Operations

```python
import nlm

# Create brain with default configuration
brain = nlm.create_brain()

# Initialize brain
brain.initialize()

# Get brain statistics
print(f"Total neurons: {brain.get_total_neuron_count()}")
print(f"Total synapses: {brain.get_total_synapse_count()}")
print(f"Active neurons: {brain.get_active_neuron_count()}")
print(f"Firing neurons: {brain.get_firing_neuron_count()}")
print(f"Total spikes: {brain.get_total_spike_count()}")
print(f"Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
print(f"E/I ratio: {brain.get_excitation_inhibition_ratio():.3f}")

# Get developmental stage
stage = brain.get_developmental_stage()
print(f"Developmental stage: {stage}")

# Set developmental stage
brain.set_developmental_stage(nlm.DevelopmentalStage.CriticalPeriod)

# Log brain status
brain.log_status()
```

#### Simulation Steps

```python
import nlm

# Create brain
brain = nlm.create_brain()
brain.initialize()

# Run simulation steps for specific number of steps
for step in range(100):
    brain.step(step)

# Run simulation with timestamps
for step in range(100):
    time = step * 0.001  # 1ms timesteps
    brain.step(step, time)
```

#### Sensory Input and Action

```python
import nlm

# Create brain and world
brain = nlm.create_brain()
world = nlm.create_simple_world()
brain.initialize()
world.configure(100, 100, 20, 20)
world.set_agent_start(50, 50)

# Simulation loop
for step in range(1000):
    # Get sensory input
    percept = world.get_sensory_percept()
    brain.receive_sensory_input(percept)
    
    # Update world
    world.update(0.001)
    
    # Get action
    action = brain.produce_action()
    
    # Apply action to world
    world.apply_motor_command(action, step * 0.001)
    
    # Print progress occasionally
    if step % 100 == 0:
        body = world.get_agent_body()
        print(f"Step {step}: Position ({body.x:.1f}, {body.y:.1f})")
```

#### Memory Systems

```python
import nlm

# Create brain with memory systems
brain = nlm.create_brain()
brain.initialize()

# Configure memory systems
brain.get_working_memory().set_capacity(100)
brain.get_episodic_memory().set_max_episodes(1000)

# Store experience in working memory
for neuron_id in [1, 2, 3, 4, 5]:
    activation = brain.get_neuron(neuron_id).get_activation()
    brain.get_working_memory().store_to_neuron(neuron_id, activation)

# Get working memory traces
traces = brain.get_working_memory().get_active_traces()
print(f"Working memory traces: {traces}")

# Store episode in episodic memory
episode = nlm.EpisodicMemoryItem()
episode.timestamp = 100
episode.reward = 1.0
episode.active_neurons = [1, 2, 3]
episode.neuron_activations = [0.5, 0.8, 0.3]

brain.get_episodic_memory().store_episode(episode)

# Get recent episodes
recent_episodes = brain.get_episodic_memory().get_recent_episodes(5)
print(f"Recent episodes: {len(recent_episodes)}")

# Replay episodes for consolidation
brain.get_episodic_memory().replay_episode(recent_episodes[0])
```

### AgentBrain Class

#### Agent Interface

```python
import nlm

# Create brain and agent brain
brain = nlm.create_brain()
brain.initialize()
world = nlm.create_simple_world()

agent_brain = nlm.create_agent_brain(brain)
agent_brain.initialize(world)

# Enable learning systems
agent_brain.enable_reward_modulation(True)
agent_brain.enable_structural_plasticity(True)
agent_brain.enable_development(True)
agent_brain.enable_curiosity(True)

# Process sensory input
for step in range(1000):
    percept = world.get_sensory_percept()
    agent_brain.process_sensory_input(percept)
    
    world.update(0.001)
    
    action = agent_brain.decode_motor_command()
    world.apply_motor_command(action, step * 0.001)
    
    # Apply reward modulation
    if step % 100 == 0:
        reward = 1.0 if step % 200 == 0 else 0.0
        agent_brain.apply_reward_modulation(reward, 0.5)
        
    # Update development
    agent_brain.update_development(0.001)
    
    # Print neuromodulation levels
    if step % 100 == 0:
        neuromod = agent_brain.get_neuromodulation_level()
        curiosity = agent_brain.get_curiosity_level()
        novelty = agent_brain.get_novelty_level()
        pred_error = agent_brain.get_prediction_error()
        
        print(f"Neuromodulation: Dopamine={neuromod:.3f}, "
              f"Curiosity={curiosity:.3f}, Novelty={novelty:.3f}, "
              f"PredError={pred_error:.3f}")
```

### SimpleWorld Class

#### World Simulation

```python
import nlm

# Create and configure world
world = nlm.create_simple_world()
world.configure(200, 200, 30, 30)  # width, height, vision_width, vision_height
world.set_agent_start(100, 100)    # Starting position
world.set_max_energy(100.0)         # Maximum energy
world.set_energy_decay_rate(0.01)   # Energy decay rate

# Add world objects
world.add_object(nlm.WorldObject(50, 50, nlm.WorldObjectType.Resource, 10.0, 1.0))
world.add_object(nlm.WorldObject(150, 150, nlm.WorldObjectType.Hazard, 5.0, 1.0))

# Simulation loop
for step in range(500):
    # Get state
    body = world.get_agent_body()
    percept = world.get_sensory_percept()
    
    print(f"Step {step}: Position ({body.x:.1f}, {body.y:.1f}), "
          f"Energy {body.energy:.1f}, Health {body.health:.1f}")
    
    # Update world
    world.update(0.001)
    
    # Create and apply action
    action = nlm.Action(nlm.ActionType.MoveForward)
    world.apply_motor_command(action, step * 0.001)
    
    # Check for agent interactions
    agent_body = world.get_agent_body()
    if agent_body.isMoving:
        print(f"  Agent is moving with velocity ({agent_body.velocityX:.2f}, {agent_body.velocityY:.2f})")
```

## Advanced Features

### Memory and Learning

#### Working Memory

```python
import nlm

# Create brain with working memory
brain = nlm.create_brain()
brain.initialize()

# Set up working memory
wm = brain.get_working_memory()
wm.set_capacity(50)

# Store and retrieve traces
wm.store_to_neuron(neuron_id, activation_strength)
traces = wm.get_active_traces()

# Get memory statistics
print(f"Working memory capacity: {wm.get_capacity()}")
print(f"Working memory traces: {wm.get_active_traces()}")
```

#### Episodic Memory

```python
import nlm

# Create brain with episodic memory
brain = nlm.create_brain()
brain.initialize()

# Configure episodic memory
em = brain.get_episodic_memory()
em.set_max_episodes(1000)

# Store episodes
for step in range(0, 1000, 10):
    episode = nlm.EpisodicMemoryItem()
    episode.timestamp = step
    episode.reward = 0.5 if step % 20 == 0 else 0.0
    episode.active_neurons = [1, 2, 3, 4, 5]
    episode.neuron_activations = [0.1, 0.5, 0.8, 0.3, 0.6]
    
    em.store_episode(episode)

# Get episodes for replay
replay_episodes = em.get_episodes_for_replay(3)
for episode in replay_episodes:
    print(f"Episode at step {episode.timestamp}: "
          f"Reward={episode.reward}, "
          f"Neurons={len(episode.active_neurons)}")
```

#### Associative Memory

```python
import nlm

# Create brain with associative memory
brain = nlm.create_brain()
brain.initialize()

# Configure associative memory
am = brain.get_associative_memory()
am.set_learning_rate(0.01)

# Store associations
am.store_association("reward_prediction", [1, 2, 3], [0.8, 0.9, 0.7])
am.store_association("sensory_pattern", [4, 5, 6], [0.3, 0.4, 0.5])

# Retrieve associations
reward_pattern = am.retrieve_association("reward_prediction")
print(f"Reward pattern: {reward_pattern}")

# Get memory statistics
stats = am.get_statistics()
print(f"Associative memory entries: {stats['entry_count']}")
print(f"Learning rate: {stats['learning_rate']}")
```

### Neuromodulation Systems

#### Dopamine System

```python
import nlm

# Create brain with neuromodulation
brain = nlm.create_brain()
brain.initialize()

# Get neuromodulation systems
dopamine = brain.get_dopamine()
curiosity = brain.get_curiosity()
novelty = brain.get_novelty()
prediction_error = brain.get_prediction_error()

# Get current levels
print(f"Dopamine level: {dopamine.get_level():.3f}")
print(f"Curiosity level: {curiosity.get_level():.3f}")
print(f"Novelty level: {novelty.get_level():.3f}")
print(f"Prediction error: {prediction_error.get_level():.3f}")

# Update neuromodulation
for step in range(100):
    # Apply reward prediction error
    prediction_error.set_level(0.5 if step % 10 == 0 else 0.1)
    
    # Apply curiosity
    curiosity.set_level(0.3 + 0.2 * (step % 5) / 5.0)
    
    # Apply novelty
    novelty.set_level(0.1 if step % 7 == 0 else 0.0)
    
    # Apply dopamine
    dopamine_level = prediction_error.get_level() * 0.5
    dopamine.set_level(dopamine_level)
    
    if step % 10 == 0:
        print(f"Step {step}: Dopamine={dopamine.get_level():.3f}, "
              f"Curiosity={curiosity.get_level():.3f}")
```

#### Curiosity System

```python
import nlm

# Create brain with curiosity system
brain = nlm.create_brain()
brain.initialize()

# Get curiosity system
curiosity = brain.get_curiosity()

# Set curiosity parameters
curiosity.set_target_exploration(0.5)
curiosity.set_exploration_decay(0.01)
curiosity.set_surprise_threshold(0.8)

# Update curiosity with prediction error
for step in range(100):
    # Simulate prediction error
    prediction_error = step % 20 == 0  # Every 20 steps
    
    curiosity.update(step * 0.001, prediction_error)
    
    if step % 10 == 0:
        curiosity_level = curiosity.get_level()
        print(f"Step {step}: Curiosity level = {curiosity_level:.3f}")
```

### Development System

#### Developmental Stages

```python
import nlm

# Create brain with development system
brain = nlm.create_brain()
brain.initialize()

# Get development system
development = brain.get_development_system()

# Check current developmental stage
stage = brain.get_developmental_stage()
print(f"Initial developmental stage: {stage}")

# Set developmental stage
brain.set_developal_stage(nlm.DevelopmentalStage.CriticalPeriod)

# Update development over time
for step in range(5000):  # Simulate 5 seconds at 1kHz
    brain.update_development(0.001)
    
    # Print stage changes
    new_stage = brain.get_developmental_stage()
    if new_stage != stage:
        print(f"Stage changed from {stage} to {new_stage} at step {step}")
        stage = new_stage
    
    if step % 1000 == 0:
        print(f"Step {step}: Developmental stage = {stage}")
```

#### Structural Plasticity

```python
import nlm

# Create brain with structural plasticity
brain = nlm.create_brain()
brain.initialize()

# Enable structural plasticity
agent_brain = nlm.create_agent_brain(brain)
agent_brain.enable_structural_plasticity(True)

# Get structural plasticity system
structural = brain.get_structural_plasticity()

# Configure structural plasticity
structural.set_synaptogenesis_rate(0.0002)
structural.set_pruning_rate(0.00002)

# Update structural plasticity
for step in range(1000):
    if step % 100 == 0:  # Every 100 steps
        structural.update(brain, brain.get_random_generator())
        
        # Print statistics
        stats = structural.get_statistics()
        print(f"Step {step}: Synapses = {stats['synapse_count']}, "
              f"Synaptogenesis = {stats['synaptogenesis_events']}, "
              f"Pruning = {stats['pruning_events']}")
```

### Cognitive Systems

#### Neural Planner

```python
import nlm

# Create brain with cognitive systems
brain = nlm.create_brain()
brain.initialize()

# Get cognitive systems
planner = brain.get_planner()
concept_formation = brain.get_concept_formation()
attention = brain.get_attention()

# Configure planner
planner.set_planning_depth(5)
planner.set_action_selection_threshold(0.3)

# Get planning statistics
stats = planner.get_statistics()
print(f"Planner depth: {stats['planning_depth']}")
print(f"Action selection threshold: {stats['action_selection_threshold']}")
```

#### Concept Formation

```python
import nlm

# Create brain with concept formation
brain = nlm.create_brain()
brain.initialize()

# Get concept formation system
concept = brain.get_concept_formation()

# Configure concept formation
concept.set_concept_threshold(0.7)
concept.set_concept_decay(0.01)
concept.set_max_concepts(100)

# Update concept formation with neural activity
for step in range(100):
    # Simulate neural activity pattern
    neural_pattern = [0.1, 0.3, 0.7, 0.5, 0.2]
    
    # Update concept formation
    concepts = concept.update(neural_pattern)
    
    if step % 10 == 0:
        print(f"Step {step}: Found {len(concepts)} concepts")
```

#### Attentional Selection

```python
import nlm

# Create brain with attentional selection
brain = nlm.create_brain()
brain.initialize()

# Get attentional selection system
attention = brain.get_attention()

# Configure attentional selection
attention.set_inhibition_strength(0.5)
attention.set_excitation_strength(1.5)
attention.set_attention_threshold(0.3)

# Update attentional selection
for step in range(100):
    # Get competing neurons
    competitors = [1, 2, 3, 4, 5]
    
    # Update attentional selection
    winner = attention.process_competition(competitors)
    
    if step % 10 == 0:
        print(f"Step {step}: Attention winner = {winner}")
```

### Prediction System

#### Prediction Error

```python
import nlm

# Create brain with prediction system
brain = nlm.create_brain()
brain.initialize()

# Get prediction system
prediction = brain.get_prediction_system()

# Configure prediction system
prediction.set_prediction_threshold(0.8)
prediction.set_error_threshold(0.5)

# Update prediction system
for step in range(100):
    # Simulate sensory input
    sensory_input = [0.1, 0.3, 0.7, 0.5, 0.2]
    
    # Get prediction
    predicted_output = prediction.predict(sensory_input)
    
    # Calculate prediction error
    actual_output = [x * 1.1 for x in sensory_input]  # Simulated actual
    error = prediction.calculate_error(predicted_output, actual_output)
    
    # Update prediction system
    prediction.update(error)
    
    if step % 10 == 0:
        print(f"Step {step}: Prediction error = {error:.3f}")
```

## Advanced Features

### Multi-Agent Simulation

```python
import nlm

# Create multiple agents
agents = []
for i in range(3):
    brain = nlm.create_brain()
    brain.initialize()
    world = nlm.create_simple_world()
    world.configure(100, 100, 20, 20)
    world.set_agent_start(i * 30, i * 30)
    
    agent_brain = nlm.create_agent_brain(brain)
    agent_brain.initialize(world)
    
    agents.append((brain, world, agent_brain))

# Run multi-agent simulation
for step in range(500):
    for brain, world, agent_brain in agents:
        # Process sensory input
        percept = world.get_sensory_percept()
        agent_brain.process_sensory_input(percept)
        
        # Update world
        world.update(0.001)
        
        # Get action
        action = agent_brain.decode_motor_command()
        world.apply_motor_command(action, step * 0.001)
    
    # Check for agent interactions
    for i, (brain1, world1, _) in enumerate(agents):
        for j, (brain2, world2, _) in enumerate(agents):
            if i < j:
                body1 = world1.get_agent_body()
                body2 = world2.get_agent_body()
                
                distance = ((body1.x - body2.x) ** 2 + (body1.y - body2.y) ** 2) ** 0.5
                
                if distance < 10:  # Close proximity
                    print(f"Step {step}: Agents {i} and {j} are {distance:.1f} units apart")
```

### Performance Optimization

```python
import nlm
import time

# Create optimized brain for large simulations
brain = (nlm.create_brain()
         .with_neuron_count(10000)
         .with_development()
         .with_structural_plasticity())

brain.initialize()

# Target high-performance simulation
update_interval = 0.0005  # 2kHz

start_time = time.time()
step_count = 0
max_steps = 10000

while step_count < max_steps:
    loop_start = time.time()
    
    # Simulation steps...
    for _ in range(10):  # Process 10 steps at once
        brain.step(step_count)
        step_count += 1
    
    # Calculate sleep time
    elapsed = time.time() - loop_start
    sleep_time = max(0, update_interval * 10 - elapsed)
    
    if sleep_time > 0:
        time.sleep(sleep_time)

end_time = time.time()
elapsed = end_time - start_time

print(f"Completed {step_count} steps in {elapsed:.2f} seconds")
print(f"Performance: {step_count / elapsed:.0f} steps/second")
```

### Visualization

```python
import nlm
import matplotlib.pyplot as plt
import numpy as np

# Create simulation for visualization
brain = nlm.create_brain()
brain.initialize()
world = nlm.create_simple_world()
world.configure(100, 100, 20, 20)
world.set_agent_start(50, 50)

# Store trajectory
trajectory = []

# Run simulation
for step in range(1000):
    # Get state
    body = world.get_agent_body()
    trajectory.append((body.x, body.y))
    
    # Simulate some movement
    if step % 5 == 0:
        action = nlm.Action(nlm.ActionType.MoveForward)
        world.apply_motor_command(action, step * 0.001)
    
    # Update world
    world.update(0.001)

# Plot trajectory
trajectory = np.array(trajectory)
plt.figure(figsize=(10, 8))
plt.plot(trajectory[:, 0], trajectory[:, 1], 'b-', alpha=0.7, linewidth=2)
plt.scatter(trajectory[0, 0], trajectory[0, 1], color='green', s=100, label='Start')
plt.scatter(trajectory[-1, 0], trajectory[-1, 1], color='red', s=100, label='End')
plt.xlabel('X Position')
plt.ylabel('Y Position')
plt.title('Agent Trajectory')
plt.legend()
plt.grid(True, alpha=0.3)
plt.show()
```

## Error Handling

### Custom Exceptions

```python
import nlm

# Define custom exception hierarchy
class NLMError(Exception):
    """Base exception for all NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Error in configuration or setup."""
    pass

class NeuronError(NLMError):
    """Error related to neuron operations."""
    pass

class SynapseError(NLMError):
    """Error related to synapse operations."""
    pass

class SimulationError(NLMError):
    """Error during simulation."""
    pass

# Example error handling
try:
    # Create brain with invalid configuration
    config = nlm.Config()
    config.set("neuron_count", -100)
    config.set("connection_probability", 1.5)
    
    brain = nlm.create_brain(config)
    brain.initialize()
    
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
    # Output: "Configuration error: Invalid neuron_count value: -100"
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
except nlm.NeuronError as e:
    print(f"Neuron error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Common Error Conditions

| Exception Type | Condition | Message |
|---------------|-----------|---------|
| `ConfigurationError` | Invalid configuration values | "Invalid {parameter} value: {value}" |
| `NeuronError` | Neuron operation error | "Neuron error: {message}" |
| `SynapseError` | Synapse operation error | "Synapse error: {message}" |
| `SimulationError` | Simulation error | "Simulation error: {message}" |

## Migration Guide

### From C++ API to Python

If migrating from the C++ API to Python, note these key differences:

1. **Memory Management**: Python handles memory automatically
2. **Error Handling**: Use try-catch with NLM exceptions
3. **Type Hints**: Use type annotations for better IDE support
4. **Documentation**: Access docstrings with help()

### Performance Considerations

1. **NumPy Integration**: Use NumPy for numerical operations
2. **Batch Processing**: Use batch operations for efficiency
3. **Caching**: Cache frequently accessed data
4. **Profiling**: Use Python profiling tools

### Common C++ to Python Patterns

#### C++
```cpp
Brain brain(config);
brain.initialize();
for (int i = 0; i < 100; ++i) {
    brain.step(i);
}
```

#### Python
```python
brain = nlm.create_brain(config)
brain.initialize()
for i in range(100):
    brain.step(i)
```

#### C++
```cpp
Neuron neuron(NeuronId(1));
neuron.setType(NeuronType::Excitatory);
neuron.setMembranePotential(-65.0f);
```

#### Python
```python
neuron = nlm.Neuron(nlm.create_neuron_id(1))
neuron.set_type(nlm.NeuronType.Excitatory)
neuron.set_membrane_potential(-65.0)
```

## Conclusion

The NLM Python bindings provide a comprehensive, Pythonic interface to the neural simulation framework. With its modular design, extensive documentation, and robust error handling, it's ideal for both beginners and advanced users.

Key features:
- **Ease of Use**: Simple API with comprehensive documentation
- **Power**: Full access to all NLM features and capabilities
- **Performance**: Optimized for both small and large simulations
- **Flexibility**: Extensible design for custom applications
- **Reliability**: Robust error handling and comprehensive testing

Start exploring the power of neural simulation with the NLM Python bindings today!