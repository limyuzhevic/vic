# Python Bindings for NLM - Project Documentation

This document provides comprehensive information about the Python bindings for NLM (Neural Learning Machine).

## Overview

The Python bindings provide access to the full power of the NLM neural simulation framework through a simple, intuitive API. The bindings expose all core NLM functionality including:

- **Core Brain Systems**: Real spiking neural computation with LIF neurons, event-driven dynamics, and integrated plasticity
- **Memory Systems**: Working, episodic, and associative memory for experience storage
- **Neuromodulation**: Reward, curiosity, novelty, and prediction error signals
- **Prediction Systems**: Temporal sequence learning and action consequence prediction
- **Cognitive Systems**: Planning, attention, and concept formation
- **Development**: Developmental stages and plasticity modulation over time
- **Environment Interaction**: Complete agent-world interaction system

## Installation

### Prerequisites

- Python 3.8 or higher
- C++ compiler with C++20 support
- CMake 3.16 or higher

### Installation Methods

#### 1. From Source (Recommended)

```bash
# Clone the repository
mkdir nlm-python
cd nlm-python
# Assuming the source is in the current directory
pip install -e .
```

This will build the C++ extension and install the Python package.

#### 2. Using Conda

```bash
conda create -n nlm-env python=3.8
conda activate nlm-env
conda install -c conda-forge cmake
pip install -e .
```

#### 3. Development Installation

For development, you can install in editable mode:

```bash
pip install -e .[dev]
```

This includes development dependencies (pytest, numpy, etc.).

## Quick Start

### Basic Brain Simulation

```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Brain simulated for {step+1} steps")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent-World Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation complete!")
```

### Learning and Plasticity

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable learning systems
if brain.getSTDP():
    brain.getSTDP().enable(true)
if brain.getHebbian():
    brain.getHebbian().enable(true)

# Run learning simulation
for step in range(1000):
    brain.injectCurrentToNeurons("Sensory", 40.0)
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getTotalSpikeCount()} spikes")

