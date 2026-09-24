# NLM Configuration Guide

## Overview

This document provides comprehensive information about the Neural Learning Machine (NLM) configuration system. It includes parameter definitions, validation rules, file formats, and examples.

## Configuration System Architecture

The NLM configuration system uses a hierarchical key-value structure with the following components:

1. **Parameters**: Individual configuration settings identified by unique keys
2. **Sources**: Four sources of configuration values:
   - Default: Built-in default values
   - File: Loaded from configuration files (.cfg, .json, .yaml, .yml)
   - Command Line: Set via command-line arguments (`--key=value` or `-k value`)
   - Runtime: Set programmatically during execution

3. **Validation**: Optional validation system that checks:
   - Parameter types
   - Parameter ranges and constraints
   - Parameter dependencies
   - Configuration consistency

## Configuration Parameter Categories

### Brain Parameters

| Parameter | Type | Default | Range/Constraints | Description |
|-----------|------|---------|-------------------|-------------|
| `brain.neuron_count` | int | 1000 | > 0 | Total number of neurons |
| `brain.synapse_density` | double | 0.1 | [0.0, 1.0] | Probability of connection between neurons |
| `brain.connection_probability` | double | 0.05 | [0.0, 1.0] | Probability of connection between neurons |
| `brain.initial_weight_mean` | double | 0.5 | [-1.0, 1.0] | Mean initial synaptic weight |
| `brain.initial_weight_std` | double | 0.1 | [0.0, ∞) | Standard deviation of initial weights |
| `brain.v_thresh` | double | -50.0 | [-100.0, 0.0] | Neuron threshold potential (mV) |
| `brain.v_rest` | double | -70.0 | [-80.0, -40.0] | Resting potential (mV) |
| `brain.v_reset` | double | -75.0 | [-90.0, -50.0] | Reset potential after spike (mV) |
| `brain.tau_mem` | double | 20.0 | (0.0, ∞) | Membrane time constant (ms) |
| `brain.tau_ref` | double | 2.0 | (0.0, ∞) | Refractory period (ms) |

### Plasticity Parameters

| Parameter | Type | Default | Range/Constraints | Description |
|-----------|------|---------|-------------------|-------------|
| `plasticity.stdp.enable` | bool | true | - | Enable Spike-Timing-Dependent Plasticity |
| `plasticity.stdp.learning_rate` | double | 0.001 | (0.0, ∞) | STDP learning rate |
| `plasticity.stdp.tau_plus` | double | 20.0 | (0.0, ∞) | STDP time constant for potentiation |
| `plasticity.stdp.tau_minus` | double | 20.0 | (0.0, ∞) | STDP time constant for depression |
| `plasticity.hebbian.enable` | bool | true | - | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | double | 0.001 | (0.0, ∞) | Hebbian learning rate |
| `plasticity.structural.enable` | bool | true | - | Enable structural plasticity |
| `plasticity.structural.synaptogenesis_rate` | double | 0.001 | [0.0, 1.0] | Rate of new synapse formation |
| `plasticity.structural.pruning_rate` | double | 0.0001 | [0.0, 1.0] | Rate of synapse elimination |

### Neuromodulation Parameters

| Parameter | Type | Default | Range/Constraints | Description |
|-----------|------|---------|-------------------|-------------|
| `neuromod.dopamine.scale` | double | 1.0 | [0.0, ∞) | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | - | Enable curiosity-driven exploration |
| `neuromod.curiosity.learning_rate` | double | 0.01 | (0.0, ∞) | Curiosity learning rate |
| `neuromod.novelty.enable` | bool | true | - | Enable novelty detection |
| `neuromod.novelty.threshold` | double | 0.5 | [0.0, 1.0] | Novelty detection threshold |

### Development Parameters

| Parameter | Type | Default | Range/Constraints | Description |
|-----------|------|---------|-------------------|-------------|
| `development.synaptogenesis_rate` | double | 0.001 | [0.0, 1.0] | Rate of new synapse formation during development |
| `development.pruning_rate` | double | 0.0001 | [0.0, 1.0] | Rate of synapse elimination during development |
| `development.critical_period` | double | 10.0 | (0.0, ∞) | Critical period duration (steps) |
| `development.maturation_rate` | double | 0.1 | [0.0, 1.0] | Rate of neural maturation |

### Environment Parameters

| Parameter | Type | Default | Range/Constraints | Description |
|-----------|------|---------|-------------------|-------------|
| `environment.name` | string | "GridWorld" | One of: GridWorld, ContinuousWorld, HybridWorld | Environment type |
| `environment.width` | int | 10 | > 0 | World width (cells) |
| `environment.height` | int | 10 | > 0 | World height (cells) |
| `environment.vision_width` | int | 8 | > 0 | Agent vision width |
| `environment.vision_height` | int | 8 | > 0 | Agent vision height |
| `environment.max_energy` | double | 100.0 | > 0 | Maximum energy for agents |

### Simulation Parameters

