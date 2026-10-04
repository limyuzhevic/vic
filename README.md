# NLM (熙然) - Neural Learning Machine

## Overview

NLM (熙然) is an experimental artificial developmental brain system that implements real spiking neural computation. Phase 2 focuses on implementing real neural computation with:

- **Leaky Integrate-and-Fire (LIF) neurons** for biological realism
- **Event-driven spike propagation** with synaptic delays
- **STDP and Hebbian plasticity** for learning
- **Structural plasticity** (synaptogenesis/pruning) for development

This document covers the improvements made in Phase 2 and provides comprehensive user guidance for the enhanced configuration system and command-line interface.

## Key Improvements

### 🚀 **Enhanced Configuration System**
- **JSON/YAML support**: Proper configuration file parsing with validation
- **Schema-based validation**: Type checking and parameter validation
- **Multiple formats**: Support for INI, JSON, and YAML configuration files
- **Backward compatibility**: Maintains compatibility with existing code

### 🎯 **Comprehensive CLI Interface**
- **Help system**: `--help` command with detailed usage documentation
- **Flexible configuration**: `--config=<file>` for loading configurations
- **Parameter overrides**: `--key=<value>` for command-line parameter setting
- **Experiment modes**: `--experiment=<name>` for specific experiment runs
- **Debug/verbose modes**: `--verbose` and `--debug` for enhanced output

### 📚 **Enhanced Documentation**
- **Comprehensive examples**: Sample configuration files in multiple formats
- **Usage documentation**: Clear examples for common use cases
- **Schema definitions**: JSON Schema for configuration validation
- **Integration guides**: How to use the improved config system

### ⚡ **Improved Error Handling**
- **Clear error messages**: Detailed feedback for configuration issues
- **Validation errors**: Specific parameter validation with descriptions
- **Graceful failures**: Better handling of malformed configurations
- **File error reporting**: Clear messages for file not found and parse errors

## Quick Start

### Installation

The NLM system requires C++11+ compiler with the following dependencies:
- Standard C++ libraries
- Boost libraries (optional for some features)
- nlohmann/json library (included)
- yaml-cpp library (optional for YAML support)

### Basic Usage

```bash
# Run with default configuration
./nlm

# Load configuration from JSON file
./nlm --config=examples/config_example.json

# Override configuration parameters
./nlm --config=examples/config_example.json --neuron_count=2000 --debug

# Run specific experiment with verbose output
./nlm --experiment=learning --verbose
```

## Configuration System

### Configuration Formats

NLM supports three configuration file formats:

#### 1. JSON Format (`config_example.json`)
```json
{
  "nlm": {
    "random_seed": 12345,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity_learning_rate": 0.01,
    "log_level": "INFO",
    "verbose": false,
    "debug": false,
    "experiment": "learning"
  }
}
```

#### 2. YAML Format (`config_example.yaml`)
```yaml
nlm:
  random_seed: 12345
  simulation_timestep: 0.001
  neuron_count: 1000
  region_count: 2
  connection_probability: 0.1
  plasticity_learning_rate: 0.01
  log_level: INFO
  verbose: false
  debug: false
  experiment: learning
```

#### 3. INI Format (`config_example.cfg`)
```ini
# NLM Configuration File
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
log_level = INFO
verbose = false
debug = false
experiment = learning
```

### Configuration Schema

The configuration system includes a JSON Schema for validation:

```json
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "NLM Configuration Schema",
  "type": "object",
  "properties": {
    "random_seed": {
      "type": "integer",
      "description": "Random seed for reproducible experiments",
      "minimum": 0
    },
    "simulation_timestep": {
      "type": "number",
      "description": "Simulation timestep in seconds",
      "minimum": 0.0,
      "exclusiveMinimum": true
    },
    "neuron_count": {
      "type": "integer",
      "description": "Number of neurons in the brain",
      "minimum": 1
    },
    "region_count": {
      "type": "integer",
      "description": "Number of brain regions",
      "minimum": 1
    },
    "connection_probability": {
      "type": "number",
      "description": "Connection probability for random connectivity",
      "minimum": 0.0,
      "maximum": 1.0
    },
    "plasticity_learning_rate": {
      "type": "number",
      "description": "Plasticity learning rate",
      "minimum": 0.0,
      "maximum": 1.0
    },
    "log_level": {
      "type": "string",
      "description": "Log level (DEBUG, INFO, WARN, ERROR)"
    },
    "verbose": {
      "type": "boolean",
      "description": "Enable verbose output"
    },
    "debug": {
      "type": "boolean",
      "description": "Enable debug output"
    },
    "experiment": {
      "type": "string",
      "description": "Specify experiment to run"
    }
  },
  "required": ["random_seed", "simulation_timestep", "neuron_count", "region_count"]
}
```

