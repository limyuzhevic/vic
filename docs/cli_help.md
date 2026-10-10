# NLM CLI Help

## Overview

NLM (熙然) - Neural Learning Machine provides comprehensive configuration management through both configuration files and command-line interface (CLI). This document describes the available CLI options and configuration parameters.

## Configuration File Options

### File Format

NLM supports multiple configuration file formats with automatic detection:

- **JSON** (`.json`): Modern, structured format
- **YAML** (`.yaml`, `.yml`): Human-readable format
- **Legacy** (`.cfg`, `.ini`): Simple key=value format

### Command Line Options

#### `--config <file>`
- **Short**: `-c`
- **Description**: Configuration file path
- **Default**: `configs/default.cfg`
- **Example**: `--config configs/production.json`

#### `--verbose, -v`
- **Type**: Flag
- **Description**: Enable verbose logging
- **Default**: Disabled

#### Simulation Parameters

##### `--neuron-count <count>`
- **Description**: Number of neurons in the brain
- **Default**: 500
- **Range**: 1-1,000,000
- **Example**: `--neuron-count 2000`

##### `--region-count <count>`
- **Description**: Number of brain regions
- **Default**: 1
- **Range**: 1-1000
- **Example**: `--region-count 3`

##### `--simulation-timestep <timestep>`
- **Description**: Simulation timestep in seconds
- **Default**: 0.001
- **Range**: 0.0001-1.0
- **Example**: `--simulation-timestep 0.0005`

##### `--connection-probability <probability>`
- **Description**: Connection probability for random connectivity
- **Default**: 0.15
- **Range**: 0.0-1.0
- **Example**: `--connection-probability 0.2`

#### STDP Parameters

##### `--stdp-ltp-weight <weight>`
- **Description**: STDP LTP weight
- **Default**: 0.02
- **Range**: 0.0-0.1

##### `--stdp-ltd-weight <weight>`
- **Description**: STDP LTD weight
- **Default**: 0.015
- **Range**: 0.0-0.1

##### `--stdp-tau <tau>`
- **Description**: STDP time constant
- **Default**: 20.0
- **Range**: 1.0-100.0

#### Structural Plasticity Parameters

##### `--synaptogenesis-rate <rate>`
- **Description**: Synaptogenesis rate
- **Default**: 0.0001
- **Range**: 0.0-1.0

##### `--pruning-rate <rate>`
- **Description**: Pruning rate
- **Default**: 0.00001
- **Range**: 0.0-1.0

#### Environment Parameters

##### `--environment-name <name>`
- **Description**: Environment name
- **Default**: GridWorld

##### `--environment-width <width>`
- **Description**: Environment width
- **Default**: 10
- **Range**: 1-1000

##### `--environment-height <height>`
- **Description**: Environment height
- **Default**: 10
- **Range**: 1-1000

#### Simulation Parameters

##### `--max-simulation-steps <steps>`
- **Description**: Maximum simulation steps
- **Default**: 10000
- **Range**: 0-1,000,000

##### `--simulation-time-limit <limit>`
- **Description**: Simulation time limit in seconds
- **Default**: 0.0
- **Range**: 0.0-1,000,000.0

#### Visualization Parameters

##### `--visualization-enabled`
- **Type**: Flag
- **Description**: Enable visualization
- **Default**: Disabled

##### `--visualization-update-rate <rate>`
- **Description**: Visualization update rate in Hz
- **Default**: 30.0
- **Range**: 0.0-1000.0

#### Random Seed

##### `--random-seed <seed>`
- **Description**: Random seed for reproducible experiments
- **Default**: 42
- **Range**: 0-2,147,483,647

## Environment Variables

NLM supports environment variables for configuration:

- `NLM_RANDOM_SEED`: Random seed
- `NLM_SIMULATION_TIMESTEP`: Simulation timestep
- `NLM_NEURON_COUNT`: Number of neurons
- `NLM_REGION_COUNT`: Number of regions
- `NLM_CONNECTION_PROBABILITY`: Connection probability
- `NLM_STDP_LTP_WEIGHT`: STDP LTP weight
- `NLM_STDP_LTD_WEIGHT`: STDP LTD weight
- `NLM_STDP_TAU`: STDP time constant
- `NLM_SYNAPTAGENESIS_RATE`: Synaptogenesis rate
- `NLM_PRUNING_RATE`: Pruning rate
- `NLM_ENVIRONMENT_NAME`: Environment name
- `NLM_ENVIRONMENT_WIDTH`: Environment width
- `NLM_ENVIRONMENT_HEIGHT`: Environment height
- `NLM_LOG_LEVEL`: Logging level
- `NLM_LOG_TO_FILE`: Log to file (true/false)
- `NLM_LOG_FILENAME`: Log filename
- `NLM_MAX_SIMULATION_STEPS`: Maximum simulation steps
- `NLM_SIMULATION_TIME_LIMIT`: Simulation time limit
- `NLM_VISUALIZATION_ENABLED`: Enable visualization (true/false)
- `NLM_VISUALIZATION_UPDATE_RATE`: Visualization update rate

## Configuration File Examples

### Basic Configuration (`configs/default.cfg`)

```
# NLM Configuration File
# This is the default configuration for Phase 1 skeleton

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

### JSON Configuration (`configs/production.json`)

```json
{
  "random_seed": 12345,
  "simulation_timestep": 0.001,
  "neuron_count": 5000,
  "region_count": 3,
  "connection_probability": 0.15,
  "stdp_ltp_weight": 0.02,
  "stdp_ltd_weight": 0.015,
  "stdp_tau": 20.0,
  "synaptogenesis_rate": 0.0001,
  "pruning_rate": 0.00001,
  "environment_name": "ComplexWorld",
  "environment_width": 50,
  "environment_height": 50,
  "visualization_enabled": true,
  "visualization_update_rate": 30.0,
  "log_level": "INFO",
  "log_to_file": true,
  "log_filename": "nlm_simulation.log",
  "max_simulation_steps": 100000,
  "simulation_time_limit": 0.0
}
```

### YAML Configuration (`configs/debug.yml`)

```yaml
# NLM Debug Configuration

