# NLM Tutorial: Comprehensive Guide to Neural Learning Machine

## Introduction

This tutorial provides comprehensive guidance on using the Neural Learning Machine (NLM) framework. NLM is a brain-inspired computational system that implements artificial developmental learning, memory systems, and cognitive capabilities.

## Prerequisites

- Python 3.8 or higher
- NLM package installed (`pip install pynlm`)
- Basic understanding of neural concepts (neurons, synapses, plasticity)

## Installation

### Installing from PyPI

```bash
pip install pynlm
```

### Building from Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build the Python bindings
pip install -e .
```

## Quick Start

### Example 1: Basic Brain Simulation

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete. Neurons: {brain.getTotalNeuronCount()}")
```

### Example 2: Complete Agent in World

```python
import pynlm

# Setup the complete agent-world system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Enable learning capabilities
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
print("Agent simulation complete!")
```

## Core Concepts

### Neurons

Neurons in NLM are Leaky Integrate-and-Fire (LIF) units that:
- Receive and integrate synaptic inputs
- Generate spikes when reaching threshold
- Have refractory periods to prevent excessive firing

### Synapse Types

NLM supports multiple synapse types:
- **Excitatory**: Positive weight, increases post-synaptic activity
- **Inhibitory**: Negative weight, decreases post-synaptic activity
- **Modulatory**: Complex effects on neural plasticity
- **Electrical**: Direct electrical coupling between neurons

### Plasticity Rules

NLM implements several plasticity rules:
- **STDP (Spike-Timing-Dependent Plasticity)**: Time-based weight adjustment
- **Hebbian**: "Neurons that fire together, wire together"
- **Reward-Modulated**: Learning driven by prediction errors
- **Structural Plasticity**: Formation and elimination of synapses

## Working with the API

### Configuration Management

NLM uses a flexible configuration system for customizing simulation parameters:

```python
# Create configuration
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
loaded_config = pynlm.createDefaultConfig()
loaded_config.loadFromFile("my_config.json")
```

### Brain Operations

Core brain operations are straightforward:

```python
# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get statistics
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### World Simulation

The world provides a structured environment for agent interaction:

```python
# Create world
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

# Get sensory input
percept = world.getSensoryPercept()

# Get agent state
agent_body = world.getAgentBody()
print(f"Agent position: ({agent_body.x}, {agent_body.y})")
```

### Agent Interface

The agent provides the interface between brain and world:

```python
# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Process sensory input
agent.processSensoryInput(percept)

# Decode action
action = agent.decodeMotorCommand()

# Apply action to world
world.applyMotorCommand(action, world.getSimulationTime())
```

## Advanced Features

### Custom Plasticity Rules

NLM supports custom plasticity rules for advanced users:

```python
from custom_plasticity import create_stdp_plus_rule, create_reward_modulated_rule

# Create custom rules
stdp_rule = create_stdp_plus_rule("custom_stdp", ltp_weight=0.02)
reward_rule = create_reward_modulated_rule("reward_based", reward_scale=1.5)

# Use in agent
agent.add_plasticity_rule(stdp_rule, is_default=True)
```

### Performance Profiling

Track and analyze simulation performance:

```python
from performance_profiler import run_performance_analysis, create_performance_report

# Run analysis
performance_data = run_performance_analysis(brain, world, agent, steps=500)

# Create report
report_path = create_performance_report("my_simulation", brain, performance_data, world, agent)
```

### Checkpoint Management

Save and restore simulation state:

```python
from checkpoint_manager import CheckpointManager, create_default_checkpoint_config

# Setup checkpointing
config = create_default_checkpoint_config()
checkpoint_manager = CheckpointManager(config)

# Create checkpoint during simulation
checkpoint = checkpoint_manager.create_checkpoint(brain, step_number=100)

# Restore from checkpoint later
checkpoint_manager.recover_from_checkpoint("checkpoint_100", brain, world, agent)
```

### Configuration Management

Advanced configuration management:

```python
from advanced_config import NLMConfigManager, PRESETS

