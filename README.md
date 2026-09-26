# Enhanced NLM Python API

## Overview

The Enhanced NLM Python API provides a comprehensive, Pythonic interface to the NLM (Neural Learning Machine) neural simulation framework. It includes advanced features for simulation, analysis, visualization, and experiment management.

## Features

### Core Functionality
- **Enhanced Brain Class**: Pythonic interface with context managers and convenience methods
- **Advanced Configuration**: Profile-based configuration with validation and presets
- **Performance Monitoring**: Real-time metrics and monitoring utilities
- **Simulation Helpers**: Quick start functions and result analysis

### User-Friendly Interfaces
- **Context Managers**: Automatic resource management and cleanup
- **Property-Based Access**: Pythonic attribute access for commonly used values
- **Iterator Support**: Convenient iteration over brain regions
- **Fluent Interface**: Chained method calls for enhanced readability

### Advanced Features
- **Experiment Management**: Comprehensive tools for scientific experiments
- **Visualization Tools**: Advanced plotting and analysis interfaces
- **Data Export**: Multiple format support (JSON, CSV, Python, NumPy)
- **Error Handling**: Enhanced exception classes with detailed error information

### Developer-Friendly Tools
- **Enhanced Exception Classes**: Custom exceptions with error codes and details
- **Utility Functions**: Helper functions for common operations
- **Module Information**: Runtime introspection and feature discovery
- **Backward Compatibility**: Support for existing NLM Python API patterns

## Installation

### Basic Installation

```bash
pip install pynlm-enhanced
```

### Development Installation

```bash
pip install -e .
```

### From Source

```bash
mkdir build
git clone https://github.com/nlm-project/nlm-python-enhanced.git
cd nlm-python-enhanced
cd build
cmake .. -DPYTHON_EXECUTABLE=$(which python3)
make -j4
pip install .
```

## Usage Examples

### Quick Start

```python
import nlmenhanced as nlm

# Create a brain with default configuration
brain = nlm.create_brain(nlm.Profile.BALANCED)

# Initialize the brain
brain.initialize()

# Run a simulation
result = nlm.run_simulation(brain, steps=200)

print(f"Simulation completed in {result.duration:.2f} seconds")
print(f"Average firing rate: {result.metrics.get('avg_firing_rate', 0):.2f} Hz")
```

### Using Context Managers

```python
import nlmenhanced as nlm

# Automatic resource management
with nlm.BrainContext(nlm.Profile.BALANCED) as brain:
    result = brain.run_simulation(100)
    print(f"Simulation completed with {result.total_steps} steps")

# Experiment management
experiment_config = nlm.ExperimentConfig(
    name="neural_network_test",
    brain_config={"neuron_count": 1000, "simulation_timestep": 0.001},
    simulation_steps=500,
    profile=nlm.Profile.RESEARCH
)

with nlm.ExperimentContext(experiment_config) as experiment:
    results = experiment.run()
    report = results.generate_report()
    print(report)
```

### Advanced Experiment Management

```python
import nlmenhanced as nlm

# Create experiment configuration
config = nlm.ExperimentConfig(
    name="deep_learning_test",
    brain_config={
        "neuron_count": 5000,
        "connection_probability": 0.2,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.025
    },
    simulation_steps=1000,
    profile=nlm.Profile.DETAILED,
    analysis_types=["basic", "advanced"],
    visualization_types=["firing_rates", "ei_ratio", "dashboard"],
    export_format=nlm.ExportFormat.JSON
)

# Run experiment
experiment = nlm.create_experiment(config)
results = experiment.run()

# Generate comprehensive report
report = results.generate_report()
print(report)

# Create visualizations
dashboard_path = results.plot_results()
print(f"Dashboard saved to: {dashboard_path}")

# Export results
export_path = results.export_data(nlm.ExportFormat.CSV)
print(f"Data exported to: {export_path}")
```

### Visualization and Analysis

```python
import nlmenhanced as nlm

# Create visualization tools
visualizer = nlm.NLMVisualizer("my_visualizations")

# Create simulation and run
brain = nlm.create_brain(nlm.Profile.RESEARCH)
simulation_result = nlm.run_simulation(brain, 500)

# Create various visualizations
firing_plot = visualizer.plot_firing_rates(simulation_result)
ei_plot = visualizer.plot_ei_ratio(simulation_result)

# Create comprehensive dashboard
dashboard = visualizer.create_dashboard(simulation_result)

# Export results in different formats
json_data = visualizer.export_results(simulation_result, nlm.ExportFormat.JSON)
csv_data = visualizer.export_results(simulation_result, nlm.ExportFormat.CSV)
```

### Performance Monitoring

