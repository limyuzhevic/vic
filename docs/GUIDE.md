# NLM Documentation - IMPROVED

## Overview

NLM (Neural Learning Machine) is an experimental computational brain simulation framework that implements a modular, biologically-inspired neural architecture with spiking neurons, plastic synapses, memory systems, and adaptive behavior.

## Quick Start

### Installation

```bash
# Install Python dependencies
pip install pybind11 scikit-build-core numpy

# Build and install NLM
pip install .

# Verify installation
python -c "import pynlm; brain = pynlm.createDefaultBrain(); brain.initialize(); print('NLM working!')"
```

### Basic Usage Example

```python
import pynlm

# Create and initialize brain
brain = pynlm.createDefaultBrain()
brain.initialize()

# Simulate for 100 steps
for step in range(100):
    brain.step(step)

# Check statistics
stats = brain.getBrainStats()
print(f"Neurons: {stats['neuron_count']}")
print(f"Active neurons: {stats['firing_neurons']}")
print(f"Total spikes: {stats['total_spikes']}")
```

### Advanced Usage Example

```python
import pynlm

# Create complete training environment
brain, world, agent = pynlm.createTrainingEnvironment()

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Custom simulation loop
for step in range(1000):
    world.update(0.1)
    
    # Sense environment
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Think and act
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learn from experience
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, agent.getPredictionError())

print("Training complete!")
```

## User Guide

### 1. Creating and Configuring Brains

#### Basic Configuration
```python
import pynlm

# Create with sensible defaults
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
```

#### Custom Configuration
```python
# Custom brain configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("brain.learning_rate", 0.005)

brain = pynlm.createBrain(config)
```

#### Validation
```python
# Validate configuration
errors = config.validate()
if errors:
    print(f"Configuration errors: {errors}")
    # Fix errors or use defaults
```

### 2. Simulation Control

#### Basic Simulation Loop
```python
brain.initialize()
for step in range(1000):
    brain.step(step)
    # Access brain state
    print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Timed Simulation
```python
for step in range(500):
    brain.step(step, time=0.1 * step)
    world.update(0.1)
    # Your simulation logic here
```

#### Advanced Simulation Patterns
```python
# Simulate until resting
steps_to_rest = brain.simulate_until_resting(10000)
print(f"Brain reached resting state after {steps_to_rest} steps")

# Simulate for duration
steps = brain.simulate_for_duration(duration=10.0, timestep=0.001)
print(f"Simulated for {duration} seconds ({steps} steps)")

# Wait for activity
steps_needed = brain.wait_for_activity(target_spikes=10000, max_steps=50000)
print(f"Reached target activity after {steps_needed} steps")
```

### 3. World Interaction

#### World Setup
```python
world = pynlm.createSimpleWorld()
world.configure(
    width=30,
    height=30,
    visionWidth=10,
    visionHeight=10
)
world.reset()
```

#### Agent Setup
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
```

#### Complete Control Loop
```python
# Complete agent control loop
def run_episode(world, agent, num_steps):
    world.reset()
    
    for step in range(num_steps):
        # World physics
        world.update(0.1)
        
        # Perception
        percept = world.getSensoryPercept()
        
        # Cognition
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learning
        if agent.isRewardModulationEnabled():
            reward = get_environment_reward()
            agent.applyRewardModulation(reward, agent.getPredictionError())
        
        # Development
        if agent.isDevelopmentEnabled():
            agent.updateDevelopment(0.1)
```

### 4. Working with Memory and Cognition

#### Memory Systems
```python
# Working memory
working_memory = brain.getWorkingMemory()
working_memory.store(pattern, priority)
pattern = working_memory.retrieve(query)

# Episodic memory
episodic_memory = brain.getEpisodicMemory()
episodic_memory.record(experience)
replay = episodic_memory.replay()

# Concept formation
concept_formation = brain.getConceptFormation()
concept = concept_formation.learn(pattern)
```

#### Prediction System
```python
# Prediction system
prediction = brain.getPredictionSystem()
prediction.update(expected_state, actual_state)
error = prediction.getPredictionError()
```

### 5. Advanced Features

#### Neuromodulation
```python
# Control neuromodulators
agent.enableCuriosity(True)
agent.enableNoveltyDetection(True)

curiosity = agent.getCuriosityLevel()
novelty = agent.getNoveltyLevel()
prediction_error = agent.getPredictionError()
neuromodulation = agent.getNeuromodulationLevel()
```

#### Development Stages
```python
# Monitor development
for step in range(1000):
    brain.step(step)
    stage = brain.getDevelopmentalStage()
    
    if stage == pynlm.DevelopmentalStage.CriticalPeriod:
        # Enhanced learning during critical period
        pass
    elif stage == pynlm.DevelopmentalStage.Aging:
        # Age-related changes
        pass
```

#### Performance Monitoring
```python
# Brain health check
health = brain.check_brain_health()
print(f"Brain healthy: {health['healthy']}")
print(f"Total neurons: {health['total_neurons']}")

# Performance benchmarking
bench_results = pynlm.benchmark_brain(brain, steps=1000, duration=0.1)
print(f"Spikes per second: {bench_results['spikes_per_second']:.2f}")
```