| Parameter | Type | Default | Range/Constraints | Description |
|-----------|------|---------|-------------------|-------------|
| `simulation.timestep` | double | 0.001 | (0.0, ∞) | Simulation timestep (seconds) |
| `simulation.max_steps` | int | 10000 | > 0 | Maximum simulation steps |
| `simulation.time_limit` | double | 0.0 | ≥ 0.0 | Simulation time limit (seconds) |
| `simulation.random_seed` | int64 | 42 | ≥ 0 | Random seed for reproducibility |
| `simulation.discount_factor` | double | 0.99 | [0.0, 1.0] | Reward discount factor |

### Logging Parameters

| Parameter | Type | Default | Range/Constraints | Description |
|-----------|------|---------|-------------------|-------------|
| `logging.level` | string | "INFO" | debug, info, warning, error, critical | Log level |
| `logging.to_file` | bool | false | - | Log to file enabled? |
| `logging.filename` | string | "nlm.log" | - | Log file name |
| `logging.max_size_mb` | int | 100 | > 0 | Maximum log file size |
| `logging.max_files` | int | 5 | ≥ 1 | Maximum number of log files |

### Visualization Parameters

| Parameter | Type | Default | Range/Constraints | Description |
|-----------|------|---------|-------------------|-------------|
| `visualization.enabled` | bool | false | - | Enable visualization |
| `visualization.update_rate` | double | 30.0 | (0.0, ∞) | Visualization update rate (Hz) |
| `visualization.window_width` | int | 1024 | > 0 | Window width (pixels) |
| `visualization.window_height` | int | 768 | > 0 | Window height (pixels) |

## Configuration File Formats

### Simple Format (.cfg)

```
# NLM Configuration File
# Key = value format

# Brain parameters
brain.neuron_count = 1000
brain.synapse_density = 0.1
brain.v_thresh = -50.0

# Plasticity parameters
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.structural.enable = true

# Simulation parameters
simulation.timestep = 0.001
simulation.random_seed = 42

# Logging parameters
logging.level = INFO
logging.to_file = false
```

### JSON Format (.json)

```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.001,
            "pruning_rate": 0.0001
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true,
            "learning_rate": 0.01
        }
    },
    "simulation": {
        "timestep": 0.001,
        "random_seed": 42,
        "discount_factor": 0.99
    },
    "logging": {
        "level": "INFO",
        "to_file": false
    }
}
```

### YAML Format (.yaml)

```yaml
# Brain parameters
brain:
    neuron_count: 1000
    synapse_density: 0.1
    v_thresh: -50.0
    v_rest: -70.0
    v_reset: -75.0

# Plasticity parameters
plasticity:
    stdp:
        enable: true
        learning_rate: 0.001
        tau_plus: 20.0
        tau_minus: 20.0
    structural:
        enable: true
        synaptogenesis_rate: 0.001
        pruning_rate: 0.0001

# Neuromodulation parameters
neuromod:
    dopamine:
        scale: 1.0
    curiosity:
        enable: true
        learning_rate: 0.01

# Simulation parameters
simulation:
    timestep: 0.001
    random_seed: 42
    discount_factor: 0.99

# Logging parameters
logging:
    level: INFO
    to_file: false
```

## Configuration Validation

### Validation Rules

1. **Type Validation**: All parameters must match their defined types
2. **Range Validation**: Numeric parameters must be within specified ranges
3. **Dependency Validation**: Some parameters depend on others being set
4. **Consistency Validation**: Parameters must be logically consistent
5. **Format Validation**: String parameters must match required formats

### Validation Examples

#### Invalid Configuration

```bash
# This will fail validation
brain.v_thresh = -40.0  # Should be -50.0 or less
brain.v_reset = -70.0   # Should be -75.0 or less (less than v_rest)

# This will fail validation
plasticity.stdp.enable = true
# Missing: plasticity.stdp.learning_rate, plasticity.stdp.tau_plus

# This will fail validation
simulation.discount_factor = 1.5  # Should be between 0.0 and 1.0
```

#### Valid Configuration

```bash
# This will pass validation
brain.neuron_count = 2000
brain.v_thresh = -55.0
brain.v_rest = -70.0
brain.v_reset = -75.0

plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.002
plasticity.stdp.tau_plus = 25.0

neuromod.dopamine.scale = 2.0

logging.level = "DEBUG"
logging.to_file = true
```

### Validation Error Messages

- **Type Error**: `"brain.neuron_count" has invalid type. Expected integer but got string`
- **Range Error**: `"simulation.discount_factor" has value 1.5 outside valid range [0.0, 1.0]`
- **Dependency Error**: `"plasticity.stdp.enable" requires "plasticity.stdp.learning_rate" to be set`
- **Consistency Error**: `"brain.v_reset" (\-70.0) must be less than "brain.v_rest" (-70.0)`
- **Format Error**: `"logging.level" must be one of: debug, info, warning, error, critical`

## Configuration Migration

