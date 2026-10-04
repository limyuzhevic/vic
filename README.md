# NLM (Neural Learning Machine) - Improved Version

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. This is the improved version of NLM with enhanced Python bindings, better error handling, and comprehensive documentation.

## What Makes This Improved Version

### 1. Enhanced Python Bindings (`pynlm`)

**New Features:**
- **Convenience methods**: `getNeuronCount()`, `getSynapseCount()`, `getSpikeCount()` for easier API access
- **Error handling**: Comprehensive exception handling with descriptive error messages
- **Type-safe getters**: `get()`, `getInt()`, `getFloat()` for safer configuration value retrieval
- **Better documentation**: Enhanced docstrings and parameter descriptions

**Key Improvements:**
- Added `config.get()` method for flexible value retrieval
- Added `brain.getNeuronCount()`, `brain.getSynapseCount()`, `brain.getSpikeCount()` for quick statistics
- Improved error messages for better debugging
- Enhanced type conversion safety

### 2. Fixed Bugs

**Configuration Bug Fix:**
- **Issue**: Duplicate `dopamine_baseline` entry in `configs/default.cfg` (lines 30-31)
- **Fix**: Removed duplicate entry, keeping the correct value

### 3. Improved CMake Configuration

**New Features:**
- **Position Independent Code**: Added `CMAKE_POSITION_INDEPENDENT_CODE ON` for better shared library support
- **Stricter compilation**: Added `-Werror` flag for better code quality
- **Phase 6 demo executable**: Added `nlm_phase6_demo` for comprehensive integration testing
- **Better build options**: More modern CMake configuration

### 4. Enhanced Documentation

**Comprehensive Examples:**
- `examples/basic_brain.py`: Simple brain simulation
- `examples/agent_world.py`: Agent-world interaction
- `examples/learning_experiment.py`: Advanced learning and plasticity

**Setup Guide:**
- `setup_guide.py`: Interactive installation and setup instructions

**Documentation improvements:**
- Better docstrings in Python bindings
- Enhanced configuration documentation
- More detailed error messages

### 5. Comprehensive Testing

**Python Unit Tests:**
- `tests/test_pynlm.py`: Extensive Python bindings tests
- Coverage for all major functionality
- Error handling tests
- Integration tests

**Test Features:**
- Test basic NLM operations
- Test agent-world simulation
- Test learning and plasticity
- Test error handling and edge cases

### 6. Better Error Handling

**Improved Error Handling:**
- **Config class**: Validation for keys and values
- **Python bindings**: Exception wrapping for better error reporting
- **File I/O**: Better error messages for file operations
- **Type safety**: Safer type conversions with fallbacks

### 7. Performance and Quality Improvements

**Performance Enhancements:**
- **Position Independent Code**: Better compatibility for shared libraries
- **Stricter compilation**: `-Werror` flag catches more issues
- **Modern CMake**: Better build system organization

**Code Quality:**
- **Validation**: Input validation in configuration loading
- **Error messages**: More descriptive error messages
- **Documentation**: Comprehensive docstrings and examples

## Installation

### From Source (Recommended)

1. **Install system dependencies:**
   ```bash
   # Ubuntu/Debian
   sudo apt-get update
   sudo apt-get install build-essential cmake python3-dev python3-pip
   
   # macOS
   xcode-select --install
   
   # Windows
   Install Visual Studio with C++ tools
   ```

2. **Install Python dependencies:**
   ```bash
   pip install scikit-build-core>=0.5.0 pybind11>=2.11.0 numpy>=1.20 scipy>=1.6.0
   ```

3. **Build NLM:**
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=$(which python3)
   make -j4
   cd ..
   ```

4. **Install Python module:**
   ```bash
   pip install -e .
   ```

### Using the Setup Guide

Run the setup guide for automated installation and verification:
```bash
python setup_guide.py
```

## Usage Examples

### Basic Brain Simulation

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Neurons: {brain.getNeuronCount()}")
print(f"Synapses: {brain.getSynapseCount()}")
print(f"Spikes: {brain.getSpikeCount()}")
```

