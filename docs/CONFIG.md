# NLM Configuration Reference Guide

This document provides comprehensive documentation for all configuration options in the NLM (Neural Learning Machine) project. It includes parameter descriptions, validation rules, command-line usage, and examples.

## Overview

The NLM configuration system manages all simulation parameters through a flexible key-value system. Configuration can be loaded from files, command-line arguments, or set programmatically at runtime.

Configuration sources (in order of precedence):
1. Command-line arguments (highest priority)
2. Configuration files
3. Runtime configuration calls
4. Default values (lowest priority)

## Configuration Files

### Format
NLM uses a simple key=value format for configuration files:

```
# This is a comment
key = value
another_key = "quoted value"
third_key = 42
```

### File Locations
- `configs/default.cfg` - Project default configuration
- Any file specified with `--config` command-line argument

### Configuration File Examples

#### Basic Configuration
```
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
```

#### Advanced Configuration
```
# Simulation parameters
simulation_timestep = 0.001
max_simulation_steps = 100000

# Neural network configuration
neuron_count = 5000
region_count = 3
connection_probability = 0.05

# Synaptic weights
initial_weight_mean = 0.5
initial_weight_std = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development
synaptogenesis_rate = 0.001
pruning_rate = 0.0001

# Neuromodulation
dopamine_baseline = 0.1

# World/environment
environment_name = GridWorld
environment_width = 20
environment_height = 20
```

## Command-Line Arguments

### Syntax
```bash
./nlm --config myconfig.cfg --neuron_count 2000 --random_seed 123
./nlm -n 1500 -s 456
```

### Available Command-Line Options

| Option | Long Form | Description | Type | Example |
|--------|-----------|-------------|------|---------|
| `-n` | `--neuron_count` | Number of neurons in the brain | int64_t | `-n 1000` |
| `-r` | `--region_count` | Number of brain regions | int64_t | `-r 2` |
| `-c` | `--connection_probability` | Probability of connections between neurons | float | `-c 0.1` |
| `-t` | `--simulation_timestep` | Time step size in seconds | double | `-t 0.001` |
| `-s` | `--random_seed` | Random seed for reproducible experiments | int64_t | `-s 42` |
| `-f` | `--config` | Configuration file path | string | `-f config/myconfig.cfg` |
| `-w` | `--environment_width` | Width of simulation world | int | `-w 20` |
| `-h` | `--environment_height` | Height of simulation world | int | `-h 20` |
| `--stdp_ltp_weight` | `--stdp_ltp_weight` | STDP long-term potentiation weight | float | `--stdp_ltp_weight 0.02` |
| `--stdp_ltd_weight` | `--stdp_ltd_weight` | STDP long-term depression weight | float | `--stdp_ltd_weight 0.015` |
| `--stdp_tau` | `--stdp_tau` | STDP time constant (ms) | float | `--stdp_tau 20.0` |

### Command-Line Usage Examples

#### Simple Run
```bash
./nlm --neuron_count 500 --region_count 1
```

#### With Configuration File
```bash
./nlm --config myconfig.cfg --random_seed 123
```

#### Override Multiple Parameters
```bash
./nlm --neuron_count 2000 --connection_probability 0.05 --stdp_ltp_weight 0.02
```

#### Short Form
```bash
./nlm -n 1000 -r 2 -c 0.1 -t 0.001 -s 42
```

## Detailed Parameter Reference

### Neural Network Parameters

| Parameter | Type | Default | Valid Range | Description |
|-----------|------|---------|-------------|-------------|
| `neuron_count` | int64_t | 1000 | ≥ 1 | Total number of neurons in the brain |
| `region_count` | int64_t | 2 | ≥ 1 | Number of brain regions |
| `connection_probability` | float | 0.1 | [0, 1] | Probability of creating synapses between neurons |
| `initial_weight_mean` | float | 0.5 | (-∞, ∞) | Mean of initial synaptic weight distribution |
| `initial_weight_std` | float | 0.1 | [0, ∞) | Standard deviation of initial synaptic weight distribution |