### From Legacy Format

Legacy configuration formats may use different parameter names or formats. The NLM configuration system supports migration from legacy configurations:

```cpp
// Legacy configuration
std::unordered_map<std::string, ConfigValue> legacyConfig = {
    {"neuronCount", 1000},
    {"synapseDensity", 0.1},
    {"plasticityLearningRate", 0.01},
    {"randomSeed", 42}
};

// Migration
cfg->migrateFromLegacy(legacyConfig);
```

### Parameter Renaming

The configuration system maintains backward compatibility for commonly used parameter names:

| Old Name | New Name |
|----------|----------|
| `neuronCount` | `brain.neuron_count` |
| `synapseDensity` | `brain.synapse_density` |
| `plasticityLearningRate` | `plasticity.hebbian.learning_rate` |
| `randomSeed` | `simulation.random_seed` |
| `logLevel` | `logging.level` |

## Command Line Configuration

### Basic Usage

```bash
# Set a single parameter
nlm --brain.neuron_count=2000

# Set multiple parameters
nlm --brain.neuron_count=2000 --simulation.timestep=0.0005 --logging.level=DEBUG

# Use shorthand notation
nlm -brain.neuron_count 2000 -simulation.timestep 0.0005

# Load configuration file
nlm --config=myconfig.json run-experiment

# Combine command line and file
nlm --config=base.cfg --brain.neuron_count=3000 experiment
```

### Parameter Override Order

Configuration values are applied in this order (later overrides earlier):

1. Default values (from schema)
2. Configuration file
3. Command line arguments
4. Runtime settings (programmatically)

## Configuration Examples

### Minimal Configuration

```bash
nlm run-experiment
```

This uses all default values and produces a basic simulation.

### High-Performance Configuration

```bash
nlm --brain.neuron_count=5000 --brain.synapse_density=0.05
    --plasticity.stdp.enable=false --plasticity.hebbian.enable=true
    --simulation.timestep=0.0001 --logging.level=WARNING
    run-experiment
```

### Research Configuration

```bash
nlm --config=research.cfg --enable-neuromodulation
    --development-mode=true --save-checkpoints=true
    run-experiment --steps=50000
```

### Debug Configuration

```bash
nlm --brain.neuron_count=100 --simulation.timestep=0.01
    --logging.level=DEBUG --logging.to_file=true
    --visualization.enabled=true
    run-experiment --debug
```

## Troubleshooting

### Configuration Validation Errors

If you encounter validation errors:

1. **Check parameter names**: Ensure you're using the correct parameter names from this guide
2. **Verify parameter types**: Double-check that values match expected types
3. **Check ranges**: Ensure numeric values are within specified ranges
4. **Resolve dependencies**: Make sure required dependent parameters are set

### Performance Issues

- For large-scale simulations, use `simulation.timestep=0.0001` and enable validation
- Consider disabling validation for production runs: `--validation-enabled=false`
- Use configuration files for complex setups

### File Format Issues

- JSON files: Ensure proper syntax and quoting
- YAML files: Be careful with indentation
- Simple format: Use `=`, `:`, or spaces as separators

## Advanced Configuration

### Configuration Subsystems

The configuration system can be accessed programmatically:

```cpp
#include "core/Config/Config.hpp"

// Create configuration
auto config = std::make_shared<Config>();

// Enable validation
ValidationOptions opts;
opts.enabled = true;
opts.validateTypes = true;
opts.validateRanges = true;
opts.validateDependencies = true;
opts.validateConsistency = true;
config->setValidationOptions(opts);

// Set parameters with validation
config->set("brain.neuron_count", 2000);
config->set("plasticity.stdp.enable", true);

// Validate configuration
if (!config->isValid()) {
    std::cout << "Configuration invalid!" << std::endl;
    auto report = config->getAnalysisReport();
    std::cout << report << std::endl;
}
```

### Configuration Validation API

The configuration validation API provides:

- Parameter validation
- Configuration consistency checking
- Warning collection
- Analysis reports
- Migration support

## Configuration Monitoring

### Runtime Validation

Enable runtime validation for production:

```bash
nlm --validation-enabled=true --validation-log=validation.log
    run-experiment
```

### Configuration Statistics

The configuration system provides statistics about parameter usage:

```cpp
auto stats = config->getValidationStatistics();
std::cout << "Success rate: " << stats.getSuccessRate() << "%" << std::endl;
std::cout << "Errors by parameter:" << std::endl;
for (const auto& pair : stats.parameterFailures) {
    std::cout << "  " << pair.first << ": " << pair.second << std::endl;
}
```

## Configuration Versioning

The configuration system supports versioning for:

- Parameter schema changes
- Validation rule updates
- Format migrations
- Backward compatibility

Always check the configuration version when upgrading NLM.

## References

- [NLM Architecture Documentation](docs/ARCHITECTURE.md)
- [Python API Configuration](python/README.md)
- [Experiment Configuration](docs/EXPERIMENTS.md)
