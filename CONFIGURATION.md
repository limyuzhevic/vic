# Configuration Documentation

This document provides comprehensive information about the NLM (Neural Learning Machine) configuration system.

## Overview

The NLM configuration system provides a robust and flexible way to manage parameters for neural computation experiments. It includes:

- **Schema-based validation** to ensure all configuration values are valid
- **Type safety** with runtime type checking
- **Multiple configuration sources** (file, command line, runtime) with proper precedence
- **Error handling and recovery** for robust operation
- **Backup and restore** capabilities
- **Documentation and introspection** features

## Configuration Sources

The configuration system supports four sources with different precedence levels:

1. **CommandLine** (highest priority) - Set via command line arguments
2. **File** - Loaded from configuration files
3. **Default** - Built-in default values
4. **Runtime** (lowest priority) - Set programmatically via API

When multiple sources provide values for the same parameter, the source with higher precedence overrides the lower priority ones.

## Configuration File Format

The configuration system uses a simple key=value format for configuration files:

```
# Comments start with # or /
key = value

# String values must be quoted
string_key = "value"

# Boolean values (case-insensitive)
bool_key = true
bool_key = false

# Vector values (comma-separated, no spaces)
vector_key = 1,2,3,4,5
vector_string = "a","b","c","d"
```

## Core Configuration Parameters

### Neural Computation Parameters

| Key | Type | Default | Description | Valid Range |
|-----|------|---------|-------------|-------------|
| `random_seed` | int64_t | 42 | Random seed for reproducible experiments (0 = use time-based) |
| `simulation_timestep` | double | 0.001 | Simulation timestep in seconds | > 0 |
| `neuron_count` | int64_t | 1000 | Number of neurons in the brain | >= 1 |
| `region_count` | int64_t | 2 | Number of brain regions | >= 1 |
| `connection_probability` | double | 0.1 | Connection probability for random connectivity | 0.0 - 1.0 |

### Plasticity Parameters

| Key | Type | Default | Description | Valid Range |
|-----|------|---------|-------------|-------------|
| `plasticity_learning_rate` | double | 0.01 | Global learning rate for plasticity rules | 0.0 - 1.0 |
| `stdp_ltp_weight` | double | 0.01 | STDP LTP weight factor | >= 0 |
| `stdp_ltd_weight` | double | 0.012 | STDP LTD weight factor | >= 0 |
| `stdp_tau` | double | 20.0 | STDP time constant in milliseconds | >= 0 |

### Developmental Parameters

| Key | Type | Default | Description | Valid Range |
|-----|------|---------|-------------|-------------|
| `synaptogenesis_rate` | double | 0.001 | Developmental synaptogenesis rate | 0.0 - 1.0 |
| `pruning_rate` | double | 0.0001 | Developmental pruning rate | 0.0 - 1.0 |

### Neuromodulation Parameters

| Key | Type | Default | Description | Valid Range |
|-----|------|---------|-------------|-------------|
| `dopamine_baseline` | double | 0.1 | Baseline dopamine level | 0.0 - 1.0 |
| `dopamine_pulse_rate` | double | 0.1 | Rate of dopamine pulses (events per second) | >= 0 |

### Reward Parameters

| Key | Type | Default | Description | Valid Range |
|-----|------|---------|-------------|-------------|
| `reward_discount_factor` | double | 0.99 | Discount factor for reward prediction error | 0.0 - 1.0 |

### Environment Parameters

| Key | Type | Default | Description | Valid Range |
|-----|------|---------|-------------|-------------|
| `environment_name` | string | "GridWorld" | Name of the environment to simulate |
| `environment_width` | int64_t | 10 | Width of the environment in grid cells | >= 1 |
| `environment_height` | int64_t | 10 | Height of the environment in grid cells | >= 1 |

### Logging Parameters

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `log_level` | string | "INFO" | Logging level (DEBUG, INFO, WARN, ERROR) |
| `log_to_file` | bool | false | Whether to log to file as well as console |
| `log_filename` | string | "nlm.log" | Filename for log file |

### Simulation Parameters

| Key | Type | Default | Description | Valid Range |
|-----|------|---------|-------------|-------------|
| `max_simulation_steps` | int64_t | 10000 | Maximum number of simulation steps | >= 0 |
| `simulation_time_limit` | double | 0.0 | Simulation time limit in seconds (0 = unlimited) | >= 0 |