print(f"Learning completed. Final firing rate: {brain.getAverageFiringRate():.4f}")
```

## API Reference

### Core Classes

#### Config
Configuration management system for brain parameters.

**Methods:**
- `createDefaultConfig()` - Create default configuration
- `loadFromFile(filepath)` - Load from JSON file
- `saveToFile(filepath)` - Save to JSON file
- `get(key)` - Get configuration value
- `getOr(key, default)` - Get with default
- `set(key, value)` - Set configuration value
- `has(key)` - Check if key exists

#### Brain
Core neural computation engine implementing real spiking neurons.

**Methods:**
- `initialize()` - Initialize brain
- `step(step_number)` - Run simulation step
- `step(step_number, time)` - Run step with time
- `receiveSensoryInput(input)` - Process sensory input
- `injectCurrent(neuron, current)` - Inject current into neuron
- `injectCurrentToNeurons(type, current)` - Inject to neuron type
- `getTotalNeuronCount()` - Get neuron count
- `getTotalSynapseCount()` - Get synapse count
- `getTotalSpikeCount()` - Get spike count
- `getAverageFiringRate()` - Get average firing rate
- `getExcitationInhibitionRatio()` - Get E/I ratio
- `getRegionCount()` - Get region count

#### AgentBrain
Interface between brain and environment.

**Methods:**
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Get motor command
- `applyRewardModulation(reward, predicted)` - Apply reward
- `getNeuromodulationLevel()` - Get neuromodulation level
- `enableRewardModulation(enable)` - Enable reward modulation

#### SimpleWorld
Environment simulation with grid-based world.

**Methods:**
- `configure(width, height, vision_width, vision_height)` - Configure world
- `reset()` - Reset world
- `update(time)` - Update world
- `getSensoryPercept()` - Get sensory percept
- `applyMotorCommand(action, time)` - Apply action

### Memory Systems

#### Working Memory
Temporary storage of active information.

**Methods:**
- `store(pattern, value)` - Store pattern-value pair
- `retrieve(pattern)` - Retrieve value for pattern
- `has(pattern)` - Check if pattern exists
- `getSize()` - Get memory size

#### Episodic Memory
Experience storage with temporal indexing.

**Methods:**
- `store(episode, timestamp)` - Store episode
- `retrieve(pattern)` - Retrieve episode
- `replay(start_time, duration)` - Replay episodes
- `getTimestampRange()` - Get time range

#### Associative Memory
Pattern association learning.

**Methods:**
- `associate(input_pattern, output_pattern, strength)` - Associate patterns
- `recall(input_pattern)` - Recall output for input
- `getStrength(input, output)` - Get association strength

### Neuromodulation Systems

#### Dopamine
Reward and reinforcement signals.

**Methods:**
- `configure(baseline, learning_rate)` - Configure dopamine system
- `process()` - Process signal
- `getLevel()` - Get signal level

#### Curiosity
Exploration motivation.

**Methods:**
- `configure(baseline, exploration_rate)` - Configure curiosity system
- `process()` - Process signal
- `getLevel()` - Get signal level

#### Novelty
Novelty detection.

**Methods:**
- `configure(decay_rate)` - Configure novelty detection
- `process()` - Process signal
- `getLevel()` - Get signal level

#### PredictionError
Error signals for learning.

**Methods:**
- `configure(sensitivity)` - Configure prediction error
- `process()` - Process signal
- `getLevel()` - Get signal level

### Cognitive Systems

#### NeuralPlanner
Predictive action selection.

**Methods:**
- `configure()` - Configure planner
- `plan(state)` - Plan actions
- `evaluate(action, state)` - Evaluate action

#### ConceptFormation
Pattern discovery without labels.

**Methods:**
- `configure()` - Configure concept formation
- `storePattern(pattern, properties)` - Store pattern
- `retrievePattern(properties)` - Retrieve pattern

#### AttentionalSelection
Competitive attention (NOT Transformer).

**Methods:**
- `configure()` - Configure attention system
- `select(state)` - Select focus
- `getFocus()` - Get current focus

### Development System

#### DevelopmentSystem
Developmental stages and plasticity modulation.

**Methods:**
- `configure()` - Configure development system
- `update(brain, timestep)` - Update development
- `getStage()` - Get current stage
- `isMature()` - Check if mature

## Command-Line Tools

The package includes command-line tools for easy experimentation:

### `nlm-run`
Run NLM simulations from command line.

**Example:**
```bash
nlm-run --steps 1000 --neurons 1000 --seed 42 --test-mode basic
```

### `nlm-explore`
Interactive exploration mode.

**Example:**
```bash
nlm-explore --mode interactive --steps 1000 --neurons 1000
```

### `nlm-demo`
Run ready-to-use demonstrations.

**Example:**
```bash
nlm-demo --demo comprehensive --steps 500 --neurons 500
```

## Testing

### Running Tests

#### Unit Tests
```bash
pytest tests/ -v
```

#### Integration Tests
```bash
python -m pytest tests/integration/ -v
```

#### Demo Tests
```bash
python -m pytest tests/demos/ -v
```

### Test Structure

The test suite includes:

1. **Core Functionality Tests**: Brain initialization, step processing, connectivity
2. **Memory System Tests**: Working, episodic, and associative memory
3. **Plasticity Tests**: STDP, Hebbian, and structural plasticity
4. **Neuromodulation Tests**: Reward, curiosity, novelty systems
5. **Prediction System Tests**: Temporal learning and prediction
6. **Development Tests**: Developmental stage progression
7. **Integration Tests**: Complete agent-world interaction
8. **Performance Tests**: Scalability and efficiency

## Advanced Usage

### Custom Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Modify parameters
config.set("random_seed", 12345)
config.set("neuron_count", 2000)
config.set("region_count", 3)
config.set("connection_probability", 0.2)
config.set("stdp_ltp_weight", 0.03)
config.set("stdp_ltd_weight", 0.028)
config.set("development_synaptogenesis_rate", 0.002)
config.set("log_to_file", true)
config.set("log_filename", "nlm_custom.log")

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()
```

### Advanced Plasticity Control

```python
# Access and control plasticity systems
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# STDP plasticity
if brain.getSTDP():
    brain.getSTDP().enable(true)
    brain.getSTDP().setLTPWeight(0.025)
    brain.getSTDP().setLTDWeight(0.023)
    brain.getSTDP().setTimeConstant(25.0)

# Hebbian plasticity
if brain.getHebbian():
    brain.getHebbian().enable(true)
    brain.getHebbian().setLearningRate(0.02)

# Structural plasticity
if brain.getStructuralPlasticity():
    brain.getStructuralPlasticity().enable(true)
    brain.getStructuralPlasticity().setSynaptogenesisRate(0.0005)
    brain.getStructuralPlasticity().setPruningRate(0.00005)
```

