# Installation instructions for NLM Python package

# Quick Start

## Installation

NLM can be installed in two ways:

### Option 1: Using pip (Recommended)

```bash
# Install NLM from PyPI (if available)
pip install nlm

# Or install directly from source
pip install -e .
```

### Option 2: Using the build script

```bash
# Build NLM from source
./python/build_nlm.sh

# Install the Python package
pip install -e ./python
```

## Basic Usage

### Creating an NLM agent

```python
import nlm
from nlm import create_agent

# Create a basic agent with default configuration
agent = create_agent()

# Create an agent with a specific configuration file
agent = create_agent(config_file='my_config.json')

# Create an agent with additional configuration options
agent = create_agent(
    config_file='my_config.json',
    neuron_count=1000,
    simulation_timestep=0.01,
    plasticity_learning_rate=0.01
)
```

### Running a complete simulation

```python
import nlm
from nlm import NLM

# Create a complete NLM simulation instance
sim = NLM(config_file='my_config.json')

# Run a single step
results = sim.step()
print(f"Results: {results}")

# Run multiple episodes
for episode in range(100):
    results = sim.step()
    if results['done']:
        break
    print(f"Episode {episode + 1}, Reward: {results['rewards']}")

# Reset the simulation
sim.reset()

# Save the simulation state
sim.save('simulation_state.json')

# Load the simulation state
sim.load('simulation_state.json')
```

## Advanced Usage

### Using the Trainer class

```python
import nlm
from nlm import NLM, Trainer

# Create an NLM simulation
sim = NLM(config_file='my_config.json')

# Create a trainer
trainer = Trainer(sim)

# Run a single episode
episode_stats = trainer.run_episode(max_steps=1000, verbose=True)
print(f"Episode stats: {episode_stats}")

# Run multiple episodes
history = trainer.train(episodes=100, max_steps=1000, verbose=True)

# Get training statistics
print(f"Total episodes: {trainer.episode_count}")
print(f"Total steps: {trainer.step_count}")
print(f"Total reward: {trainer.total_reward}")
print(f"Average reward: {trainer.total_reward / trainer.episode_count if trainer.episode_count > 0 else 0}")
```

### Configuration management

```python
import nlm
from nlm import Config, load_config, save_config, get_config

# Load configuration from file
config = load_config('my_config.json')

# Access configuration values
neuron_count = config.getOr('neuron_count', 1000)
simulation_timestep = config.getOr('simulation_timestep', 0.001)

# Set configuration values
config.set('neuron_count', 500)
config.set('simulation_timestep', 0.005)

# Save configuration
save_config(config, 'updated_config.json')

# Get the global configuration
global_config = get_config()
```

### Logging

```python
import nlm
from nlm import Logger, LogLevel, setup_logging

# Setup logging with default configuration
setup_logging()

# Setup logging with specific level
Logger.getGlobal().setLevel(LogLevel::Debug)

# Log messages at different levels
Logger.getGlobal().info("Starting simulation")
Logger.getGlobal().warning("Performance warning: High memory usage")
Logger.getGlobal().error("Simulation failed: Division by zero")
Logger.getGlobal().critical("Critical error: System shutdown")

# Use convenience macros
NLM_LOG_INFO("This is an info message")
NLM_LOG_ERROR("This is an error message")
```

## Configuration Options

The default configuration file (`configs/default.cfg`) includes the following options:

### General Settings
- `random_seed`: Random seed for reproducible experiments
- `simulation_timestep`: Simulation timestep in seconds
- `neuron_count`: Number of neurons in the brain
- `region_count`: Number of brain regions
- `connection_probability`: Connection probability for random connectivity

### Plasticity Settings
- `plasticity_learning_rate`: Learning rate for plasticity
- `stdp_ltp_weight`: STDP LTP weight
- `stdp_ltd_weight`: STDP LTD weight
- `stdp_time_constant`: STDP time constant

### Development Settings
- `development_synaptogenesis_rate`: Synaptogenesis rate
- `development_pruning_rate`: Pruning rate

### Neuromodulation Settings
- `dopamine_baseline`: Baseline dopamine level
- `dopamine_release_rate`: Dopamine release rate
- `dopamine_reuptake_rate`: Dopamine reuptake rate

### Reward Settings
- `reward_discount_factor`: Discount factor for rewards
- `reward_scaling_factor`: Scaling factor for rewards

### Environment Settings
- `environment_name`: Name of the environment
- `environment_width`: Width of the environment
- `environment_height`: Height of the environment

### Logging Settings
- `log_level`: Logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)
- `log_to_file`: Whether to log to file
- `log_filename`: Log file name
- `log_use_colors`: Whether to use colored output

