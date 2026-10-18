# NLM Configuration Documentation

This document provides comprehensive documentation for the NLM (Neural Learning Machine) configuration system.

## Overview

The NLM configuration system provides flexible and extensible configuration management for neural simulation parameters. It supports multiple file formats, validation, and advanced features for power users.

## Configuration File Formats

### 1. Legacy Format (key=value)

**File:** `configs/default.cfg`

**Format:** Simple key-value pairs with comments

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

### 2. JSON Format

**Example:** `config.json`

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
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "dopamine_baseline": 0.1,
    "reward_discount_factor": 0.99,
    "environment_name": "GridWorld",
    "environment_width": 10,
    "environment_height": 10,
    "log_level": "INFO",
    "log_to_file": false,
    "max_simulation_steps": 10000,
    "visualization_enabled": false,
    "visualization_update_rate": 30.0
}
```

### 3. YAML Format

**Example:** `config.yaml`

```yaml
# NLM Configuration File
random_seed: 42
simulation_timestep: 0.001
neuron_count: 1000
region_count: 2
connection_probability: 0.1

# Plasticity settings
plasticity_learning_rate: 0.01
stdp_ltp_weight: 0.01
stdp_ltd_weight: 0.012
stdp_time_constant: 20.0

# Development settings
development_synaptogenesis_rate: 0.001
development_pruning_rate: 0.0001

# Neuromodulation settings
dopamine_baseline: 0.1

# Reward settings
reward_discount_factor: 0.99

# Environment settings
environment_name: GridWorld
environment_width: 10
environment_height: 10

# Logging settings
log_level: INFO
log_to_file: false
log_filename: nlm.log

# Simulation settings
max_simulation_steps: 10000
simulation_time_limit: 0.0

