# NLM (Neural Learning Machine) - Python Bindings and Tools

**An Experimental Artificial Developmental Brain** with comprehensive Python interface

## Overview

This repository provides a complete Python ecosystem for working with NLM (Neural Learning Machine) - a neural system that learns from experience through spiking neural networks. The Python bindings and tools provide an intuitive, powerful interface for brain simulation, visualization, and experimentation.

## Key Improvements

### 1. Enhanced Python Bindings (`python/bindings.cpp`)

#### Original Limitations:
- Basic C++ to Python bindings only
- No context managers
- Limited error handling
- No Pythonic convenience methods
- No visualization support

#### Improvements Made:

##### **Context Managers**
```python
# Context manager for automatic reset
with brain.create_default_config() as config:
    brain.initialize()
    brain.step(10)
# Automatically resets on exit
```

##### **Property-Based Access**
```python
# Pythonic property access
brain.neuron_count        # Get total neurons
brain.synapse_count       # Get total synapses  
brain.active_neuron_count # Get active neurons
brain.avg_firing_rate    # Get average firing rate
brain.e_i_ratio          # Get excitation/inhibition ratio
brain.total_spikes       # Get total spike count
```

##### **Convenience Methods**
```python
# Run multiple steps with checkpointing
brain.run_steps(100, auto_save=True, checkpoint="save_point.bin")

# Get comprehensive statistics
stats = brain.get_brain_stats()  # Returns tuple of all metrics

# Batch operations
brain.batch_inject_current(NeuronType.Excitatory, 0.5)

# Visualization helper
brain.visualize("brain_viz.png")
```

##### **Enhanced AgentBrain Interface**
```python
# Context manager for agent operations
with agent.create_agent_brain(brain) as agent_brain:
    agent_brain.initialize(world)
    # Automatic cleanup on exit
```

### 2. Advanced Python Tools

#### Command Line Interface (`scripts/nlm_cli.py`)

**Original:** Simple Python script with basic functionality

**Improved:** Comprehensive CLI with 10+ commands:

##### **Core Commands:**
- `nlm create-config` - Create brain configuration files
- `nlm run` - Run brain simulations with verbose output
- `nlm visualize` - Generate brain state visualizations
- `nlm benchmark` - Performance benchmarking tools
- `nlm explore` - Interactive brain exploration

##### **Advanced Commands:**
- `nlm visualize-dashboard` - Create comprehensive dashboards
- `nlm create-graph` - Generate network and activity graphs
- `nlm run-phase6` - Run Phase 6 integration experiments
- `nlm experiments` - Full experiment management system

#### Experiment Framework (`scripts/experiments.py`)

**Original:** Simple C++ tests

**Improved:** Full experiment management:

##### **Features:**
- **Experiment Configuration**: Structured experiment definitions with all parameters
- **Parallel Execution**: Multi-threaded experiment running
- **Checkpointing**: Automatic and manual checkpoint saving
- **Performance Monitoring**: Resource usage tracking
- **Result Analysis**: Comprehensive statistics and trend analysis
- **Comparison Tools**: Compare multiple experiment results
- **Phase 6 Integration**: Specialized Phase 6 experiment runner

#### Visualization Tools (`scripts/visualization.py`)

**Original:** No visualization support

**Improved:** Four visualization modules:

##### **BrainVisualizer**
- Comprehensive dashboard creation
- Network topology visualization
- Activity heatmap generation
- Population distribution charts
- Development timeline tracking
- Memory systems monitoring

##### **SimplePlotter**
- Activity over time plots
- Region comparison charts
- Neural network visualizations

##### **GraphRenderer**
- Network topology graphs
- Connection strength heatmaps
- Multi-graph visualization system

### 3. Package Structure

#### Enhanced Package Layout (`python/`

**Before:**
```
python/
  bindings.cpp
  CMakeLists.txt
```

**After:**
```
python/
  __init__.py              # Package initialization
  pynlm.py                 # Module with all bindings
  bindings.cpp             # C++ bindings implementation
  CMakeLists.txt           # Build configuration
```