# Use configuration manager
config_manager = NLMConfigManager()

# Apply presets
config_manager.apply_preset("research")
config_manager.apply_preset("production")
config_manager.apply_preset("learning")

# Export/import configurations
config_manager.export_config("my_config.json")
config_manager.import_config("backup_config.json")
```

## Tutorials

### Tutorial 1: Building Your First NLM System

This tutorial covers the basics of creating and running an NLM simulation.

**Topics covered:**
- Creating a basic brain
- Setting up a world environment
- Creating and initializing an agent
- Running a simple simulation
- Analyzing results

**Exercise:** Create a simulation that demonstrates learning through STDP

### Tutorial 2: Advanced Agent Behavior

This tutorial covers advanced agent capabilities and learning mechanisms.

**Topics covered:**
- Neuromodulation (reward, curiosity, novelty)
- Developmental stages
- Memory systems integration
- Action planning and selection

**Exercise:** Implement an agent that learns through curiosity-driven exploration

### Tutorial 3: Custom Plasticity Rules

This tutorial covers creating custom plasticity rules for specialized learning.

**Topics covered:**
- Understanding the plasticity rule interface
- Implementing custom STDP variants
- Reward-modulated learning rules
- Compound plasticity rules

**Exercise:** Create a novel plasticity rule that implements homeostatic learning

### Tutorial 4: Performance Optimization

This tutorial covers performance analysis and optimization techniques.

**Topics covered:**
- Performance profiling
- Memory optimization
- Bottleneck identification
- Scalability analysis

**Exercise:** Profile your simulation and identify performance bottlenecks

## Reference Guide

### Python API Reference

This section provides detailed reference information for the NLM Python API.

#### Core Classes

**Brain Class**
- `Brain(config)` - Create a new brain with configuration
- `initialize()` - Initialize the brain
- `step(step_number)` - Run simulation step
- `step(step_number, current_time)` - Run step with timestamp
- `injectCurrentToNeurons(type, current)` - Inject current into neurons
- `produceAction()` - Generate motor action from neural activity

**Config Class**
- `createDefaultConfig()` - Create default configuration
- `loadFromFile(filepath)` - Load from file
- `loadFromArgs(argc, argv)` - Load from command line
- `set(key, value)` - Set configuration value
- `get(key)` - Get configuration value
- `getOr(key, default)` - Get with default

**SimpleWorld Class**
- `createSimpleWorld()` - Create new world
- `configure(width, height, visionWidth, visionHeight)` - Configure world
- `reset()` - Reset world state
- `update(timestep)` - Update world state
- `getSensoryPercept()` - Get sensory input
- `getAgentBody()` - Get agent state

**AgentBrain Class**
- `createAgentBrain(brain)` - Create agent brain interface
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode action from brain activity
- `applyRewardModulation(reward, predicted)` - Apply reward-based learning
- `enableRewardModulation(bool)` - Enable/disable reward modulation
- `enableCuriosity(bool)` - Enable/disable curiosity
- `enableDevelopment(bool)` - Enable/disable development

#### Enumerations

**NeuronType**
- `Excitatory` - Excitatory neurons
- `Inhibitory` - Inhibitory neurons
- `Sensory` - Sensory neurons
- `Motor` - Motor neurons
- `Internal` - Internal neurons

**ActionType**
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `Interact` - Interact with environment
- `Eat` - Eat food item
- `Rest` - Rest (no action)

**DevelopmentalStage**
- `Initial` - Initial developmental stage
- `CriticalPeriod` - Critical learning period
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

### Mathematical Background

This section covers the mathematical foundations of NLM:

#### Leaky Integrate-and-Fire Neuron

The LIF neuron model is described by:

```
dv/dt = (-v + I) / τ_m
v → v_reset if v ≥ θ
```

Where:
- v: Membrane potential
- τ_m: Membrane time constant
- I: Input current
- θ: Threshold potential
- v_reset: Reset potential

#### STDP Learning Rule

STDP is based on spike timing:

For pre-post spike pairs (Δt > 0):
```
Δw = A+ * exp(-Δt / τ+)
```

For post-pre spike pairs (Δt < 0):
```
Δw = A- * exp(Δt / τ-)
```

Where:
- A+: Potentiation amplitude
- A-: Depression amplitude
- τ+: Time constant for potentiation
- τ-: Time constant for depression

#### Reward Prediction Error

RPE is calculated as:
```
δ = reward - predicted_reward
```

This drives reward-modulated learning.

## Best Practices

### Simulation Design

1. **Start Simple**: Begin with small networks and gradually increase complexity
2. **Use Checkpoints**: Regularly save checkpoints to avoid lost work
3. **Profile Early**: Profile performance early to identify bottlenecks
4. **Validate Configurations**: Validate configuration before running simulations
5. **Document Parameters**: Document all simulation parameters for reproducibility

### Code Organization

1. **Use Configuration Files**: Store simulation parameters in configuration files
2. **Modular Design**: Create separate files for different components
3. **Error Handling**: Implement proper error handling and logging
4. **Testing**: Write tests for critical functionality
5. **Documentation**: Document code with docstrings and examples

### Performance Optimization

1. **Profile First**: Identify bottlenecks before optimizing
2. **Use Appropriate Network Size**: Match network size to hardware
3. **Memory Management**: Monitor and optimize memory usage
4. **Parallel Processing**: Use parallel processing for large simulations
5. **Caching**: Cache frequently used data

## Frequently Asked Questions

### Q: How do I get started with NLM?

A: Start with the "Quick Start" section. Create a basic brain simulation using the examples provided.

### Q: What simulation parameters should I adjust?

A: Key parameters to adjust include:
- `brain.neuron_count` (network size)
- `brain.synapse_density` (connection density)
- `plasticity.stdp.enable` (STDP learning)
- `plasticity.hebbian.enable` (Hebbian learning)
- `neuromod.curiosity.enable` (exploration behavior)

### Q: How do I run long simulations?

A: Use checkpointing to save progress periodically. Create checkpoints every 1000 steps or use time-based checkpointing.

### Q: How do I profile performance?

A: Use the `performance_profiler` module. Run `run_performance_analysis()` to get detailed performance metrics.

### Q: Can I create custom learning rules?

A: Yes! Use the `custom_plasticity` module to create custom plasticity rules implementing novel learning mechanisms.

## Troubleshooting

### Common Issues

1. **Segmentation Faults**: Ensure configuration parameters are within valid ranges
2. **Memory Issues**: Reduce network size or optimize memory usage
3. **Performance Issues**: Profile performance to identify bottlenecks
4. **Configuration Errors**: Validate configuration before running simulations

### Error Messages

- **RuntimeError**: Usually indicates invalid configuration or invalid operation
- **TypeError**: Usually indicates incorrect parameter types
- **ValueError**: Usually indicates invalid parameter values

## Resources

### Documentation
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

### Examples
- [advanced_examples.py](advanced_examples.py) - Advanced examples
- [custom_plasticity.py](custom_plasticity.py) - Custom plasticity examples
- [checkpoint_manager.py](checkpoint_manager.py) - Checkpointing examples
- [performance_profiler.py](performance_profiler.py) - Performance profiling examples

### Community
- [GitHub Repository](https://github.com/nlm-project/nlm) - Source code and issues
- [Documentation](https://nlm.readthedocs.io) - Online documentation
- [Discussions](https://github.com/nlm-project/nlm/discussions) - Community discussions

## Acknowledgments

This NLM framework is inspired by computational neuroscience research on:
- Spiking neural networks
- Neuronal plasticity mechanisms
- Brain-inspired learning algorithms
- Developmental psychology

The framework is continuously evolving, and contributions from the community are welcome!

## License

MIT License. See LICENSE file for details.

---

This tutorial is continuously updated as new features are added to NLM. For the latest version, visit the GitHub repository.
