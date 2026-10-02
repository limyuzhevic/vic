# NLM (Neural Learning Machine) - Enhanced Python Bindings

A comprehensive neural simulation framework with enhanced Python bindings, command-line interface, and advanced features for research and experimentation.

## Overview

NLM is an experimental computational brain project designed to create a neural system that learns and develops through interaction with an environment. This project provides enhanced Python bindings for easy access to the full NLM simulation capabilities, along with tools for experimentation and research.

## What's New

This enhanced version includes:

### 1. **Enhanced Python Bindings** (`python/bindings.cpp`)
- **Advanced Brain API**: Added comprehensive methods for accessing internal systems (STDP, Hebbian, working memory, episodic memory, prediction system, planning, concept formation, attention, development, neuromodulation)
- **Convenience Methods**: Added aliases (`getNeuronCount()`, `getSynapseCount()`, etc.) for Pythonic access
- **Simulation Helpers**: `runForSteps()`, `runForTime()`, `getStats()`, `clone()`
- **Statistical Analysis**: `computeBrainStats()`, `formatBrainStats()` for comprehensive statistics
- **Enhanced AgentBrain**: Added `setAllFeatures()`, `runEpisode()`, `getFeatureStatus()`
- **Quick Setup Functions**: `createQuickBrain()`, `createQuickAgentBrain()`, `runQuickSimulation()`
- **Version and Documentation**: `__version__`, `__author__`, `__description__`, `getHelp()`

### 2. **Command-Line Interface** (`nlm_cli.py`)

A comprehensive CLI tool for:

- `nlm_cli quick-test [--steps N]`: Run quick brain tests with default settings
- `nlm_cli run-simulation [--brain FILE] [--steps N] [--save FILE]`: Run complete simulations
- `nlm_cli run-agent [options]`: Run agents in world environments
- `nlm_cli stats [--output FILE] [brain_file]`: Show brain statistics
- `nlm_cli interactive`: Start Python interactive session with NLM loaded
- `nlm_cli help`: Show help information

### 3. **Configuration Utility** (`nlm_config.py`)

A powerful configuration management tool:

- `nlm_config create [--output FILE] [options]`: Create new configuration files with custom values
- `nlm_config validate [input]`: Validate configuration files
- `nlm_config compare config1 config2`: Compare two configuration files
- `nlm_config show [input]`: View configuration details
- `nlm_config help`: Show help information

### 4. **Advanced Examples** (`nlm_advanced_examples.py`)

Seven comprehensive examples demonstrating NLM capabilities:

- **Agent Learning**: Curiosity-driven exploration with memory integration
- **Development**: Brain maturation and developmental stages
- **Plasticity Experiment**: STDP and Hebbian learning experiments
- **Multi-Region**: Multi-region brain architectures with inter-region connections
- **Memory Consolidation**: Memory storage, replay, and consolidation
- **World Interaction**: Complex agent-world interaction with reward and curiosity
- **Performance**: Benchmarking and performance analysis

### 5. **Bug Fixes**

