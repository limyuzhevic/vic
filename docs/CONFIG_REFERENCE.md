# NLM Configuration Reference

This document provides comprehensive information about all configuration parameters available in the NLM (Neural Learning Machine) project.

## Overview

NLM uses a flexible configuration system that supports multiple formats (JSON and key=value) and includes comprehensive validation. Configuration can be specified through:

1. Configuration files (`--config FILE`)
2. Command-line arguments (`--key=value`)
3. Predefined templates (`--template NAME`)
4. Default values (built into the code)

Configuration files are searched in the following order:
1. Command-line specified file (`--config FILE`)
2. Default file (`configs/default.cfg`)
3. No configuration (using only defaults)

## Configuration Categories

Configuration parameters are organized into logical categories for easier management:

### Brain Configuration
Controls the neural network architecture and neuron properties.

#### Parameters

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons in the brain | [100, 100000] |
| `brain.region_count` | int | 2 | Number of brain regions | [1, 100] |
| `brain.connection_probability` | double | 0.1 | Probability of connection between neurons | [0.0, 1.0] |
| `brain.initial_weight_mean` | double | 0.5 | Mean initial synaptic weight | [-1.0, 1.0] |
| `brain.initial_weight_std` | double | 0.1 | Standard deviation of initial weights | [0.0, 2.0] |
| `brain.v_thresh` | double | -50.0 | Neuron threshold potential (mV) | [-100.0, -20.0] |
| `brain.v_rest` | double | -70.0 | Resting potential (mV) | [-100.0, 0.0] |
| `brain.v_reset` | double | -75.0 | Reset potential after spike (mV) | [-100.0, 0.0] |
| `brain.tau_mem` | double | 20.0 | Membrane time constant (ms) | [1.0, 100.0] |
| `brain.tau_ref` | double | 2.0 | Refractory period (ms) | [0.0, 20.0] |

#### Example Configuration
```json
"brain": {
    "neuron_count": 2000,
    "region_count": 3,
    "connection_probability": 0.08,
    "initial_weight_mean": 0.5,
    "initial_weight_std": 0.2,
    "v_thresh": -50.0,
    "v_rest": -70.0,
    "v_reset": -75.0,
    "tau_mem": 20.0,
    "tau_ref": 2.0
}
```

### Plasticity Configuration
Controls learning mechanisms that modify synaptic connections.

#### Parameters

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity | - |
| `plasticity.stdp.learning_rate` | double | 0.001 | STDP learning rate | [0.0, 1.0] |
| `plasticity.stdp.tau_plus` | double | 20.0 | STDP time constant for potentiation (ms) | [1.0, 100.0] |
| `plasticity.stdp.tau_minus` | double | 20.0 | STDP time constant for depression (ms) | [1.0, 100.0] |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning | - |
| `plasticity.hebbian.learning_rate` | double | 0.001 | Hebbian learning rate | [0.0, 1.0] |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity | - |
| `plasticity.structural.synaptogenesis_rate` | double | 0.001 | Synaptogenesis rate | [0.0, 1.0] |
| `plasticity.structural.pruning_rate` | double | 0.0001 | Pruning rate | [0.0, 1.0] |

#### Example Configuration
```json
"plasticity": {
    "stdp": {
        "enable": true,
        "learning_rate": 0.02,
        "tau_plus": 20.0,
        "tau_minus": 20.0
    },
    "hebbian": {
        "enable": true,
        "learning_rate": 0.005
    },
    "structural": {
        "enable": true,
        "synaptogenesis_rate": 0.001,
        "pruning_rate": 0.0001
    }
}
```

### Neuromodulation Configuration
Controls reward and prediction systems that guide behavior.

#### Parameters

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `neuromod.dopamine.scale` | double | 1.0 | Dopamine modulation scale | [0.0, 10.0] |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration | - |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection | - |
| `neuromod.reward.scale` | double | 1.0 | Reward modulation scale | [0.0, 10.0] |
| `neuromod.reward.discount_factor` | double | 0.99 | Reward discount factor | [0.0, 1.0] |

#### Example Configuration
```json
"neuromodulation": {
    "dopamine": {
        "scale": 1.5,
        "baseline": 0.1
    },
    "curiosity": {
        "enable": true,
        "threshold": 0.3
    },
    "novelty": {
        "enable": true
    },
    "reward": {
        "scale": 2.0,
        "discount_factor": 0.95
    }
}
```

### Development Configuration
Controls brain maturation and development processes.

#### Parameters

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `development.enable` | bool | true | Enable developmental processes | - |
| `development.critical_period_length` | double | 100.0 | Critical period length (ms) | [0.0, 1000.0] |
| `development.maturation_rate` | double | 0.01 | Brain maturation rate | [0.0, 1.0] |
| `development.aging_rate` | double | 0.001 | Brain aging rate | [0.0, 0.1] |

