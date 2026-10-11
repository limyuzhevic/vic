# NLM Framework - Comprehensive Documentation

## Overview

NLM (Neural Learning Machine) is an experimental artificial developmental brain project that implements real spiking neural computation with event-driven dynamics, plasticity, and learning systems. This document provides comprehensive guidance on using the enhanced NLM framework.

## Installation

### Prerequisites

- CMake 3.20 or higher
- C++20 compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)
- Python 3.8+ with pybind11
- Optional: NumPy for Python examples

### Building from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with options
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DNLM_BUILD_PYTHON_BINDINGS=ON \
    -DNLM_BUILD_TESTS=ON \
    -DNLM_BUILD_EXAMPLES=ON \
    -DNLM_ENABLE_PERFORMANCE_OPTIMIZATIONS=ON \
    -DNLM_ENABLE_DEBUG_FEATURES=OFF

# Build
make -j$(nproc)

# Install (optional)
sudo make install
```

### Python Installation

```bash
# Install Python package (if available)
pip install pynlm

# Or build from source
pip install -e .
```

## Quick Start Guide

### Basic Python Usage

```python
import pynlm as nlm

# Create default configuration
config = nlm.createDefaultConfig()

# Create brain with configuration
brain = nlm.createBrain(config)

# Initialize brain
success = brain.initialize()
if not success:
    print("Failed to initialize brain")
    exit(1)

# Run simulation
for step in range(1000):
    brain.step(step, step * 0.001)

# Get statistics
stats = brain.get_statistics()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Total synapses: {stats['total_synapses']}")
print(f"Total spikes: {stats['total_spikes']}")
```

### Advanced Python API

```python
import pynlm as nlm
import numpy as np

# Create enhanced analyzer
analyzer = nlm.NLMAnalyzer("complex")

# Get comprehensive network analysis
stats = analyzer.get_network_statistics()
print("Network Statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")

# Run learning experiment
experiment_results = analyzer.create_learning_experiment({
    "duration": 2000,
    "pattern_size": 30,
    "learning_rate": 0.02,
})

# Run agent simulation
agent_results = analyzer.run_agent_simulation(500)
print(f"Agent simulation completed: {agent_results['total_steps']} steps")
print(f"Average reward: {agent_results['average_reward']:.2f}")
```

## Configuration System

### Configuration Options

The NLM configuration system supports multiple parameter types:

```python
import pynlm as nlm

# Create and configure brain
config = nlm.createDefaultConfig()

# Set various configuration options
config.set("neuron_count", 1000)                    # Integer
config.set("random_seed", 42)                      # Integer  
config.set("simulation_timestep", 0.001)           # Double
config.set("connection_probability", 0.15)         # Float
config.set("learning_enabled", True)               # Boolean
config.set("debug_mode", False)                    # Boolean
config.set("model_name", "my_neural_model")        # String
config.set("int_list", [1, 2, 3])                  # Integer list
config.set("float_list", [0.1, 0.2, 0.3])          # Float list

# Load configuration from file
success = config.loadFromFile("configs/my_config.cfg")

# Save configuration to file
success = config.saveToFile("configs/saved_config.cfg")
```

### Preset Configurations

The framework provides several preset configurations:

```python
# Simple configuration (small network, basic settings)
config = nlm.createPresetConfig("simple")

# Complex configuration (large network, advanced settings)
config = nlm.createPresetConfig("complex")