### 6. Development and Research

#### Reproducible Experiments
```python
# Create reproducible simulations
brain, world = pynlm.create_reproducible_simulation(seed=42, steps=1000)

# Run experiments multiple times
results = []
for run in range(3):
    brain.reset()
    world.reset()
    # Your experiment logic
    results.append(brain.getTotalSpikeCount())

print(f"Results: {results}")  # Should be consistent
```

#### Experiment Management
```python
# Create experiment
description = "Testing curiosity-driven exploration"
experiment = pynlm.createExperiment(description)

# Add conditions
condition1 = experiment.addCondition("no_curiosity", curiosity_enabled=False)
condition2 = experiment.addCondition("with_curiosity", curiosity_enabled=True)

# Run experiment
results = experiment.run()
experiment.save_results("experiment_results.json")
```

### 7. File I/O and Persistence

#### Saving and Loading
```python
# Save brain state
brain.save("my_brain.checkpoint")

# Load brain state
brain.load("my_brain.checkpoint")

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config.loadFromFile("my_config.json")
```

#### Visualization
```python
# Export for visualization
viz_data = pynlm.export_brain_for_visualization(brain)

# Create snapshot
brain.create_snapshot("brain_snapshot.bin")
```

## API Reference

### Core Classes

#### Config
- `createDefaultConfig()` - Create default configuration
- `validate()` - Validate configuration
- `set(key, value)` - Set configuration value
- `get(key, default=None)` - Get configuration value
- `loadFromFile(path)` - Load from JSON file
- `saveToFile(path)` - Save to JSON file

#### Brain
- `createBrain(config)` - Create brain
- `createDefaultBrain()` - Create with defaults
- `initialize()` - Initialize brain
- `step(step, time=0.0)` - Run simulation step
- `reset()` - Reset state
- `getBrainStats()` - Get statistics as dictionary
- `check_brain_health()` - Check brain health
- `save(path)` - Save brain state
- `load(path)` - Load brain state

#### AgentBrain
- `createAgentBrain(brain)` - Create agent interface
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Get motor action
- `applyRewardModulation(reward, predicted)` - Apply learning
- `enableRewardModulation(enabled)` - Control reward learning
- `enableCuriosity(enabled)` - Control exploration
- `enableDevelopment(enabled)` - Control development
- `trainEpisode(world, max_steps, verbose)` - Run training episode

#### SimpleWorld
- `createSimpleWorld()` - Create world
- `configure(width, height, visionWidth, visionHeight)` - Set dimensions
- `reset()` - Reset world
- `update(timestep)` - Update world
- `applyMotorCommand(action, time)` - Apply action
- `getSensoryPercept()` - Get percept
- `getAgentBody()` - Get agent state
- `setRandomSeed(seed)` - Set random seed

### Utility Functions

- `format_duration(seconds)` - Format duration
- `format_spike_count(count)` - Format spike count
- `benchmark_brain(brain, steps, duration)` - Benchmark performance
- `simulate_until_stable(brain, threshold, max_steps)` - Wait for stability
- `create_reproducible_simulation(seed, steps)` - Create reproducible sim
- `safe_step(brain, step, time)` - Safe simulation step

### Enhanced Python Features

The improved bindings provide:

#### Context Managers
```python
with world:
    world.update(0.1)
    # World auto-resets after context
```

#### Properties
```python
config.neuron_count = 1000  # Property-like access
config.synapse_count = 50000
```

#### Factory Methods
```python
# Easy creation methods
brain = pynlm.createBrain()  # With defaults
brain = pynlm.createDefaultBrain()
world, brain, agent = pynlm.createTrainingEnvironment()
```

#### Error Handling
```python
try:
    brain.step(step)
except pynlm.RuntimeError as e:
    print(f"Simulation error: {e}")
```

## Examples Directory

For practical examples and templates, see the `examples/` directory:

### Basic Examples
- `minimal.py` - Minimal working example
- `agent_basic.py` - Basic agent implementation
- `world_basic.py` - World-only simulation

### Advanced Examples
- `training.py` - Complete training loop
- `research.py` - Research experiment template
- `visualization.py` - Visualization data export

### Complex Examples
- `multi_agent.py` - Multi-agent system
- `environment.py` - Custom environment
- `memory.py` - Memory system exploration

## Getting Help

### Documentation
- This guide: `docs/GUIDE.md`
- API Reference: `docs/API.md`
- Architecture: `docs/ARCHITECTURE.md`
- Tutorials: `docs/TUTORIALS.md`

### Community
- GitHub Discussions: [link]
- Stack Overflow: [tag]
- Discord: [invite]

### Examples and Templates
- Check the `examples/` directory
- Review `docs/examples.md` for detailed examples
- Look at `docs/patterns.md` for common patterns

## Next Steps

### For Beginners
1. Start with `examples/minimal.py`
2. Try `examples/agent_basic.py`
3. Read `docs/TUTORIALS.md`

### For Researchers
1. Explore `examples/research.py`
2. Use `docs/experiments.md`
3. Check `examples/memory.py`

### For Developers
1. Look at `docs/API.md` for detailed API
2. Study `examples/visualization.py`
3. Review `docs/architecture.md`