### Agent in a World

```python
import pynlm

# Setup agent-world simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Learning Experiment

```python
import pynlm

# Create learning-optimized configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 300)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.curiosity.enable", True)

brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Run learning experiment
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Testing

### Run Python Tests

```bash
pytest tests/ -v
```

### Run C++ Tests

```bash
cd build && ctest --output-on-failure
```

### Run Examples

```bash
python examples/basic_brain.py
python examples/agent_world.py
python examples/learning_experiment.py
```

## Configuration

### Configuration File Format

Configuration files use a simple `key=value` format:

```
# Example configuration
brain.neuron_count = 1000
brain.synapse_density = 0.1
plasticity.stdp.enable = true
neuromod.dopamine.scale = 1.0
```

### Available Configuration Options

#### Brain Configuration
- `brain.neuron_count`: Total number of neurons
- `brain.synapse_density`: Synapse connectivity density
- `brain.connection_probability`: Probability of connection between neurons
- `brain.initial_weight_mean`: Mean initial synaptic weight
- `brain.initial_weight_std`: Standard deviation of initial weights

#### Plasticity Configuration
- `plasticity.stdp.enable`: Enable STDP
- `plasticity.stdp.learning_rate`: STDP learning rate
- `plasticity.stdp.tau_plus`: STDP time constant (ms)
- `plasticity.stdp.tau_minus`: STDP time constant (ms)
- `plasticity.hebbian.enable`: Enable Hebbian learning
- `plasticity.structural.enable`: Enable structural plasticity

#### Neuromodulation Configuration
- `neuromod.dopamine.scale`: Dopamine modulation scale
- `neuromod.curiosity.enable`: Enable curiosity-driven exploration
- `neuromod.novelty.enable`: Enable novelty detection

## API Reference

### Brain Class

#### Creation and Initialization
- `brain = pynlm.createBrain(config)`: Create brain with configuration
- `brain.initialize()`: Initialize the brain
- `brain.step(step_number)`: Run simulation step
- `brain.reset()`: Reset brain state

#### State Access
- `brain.getNeuronCount()`: Get total neuron count
- `brain.getSynapseCount()`: Get total synapse count
- `brain.getSpikeCount()`: Get total spike count
- `brain.getFiringCount()`: Get firing neuron count
- `brain.getAverageRate()`: Get average firing rate
- `brain.getEI_ratio()`: Get excitation/inhibition ratio

#### Configuration
- `brain.getConfig()`: Get configuration object

#### I/O Operations
- `brain.save("filepath")`: Save brain state
- `brain.load("filepath")`: Load brain state

### Config Class

#### Creation and Modification
- `config = pynlm.createDefaultConfig()`: Create default config
- `config.set(key, value)`: Set configuration value
- `config.loadFromFile(filepath)`: Load from file
- `config.saveToFile(filepath)`: Save to file

#### Value Access
- `config.has(key)`: Check if key exists
- `config.get(key)`: Get value as string
- `config.getInt(key)`: Get value as int
- `config.getFloat(key)`: Get value as float
- `config.getKeys()`: Get all configuration keys

#### Operations
- `config.clear()`: Clear all configuration
- `config.summary()`: Get configuration summary

### AgentBrain Class

#### Creation and Initialization
- `agent = pynlm.createAgentBrain(brain)`: Create agent interface
- `agent.initialize(world)`: Initialize with world

#### Sensory Processing
- `agent.processSensoryInput(percept)`: Process sensory input

#### Motor Output
- `action = agent.decodeMotorCommand()`: Get motor action

#### Neuromodulation
- `agent.applyRewardModulation(reward, predicted)`: Apply reward modulation
- `agent.updateDevelopment(timestep)`: Update development

