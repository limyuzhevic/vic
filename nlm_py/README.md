"""
NLM Python Module
================

This is the main entry point for the NLM Python bindings. This module provides
access to the full NLM (Neural Learning Machine) neural simulation framework
through a Python interface.

The NLM is an experimental artificial developmental brain that simulates
neural computation with spiking neurons, plasticity, memory, and neuromodulation
systems.

## What This Module Provides

This module exposes the C++ NLM implementation through Python bindings,
providing:

- **Core Brain Classes**: Brain, NeuralRegion, NeuralPopulation, Neuron, Synapse
- **Sensory Systems**: Vision, Audio, InternalSignals, SensoryInput
- **Motor Systems**: Action, MotorCommand
- **Memory Systems**: WorkingMemory, EpisodicMemory, AssociativeMemory
- **Neuromodulation Systems**: Dopamine, Curiosity, Novelty, PredictionError
- **Prediction System**: NeuralPrediction, PredictionSystem
- **Cognition Systems**: NeuralPlanner, ConceptFormation
- **Development System**: DevelopmentSystem, Synaptogenesis, Pruning, Maturation
- **Agent Interface**: AgentBrain for interaction with world
- **Environment**: SimpleWorld for simulation environments
- **Convenience Functions**: Easy-to-use factory functions and utilities

## Getting Started

### Quick Installation

```bash
pip install nlm-py
```

### Basic Usage

```python
import nlm_py

# Create a default brain
brain = nlm_py.create_brain(nlm_py.create_default_config())
brain.initialize()

# Run for some steps
for i in range(100):
    brain.step(i)

print(f"Brain has {brain.get_total_neuron_count()} neurons")
print(f"Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
```

### Complete Example with World Interaction

```python
import nlm_py

# Create brain and agent
brain = nlm_py.create_brain()
brain.initialize()

world = nlm_py.create_simple_world()
world.configure(width=20, height=20, vision_width=8, vision_height=8)
world.reset()

agent = nlm_py.create_agent_brain(brain)
agent.initialize(world)

# Enable learning features
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    brain.step(step)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())
```

## Core Concepts

### Neurons
Neurons are the basic computational units of the NLM brain. They:
- Integrate incoming currents and generate spikes
- Have membrane potentials that evolve over time
- Participate in plasticity rules (STDP, Hebbian)
- Can be of different types (excitatory, inhibitory, modulatory, etc.)

### Synapses
Synapses connect neurons and transmit signals:
- Have weights that can be modified through plasticity
- Can be excitatory or inhibitory
- Support short-term plasticity
- Have transmission delays

### Regions and Populations
- **Regions**: Groups of neurons with shared properties
- **Populations**: Groups of neurons with same type and characteristics

### Memory Systems
The brain implements multiple memory systems:
- **Working Memory**: Transient active information
- **Episodic Memory**: Experience storage and replay
- **Associative Memory**: Pattern associations

### Neuromodulation
Neuromodulators regulate brain function:
- **Dopamine**: Reward and reinforcement
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection
- **Prediction Error**: Learning signal

### Development
The brain develops over time:
- Different developmental stages (initial, critical period, maturation, adult, aging)
- Structural plasticity during development
- Changes in excitability and plasticity rates

## Advanced Usage

### Configuration
You can customize the brain configuration:

```python
config = nlm_py.create_default_config()
config.set('brain.region_count', 5)
config.set('brain.population_size', 200)
config.set('plasticity.enable_stdp', True)
```

### Statistics
The brain provides extensive statistics:

```python
brain = nlm_py.create_brain()
brain.initialize()

print(f"Neurons: {brain.get_total_neuron_count()}")
print(f"Synapses: {brain.get_total_synapse_count()}")
print(f"Firing neurons: {brain.get_firing_neuron_count()}")
print(f"Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
print(f"Excitation/Inhibition ratio: {brain.get_excitation_inhibition_ratio():.2f}")
print(f"Developmental stage: {brain.get_developmental_stage().name}")
```

### Saving and Loading
Brain states can be saved and loaded:

```python
brain.save("my_brain.nlm")
# Later...
brain.load("my_brain.nlm")
```

## Python API Reference

### Classes

#### Core Types
- `NeuronId`: Unique identifier for neurons
- `SynapseId`: Unique identifier for synapses
- `RegionId`: Unique identifier for regions
- `PopulationId`: Unique identifier for populations
- `Config`: Configuration for NLM simulation
- `Brain`: Central neural simulation brain class

#### Neural Components
- `NeuralRegion`: Brain region containing multiple populations
- `NeuralPopulation`: Group of neurons with shared properties
- `Neuron`: Single neuron with LIF dynamics
- `Synapse`: Connection between neurons

#### Systems
- `SpikeSystem`: Spike generation and propagation
- `STDP`: Spike-timing dependent plasticity
- `Hebbian`: Hebbian plasticity
- `StructuralPlasticity`: Structural plasticity
- `Neuromodulator`: Base class for neuromodulators
- `Dopamine`: Reward and reinforcement
- `Curiosity`: Exploration motivation
- `Novelty`: Novelty detection
- `PredictionError`: Prediction error signal

#### Memory Systems
- `NeuralWorkingMemory`: Transient active information
- `NeuralEpisodicMemory`: Experience storage
- `NeuralAssociativeMemory`: Pattern associations

#### Prediction and Cognition
- `PredictionSystem`: Prediction and error computation
- `NeuralPrediction`: Neural prediction implementation
- `NeuralPlanner`: Action planning
- `ConceptFormation`: Pattern discovery

#### Development
- `DevelopmentSystem`: Developmental regulation
- `Synaptogenesis`: Creation of new synapses
- `Pruning`: Elimination of synapses
- `Maturation`: Neural maturation

#### Sensory and Motor
- `SensoryInput`: Base class for sensory input
- `Vision`: Visual sensory input
- `Audio`: Audio sensory input
- `InternalSignals`: Internal sensory signals
- `Action`: Action representation for motor output
- `MotorCommand`: Low-level motor commands

#### Agent and Environment
- `AgentBrain`: Agent brain interface
- `AgentBody`: Agent body state
- `SensoryPercept`: Sensory percept data
- `ActionResult`: Action result from world
- `SimpleWorld`: Simple 2D world for simulation
- `WorldObject`: World object representation

### Convenience Functions

#### Factory Functions
- `create_default_config()`: Create a default configuration
- `create_brain(config)`: Create a new brain with configuration
- `create_simple_world()`: Create a new simple world
- `create_agent_brain(brain)`: Create an agent brain interface

#### Utility Functions
- `run_simulation(brain, world, agent, num_steps)`: Run a simulation loop
- `format_spike_statistics(brain)`: Format spike statistics
- `format_memory_stats(brain)`: Format memory statistics
- `format_prediction_stats(brain)`: Format prediction statistics

### Examples

The `examples` directory contains comprehensive examples:

- `main.py`: Runs all example demonstrations
- Other examples show different aspects of the NLM system

## Error Handling

The module provides custom exceptions:

- `NLMError`: Base exception for all NLM errors
- `ConfigError`: Error in configuration
- `BrainError`: Error in brain operations
- `NeuronError`: Error in neuron operations
- `SynapseError`: Error in synapse operations
- `SimulationError`: Error during simulation
- `MemoryError`: Error in memory operations
- `PredictionError`: Error in prediction operations
- `NeuromodulationError`: Error in neuromodulation operations

```python
import nlm_py

try:
    brain = nlm_py.create_brain()
    brain.initialize()
    for i in range(100):
        brain.step(i)
except nlm_py.BrainError as e:
    print(f"Brain error: {e}")
except nlm_py.ConfigError as e:
    print(f"Configuration error: {e}")
```

## Performance Considerations

### Memory Efficiency
The NLM implementation is designed for memory efficiency:
- Uses memory pools for efficient allocation
- Supports sparse connectivity
- Implements event-driven computation

### Parallel Processing
The brain supports parallel processing:
- SIMD vectorization for neural computations
- Event queues for spike propagation
- Parallel plasticity updates

### Scalability
The implementation can handle large networks:
- Millions of neurons and synapses
- Distributed computation support
- Checkpoint and restart capabilities

## Integration with Other Systems

The NLM Python bindings can be integrated with:

### Simulation Environments
- Custom worlds can be built on top of SimpleWorld
- Sensory inputs can be provided from external sources
- Motor outputs can be connected to actuators

### Machine Learning
- The brain can be used as a neural network component
- Supports reinforcement learning through reward modulation
- Can be combined with traditional ML algorithms

### Robotics
- The brain can control robotic agents
- Sensory inputs from various sensors
- Motor outputs to control motors and actions

### Neuroscience Research
- The brain can be used to study neural dynamics
- Supports developmental studies
- Can be used to investigate memory systems

## Future Directions

This is a work in progress. Future enhancements include:

1. **More Complete Python Bindings**: Full bindings for all C++ components
2. **Visualization Tools**: Integration with plotting libraries
3. **Distributed Computing**: Support for multi-machine simulation
4. **Advanced Learning**: Integration with modern learning algorithms
5. **Documentation**: Complete API documentation with examples
6. **Testing**: Comprehensive test suite

## License

MIT License

## Authors

NLM Authors

## Version

Version 0.1.0