### Simulation Settings
- `max_simulation_steps`: Maximum number of simulation steps
- `simulation_time_limit`: Simulation time limit in seconds

### Visualization Settings
- `visualization_enabled`: Whether to enable visualization
- `visualization_update_rate`: Visualization update rate

## Configuration File Formats

NLM supports multiple configuration file formats:

### JSON (.json)
Modern format with nested objects and arrays:

```json
{
    "neuron_count": 1000,
    "simulation_timestep": 0.001,
    "brain": {
        "plasticity": {
            "learning_rate": 0.01
        }
    }
}
```

### YAML (.yaml/.yml)
Human-readable format with rich data types:

```yaml
neuron_count: 1000
simulation_timestep: 0.001
brain:
  plasticity:
    learning_rate: 0.01
```

### Legacy (.cfg/.txt)
Simple key=value format (backward compatible):

```
neuron_count = 1000
simulation_timestep = 0.001
plasticity_learning_rate = 0.01
```

## Command Line Interface

NLM supports command line argument parsing:

```bash
# Show help
nlm --help

# Load configuration from file
nlm --config=config.json

# Set configuration values
nlm --neuron_count=500 --simulation_timestep=0.01

# Verbose output
nlm --verbose --neuron_count=500

# Combined options
nlm -v -c=my_config.yaml -random_seed=123
```

## Exception Handling

NLM provides a comprehensive exception hierarchy:

### Base Exceptions
- `NLMException`: Base exception for all NLM errors
- `ConfigException`: Configuration-specific exceptions
- `LogException`: Logger-specific exceptions
- `BrainException`: Brain-specific exceptions
- `RandomException`: Random number generator exceptions

### Specific Exceptions
- `ValidationError`: Validation errors
- `InitializationError`: Initialization errors
- `ResourceError`: Resource allocation errors
- `OperationError`: Operation errors
- `ErrorCodeException`: Exceptions with error codes

### Catching Exceptions

```python
import nlm
from nlm import NLMException, ConfigException

try:
    sim = NLM(config_file='my_config.json')
    results = sim.step()
except NLMException as e:
    print(f"NLM error occurred: {e}")
except ConfigException as e:
    print(f"Configuration error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Performance Tips

### Memory Usage
- Monitor memory usage during long-running simulations
- Use smaller networks for memory-constrained environments
- Enable log file rotation to prevent disk space issues

### Simulation Speed
- Adjust simulation timestep for performance vs. accuracy trade-offs
- Use appropriate connection probability for network size
- Configure logging level to reduce overhead

### Logging Best Practices
- Use appropriate log levels to reduce overhead
- Enable log rotation to manage disk space
- Use structured logging for better analysis

## Troubleshooting

### Common Issues

#### 1. "ModuleNotFoundError: No module named 'nlm'"
Solution: Ensure the NLM Python package is installed:
```bash
pip install -e ./python
```

#### 2. "ImportError: cannot import name 'Config'"
Solution: Check that the Python package structure is correct:
```bash
# Create the package structure
mkdir -p python/nlm
mkdir -p python/nlm/core
mkdir -p python/nlm/brain
# ... and so on for all submodules
```

#### 3. "RuntimeError: Failed to load configuration"
Solution: Check that the configuration file exists and is readable:
```bash
# Check if file exists
ls -la my_config.json

# Check file permissions
chmod 644 my_config.json
```

#### 4. "ImportError: cannot import name 'pybind11'"
Solution: Install pybind11:
```bash
pip install pybind11
```

### Getting Help

If you encounter issues:

1. **Check the documentation**: Visit https://docs.nlm.org for comprehensive documentation
2. **Check the examples**: Look at the examples directory for working examples
3. **Search for solutions**: Search GitHub issues for similar problems
4. **Create an issue**: If you can't find a solution, create an issue on GitHub
5. **Contact support**: Email contact@nlm.org for commercial support

## Development

### Building from Source

To build NLM from source:

```bash
# Clone the repository
git clone https://github.com/nlmlab/nlm.git
cd nlm

# Build using the provided build script
./python/build_nlm.sh

# Install the Python package
pip install -e ./python
```

### Running Tests

To run the NLM tests:

```bash
# Navigate to the NLM root directory
cd nlm

# Run the Python tests
pytest python/tests/

# Run specific test modules
pytest python/tests/test_config.py
pytest python/tests/test_brain.py
```

### Contributing

We welcome contributions to the NLM project. To contribute:

1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Write tests for your changes
5. Submit a pull request

Please follow the project's coding standards and write comprehensive tests.

## License

This project is licensed under the MIT License. See the LICENSE file for more information.

## Acknowledgments

This work was supported by the Neural Learning Machine (NLM) project.

We thank our contributors and users for their support and feedback.