### Simulation Parameters

| Parameter | Type | Default | Valid Range | Description |
|-----------|------|---------|-------------|-------------|
| `simulation_timestep` | double | 0.001 | > 0 | Time step size in seconds |
| `max_simulation_steps` | int64_t | 10000 | ≥ 0 | Maximum number of simulation steps |
| `simulation_time_limit` | double | 0.0 | ≥ 0 | Time limit in seconds (0 = unlimited) |

### Plasticity Parameters

| Parameter | Type | Default | Valid Range | Description |
|-----------|------|---------|-------------|-------------|
| `plasticity_learning_rate` | float | 0.01 | [0, ∞) | Global learning rate for plasticity mechanisms |
| `stdp_ltp_weight` | float | 0.01 | (-∞, ∞) | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.012 | (-∞, ∞) | STDP long-term depression weight |
| `stdp_tau` | float | 20.0 | > 0 | STDP time constant (milliseconds) |

### Development Parameters

| Parameter | Type | Default | Valid Range | Description |
|-----------|------|---------|-------------|-------------|
| `synaptogenesis_rate` | float | 0.001 | [0, ∞) | Rate of new synapse formation |
| `pruning_rate` | float | 0.0001 | [0, ∞) | Rate of synapse elimination |

### Neuromodulation Parameters

| Parameter | Type | Default | Valid Range | Description |
|-----------|------|---------|-------------|-------------|
| `dopamine_baseline` | float | 0.1 | (-∞, ∞) | Baseline dopamine level |

### Environment Parameters

| Parameter | Type | Default | Valid Range | Description |
|-----------|------|---------|-------------|-------------|
| `environment_name` | string | "GridWorld" | - | Name of the simulation environment |
| `environment_width` | int64_t | 10 | ≥ 1 | Width of the 2D environment |
| `environment_height` | int64_t | 10 | ≥ 1 | Height of the 2D environment |
| `visualization_enabled` | bool | false | - | Enable/disable visualization |
| `visualization_update_rate` | float | 30.0 | > 0 | Visualization update frequency (Hz) |

### Logging Parameters

| Parameter | Type | Default | Valid Range | Description |
|-----------|------|---------|-------------|-------------|
| `log_level` | string | "INFO" | Debug, Info, Warning, Error, Critical | Minimum log level |
| `log_to_file` | bool | false | - | Write logs to file |
| `log_filename` | string | "nlm.log" | - | Log file name |

## Configuration Validation Rules

### Required Parameters
The following parameters must be set:
- `neuron_count`: Must be a positive integer
- `region_count`: Must be at least 1
- `simulation_timestep`: Must be positive

### Type Validation
Each parameter must match its expected type:
- `int64_t` parameters: Accept both positive and negative integers
- `float` parameters: Accept decimal values
- `double` parameters: Accept high-precision decimal values
- `bool` parameters: Accept true/false values
- `string` parameters: Accept quoted strings

### Range Validation
| Parameter | Minimum | Maximum | Notes |
|-----------|---------|---------|-------|
| `neuron_count` | 1 | ∞ | Must be positive |
| `region_count` | 1 | ∞ | Must be at least 1 |
| `connection_probability` | 0 | 1 | Cannot exceed 1 |
| `simulation_timestep` | > 0 | ∞ | Cannot be zero or negative |
| `plasticity_learning_rate` | 0 | ∞ | Cannot be negative |
| `stdp_ltp_weight` | -∞ | ∞ | No restrictions |
| `stdp_ltd_weight` | -∞ | ∞ | No restrictions |
| `stdp_tau` | > 0 | ∞ | Must be positive |
| `synaptogenesis_rate` | 0 | ∞ | Cannot be negative |
| `pruning_rate` | 0 | ∞ | Cannot be negative |
| `dopamine_baseline` | -∞ | ∞ | No restrictions |