### Visualization Parameters

| Key | Type | Default | Description | Valid Range |
|-----|------|---------|-------------|-------------|
| `visualization_enabled` | bool | false | Whether to enable visualization |
| `visualization_update_rate` | double | 30.0 | Visualization update rate in Hz | >= 0 |

## Command Line Interface

The NLM application provides a comprehensive command line interface for configuration management:

### Basic Usage

```bash
nlm --help                    # Show help information
nlm --version                 # Show version information
```

### Configuration Options

```bash
nlm --config=mysimulation.cfg          # Load configuration from file
nlm --config configs/custom.cfg         # Load configuration from file (alternative syntax)
nlm --set=neuron_count=2000            # Set configuration value
nlm --set=plasticity_learning_rate=0.05 # Set multiple configuration values
nlm --list-configs                      # List all available configuration options
nlm --validate-config                   # Validate current configuration
nlm --demo-mode                         # Run in demo mode with preset configurations
```

### Advanced Usage Examples

```bash
# Load a specific configuration file and override some values
nlm --config=experiment.cfg --set=neuron_count=500 --set=simulation_timestep=0.01

# List all available configuration options
nlm --list-configs

# Validate the current configuration
nlm --validate-config

# Run in demo mode with specific settings
nlm --demo-mode --set=neuron_count=200 --set=plasticity_learning_rate=0.1
```

### Configuration Value Types

The configuration system supports the following value types:

- **Integer**: `int` (e.g., `--set=neuron_count=100`)
- **64-bit integer**: `int64_t` (e.g., `--set=random_seed=42`)
- **Floating-point**: `double` (e.g., `--set=simulation_timestep=0.001`)
- **Boolean**: `bool` (e.g., `--set=log_to_file=true`)
- **String**: `string` (e.g., `--set=environment_name="TestWorld"`)
- **Integer vector**: `vector<int>` (e.g., `--set=neurons=[1,2,3,4,5]`)
- **Double vector**: `vector<double>` (e.g., `--set=weights=[0.1,0.2,0.3]`)
- **String vector**: `vector<string>` (e.g., `--set=labels=["a","b","c"]`)

## Configuration Examples

### Simple Simulation Configuration

```
# Simple neural simulation configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 2
connection_probability = 0.15
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
log_level = INFO
log_to_file = true
max_simulation_steps = 5000
```

### High-Performance Configuration

```
# High-performance simulation configuration
random_seed = 12345
simulation_timestep = 0.0005
neuron_count = 2000
region_count = 3
connection_probability = 0.05
plasticity_learning_rate = 0.005
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
log_level = WARN
log_to_file = false
max_simulation_steps = 20000
visualization_enabled = false
```

### Research Configuration

```
# Research-oriented configuration with detailed logging and visualization
random_seed = 0  # Use time-based seed for variability
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
log_level = DEBUG
log_to_file = true
log_filename = research_experiment.log
max_simulation_steps = 100000
simulation_time_limit = 3600.0
visualization_enabled = true
visualization_update_rate = 10.0
```

### Development Configuration

```
# Development and testing configuration
random_seed = 12345
simulation_timestep = 0.01  # Larger timestep for faster testing
neuron_count = 100  # Smaller network for development
region_count = 1
connection_probability = 0.2
plasticity_learning_rate = 0.1  # Higher learning rate for faster development
stdp_ltp_weight = 0.05
stdp_ltd_weight = 0.06
log_level = DEBUG
log_to_file = true
max_simulation_steps = 1000
```

## Error Handling and Recovery

The configuration system includes comprehensive error handling and recovery features:

### Error Types

1. **Validation Errors**: Configuration values that don't meet schema requirements
2. **File I/O Errors**: Problems reading or writing configuration files
3. **Parsing Errors**: Malformed configuration files or command line arguments
4. **Type Errors**: Configuration values with incorrect types
5. **Constraint Errors**: Values that violate schema constraints (ranges, patterns, etc.)

### Error Recovery

The system provides automatic recovery from configuration errors:

- **Fallback Values**: Unknown keys are added with string values as fallback
- **Graceful Degradation**: Invalid values in config files are logged but don't stop loading
- **Auto-Recovery**: Automatic recovery from validation errors
- **Backups**: Configuration backups for recovery from corrupted states

### Common Error Scenarios and Solutions

