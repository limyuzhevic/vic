# NLM Configuration File Schema

## Overview
This document provides a comprehensive reference for all configuration parameters available in the NLM (熙然) computational brain project. The configuration system supports multiple data types and follows a precedence hierarchy for resolving configuration values.

## Configuration Hierarchy
NLM uses a configuration precedence system where values are resolved in the following order (highest to lowest):

1. **Command Line Arguments** (`--key value` format)
2. **Configuration File** (JSON or key=value format)
3. **Runtime `set()` calls**
4. **Default Values** (lowest precedence)

## Configuration File Format

### Supported File Formats
- **JSON files** (`.json` extension) - Recommended format
- **Plain text files** (`.cfg` or `.txt` extension) - Key=value format

### JSON Format Example
```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity_learning_rate": 0.01,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_time_constant": 20.0,
    "development_synaptogenesis_rate": 0.001,
    "development_pruning_rate": 0.0001,
    "dopamine_baseline": 0.1,
    "reward_discount_factor": 0.99,
    "environment_name": "GridWorld",
    "environment_width": 10,
    "environment_height": 10,
    "log_level": "INFO",
    "log_to_file": false,
    "log_filename": "nlm.log",
    "max_simulation_steps": 10000,
    "simulation_time_limit": 0.0,
    "visualization_enabled": false,
    "visualization_update_rate": 30.0
}
```

### Plain Text Format Example
```
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
dopamine_baseline = 0.1
reward_discount_factor = 0.99
environment_name = GridWorld
environment_width = 10
environment_height = 10
log_level = INFO
log_to_file = false
log_filename = nlm.log
max_simulation_steps = 10000
simulation_time_limit = 0.0
visualization_enabled = false
visualization_update_rate = 30.0
```

## Configuration Categories

### Core Simulation Parameters
These parameters control the basic simulation behavior and scale.

| Parameter | Type | Default | Description | Range/Valid Values |
|-----------|------|---------|-------------|-------------------|
| `random_seed` | int | 42 | Random seed for reproducible experiments | Any valid integer |
| `simulation_timestep` | double | 0.001 | Simulation timestep in seconds | (0.0, ∞) |
| `neuron_count` | int | 1000 | Number of neurons in the brain | ≥ 1 |
| `region_count` | int | 2 | Number of brain regions | ≥ 1 |
| `connection_probability` | double | 0.1 | Connection probability for random connectivity | [0.0, 1.0] |

### Plasticity Parameters
These parameters control synaptic plasticity mechanisms (STDP and Hebbian learning).

| Parameter | Type | Default | Description | Range/Valid Values |
|-----------|------|---------|-------------|-------------------|
| `plasticity_learning_rate` | double | 0.01 | Global learning rate multiplier for plasticity rules | [0.0, ∞) |
| `stdp_ltp_weight` | double | 0.01 | Spike-timing-dependent plasticity Long-Term Potentiation weight | [0.0, ∞) |
| `stdp_ltd_weight` | double | 0.012 | Spike-timing-dependent plasticity Long-Term Depression weight | [0.0, ∞) |
| `stdp_time_constant` | double | 20.0 | Time constant for STDP windows (in simulation steps) | [0.0, ∞) |

### Development Parameters
These parameters control developmental processes during brain development.

| Parameter | Type | Default | Description | Range/Valid Values |
|-----------|------|---------|-------------|-------------------|
| `development_synaptogenesis_rate` | double | 0.001 | Rate of new synapse formation during development | [0.0, ∞) |
| `development_pruning_rate` | double | 0.0001 | Rate of synapse elimination during development | [0.0, ∞) |

### Neuromodulation Parameters
These parameters control neuromodulatory systems that influence learning and behavior.

| Parameter | Type | Default | Description | Range/Valid Values |
|-----------|------|---------|-------------|-------------------|
| `dopamine_baseline` | double | 0.1 | Baseline dopamine level affecting neural excitability and plasticity | [0.0, ∞) |

### Reward Parameters
These parameters control reward prediction and learning systems.

| Parameter | Type | Default | Description | Range/Valid Values |
|-----------|------|---------|-------------|-------------------|
| `reward_discount_factor` | double | 0.99 | Discount factor for temporal reward prediction | [0.0, 1.0] |

### Environment Parameters
These parameters control the simulated environment where the agent operates.

| Parameter | Type | Default | Description | Range/Valid Values |
|-----------|------|---------|-------------|-------------------|
| `environment_name` | string | "GridWorld" | Name of the environment type | "GridWorld", "MountainCar", "Custom", etc. |
| `environment_width` | int | 10 | Width of the 2D environment | ≥ 1 |
| `environment_height` | int | 10 | Height of the 2D environment | ≥ 1 |

### Logging Parameters
These parameters control logging and debugging output.

| Parameter | Type | Default | Description | Range/Valid Values |
|-----------|------|---------|-------------|-------------------|
| `log_level` | string | "INFO" | Minimum log level to output | "Debug", "Info", "Warning", "Error", "Critical" |
| `log_to_file` | bool | false | Whether to log to file in addition to console | true, false |
| `log_filename` | string | "nlm.log" | Name of the log file when file logging is enabled | Any valid filename |

### Simulation Parameters
These parameters control simulation execution and termination.

