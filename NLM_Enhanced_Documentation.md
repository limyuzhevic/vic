# NLM (Neural Learning Machine) - Enhanced Documentation

## Overview

NLM (熙然) is an experimental artificial developmental brain simulation framework in Phase 6 (FINAL INTEGRATION). This enhanced version provides comprehensive improvements to the original NLM framework, including:

- **Enhanced Command-Line Interface**: User-friendly CLI with nlm-cli
- **Improved Python API**: More intuitive and well-documented Python interface
- **Better Configuration Management**: Simplified configuration system with validation
- **Advanced Error Handling**: Comprehensive error handling with recovery suggestions
- **Visualization Tools**: Built-in visualization capabilities
- **Experiment Management**: Enhanced experiment running and analysis
- **Comprehensive Documentation**: Complete documentation with examples

## Table of Contents

1. [Quick Start](#quick-start)
2. [Installation](#installation)
3. [Enhanced Features](#enhanced-features)
4. [Command-Line Interface](#command-line-interface)
5. [Python API](#python-api)
6. [Configuration Management](#configuration-management)
7. [Error Handling](#error-handling)
8. [Visualization](#visualization)
9. [Experiment Management](#experiment-management)
10. [Examples](#examples)
11. [Best Practices](#best-practices)
12. [Troubleshooting](#troubleshooting)
13. [API Reference](#api-reference)

## Quick Start

### Basic Usage

```python
# Install NLM
pip install nlm

# Import and use
from nlm import BrainSimulation, create_medium_config

# Create simulation
simulation = BrainSimulation(create_medium_config())

# Initialize and run
simulation.initialize()
results = simulation.run_simulation(1000)

print(f"Total spikes: {results['total_spikes']}")
print(f"Average firing rate: {results['average_firing_rate']:.3f} Hz")
```

### Command Line Usage

```bash
# Run quick demonstration
nlm-run --quick-demo

# Run Phase 2 neural computation demo
nlm-run --phase2 --neurons 500 --steps 500

# Run Phase 4 cognition demo
nlm-run --phase4

# Run Phase 6 integration demo
nlm-run --phase6

# Create configuration template
nlm-config create my_config

# Validate configuration
nlm-config validate my_config.json

# Start interactive Python session
nlm-python interactive
```

## Installation

### Prerequisites

- Python 3.8 or higher
- C++ compiler (for NLM core)
- Optional: matplotlib for visualization

### Installation Steps

```bash
# Install NLM
pip install nlm

# For development, clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm
pip install -e .

# Install optional dependencies
pip install matplotlib numpy
```

### Build from Source

```bash
# Clone repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build and install
pip install -e .
```

## Enhanced Features

### 1. Enhanced Command-Line Interface

The new `nlm-cli` provides comprehensive command-line functionality:

- **nlm-run**: Run simulations with various options
- **nlm-config**: Manage configuration files
- **nlm-python**: Python interface for programmatic control
- **nlm-visualize**: Visualization tools
- **nlm-status**: Show system status

### 2. Improved Python API

The enhanced Python API provides:

- **BrainSimulation**: High-level brain simulation wrapper
- **ExperimentManager**: Easy-to-use experiment management
- **NLMConfigManager**: Enhanced configuration management
- **Comprehensive Error Handling**: Informative error messages and recovery
- **Type Hints**: Better code completion and documentation

### 3. Better Configuration Management

- **Simplified Configuration Files**: JSON and INI formats
- **Configuration Validation**: Check configuration validity
- **Configuration Templates**: Pre-defined configuration templates
- **Export/Import**: Convert between configuration formats

### 4. Advanced Error Handling

- **Informative Error Messages**: Detailed error information
- **Recovery Suggestions**: Actionable recovery suggestions
- **Error Context**: Context-specific error handling
- **Error Logging**: Comprehensive error logging

### 5. Visualization Tools

- **Basic Brain Visualization**: Brain state and activity visualization
- **Connectivity Visualization**: Neural connectivity graphs
- **Activity Animation**: Animated neuron activity visualization
- **Comparison Visualization**: Compare different configurations

### 6. Experiment Management

- **Multiple Experiment Types**: Working memory, episodic recall, concept formation, etc.
- **Experiment Analysis**: Built-in analysis tools
- **Parameter Sweeps**: Run experiments with different parameters
- **Results Comparison**: Compare results across experiments

### 7. Comprehensive Documentation

- **Complete Documentation**: All features documented
- **Usage Examples**: Step-by-step examples
- **API Reference**: Detailed API documentation
- **Best Practices**: Recommended usage patterns
- **Troubleshooting**: Common issues and solutions

## Command-Line Interface

### nlm-cli Overview

The `nlm-cli` script provides a unified interface for NLM operations:

```bash
nlm-cli help           # Show main help
nlm-cli help run       # Show run command help
nlm-cli help config    # Show config command help
nlm-cli help python    # Show python command help
nlm-cli help visualize # Show visualize command help
```

### nlm-run Command

Run NLM simulations with various options:

```bash
# Run quick demonstration
nlm-run --quick-demo

# Run Phase 2 neural computation demo
nlm-run --phase2 --neurons 500 --steps 500

# Run Phase 4 cognition demo
nlm-run --phase4 --neurons 1000 --steps 1000

# Run specific experiment
nlm-run --experiment working_memory --trials 10 --steps 500

# Use custom configuration
nlm-run --config my_config.json

# Save checkpoint
nlm-run --checkpoint brain.checkpoint

# Enable verbose output
nlm-run --verbose
```

### nlm-config Command

Manage configuration files:

```bash
# Create configuration template
nlm-config create my_config

# Validate configuration
nlm-config validate my_config.json

# List available configurations
nlm-config list

# Show configuration details
nlm-config show my_config

# Generate example configurations
nlm-config examples ./examples

# Export configuration in different formats
nlm-config export my_config --format ini
```

### nlm-python Command

Python interface for programmatic control:

```bash
# Start interactive Python session
nlm-python interactive

# Run Python script
nlm-python run my_script.py

# Execute Python code
nlm-python execute "from nlm import BrainSimulation; bs = BrainSimulation()"

# Explore NLM Python API
nlm-python explore
```

### nlm-visualize Command

Visualization tools:

```bash
# Visualize brain state
nlm-visualize brain current

# Visualize simulation progress
nlm-visualize simulation 1000

# Visualize connectivity
nlm-visualize connectivity

# Export visualization data
nlm-visualize export brain_data.json
```

### nlm-status Command

Show system status and statistics:

```bash
nlm-status
```

## Python API

### Core Classes

#### BrainSimulation
High-level brain simulation wrapper with enhanced functionality.

```python
from nlm import BrainSimulation, create_medium_config

# Create simulation
simulation = BrainSimulation(create_medium_config())

# Initialize
simulation.initialize()

# Run simulation
results = simulation.run_simulation(1000)

# Inject sensory input
simulation.inject_sensory_input([0.1, 0.5, 0.9], "vision")

# Get brain state
state = simulation.get_brain_state()

# Save/load simulation
simulation.save_simulation("brain.checkpoint")
simulation.load_simulation("brain.checkpoint")
```

#### ExperimentManager
Manage and run experiments.

```python
from nlm import get_experiment_manager

experiment_manager = get_experiment_manager()

# Run specific experiment
results = experiment_manager.run_experiment(
    brain, "working_memory",
    trials=10, steps_per_trial=100
)

# Run all experiments
all_results = experiment_manager.run_all_experiments(
    brain, trials=5, steps_per_trial=50
)
```

#### NLMConfigManager
Enhanced configuration management.

```python
from nlm import NLMConfigManager

config_manager = NLMConfigManager()

# Create configuration
config_manager.create_config("my_config", {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1
})

# Load configuration
config = config_manager.load_config("my_config")

# Validate configuration
is_valid = config_manager.validate_config(config)

# Show configuration
config_manager.show_config("my_config")
```

### Convenience Functions

#### create_default_config()
```python
from nlm import create_default_config
config = create_default_config()
```

#### create_small_config()
```python
from nlm import create_small_config
config = create_small_config()
```

#### create_medium_config()
```python
from nlm import create_medium_config
config = create_medium_config()
```

#### create_large_config()
```python
from nlm import create_large_config
config = create_large_config()
```

### Error Handling

#### NLMError
```python
from nlm.error_handling import NLMError, ConfigurationError, SimulationError

try:
    # Your code here
    simulation.run_simulation(1000)
except ConfigurationError as e:
    print(f"Configuration error: {e}")
    print("Suggestions:", e.suggestions)
except SimulationError as e:
    print(f"Simulation error: {e}")
    print("Suggestions:", e.suggestions)
```

#### Error Handler
```python
from nlm.error_handling import get_error_handler, handle_error

error_handler = get_error_handler()

# Handle errors with recovery
try:
    simulation.run_simulation(1000)
except Exception as e:
    recovered = handle_error(
        e, "SIMULATION_ERROR",
        recovery_strategy="recover"
    )
```

## Configuration Management

### Configuration Files

NLM supports multiple configuration formats:

#### JSON Configuration
```json
{
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "random_seed": 42,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_tau": 20.0
}
```

#### INI Configuration
```ini
[NLM Configuration]
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
random_seed = 42

[Neural Dynamics]
lif_threshold = -55.0
lif_reset_potential = -70.0
lif_resting_potential = -70.0
lif_leak_conductance = 10.0

[Plasticity Parameters]
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
```

### Configuration Templates

Create configuration templates using `nlm-config create`:

```bash
nlm-config create my_config
```

This creates a `configs/my_config.json` file with default settings.

### Configuration Validation

Validate configurations using `nlm-config validate`:

```bash
nlm-config validate configs/my_config.json
```

Or validate programmatically:

```python
from nlm.error_handling import validate_configuration, ConfigurationError

config = {"neuron_count": 1000, "region_count": 2, "connection_probability": 0.1}

try:
    if validate_configuration(config):
        print("Configuration is valid")
    else:
        print("Configuration is invalid")
except ConfigurationError as e:
    print(f"Error: {e}")
```

### Configuration Export/Import

Export configurations in different formats:

```bash
# Export to JSON
nlm-config export my_config --format json > config.json

# Export to INI
nlm-config export my_config --format ini > config.ini
```

## Error Handling

### Error Types

NLM provides several types of errors:

#### ConfigurationError
Errors related to configuration handling.

#### SimulationError
Errors related to simulation execution.

#### BrainError
Errors related to brain initialization or operation.

#### NLMError
Base error class for all NLM errors.

### Error Information

Each error provides comprehensive information:

- **Error Code**: Specific error code
- **Error Message**: Detailed error description
- **Suggestions**: Recovery suggestions
- **Context**: Additional context information
- **Timestamp**: When the error occurred

### Error Handling Examples

```python
from nlm.error_handling import (
    NLMError, ConfigurationError, SimulationError, BrainError,
    handle_error, create_error_context
)

# Basic error handling
try:
    simulation.run_simulation(1000)
except ConfigurationError as e:
    print(f"Configuration problem: {e}")
    print("Suggestions:", e.suggestions)
except SimulationError as e:
    print(f"Simulation problem: {e}")
    print("Suggestions:", e.suggestions)

# Error context for specific code blocks
with create_error_context("SIMULATION_ERROR", recovery_strategy="recover"):
    simulation.run_simulation(1000)

# Global error handling
from nlm.error_handling import handle_error

try:
    simulation.run_simulation(1000)
except Exception as e:
    recovered = handle_error(
        e, "SIMULATION_ERROR",
        recovery_strategy="recover"
    )
```

### Error Logging

Errors are automatically logged to `nlm_errors.log`:

```bash
# Check error log
nlm-errors.log
```

## Visualization

### Basic Brain Visualization

```python
from nlm.examples.visualization_examples import example_basic_brain_visualization
example_basic_brain_visualization()
```

### Connectivity Visualization

```python
from nlm.examples.visualization_examples import example_connectivity_visualization
example_connectivity_visualization()
```

### Activity Animation

```python
from nlm.examples.visualization_examples import example_activity_animation
example_activity_animation()
```

### Comparison Visualization

```python
from nlm.examples.visualization_examples import example_comparison_visualization
example_comparison_visualization()
```

### Progress Visualization

```python
from nlm.examples.visualization_examples import example_progress_visualization
example_progress_visualization()
```

## Experiment Management

### Running Experiments

```python
from nlm import get_experiment_manager, BrainSimulation, create_medium_config

# Create brain and experiment manager
brain = BrainSimulation(create_medium_config())
brain.initialize()
experiment_manager = get_experiment_manager()

# Run specific experiment
results = experiment_manager.run_experiment(
    brain, "working_memory",
    trials=10, steps_per_trial=100
)

# Run all experiments
all_results = experiment_manager.run_all_experiments(
    brain, trials=5, steps_per_trial=50
)
```

### Available Experiments

- **temporal_prediction**: Temporal prediction learning
- **working_memory**: Working memory capacity testing
- **episodic_recall**: Episodic memory recall accuracy
- **concept_formation**: Concept formation and generalization
- **attention**: Neural attention mechanisms
- **planning**: Multi-step planning abilities
- **self_model**: Self-model development
- **social_learning**: Social learning and imitation
- **continual_learning**: Continual learning and adaptation
- **phase4_integrated**: Integrated Phase 4 experiments

## Examples

### Basic Usage Examples

See `examples/basic_usage.py` for:

- Basic brain simulation
- Configuration management
- Quick demo
- Simulation with sensory input
- Configuration comparison
- Error handling
- Save and load

### Experiment Management Examples

See `examples/experiment_templates.py` for:

- Experiment manager setup
- Single experiment
- All experiments
- Experiment with input
- Results analysis
- Experiment reproduction
- Parameter sweeps

### Visualization Examples

See `examples/visualization_examples.py` for:

- Basic brain visualization
- Connectivity visualization
- Activity animation
- Comparison visualization
- Progress visualization

### How to Run Examples

```bash
# Run basic usage examples
python -c "import nlm.examples.basic_usage as examples; examples.example_basic_usage()"

# Run experiment examples
python -c "import nlm.examples.experiment_templates as examples; examples.example_single_experiment()"

# Run visualization examples
python -c "import nlm.examples.visualization_examples as examples; examples.example_progress_visualization()"
```

## Best Practices

### Configuration Management

1. **Use appropriate configuration sizes**:
   - Small for testing and debugging
   - Medium for regular experiments
   - Large for intensive research

2. **Validate configurations before running**:
   ```python
   from nlm.error_handling import validate_configuration
   if validate_configuration(config):
       # Run simulation
   ```

### Simulation Management

1. **Start with simple simulations**:
   ```python
   simulation = BrainSimulation(create_small_config())
   simulation.initialize()
   results = simulation.run_simulation(100)
   ```

2. **Use checkpoints for long simulations**:
   ```python
   results = simulation.run_simulation(5000, enable_checkpoints=True)
   ```

3. **Monitor progress and resources**:
   ```python
   # Check simulation status
   simulation.get_brain_state()
   ```

### Error Handling

1. **Use built-in error recovery**:
   ```python
   from nlm.error_handling import handle_error
   handle_error(error, recovery_strategy="recover")
   ```

2. **Handle different error types**:
   ```python
   except ConfigurationError as e:
       print(f"Configuration problem: {e}")
   except SimulationError as e:
       print(f"Simulation problem: {e}")
   ```

3. **Use error context for specific code blocks**:
   ```python
   with create_error_context("SIMULATION_ERROR"):
       # Your code here
   ```

### Experiment Management

1. **Design experiments properly**:
   - Keep parameters constant except the one being tested
   - Use randomization to avoid order effects
   - Document all experimental conditions

2. **Run controlled experiments**:
   ```python
   # Run multiple conditions
   for condition in conditions:
       results = experiment_manager.run_experiment(
           brain, "working_memory",
           trials=condition["trials"],
           steps_per_trial=condition["steps"]
       )
   ```

### Visualization

1. **Use appropriate visualization methods**:
   - 2D plots for quick insights
   - 3D plots for multi-dimensional data
   - Animations for time-series data

2. **Follow visualization best practices**:
   - Label all axes and provide clear titles
   - Use appropriate color schemes and scales
   - Include statistical indicators where appropriate

### Documentation and Code Quality

1. **Write self-documenting code**:
   - Use descriptive variable and function names
   - Add docstrings to all public functions
   - Include type hints where possible

2. **Maintain comprehensive documentation**:
   - Keep README files updated with examples
   - Document configuration parameters and their effects
   - Provide troubleshooting guides for common issues

## Troubleshooting

### Common Issues

#### Configuration Errors
- **Issue**: Configuration file not found or invalid format
- **Solution**: Use `nlm-config validate <file>` to check configuration

#### Simulation Errors
- **Issue**: Simulation runs too slowly or hangs
- **Solution**: Reduce configuration size or check for infinite loops

#### Brain Initialization Errors
- **Issue**: Brain fails to initialize
- **Solution**: Check configuration values and verify initialization

#### Memory System Errors
- **Issue**: Memory systems run out of capacity
- **Solution**: Increase memory capacity or reduce simulation complexity

#### Experiment Errors
- **Issue**: Experiment runs for too long or fails
- **Solution**: Reduce number of trials or check experiment implementation

#### Visualization Errors
- **Issue**: Visualization fails or produces empty plots
- **Solution**: Check that visualization libraries are installed and data is available

### Getting Help

#### Online Resources
- **Documentation**: https://nlm.readthedocs.io
- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Community Forum**: https://community.nlm-project.org

#### Troubleshooting Guide
See `docs/TROUBLESHOOTING.md` for detailed troubleshooting guidance.

## API Reference

### Core Classes

#### BrainSimulation
- `__init__(config=None)`
- `initialize()`
- `run_simulation(steps, enable_checkpoints=False)`
- `inject_sensory_input(input_data, input_type='internal')`
- `get_brain_state()`
- `save_simulation(filepath)`
- `load_simulation(filepath)`

#### ExperimentManager
- `__init__()`
- `run_experiment(brain, experiment_name, trials=10, steps_per_trial=100)`
- `run_all_experiments(brain, trials=10, steps_per_trial=100)`

#### NLMConfigManager
- `__init__(config_dir='configs')`
- `create_config(name, config_data=None)`
- `load_config(name)`
- `validate_config(config)`
- `show_config(name)`
- `list_configs()`
- `export_config(name, output_format='ini')`

### Error Classes

- `NLMError`
- `ConfigurationError`
- `SimulationError`
- `BrainError`

### Utility Functions

- `handle_error(error, error_code='UNKNOWN_ERROR', context=None, recovery_strategy='log_and_reraise')`
- `validate_configuration(config)`
- `format_error_for_display(error)`
- `create_error_context(error_code, recovery_strategy='log_and_reraise', context=None)`
- `create_default_config()`
- `create_small_config()`
- `create_medium_config()`
- `create_large_config()`

## Acknowledgments

This enhanced NLM framework builds upon the original NLM (熙然) project and provides comprehensive improvements for research and education in neural simulation.

Special thanks to all contributors and users who have helped develop and improve this framework.

## License

This project is licensed under the MIT License.

See `LICENSE` file for details.