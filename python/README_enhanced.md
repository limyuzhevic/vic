# NLM Enhanced Python Bindings - README

## Overview

This directory contains enhanced Python bindings for the NLM (Neural Learning Machine) neural simulation framework.
These bindings provide a much more user-friendly, powerful, and feature-rich Python interface for interacting with the NLM brain simulation system.

## What's New

### Major Improvements

1. **Enhanced User Experience**
   - Better error handling with custom exception classes
   - Intuitive, Pythonic APIs
   - Comprehensive documentation and examples
   - Progress monitoring and callbacks

2. **Comprehensive Metrics Collection**
   - Detailed simulation metrics collection
   - Automatic statistics and analysis
   - Export capabilities (JSON, CSV)
   - Real-time progress monitoring

3. **Environment Simulation**
   - Full world/environment integration
   - Agent simulation support
   - Sensor/motor control
   - Environment interaction

4. **Configuration Management**
   - Enhanced configuration system
   - Type-safe configuration access
   - Configuration validation
   - Save/load configuration

5. **Command-Line Tools**
   - `nlm-cli` command-line interface
   - Batch processing capabilities
   - Experiment management
   - Benchmarking tools

6. **Advanced Features**
   - Custom timesteps and simulation control
   - Progress callbacks
   - Data export and analysis
   - Performance monitoring
   - Error recovery and retry logic

## Installation

### Prerequisites

```bash
pip install scikit-build-core pybind11 pytest numpy pandas
```

### Installation Options

#### Standard Installation

```bash
cd /path/to/nlm/python
pip install .
```

#### Development Installation

```bash
cd /path/to/nlm/python
pip install -e .
```

#### Build from Source

```bash
cd /path/to/nlm/python
python -m build
pip install dist/*.whl
```

## Usage

### Basic Usage

```python
from pynlm_enhanced import BrainSimulator

# Create and initialize brain simulator
brain_sim = BrainSimulator()
brain_sim.initialize()

# Run simulation with automatic metrics collection
brain_sim.run(1000)

# Get simulation results
summary = brain_sim.get_metrics_summary()
print(f"Simulation completed: {summary['total_steps']} steps")
print(f"Average firing rate: {summary['avg_firing_rate']:.2f} Hz")
```

### Advanced Usage

```python
from pynlm_enhanced import BrainSimulator, NLMEnvironment, NLMConfigurator

# Create brain with custom configuration
config = NLMConfigurator()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)

brain_sim = BrainSimulator(config.config)
brain_sim.initialize()

# Create environment
env = NLMEnvironment()
env.configure(width=20, height=20, vision_width=8, vision_height=8)
env.reset()

# Define progress callback
def progress_callback(metrics):
    if metrics.step % 100 == 0:
        print(f"Step {metrics.step}: {metrics.active_neurons} firing neurons")

# Run simulation with custom timestep and callback
brain_sim.run(500, timestep=0.01, callback=progress_callback)

# Export results
brain_sim.export_metrics("simulation_results.json", "json")
```

### Complete Agent Simulation

```python
from pynlm_enhanced import BrainSimulator, NLMEnvironment

# Setup
brain_sim = BrainSimulator()
brain_sim.initialize()

env = NLMEnvironment()
env.configure(width=30, height=30, vision_width=12, vision_height=12)
env.reset()
env.set_agent_start(15.0, 15.0)

# Run simulation loop
for step in range(1000):
    # Update environment
    env_result = env.update(timestep=0.1)
    
    # Run brain simulation
    metrics = brain_sim.step(step)
    
    # Here you would decode motor commands and apply them to the environment
    # action = brain_sim.decode_motor_command(metrics)
    # env.apply_action(action, env_result['timestamp'])
    
    # Apply reward modulation based on environment feedback
    # agent.apply_reward_modulation(reward, predicted_reward)

print("✓ Simulation completed successfully")
```

## Command-Line Interface

The enhanced Python bindings include a comprehensive command-line interface (`nlm-cli`) for batch processing and automation.

### Available Commands