#### Example Configuration
```json
"development": {
    "enable": true,
    "critical_period_length": 200.0,
    "maturation_rate": 0.02,
    "aging_rate": 0.005
}
```

### Agent Configuration
Controls agent behavior and interaction with the environment.

#### Parameters

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `agent.enable_reward_modulation` | bool | true | Enable reward modulation | - |
| `agent.enable_structural_plasticity` | bool | true | Enable structural plasticity | - |
| `agent.enable_development` | bool | true | Enable development | - |
| `agent.enable_curiosity` | bool | true | Enable curiosity | - |
| `agent.exploration_rate` | double | 0.1 | Agent exploration rate | [0.0, 1.0] |

#### Example Configuration
```json
"agent": {
    "enable_reward_modulation": true,
    "enable_structural_plasticity": true,
    "enable_development": true,
    "enable_curiosity": true,
    "exploration_rate": 0.2
}
```

### Environment Configuration
Controls the virtual environment where the agent operates.

#### Parameters

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `environment.name` | string | "GridWorld" | Environment name | - |
| `environment.width` | int | 10 | Environment width | [1, 1000] |
| `environment.height` | int | 10 | Environment height | [1, 1000] |
| `environment.max_energy` | double | 100.0 | Maximum energy in environment | [0.0, 10000.0] |
| `environment.vision_width` | int | 8 | Agent vision width | [1, 100] |
| `environment.vision_height` | int | 8 | Agent vision height | [1, 100] |

#### Example Configuration
```json
"environment": {
    "name": "GridWorld",
    "width": 20,
    "height": 20,
    "max_energy": 100.0,
    "vision_width": 8,
    "vision_height": 8
}
```

### Simulation Configuration
Controls simulation parameters and runtime behavior.

#### Parameters

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `simulation.max_steps` | int | 10000 | Maximum simulation steps | [1, 1000000] |
| `simulation.time_limit` | double | 0.0 | Simulation time limit (s) | [0.0, 1000.0] |
| `simulation.verbose` | bool | false | Enable verbose logging | - |

#### Example Configuration
```json
"simulation": {
    "max_steps": 5000,
    "time_limit": 10.0,
    "verbose": false
}
```

### Legacy Configuration Parameters

Legacy configuration parameters (key=value format) for backward compatibility:

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `random_seed` | int | 42 | Random seed for reproducible experiments |
| `simulation_timestep` | double | 0.001 | Simulation timestep in seconds |
| `neuron_count` | int | 1000 | Number of neurons in the brain |
| `region_count` | int | 2 | Number of brain regions |
| `connection_probability` | double | 0.1 | Connection probability for random connectivity |
| `plasticity_learning_rate` | double | 0.01 | Plasticity learning rate |
| `stdp_ltp_weight` | double | 0.01 | STDP LTP weight |
| `stdp_ltd_weight` | double | 0.012 | STDP LTD weight |
| `stdp_time_constant` | double | 20.0 | STDP time constant |
| `development_synaptogenesis_rate` | double | 0.001 | Synaptogenesis rate |
| `development_pruning_rate` | double | 0.0001 | Pruning rate |
| `dopamine_baseline` | double | 0.1 | Dopamine baseline value |
| `reward_discount_factor` | double | 0.99 | Reward discount factor |
| `environment_name` | string | "GridWorld" | Environment name |
| `environment_width` | int | 10 | Environment width |
| `environment_height` | int | 10 | Environment height |
| `log_level` | string | "INFO" | Log level |
| `log_to_file` | bool | false | Log to file |
| `log_filename` | string | "nlm.log" | Log filename |
| `max_simulation_steps` | int | 10000 | Maximum simulation steps |
| `simulation_time_limit` | double | 0.0 | Simulation time limit |
| `visualization_enabled` | bool | false | Enable visualization |
| `visualization_update_rate` | double | 30.0 | Visualization update rate |

## Configuration Validation

NLM includes comprehensive configuration validation to ensure parameter correctness:

### Validation Rules

1. **Type Validation**: Ensures parameters have the correct data type
2. **Range Validation**: Checks numeric parameters against defined ranges
3. **Format Validation**: Validates string formats (paths, patterns, etc.)
4. **Dependency Validation**: Ensures related parameters have valid combinations

### Common Validation Errors

- **Type Mismatch**: "Configuration error for key 'brain.neuron_count': Invalid type. Expected int. Current value: 'abc'"
- **Range Violation**: "Configuration error for key 'brain.neuron_count': Value out of range. Expected between [100, 100000]. Current value: 50"
- **Format Error**: "Configuration error for key 'environment.name': Invalid format. Current value: 'invalid name with spaces'"
- **Dependency Violation**: "Dependency violation: plasticity.stdp.tau_plus requires plasticity.stdp.enable to be enabled"