- Fixed assertion bug in `tests/test_random.cpp` (assert(true || false) → assert(valid boolean)

## Python Installation

### Prerequisites

```bash
pip install pynlm
```

### Building from Source

```bash
# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Using with pip

```bash
# Install from PyPI (if available)
pip install nlm

# Or install from source
pip install .
```

## Quick Start Guide

### Basic Brain Operations

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.region_count", 1)

brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Get statistics
stats = pynlm.computeBrainStats(brain)
print(f"Total spikes: {stats['total_spikes']}")
print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")

# Save brain
brain.save("my_brain.bin")
```

### Agent in World

```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setAgentStart(10.0, 10.0)

# Create brain
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 800)

brain = nlm.createBrain(config)
brain.initialize()

# Create agent
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run episode
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Episode complete. Agent moved to ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
```

### Command-Line Usage

```bash
# Quick test
./nlm_cli.py quick-test --steps 100

# Run simulation with saved brain
./nlm_cli.py run-simulation --brain my_brain.bin --steps 1000 --save trained_brain.bin

# Run agent in world
./nlm_cli.py run-agent --world-width 30 --world-height 30 --steps 500 --enable-features

# Show brain statistics
./nlm_cli.py stats my_brain.bin --output stats.txt

# Interactive mode
./nlm_cli.py interactive
```

### Configuration Management

```bash
# Create configuration file
./nlm_config.py create --neuron-count 2000 --region-count 3 --random-seed 12345

# Validate configuration
./nlm_config.py validate my_config.json

# Compare configurations
./nlm_config.py compare config1.json config2.json

# Show configuration details
./nlm_config.py show my_config.json
```

### Advanced Examples

```bash
# Run all examples (takes several minutes)
python nlm_advanced_examples.py

# Run specific example
python nlm_advanced_examples.py agent-learning --steps 500
python nlm_advanced_examples.py development
python nlm_advanced_examples.py world-interaction --steps 1000
```

## API Reference

### Brain Class

The enhanced Brain class provides comprehensive access to neural systems:

#### Core Methods
- `brain.initialize()`: Initialize the brain
- `brain.step(step)`: Perform simulation step
- `brain.step(step, time)`: Perform step with timestamp
- `brain.receiveSensoryInput(input)`: Inject sensory input
- `brain.produceAction()`: Generate motor action
- `brain.reset()`: Reset brain state
- `brain.save(filepath)`: Save brain state
- `brain.load(filepath)`: Load brain state

#### Statistics Methods
- `brain.getTotalNeuronCount()`: Get total neuron count
- `brain.getTotalSynapseCount()`: Get total synapse count
- `brain.getTotalSpikeCount()`: Get total spike count
- `brain.getFiringNeuronCount()`: Get currently firing neurons
- `brain.getAverageFiringRate()`: Get average firing rate
- `brain.getExcitationInhibitionRatio()`: Get E/I balance ratio

#### Advanced Access Methods
- `brain.getSTDP()`: Get STDP plasticity system
- `brain.getHebbian()`: Get Hebbian plasticity system
- `brain.getStructuralPlasticity()`: Get structural plasticity system
- `brain.getSpikeSystem()`: Get spike system
- `brain.getWorkingMemory()`: Get working memory system
- `brain.getEpisodicMemory()`: Get episodic memory system
- `brain.getPredictionSystem()`: Get prediction system
- `brain.getPlanner()`: Get neural planner
- `brain.getConceptFormation()`: Get concept formation system
- `brain.getAttention()`: Get attentional selection system
- `brain.getDevelopmentSystem()`: Get development system

#### Convenience Methods
- `brain.getNeuronCount()`: Alias for `getTotalNeuronCount()`
- `brain.getSynapseCount()`: Alias for `getTotalSynapseCount()`
- `brain.getSpikeCount()`: Alias for `getTotalSpikeCount()`
- `brain.getFiringRate()`: Alias for `getAverageFiringRate()`
- `brain.getEIRatio()`: Alias for `getExcitationInhibitionRatio()`

#### Simulation Helpers
- `brain.runForSteps(steps)`: Run simulation for given steps
- `brain.runForTime(duration, timestep)`: Run simulation for given duration
- `brain.getStats()`: Get comprehensive statistics as dictionary
- `brain.clone()`: Create deep copy of brain

### AgentBrain Class

Enhanced AgentBrain interface:

#### Core Methods
- `agent.initialize(world)`: Initialize with world
- `agent.processSensoryInput(percept)`: Process sensory input
- `agent.decodeMotorCommand()`: Decode brain motor activity
- `agent.applyRewardModulation(reward, predicted)`: Apply reward-based neuromodulation
- `agent.updateDevelopment(timestep)`: Update development system
- `agent.reset()`: Reset agent for new episode

#### Feature Control
- `agent.enableRewardModulation(enable)`: Enable/disable reward modulation
- `agent.enableStructuralPlasticity(enable)`: Enable/disable structural plasticity
- `agent.enableDevelopment(enable)`: Enable/disable development
- `agent.enableCuriosity(enable)`: Enable/disable curiosity
- `agent.setAllFeatures(enable)`: Enable/disable all features at once

#### Advanced Methods
- `agent.getFeatureStatus()`: Get status of all enabled features
- `agent.getSensoryInputSize()`: Get expected sensory input size
- `agent.getMotorOutputSize()`: Get expected motor output size

### Configuration Class

Enhanced Configuration class:

#### Core Methods
- `config = pynlm.createDefaultConfig()`: Create default configuration
- `config.loadFromFile(filepath)`: Load from JSON file
- `config.saveToFile(filepath)`: Save to JSON file
- `config.set(key, value)`: Set configuration value
- `config.get(key)`: Get configuration value
- `config.has(key)`: Check if key exists
- `config.getKeys()`: Get all configuration keys
- `config.summary()`: Get configuration summary

### Utility Functions

#### Quick Setup
- `pynlm.createQuickBrain()`: Create brain with default settings
- `pynlm.createQuickAgentBrain(world)`: Create complete agent setup
- `pynlm.runQuickSimulation(brain, steps)`: Run quick simulation

#### Statistics
- `pynlm.computeBrainStats(brain)`: Compute comprehensive brain statistics
- `pynlm.formatBrainStats(brain)`: Format statistics as readable string

#### Version and Help
- `pynlm.__version__`: Version string
- `pynlm.__author__`: Author string
- `pynlm.__description__`: Description string
- `pynlm.getHelp()`: Get help documentation

## Files and Directories

```
workspace/
├── src/                    # C++ source code
│   ├── brain/              # Neural components
│   ├── plasticity/         # Plasticity rules
│   ├── memory/             # Memory systems
│   ├── neuromodulation/     # Neuromodulators
│   ├── cognition/          # Cognitive systems
│   ├── ...                 # Many other directories
│
├── python/                 # Python bindings
│   └── bindings.cpp        # Enhanced C++/Python bindings
│
├── nlm_cli.py             # Command-line interface
├── nlm_config.py          # Configuration utility
├── nlm_advanced_examples.py # Advanced examples
├── tests/                 # Unit tests
│   ├── test_main.cpp       # Test runner
│   ├── test_brain.cpp      # Brain tests
│   ├── test_neuron.cpp     # Neuron tests
│   ├── test_synapse.cpp    # Synapse tests
│   ├── ...                # Many other tests
│
├── easy_usage.md          # Beginner usage guide
├── HOW_TO_USE.md          # Comprehensive usage documentation
├── README.md              # Project overview
└── pyproject.toml        # Python package configuration
```

## Testing

### Running Tests

```bash
# Build the project
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run tests
ctest --output-on-failure

# Or run Python tests
pytest tests/
```

### Unit Tests

The project includes comprehensive unit tests:

- **test_main.cpp**: Test runner that executes all unit tests
- **test_brain.cpp**: Brain functionality tests
- **test_neuron.cpp**: Neuron dynamics tests
- **test_synapse.cpp**: Synapse properties and plasticity tests
- **test_stdp.cpp**: STDP plasticity rule tests
- **test_config.cpp**: Configuration management tests
- **test_random.cpp**: Random number generator tests
- **test_clock.cpp**: Simulation clock tests

### Test Results

All unit tests should pass after the fixes:

```
=== NLM Phase 2 Tests ===
Testing Real Neural Computation:
  - LIF Neuron Dynamics
  - Synaptic Transmission
  - STDP Plasticity

Running Types tests... PASSED
Running Config tests... PASSED
Running Random tests... PASSED
Running Clock tests... PASSED
Running Neuron tests... PASSED
Running Synapse tests... PASSED
Running STDP tests... PASSED
Running Brain tests... PASSED

=== All Tests PASSED ===
```

## Performance Considerations

### Memory Usage

NLM is designed for efficient memory usage:

- **Neuron Storage**: ~100 bytes per neuron for state and connections
- **Synapse Storage**: ~50 bytes per synapse
- **Total Estimate**: ~0.5 KB per 1000 neurons and 10000 synapses

### Simulation Speed

Performance depends on:

- **Neuron Count**: ~1000 neurons per second on modern hardware
- **Synapse Count**: ~10000 synapses per second
- **Region Count**: Minimal impact due to efficient caching

### Optimization Techniques

1. **Event-Driven Processing**: Spike events are processed only when they occur
2. **Memory Pools**: Reuse memory allocations to reduce garbage collection
3. **Parallel Processing**: Some operations can be parallelized across regions
4. **Sparse Connectivity**: Most neurons have limited connections (10-20% density)

## Scientific Background

NLM is based on several key principles:

### Leaky Integrate-and-Fire (LIF) Neurons

- Membrane potential integrates incoming currents
- Exponential decay toward resting potential
- Threshold-based spike generation
- Refractory period after spikes

### Synaptic Plasticity

- **STDP (Spike-Timing-Dependent Plasticity)**: Pre-before-post strengthens synapses
- **Hebbian Learning**: Correlated activity strengthens connections
- **Structural Plasticity**: Formation and elimination of synapses

### Neuromodulation

- **Dopamine**: Reward prediction error and reinforcement
- **Curiosity**: Novelty detection and exploration motivation
- **Novelty**: Change detection and learning triggers
- **Prediction Error**: Comparison of predicted vs. actual outcomes

### Memory Systems

- **Working Memory**: Transient active information storage
- **Episodic Memory**: Experience replay and consolidation
- **Associative Memory**: Pattern associations and recall

## Applications

### Research

- **Neural Computation**: Study of spiking neural networks
- **Machine Learning**: Neuromorphic AI and bio-inspired algorithms
- **Cognitive Modeling**: Artificial cognitive architectures
- **Developmental Psychology**: Simulation of cognitive development

### Education

- **Neuroscience**: Understanding neural dynamics and plasticity
- **AI**: Teaching neural network concepts
- **Brain-Computer Interfaces**: Research and development

### Industry

- **Gaming**: Intelligent non-player characters
- **Robotics**: Adaptive control systems
- **Simulation**: Training and testing environments

## Future Enhancements

### Planned Features

1. **Advanced Visualization**: 3D neural network visualization
2. **Multi-Modal Sensing**: Integration with vision, audio, and other sensors
3. **Social Learning**: Multi-agent systems and social dynamics
4. **Evolutionary Algorithms**: Genetic evolution of neural networks
5. **Hardware Acceleration**: GPU/TPU support for faster simulations

### Research Directions

1. **Biological Plausibility**: More realistic neuron models
2. **Scale-Up**: Larger networks for complex cognition
3. **Hybrid Systems**: Integration with traditional AI methods
4. **Real-World Applications**: Brain-computer interfaces and robotics

## Contributors

This project is maintained by the NLM Project Team. Contributions are welcome!

## License

MIT License. See `LICENSE` file for details.

## Support

For questions and support:

1. **Documentation**: See `README.md`, `HOW_TO_USE.md`, and `easy_usage.md`
2. **Examples**: Run `python nlm_advanced_examples.py --help`
3. **Community**: Join the NLM discussion forums
4. **Issues**: Report bugs and feature requests on GitHub

## Conclusion

NLM provides a comprehensive framework for neural simulation with enhanced Python bindings, command-line tools, and advanced examples. Whether you're a beginner looking to understand basic neural dynamics or a researcher exploring complex learning systems, NLM offers powerful tools for exploration and discovery.

The enhanced Python bindings make it easy to access the full power of NLM from Python, while the command-line tools and examples provide ready-to-use solutions for common tasks.

**Happy Neural Simulation!**
