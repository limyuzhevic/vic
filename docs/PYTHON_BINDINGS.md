# NLM Python Bindings - Comprehensive Documentation
# Version 0.1.0

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Python bindings, which expose the powerful C++ neural simulation framework to Python users.

## Overview

NLM Python bindings provide a high-level interface to a biologically inspired spiking neural network simulator that implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, multiple plasticity mechanisms (STDP, Hebbian), and integrated memory, prediction, cognition, and neuromodulation systems.

### Key Features

- **Real-time neural simulation** with biologically plausible neuron models
- **Event-driven architecture** for efficient computation
- **Multiple memory systems** (working, episodic, semantic, procedural)
- **Neuromodulation** (dopamine, curiosity, novelty, prediction error)
- **Learning mechanisms** (STDP, Hebbian, structural plasticity)
- **Agent-world interaction** for embodied cognition
- **Development stages** affecting brain plasticity
- **Checkpointing and replay** for persistent memory
- **Performance optimizations** (SIMD, parallel processing)

## Installation

### Prerequisites

```bash
python 3.8+
scikit-build-core
pybind11
numpy
pytest
```

### Installation Commands

```bash
# Install from source (recommended)
pip install scikit-build-core pybind11 numpy pytest
pip install -e .  # Install in development mode

# Or install from wheel
pip install dist/pynlm-*.whl

# Build only (without installing)
pip install build
python -m build
```

### Verification

```python
import pynlm
print(f"NLM version: {pynlm.__version__}")
config = pynlm.createDefaultConfig()
print(f"Config created: {config}")
```

## Getting Started

### Basic Example: Simple Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Complete Agent Example

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

## API Reference

### Core Classes

#### Brain
The central neural simulation brain class that manages neurons, synapses, and all integrated systems.

**Methods:**

- `__init__(config)` - Create brain with configuration
- `initialize()` - Initialize the brain with configuration
- `step(step_number)` - Perform one simulation step
- `step(step_number, time)` - Perform step with timestamp
- `reset()` - Reset brain state
- `receiveSensoryInput(input)` - Inject sensory input
- `injectCurrent(neuron, current)` - Inject current into specific neuron
- `injectCurrentToNeurons(type, current)` - Inject current to neurons of specific type
- `produceAction()` - Produce motor action from neural activity
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file
- `addRegion(name)` - Add a new neural region
- `getRegion(id)` - Get a region by ID
- `getRegionCount()` - Get number of regions
- `getRegionIds()` - Get all region IDs
- `getRegions()` - Get all regions
- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSynapseCount()` - Get total synapse count
- `getActiveNeuronCount()` - Get active neuron count
- `getFiringNeuronCount()` - Get currently firing neurons
- `getAverageFiringRate()` - Get average firing rate
- `getExcitationInhibitionRatio()` - Get E/I balance ratio
- `getTotalSpikeCount()` - Get total spike count
- `getDevelopmentalStage()` - Get current developmental stage
- `setDevelopmentalStage(stage)` - Set developmental stage
- `getConfig()` - Get configuration
- `logStatus()` - Log brain status

**Properties:**

- `brain.neuron_count` (via config)
- `brain.synapse_density`
- `brain.connection_probability`
- `brain.initial_weight_mean`
- `brain.v_thresh`
- `brain.v_rest`
- `brain.tau_mem`

#### Config
Configuration class for NLM system.

**Methods:**

- `loadFromFile(filepath)` - Load configuration from JSON file
- `loadFromArgs(argc, argv)` - Load from command line arguments
- `saveToFile(filepath)` - Save configuration to JSON file
- `has(key)` - Check if key exists
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all configuration entries
- `summary()` - Get configuration summary

#### AgentBrain
Connects NLM brain to the world, handling sensory transduction and motor decoding.

**Methods:**

- `initialize(world)` - Initialize with world
- `getSensoryInputSize()` - Get expected sensory input size
- `getMotorOutputSize()` - Get expected motor output size
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, predicted_reward)` - Apply reward modulation
- `updateDevelopment(timestep)` - Update development system
- `getDevelopmentalStage()` - Get developmental stage
- `getNeuromodulationLevel()` - Get neuromodulation level
- `getCuriosityLevel()` - Get curiosity level
- `getNoveltyLevel()` - Get novelty level
- `getPredictionError()` - Get prediction error
- `reset()` - Reset agent
- `getBrain()` - Get underlying brain