### Memory Management

```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Working memory
working_mem = brain.getWorkingMemory()
working_mem.store("temperature", 25.5)
working_mem.store("pressure", 101.3)
value = working_mem.retrieve("temperature")

# Episodic memory
episodic_mem = brain.getEpisodicMemory()
episodic_mem.store("morning_routine", 0.0)  # timestamp

# Associative memory
assoc_mem = brain.getAssociativeMemory()
assoc_mem.associate("hot", "summer", 0.8)
assoc_mem.associate("cold", "winter", 0.9)
```

### Development and Experimentation

```python
# Create agent with all systems enabled
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30)

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run experiment with tracking
start_time = time.time()
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Save results
if agent.getNeuromodulationLevel() > 0:
    print("Agent showed learning behavior")
```

## Performance Optimization

### Tips for Large-Scale Simulations

1. **Adjust Neuron Count**: Start with smaller networks for testing
   ```python
   config.set("neuron_count", 100)  # For testing
   config.set("neuron_count", 10000)  # For large-scale simulation
   ```

2. **Configure Logging**: Reduce logging for production runs
   ```python
   config.set("log_level", "WARNING")
   config.set("log_to_file", false)
   ```

3. **Memory Management**: Monitor memory usage
   ```python
   import psutil
   process = psutil.Process()
   print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")
   ```

4. **Parallel Processing**: Use multiple regions for parallel computation
   ```python
   config.set("region_count", 4)
   ```

## Troubleshooting

### Common Issues and Solutions

#### "ModuleNotFoundError: No module named 'pynlm'"
**Solution**: Install the package
```bash
pip install pynlm
```

#### "ImportError: libstdc++" or "libgomp" errors
**Solution**: Update system libraries
```bash
# On Linux
sudo apt-get update
sudo apt-get install libstdc++6

# On macOS
xcode-select --install
```

#### "BuildError: CMake must be installed"
**Solution**: Install CMake
```bash
# On Ubuntu/Debian
sudo apt-get install cmake

# On macOS
brew install cmake

# On Windows
choco install cmake
```

#### Slow Performance
**Solution**: Optimize configuration
```python
# Reduce complexity
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.1)
```

#### Memory Issues
**Solution**: Monitor and adjust memory usage
```python
import psutil
process = psutil.Process()
mem_mb = process.memory_info().rss / 1024 / 1024
print(f"Current memory: {mem_mb:.2f} MB")
if mem_mb > 2000:  # 2 GB
    print("Warning: High memory usage!")
```

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgements

This project builds upon years of research in computational neuroscience and artificial intelligence. Special thanks to:

- The NLM research team
- Contributors to the C++ core library
- The Python community for pybind11 and related tools
- Users who provide feedback and report issues

## Future Development

### Planned Features

1. **Enhanced Visualization**: Built-in plotting and visualization tools
2. **Distributed Computing**: Support for multi-process and multi-node simulations
3. **Cloud Integration**: Deployment to cloud platforms
4. **Web Interface**: Browser-based visualization and control
5. **Plugin System**: Extensible architecture for custom components
6. **Advanced Analytics**: Performance monitoring and analysis tools

### Research Directions

1. **Neuromorphic Computing**: Integration with specialized hardware
2. **Developmental Robotics**: Embodied learning and development
3. **Social Learning**: Multi-agent systems and social cognition
4. **Transfer Learning**: Knowledge transfer across tasks
5. **Meta-Learning**: Learning how to learn

## Contact

For questions, issues, or collaboration:

- GitHub Repository: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Issues: https://github.com/nlm-project/nlm/issues
- Discussions: https://github.com/nlm-project/nlm/discussions

## Build Information

**Package Version**: 0.1.0
**Build Date**: $(date)
**Python Version**: $(python --version)
**Compiler**: $(g++ --version | head -1)
**CMake**: $(cmake --version | head -1)

The Python bindings for NLM represent a significant step forward in making advanced neural simulation accessible to researchers, developers, and educators. The combination of powerful C++ performance with intuitive Python APIs enables both rapid prototyping and large-scale production deployments.