1. **Unknown Configuration Key**
   - Error: `Warning: Unknown configuration key "unknown_key" in config file`
   - Solution: Add the key to the configuration schema or check for typos

2. **Invalid Value Type**
   - Error: `Error: Invalid value type for key "neuron_count": expected integer`
   - Solution: Provide a value of the correct type (e.g., `100` instead of `"100"`)

3. **Value Out of Range**
   - Error: `Error: Value 2.0 for key "connection_probability" is greater than maximum allowed value 1.0`
   - Solution: Provide a value within the valid range (e.g., `0.5`)

4. **Malformed Configuration File**
   - Error: `Error: Invalid configuration format in file config.cfg line 5`
   - Solution: Check the file format and ensure it's `key = value`

5. **Missing Configuration File**
   - Warning: `Warning: Could not open configuration file: missing.cfg`
   - Solution: Use a different file name or let the system use defaults

## Advanced Configuration Features

### Configuration Validation

The configuration system includes comprehensive validation:

```bash
nlm --validate-config
```

This command validates the current configuration against the schema and reports:
- Valid configuration entries
- Unknown keys with warnings
- Invalid values with error details

### Configuration Listing

The configuration system includes a comprehensive listing feature:

```bash
nlm --list-configs
```

This command displays all available configuration parameters with:
- Parameter name and type
- Default values
- Description
- Valid range constraints (if applicable)

### Demo Mode

The demo mode provides preset configurations for different use cases:

```bash
nlm --demo-mode
```

Demo mode provides:
- Educational examples for different simulation scenarios
- Performance tuning recommendations
- Research configuration presets
- Development environment setup

## Configuration Best Practices

### For Research and Production

1. **Use Configuration Files**: Store experiment configurations in files
2. **Document Configuration**: Add comments to configuration files
3. **Validate Configuration**: Always validate configuration before running experiments
4. **Backup Configuration**: Create backups before major changes
5. **Version Control**: Store configuration files in version control
6. **Environment Separation**: Use different configuration files for different environments

### For Development

1. **Use Small Networks**: Start with small networks for development
2. **Enable Debug Logging**: Use `log_level = DEBUG` for development
3. **Quick Testing**: Use `--set=neuron_count=10` for quick testing
4. **Automated Testing**: Use command line arguments for automated testing
5. **Configuration Validation**: Validate configuration during development

### For Performance

1. **Adjust Time Steps**: Use larger time steps for performance
2. **Reduce Network Size**: Use fewer neurons for faster simulations
3. **Disable Visualization**: Set `visualization_enabled = false`
4. **Optimize Logging**: Use `log_level = WARN` for production
5. **Memory Management**: Monitor memory usage with large configurations

## Configuration Troubleshooting

### Common Issues and Solutions

**Issue**: Application fails to start due to configuration errors
**Solution**:
1. Run `nlm --list-configs` to see all available configuration options
2. Check for typos in configuration files
3. Validate configuration with `nlm --validate-config`
4. Check file permissions and paths

**Issue**: Configuration values are not being applied
**Solution**:
1. Check command line argument syntax
2. Verify configuration source precedence
3. Ensure values are set after configuration loading
4. Check for conflicting configuration sources

**Issue**: Configuration file is empty or contains only comments
**Solution**:
1. Add at least one valid configuration parameter
2. Use default configuration if file is missing
3. Check file format and encoding

**Issue**: Configuration values are not being saved to file
**Solution**:
1. Check file permissions
2. Verify disk space is available
3. Use absolute paths for file operations
4. Check file system limits

## Configuration Migration

### From Version 1.x to Version 2.x

Configuration format changes in version 2.x:

**Changes**:
1. **New Configuration Schema**: All configuration parameters are now validated against a schema
2. **New Parameters**: Additional configuration parameters added
3. **Validation**: All configuration values are now validated
4. **Error Handling**: Improved error messages and recovery

**Migration Steps**:
1. Backup existing configuration files
2. Update configuration files to match new format
3. Validate configuration with `nlm --validate-config`
4. Test application with new configuration
5. Update scripts and documentation to use new configuration options

### Configuration Backup and Restore

**Creating Backups**:
```bash
# Create a backup of the current configuration
nlm --config=current.cfg
# The backup will be saved as backups/current_YYYYMMDD_HHMMSS.cfg
```

**Restoring from Backups**:
```bash
# Restore a specific backup
nlm --config=backups/experiment_20240115_143022.cfg
```

