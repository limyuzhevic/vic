# Installation Instructions for NLM Python Bindings

## Quick Start

```bash
# Install from PyPI (stable release)
pip install pynlm

# Install from source (development version)
pip install -e .
```

## System Requirements

- Python 3.8 or higher
- C++ compiler (g++, clang, MSVC)
- CMake 3.16 or higher
- pybind11
- scikit-build-core

## Build from Source

```bash
# Clone the repository
 git clone <repository-url>
 cd nlm

# Build Python bindings
 mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install
pip install .
```

## Alternative Build Methods

### Using conda
```bash
conda install -c conda-forge pynlm
```

### Using Docker
```bash
# Build and run Docker container
docker build -t nlm-python .
docker run nlm-python
```

## Testing

### Install development dependencies
```bash
pip install pytest numpy
```

### Run tests
```bash
# Run unit tests
python -m pytest tests/

# Run integration tests
python -m pytest tests/integration/
```

## Usage Examples

### Basic Brain Creation and Simulation

```python
from pynlm import Brain, Config

# Create brain with default settings
brain = Brain()

# Create brain with custom configuration
config = Config({
    'neuron_count': 2000,
    'region_count': 3,
    'simulation_timestep': 0.0005,
    'random_seed': 42
})
brain = Brain(config)
```

### Running Simulations

```python
# Initialize brain
brain.initialize()

# Run simulation steps
for step in range(1000):
    brain.step(step, step * 0.001)

# Inject sensory input
brain.inject_current_to_neurons('sensory', 50.0)
```

### Learning and Plasticity

```python
# Enable plasticity
brain.enable_stdp(True)
brain.enable_hebbian(True)
brain.enable_structural(True)

# Run learning experiment
for episode in range(50):
    brain.reset()
    brain.initialize()
    
    for step in range(200):
        brain.inject_current_to_sensory_neurons(30.0)
        brain.step(step, step * 0.001)
```

## Configuration Files

### Example Configuration Files

```bash
# Default configuration
configs/default.cfg

# Custom configuration
configs/custom.cfg

# Development configuration
configs/development.cfg
```

### Configuration Schema

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | int | 1000 | Number of neurons |
| `region_count` | int | 2 | Number of brain regions |
| `connection_probability` | float | 0.15 | Synapse formation probability |
| `simulation_timestep` | float | 0.001 | Time step size |
| `random_seed` | int | 42 | Random seed |

### Loading Configuration

```python
from pynlm import Config

# Load configuration from file
config = Config()
config.load_from_file('configs/custom.cfg')

# Create configuration from dictionary
config_dict = {
    'neuron_count': 1000,
    'region_count': 2,
    'simulation_timestep': 0.001
}
config = Config(config_dict)
```

## Command Line Tools

### NLM Command Line Interface

```bash
# Show help
nlm --help

# Run basic simulation
nlm --experiment basic --steps 1000

# Run plasticity experiment
nlm --experiment plasticity --neurons 2000 --steps 2000

# Run STDP verification
nlm --experiment stdp --steps 500

# Run with custom configuration
nlm --config configs/myconfig.cfg --verbose

# Save checkpoint
nlm --experiment all --checkpoint brain_state.nlm
```

### Python Command Line Examples

```bash
# Python API usage examples
python -c "from pynlm import Brain; brain = Brain(); brain.initialize(); print('Brain created successfully')"

# Run learning experiment script
python examples/learning_experiment.py

# Run simulation with monitoring
python examples/monitor_simulation.py
```

## Examples Directory

The examples directory contains practical demonstrations of NLM usage:

### Basic Examples
- `examples/basic_brain.py` - Basic brain creation and simulation
- `examples/learning_basics.py` - Introduction to learning and plasticity
- `examples/simulation_control.py` - Simulation control and monitoring

### Advanced Examples
- `examples/multi_agent.py` - Multi-agent systems
- `examples/environment_interaction.py` - Complex environment interaction
- `examples/performance_tuning.py` - Performance optimization