#### `nlm-cli brain create [options]`
Create and initialize a brain.

```bash
nlm-cli brain create --neurons 1000 --regions 1 --steps 100
```

#### `nlm-cli sim run [options]`
Run a brain simulation.

```bash
nlm-cli sim run --steps 1000 --timestep 0.001 --export-metrics --format json
```

#### `nlm-cli env [command]`
Manage world/environment simulation.

```bash
nlm-cli env configure --width 20 --height 20 --vision-width 8 --vision-height 8
nlm-cli env reset
nlm-cli env update --timestep 0.1
nlm-cli env start --x 10.0 --y 10.0
```

#### `nlm-cli config [command]`
Manage configuration.

```bash
nlm-cli config load config.json
nlm-cli config set brain.neuron_count 1500
nlm-cli config get brain.neuron_count
```

#### `nlm-cli experiment [options]`
Run a complete experiment.

```bash
nlm-cli experiment --steps 1000 --name "My Experiment" --output ./results
```

#### `nlm-cli export [source] [options]`
Export simulation data.

```bash
nlm-cli export config --output config.json --format json
```

#### `nlm-cli benchmark [options]`
Run performance benchmarks.

```bash
nlm-cli benchmark --steps 100 --repeats 5 --output benchmark.json
```

### Common CLI Usage

```bash
# Run a basic simulation with metrics export
nlm-cli sim run --steps 500 --export-metrics

# Run a complete experiment with environment
nlm-cli experiment --steps 1000 --output ./results --name "Agent Study"

# Benchmark performance
nlm-cli benchmark --steps 200 --repeats 10
```

## Examples

### Example 1: Silent Brain Simulation

```python
"""Example: Simple brain simulation without environment."""

from pynlm_enhanced import BrainSimulator

# Create brain simulator
brain_sim = BrainSimulator()
brain_sim.initialize()

# Run silent simulation
brain_sim.run(100)

# Analyze results
summary = brain_sim.get_metrics_summary()
print(f"Brain simulation complete: {summary['total_steps']} steps")
print(f"Neurons firing: {summary['max_firing_neurons']}")
```

### Example 2: Agent in World

```python
"""Example: Complete agent simulation in a world environment."""

from pynlm_enhanced import BrainSimulator, NLMEnvironment

# Create brain and environment
brain_sim = BrainSimulator()
brain_sim.initialize()

env = NLMEnvironment()
env.configure(width=15, height=15, vision_width=6, vision_height=6)
env.reset()
env.set_agent_start(7.5, 7.5)

# Run simulation
for step in range(200):
    # Update world
    env_result = env.update(timestep=0.1)
    
    # Brain processes sensory input
    metrics = brain_sim.step(step)
    
    # Agent receives sensory percept
    percept = env_result['percept']
    
    # Here you would implement action selection
    # action = agent.decode_motor_command()
    # env.apply_action(action, env_result['timestamp'])

print("✓ Agent simulation completed successfully")
```

### Example 3: Configuration Management

```python
"""Example: Configuration management with validation."""

from pynlm_enhanced import NLMConfigurator

# Create configurator
config = NLMConfigurator()

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("neuromod.curiosity.enable", True)

# Load configuration from file
if config.load("config.json"):
    print("✓ Configuration loaded successfully")

# Get configuration values
neuron_count = config.get("brain.neuron_count")
print(f"Neuron count: {neuron_count}")

# Save configuration
config.save("new_config.json")
print("✓ Configuration saved")
```

### Example 4: Metrics Analysis

```python
"""Example: Metrics collection and analysis."""

from pynlm_enhanced import BrainSimulator

# Create and run brain
brain_sim = BrainSimulator()
brain_sim.initialize()
brain_sim.run(500)

# Get metrics summary
summary = brain_sim.get_metrics_summary()

# Analyze metrics
print("Simulation Analysis:")
print(f"  Total steps: {summary['total_steps']}")
print(f"  Time range: {summary['step_range'][0]} to {summary['step_range'][1]} steps")
print(f"  Neuron activity: {summary['min_firing_neurons']} to {summary['max_firing_neurons']} neurons")
print(f"  Average firing rate: {summary['avg_firing_rate']:.2f} Hz")
print(f"  Total spikes: {summary['total_spikes']}")

# Export metrics
brain_sim.export_metrics("analysis_results.json", "json")
print("✓ Analysis results exported")
```