#### Enhanced Package Metadata (`pyproject.toml`)

**Before:**
- Basic dependencies: scikit-build-core, pybind11
- Minimal optional dependencies
- No development tools

**After:**
- **Core dependencies**: numpy, matplotlib, pandas, scipy
- **Development dependencies**: pytest, black, flake8, mypy, sphinx
- **Experimental dependencies**: plotly, seaborn, jupyter
- **Performance dependencies**: numba, cython
- **Modern packaging**: Black formatting, mypy type checking
- **Comprehensive metadata**: All relevant classifiers

### 4. Pythonic API Design

#### Convenience Functions

```python
# Quick configuration creation
config = nlm.create_config(neuron_count=1000, region_count=4)

# Quick simulation
brain = nlm.run_brain_simulation("config.json", steps=500)

# Quick visualization
nlm.visualize_brain_state("checkpoint.bin", "output.png")

# Interactive exploration
nlm.interactive_explore("config.json", steps=2000)
```

#### Type Hints and Documentation

- Comprehensive type annotations
- Pythonic docstrings
- Interactive help support
- IDE autocomplete support

### 5. Performance and Usability Improvements

#### Performance Features:
- **Batch Operations**: Process multiple neurons/synapses efficiently
- **Memory Management**: Context managers for automatic cleanup
- **Parallel Processing**: Multi-threaded experiment execution
- **Optimized APIs**: Reduced Python overhead

#### Usability Features:
- **Intuitive Naming**: Pythonic method and variable names
- **Error Handling**: Comprehensive error messages and recovery
- **Progress Tracking**: Real-time progress monitoring
- **Help System**: Built-in help and documentation
- **Configuration Management**: JSON-based configuration files
- **Cross-Platform**: Works on Windows, macOS, Linux

### 6. Advanced Features for Power Users

#### Event-Driven Architecture:
- Callbacks for simulation events
- Custom visualization plugins
- Experiment hooks and triggers

#### Integration Points:
- Jupyter notebook support
- Web-based visualization
- REST API endpoints
- Database integration

#### Debugging and Monitoring:
- Real-time logging
- Performance profiling
- Memory usage tracking
- Network monitoring

## Installation and Usage

### Installation

```bash
# Install with all dependencies
pip install -e .

# Install with experimental tools
pip install -e .[experimental]

# Install with performance optimizations
pip install -e .[perf]

# Install development version
pip install -e .[dev]
```

### Quick Start

```python
import nlm

# Create a brain configuration
config = nlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 4)

# Create and initialize brain
brain = nlm.createBrain(config)

# Run simulation
brain.initialize()
for step in range(100):
    brain.step(step)

# Get statistics
stats = brain.get_brain_stats()
print(f"Neurons: {stats[0]}, Synapses: {stats[1]}")

# Create visualization
brain.visualize("my_brain.png")
```

### Command Line Interface

```bash
# Create configuration
nlm create-config --neurons 1000 --regions 4 --excitatory-ratio 0.85

# Run simulation
nlm run --config brain_config.json --steps 500 --checkpoint save_point.bin

# Visualize brain state
nlm visualize --checkpoint brain_state.bin --output viz.png

# Benchmark performance
nlm benchmark --neurons 2000 --duration 30 --output results.json

# Interactive exploration
nlm explore --steps 1000 --auto-save 200

# Create comprehensive dashboard
nlm visualize-dashboard --output dashboard.png

# Run Phase 6 integration
nlm run-phase6

# Experiment management
nlm experiments create --name my_exp --description "My experiment"
nlm experiments run --name my_exp
nlm experiments list
nlm experiments analyze --name my_exp --type comprehensive
```

## Configuration

### Configuration Files