# Visualization settings
visualization_enabled: false
visualization_update_rate: 30.0
```

## Configuration Parameters

### Basic Configuration

| Parameter | Type | Default | Description | Unit | Category |
|-----------|------|---------|-------------|------|----------|
| `random_seed` | int64_t | 42 | Random seed for reproducible experiments | - | Basic |
| `simulation_timestep` | double | 0.001 | Simulation timestep in seconds | s | Basic |
| `neuron_count` | int64_t | 1000 | Number of neurons in the brain | count | Basic |
| `region_count` | int64_t | 2 | Number of brain regions | count | Basic |
| `connection_probability` | float | 0.1 | Connection probability for random connectivity | - | Basic |

### Learning Configuration

| Parameter | Type | Default | Description | Unit | Category | Advanced |
|-----------|------|---------|-------------|------|----------|----------|
| `plasticity_learning_rate` | double | 0.01 | Hebbian plasticity learning rate | - | Learning | No |
| `stdp_ltp_weight` | double | 0.01 | STDP long-term potentiation weight | - | Learning | No |
| `stdp_ltd_weight` | double | 0.012 | STDP long-term depression weight | - | Learning | No |
| `stdp_time_constant` | double | 20.0 | STDP time constant | ms | Learning | No |
| `stdp_tau` | double | 20.0 | STDP time constant (Phase 2) | ms | Learning | No |
| `plasticity_rate` | double | 0.01 | Advanced plasticity rate control | - | Learning | Yes |

### Development Configuration

| Parameter | Type | Default | Description | Unit | Category | Advanced |
|-----------|------|---------|-------------|------|----------|----------|
| `development_synaptogenesis_rate` | double | 0.001 | Synaptogenesis rate during development | per_step | Development | No |
| `development_pruning_rate` | double | 0.0001 | Pruning rate during development | per_step | Development | No |
| `synaptogenesis_rate` | double | 0.0001 | Synaptogenesis rate (Phase 2) | per_step | Development | No |
| `pruning_rate` | double | 0.00001 | Pruning rate (Phase 2) | per_step | Development | No |
| `development_maturation_rate` | double | 0.01 | Development maturation rate (advanced) | per_step | Development | Yes |

### Neuromodulation Configuration

| Parameter | Type | Default | Description | Unit | Category | Advanced |
|-----------|------|---------|-------------|------|----------|----------|
| `dopamine_baseline` | double | 0.1 | Baseline dopamine level | - | Neuromodulation | No |
| `dopamine_modulation_strength` | double | 1.0 | Dopamine modulation strength (advanced) | - | Neuromodulation | Yes |
| `curiosity_drive` | double | 0.1 | Curiosity drive coefficient (advanced) | - | Neuromodulation | Yes |
| `novelty_sensitivity` | double | 0.5 | Novelty detection sensitivity (advanced) | - | Neuromodulation | Yes |

### Memory Configuration

| Parameter | Type | Default | Description | Unit | Category | Advanced |
|-----------|------|---------|-------------|------|----------|----------|
| `memory_replay_frequency` | double | 0.1 | Memory replay frequency (advanced) | per_step | Memory | Yes |

### Prediction Configuration

| Parameter | Type | Default | Description | Unit | Category | Advanced |
|-----------|------|---------|-------------|------|----------|----------|
| `prediction_confidence_threshold` | double | 0.8 | Prediction confidence threshold (advanced) | - | Prediction | Yes |

### Network Configuration

| Parameter | Type | Default | Description | Unit | Category | Advanced |
|-----------|------|---------|-------------|------|----------|----------|
| `structural_plasticity_rate` | double | 0.0001 | Structural plasticity rate (advanced) | per_step | Network | Yes |
| `network_connectivity_threshold` | double | 0.15 | Network connectivity threshold (advanced) | - | Network | Yes |

### Environment Configuration

| Parameter | Type | Default | Description | Unit | Category | Advanced |
|-----------|------|---------|-------------|------|----------|----------|
| `environment_name` | string | "GridWorld" | Environment name | - | Environment | No |
| `environment_width` | int64_t | 10 | Environment width | cells | Environment | No |
| `environment_height` | int64_t | 10 | Environment height | cells | Environment | No |

### Debugging Configuration

| Parameter | Type | Default | Description | Unit | Category | Advanced |
|-----------|------|---------|-------------|------|----------|----------|
| `log_level` | string | "INFO" | Log level | - | Debugging | No |
| `log_to_file` | bool | false | Log to file | - | Debugging | No |
| `log_filename` | string | "nlm.log" | Log filename | - | Debugging | No |
| `visualization_enabled` | bool | false | Enable visualization | - | Debugging | No |
| `visualization_update_rate` | double | 30.0 | Visualization update rate in Hz | Hz | Debugging | No |

### Performance Configuration

| Parameter | Type | Default | Description | Unit | Category | Advanced |
|-----------|------|---------|-------------|------|----------|----------|
| `max_simulation_steps` | int64_t | 10000 | Maximum simulation steps | steps | Performance | No |
| `simulation_time_limit` | double | 0.0 | Simulation time limit in seconds | s | Performance | No |
| `adaptation_variable_decay` | double | 0.95 | Adaptation variable decay rate (advanced) | per_step | Performance | Yes |
| `energy_efficiency_factor` | double | 0.8 | Energy efficiency factor (advanced) | - | Performance | Yes |

## Configuration Validation

### Parameter Validation Rules

1. **Random Seed (`random_seed`)**: Must be a non-negative integer
2. **Simulation Timestep (`simulation_timestep`)**: Must be > 0.0 and ≤ 1.0
3. **Neuron Count (`neuron_count`)**: Must be > 0 and ≤ 100,000
4. **Region Count (`region_count`)**: Must be > 0 and ≤ 1,000
5. **Connection Probability (`connection_probability`)**: Must be between 0.0 and 1.0
6. **Learning Rate (`plasticity_learning_rate`)**: Must be ≥ 0.0 and ≤ 10.0
7. **STDP Weights (`stdp_ltp_weight`, `stdp_ltd_weight`)**: Must be ≥ 0.0 and ≤ 10.0
8. **Time Constant (`stdp_time_constant`, `stdp_tau`)**: Must be > 0.0 and ≤ 1,000.0
9. **Rates (`synaptogenesis_rate`, `pruning_rate`)**: Must be between 0.0 and 1.0
10. **Neuromodulation Values (`dopamine_baseline`)**: Must be ≥ 0.0 and ≤ 10.0
11. **Discount Factor (`reward_discount_factor`)**: Must be between 0.0 and 1.0
12. **Environment Dimensions (`environment_width`, `environment_height`)**: Must be > 0 and ≤ 1,000
13. **Visualization Update Rate (`visualization_update_rate`)**: Must be > 0.0

### Validation Error Messages

- **Invalid Parameter Type**: "Invalid value for parameter '{key}': {value}. Expected {type}."
- **Out of Range**: "Value for parameter '{key}' ({value}) is out of range. Valid range: [{min}, {max}]."
- **Missing Required Parameter**: "Missing required parameter: {key}. Using default value."
- **Conflicting Parameters**: "Warning: Inconsistent parameter combination detected for '{key}'."

## Command-Line Arguments

### Usage

```bash
./nlm [options]
```

### Available Options

| Option | Format | Description | Example |
|--------|--------|-------------|---------|
| `--config` | `--config=file` | Load configuration from file | `--config=experiments/config.json` |
| `-c` | `-c file` | Load configuration from file (short) | `-c experiments/config.yaml` |
| `--seed` | `--seed=value` | Override random seed | `--seed=123` |
| `-s` | `-s value` | Override random seed (short) | `-s 456` |
| `--neurons` | `--neurons=value` | Override neuron count | `--neurons=500` |
| `-n` | `-n value` | Override neuron count (short) | `-n 750` |
| `--timestep` | `--timestep=value` | Override simulation timestep | `--timestep=0.0001` |
| `-t` | `-t value` | Override simulation timestep (short) | `-t 0.0005` |

### Command-Line Example

```bash
# Load from JSON config with command-line overrides
./nlm --config=experiments/config.json --seed=12345 --neurons=2000 --timestep=0.0005

