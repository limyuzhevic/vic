# NLM CLI Reference

This document provides comprehensive information about the NLM (Neural Learning Machine) command line interface.

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. This document covers the command line interface, configuration options, and available templates.

## Command Line Syntax

```
nlm [OPTIONS] [EXTRA_ARGS]
```

## General Options

### `--help [TOPIC]`
Show help information about the specified topic or general help if no topic is provided.

**Available Topics:**
- `general` - General NLM information and capabilities
- `commands` - Available commands and operations
- `config` - Configuration options and formats
- `templates` - Available configuration templates
- `validation` - Configuration validation options

**Examples:**
```bash
nlm --help                    # Show general help
nlm --help commands           # Show available commands
nlm --help config              # Show configuration options
nlm --help templates           # Show available templates
nlm --help validation          # Show validation options
```

### `--version`
Show version information and build details.

**Example:**
```bash
nlm --version
```

### `--commands`
List all available commands and operations.

**Example:**
```bash
nlm --commands
```

### `--validate`
Validate configuration before running. This will check for type errors, range violations, format issues, and dependency problems.

**Example:**
```bash
nlm --validate
```

### `--templates`
List all available configuration templates.

**Example:**
```bash
nlm --templates
```

### `--config FILE`
Load configuration from the specified file.

**File Formats:**
- JSON format (preferred): `configs/default.json`
- Key=value format (legacy): `configs/default.cfg`

**Examples:**
```bash
nlm --config myconfig.json
nlm --config=prod.cfg
```

### `--template NAME`
Apply a predefined configuration template to the current configuration.

**Examples:**
```bash
nlm --template beginner
nlm --template research
nlm --template performance
nlm --template learning
```

## Configuration Arguments

### `--key=value`
Set a configuration value. This overrides values loaded from the configuration file.

**Supported Configuration Categories:**

#### Brain Configuration
- `brain.neuron_count` - Total number of neurons (int: [100, 100000])
- `brain.region_count` - Number of brain regions (int: [1, 100])
- `brain.connection_probability` - Connection probability for random connectivity (double: [0.0, 1.0])
- `brain.initial_weight_mean` - Mean initial synaptic weight (double: [-1.0, 1.0])
- `brain.initial_weight_std` - Standard deviation of initial weights (double: [0.0, 2.0])
- `brain.v_thresh` - Neuron threshold potential (mV) (double: [-100.0, -20.0])
- `brain.v_rest` - Resting potential (mV) (double: [-100.0, 0.0])
- `brain.v_reset` - Reset potential after spike (mV) (double: [-100.0, 0.0])
- `brain.tau_mem` - Membrane time constant (ms) (double: [1.0, 100.0])
- `brain.tau_ref` - Refractory period (ms) (double: [0.0, 20.0])

#### Plasticity Configuration
- `plasticity.stdp.enable` - Enable STDP plasticity (bool)
- `plasticity.stdp.learning_rate` - STDP learning rate (double: [0.0, 1.0])
- `plasticity.stdp.tau_plus` - STDP time constant for potentiation (ms) (double: [1.0, 100.0])
- `plasticity.stdp.tau_minus` - STDP time constant for depression (ms) (double: [1.0, 100.0])
- `plasticity.hebbian.enable` - Enable Hebbian plasticity (bool)
- `plasticity.hebbian.learning_rate` - Hebbian learning rate (double: [0.0, 1.0])
- `plasticity.structural.enable` - Enable structural plasticity (bool)
- `plasticity.structural.synaptogenesis_rate` - Synaptogenesis rate (double: [0.0, 1.0])
- `plasticity.structural.pruning_rate` - Pruning rate (double: [0.0, 1.0])

#### Neuromodulation Configuration
- `neuromod.dopamine.scale` - Dopamine modulation scale (double: [0.0, 10.0])
- `neuromod.curiosity.enable` - Enable curiosity-driven exploration (bool)
- `neuromod.novelty.enable` - Enable novelty detection (bool)
- `neuromod.reward.scale` - Reward modulation scale (double: [0.0, 10.0])
- `neuromod.reward.discount_factor` - Reward discount factor (double: [0.0, 1.0])

#### Development Configuration
- `development.enable` - Enable developmental processes (bool)
- `development.critical_period_length` - Critical period length (ms) (double: [0.0, 1000.0])
- `development.maturation_rate` - Brain maturation rate (double: [0.0, 1.0])
- `development.aging_rate` - Brain aging rate (double: [0.0, 0.1])

#### Agent Configuration
- `agent.enable_reward_modulation` - Enable reward modulation (bool)
- `agent.enable_structural_plasticity` - Enable structural plasticity (bool)
- `agent.enable_development` - Enable development (bool)
- `agent.enable_curiosity` - Enable curiosity (bool)
- `agent.exploration_rate` - Agent exploration rate (double: [0.0, 1.0])