```python
import nlmenhanced as nlm

# Create brain with monitoring
brain = nlm.create_brain(nlm.Profile.BALANCED)

# Create performance monitor
with brain.monitor() as monitor:
    # Run simulation
    for step in range(100):
        brain.step(step)
        
        # Get current metrics
        metrics = monitor.getMetrics()
        print(f"Step {step}: Firing rate = {metrics.avgFiringRate:.2f} Hz")

# Get final metrics
final_metrics = monitor.getMetrics()
print(f"Total simulation time: {final_metrics.processingTimeMs:.2f} ms")
print(f"Memory usage: {final_metrics.memoryUsageMB} MB")
```

### Error Handling

```python
import nlmenhanced as nlm

# Error handling with enhanced exceptions
try:
    brain = nlm.create_brain(nlm.Profile.BALANCED)
    brain.initialize()
    
    # Perform operations with error handling
    for step in range(100):
        brain.step(step)
        
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
    print(f"Error code: {e.error_code}")
    if e.details:
        print(f"Details: {e.details}")
        
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
    
except nlm.NLMError as e:
    print(f"NLM error: {e}")
```

## API Reference

### Core Classes

#### Brain
Enhanced brain class with Pythonic interfaces:

- `initialize()`: Initialize the brain
- `step(currentStep)`: Perform a simulation step
- `step(currentStep, currentTime)`: Perform a step with timestamp
- `inject(neuron, current)`: Inject current into a neuron
- `inject_type(type, current)`: Inject current into neurons of specific type
- `quick_start(profile)`: Initialize with quick start profile
- `with_context()`: Create context manager
- `monitor()`: Create performance monitor
- `run_simulation(steps)`: Run quick simulation
- `analyze_results(results)`: Analyze simulation results
- `generate_report(title)`: Generate human-readable report

#### Config
Enhanced configuration class:

- `loadFromFile(filepath)`: Load from JSON file
- `loadFromArgs(argc, argv)`: Load from command line
- `saveToFile(filepath)`: Save to JSON file
- `has(key)`: Check if key exists
- `get(key, default)`: Get configuration value
- `set(key, value)`: Set configuration value
- `getKeys()`: Get all keys
- `validate()`: Validate configuration

#### AgentBrain
Enhanced agent brain interface:

- `initialize(world)`: Initialize with world
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Decode motor command
- `applyRewardModulation(reward, predictedReward)`: Apply reward modulation
- `updateDevelopment(timestep)`: Update development system

#### SimpleWorld
Enhanced world interface:

- `configure(width, height, visionWidth, visionHeight)`: Configure world
- `reset()`: Reset world
- `setAgentStart(x, y)`: Set agent start position
- `update(timestep)`: Update world
- `applyMotorCommand(cmd, currentTime)`: Apply motor command

### Enhanced Classes

#### NLMError, ConfigurationError, InitializationError, SimulationError, VisualizationError
Custom exception classes with error codes and detailed information.

#### BrainContext, ExperimentContext
Context managers for automatic resource management.

#### NLMVisualizer
Visualization and analysis tools with plotting capabilities.

#### ExperimentManager, ExperimentResult
Comprehensive experiment management and results analysis.

### Enumerations

#### Profile
- `MINIMAL`: Minimal configuration for testing
- `BALANCED`: Balanced configuration for general use
- `FAST`: Fast configuration with fewer neurons
- `DETAILED`: Detailed configuration with more neurons
- `RESEARCH`: Research-grade configuration with advanced settings

#### VisualizationType
- `FIRING_RATES`: Plot firing rates over time
- `EXCITATION_INHIBITION`: Plot excitation/inhibition ratio
- `NEURON_ACTIVITY`: Plot neuron activity patterns
- `SPIKE_TIMES`: Plot spike times
- `TEMPORAL_DYNAMICS`: Plot temporal dynamics
- `PATTERN_ANALYSIS`: Analyze pattern consistency
- `SPATIAL_DISTRIBUTION`: Plot spatial distribution

#### ExportFormat
- `JSON`: JSON format
- `CSV`: CSV format
- `PYTHON`: Python format
- `NUMPY`: NumPy format
- `HDF5`: HDF5 format

### Utility Functions

#### create_brain(profile, custom_config)
Create a brain with specified configuration profile.

#### run_simulation(brain, steps, profile, custom_config)
Run a simulation with the specified brain configuration.

#### create_experiment(config)
Create and run an experiment with the specified configuration.

## Advanced Features

### Experiment Configuration

The `ExperimentConfig` class provides comprehensive configuration for scientific experiments:

```python
config = nlm.ExperimentConfig(
    name="my_experiment",
    brain_config={
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1,
        "simulation_timestep": 0.001
    },
    simulation_steps=500,
    profile=nlm.Profile.RESEARCH,
    analysis_types=["basic", "advanced"],
    visualization_types=["firing_rates", "ei_ratio", "dashboard"],
    export_format=nlm.ExportFormat.JSON,
    parallel=False,
    max_workers=1
)
```