### CLI Arguments

The enhanced CLI provides the following options:

#### Basic Options
- `--help`: Display help documentation and available options
- `--version`: Display version information

#### Configuration Options
- `--config=<file>`: Load configuration from JSON/YAML/INI file
- `--key=<value>`: Set configuration parameter from command line
- `--load=<file>`: Alias for `--config`

#### Output Options
- `--verbose`: Enable verbose output for detailed information
- `--debug`: Enable debug output for troubleshooting
- `--quiet`: Suppress non-essential output

#### Experiment Options
- `--experiment=<name>`: Specify which experiment to run
- `--experiments`: List available experiments
- `--run-experiment=<name>`: Run specific experiment with configuration

#### File Options
- `--save-config=<file>`: Save current configuration to file
- `--export-config=<file>`: Export configuration to different format

### Usage Examples

#### Example 1: Run with Default Configuration
```bash
./nlm
```

#### Example 2: Load JSON Configuration
```bash
./nlm --config=examples/config_example.json
```

#### Example 3: Override Parameters
```bash
./nlm --config=examples/config_example.json --neuron_count=2000 --debug
```

#### Example 4: Run Specific Experiment
```bash
./nlm --experiment=learning --verbose
```

#### Example 5: Save Current Configuration
```bash
./nlm --verbose --debug --save-config=current_config.json
```

#### Example 6: Load YAML and Export as JSON
```bash
./nlm --config=examples/config_example.yaml --export-config=exported_config.json
```

## Configuration Parameters

### Core Parameters
- **random_seed**: Random seed for reproducible experiments
- **simulation_timestep**: Simulation timestep in seconds
- **neuron_count**: Number of neurons in the brain
- **region_count**: Number of brain regions
- **connection_probability**: Connection probability for random connectivity

### Plasticity Parameters
- **plasticity_learning_rate**: Plasticity learning rate
- **stdp_ltp_weight**: STDP long-term potentiation weight
- **stdp_ltd_weight**: STDP long-term depression weight
- **stdp_time_constant**: STDP time constant

### Development Parameters
- **development_synaptogenesis_rate**: Development synaptogenesis rate
- **development_pruning_rate**: Development pruning rate

### Neuromodulation Parameters
- **dopamine_baseline**: Dopamine baseline value
- **reward_discount_factor**: Reward discount factor

### Environment Parameters
- **environment_name**: Environment name
- **environment_width**: Environment width
- **environment_height**: Environment height

### Logging Parameters
- **log_level**: Log level (DEBUG, INFO, WARN, ERROR)
- **log_to_file**: Whether to log to file
- **log_filename**: Log filename

### Simulation Parameters
- **max_simulation_steps**: Maximum simulation steps
- **simulation_time_limit**: Simulation time limit in seconds

### Visualization Parameters
- **visualization_enabled**: Whether visualization is enabled
- **visualization_update_rate**: Visualization update rate in Hz

### Debug/Experiment Parameters
- **verbose**: Enable verbose output
- **debug**: Enable debug output
- **experiment**: Specify experiment to run

## Error Handling

### Configuration Errors

The enhanced config system provides detailed error messages for various issues:

#### File Errors
```
ERROR: Configuration file not found: examples/missing.json
Usage: ./nlm [--help] [--version] [--config=<file>] [--key=<value>] [--verbose] [--debug]
```

#### Parse Errors
```
ERROR: Invalid JSON configuration: Syntax error at line 5, column 10
  ^
Expected: },

JSON parse error in examples/config_invalid.json
```

