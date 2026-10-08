# NLM Python Package - New Installation Instructions

This document provides the latest installation instructions for the NLM Python package, which includes the core NLM bindings and advanced functionality for neural simulation research and development.

## Quick Start

### Prerequisites

- **Python 3.8+**: Python 3.8 or higher is recommended
- **CMake 3.16+**: For building the C++ extension
- **C++ Compiler**: Must support C++20

### Basic Installation

```bash
# Clone the NLM repository
 git clone https://github.com/nlm-project/nlm.git
 cd nlm

# Install the Python package (this will build the C++ extension)
pip install -e .

# Or install from the package directory
pip install -e ./path/to/nlm
```

## Advanced Installation

### For Development

If you plan to contribute to the NLM Python package, use this setup:

```bash
# Clone with all dependencies
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Install in development mode
pip install -e .[dev]

# Install additional dependencies for advanced features
pip install matplotlib pandas scikit-learn seaborn jupyter

# Install build dependencies
pip install pybind11 scikit-build-core pytest
```

### For Production Use

For production environments, install the core package:

```bash
# Install core package only
pip install .

# For specific advanced features
pip install .
# Some features may require additional packages
```

### Building from Source

If you need to build the C++ extension manually:

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake (debug for development, Release for production)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the extension
make -j4

# Install the Python module
pip install -e ../path/to/nlm
```

### Installation Verification

After installation, verify that the package is working:

```python
import pynlm

# Check module information
info = pynlm.get_module_info()
print(f"NLM Python version: {info['version']}")
print(f"Available classes: {len(info['available_classes'])}")

# Check if advanced features are available
features = info['advanced_features']
print("Advanced features available:")
for feature, available in features.items():
    print(f"  {feature}: {available}")

# Try creating a basic brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
```

## Advanced Features

### Performance Monitoring

The NLM package includes advanced performance monitoring tools:

```python
from pynlm import PerformanceMonitor

# Create performance monitor
monitor = PerformanceMonitor(brain)

# Start monitoring
monitor.start_monitoring(interval=0.1)

# Run simulation
for step in range(1000):
    brain.step(step)

# Stop monitoring and analyze
monitor.stop_monitoring()
stats = monitor.get_performance_stats()

# Generate comprehensive report
report = monitor.generate_performance_report()
```

### Batch Processing

Process multiple simulations efficiently:

```python
from pynlm import BatchProcessor

# Create batch processor
processor = BatchProcessor({
    'brain.neuron_count': 2000,
    'plasticity.stdp.learning_rate': 0.01
})

# Run batch of simulations
results = processor.run_batch(num_simulations=10, steps_per_simulation=1000)
summary = processor.get_batch_statistics()
```

### Advanced Analytics

Analyze brain simulation data:

```python
from pynlm import Analytics

# Create analytics engine
analytics = Analytics(brain)

# Analyze learning dynamics
learning_curves = analytics.analyze_learning_dynamics(steps=1000)

# Memory system analysis
memory_stats = analytics.analyze_memory_systems()

# Generate visualizations
analytics.plot_neural_dynamics()
analytics.plot_learning_curves()
```

## Configuration Management

Advanced configuration utilities for complex experimental setups:

```python
from pynlm.utils import ConfigManager

# Create configuration manager
config_manager = ConfigManager()

# Load configuration from file
config = config_manager.load_config('advanced_config.json')

# Create complex brain configuration
brain_config = config_manager.create_brain_config(
    neuron_count=5000,
    regions=[
        {'type': 'sensory', 'count': 1500, 'excitatory_ratio': 0.8},
        {'type': 'association', 'count': 2000, 'excitatory_ratio': 0.7},
        {'type': 'motor', 'count': 1500, 'excitatory_ratio': 0.6}
    ]
)

# Save configuration
config_manager.save_config(brain_config, 'my_config.json')
```

## Troubleshooting

### Common Installation Issues

#### Issue 1: CMake cannot find Python

Solution:
```bash
# Install pybind11 and scikit-build-core
pip install pybind11 scikit-build-core

# Set Python executable explicitly
cmake .. -DPYTHON_EXECUTABLE=$(which python3)
```

#### Issue 2: Build fails due to missing dependencies

Solution:
```bash
# Check for required system dependencies
# On Ubuntu/Debian:
apt-get update
apt-get install build-essential cmake python3-dev python3-pip

# On macOS:
# Install Xcode command line tools
xcode-select --install

# Install Homebrew if not present
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake python3
```

#### Issue 3: Python module not found after installation

Solution:
```bash
# Check if the package is installed correctly
pip list | grep pynlm