## Testing

### Running Tests

```bash
cd /path/to/nlm/python
python test_enhanced.py
```

### Test Coverage

The test suite includes:

1. **Basic Functionality Tests**
   - Brain simulation initialization
   - Step execution
   - Reset functionality
   - Metrics collection

2. **Environment Tests**
   - World configuration
   - Environment updates
   - Agent interaction
   - Sensor/mission control

3. **Configuration Tests**
   - Configuration setting/getting
   - Configuration validation
   - File I/O operations
   - Default value handling

4. **Advanced Feature Tests**
   - Custom timesteps
   - Progress callbacks
   - Data export
   - Error handling

5. **Performance Tests**
   - Benchmark testing
   - Memory usage
   - Execution speed

### Example Test Output

```
NLM Enhanced Python Bindings - Test Suite
==================================================

Test 1: Basic functionality... ✓ Passed
Test 2: Environment functionality... ✓ Passed
Test 3: Configuration functionality... ✓ Passed
Test 4: Advanced features... ✓ Passed
Test 5: Error handling... ✓ Passed
Test 6: Convenience functions... ✓ Passed
Test 7: Metrics analysis... ✓ Passed
Test 8: Performance test... ✓ Passed

Test Summary: 8 passed, 0 failed

🎉 All tests passed!
```

## Features

### Core Features

#### 1. Enhanced APIs
- **BrainSimulator**: High-level brain simulation interface
- **NLMEnvironment**: World/environment simulation
- **NLMConfigurator**: Configuration management
- **SimulationMetrics**: Metrics collection and analysis

#### 2. Error Handling
- **Custom exception classes**: `NLMError`, `InitializationError`, `ConfigurationError`, `SimulationError`
- **Comprehensive error messages**: Detailed error information
- **Graceful recovery**: Automatic error recovery where possible
- **Validation**: Input validation and parameter checking

#### 3. Metrics Collection
- **Automatic metrics**: Real-time metrics collection during simulation
- **Comprehensive statistics**: Multiple statistical measures
- **Data export**: Export metrics to JSON and CSV formats
- **Progress monitoring**: Real-time progress reporting

#### 4. Simulation Control
- **Custom timesteps**: Flexible timestep control
- **Progress callbacks**: User-defined callback functions
- **Batch processing**: Run multiple simulations efficiently
- **State management**: Save/load simulation state

#### 5. Configuration Management
- **Type-safe access**: Get configuration values with type conversion
- **Configuration validation**: Validate configuration parameters
- **File I/O**: Load/save configuration to files
- **Default values**: Handle missing configuration with defaults

### Advanced Features

#### 1. Command-Line Interface
- **Comprehensive CLI**: Full-featured command-line interface
- **Batch processing**: Process multiple simulations
- **Experiment management**: Manage and run experiments
- **Performance benchmarking**: Measure and optimize performance

#### 2. Integration
- **Environment simulation**: Complete world simulation
- **Agent interaction**: Agent-world interaction
- **Sensor/mission control**: Sensory input and motor output
- **Action execution**: Apply motor commands to world

#### 3. Data Analysis
- **Statistical analysis**: Comprehensive statistical measures
- **Progress tracking**: Track simulation progress
- **Result export**: Export results for analysis
- **Visualization support**: Export data for visualization

#### 4. Extensibility
- **Plugin architecture**: Easy to extend with new features
- **Custom callbacks**: User-defined callback functions
- **Configuration plugins**: Custom configuration loaders
- **Export plugins**: Additional export formats

## Technical Details

### Package Structure