# High plasticity configuration (enhanced learning)
config = nlm.createPresetConfig("high_plasticity")
```

## Python API Reference

### Core Classes

#### Brain
Represents the central neural simulation brain class.

**Constructor:**
```python
brain = nlm.createBrain(config)  # Using factory function
brain = nlm.Brain(config)        # Direct constructor
```

**Key Methods:**
- `initialize()` - Initialize the brain with configuration
- `step(step, time)` - Perform simulation step
- `get_statistics()` - Get brain statistics as dictionary
- `get_region_count()` - Get number of regions
- `get_total_neuron_count()` - Get total neuron count
- `get_total_synapse_count()` - Get total synapse count

**Pythonic Features:**
- `inject_current_to_all(current)` - Inject current into all neurons
- `analyze_network_topology()` - Analyze network topology
- `generate_network_report()` - Generate comprehensive network report
- `create_learning_experiment(config)` - Run learning experiment

#### Config
Configuration class for NLM system.

**Key Methods:**
- `loadFromFile(filepath)` - Load from JSON file
- `loadFromArgs(argc, argv)` - Load from command line
- `saveToFile(filepath)` - Save to JSON file
- `has(key)` - Check if key exists
- `getKeys()` - Get all configuration keys
- `set(key, value, source)` - Set configuration value

#### SimpleWorld
Simple 2D world for NLM simulation.

**Key Methods:**
- `configure(width, height, vision_width, vision_height)` - Configure world
- `reset()` - Reset world state
- `update(timestep)` - Update world state
- `applyMotorCommand(command, time)` - Apply motor command
- `getAgentBody()` - Get agent body state

#### AgentBrain
Agent brain interface connecting NLM brain to world.

**Key Methods:**
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory percept
- `decodeMotorCommand()` - Decode motor command
- `get_brain_state()` - Get current brain state

### Factory Functions

```python
# Create default configuration
config = nlm.createDefaultConfig()

# Create brain
brain = nlm.createBrain(config)

# Create simple world
world = nlm.createSimpleWorld()

# Create agent brain
agent_brain = nlm.createAgentBrain(brain)

# Create vision input
vision = nlm.Vision(320, 240, 3)

# Create audio input
audio = nlm.Audio(44100, 1024)
```

## Examples Directory

The repository includes several example files:

### `nlm_examples.py`
Comprehensive examples demonstrating both basic and advanced usage.

**Running Examples:**
```bash
python nlm_examples.py
```

### `basic_examples.py` 
Basic examples for beginners.

### `advanced_examples.py`
Advanced examples showcasing sophisticated features.

### `agent_examples.py`
Examples focused on agent-based simulations.

### `learning_examples.py`
Examples demonstrating learning and plasticity.

## Building Examples

### Build Instructions

```bash
# In the build directory
make

# Or build with specific options
cmake .. -DNLM_BUILD_EXAMPLES=ON -DNLM_ENABLE_PERFORMANCE_OPTIMIZATIONS=ON
make
```

### Running Examples

```bash
# Run the main example executable
./nlm

# Run test executables
./test_brain
./test_neuron
./test_config

# Run Python examples
python nlm_examples.py
```

## Configuration Files

### Default Configuration (`configs/default.cfg`)

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.15,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001
}
```

### Example Configuration (`configs/example.cfg`)

```json
{
    "random_seed": 123,
    "simulation_timestep": 0.0005,
    "neuron_count": 1000,
    "region_count": 3,
    "connection_probability": 0.2,
    "stdp_ltp_weight": 0.03,
    "stdp_ltd_weight": 0.025,
    "stdp_tau": 30.0,
    "enable_plasticity": true,
    "enable_development": true,
    "enable_curiosity": true
}
```

## Configuration File Format

Configuration files are JSON format with the following supported types:

```json
{
    "integer_value": 42,
    "double_value": 3.14159,
    "float_value": 2.5,
    "boolean_value": true,
    "string_value": "my_model",
    "integer_list": [1, 2, 3, 4, 5],
    "float_list": [0.1, 0.2, 0.3, 0.4, 0.5],
    "string_list": ["model1", "model2", "model3"]
}
```

## Command Line Interface

### Command Line Arguments

The NLM executables accept various command line arguments:

```bash
./nlm --help
# Shows all available options

./nlm --config my_config.cfg
# Load configuration from file

./nlm --neuron_count 1000 --connection_probability 0.2
# Override configuration values

./nlm --random_seed 123 --simulation_timestep 0.0005
# Set specific parameters
```

### Environment Variables

```bash
export NLM_LOG_LEVEL=INFO
export NLM_ENABLE_PROFILING=true
export NLM_DEBUG_MODE=false
```

## Testing and Validation

### Running Tests

```bash
# Build and run all tests
make test

# Run specific test suite
cd build && ./test_brain
./test_neuron
./test_config
./test_clock
./test_stdp
./test_synapse
./test_types
```

### Test Coverage

The framework includes comprehensive tests for:

- Brain creation and initialization
- Neuron dynamics and LIF simulation
- Synapse formation and plasticity
- Configuration management
- Plasticity rules (STDP, Hebbian, structural)
- Memory systems
- Neuromodulation
- Development stages
- Agent behavior
- World interaction

### Performance Testing

```bash
# Build with performance optimizations
cmake .. -DNLM_ENABLE_PERFORMANCE_OPTIMIZATIONS=ON
make

# Run performance benchmarks
./benchmark_nlm
```

## Performance Optimization

### Build Options

The framework supports several performance optimization options:

```bash
# Enable all performance optimizations
cmake .. -DNLM_ENABLE_PERFORMANCE_OPTIMIZATIONS=ON

# Enable debug features for development
cmake .. -DNLM_ENABLE_DEBUG_FEATURES=ON

# Disable Python bindings (smaller build)
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=OFF
```

### Modern C++ Features

The framework utilizes:

- C++20 standard with modules
- SIMD vectorization where possible
- Memory pools for efficiency
- Event-driven architecture
- Cache-friendly data layouts

### Python Performance

For optimal Python performance:

```python
import pynlm as nlm

# Use efficient data structures
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)

# Batch operations where possible
for step in range(0, 10000, 100):
    # Process in chunks for better performance
    for i in range(100):
        brain.step(step + i, (step + i) * 0.001)
    
    # Get statistics periodically
    stats = brain.get_statistics()
```

## Extensibility and Customization

### Adding New Neuron Types

```cpp
// Create new neuron type class
class MyNeuron : public Neuron {
public:
    MyNeuron(NeuronId id) : Neuron(id) {}
    
    // Override virtual methods
    void stepLIF(Timestamp time, TimestepDuration dt) override;
    
    // Add custom functionality
    void setCustomParameter(float value);
    float getCustomParameter() const;
};

// Register with Python bindings
py::class_<MyNeuron, std::unique_ptr<MyNeuron>, py::base<Neuron>>(m, "MyNeuron")
    .def(py::init<NeuronId>())
    .def("set_custom_parameter", &MyNeuron::setCustomParameter)
    .def("get_custom_parameter", &MyNeuron::getCustomParameter);
```

### Adding New Plasticity Rules

```cpp
// Create new plasticity rule
class MyPlasticityRule : public PlasticityRule {
public:
    void update(Neuron* pre, Neuron* post, float dt) override;
    
    // Custom parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
};

// Register with Brain
brain->addPlasticityRule(std::make_unique<MyPlasticityRule>());
```

### Custom Visualization

```cpp
#include "visualization/VisualizationInterface.hpp"

class CustomVisualizer : public VisualizationInterface {
public:
    void update(const Brain& brain, const World& world) override;
    void render() override;
    void saveScreenshot(const std::string& filename) override;
};

// Integrate with world
world.setVisualizer(std::make_unique<CustomVisualizer>());
```

## Advanced Features

### Neuromodulation Integration

```python
# Apply neuromodulatory signals
brain.applyNeuromodulation(neuromodulator)

# Access neuromodulation levels
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionErrorSignal()

# Enable/disable neuromodulation
brain.enableRewardModulation(True)
brain.enableCuriosity(True)
brain.enableNovelty(True)
```

### Memory Systems

```python
# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Get memory statistics
memory_stats = brain.get_memory_stats()
```

### Development Stages

```python
# Get developmental stage
stage = brain.getDevelopmentalStage()
print(f"Current stage: {stage}")

# Check if developing
if brain.is_developing():
    print("Brain is in developmental stage")

# Set developmental stage
brain.setDevelopmentalStage(nlm.DevelopmentalStage.Adult)
```

### Advanced Plasticity Control

```python
# Set plasticity parameters
brain.set_plasticity_parameters({
    "stdp_ltp_weight": 0.05,
    "stdp_ltd_weight": 0.04,
    "stdp_tau": 25.0,
    "enable_hebbian": True,
    "enable_stdp": True,
    "enable_structural": True,
})

# Run plasticity analysis
analysis_results = brain.run_plasticity_experiment({
    "duration": 2000,
    "pattern_size": 50,
})
```

## Troubleshooting

### Common Issues

#### Python Import Errors