**Enable/Disable Methods:**

- `enableRewardModulation(enable)`
- `enableStructuralPlasticity(enable)`
- `enableDevelopment(enable)`
- `enableCuriosity(enable)`
- `isRewardModulationEnabled()`
- `isStructuralPlasticityEnabled()`
- `isDevelopmentEnabled()`
- `isCuriosityEnabled()`

#### SimpleWorld
Simple 2D world for NLM simulation.

**Methods:**

- `configure(width, height, visionWidth, visionHeight)` - Configure world
- `reset()` - Reset world
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world
- `applyMotorCommand(cmd, currentTime)` - Apply motor command
- `getSensoryPercept()` - Get sensory percept
- `getAgentBody()` - Get agent body
- `addObject(obj)` - Add world object
- `removeObject(x, y)` - Remove object
- `isValidPosition(x, y)` - Check position validity

#### Vision, Audio, InternalSignals
Sensory input classes.

#### Action
Motor action representation.

#### WorldObject, AgentBody, ActionResult
Supporting classes for world simulation.

### Enumerations

#### NeuronType
- `Excitatory` - Excitatory neuron
- `Inhibitory` - Inhibitory neuron
- `Modulatory` - Modulatory neuron
- `Sensory` - Sensory neuron
- `Motor` - Motor neuron
- `Internal` - Internal neuron

#### SynapseType
- `Excitatory` - Excitatory synapse
- `Inhibitory` - Inhibitory synapse
- `Modulatory` - Modulatory synapse
- `Electrical` - Electrical synapse
- `GapJunction` - Gap junction

#### DevelopmentalStage
- `Initial` - Initial development stage
- `CriticalPeriod` - Critical period of plasticity
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

#### ActionType
- `MoveForward`, `MoveBackward`
- `TurnLeft`, `TurnRight`, `Look`, `LookUp`, `LookDown`
- `Interact`, `Eat`, `Drink`, `Rest`, `Wait`
- `Custom` - Custom action

#### MotorCommand
- Motor commands for low-level control

#### WorldObjectType
- `Empty`, `Resource`, `Hazard`, `Wall`, `Marker`

### Factory Functions

- `createDefaultConfig()` - Create default configuration
- `createBrain(config)` - Create new brain with configuration
- `createSimpleWorld()` - Create new simple world
- `createAgentBrain(brain)` - Create new agent brain interface

### Invalid ID Constants

- `INVALID_NEURON_ID`
- `INVALID_SYNAPSE_ID`
- `INVALID_REGION_ID`
- `INVALID_POPULATION_ID`

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Tutorials

### Tutorial 1: Creating Your First Brain

```python
import pynlm

# Quick start
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(10):
    brain.step(i)

print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
```

### Tutorial 2: Brain in a World

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=16, height=16, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(50):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation finished!")
```

### Tutorial 3: Learning and Development

```python
import pynlm

# Setup with learning enabled
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run learning episode
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    agent.updateDevelopment(0.1)

print("Learning complete!")
```

## Advanced Usage

### Custom Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("neuromod.curiosity.enable", False)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")
```

### Multiple Regions

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Add multiple regions
cortex_id = brain.addRegion("Cortex")
hippocampus_id = brain.addRegion("Hippocampus")

# You can now work with individual regions
region = brain.getRegion(cortex_id)
```

### Saving and Loading Checkpoints

```python
import pynlm

# Create and run a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)

# Save checkpoint
brain.save("brain_checkpoint.bin")

# Later, load the checkpoint
brain2 = pynlm.createBrain(config)
brain2.initialize()
brain2.load("brain_checkpoint.bin")