```
python/
├── __init__.py                    # Package initialization
├── pynlm_enhanced.py              # Enhanced Python bindings
├── nlm_cli.py                     # Command-line interface
├── test_enhanced.py                # Test suite
└── examples/                      # Example scripts
    ├── enhanced_examples.py        # Enhanced examples
    └── other_examples/             # Additional examples
```

### Dependencies

#### Required Dependencies
- **Python**: 3.8+
- **pybind11**: 2.11.0+ (for C++ bindings)
- **numpy**: 1.20+ (for array operations)
- **scikit-build-core**: For building C++ extensions

#### Optional Dependencies
- **pandas**: For CSV export
- **matplotlib**: For visualization (not included)
- **jupyter**: For interactive notebooks (not included)

### Performance Considerations

1. **Memory Usage**: Monitor memory usage during long simulations
2. **Simulation Speed**: Use appropriate timesteps for performance
3. **Export Overhead**: Large simulations may take time to export
4. **Callback Performance**: Minimize callback execution time

### Compatibility

- **Python Versions**: 3.8, 3.9, 3.10, 3.11, 3.12
- **Operating Systems**: Linux, macOS, Windows
- **Python Bindings**: Compatible with existing pynlm C++ bindings
- **Backward Compatibility**: Maintains compatibility with existing APIs

## Migration Guide

### From pynlm to pynlm_enhanced

#### Basic Migration

**Before:**
```python
import pynlm
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(0)
```

**After:**
```python
from pynlm_enhanced import BrainSimulator
brain_sim = BrainSimulator()
brain_sim.initialize()
brain_sim.step(0)
```

#### Enhanced Features

**Before:**
```python
brain.getTotalNeuronCount()
brain.getTotalSpikeCount()
brain.getAverageFiringRate()
```

**After:**
```python
brain_sim.brain.getTotalNeuronCount()
metrics = brain_sim.step(0)
print(f"Firing neurons: {metrics.active_neurons}")
print(f"Firing rate: {metrics.firing_rate:.2f} Hz")
```

### Benefits of Upgrade

1. **Better Error Handling**: Clear error messages and exception handling
2. **Enhanced Metrics**: Automatic metrics collection and analysis
3. **Environment Support**: Complete world simulation support
4. **Configuration Management**: Better configuration handling
5. **Command-Line Tools**: Command-line interface for batch processing
6. **Performance Monitoring**: Built-in performance monitoring
7. **Data Export**: Easy export of simulation results
8. **Progress Callbacks**: Custom progress monitoring

## Troubleshooting

### Common Issues

#### Import Errors

**Issue**: `ImportError: No module named 'pynlm_enhanced'`

**Solution**:
```bash
# Check if module is available
ls python/

# Install in development mode
pip install -e ./python

# Or install the package
pip install ./python
```

#### Configuration Errors

**Issue**: Configuration file not found or invalid

**Solution**:
```python
# Check if file exists
if not os.path.exists("config.json"):
    print("Configuration file not found")

# Create default configuration
config = NLMConfigurator()
config.set("brain.neuron_count", 1000)
config.save("config.json")
```

#### Simulation Errors

**Issue**: "Brain not initialized. Call initialize() first."

**Solution**:
```python
from pynlm_enhanced import BrainSimulator

brain_sim = BrainSimulator()
brain_sim.initialize()  # Always call initialize() first
brain_sim.step(0)       # Then run simulation steps
```

#### Environment Errors

**Issue**: "World not configured. Call configure() first."

**Solution**:
```python
from pynlm_enhanced import NLMEnvironment

env = NLMEnvironment()
env.configure(width=20, height=20)  # Always configure first
env.reset()
env.update(0.1)
```

#### Export Errors

**Issue**: "Unsupported format" or pandas import errors

**Solution**:
```python
# Check if pandas is available
try:
    import pandas as pd
    brain_sim.export_metrics("results.csv", "csv")
except ImportError:
    print("Pandas not available. Install with: pip install pandas")
    brain_sim.export_metrics("results.json", "json")
```

### Getting Help

#### Documentation

- **Main Documentation**: `README.md` in this directory
- **User Guide**: `HOW_TO_USE.md` in the root directory
- **Examples**: `python/examples/enhanced_examples.py`
- **API Reference**: `pynlm_enhanced.py` module documentation

