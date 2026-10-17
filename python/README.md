# NLM Python Bindings Documentation

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Python bindings.

## Overview

The NLM Python bindings provide a high-level interface to the NLM neural simulation framework. The bindings are implemented using **pybind11**, which allows seamless integration between Python and the C++ NLM library.

The NLM framework is a computational brain model that implements:
- **Real spiking neural computation** with Leaky Integrate-and-Fire (LIF) neurons
- **Event-driven dynamics** with synaptic delays and spike propagation
- **Multiple plasticity rules**: STDP, Hebbian, and structural plasticity
- **Integrated brain systems**: Memory, prediction, cognition, and neuromodulation
- **Developmental processes** affecting neural connectivity
- **Agent-based simulation** with sensory perception and motor actions

## Installation

### Prerequisites

- Python 3.8 or higher
- C++ compiler with C++20 support
- CMake 3.16 or higher

### Installation Methods

#### 1. From Source (Recommended)

```bash
# Clone the repository
.gitignore
python/
    nlm_cli.py
    basic_examples.py
    __init__.py
    pyproject.toml
    setup.cfg

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DPYTHON_EXECUTABLE=$(which python3) \
    -DNLM_BUILD_PYTHON_BINDINGS=ON

# Build the Python module
make -j$(nproc)

# Install the package
cmake --install .

# Verify installation
python3 -c "import pynlm; print('pynlm version:', pynlm.__version__)"
```

#### 2. Using pip (if available)

```bash
pip install -e .
```

#### 3. Development Installation

```bash
pip install -e .[dev]
```

## Quick Start

### Creating a Basic Brain

```python
import pynlm as nlm

# Create a default configuration
config = nlm.createDefaultConfig()
config.set("random_seed", 42)
config.set("neuron_count", 100)
config.set("region_count", 1)

# Create brain
brain = nlm.createBrain(config)
brain.initialize()

print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
```

### Running a Simulation

```python
# Run a simple simulation
for step in range(100):
    brain.step(step, step * 0.001)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Using the Command Line Interface

```bash
# Basic usage
./python/nlm_cli.py --steps 100 --neuron-count 50

# With learning enabled
./python/nlm_cli.py --steps 500 --plasticity --verbose

# Help information
./python/nlm_cli.py --help
```

## API Reference

### Core Classes

#### Brain
Main neural simulation brain class implementing real spiking neural computation.

**Constructor:**
```python
brain = nlm.createBrain(config)
```

**Key Methods:**
- `initialize()` - Initialize the brain with configuration
- `step(step, time)` - Perform a simulation step
- `injectCurrent(neuron, current)` - Inject current into specific neuron
- `produceAction()` - Produce motor action based on neural activity
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file

#### Config
Configuration management for NLM simulation parameters.

**Constructor:**
```python
config = nlm.createDefaultConfig()
```

**Key Methods:**
- `loadFromFile(filepath)` - Load configuration from JSON file
- `set(key, value)` - Set configuration value
- `get(key, default)` - Get configuration value
- `summary()` - Get configuration summary string

#### Neuron Types
Enumeration of different neuron types:
- `Excitatory` - Excitatory neurons
- `Inhibitory` - Inhibitory neurons
- `Modulatory` - Modulatory neurons
- `Sensory` - Sensory neurons
- `Motor` - Motor neurons
- `Internal` - Internal neurons

#### Action
Motor action representation for agent behavior.

**Constructor:**
```python
action = nlm.Action(nlm.ActionType.MoveForward)
```

**Key Methods:**
- `getType()` - Get action type
- `getParameters()` - Get action parameters
- `setType(type)` - Set action type

### Memory Systems

#### NeuralWorkingMemory
Transient active information storage.

#### NeuralEpisodicMemory
Experience storage with encoding and replay capabilities.

#### NeuralAssociativeMemory
Pattern association and memory linking.

#### SemanticMemory
Conceptual knowledge and semantic information.

#### ProceduralMemory
Skill and behavior memory.

#### NeuralEpisodicMemory
Episodic memory with experience replay.

### Neuromodulation Systems

#### Dopamine
Reward prediction and reinforcement signals.

#### Curiosity
Exploration motivation and intrinsic reward.

#### Novelty
Novelty detection and surprise signals.

#### PredictionError
Prediction error signals for learning.

### Development System

#### DevelopmentSystem
Developmental stage management and age-dependent plasticity.

#### Synaptogenesis
Formation of new synaptic connections during development.

#### Pruning
Elimination of synapses during development.

#### Maturation
Neuron and connection maturation processes.

### Experiment Framework

#### Experiment
Base class for NLM experiments.

#### ExperimentRunner
Manages and executes experiments.

#### Metrics
Performance metrics collection and analysis.

## Examples

### 1. Basic Brain Simulation

See `examples/basic_examples.py` for comprehensive examples.

### 2. Learning Simulation

```python
# Create configuration for learning
config = nlm.createDefaultConfig()
config.set("neuron_count", 200)
config.set("plasticity_learning_rate", 0.02)