### Development Examples
- `examples/debugging.py` - Debugging and testing
- `examples/benchmarking.py` - Performance benchmarking
- `examples/configuration_testing.py` - Configuration testing

## Development Guidelines

### Code Style
- Follow PEP 8 style guide
- Use descriptive variable names
- Include docstrings for all public functions
- Add type hints where possible

### Testing
- Write unit tests for all public functions
- Include integration tests for complex workflows
- Add performance benchmarks
- Test with different Python versions

### Documentation
- Update docstrings for all new functions
- Maintain examples directory
- Update API documentation
- Add release notes

## Troubleshooting

### Common Issues

#### Installation Problems

**Error: CMake not found**
```bash
# Install CMake
# Ubuntu/Debian:
sudo apt-get install cmake
# macOS:
brew install cmake
# Windows:
choco install cmake
```

**Error: Python version not supported**
```bash
# Check Python version
python --version
# Upgrade if necessary
pip install --upgrade python
```

#### Runtime Problems

**Error: Cannot load configuration**
```bash
# Check if file exists
ls -la configs/default.cfg
# Check file permissions
chmod 644 configs/default.cfg
```

**Error: Simulation crashes**
```bash
# Enable verbose logging
python -c "from pynlm import Config; Config.set_log_level('DEBUG')"
# Run with smaller simulation
# Decrease neuron_count in configuration
```

#### Performance Issues

**Slow simulation**
```bash
# Optimize configuration
config = Config({
    'neuron_count': 500,  # Reduce neurons
    'simulation_timestep': 0.001  # Adjust timestep
})
```

**Memory usage too high**
```bash
# Clear memory between simulations
brain.clear_memory()
# Use checkpointing for long simulations
brain.save('checkpoint.nlm')
```

## Getting Help

### Online Resources
- **Documentation**: https://nlm.readthedocs.io
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Issue Tracker**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions

### Community
- **Discord**: Join the NLM Discord server
- **Reddit**: r/nlm_community
- **Email**: support@nlm-project.org

### Support Levels

| Support Type | Description | How to Access |
|--------------|-------------|---------------|
| **Basic Support** | Answer questions, help with setup | GitHub Issues |
| **Advanced Support** | Code review, optimization help | Contact support |
| **Professional Support** | Dedicated support, SLAs | Contact sales |

## Contributing

### Contributing Guidelines

#### Code Contributions
1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Add tests
5. Update documentation
6. Submit a pull request

#### Documentation Contributions
1. Fork the repository
2. Update documentation files
3. Add examples
4. Fix typos and errors
5. Submit a pull request

### Development Setup

```bash
# Clone repository
 git clone <repository-url>
 cd nlm

# Create virtual environment
python -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate

# Install development dependencies
pip install -e .[dev]

# Run tests
pytest

# Build documentation
gitbook build docs/
```

## Migration Guide

### From C++ to Python

If migrating from C++ NLM:

**C++**
```cpp
auto config = std::make_shared<Config>();
config->set("neuron_count", 1000);
auto brain = std::make_shared<Brain>(config);
brain->initialize();
```

**Python**
```python
from pynlm import Brain, Config

config = Config({'neuron_count': 1000})
brain = Brain(config)
brain.initialize()
```

### Configuration Parameter Mapping

| C++ Parameter | Python Parameter | Default |
|---------------|------------------|---------|
| `neuron_count` | `neuron_count` | 1000 |
| `region_count` | `region_count` | 2 |
| `simulation_timestep` | `simulation_timestep` | 0.001 |
| `random_seed` | `random_seed` | 42 |

## Release Process

### Version Bumping
1. Update version in pyproject.toml
2. Update changelog
3. Update documentation
4. Run full test suite
5. Create release notes
6. Tag the release
7. Publish to PyPI

### Pre-release Checklist
- [ ] All tests pass
- [ ] Documentation updated
- [ ] Examples working
- [ ] Version bumped
- [ ] Release notes prepared
- [ ] Branch cleaned up
- [ ] Remote branch updated

## License

MIT License

Copyright (c) 2024 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