Configurations are stored as JSON files with the following structure:

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 4,
        "excitatory_ratio": 0.8,
        "connection_probability": 0.1,
        "plasticity_enabled": true,
        "development_enabled": true,
        "novelty_enabled": true,
        "curiosity_enabled": true
    },
    "world": {
        "width": 100.0,
        "height": 100.0,
        "max_energy": 100.0,
        "energy_decay_rate": 0.001,
        "agent_start_x": 50.0,
        "agent_start_y": 50.0
    }
}
```

### Experiment Configuration

```json
{
    "name": "example_experiment",
    "description": "Example experiment for demonstration",
    "brain_config": {...},
    "world_config": {...},
    "duration": 200,
    "steps_per_iteration": 10,
    "save_checkpoints": true,
    "checkpoint_interval": 50,
    "metrics_to_record": ["activity_level", "plasticity_change", "memory_utilization"],
    "parallel_runs": 1,
    "max_memory_usage": 2048.0
}
```

## Development and Testing

### Running Tests

```bash
# Run all tests
pytest

# Run specific test module
pytest tests/test_brain.py

# Run with coverage
pytest --cov=nlm

# Run benchmarks
nlm benchmark --neurons 1000 --duration 10
```

### Code Quality

```bash
# Format code with Black
black python/ scripts/

# Check code style with Flake8
flake8 python/ scripts/

# Type checking with MyPy
mypy python/ scripts/

# Generate documentation
sphinx-build docs/ docs/_build/
```

### Building from Source

```bash
# Build with CMake
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python package
pip install -e .
```

## API Reference

### Core Classes

#### Config
- `loadFromFile(filepath)` - Load configuration from JSON file
- `saveToFile(filepath)` - Save configuration to JSON file
- `set(key, value, source)` - Set configuration value
- `get(key)` - Get configuration value
- `has(key)` - Check if key exists
- `summary()` - Get configuration summary

#### Brain
- `initialize()` - Initialize the brain
- `step(currentStep)` - Perform simulation step
- `step(currentStep, currentTime)` - Perform simulation step with timestamp
- `receiveSensoryInput(input)` - Inject sensory input
- `injectCurrent(neuron, current)` - Inject current into neuron
- `produceAction()` - Produce motor action
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state
- `addRegion(name)` - Add new region
- `getRegion(id)` - Get region by ID
- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSynapseCount()` - Get total synapse count
- `getAverageFiringRate()` - Get average firing rate
- `logStatus()` - Log brain status

#### AgentBrain
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, predictedReward)` - Apply reward modulation
- `reset()` - Reset agent
- `getBrain()` - Get underlying brain

### Utility Functions

- `createDefaultConfig()` - Create default configuration
- `createBrain(config)` - Create brain with configuration
- `createSimpleWorld()` - Create simple world
- `createAgentBrain(brain)` - Create agent brain interface

### Convenience Functions

- `create_config(neuron_count, region_count, excitatory_ratio, output_file)` - Quick config creation
- `run_brain_simulation(config_file, steps, verbose)` - Quick simulation
- `visualize_brain_state(checkpoint_file, output_file)` - Quick visualization
- `interactive_explore(config_file, steps)` - Interactive exploration

## Examples and Tutorials

### Basic Usage
See `examples/` directory for detailed examples and tutorials.

### Advanced Usage
See `examples/advanced/` for advanced use cases and integration examples.

## Troubleshooting

### Common Issues

#### Import Error
```bash
# Solution: Ensure package is installed
pip install -e .
```

#### Missing Dependencies
```bash
# Solution: Install with extra dependencies
pip install -e .[experimental]
```

#### Build Errors
```bash
# Solution: Ensure CMake is installed
cmake --version
```

### Getting Help

- **Documentation**: https://nlm.readthedocs.io
- **Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Examples**: Check `examples/` directory

## License

MIT License. See `LICENSE` file for details.

## Authors

Research project — See docs for scientific background.

## Acknowledgements

This project builds on extensive research in neural computation and artificial developmental systems. Special thanks to the research community and contributors.

## Version Information

- **Python Bindings Version**: 0.1.0
- **Build System**: CMake 3.16+
- **Python API**: Python 3.8+
- **Dependencies**: numpy, matplotlib, pandas, scipy