### Default Values
If parameters are not explicitly set, the following defaults are used:

```
neuron_count = 1000
region_count = 2
connection_probability = 0.1
simulation_timestep = 0.001
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
synaptogenesis_rate = 0.001
pruning_rate = 0.0001
dopamine_baseline = 0.1
environment_name = "GridWorld"
environment_width = 10
environment_height = 10
log_level = "INFO"
log_to_file = false
log_filename = "nlm.log"
max_simulation_steps = 10000
simulation_time_limit = 0.0
visualization_enabled = false
visualization_update_rate = 30.0
```

## Configuration Validation

### Validation Process
After loading all configuration sources, NLM validates all configuration values:

1. **Type Validation**: Ensures values match expected types
2. **Range Validation**: Ensures values are within acceptable ranges
3. **Dependency Validation**: Ensures parameter combinations are valid
4. **Warning Generation**: Issues warnings for potentially problematic values
5. **Error Generation**: Reports errors for invalid values

### Validation Examples

#### Valid Configuration
```
# Valid configuration
neuron_count = 500
connection_probability = 0.15
stdp_tau = 25.0
```

#### Invalid Configuration
```
# This will generate warnings/errors
neuron_count = -100    # Invalid: negative neuron count
connection_probability = 1.5    # Invalid: > 1.0
stdp_tau = 0    # Invalid: zero or negative
```

## Configuration Validation Report

### Example Validation Report
```
=== Configuration Validation Report ===
neuron_count: 1000 (type: int64_t, source: File)
region_count: 2 (type: int64_t, source: CommandLine)
connection_probability: 0.1 (type: double, source: Default)
simulation_timestep: 0.001 (type: double, source: Default)
plasticity_learning_rate: 0.01 (type: double, source: File)
stdp_ltp_weight: 0.02 (type: double, source: CommandLine)
stdp_ltd_weight: 0.015 (type: double, source: Default)
stdp_tau: 20.0 (type: double, source: Default)
synaptogenesis_rate: 0.001 (type: double, source: Default)
pruning_rate: 0.0001 (type: double, source: Default)
dopamine_baseline: 0.1 (type: double, source: File)
environment_name: "GridWorld" (type: string, source: Default)
environment_width: 20 (type: int64_t, source: CommandLine)
environment_height: 20 (type: int64_t, source: CommandLine)
log_level: "INFO" (type: string, source: Default)
log_to_file: false (type: bool, source: File)
log_filename: "nlm.log" (type: string, source: Default)
max_simulation_steps: 10000 (type: int64_t, source: Default)
simulation_time_limit: 0.0 (type: double, source: Default)
visualization_enabled: false (type: bool, source: Default)
visualization_update_rate: 30.0 (type: double, source: Default)
=== End Validation Report ===
Config validation passed: All configuration values are valid
```

## Configuration Templates

### Template: Small Network (Debugging)
```
# Small network for quick testing
neuron_count = 100
region_count = 1
connection_probability = 0.2
simulation_timestep = 0.001
log_level = "Debug"
max_simulation_steps = 1000
```

### Template: Medium Network (Development)
```
# Medium-sized network for development
neuron_count = 1000
region_count = 2
connection_probability = 0.1
simulation_timestep = 0.001
log_level = "Info"
max_simulation_steps = 10000
plasticity_learning_rate = 0.01
stdp_tau = 20.0
```

### Template: Large Network (Performance Testing)
```
# Large network for performance testing
neuron_count = 10000
region_count = 4
connection_probability = 0.05
simulation_timestep = 0.0005
log_level = "Warning"
max_simulation_steps = 100000
plasticity_learning_rate = 0.005
stdp_tau = 30.0
synaptogenesis_rate = 0.0005
pruning_rate = 0.00005
```