# Load from legacy config with short options
./nlm -c configs/default.cfg -s 999 -n 3000 -t 0.0002
```

## Configuration Migration Guide

### From Phase 1 to Phase 2

Phase 1 used different parameter names than Phase 2. The configuration system automatically handles this migration:

**Phase 1 parameters (deprecated but supported):**
- `stdp_time_constant` → `stdp_tau`
- `development_synaptogenesis_rate` → `synaptogenesis_rate`
- `development_pruning_rate` → `pruning_rate`
- `plasticity_learning_rate` → `plasticity_rate`

**Migration Example:**

**Phase 1 config (`configs/default.cfg`):**
```
stdp_time_constant = 20.0
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
```

**Phase 2 config (automatically mapped):**
```
stdp_tau = 20.0
synaptogenesis_rate = 0.001
pruning_rate = 0.0001
```

## Advanced Configuration Examples

### Example 1: High-Performance Configuration

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.0001,
    "neuron_count": 5000,
    "region_count": 5,
    "connection_probability": 0.2,
    "plasticity_learning_rate": 0.05,
    "stdp_ltp_weight": 0.03,
    "stdp_ltd_weight": 0.02,
    "stdp_tau": 30.0,
    "synaptogenesis_rate": 0.0005,
    "pruning_rate": 0.00005,
    "dopamine_baseline": 0.5,
    "reward_discount_factor": 0.95,
    "environment_name": "ComplexWorld",
    "environment_width": 20,
    "environment_height": 20,
    "log_level": "DEBUG",
    "log_to_file": true,
    "max_simulation_steps": 50000,
    "visualization_enabled": true,
    "visualization_update_rate": 60.0,
    "adaptation_variable_decay": 0.98,
    "energy_efficiency_factor": 0.9
}
```

### Example 2: Development-Staged Configuration

```yaml
# Development-stage specific configuration
random_seed: 123

# Early stage: High learning rate, frequent synaptogenesis
plasticity_learning_rate: 0.1
stdp_ltp_weight: 0.05
stdp_ltd_weight: 0.04
stdp_tau: 25.0
synaptogenesis_rate: 0.002
pruning_rate: 0.0002

# Neuromodulation for exploration
dopamine_baseline: 0.3
curiosity_drive: 0.5
novelty_sensitivity: 0.7

# Memory system for learning
memory_replay_frequency: 0.2

# Performance settings for early development
max_simulation_steps: 20000
adaptation_variable_decay: 0.99
```

## Best Practices

### 1. Configuration File Organization

- Use comments to document important configuration choices
- Group related parameters logically
- Use consistent naming conventions
- Include validation comments for complex parameters