### Validation Commands

```bash
# Validate configuration before running
nlm --validate

# Validate with specific configuration file
nlm --config myconfig.json --validate
```

## Configuration Templates

Pre-configured templates for common use cases:

### Beginner Template
- Minimal configuration for first-time users
- Reasonable defaults for basic experimentation
- Simplified key=value format

### Research Template
- Extensive configuration options
- Advanced plasticity settings
- Detailed neuromodulation
- Full developmental processes

### Performance Template
- Optimized for speed
- Reduced logging
- Simplified connectivity
- Minimal plasticity

### Learning Template
- Enhanced plasticity
- Strong learning signals
- Curiosity-driven exploration
- Full development stages

## Configuration File Examples

### JSON Configuration
```json
{
    "nlm": {
        "version": "2.0",
        "brain": {
            "neuron_count": 2000,
            "connection_probability": 0.08
        },
        "plasticity": {
            "stdp": {
                "enable": true,
                "learning_rate": 0.02
            }
        },
        "environment": {
            "width": 20,
            "height": 20
        }
    }
}
```

### Key=Value Configuration
```
# Brain configuration
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# Plasticity configuration
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01

# Environment configuration
environment_name = GridWorld
environment_width = 10
environment_height = 10
```

## Command Line Configuration

Configuration can also be set via command line arguments:

```bash
# Set individual configuration values
nlm --brain.neuron_count=2000 --plasticity.stdp.enable=true

# Load configuration file and override values
nlm --config myconfig.json --brain.neuron_count=3000
```

## Configuration Sources Priority

When multiple configuration sources are available, NLM uses the following priority order:

1. **Command-line arguments** (`--key=value`) - Highest priority
2. **Configuration file** (`--config FILE`) - Medium priority
3. **Template** (`--template NAME`) - Medium priority
4. **Default values** (built into code) - Lowest priority

## Best Practices

### For Configuration Management
1. **Use JSON format for complex configurations** - It's easier to read and maintain
2. **Validate configuration before running** - Catch issues early
3. **Use templates for common configurations** - Reduce repetition
4. **Document your custom configurations** - Use comments in configuration files

### For Configuration Testing
1. **Always validate configuration** - Use `--validate` flag
2. **Test with minimal configuration** - Start with beginner template
3. **Test with maximum configuration** - Start with research template
4. **Check error messages** - They provide actionable guidance

### For Configuration Debugging
1. **Use verbose logging** - Enable `simulation.verbose = true`
2. **Check configuration summary** - View loaded configuration
3. **Validate dependencies** - Ensure related parameters are compatible
4. **Use templates** - They are pre-tested and validated

## Configuration Migration

### From Legacy Key=Value to JSON

Legacy configuration files (configs/default.cfg) can be converted to JSON using the `nlm --config` command:

```bash
# Load legacy configuration
nlm --config configs/default.cfg

# The system will automatically convert to JSON format
```

### Adding New Configuration Parameters

To add new configuration parameters:

1. **Define the parameter** in the appropriate section
2. **Add validation rules** in the ConfigValidator
3. **Update documentation** in this file
4. **Test the parameter** with existing tests
5. **Update examples** as needed

## Configuration Security

Configuration files may contain sensitive information. Follow these security guidelines:

1. **Use appropriate file permissions** - Restrict access to configuration files
2. **Avoid hardcoding secrets** - Use environment variables for sensitive data
3. **Validate all configuration values** - Ensure they meet security requirements
4. **Use templates for sensitive configurations** - Share templates without actual values

## Configuration Versioning

Configuration uses semantic versioning:

- **Major version**: Breaking changes to configuration format
- **Minor version**: New configuration options added
- **Patch version**: Bug fixes and improvements

Configuration files should include a version field:

```json
{
    "nlm": {
        "version": "2.0",
        ...
    }
}
```

## Configuration Logging

NLM logs configuration information when `simulation.verbose = true`:

```
Configuration (5 entries):
  brain.neuron_count = [1000] (0)
  brain.connection_probability = [0.1] (0)
  plasticity.stdp.enable = [true] (0)
  environment.width = [10] (0)
  environment.height = [10] (0)
```

Configuration source codes:
- `0` = Default
- `1` = File
- `2` = Command Line
- `3` = Runtime

## Configuration Troubleshooting

### Common Issues and Solutions

**Issue**: "Configuration error for key 'brain.neuron_count': Invalid type. Expected int. Current value: 'abc'"

**Solution**: Ensure the value is a number, not a string:
```bash
nlm --brain.neuron_count=1000
```

**Issue**: "Configuration error for key 'brain.neuron_count': Value out of range. Expected between [100, 100000]. Current value: 50"