### Performance Monitoring

```python
# Performance monitoring with detailed metrics
with brain.monitor() as monitor:
    for step in range(1000):
        brain.step(step)
        metrics = monitor.getMetrics()
        print(f"Step {step}: {metrics}")
```

### Data Analysis

```python
# Advanced analysis of simulation results
analysis = simulation_result.calculate_statistics()
print(f"Average firing rate: {analysis.get('avg_firing_rate', 0):.2f} Hz")
print(f"Firing rate std: {analysis.get('std_firing_rate', 0):.2f} Hz")
print(f"E/I ratio range: {analysis.get('min_ei_ratio', 1.0):.3f} - {analysis.get('max_ei_ratio', 1.0):.3f}")
```

## Installation and Setup

### Dependencies

```bash
# Core dependencies (automatically installed)
numpy>=1.21.0
matplotlib>=3.5.0
pandas>=1.3.0
scipy>=1.7.0

# Optional dependencies
pip install pynlm-enhanced[full]
```

### Building from Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm-python-enhanced.git
cd nlm-python-enhanced

# Create build directory and build
mkdir build
cd build
cmake .. -DPYTHON_EXECUTABLE=$(which python3)
make -j4

# Install
pip install .
```

### Testing

```bash
# Run tests
pytest

# Or run with coverage
pytest --cov=nlmenhanced --cov-report=html
```

## Development

### Project Structure

```
nlmenhanced/
├── __init__.py              # Main module
├── brain.py                 # Enhanced brain interface
├── config.py                # Enhanced configuration
├── experiment.py            # Experiment management
├── visualization.py         # Visualization tools
├── utils.py                 # Utility functions
├── exceptions.py            # Custom exceptions
├── enums.py                 # Enumerations
└── __main__.py              # Command line interface

examples/
├── basic_usage.py           # Basic usage examples
├── advanced_usage.py        # Advanced usage examples
├── experiment_examples.py   # Experiment examples
└── visualization_examples.py # Visualization examples

docs/
├── README.md                # Project documentation
├── API_REFERENCE.md         # API documentation
├── TUTORIAL.md              # Tutorial
└── examples/               # Example documentation
```

### Running Examples

```bash
# Run basic usage example
python examples/basic_usage.py

# Run advanced usage example
python examples/advanced_usage.py

# Run experiment example
python examples/experiment_examples.py

# Run visualization example
python examples/visualization_examples.py
```

### Testing and Development

```bash
# Install development dependencies
pip install -e .[dev]

# Run unit tests
pytest

# Run integration tests
pytest --integration

# Run tests with coverage
pytest --cov=nlmenhanced --cov-report=html --cov-report=term

# Run linters
black nlmenhanced/
flake8 nlmenhanced/
mypy nlmenhanced/
```

## Contributing

### Code Style

- Use Black for code formatting
- Use Flake8 for linting
- Use MyPy for type checking
- Follow PEP 8 style guide

### Testing

- Write comprehensive unit tests
- Include integration tests
- Add examples to test suite
- Maintain test coverage above 80%

### Documentation

- Write docstrings for all functions and classes
- Include examples in documentation
- Use Sphinx for documentation generation
- Maintain consistent documentation style

## License

This project is licensed under the MIT License.

## Acknowledgments

- NLM Development Team
- Open-source contributors
- Scientific community for neural network research
- Python ecosystem for powerful tools

## Support

For support, please visit:
- GitHub Issues: https://github.com/nlm-project/nlm-python-enhanced/issues
- Documentation: https://nlm-project.github.io/nlm-python-enhanced/
- Email: support@nlm-project.org

## Changelog

### Version 1.0.0 (2024)
- Initial release with enhanced Python API
- Comprehensive error handling
- Context managers and Pythonic interfaces
- Experiment management tools
- Visualization and analysis capabilities
- Performance monitoring utilities

### Future Plans
- Advanced machine learning integration
- Distributed computing support
- Cloud deployment capabilities
- Real-time visualization
- Interactive debugging tools
- Web API interface

## Conclusion

The Enhanced NLM Python API provides a powerful, user-friendly interface to the NLM neural simulation framework. It combines the computational power of C++ with the flexibility and ease of Python, making it accessible to researchers, developers, and practitioners alike.

With comprehensive error handling, advanced configuration management, experiment tools, and visualization capabilities, the enhanced API meets the needs of both beginners and advanced users.

Whether you're doing quick simulations, running complex experiments, or analyzing results, the Enhanced NLM Python API provides the tools you need for neural simulation research and development.