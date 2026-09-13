# NLM Configuration Guide

## Overview

The NLM (Neural Learning Machine) uses a comprehensive configuration system to control neural network behavior, simulation parameters, and system settings. The configuration system supports multiple sources with different priority levels, allowing flexible configuration for various use cases.

## Configuration Sources and Priority

1. **Command Line Arguments** (Highest Priority)
   - Override all other configuration sources
   - `--key=value` or `-k value` format

2. **Config File** (`.cfg` format)
   - `configs/default.cfg` - Default configuration
   - Override configuration for specific scenarios

3. **Runtime Settings** (Lowest Priority)
   - Applied programmatically during execution

## Configuration File Format

### Current Format (Phase 1)
The current implementation uses a simple `key = value` format:

```
# Example configuration file
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
stdp_tau = 20.0
synaptogenesis_rate = 0.001
pruning_rate = 0.0001
dopamine_baseline = 0.1
```

### Future Format (Phase 2+)
Planned JSON/YAML format:

```json
{
    "nlm": {
        "version": "0.1.0",
        "random_seed": 42,
        "simulation": {
            "timestep": 0.001,
            "max_steps": 10000,
            "time_limit": 0.0
        },
        "brain": {
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.1
        },
        "plasticity": {
            "stdp": {
                "ltp_weight": 0.01,
                "ltd_weight": 0.012,
                "tau": 20.0
            },
            "structural": {
                "synaptogenesis_rate": 0.001,
                "pruning_rate": 0.0001
            }
        },
        "neuromodulation": {
            "dopamine_baseline": 0.1
        }
    }
}
```

## Available Configuration Parameters

### Basic Parameters

| Key | Type | Description | Default | Range | Required |
|-----|------|-------------|---------|-------|----------|
| `random_seed` | integer | Random seed for reproducible experiments | 42 | ≥ 0 | No |
| `simulation_timestep` | double | Simulation timestep in seconds | 0.001 | (0.0, 1.0] | No |

### Neural Network Parameters

| Key | Type | Description | Default | Range | Required |
|-----|------|-------------|---------|-------|----------|
| `neuron_count` | integer | Number of neurons in the brain | 1000 | [1, 1000000] | No |
| `region_count` | integer | Number of brain regions | 2 | [1, 100] | No |
| `connection_probability` | double | Connection probability for random connectivity | 0.1 | [0.0, 1.0] | No |

### Plasticity Parameters

| Key | Type | Description | Default | Range | Required |
|-----|------|-------------|---------|-------|----------|
| `plasticity_learning_rate` | double | Global plasticity learning rate | 0.01 | [0.0, 1.0] | No |
| `stdp_ltp_weight` | double | STDP Long-Term Potentiation weight | 0.01 | [0.0, 1.0] | No |
| `stdp_ltd_weight` | double | STDP Long-Term Depression weight | 0.012 | [0.0, 1.0] | No |
| `stdp_tau` | double | STDP time constant (tau) | 20.0 | (0.0, 1000.0] | No |

### Development Parameters

| Key | Type | Description | Default | Range | Required |
|-----|------|-------------|---------|-------|----------|
| `synaptogenesis_rate` | double | Synaptogenesis rate | 0.001 | [0.0, 1.0] | No |
| `pruning_rate` | double | Pruning rate | 0.0001 | [0.0, 1.0] | No |

### Neuromodulation Parameters

| Key | Type | Description | Default | Range | Required |
|-----|------|-------------|---------|-------|----------|
| `dopamine_baseline` | double | Dopamine baseline level | 0.1 | [0.0, 10.0] | No |

### Reward Parameters

| Key | Type | Description | Default | Range | Required |
|-----|------|-------------|---------|-------|----------|
| `reward_discount_factor` | double | Reward discount factor | 0.99 | [0.0, 1.0] | No |

### Environment Parameters

| Key | Type | Description | Default | Valid Values | Required |
|-----|------|-------------|---------|---------------|----------|
| `environment_name` | string | Environment name | "GridWorld" | ["GridWorld", "Empty", "Maze", "Custom"] | No |
| `environment_width` | integer | Environment width | 10 | [1, 1000] | No |
| `environment_height` | integer | Environment height | 10 | [1, 1000] | No |

### Logging Parameters

| Key | Type | Description | Default | Valid Values | Required |
|-----|------|-------------|---------|---------------|----------|
| `log_level` | string | Log level | "INFO" | ["DEBUG", "INFO", "WARN", "ERROR", "FATAL"] | No |
| `log_to_file` | boolean | Log to file | false | [true, false] | No |
| `log_filename` | string | Log filename | "nlm.log" | - | No |

### Simulation Parameters

| Key | Type | Description | Default | Range | Required |
|-----|------|-------------|---------|-------|----------|
| `max_simulation_steps` | integer | Maximum simulation steps | 10000 | (0, 1000000000] | No |
| `simulation_time_limit` | double | Simulation time limit (seconds) | 0.0 | [0.0, 1000000.0] | No |

### Visualization Parameters