### Template: Research Configuration
```
# Configuration for research experiments
neuron_count = 5000
region_count = 3
connection_probability = 0.08
simulation_timestep = 0.001
max_simulation_steps = 1000000
simulation_time_limit = 3600.0  # 1 hour
plasticity_learning_rate = 0.001
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.018
stdp_tau = 25.0
synaptogenesis_rate = 0.002
pruning_rate = 0.0002
dopamine_baseline = 0.5
```

## Configuration Best Practices

### 1. File Organization
- Use descriptive names for configuration files (e.g., `debug.cfg`, `production.cfg`)
- Store configuration files in a `configs/` directory
- Use version control for common configuration patterns

### 2. Parameter Validation
- Always validate configuration before running simulations
- Use validation reports to debug configuration issues
- Test configuration changes in small simulations first

### 3. Environment Separation
- Use different configurations for development, testing, and production
- Keep sensitive settings (like random seeds) separate for reproducibility
- Use environment variables for machine-specific settings

### 4. Documentation
- Document custom parameters in configuration files
- Include comments explaining parameter choices
- Keep configuration files self-documenting

### 5. Performance Considerations
- For large simulations, increase `neuron_count` and decrease `connection_probability`
- Use smaller `simulation_timestep` for stability, larger for performance
- Balance plasticity rates with simulation duration

## Configuration Troubleshooting

### Common Issues and Solutions

#### "Invalid numeric value" Errors
**Cause**: Non-numeric values for numeric parameters
**Solution**: Ensure values are properly formatted numbers

#### Parameter Not Found
**Cause**: Typo in parameter name
**Solution**: Check parameter names against valid parameters

#### Configuration File Not Found
**Cause**: Incorrect file path or file doesn't exist
**Solution**: Verify file path and file permissions

#### Dependency Errors
**Cause**: Conflicting parameter values
**Solution**: Review validation report and fix conflicting parameters

## Configuration Automation

### Environment Variables
NLM supports configuration through environment variables:

```bash
export NLM_NEURON_COUNT=1000
export NLM_RANDOM_SEED=42
export NLM_CONNECTION_PROBABILITY=0.1
```

### Configuration Scripts
Create shell scripts for common configuration patterns:

```bash
#!/bin/bash
# run_debug.sh - Run NLM in debug mode
./nlm --neuron_count 200 --region_count 1 --log_level Debug --max_simulation_steps 500

#!/bin/bash
# run_production.sh - Run NLM in production mode
./nlm --config configs/production.cfg
```

## Configuration Migration

### From Version 1.x to 2.x
When upgrading from older versions:

1. Parameter names have remained the same
2. Default values have been standardized
3. New validation rules have been added
4. Configuration file format supports additional parameters

Migration is generally straightforward - simply use existing configuration files.

## Appendix: Configuration Sources Priority

Configuration sources are evaluated in order of precedence (later sources override earlier ones):

1. Runtime configuration calls (highest priority)
2. Command-line arguments
3. Configuration files
4. Default values (lowest priority)

This means command-line arguments override configuration files, and programmatic calls override everything else.

## Configuration Schema Reference

### Configuration Value Types
NLM supports the following configuration value types:

- `int64_t`: 64-bit signed integers
- `double`: Floating-point numbers with high precision
- `float`: Floating-point numbers
- `bool`: Boolean values (true/false)
- `std::string`: Text strings
- `std::vector<int>`: Integer arrays
- `std::vector<double>`: Double arrays
- `std::vector<std::string>`: String arrays

### Configuration Type Conversion
NLM automatically handles type conversion where possible:

```
"42"           # Can be converted to int64_t, double
"0.5"          # Can be converted to float, double
"true"         # Can be converted to bool
"hello"        # Remains as string
```

## Version Information

This configuration reference applies to NLM Phase 2.
For information about future configuration changes, refer to the release notes.

Last updated: Current date
Version: 2.0

---

For technical support or configuration questions, refer to the NLM documentation or contact the development team.