#### Subsystem Control
- `agent.enableRewardModulation(enabled)`: Enable/disable reward modulation
- `agent.enableStructuralPlasticity(enabled)`: Enable/disable structural plasticity
- `agent.enableDevelopment(enabled)`: Enable/disable development
- `agent.enableCuriosity(enabled)`: Enable/disable curiosity

#### Statistics
- `agent.getCuriosityLevel()`: Get curiosity level
- `agent.getNoveltyLevel()`: Get novelty level
- `agent.getPredictionError()`: Get prediction error
- `agent.getNeuromodulationLevel()`: Get neuromodulation level

### SimpleWorld Class

#### Creation and Configuration
- `world = pynlm.createSimpleWorld()`: Create world
- `world.configure(width, height, visionWidth, visionHeight)`: Configure world

#### Control
- `world.reset()`: Reset world
- `world.update(timestep)`: Update world
- `world.setAgentStart(x, y)`: Set agent start position

#### State Access
- `percept = world.getSensoryPercept()`: Get sensory percept
- `body = world.getAgentBody()`: Get agent body state

#### Actions
- `world.applyMotorCommand(action, time)`: Apply motor command

## Building from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+
- **pybind11**: Version 2.11.0+

### Build Commands

```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4

# Run tests (if built)
ctest --output-on-failure
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase6_demo` | Executable | Phase 6 integration test |

## Development

### Python Development

To develop Python bindings:

1. Make changes to `python/bindings.cpp`
2. Reinstall the module in development mode:
   ```bash
   pip install -e .
   ```
3. Run tests to verify changes:
   ```bash
   pytest tests/test_pynlm.py -v
   ```

### C++ Development

To develop C++ core:

1. Make changes to C++ source files in `src/`
2. Rebuild:
   ```bash
   cd build
   make -j4
   ```
3. Run C++ tests:
   ```bash
   ctest --output-on-failure
   ```

## Troubleshooting

### Common Issues

**"ImportError: No module named 'pynlm'":**
```bash
pip install --force-reinstall .
```

**"Segmentation faults":**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

**"CMake cannot find Python":**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python3)
```

**"Build errors with CMake":**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

## Performance Tips

### For Large Simulations

1. **Increase neuron count**: More neurons for complex behavior
2. **Enable optimizations**: Compile with `-O3` or `-Ofast`
3. **Use checkpointing**: Save state periodically for long simulations
4. **Parallel processing**: Use multiple CPU cores

### For Real-time Applications

1. **Reduce timestep**: Smaller timesteps for smoother updates
2. **Limit simulation steps**: Run fewer but more meaningful steps
3. **Optimize configurations**: Use performance-tuned parameters

## Future Enhancements

This improved version includes several areas for future enhancement:

### 1. Advanced Python Features
- **Interactive notebooks**: Jupyter notebook examples
- **Visualization**: Real-time brain activity plotting
- **Machine learning integration**: Connect with scikit-learn and TensorFlow

### 2. Extended Functionality
- **Advanced learning algorithms**: Deep learning integration
- **Multi-agent systems**: Support for multiple interacting agents
- **Custom neural architectures**: User-defined network topologies

### 3. Documentation and Examples
- **Comprehensive tutorials**: Step-by-step learning guides
- **API documentation**: Auto-generated documentation
- **Use case examples**: Real-world application examples

### 4. Performance Optimization
- **GPU acceleration**: CUDA support for faster computation
- **Memory optimization**: More efficient data structures
- **Adaptive algorithms**: Dynamic parameter tuning

## License

MIT License. See `LICENSE` file for details.

## Authors

Research project — See `docs/` for scientific background.

---

**Version**: 0.1.0 Improved
**Last Updated**: October 2026
**Status**: Active Development

This improved version of NLM provides a more robust, user-friendly, and well-documented experience for both beginners and advanced users of neural simulation technology.