| Key | Type | Description | Default | Range | Required |
|-----|------|-------------|---------|-------|----------|
| `visualization_enabled` | boolean | Enable visualization | false | [true, false] | No |
| `visualization_update_rate` | double | Visualization update rate (Hz) | 30.0 | (0.0, 1000.0] | No |

## Command Line Usage

### Override Configuration

Use command line arguments to override configuration values:

```bash
# Override single value
./nlm --simulation_timestep=0.01

# Override multiple values
./nlm --neuron_count=500 --connection_probability=0.2

# Override with shorthand syntax
./nlm -t 0.01 -n 500 -c 0.2
```

### Load Configuration File

Specify a configuration file to load:

```bash
# Load custom configuration file
./nlm --config=configs/production.cfg
```

### Combined Usage

```bash
# Load base config and override with command line
./nlm --config=configs/default.cfg --neuron_count=2000

# Environment variable overrides
export NLM_CONFIG_PATH=configs/custom.cfg
./nlm
```

## Configuration Validation

The configuration system includes validation for parameter types and ranges:

### Validation Rules

1. **Type Validation**: Ensures parameter values match expected types
2. **Range Validation**: Validates numeric ranges
3. **Enum Validation**: Validates allowed string values
4. **Cross-Parameter Validation**: Checks consistency between related parameters
5. **Dependency Validation**: Ensures required parameters are present

### Validation Output

The validation system provides detailed error and warning messages:

```
Validation Results:
✓ Configuration valid (47 parameters validated)

Warnings:
- synaptogenesis_rate (0.0005) is below recommended minimum (0.001)
- neuron_count (100) is below recommended minimum for realistic behavior

Errors:
- No errors
```

## Configuration Examples

### Example 1: Development Configuration

```
# config/development.cfg
random_seed = 123
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15
stdp_tau = 15.0
synaptogenesis_rate = 0.002
pruning_rate = 0.0002
dopamine_baseline = 0.2
reward_discount_factor = 0.95
environment_name = Empty
environment_width = 20
environment_height = 20
log_level = DEBUG
log_to_file = true
log_filename = nlm_dev.log
max_simulation_steps = 5000
simulation_time_limit = 60.0
visualization_enabled = true
visualization_update_rate = 60.0
```

### Example 2: Production Configuration

```
# config/production.cfg
random_seed = 42
simulation_timestep = 0.001
neuron_count = 10000
region_count = 10
connection_probability = 0.1
stdp_tau = 20.0
synaptogenesis_rate = 0.001
pruning_rate = 0.0001
dopamine_baseline = 0.1
reward_discount_factor = 0.99
environment_name = GridWorld
environment_width = 100
environment_height = 100
log_level = INFO
log_to_file = true
log_filename = nlm.log
max_simulation_steps = 100000
simulation_time_limit = 0.0
visualization_enabled = false
```

### Example 3: High-Performance Configuration

```
# config/performance.cfg
random_seed = 42
simulation_timestep = 0.0001
neuron_count = 5000
region_count = 5
connection_probability = 0.05
stdp_tau = 50.0
synaptogenesis_rate = 0.0005
pruning_rate = 0.00005
dopamine_baseline = 0.05
reward_discount_factor = 0.999
environment_name = Custom
environment_width = 50
environment_height = 50
log_level = WARN
log_to_file = true
log_filename = nlm_perf.log
max_simulation_steps = 50000
simulation_time_limit = 0.0
visualization_enabled = false
```

## Configuration Best Practices

### 1. Start with Defaults

Always start with the default configuration and only override parameters as needed:

```
# Good
./nlm --neuron_count=1000

# Better
./nlm --neuron_count=2000 --connection_probability=0.2

# Bad (too many overrides)
./nlm --neuron_count=1000 --region_count=5 --connection_probability=0.1 --stdp_tau=10.0 --synaptogenesis_rate=0.0005 --pruning_rate=0.00005
```

### 2. Test Configuration Changes

Test configuration changes in development before applying to production:

```bash
# Create development config based on production
cp configs/production.cfg configs/development.cfg

# Modify development config
# ...

# Test with development config
./nlm --config=configs/development.cfg
```

### 3. Use Descriptive Configuration Names

Use descriptive names for configuration files:

- `configs/default.cfg` - Default configuration
- `configs/development.cfg` - Development environment
- `configs/production.cfg` - Production environment
- `configs/debug.cfg` - Debug configuration
- `configs/performance.cfg` - High-performance configuration

### 4. Validate Configuration

Always validate configuration before running:

```bash
# Validate configuration
./nlm --validate

# Or use validation tools
python3 -m nlm.config_validator --config=configs/production.cfg
```

### 5. Document Configuration Changes

Document any custom configuration changes:

```
# configs/production.cfg
# Production configuration for NLM
# Increased neuron_count for better performance
# Reduced logging level for cleaner output
# Disabled visualization to save resources
```

## Configuration Testing

### Test Configuration Files

The repository includes test configuration files:

- `tests/config.ini` - Generated test configuration
- `tests/config.ini.in` - Template for test configuration

### Generate Test Data

Generate test data for configuration testing:

```bash
# Generate test data
python3 tests/generate_test_data.py

# Generate with custom parameters
python3 tests/generate_test_data.py --TEST_NEURON_COUNT=500 --TEST_REGION_COUNT=3
```

### Configuration Tests

Run configuration-related tests:

```bash
# Run configuration tests
make test_config

# Or run all tests
make test
```

## Migration Guide

### From Phase 1 to Phase 2+

Phase 2 and beyond will introduce JSON/YAML configuration format:

1. **Backward Compatibility**: Current `.cfg` files continue to work
2. **Migration Path**: Gradual transition to JSON/YAML
3. **Tools**: Migration tools will be provided

### Example Migration

Current format:
```
# configs/default.cfg
neuron_count = 1000
stdp_tau = 20.0
```

New format:
```json
{
    "nlm": {
        "brain": {
            "neuron_count": 1000
        },
        "plasticity": {
            "stdp": {
                "tau": 20.0
            }
        }
    }
}
```

## Troubleshooting

### Common Issues

1. **Duplicate Keys**
   - Error: Configuration contains duplicate keys
   - Solution: Remove duplicate keys from config file
   - Example: `dopamine_baseline` appeared twice in `configs/default.cfg`

2. **Invalid Values**
   - Error: Parameter value is out of range
   - Solution: Check parameter type and range
   - Example: `connection_probability` must be between 0.0 and 1.0

3. **Missing Parameters**
   - Error: Required parameter is missing
   - Solution: Add the missing parameter or use a default
   - Example: Some configurations require `random_seed`

4. **File Not Found**
   - Error: Configuration file not found
   - Solution: Check file path or use default configuration
   - Example: `./nlm --config=configs/missing.cfg`

### Validation Error Codes

| Code | Description | Solution |
|------|-------------|----------|
| E001 | Invalid parameter type | Check parameter type |
| E002 | Parameter out of range | Check parameter value |
| E003 | Missing required parameter | Add parameter or use default |
| E004 | Duplicate parameter | Remove duplicate key |
| E005 | File not found | Check file path |

### Validation Commands

```bash
# Validate configuration
./nlm --validate

# Show configuration summary
./nlm --summary

# Check for specific issues
./nlm --check config.ini

# Validate and list all issues
./nlm --validate --verbose
```

## Configuration Documentation

For the most up-to-date documentation, see:

- `docs/CONFIGURATION.md` - Comprehensive configuration guide
- `docs/CONFIG_FORMAT.md` - Configuration file format details
- `docs/CONFIG_VALIDATION.md` - Validation system documentation
- `examples/` - Configuration examples and tutorials

## See Also

- [NLM Documentation](https://github.com/nlmlab/nlm/blob/main/docs/)
- [Configuration API Reference](https://github.com/nlmlab/nlm/blob/main/src/core/Config/)
- [Test Configuration](tests/config.ini)
- [Development Guide](docs/DEVELOPMENT.md)

## Version Information

- Configuration System Version: 1.0 (Phase 1)
- Planned Upgrade: JSON/YAML support in Phase 2
- Backward Compatibility: Maintained for Phase 1 configuration
- Validation System: Comprehensive parameter validation
- Test Suite: Configuration-specific tests included

This document is continuously updated as the configuration system evolves. For the latest information, refer to the repository's documentation.

## Core Configuration System Improvements

This document provides comprehensive documentation for the NLM configuration system. The configuration system includes:

1. **Configuration Source Management**: Support for command line arguments, config files, and runtime settings with proper priority handling
2. **Parameter Validation**: Comprehensive validation of parameter types, ranges, and cross-parameter consistency
3. **Configuration File Formats**: Support for both current `.cfg` format and planned JSON/YAML format
4. **Command Line Interface**: Flexible command line argument parsing with both long and short forms
5. **Testing and Validation**: Extensive test suite and validation tools
6. **Build System Improvements**: Enhanced CMake configuration with glob patterns and conditional builds

### Key Improvements Made

1. **Fixed Duplicate Configuration Keys**: Removed duplicate `dopamine_baseline` key from `configs/default.cfg`
2. **Standardized Parameter Naming**: Consistent naming between configuration file keys and C++ code
3. **Enhanced Build System**: Added glob patterns and conditional builds for better maintainability
4. **Comprehensive Validation**: Implemented validation for parameter types, ranges, and cross-parameter dependencies
5. **Improved Documentation**: Created extensive documentation for the configuration system
6. **Test Infrastructure**: Enhanced test suite with configuration-specific tests

### Technical Enhancements

1. **Dynamic Source Management**: Configuration can be loaded from multiple sources with proper priority handling
2. **Flexible Parameter Types**: Support for various parameter types (int, float, string, bool, vectors)
3. **Cross-Parameter Validation**: Checks consistency between related parameters
4. **Documentation Generation**: Automatic documentation generation from parameter definitions
5. **Test Data Generation**: Automated test data generation for configuration testing

The configuration system is now more robust, maintainable, and user-friendly, with comprehensive validation and extensive documentation.

---

*This document was generated by the NLM Configuration System Improvement Task.*
*Version: 1.0.0 (Phase 1)
*Last Updated: 2026-09-13*