brain = nlm.createBrain(config)
brain.initialize()

# Enable plasticity
for region in brain.getRegions():
    for synapse in region.getSynapses():
        synapse.enablePlasticity(True, True, False)

# Run learning simulation
for step in range(1000):
    brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 40.0)
    brain.step(step, step * 0.001)
```

### 3. Agent-Based Simulation

```python
# Create world
world = nlm.createSimpleWorld()
world.configure(80, 80, 20, 20)

# Create brain and agent brain
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

agent_brain = nlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Run agent simulation
for step in range(100):
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent_brain.processSensoryInput(percept)
    
    # Get motor command
    cmd = agent_brain.decodeMotorCommand()
    
    # Apply motor command
    world.applyMotorCommand(cmd, world.getSimulationTime())
    
    # Step world
    world.update(0.001)
```

## Advanced Features

### 1. Multiple Brain Regions

```python
# Create multiple regions
region1 = brain.addRegion("Sensory")
region2 = brain.addRegion("Motor")

# Connect regions
brain.addInterRegionConnection(region1, region2, 0.5, 5)
```

### 2. STDP Learning

```python
# Enable STDP plasticity
for synapse in synapses:
    synapse.enablePlasticity(False, True, False)  # Hebbian=False, STDP=True, Structural=False
```

### 3. Development Stages

```python
# Set developmental stage
brain.setDevelopmentalStage(nlm.DevelopmentalStage.CriticalPeriod)

# Update development
brain.develop()
```

### 4. Checkpointing

```python
# Save brain state
brain.save("nlm_checkpoint_001.bin")

# Reset and restore
brain.reset()
brain.load("nlm_checkpoint_001.bin")
```

## Configuration Reference

### Core Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| random_seed | int64 | 42 | Random seed for reproducible experiments |
| simulation_timestep | double | 0.001 | Simulation timestep in seconds |
| neuron_count | int64 | 1000 | Number of neurons in the brain |
| region_count | int64 | 2 | Number of brain regions |
| connection_probability | float | 0.1 | Connection probability for random connectivity |

### Plasticity Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| plasticity_learning_rate | float | 0.01 | Base learning rate for plasticity |
| stdp_ltp_weight | float | 0.01 | STDP LTP weight |
| stdp_ltd_weight | float | 0.012 | STDP LTD weight |
| stdp_time_constant | float | 20.0 | STDP time constant |

### Development Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| development_synaptogenesis_rate | float | 0.001 | Synaptogenesis rate |
| development_pruning_rate | float | 0.0001 | Pruning rate |

### Neuromodulation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| dopamine_baseline | float | 0.1 | Dopamine baseline level |
| curiosity_rate | float | 0.05 | Curiosity rate |
| novelty_threshold | float | 0.1 | Novelty detection threshold |

## Troubleshooting

### Common Issues

#### 1. Python Module Import Error

**Problem:** `ImportError: No module named 'pynlm'`

**Solution:**
```bash
# Build the module
mkdir build && cd build
cmake ..
make
python3 -c "import pynlm"
```

#### 2. Missing Dependencies

**Problem:** `ImportError: cannot import name 'SomeClass'`

**Solution:** Ensure the Python module is properly built. Check if all pybind11 modules are compiled.

#### 3. Configuration File Not Found

**Problem:** Configuration file not found or invalid

**Solution:** Use default configuration or provide valid JSON configuration file.

### Build Issues

#### CMake Configuration Error

**Problem:** `CMake Error: Unknown argument "-DPYTHON_EXECUTABLE"`

**Solution:** Ensure CMake version is 3.16 or higher. For pybind11 integration, use `pybind11_add_module`.

## Testing

### Running Tests

```bash
# Run Python tests
pytest tests/ -v

# Run examples
python3 examples/basic_examples.py

# Run CLI tool
./python/nlm_cli.py --steps 50
```

### Test Structure

The test suite includes:
- **Unit tests** for individual NLM components
- **Integration tests** for system interactions
- **Performance benchmarks** for optimization validation
- **Example demonstrations** for user guidance

## Future Enhancements

### Planned Features

1. **Enhanced Python API**
   - Context managers for resource handling
   - Iterator protocols for memory traversal
   - More comprehensive documentation

2. **Advanced Visualization**
   - Neural activity plotting
   - Brain state visualization
   - Simulation monitoring

3. **Machine Learning Integration**
   - Neural network interfaces
   - Learning curve analysis
   - Performance optimization suggestions

4. **Distributed Computing**
   - Multi-GPU support
   - Parallel simulation
   - Cluster computing capabilities

### Contributing

Contributions are welcome! Please follow the project's contribution guidelines:

1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Add tests and documentation
5. Submit a pull request

## License

This project is licensed under the MIT License. See `LICENSE` file for details.

## Acknowledgments

This project builds upon the foundational work of the NLM research team and the pybind11 community. We acknowledge the contributions of all project maintainers and contributors.

## Contact

For questions, issues, or suggestions, please visit the project repository or contact the development team.

---

*Last updated: 2026-10-17*
*Version: 0.1.0*