**Listing Backups**:
```bash
# List all available backups
ls -la backups/
```

## Configuration Examples for Different Scenarios

### Scenario 1: Basic Learning Experiment

```
# Basic neural learning experiment configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
log_level = INFO
max_simulation_steps = 10000
```

### Scenario 2: High-Fidelity Simulation

```
# High-fidelity simulation with visualization
random_seed = 12345
simulation_timestep = 0.0001
neuron_count = 2000
region_count = 3
connection_probability = 0.05
plasticity_learning_rate = 0.005
log_level = DEBUG
log_to_file = true
max_simulation_steps = 50000
visualization_enabled = true
visualization_update_rate = 20.0
```

### Scenario 3: Fast Development Testing

```
# Fast development and testing configuration
random_seed = 123
simulation_timestep = 0.01
neuron_count = 50
region_count = 1
connection_probability = 0.2
plasticity_learning_rate = 0.1
log_level = DEBUG
max_simulation_steps = 100
```

### Scenario 4: Large-Scale Network

```
# Large-scale network configuration
random_seed = 456
simulation_timestep = 0.0005
neuron_count = 5000
region_count = 4
connection_probability = 0.02
plasticity_learning_rate = 0.002
log_level = WARN
log_to_file = true
max_simulation_steps = 100000
visualization_enabled = false
```

## Configuration Command Summary

### Command Line Arguments

| Option | Description | Example |
|--------|-------------|---------|
| `--help`, `-h` | Show help information | `nlm --help` |
| `--version`, `-v` | Show version information | `nlm --version` |
| `--config=<file>`, `-c <file>` | Load configuration from file | `nlm --config=experiment.cfg` |
| `--set=<key>=<value>` | Set configuration value | `nlm --set=neuron_count=1000` |
| `--list-configs` | List all configuration options | `nlm --list-configs` |
| `--validate-config` | Validate configuration | `nlm --validate-config` |
| `--demo-mode` | Run in demo mode | `nlm --demo-mode` |

### Configuration File Format

| Syntax | Example | Description |
|--------|---------|-------------|
| `key = value` | `neuron_count = 100` | Basic key-value pair |
| `key = "value"` | `environment_name = "TestWorld"` | String value with quotes |
| `key = true` | `log_to_file = false` | Boolean value |
| `key = 1,2,3,4` | `vector_key = 1,2,3,4` | Vector value |

### Configuration Operations

| Operation | Description | Command |
|-----------|-------------|---------|
| Load Configuration | Load configuration from file | `nlm --config=experiment.cfg` |
| Set Configuration | Set configuration value | `nlm --set=neuron_count=500` |
| Validate Configuration | Validate current configuration | `nlm --validate-config` |
| List Configuration | List available configuration options | `nlm --list-configs` |
| Backup Configuration | Create backup of current configuration | Manual backup creation |
| Restore Configuration | Restore from backup | `nlm --config=backup.cfg` |

## Configuration Best Practices Summary

### For Production Use

1. **Use Configuration Files**: Store all configuration in `.cfg` files
2. **Validate Before Running**: Always validate configuration
3. **Monitor Logs**: Enable appropriate logging levels
4. **Backup Regularly**: Create backups before major changes
5. **Test Configuration**: Validate configuration in test environment first

### For Research Use

1. **Document Configuration**: Add comments and documentation
2. **Use Reproducible Seeds**: Set `random_seed` for reproducibility
3. **Validate Constraints**: Ensure all values meet schema requirements
4. **Monitor Performance**: Track simulation performance and resource usage
5. **Version Control**: Store configuration files in version control

### For Development Use

1. **Use Small Networks**: Start with small networks for faster iteration
2. **Enable Debug Logging**: Use `log_level = DEBUG` for development
3. **Quick Testing**: Use command line arguments for quick testing
4. **Validate Configuration**: Validate configuration during development
5. **Test Edge Cases**: Test configuration with extreme values

## Conclusion

The NLM configuration system provides a comprehensive and robust solution for managing neural computation experiment parameters. It combines:

- **Type safety** through schema validation
- **Flexibility** through multiple configuration sources
- **Robustness** through error handling and recovery
- **Usability** through comprehensive documentation and examples
- **Performance** through efficient validation and loading

By following the guidelines and best practices outlined in this document, users can effectively configure and manage NLM experiments for research, development, and production use.