| Parameter | Type | Default | Description | Range/Valid Values |
|-----------|------|---------|-------------|-------------------|
| `max_simulation_steps` | int | 10000 | Maximum number of simulation steps | ≥ 0 |
| `simulation_time_limit` | double | 0.0 | Absolute time limit for simulation (0 = unlimited) | [0.0, ∞) |

### Visualization Parameters
These parameters control visualization and debugging output.

| Parameter | Type | Default | Description | Range/Valid Values |
|-----------|------|---------|-------------|-------------------|
| `visualization_enabled` | bool | false | Whether to enable visualization system | true, false |
| `visualization_update_rate` | double | 30.0 | Visualization update rate (frames per second) | [1.0, ∞) |

## Configuration Validation

### Type Validation
NLM automatically validates configuration values:
- **Type Checking**: Values must match the expected type (int, double, bool, string, arrays)
- **Range Validation**: Numeric values are checked against valid ranges where applicable
- **Value Validation**: String values are checked against valid enums or lists

### Source Resolution
Each configuration parameter includes metadata about its source:
- **Default**: Built into the system as fallback values
- **File**: Loaded from configuration file (JSON or plain text)
- **Command Line**: Set via command line arguments (`--key value`)
- **Runtime**: Set programmatically during runtime via `set()` calls

Configuration files can include comments in both JSON and plain text formats.

## Loading Configuration

### Command Line Loading
```bash
./nlm --random_seed 123 --neuron_count 500 --log_level Debug
```

### File Loading
```bash
./nlm --config configs/experiment1.cfg
```

### Programmatic Loading
```cpp
#include "nlm/core/Config/Config.hpp"

nlm::Config config;
config.loadFromFile("configs/my_config.json");
config.set("neuron_count", 1500);
```

## Configuration Examples

### Research Experiment Configuration
```json
{
    "random_seed": 12345,
    "neuron_count": 5000,
    "region_count": 3,
    "connection_probability": 0.15,
    "plasticity_learning_rate": 0.02,
    "stdp_time_constant": 30.0,
    "environment_name": "GridWorld",
    "environment_width": 20,
    "environment_height": 20,
    "log_level": "Info",
    "log_to_file": true,
    "log_filename": "experiment_research.log",
    "max_simulation_steps": 50000,
    "visualization_enabled": true,
    "visualization_update_rate": 60.0
}
```

### Development/Debug Configuration
```json
{
    "random_seed": 999,
    "simulation_timestep": 0.0005,
    "neuron_count": 100,
    "region_count": 1,
    "connection_probability": 0.3,
    "plasticity_learning_rate": 0.1,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.02,
    "stdp_time_constant": 50.0,
    "dopamine_baseline": 0.5,
    "reward_discount_factor": 0.9,
    "environment_name": "GridWorld",
    "environment_width": 10,
    "environment_height": 10,
    "log_level": "Debug",
    "log_to_file": true,
    "log_filename": "debug.log",
    "max_simulation_steps": 1000,
    "simulation_time_limit": 60.0,
    "visualization_enabled": true,
    "visualization_update_rate": 30.0
}
```

### Minimal Configuration (Using Defaults)
```bash
# Only set parameters that differ from defaults
./nlm --neuron_count 2000 --connection_probability 0.05 --environment_width 15
```

## Configuration Key Reference

### Common Keys (Alphabetical)
- `connection_probability`: [0.0, 1.0] - Probability of random connections between neurons
- `critical`: Critical logging level (5)
- `debug`: Debug logging level (0)
- `development_pruning_rate`: Synapse elimination rate during development
- `development_synaptogenesis_rate`: New synapse formation rate during development
- `dopamine_baseline`: Baseline neuromodulation level
- `environment_height`: Environment height in cells
- `environment_name`: Type of environment simulation
- `environment_width`: Environment width in cells
- `error`: Error logging level (3)
- `log_filename`: Log file name when file logging is enabled
- `log_level`: Minimum log level (Debug, Info, Warning, Error, Critical)
- `log_to_file`: Enable file logging in addition to console
- `max_simulation_steps`: Maximum simulation steps before auto-termination
- `neuron_count`: Number of neurons in the brain
- `plasticity_learning_rate`: Global learning rate multiplier
- `random_seed`: Random number generator seed for reproducibility
- `reward_discount_factor`: Temporal discounting for reward prediction
- `region_count`: Number of brain regions
- `simulation_time_limit`: Absolute time limit in seconds (0 = unlimited)
- `simulation_timestep`: Time step duration in seconds
- `stdp_ltp_weight`: STDP Long-Term Potentiation weight
- `stdp_ltd_weight`: STDP Long-Term Depression weight
- `stdp_time_constant`: STDP time window constant in steps
- `warning`: Warning logging level (2)
- `visualization_enabled`: Enable visualization system
- `visualization_update_rate`: Visualization update rate in Hz

## Best Practices

1. **Use Relative Paths**: For configuration files, use relative paths from the project root
2. **Environment-Specific Configs**: Create separate config files for different environments (dev, test, prod)
3. **Version Control**: Store configuration files in version control for experiment reproducibility
4. **Logging**: Enable logging for debugging and monitoring configuration sources
5. **Reproducibility**: Set random_seed for reproducible experimental results
6. **Performance Tuning**: Adjust simulation_timestep and neuron_count for performance vs. accuracy trade-offs

This comprehensive configuration reference should help users understand all available parameters and use them effectively for their NLM experiments.