```bash
# Install dependencies
pip install numpy matplotlib

# Check pybind11 installation
python -c "import pybind11; print(pybind11.__version__)"
```

#### Build Errors

```bash
# Check CMake version
cmake --version

# Ensure you have required dependencies
apt-get install build-essential cmake libpython3-dev python3-dev
```

#### Runtime Errors

```python
import pynlm as nlm

# Debug brain initialization
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)

# Enable logging
import logging
logging.basicConfig(level=logging.DEBUG)

# Check configuration
print(f"Neuron count: {config.get('neuron_count', 'not set')}")
print(f"Has random seed: {config.has('random_seed')}")
```

### Debugging Techniques

```python
# Use Python's interactive debugger
import pdb
pdb.set_trace()

# Add print statements for debugging
stats = brain.get_statistics()
print(f"DEBUG: Total neurons = {stats['total_neurons']}")
print(f"DEBUG: Total synapses = {stats['total_synapses']}")

# Use custom logging
import logging
logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)
```

## Migration Guide

### From Phase 1 to Phase 6

The NLM framework has evolved significantly from Phase 1 to Phase 6. Key changes include:

1. **Enhanced API**: The Python bindings have been completely rewritten with more Pythonic interfaces.

2. **New Configuration System**: Replaced simple string-based configuration with comprehensive typed configuration.

3. **Memory Integration**: Added comprehensive memory systems (working, episodic, associative).

4. **Neuromodulation**: Integrated sophisticated neuromodulation systems.

5. **Development**: Added multi-stage developmental system.

6. **Performance**: Significant performance improvements with modern C++ optimizations.

### Migration Examples

```python
# OLD: Phase 1 style
brain = nlm.Brain()
brain.initialize()
for step in range(1000):
    brain.step(step)

# NEW: Phase 6 style
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
success = brain.initialize()
if success:
    stats = brain.get_statistics()
    print(f"Network size: {stats['total_neurons']} neurons")
    
    # Run advanced analysis
    analysis = brain.analyze_network_topology()
    report = brain.generate_network_report()
```

## Future Directions

### Planned Enhancements

1. **Real-time Visualization**: Live plotting of neural activity
2. **Distributed Computing**: Support for multi-GPU and distributed simulations
3. **Model Zoo**: Pre-trained models and example configurations
4. **Mobile Support**: ARM architecture optimizations
5. **Web Integration**: Web-based visualization and interaction

### Research Integration

The framework is designed to support:

- Computational neuroscience research
- Artificial intelligence development
- Machine learning experimentation
- Cognitive modeling
- Neuromorphic computing research

## License and Credits

### License
MIT License. See `LICENSE` file for details.

### Authors
Research project - See docs for scientific background.

### Contributors
The NLM framework is developed by a team of researchers and engineers dedicated to creating artificial neural systems that learn from experience.

## Support and Community

### Documentation
- Online documentation at `docs/`
- API reference in this document
- Examples in `examples/` directory

### Support
For issues and support, please:
1. Check the documentation
2. Review examples
3. Create an issue on GitHub
4. Join the discussion forum

### Getting Help
```bash
# Check build logs for errors
make 2>&1 | tail -50

# Check Python installation
python -c "import pynlm; print(pynlm.__version__)"

# Get help with specific functions
help(pynlm.Brain)
help(pynlm.createBrain)
```

## Conclusion

The NLM framework provides a comprehensive platform for developing and experimenting with artificial neural systems. The enhanced Python API makes it accessible to researchers, developers, and students while maintaining the powerful capabilities needed for advanced neural simulation research.

Key strengths:

- **Comprehensive API**: Extensive Python bindings with Pythonic interfaces
- **Flexible Configuration**: Typed configuration system with preset options
- **Advanced Features**: Plasticity, memory, neuromodulation, development
- **Performance**: Modern C++ optimizations and efficient Python integration
- **Documentation**: Extensive documentation and examples
- **Extensibility**: Easy to extend with custom components

The framework is ready for immediate use and provides a solid foundation for neural system research and development.

---

*Document generated: 2026-10-11*
*Version: 0.1.0 (Phase 6)*
*For the latest updates and documentation, visit the NLM GitHub repository.*