#### Environment Configuration
- `environment.name` - Environment name (string)
- `environment.width` - Environment width (int: [1, 1000])
- `environment.height` - Environment height (int: [1, 1000])
- `environment.max_energy` - Maximum energy in environment (double: [0.0, 10000.0])
- `environment.vision_width` - Agent vision width (int: [1, 100])
- `environment.vision_height` - Agent vision height (int: [1, 100])

#### Simulation Configuration
- `simulation.max_steps` - Maximum simulation steps (int: [1, 1000000])
- `simulation.time_limit` - Simulation time limit (s) (double: [0.0, 1000.0])
- `simulation.verbose` - Enable verbose logging (bool)

### `-key value`
Alternative format for configuration arguments (primarily for compatibility).

**Example:**
```bash
nlm -neuron_count 1000
```

## Configuration File Formats

### JSON Format (Recommended)
JSON configuration files use a nested structure with a top-level "nlm" key:

```json
{
    "nlm": {
        "version": "2.0",
        "brain": {
            "neuron_count": 1000,
            "connection_probability": 0.1
        },
        "plasticity": {
            "stdp": {
                "enable": true,
                "learning_rate": 0.01
            }
        }
    }
}
```

### Key=Value Format (Legacy)
Legacy configuration files use simple key=value pairs:

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

## Configuration Templates

NLM provides several pre-configured templates for common use cases:

### Beginner Template (`beginner.json`)
- Minimal configuration for first-time users
- Simplified key=value format
- Reasonable defaults for basic experimentation

### Research Template (`research.json`)
- Extensive configuration options
- Advanced plasticity settings
- Detailed neuromodulation
- Full developmental processes

### Performance Template (`performance.json`)
- Optimized for speed
- Reduced logging
- Simplified connectivity
- Minimal plasticity

### Learning Template (`learning.json`)
- Enhanced plasticity
- Strong learning signals
- Curiosity-driven exploration
- Full development stages

## Examples

### Basic Usage
```bash
# Run with default configuration
nlm

# Load JSON configuration
nlm --config myconfig.json

# Override configuration values
nlm --config config.json --brain.neuron_count=2000 --plasticity.stdp.enable=true
```

### Configuration Validation
```bash
# Validate configuration before running
nlm --validate

# Validate with specific configuration file
nlm --config myconfig.json --validate
```

### Template Application
```bash
# List available templates
nlm --templates

# Apply beginner template
nlm --template beginner

# Apply research template
nlm --template research
```

### Help and Discovery
```bash
# Show general help
nlm --help

# Show specific help topic
nlm --help config

# List available commands
nlm --commands
```

## Error Handling

NLM provides clear, actionable error messages:

### Configuration Errors
- **Type Mismatch**: "Configuration error for key 'brain.neuron_count': Invalid type. Expected int. Current value: 'abc'"
- **Range Violation**: "Configuration error for key 'brain.neuron_count': Value out of range. Expected between [100, 100000]. Current value: 50"
- **Format Error**: "Configuration error for key 'environment.name': Invalid format. Current value: 'invalid name with spaces'"
- **Dependency Violation**: "Dependency violation: plasticity.stdp.tau_plus requires plasticity.stdp.enable to be enabled"

### File Errors
- **File Not Found**: "Unable to load configuration from: myconfig.json (file not found)"
- **Invalid Format**: "Unable to parse configuration file: myconfig.json (invalid JSON format)"

### Command Line Errors
- **Unknown Option**: "Unknown command line option: --unknown"
- **Missing Argument**: "Missing required argument for option --template"

## Tips and Best Practices

### For Beginners
1. Start with the beginner template: `nlm --template beginner`
2. Use `--help` to explore available options
3. Validate your configuration before running: `nlm --validate`
4. Check the documentation in `docs/CLI_REFERENCE.md` and `docs/CONFIG_REFERENCE.md`

### For Advanced Users
1. Use JSON format for complex configurations
2. Combine templates with command-line overrides
3. Use validation to catch configuration issues early
4. Experiment with different configuration combinations

### For Troubleshooting
1. Use `--verbose` to see detailed logs
2. Check error messages for specific guidance
3. Consult the documentation for configuration parameters
4. Use the validation system to identify issues

## See Also

- [CONFIG_REFERENCE.md](CONFIG_REFERENCE.md) - Complete configuration parameter reference
- [NLM Documentation](https://github.com/yourusername/nlm) - General documentation
- [NLM Examples](https://github.com/yourusername/nlm-examples) - Example configurations and scripts

## Version Information

This CLI is part of NLM Phase 2: Real Neural Computation
- Version: 2.0.0
- Build Date: (compiler date)
- C++ Standard: C++20