# Reinstall in development mode
pip install -e . --force-reinstall

# Check Python path
python -c "import sys; print(sys.path)"
```

### Common Runtime Issues

#### Issue 1: Brain initialization fails

Solution:
```python
# Check if the brain is properly initialized
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
brain = pynlm.createBrain(config)

if brain.initialize():
    print("Brain initialized successfully")
else:
    print("Brain initialization failed")
    print("Error code:", brain.getLastError())
```

#### Issue 2: Simulation performance issues

Solution:
```python
# Monitor simulation performance
from pynlm import PerformanceMonitor

monitor = PerformanceMonitor(brain)
monitor.start_monitoring(interval=0.05)

# Run simulation with performance monitoring
for step in range(1000):
    brain.step(step)

monitor.stop_monitoring()
stats = monitor.get_performance_stats()

print(f"Average firing rate: {stats['avg_firing_rate']}")
print(f"Peak CPU usage: {stats['peak_cpu']}")
```

#### Issue 3: Memory usage increases during simulation

Solution:
```python
# Check memory usage
import psutil
import os

process = psutil.Process(os.getpid())
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")

# Force garbage collection
import gc
gc.collect()
```

## Example Scripts

### Basic Examples

#### Simple Brain Simulation
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Simulation completed with {brain.getTotalSpikeCount()} spikes")
```

#### Agent with World
```python
import pynlm

# Create brain, world, and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run agent simulation
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Agent simulation completed")
```

### Advanced Examples

#### Performance Monitoring
```python
import pynlm
from pynlm import PerformanceMonitor

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Start performance monitoring
monitor = PerformanceMonitor(brain, world)
monitor.start_monitoring(interval=0.1)

# Run simulation with monitoring
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Stop monitoring and analyze
monitor.stop_monitoring()
stats = monitor.get_performance_stats()

# Generate comprehensive report
report = monitor.generate_performance_report()
```

#### Batch Processing
```python
import pynlm
from pynlm import BatchProcessor

# Create batch processor with specific configuration
processor = BatchProcessor({
    'brain.neuron_count': 1000,
    'brain.region_count': 2,
    'plasticity.stdp.learning_rate': 0.01,
    'neuromod.dopamine.scale': 1.5
})

# Run batch of simulations
results = processor.run_batch(num_simulations=5, steps_per_simulation=500)
summary = processor.get_batch_statistics()

print(f"Batch completed with {len(results)} simulations")
print(f"Average firing rate: {summary['firing_rate_statistics']['mean']}")
```

## Additional Resources

### Documentation

- **Advanced API Guide**: `python/ADVANCED_API_GUIDE.md` - Comprehensive documentation for advanced usage
- **API Reference**: `python/docs/API_REFERENCE.md` - Complete API documentation
- **Examples**: `python/examples/` - Collection of example scripts
- **Best Practices**: `python/docs/BEST_PRACTICES.md` - Tips and best practices

### GitHub Repository

- **Repository**: https://github.com/nlm-project/nlm
- **Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Documentation**: https://nlm.readthedocs.io

### Getting Help

- **GitHub Issues**: Report bugs and request features
- **Stack Overflow**: Ask questions using the `pynlm` tag
- **Discord**: Join the NLM community Discord server
- **Email**: For commercial support, contact the NLM team

### Community Resources

- **NLM Forum**: Discussion of NLM features and research
- **Examples Gallery**: Community-contributed examples and scripts
- **Tutorials**: Step-by-step tutorials for getting started
- **Webinars**: Regular webinars on NLM usage and development

## License

The NLM Python package is licensed under the MIT License. See `LICENSE` file for details.

## Copyright

Copyright © 2026 NLM Project Authors. All rights reserved.

## Acknowledgments

The NLM Python package is developed by the NLM Project team. We thank the following contributors:

- C++ core developers for the NLM simulation framework
- Python binding developers for the initial pybind11 integration
- Contributors to the NLM research project
- Users who have provided feedback and reported issues

## Change Log

### Version 0.1.0
- Initial release with core NLM Python bindings
- Basic brain, world, and agent functionality
- Configuration management
- Simple simulation examples

### Version 0.2.0 (Upcoming)
- Advanced performance monitoring
- Batch processing capabilities
- Analytics and visualization tools
- Enhanced configuration management
- Comprehensive documentation and examples

### Future Plans

- Integration with machine learning frameworks
- Advanced neural architecture search
- Multi-agent simulation
- Cloud-based distributed computing
- Real-time simulation capabilities
- Web-based visualization tools

## Support

For support, please visit the NLM GitHub repository or contact the NLM team directly. We appreciate your feedback and contributions to the NLM project!
