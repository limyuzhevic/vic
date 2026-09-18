# Configuration Schema Validation for NLM

## Overview

This module provides configuration validation and error handling for NLM configuration files. It ensures that configuration values are valid, properly typed, and within acceptable ranges.

## Key Features

- Type validation for all configuration parameters
- Range validation (min/max values)
- Required field checking
- Configuration file validation
- Error reporting with descriptive messages
- Backward compatibility support

## Configuration Schema

### Valid Configuration Parameters

#### Core Parameters

| Parameter | Type | Default | Min | Max | Required | Description |
|-----------|------|---------|-----|-----|----------|-------------|
| `random_seed` | int64_t | 42 | 0 | 2^63-1 | No | Random seed for reproducible experiments |
| `simulation_timestep` | double | 0.001 | 1e-6 | 1.0 | No | Simulation timestep in seconds |
| `neuron_count` | int64_t | 1000 | 1 | 100000 | No | Number of neurons in the brain |
| `region_count` | int64_t | 2 | 1 | 100 | No | Number of brain regions |

#### Connectivity Parameters

| Parameter | Type | Default | Min | Max | Required | Description |
|-----------|------|---------|-----|-----|----------|-------------|
| `connection_probability` | float | 0.1 | 0.0 | 1.0 | No | Probability of random connectivity between neurons |

#### Plasticity Parameters

| Parameter | Type | Default | Min | Max | Required | Description |
|-----------|------|---------|-----|-----|----------|-------------|
| `plasticity_learning_rate` | float | 0.01 | 0.0 | 1.0 | No | General plasticity learning rate |
| `stdp_ltp_weight` | float | 0.01 | 0.0 | 1.0 | No | STDP LTP weight |
| `stdp_ltd_weight` | float | 0.012 | 0.0 | 1.0 | No | STDP LTD weight |
| `stdp_time_constant` | float | 20.0 | 1.0 | 1000.0 | No | STDP time constant |

#### Development Parameters

| Parameter | Type | Default | Min | Max | Required | Description |
|-----------|------|---------|-----|-----|----------|-------------|
| `development_synaptogenesis_rate` | float | 0.001 | 0.0 | 0.01 | No | Synaptogenesis rate during development |
| `development_pruning_rate` | float | 0.0001 | 0.0 | 0.001 | No | Pruning rate during development |

#### Neuromodulation Parameters

| Parameter | Type | Default | Min | Max | Required | Description |
|-----------|------|---------|-----|-----|----------|-------------|
| `dopamine_baseline` | float | 0.1 | 0.0 | 1.0 | No | Baseline dopamine level |
| `dopamine_curiosity_weight` | float | 0.5 | 0.0 | 1.0 | No | Weight of curiosity in dopamine computation |
| `dopamine_reward_weight` | float | 1.0 | 0.0 | 1.0 | No | Weight of reward in dopamine computation |
| `dopamine_novelty_weight` | float | 0.3 | 0.0 | 1.0 | No | Weight of novelty in dopamine computation |

#### Reward Parameters

| Parameter | Type | Default | Min | Max | Required | Description |
|-----------|------|---------|-----|-----|----------|-------------|
| `reward_discount_factor` | float | 0.99 | 0.0 | 1.0 | No | Discount factor for delayed rewards |

#### Environment Parameters

| Parameter | Type | Default | Min | Max | Required | Description |
|-----------|------|---------|-----|-----|----------|-------------|
| `environment_name` | string | "GridWorld" | - | - | No | Name of environment |
| `environment_width` | int | 10 | 1 | 100 | No | Width of environment grid |
| `environment_height` | int | 10 | 1 | 100 | No | Height of environment grid |

#### Logging Parameters

| Parameter | Type | Default | Min | Max | Required | Description |
|-----------|------|---------|-----|-----|----------|-------------|
| `log_level` | string | "INFO" | - | - | No | Log level (DEBUG, INFO, WARNING, ERROR) |
| `log_to_file` | bool | false | - | - | No | Whether to log to file |
| `log_filename` | string | "nlm.log" | - | - | No | Log file name |

#### Simulation Parameters

| Parameter | Type | Default | Min | Max | Required | Description |
|-----------|------|---------|-----|-----|----------|-------------|
| `max_simulation_steps` | int64_t | 10000 | 1 | 1000000 | No | Maximum simulation steps |
| `simulation_time_limit` | double | 0.0 | 0.0 | 1e6 | No | Time limit for simulation |

#### Visualization Parameters

| Parameter | Type | Default | Min | Max | Required | Description |
|-----------|------|---------|-----|-----|----------|-------------|
| `visualization_enabled` | bool | false | - | - | No | Whether visualization is enabled |
| `visualization_update_rate` | float | 30.0 | 1.0 | 1000.0 | No | Visualization update rate (Hz) |

## Configuration File Format

### Simple Key-Value Format

```
# Comment
key = value
key = "quoted value"
key = [1, 2, 3]
```

### Required vs Optional Parameters

- **Required parameters** must be present in the configuration file
- **Optional parameters** will use default values if not present
- **Deprecated parameters** may trigger warnings

### Validation Rules

1. **Type Validation**: Parameters must match expected types
2. **Range Validation**: Parameters must be within specified ranges
3. **Consistency Validation**: Parameters must be consistent with each other
4. **Dependency Validation**: Parameters must satisfy dependencies

## Error Handling

### Error Categories

1. **Configuration File Errors**: File not found, parsing errors, syntax errors
2. **Type Errors**: Invalid parameter types
3. **Range Errors**: Parameters outside valid ranges
4. **Consistency Errors**: Inconsistent parameter values
5. **Deprecated Errors**: Use of deprecated parameters

### Error Reporting

Errors are reported with descriptive messages including:
- Parameter name
- Expected value/type/range
- Actual value
- Suggestion for fix

## Backward Compatibility

This validation system maintains backward compatibility by:

1. Accepting deprecated parameter names with warnings
2. Providing sensible defaults for missing parameters
3. Graceful handling of invalid configuration values

## Usage

### Programmatic API

```cpp
#include "ConfigValidation.hpp"

bool validateConfiguration(const Config& config);
std::vector<std::string> getValidationErrors(const Config& config);
void applyConfigurationDefaults(Config& config);
```

### Command Line

```bash
./nlm --validate-config configs/myconfig.cfg
./nlm --check-config --config=myconfig.cfg
```

## Testing

Configuration validation is tested with:

1. Valid configuration files
2. Invalid configuration files (type errors)
3. Configuration files with out-of-range values
4. Missing required parameters
5. Deprecated parameter usage

## Performance Considerations

Validation is optimized for:

1. **Fast validation**: Linear time complexity O(n) where n is number of parameters
2. **Lazy validation**: Validation can be deferred until needed
3. **Caching**: Validation results can be cached for repeated use
4. **Parallelization**: Independent parameter validation can be parallelized

## Future Enhancements

1. JSON/YAML configuration file support
2. Schema evolution support
3. Configuration templates
4. Configuration inheritance
5. Runtime configuration updates