**Solution**: Set the value within the valid range:
```bash
nlm --brain.neuron_count=500
```

**Issue**: "Dependency violation: plasticity.stdp.tau_plus requires plasticity.stdp.enable to be enabled"

**Solution**: Enable STDP before setting its parameters:
```bash
nlm --plasticity.stdp.enable=true --plasticity.stdp.tau_plus=20.0
```

### Getting Help with Configuration

```bash
# Show configuration help
nlm --help config

# Show validation help
nlm --help validation

# List available templates
nlm --help templates
```

## Configuration Example Scripts

### Create a Custom Configuration
```bash
#!/bin/bash
# Create a custom configuration for research
nlm --template research --brain.neuron_count=5000 > custom_research.json

# Validate the configuration
nlm --config custom_research.json --validate
```

### Batch Configuration Testing
```bash
#!/bin/bash
# Test multiple configurations
for config in beginner.json research.json performance.json learning.json; do
    echo "Testing $config..."
    nlm --template ${config%%.json} --validate
    if [ $? -eq 0 ]; then
        echo "✓ $config passed validation"
    else
        echo "✗ $config failed validation"
    fi
done
```

## Configuration Best Practices for Development

### For New Developers
1. **Read the configuration documentation** - Understand all available options
2. **Use templates for development** - Start with beginner or research template
3. **Validate configuration** - Always use `--validate` before running
4. **Check logs** - Enable verbose logging for debugging

### For Production Use
1. **Use performance template** - Optimized for speed and stability
2. **Test thoroughly** - Validate all configuration changes
3. **Monitor configuration** - Log configuration usage in production
4. **Backup configurations** - Save important configuration files

## Configuration Version History

### Version 1.0 (Legacy)
- Simple key=value format
- Basic configuration options
- Limited validation

### Version 2.0 (Current)
- JSON configuration format
- Advanced configuration options
- Comprehensive validation
- Template support
- Category-based organization

## Configuration Limits and Constraints

### System Limits
| Parameter | Minimum | Maximum | Notes |
|-----------|---------|---------|-------|
| `brain.neuron_count` | 100 | 100000 | System memory constraints |
| `brain.region_count` | 1 | 100 | Computational limits |
| `simulation.max_steps` | 1 | 1000000 | Time constraints |
| `simulation.time_limit` | 0.0 | 1000.0 | Performance constraints |

### Performance Considerations
- Higher `neuron_count` values increase memory usage
- More `region_count` values improve parallelization
- `verbose = true` increases logging overhead
- `visualization_enabled = true` requires additional resources

## Configuration Examples by Use Case

### Machine Learning Research
```json
{
    "nlm": {
        "brain": {
            "neuron_count": 5000,
            "region_count": 5
        },
        "plasticity": {
            "stdp": { "enable": true, "learning_rate": 0.02 },
            "hebbian": { "enable": true, "learning_rate": 0.01 },
            "structural": { "enable": true }
        },
        "neuromodulation": {
            "curiosity": { "enable": true },
            "novelty": { "enable": true },
            "reward": { "scale": 2.0 }
        },
        "development": {
            "enable": true,
            "maturation_rate": 0.03
        }
    }
}
```

### Real-Time Applications
```json
{
    "nlm": {
        "brain": {
            "neuron_count": 1000,
            "connection_probability": 0.05
        },
        "plasticity": {
            "stdp": { "enable": false },
            "hebbian": { "enable": true, "learning_rate": 0.001 },
            "structural": { "enable": false }
        },
        "simulation": {
            "max_steps": 1000,
            "time_limit": 10.0,
            "verbose": false
        }
    }
}
```

### Educational Demo
```json
{
    "nlm": {
        "brain": {
            "neuron_count": 500,
            "region_count": 1
        },
        "plasticity": {
            "stdp": { "enable": true, "learning_rate": 0.01 },
            "hebbian": { "enable": true, "learning_rate": 0.001 }
        },
        "environment": {
            "width": 10,
            "height": 10,
            "vision_width": 5,
            "vision_height": 5
        },
        "simulation": {
            "max_steps": 100,
            "verbose": true
        }
    }
}
```

## Configuration Summary

NLM provides a comprehensive configuration system that supports:

1. **Multiple formats**: JSON (recommended) and key=value (legacy)
2. **Comprehensive validation**: Type, range, format, and dependency checking
3. **Template support**: Four pre-configured templates for different use cases
4. **Category organization**: Brain, plasticity, neuromodulation, development, agent, environment, and simulation parameters
5. **Flexible overrides**: Command-line arguments override configuration files
6. **Clear error messages**: Actionable guidance for fixing configuration issues
7. **Extensive documentation**: Complete reference for all configuration parameters

This configuration system makes NLM accessible to beginners while providing advanced features for experienced users, ensuring a smooth and productive experience for all users.