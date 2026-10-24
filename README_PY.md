# NLM Python Package

**NLM (Neural Learning Machine) Python Package** - A comprehensive Python interface for the NLM neural simulation framework.

![Python Package](https://img.shields.io/badge/python-3.8%2B-blue.svg)
![License](https://img.shields.io/badge/MIT-license-blue.svg)
![Status](https://img.shields.io/badge/Phase%206%20Complete-green.svg)

## Overview

The NLM Python package provides a Python interface to the Neural Learning Machine - an experimental artificial developmental brain system. NLM is a comprehensive neural simulation framework that implements real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

### Key Features

- **Real Neural Computation**: Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation
- **Complete Brain Integration**: Phase 6 systems including memory, prediction, cognition, and neuromodulation
- **High-Level API**: Pythonic interface for easy neural simulation
- **Command-Line Interface**: Built-in CLI for running simulations and experiments
- **Development Tools**: Comprehensive tools for research and experimentation
- **Production Ready**: Thoroughly tested and documented

## Installation

### System Requirements
- **Python**: 3.8 or higher
- **C++ Compiler**: C++20 support (for core functionality)
- **CMake**: 3.16 or higher (for building)

### Installation Commands

```bash
# From source
cd /workspace/app
mkdir -p build
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
pip install .

# In development mode
pip install -e .

# Quick test (limited functionality)
pip install --no-deps -e .
```

### Verify Installation

```bash
# Test basic functionality
python -c "import nlm; print('NLM version:', nlm.__version__)"

# Run demo simulation
python -m nlm demo
```

## Usage Examples

### 1. Quick Start

```python
import nlm

# Create simulation with fluent API
simulation = nlm.NLMSimulation().quick_start(
    neurons=1000,
    width=800,
    height=600,
    timestep=0.001,
    seed=42
)

# Run simulation
results = simulation.run_simulation(1000, verbose=True)
```

### 2. Basic Brain Simulation

```python
import nlm

# Create and initialize brain
brain = nlm.Brain()
brain.initialize()

# Run simulation
for step in range(100):
    brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 20.0)
    brain.step(step, step * 0.001)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### 3. Agent Simulation

```python
import nlm

# Create agent in world
simulation = nlm.NLMSimulation().quick_start()
simulation.world.configure(800, 600, 40, 40)
simulation.world.setAgentStart(100, 100)

# Add world objects
simulation.world.add_resource_object(200, 200, 1.0)
simulation.world.add_hazard_object(600, 400, -1.0)

# Run simulation
results = simulation.run_simulation(2000, steps_per_episode=200)
```

### 4. Learning Experiment

```python
import nlm

# Create learning simulation
simulation = nlm.NLMSimulation().quick_start(neurons=500)

# Run learning experiment
results = simulation.run_learning_experiment(
    experiences=10,
    steps_per_experiment=200,
    sensory_patterns=True
)

if results['learning_detected']:
    print(f"✓ Learning successful! Weight change: {results['weight_change']:.4f}")
```

### 5. Experiment Framework

```python
import nlm

# Create experiment
experiment = nlm.NLMExperiment("Neural Plasticity Study")

# Add parameter sets
param_sets = [
    {"random_seed": 42, "neuron_count": 100},
    {"random_seed": 43, "neuron_count": 200},
    {"random_seed": 44, "neuron_count": 300},
]

# Run parameter sweep
results = experiment.run_parameter_sweep(param_sets, 100)

# Analyze results
for i, result in enumerate(results):
    print(f"Parameter set {i+1}: {result['standard_results']['final_spike_count']} spikes")
```

## Command-Line Interface

The NLM package includes a comprehensive command-line interface:

```bash
# Available commands
python -m nlm demo           # Run demonstration simulation
python -m nlm agent          # Run agent simulation
python -m nlm learning       # Run learning experiment
python -m nlm brain-test     # Test brain functionality
python -m nlm config-test    # Test configuration system
python -m nlm experiment     # Run parameter sweep experiment
python -m nlm plotting       # Demonstrate plotting tools
python -m nlm developer      # Show developer tools

# Get help for specific commands
python -m nlm help <command>

# All commands help
python -m nlm help
```

### Command Examples

```bash
# Demo simulation
python -m nlm demo

# Agent simulation with custom settings
python -m nlm agent --steps 1000 --seed 42 --width 800 --height 600 --start-x 100 --start-y 100

# Learning experiment
python -m nlm learning --experiences 5 --steps-per-experience 200 --seed 42

# Brain functionality test
python -m nlm brain-test

# Configuration system test
python -m nlm config-test

# Parameter sweep experiment
python -m nlm experiment --steps 500
```

## API Reference

### Core Classes

#### Brain
Main neural simulation brain class with comprehensive functionality.

**Key Methods**:
- `initialize()` - Initialize brain with configuration
- `step(step, time)` - Perform simulation step
- `injectCurrent(neuron, current)` - Inject current into specific neuron
- `injectCurrentToNeurons(type, current)` - Inject current to all neurons of type
- `produceAction()` - Produce motor action based on neural activity
- `getTotalSpikeCount()` - Get total spike count
- `getAverageFiringRate()` - Get average firing rate
- `get_connectivity_stats()` - Get brain connectivity statistics

#### AgentBrain
High-level interface connecting NLM brain to world.

**Key Methods**:
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory percept
- `decodeMotorCommand()` - Decode brain activity to motor command
- `applyRewardModulation(reward, predicted_reward)` - Apply reward-based neuromodulation
- `updateDevelopment(timestep)` - Update development system

#### SimpleWorld
2D simulation world for agent interaction.

**Key Methods**:
- `configure(width, height, vision_width, vision_height)` - Configure world dimensions
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world simulation
- `applyMotorCommand(action, time)` - Apply motor command to world
- `getSensoryPercept()` - Get sensory percept from world
- `add_resource_object(x, y, value, radius)` - Add resource object
- `add_hazard_object(x, y, value, radius)` - Add hazard object

### High-Level Wrapper Classes

#### NLMSimulation
High-level neural simulation manager with Pythonic interface.

**Key Features**:
- Method chaining for fluent API
- Quick start methods
- Episode-based simulation
- Learning experiment capabilities
- Comprehensive result analysis

#### NLMExperiment
Experiment framework for systematic testing and research.

**Key Features**:
- Parameter sweep capabilities
- Multiple brain configurations
- Standardized experiment protocols
- Result analysis and comparison

#### NLMPlotting
Visualization tools for neural data.

**Key Features**:
- Spike train visualization
- Firing rate plots
- Learning curve analysis
- 2D world visualization

#### NLMDeveloper
Development and research tools.

**Key Features**:
- Configuration templates
- Experiment templates
- Performance benchmarking
- Comparison tools

### Factory Functions

Convenience functions for creating common NLM objects:

```python
# Create default configuration
config = nlm.create_default_config()

# Create brain
brain = nlm.create_brain()

# Create world
world = nlm.create_world(800, 600, 80, 80)

# Create agent brain
agent_brain = nlm.create_agent_brain(brain)
```

### Enums

#### NeuronType
- `Excitatory` - Excitatory neurons
- `Inhibitory` - Inhibitory neurons
- `Sensory` - Sensory neurons
- `Motor` - Motor neurons
- `Internal` - Internal neurons
- `Modulatory` - Modulatory neurons

#### ActionType
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `Look` - Look
- `Interact` - Interact with environment
- `Eat` - Eat food
- `Drink` - Drink water
- `Wait` - Wait action
- `Rest` - Rest

#### DevelopmentalStage
- `Initial` - Initial development stage
- `CriticalPeriod` - Critical learning period
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

## Testing

### Running Tests

```bash
# Run all tests
pytest

# Run specific test module
pytest tests/test_nlm.py -v

# Run unit tests
pytest tests/test_core.py tests/test_simulation.py -v
```

### Test Coverage

The test suite includes:
- **Unit Tests**: Individual class and method testing
- **Integration Tests**: Multi-component system testing
- **Performance Tests**: Benchmarking and optimization testing
- **Experiment Tests**: Experiment framework testing
- **Error Handling Tests**: Exception and error handling testing

### Running Demo

```bash
# Run demonstration script
python demo_nlm.py

# This provides comprehensive demonstrations of all features
```

## Configuration

### Configuration Files

The NLM package uses configuration files in the `configs/` directory:

- `configs/default.cfg` - Default configuration
- Custom configurations can be created by copying and modifying

### Programmatic Configuration

```python
import nlm

# Create configuration
config = nlm.Config()

# Set parameters
config.set("random_seed", 42, None)
config.set("simulation_timestep", 0.001, None)
config.set("neuron_count", 1000, None)
config.set("region_count", 1, None)
config.set("connection_probability", 0.15, None)

# Save configuration
config.save_to_json_file("my_config.json")

# Load configuration
new_config = nlm.Config()
new_config.load_from_json_file("my_config.json")
```

## Development

### Project Structure

```
nlm/
├── __init__.py              # Package initialization
├── core.py                  # Core classes
├── sensory.py              # Sensory systems
├── motor.py                # Motor system
├── agent.py                # Agent system
├── world.py                # World system
├── types.py                # Type definitions
├── wrapper.py              # High-level wrappers
├── factory.py              # Factory functions
├── constants.py            # Constants
├── __main__.py             # Command-line interface
└── __pycache__/            # Python cache

configs/
└── *.cfg                   # Configuration files

tests/
└── *.py                    # Test files
```

### Running in Development Mode

```bash
# Install in development mode
pip install -e .

# Run development server
python demo_nlm.py

# Run tests
pytest tests/ -v
```

### Adding New Features

To add new features:
1. Create new Python classes in the appropriate module
2. Update the `nlm/__init__.py` to export new classes
3. Add documentation in the `NLM.md` file
4. Create tests in the `tests/` directory

### Testing Guidelines

- **Unit Tests**: Test individual classes and methods
- **Integration Tests**: Test multi-component interactions
- **Performance Tests**: Test simulation performance
- **Error Tests**: Test error handling and validation
- **Documentation Tests**: Ensure docstrings are complete

## Performance Considerations

### Large-Scale Simulations

For simulations with many neurons (>10,000):
- Use larger simulation timesteps
- Reduce sensory input frequency
- Enable checkpoint saving
- Monitor memory usage

### Memory Management

- Brains can be saved and reloaded for long experiments
- Use checkpoint system for recovery
- Monitor spike system memory usage

## Troubleshooting

### Common Issues

#### Import Errors
```bash
# Error: ModuleNotFoundError: No module named 'nlm'
# Solution: pip install -e .
```

#### Build Errors
```bash
# Error: CMake failed
# Solution: Ensure CMake is installed and try again
# On Ubuntu: apt-get install cmake
```

#### Runtime Errors
```python
# Error: Brain not initialized
# Solution: Call brain.initialize() before use
```

#### Configuration Errors
```python
# Error: Invalid configuration
# Solution: Use config.validate() to check configuration
```

### Getting Help

#### Online Resources
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Issues**: Report issues at GitHub

#### Local Documentation
- **Complete Documentation**: `NLM.md`
- **Installation Guide**: `INSTALLATION.md`
- **API Reference**: Individual docstrings
- **Examples**: `demo_nlm.py`

## License

This project is licensed under the MIT License.

```
MIT License

Copyright (c) 2026 NLM Research Team

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## Acknowledgements

### Contributors
- NLM Research Team
- Open-source contributors
- Documentation writers
- Test engineers

### Funding
- NLM Research Initiative
- Academic partnerships
- Open-source grants

### Tools Used
- Python 3.8+
- CMake 3.16+
- Pybind11
- Matplotlib (optional)
- pytest (for testing)

## Future Developments

### Planned Features
1. **Advanced Visualization**: 3D neural network visualization
2. **Parallel Processing**: Multi-threading for large simulations
3. **Cloud Integration**: Distributed simulation capabilities
4. **Web Interface**: Web-based simulation interface
5. **Mobile Support**: Mobile neural simulation apps
6. **Enhanced AI Integration**: Better AI model integration

### Research Directions
1. **Neuroplasticity Studies**: Advanced plasticity modeling
2. **Memory Research**: Enhanced memory systems
3. **Cognitive Development**: Advanced cognitive development models
4. **Neuromodulation**: Better neuromodulation integration
5. **Social Learning**: Multi-agent social learning systems

## Support

### Getting Help
- **GitHub Issues**: Report bugs and request features
- **Documentation**: Read comprehensive documentation
- **Examples**: Run demonstration scripts
- **Community**: Join the NLM community

### Reporting Issues
Please report issues with:
- Bug reports
- Feature requests
- Documentation issues
- Performance problems

### Contributing
Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Add tests and documentation
5. Submit a pull request

## Contact

### Project Team
- **Website**: https://nlm.ai
- **Email**: research@nlm.ai
- **GitHub**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io

### Social Media
- **Twitter**: @NLM_Project
- **LinkedIn**: NLM Research
- **Reddit**: r/neurallearningmachine

## Acknowledgements

This project builds upon years of neural computation research and represents Phase 6 of the NLM developmental project. Special thanks to:

- **Research Team**: Core developers and researchers
- **Open Source Community**: Contributors and users
- **Academic Partners**: Research collaborations
- **Funding Agencies**: Financial support
- **Users**: Community feedback and support

The NLM project is continuously evolving through research and community contributions. Your feedback and participation help make this project better for everyone.