### 2. Environment-Specific Configurations

- Create separate configuration files for different environments:
  - `configs/development.cfg` for development
  - `configs/staging.cfg` for staging
  - `configs/production.cfg` for production

### 3. Configuration Validation

- Always validate configuration before running simulations
- Use command-line arguments for environment-specific overrides
- Log configuration summaries for debugging

### 4. Performance Considerations

- Balance between simulation accuracy and performance
- Use appropriate timestep for the timescale of interest
- Monitor memory usage for large-scale simulations

### 5. Reproducibility

- Always record the exact configuration used for experiments
- Use deterministic random seeds for reproducible results
- Document any configuration changes between experimental runs

## Troubleshooting

### Common Issues and Solutions

#### Issue: Configuration file not found

**Problem:** Configuration file specified with `--config` option is not found.

**Solution:**
```bash
# Check if file exists
ls -la experiments/config.json

# Or use absolute path
./nlm --config=/absolute/path/to/config.json

# Or ensure file is in the correct location
./nlm --config=configs/default.cfg
```

#### Issue: Invalid configuration value

**Problem:** Configuration parameter has an invalid value (wrong type or out of range).

**Solution:**
```bash
# Check the specific parameter value
# Edit the configuration file and fix the issue
# For example, change neuron_count from "invalid" to 1000
```

#### Issue: Parameter name not recognized

**Problem:** Configuration file uses deprecated parameter name.

**Solution:**
```bash
# Use modern parameter names
# For example, use "stdp_tau" instead of "stdp_time_constant"
```

#### Issue: Advanced configuration not working

**Problem:** Advanced configuration parameters are not being recognized.

**Solution:**
```bash
# Check if parameter is marked as advanced in Config.hpp
# Ensure the parameter is defined with isAdvanced=true
# For example: set("adaptation_variable_decay", 0.98, ..., true)
```

## Getting Help

### Documentation Resources

1. **Configuration Reference:** This document
2. **NLM Documentation:** `docs/` directory for detailed information
3. **API Documentation:** Generated documentation for the Config class
4. **Tutorials:** `docs/CONFIGURATION.md` for step-by-step configuration guides

### Support Channels

1. **GitHub Issues:** Report bugs and request features
2. **Documentation:** Check `docs/` for comprehensive guides
3. **Examples:** Review `examples/` directory for real-world configurations

### Configuration Examples

For more configuration examples, see:
- `examples/advanced_config.json` - Advanced configuration examples
- `examples/development_config.yaml` - Development-stage configuration
- `examples/performance_config.cfg` - High-performance configuration

## Configuration Versioning

### Current Version
- **Format Version:** 3.0 (supports legacy, JSON, and YAML)
- **API Version:** 2.0 (enhanced Config class with validation)
- **Parameter Names:** Phase 2 (with backward compatibility to Phase 1)

### Migration Notes

- Version 3.0+: Full backward compatibility with automatic parameter mapping
- Version 2.0+: Enhanced validation and error handling
- Version 1.0+: Legacy key=value format only

## Configuration Performance Tips

### 1. File Format Selection

- **JSON:** Best for programmatic configuration and validation
- **YAML:** Best for human-readable configuration with comments
- **Legacy:** Best for simple, lightweight configuration

### 2. Configuration Loading

```cpp
// Fast configuration loading
auto config = std::make_shared<Config>();
config->loadFromFile("config.json");  // Automatic format detection

// Override with command-line arguments
config->loadFromArgs(argc, argv);
```

### 3. Configuration Usage

```cpp
// Efficient parameter access
auto neurons = config->getOr<int64_t>("neuron_count", 1000);
auto timestep = config->getOr<double>("simulation_timestep", 0.001);
```

## Conclusion

The NLM configuration system provides comprehensive and flexible configuration management for neural simulations. It supports multiple file formats, validation, advanced features, and maintains backward compatibility with Phase 1 configurations.

For the best experience:
1. Use JSON or YAML for new configurations
2. Leverage command-line arguments for environment-specific overrides
3. Validate configuration before running simulations
4. Document configuration choices for reproducibility

The configuration system is designed to be both powerful for advanced users and accessible for beginners, with comprehensive documentation and examples to guide users through the configuration process.