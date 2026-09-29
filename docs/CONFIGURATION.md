# NLM Configuration File Format Documentation

## Overview

This document describes the configuration file format used by the NLM (Neural Learning Machine) project. Configuration files are used to specify simulation parameters, system behavior, and experiment settings.

NLM supports two configuration file formats:

1. **Simple key=value format** (Phase 1/2 style) - For basic configuration
2. **JSON format** (Phase 6+) - For comprehensive configuration with nested structures

## Simple key=value Format

The simple format is line-based, with each line containing a key-value pair:

```
# This is a comment
key = value
another_key = "quoted value"
boolean_key = true
float_key = 3.14
integer_key = 42
```

### Basic Syntax Rules:

- Lines starting with `#` or `/` are treated as comments
- Empty lines are ignored
- Keys and values are separated by `=`
- Values can be:
  - **Strings**: Enclosed in double quotes `"string"` or single quotes `'string'`
  - **Numbers**: Integers, floats, scientific notation
  - **Booleans**: `true` or `false`
  - **Vectors**: Space-separated lists, e.g., `vector = 1 2 3`

### Configuration Categories:

#### Core Simulation Settings
```
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
```

#### Plasticity Settings
```
# Plasticity learning rate
plasticity_learning_rate = 0.01

# STDP (Spike-Timing-Dependent Plasticity) parameters
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0
```

#### Development Settings
```
# Development parameters
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
```

#### Neuromodulation Settings
```
# Neuromodulator baseline levels
dopamine_baseline = 0.1
```

#### Reward Settings
```
# Reward calculation parameters
reward_discount_factor = 0.99
```

#### Environment Settings
```
# Environment configuration
environment_name = GridWorld
environment_width = 10
environment_height = 10
```

#### Logging Settings
```
# Logging configuration
log_level = INFO
log_to_file = false
log_filename = nlm.log
```

#### Simulation Settings
```
# Simulation limits
max_simulation_steps = 10000
simulation_time_limit = 0.0
```