print("Checkpoint loaded successfully!")
```

### Statistical Monitoring

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Collect statistics over time
statistics = []
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        stats = {
            'step': step,
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'total_spikes': brain.getTotalSpikeCount(),
            'development_stage': brain.getDevelopmentalStage().name
        }
        statistics.append(stats)
        print(f"Step {step}: {stats['firing_neurons']} neurons firing")
```

## Performance Tips

### For Large Networks

1. **Use smaller timesteps**: `config.set("simulation_timestep", 0.0001)`
2. **Reduce neuron count for testing**: `config.set("brain.neuron_count", 100)`
3. **Disable unnecessary features**: `agent.enableCuriosity(False)`

### For Real-time Applications

1. **Use single-threaded mode**
2. **Pre-allocate memory** with larger configurations
3. **Use checkpointing** for long-running simulations

### For Debugging

1. **Enable verbose logging**:
   ```python
   from nlm.core.Logger import Logger
   Logger::getGlobal().setLevel(LogLevel::Debug)
   ```

2. **Use smaller world sizes** for easier debugging
3. **Add print statements** in key locations

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that neurons have input

**"The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Ensure agent has valid motor commands

**"Everything is 0"
- Brains need time to "warm up"
- Some neurons need input to fire
- Check sensory input connection

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

## Examples and Templates

### Template: Simulation Runner

```python
import pynlm

def run_simulation(config_file=None, num_steps=1000, headless=False):
    """Template for running NLM simulations."""
    
    # Load or create configuration
    if config_file:
        config = pynlm.createDefaultConfig()
        config.loadFromFile(config_file)
    else:
        config = pynlm.createDefaultConfig()
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent if needed
    world = pynlm.createSimpleWorld()
    world.configure(**world_config)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable subsystems
    for subsystem, enabled in subsystems.items():
        if hasattr(agent, f'enable{system}'):
            getattr(agent, f'enable{system}')(enabled)
    
    # Run simulation
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Process input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Execute action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        if headless and step % 100 == 0:
            log_stats(brain, agent, step)
    
    return brain, world, agent
```

### Template: Research Experiment

```python
import pynlm
import numpy as np

class NLMExperiment:
    def __init__(self, config):
        self.config = config
        self.results = []
        
    def run_experiment(self, condition, num_trials=100):
        """Run experiment with specific condition."""
        
        # Setup NLM system
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Configure based on condition
        self.configure_condition(agent, condition)
        
        # Run trials
        for trial in range(num_trials):
            trial_data = self.run_trial(agent, world, trial)
            self.results.append(trial_data)
        
        return self.results
    
    def configure_condition(self, agent, condition):
        """Configure agent based on experimental condition."""
        if condition == 'high_curiosity':
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
        elif condition == 'low_plasticity':
            agent.enableStructuralPlasticity(False)
        elif condition == 'reward_learning':
            agent.enableRewardModulation(True)
    
    def run_trial(self, agent, world, trial):
        """Run a single trial."""
        # Reset world for new trial
        world.reset()
        
        # Run trial
        for step in range(200):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.updateDevelopment(0.1)
        
        # Return trial data
        return {
            'trial': trial,
            'firing_rate': brain.getAverageFiringRate(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'reward': agent.getPredictionError()
        }
```

## Further Reading

### Documentation

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 integration audit

### Papers and Publications

- [NLM Research Papers](https://github.com/nlm-project/nlm/publications)
- [Phase-wise Development Reports](https://github.com/nlm-project/nlm/reports)

### Community

- [GitHub Repository](https://github.com/nlm-project/nlm)
- [Documentation Wiki](https://wiki.nlm-project.org)
- [User Forum](https://forum.nlm-project.org)
- [Slack Channel](https://nlm.slack.com)

## Version Information

**Current Version:** 0.1.0

**Release Date:** October 2026

**License:** MIT

**Author:** NLM Research Team

**Contact:** nlm-project@nlm.org

---

*This documentation is continuously updated. For the latest information, visit the official NLM documentation portal.*