random_seed: 999
simulation_timestep: 0.01
neuron_count: 100
region_count: 1
connection_probability: 0.3
visualization_enabled: true
log_level: Debug
log_to_file: true
log_filename: nlm_debug.log
max_simulation_steps: 1000
```

## Configuration Templates

NLM provides several configuration templates:

### Basic Template (`configs/templates/basic.json`)
A minimal configuration suitable for testing and initial setup.

### Advanced Template (`configs/templates/advanced.json`)
A comprehensive configuration with all advanced features enabled.

### Debug Template (`configs/templates/debug.json`)
A configuration optimized for debugging and development.

## Configuration Templates Presets

### Quick Test (`configs/presets/quick_test.json`)
A minimal configuration for quick testing of basic functionality.

### Full Simulation (`configs/presets/full_simulation.json`)
A comprehensive configuration suitable for full-scale simulations.

### Research (`configs/presets/research.json`)
A configuration optimized for research experiments with detailed logging.

## Configuration File Operations

### Loading Configuration

Configurations are loaded in the following order of precedence (highest to lowest):

1. **Command Line Arguments**: Override all other sources
2. **Environment Variables**: Override configuration files
3. **Configuration Files**: Multiple files can be specified
4. **Default Values**: Built-in defaults
5. **Runtime Values**: Set during execution

### Saving Configuration

Configuration can be saved to file in JSON format:

```bash
# Save configuration to file
nlm --config saved_config.json
```

### Configuration Backup

Automatic backups are created before saving changes:

```bash
# Create a backup
nlm --config my_config.json

# Restore from backup
nlm --config backup.json
```

## Configuration Validation

The enhanced configuration system includes comprehensive validation:

### Parameter Validation

- **Type Validation**: Ensures parameters are of the correct type
- **Range Validation**: Checks parameter values against allowed ranges
- **Size Validation**: Validates array lengths for list parameters
- **Pattern Validation**: Supports regular expressions for string parameters

### Cross-Parameter Validation

- **Neuron-Region Ratio**: Ensures neuron_count >= region_count * 10
- **Time Step Validation**: Ensures simulation timestep is reasonable
- **Probability Validation**: Ensures connection probability is within [0, 1]

### Error Handling

Detailed error messages are provided for validation failures:

```
Invalid parameter 'neuron_count': Expected integer64, got string
Cross-parameter validation failed: neuron_count (100) should be at least 10 times region_count (20) for proper connectivity.
```

## Examples

### Basic Usage

```bash
# Run with default configuration
nlm

# Run with custom configuration file
nlm --config configs/production.json

# Run with verbose logging
nlm --verbose

# Run with custom simulation parameters
nlm --neuron-count 2000 --region-count 3 --simulation-timestep 0.0005

# Run with environment variable override
export NLM_RANDOM_SEED=12345
nlm
```

### Production Environment

```bash
# Configure for production environment
export NLM_ENVIRONMENT_NAME=ProductionWorld
export NLM_ENVIRONMENT_WIDTH=100
export NLM_ENVIRONMENT_HEIGHT=100
export NLM_LOG_LEVEL=INFO
export NLM_LOG_TO_FILE=true

# Run simulation
nlm --config configs/production.json
```

### Development Environment

```bash
# Configure for development environment
export NLM_RANDOM_SEED=999
export NLM_LOG_LEVEL=Debug
export NLM_VISUALIZATION_ENABLED=true

# Run with debug configuration
nlm --config configs/debug.yml --verbose
```

## Help Commands

### General Help

```bash
nlm --help
```

### Configuration File Help

```bash
nlm --config --help
```

### Detailed Parameter Help

Use the `--verbose` flag to see detailed parameter descriptions and validation rules.

## Troubleshooting

### Configuration File Not Found

If the specified configuration file is not found, NLM will fall back to the default configuration. To check which configuration file is being loaded:

```bash
nlm --verbose
```

### Invalid Configuration Values

Invalid values will result in descriptive error messages. The configuration will still be loaded with default values for invalid parameters.

### Permission Errors

Ensure that NLM has write permissions to the configuration directory when saving or creating backup files.

## Migration from Legacy Configuration

NLM supports migration from legacy configuration formats (key=value format). Use the configuration file conversion utility:

```bash
# Convert configuration file format
nlm --config input.cfg --convert output.json
```

## Configuration Management Commands

### List Available Configuration Files

The configuration system can list all configured files:

```bash
# This information is included in the verbose output
nlm --verbose
```

### Configuration Comparison

To compare two configurations:

```bash
# Generate configuration differences
nlm --config config1.json --diff config2.json
```

### Configuration Templates

To use configuration templates:

```bash
# Load a template
nlm --config configs/templates/basic.json

# Save current configuration as a template
nlm --config my_config.json --save-template my_template
```

## Conclusion

The enhanced configuration system provides:

- **Flexible Format Support**: JSON, YAML, and legacy formats with automatic detection
- **Comprehensive Validation**: Type, range, and cross-parameter validation
- **Robust CLI Integration**: CLI11 library with comprehensive options
- **Environment Variable Support**: Full support for environment-based configuration
- **Template System**: Built-in templates for common use cases
- **Backup and Recovery**: Automatic backup and restore functionality
- **Detailed Logging**: Comprehensive logging of configuration changes
- **Documentation**: Complete parameter documentation and examples

This system makes NLM easy to configure, maintain, and scale for different deployment scenarios.