#### Online Resources

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Issue Tracker**: https://github.com/nlm-project/nlm/issues
- **Discussion Forum**: https://github.com/nlm-project/nlm/discussions

#### Support

For support and assistance:

1. **File Bug Reports**: Use the GitHub issue tracker
2. **Request Features**: Submit feature requests via GitHub
3. **Ask Questions**: Use the GitHub discussions
4. **Get Help**: Check the documentation and examples

### Contributing

#### Contributing Guidelines

This project follows standard Python development practices. Contributions are welcome and encouraged!

#### How to Contribute

1. **Fork the Repository**: Create your own copy of the project
2. **Create a Branch**: Work on your feature or bug fix
3. **Make Changes**: Implement your changes
4. **Test**: Run the test suite
5. **Documentation**: Update documentation if needed
6. **Submit Pull Request**: Submit your changes for review

#### Code Quality Standards

- **Python Style**: PEP 8 compliant
- **Documentation**: Comprehensive docstrings and comments
- **Testing**: Comprehensive test coverage
- **Error Handling**: Robust error handling
- **Performance**: Efficient implementation

#### Development Workflow

1. **Clone the Repository**
   ```bash
   git clone https://github.com/nlm-project/nlm.git
   cd nlm/python
   ```

2. **Set Up Development Environment**
   ```bash
   pip install -e .
   pip install -e .[dev]
   ```

3. **Run Tests**
   ```bash
   python test_enhanced.py
   ```

4. **Run Examples**
   ```bash
   python examples/enhanced_examples.py
   ```

5. **Run CLI Tools**
   ```bash
   ./python/nlm_cli.py --help
   ```

## License

This package is licensed under the MIT License. See the `LICENSE` file for details.

## Copyright

Copyright © 2024 NLM Project. All rights reserved.

## Acknowledgments

This enhanced Python bindings package is built upon the existing NLM neural simulation framework. We thank the original NLM developers for their work and contributions.

Special thanks to the open-source community for their valuable contributions and support.

## Contact Information

For questions, issues, or suggestions:

- **Email**: nlm-project@contact.org
- **GitHub**: https://github.com/nlm-project/nlm
- **Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions

## Version History

### Version 2.0.0 (Current)
- Complete rewrite with enhanced Python bindings
- Comprehensive error handling and user-friendly APIs
- Full environment simulation support
- Command-line interface and batch processing
- Advanced metrics collection and analysis
- Extensive testing and documentation

### Version 1.0.0 (Legacy)
- Original Python bindings
- Basic functionality only
- Limited error handling
- No environment support

## Future Development

### Planned Features

1. **Enhanced Visualization**
   - Built-in plotting and visualization
   - Interactive web interfaces
   - Real-time monitoring dashboards

2. **Advanced Simulation Features**
   - Multi-agent simulations
   - Complex environment models
   - Advanced learning algorithms

3. **Integration**
   - Machine learning integration
   - Neuroscience data analysis
   - Brain-computer interface support

4. **Performance**
   - GPU acceleration
   - Parallel processing
   - Distributed computing

### Research Directions

1. **Memory Systems**
   - Working memory implementation
   - Episodic memory
   - Semantic memory

2. **Cognitive Functions**
   - Attention mechanisms
   - Planning algorithms
   - Concept formation

3. **Neuromodulation**
   - Dopamine systems
   - Serotonin systems
   - Novelty detection

4. **Development**
   - Developmental trajectories
   - Maturation effects
   - Critical periods

## Conclusion

The NLM Enhanced Python bindings represent a significant improvement over the original implementation. It provides a comprehensive, user-friendly, and powerful interface for neural simulation research and development.

The enhanced bindings make the NLM framework more accessible to researchers, students, and developers while maintaining the power and flexibility needed for advanced research.

This package opens new possibilities for neural simulation research and brings the NLM framework to a wider audience.

Thank you for using the NLM Enhanced Python bindings! 🎉