#### Visualization Settings
```
# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

## JSON Format

The JSON format provides a more structured and comprehensive configuration approach. It's especially useful for:

- Large configurations
- Configurations with complex nested structures
- Programmatic configuration generation
- Version control of configurations

### Basic JSON Syntax:

```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_time_constant": 20.0,
    "working_memory_capacity_factor": 0.1,
    "episodic_memory_max_episodes": 1000,
    "attention_inhibition_strength": 0.5,
    "dopamine_excitability_mod": 0.5,
    "prediction_confidence_threshold": 0.7,
    "checkpoint_dir": "./checkpoints",
    "checkpoint_max_saves": 10,
    "checkpoint_save_interval": 10000,
    "checkpoint_compression": "balanced"
}
```

### JSON Data Types:

- **Numbers**: `{ "key": 42 }`, `{ "key": 3.14 }`, `{ "key": -2.5e-3 }`
- **Strings**: `{ "key": "value" }`, `{ "key": "GridWorld" }`
- **Booleans**: `{ "key": true }`, `{ "key": false }`
- **Arrays**: `{ "key": [1, 2, 3] }`, `{ "key": ["a", "b", "c"] }`

### JSON Configuration Categories:

#### Phase 6 Extended Settings
```json
{
    "working_memory_settings": {
        "capacity_factor": 0.1,
        "decay_rate": 0.01,
        "maintenance_strength": 2.0
    },
    "episodic_memory_settings": {
        "max_episodes": 1000,
        "replay_interval": 100,
        "consolidation_interval": 1000,
        "replay_enabled": true
    },
    "development_settings": {
        "stage_duration": 1000,
        "plasticity_factor": 1.0
    },
    "attention_settings": {
        "inhibition_strength": 0.5,
        "excitation_strength": 1.5,
        "competition_threshold": 0.5
    },
    "planning_settings": {
        "planning_depth": 5,
        "action_evaluation_window": 10
    },
    "neuromodulation_settings": {
        "dopamine_excitability_mod": 0.5,
        "curiosity_novelty_weight": 0.5,
        "curiosity_prediction_error_weight": 0.5
    },
    "prediction_settings": {
        "confidence_threshold": 0.7,
        "memory_decay_rate": 0.05
    },
    "checkpoint_settings": {
        "directory": "./checkpoints",
        "max_saves": 10,
        "save_interval": 10000,
        "compression": "balanced"
    }
}
```

## Configuration Value Types

NLM configuration supports the following value types:

### 1. Integer Values
```
integer_key = 42
```

### 2. Floating-Point Values
```
float_key = 3.14
negative_float = -2.5
scientific_notation = 1.23e-4
```

### 3. Boolean Values
```
boolean_enabled = true
boolean_disabled = false
```

### 4. String Values
```
string_value = "hello"
path_value = "configs/default.cfg"
environment_name = "GridWorld"
```

### 5. Vector/Array Values
```
vector_int = 1 2 3 4 5
vector_float = 0.1 0.2 0.3
vector_string = "item1" "item2" "item3"
```

## Configuration Loading Order

NLM uses a multi-stage configuration loading process:

1. **Default Configuration**: Built-in default values
2. **Configuration File**: Loaded from `.cfg` or `.json` files
3. **Command Line Arguments**: Override file settings
4. **Runtime Overrides**: Programmatic setting changes

### Priority Order (Highest to Lowest):

1. Command Line Arguments
2. Configuration File
3. Default Values
4. Runtime Settings

## Configuration Validation

NLM performs basic validation on configuration values:

### Required Values:
- `neuron_count`: Must be positive
- `region_count`: Must be positive
- `simulation_timestep`: Must be positive and small
- `connection_probability`: Must be between 0.0 and 1.0

### Optional Values:
- Missing optional values use reasonable defaults
- Invalid optional values may be clamped or defaulted

## Configuration Examples

### Minimal Configuration
```
# Minimal configuration for testing
random_seed = 42
simulation_timestep = 0.001
neuron_count = 100
region_count = 1
```

### Comprehensive Configuration
```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "working_memory_capacity_factor": 0.1,
    "episodic_memory_max_episodes": 1000,
    "attention_inhibition_strength": 0.5,
    "dopamine_excitability_mod": 0.5,
    "prediction_confidence_threshold": 0.7,
    "checkpoint_dir": "./checkpoints"
}
```

### Configuration for Research
```
# Research configuration with extended settings
random_seed = 12345
simulation_timestep = 0.0005
neuron_count = 2000
region_count = 3
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.018
stdp_time_constant = 25.0
development_synaptogenesis_rate = 0.002
development_pruning_rate = 0.0002
dopamine_baseline = 0.2
reward_discount_factor = 0.95
environment_name = "ComplexWorld"
environment_width = 20
environment_height = 20
working_memory_capacity_factor = 0.15
working_memory_decay_rate = 0.005
episodic_memory_max_episodes = 5000
attention_inhibition_strength = 0.6
attention_excitation_strength = 1.2
attention_competition_threshold = 0.6
planner_planning_depth = 8
prediction_confidence_threshold = 0.8
checkpoint_dir = "./research_checkpoints"
checkpoint_max_saves = 50
checkpoint_save_interval = 5000
```

## Command Line Configuration

Configuration values can also be specified via command line arguments:

### Key-Value Arguments:
```bash
./nlm --neuron_count=500 --simulation_timestep=0.001 --log_level=DEBUG
```

### Short Arguments:
```bash
./nlm -n 500 -t 0.001 -l DEBUG
```

### Argument Priority:
Command line arguments override configuration file values.

## Best Practices

### 1. Reproducibility
- Always specify `random_seed` for reproducible experiments
- Use JSON format for complex experiments
- Commit configuration files to version control

### 2. Validation
- Test configuration files before running experiments
- Use different configuration files for different experimental conditions
- Validate configuration values programmatically

### 3. Organization
- Group related settings in sections (JSON) or comments (simple format)
- Use descriptive key names
- Add comments to explain complex settings

### 4. Performance
- Use minimal configurations for performance testing
- Optimize for your specific use case
- Test performance with different configuration values

## File Reference

### Configuration File Examples
- `configs/default.cfg`: Default configuration for basic testing
- `configs/research.cfg`: Configuration for research experiments
- `configs/performance.cfg`: Configuration optimized for performance
- `configs/debug.cfg`: Configuration for debugging

### Configuration Documentation
- This file: `docs/CONFIGURATION.md`
- Command line arguments: `docs/CLI_ARGS.md`
- Advanced configuration: `docs/ADVANCED_CONFIG.md`

## Troubleshooting

### Common Issues

#### Configuration File Not Found
- Ensure the configuration file path is correct
- Check file permissions
- Verify file existence

#### Invalid Configuration Values
- Check that values are in the correct format
- Verify that ranges are valid (e.g., probabilities between 0 and 1)
- Use default values for missing required settings

#### Configuration Conflicts
- Command line arguments should override file settings
- Resolve conflicting configuration sources
- Check for duplicate configuration keys

### Validation Commands
```bash
# Validate configuration file syntax
./nlm --config=config.json --validate

# Test configuration loading
./nlm --config=config.json --test_config

# Check configuration compatibility
./nlm --config=config.json --check_compatibility
```

## Version Notes

### Version 0.1.0 (Current)
- Initial configuration file support
- Simple key=value format
- Basic JSON parser

### Version 1.0.0 (Future)
- Enhanced JSON format with nested structures
- Configuration validation
- Configuration templates
- Configuration comparison tools

### Version 2.0.0 (Future)
- Configuration inheritance
- Configuration templates
- Machine learning based configuration optimization
- Automated configuration tuning

## References

1. NLM Project Documentation
2. Configuration System Design Notes
3. Command Line Interface Documentation
4. Phase 6 Integration Specifications

---
*Last updated: September 2026*
*NLM Configuration System Documentation*  
*Phase 6: Final Integration*