#### Validation Errors
```
ERROR: Configuration validation failed:
  - random_seed: Must be between 0 and 10000 (got 50000)
  - simulation_timestep: Must be positive (got 0.0)
  - neuron_count: Must be at least 1 (got 0)

Please fix the configuration and try again.
```

#### Type Errors
```
ERROR: Type mismatch for parameter 'neuron_count':
  Expected: integer
  Got: string
  Configuration file: examples/config_wrong_type.json
```

### Logging Levels

- **DEBUG**: Detailed debug information for troubleshooting
- **INFO**: General information for normal operation
- **WARN**: Warnings about potential issues
- **ERROR**: Error conditions that affect operation

### Output Verbosity

- **Normal**: Standard output with essential information
- **Verbose**: Detailed output including intermediate states
- **Debug**: Maximum detail including debugging information

## Advanced Usage

### Configuration Templates

The examples directory includes templates for common use cases:

#### Research Configuration
Optimized for experimental research with detailed logging and longer simulations.

#### Production Configuration
Optimized for stable operation with minimal logging overhead.

#### Development Configuration
Optimized for rapid prototyping and testing.

### Configuration Validation

To validate a configuration file before running:

```bash
./nlm --validate=examples/config_example.json
```

### Configuration Merging

To merge configurations:

```bash
./nlm --config=examples/config_base.json --config=examples/config_override.json
```

### Batch Processing

To run multiple configurations:

```bash
for config in examples/configs/*.json; do
    echo "Running configuration: $config"
    ./nlm --config="$config" --verbose
    echo "---"
done
```

## Troubleshooting

### Common Issues

#### 1. Configuration File Not Found
**Solution**: Ensure the configuration file exists and provide the correct path.

#### 2. Invalid JSON/YAML
**Solution**: Use a valid JSON/YAML parser to check the configuration file format.

#### 3. Parameter Validation Errors
**Solution**: Check the parameter values against the schema requirements.

#### 4. Missing Required Parameters
**Solution**: Provide values for all required parameters.

#### 5. Permission Errors
**Solution**: Ensure read/write permissions for configuration files.

### Debug Mode

To enable debug mode for troubleshooting:

```bash
./nlm --debug --verbose --config=examples/config_example.json
```

Debug mode provides:
- Detailed logging of all operations
- Configuration validation details
- Parameter value changes
- Step-by-step simulation progress

### Configuration Editor

To create or edit configuration files:

1. Use any text editor for INI format
2. Use jq (JSON) or yq (YAML) tools for JSON/YAML
3. Use the built-in configuration editor in the examples directory

## Migration Guide

### From Old Config System

The enhanced config system maintains backward compatibility with the old Config class. To migrate:

1. **Keep existing config files**: The system supports the old key=value format
2. **Update to new format**: Gradually migrate to JSON/YAML format for better validation
3. **Use new CLI options**: Leverage the enhanced command-line interface
4. **Maintain old code**: Existing code using the Config class will continue to work

### Upgrading Checklist

- [ ] Review configuration files and update format if needed
- [ ] Test with new configuration validation
- [ ] Update any scripts using the old config format
- [ ] Utilize new CLI options for better control
- [ ] Add configuration schema validation where needed

## Conclusion

The NLM Phase 2 improvements provide a significant enhancement to the configuration system:

- **Better user experience**: Intuitive CLI with comprehensive help
- **Robust validation**: Schema-based parameter checking
- **Multiple formats**: Support for INI, JSON, and YAML
- **Enhanced documentation**: Comprehensive examples and guides
- **Improved error handling**: Clear, actionable error messages
- **Backward compatibility**: Smooth transition path

These improvements make NLM more accessible to new users while providing advanced features for experienced users and researchers.

## References

- [NLM Architecture Documentation](docs/ARCHITECTURE.md)
- [Phase 6 Architecture Documentation](docs/PHASE6_ARCHITECTURE.md)
- [Configuration Examples](examples/)
- [Experiment Documentation](docs/EXPERIMENTS.md)

## Version Information

- **Phase**: 2 (Real Neural Computation)
- **Version**: 2.0
- **Release Date**: Current
- **Documentation**: Generated from codebase improvements
