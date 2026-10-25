# Sample NLM Configuration Files

This directory contains example configuration files for NLM (Neural Learning Machine).

## Configuration File Formats

### JSON Configuration (.json)

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1,
        "initial_weight_mean": 0.5,
        "initial_weight_std": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.01
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.0001f,
            "pruning_rate": 0.00001f
        }
    },
    "neuromodulation": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true,
            "exploration_rate": 0.5
        },
        "novelty": {
            "enable": true,
            "threshold": 0.3
        }
    },
    "development": {
        "enable": true,
        "initial_stage": "Initial",
        "maturation_rate": 0.1,
        "plasticity_decay": 0.05
    },
    "memory": {
        "working_memory": {
            "capacity": 100,
            "decay_rate": 0.01
        },
        "episodic_memory": {
            "max_episodes": 1000,
            "replay_interval": 100
        },
        "associative_memory": {
            "pattern_threshold": 0.8,
            "association_strength": 0.5
        }
    },
    "config": {
        "version": "2.0",
        "format": "json",
        "validate_on_load": true,
        "validate_on_set": false,
        "auto_save_enabled": false,
        "auto_save_interval": 0.0
    }
}
```

### YAML Configuration (.yaml)

```yaml
brain:
  neuron_count: 1000
  region_count: 1
  connection_probability: 0.1
  initial_weight_mean: 0.5
  initial_weight_std: 0.1
  v_thresh: -50.0
  v_rest: -70.0
  v_reset: -75.0
  tau_mem: 20.0
  tau_ref: 2.0

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
    tau_plus: 20.0
    tau_minus: 20.0
  hebbian:
    enable: true
    learning_rate: 0.01
  structural:
    enable: true
    synaptogenesis_rate: 0.0001
    pruning_rate: 0.00001

neuromodulation:
  dopamine:
    scale: 1.0
  curiosity:
    enable: true
    exploration_rate: 0.5
  novelty:
    enable: true
    threshold: 0.3

development:
  enable: true
  initial_stage: Initial
  maturation_rate: 0.1
  plasticity_decay: 0.05

memory:
  working_memory:
    capacity: 100
    decay_rate: 0.01
  episodic_memory:
    max_episodes: 1000
    replay_interval: 100
  associative_memory:
    pattern_threshold: 0.8
    association_strength: 0.5

config:
  version: "2.0"
  format: json
  validate_on_load: true
  validate_on_set: false
  auto_save_enabled: false
  auto_save_interval: 0.0
```

### Simple Configuration (.cfg)

```
# NLM Simple Configuration File
# Generated from NLM configuration system

# Brain Configuration
brain.neuron_count = 1000
brain.region_count = 1
brain.connection_probability = 0.1
brain.initial_weight_mean = 0.5
brain.initial_weight_std = 0.1
brain.v_thresh = -50.0
brain.v_rest = -70.0
brain.v_reset = -75.0
brain.tau_mem = 20.0
brain.tau_ref = 2.0

# Plasticity Configuration
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.stdp.tau_plus = 20.0
plasticity.stdp.tau_minus = 20.0
plasticity.hebbian.enable = true
plasticity.hebbian.learning_rate = 0.01
plasticity.structural.enable = true
plasticity.structural.synaptogenesis_rate = 0.0001
plasticity.structural.pruning_rate = 0.00001

# Neuromodulation Configuration
neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

# Development Configuration
development.enable = true
development.initial_stage = Initial
development.maturation_rate = 0.1
development.plasticity_decay = 0.05

# Memory Configuration
memory.working_memory.capacity = 100
memory.working_memory.decay_rate = 0.01
memory.episodic_memory.max_episodes = 1000
memory.episodic_memory.replay_interval = 100
memory.associative_memory.pattern_threshold = 0.8
memory.associative_memory.association_strength = 0.5

# Configuration System
config.version = "2.0"
config.format = json
config.validate_on_load = true
config.validate_on_set = false
config.auto_save_enabled = false
config.auto_save_interval = 0.0
```

## Configuration Loading Examples

### Loading from JSON

```bash
# Using CLI
nlm-cli config load config.json --format json --validate

# Using Python API
import pynlm
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")
```

### Loading from YAML

```bash
nlm-cli config load config.yaml --format yaml
```

### Loading from Simple Format

```bash
nlm-cli config load config.cfg
```

## Configuration Saving Examples

### Saving to JSON

```bash
nlm-cli config save config.json --format json
```

### Saving to YAML

```bash
nlm-cli config save config.yaml --format yaml
```

### Saving to Simple Format

```bash
nlm-cli config save config.cfg
```

## Configuration Management Examples

### Setting Configuration Values

```bash
nlm-cli config set brain.neuron_count 2000
nlm-cli config set plasticity.stdp.learning_rate 0.002 --source command-line
```

### Getting Configuration Values

```bash
nlm-cli config get brain.neuron_count
nlm-cli config get plasticity.stdp.learning_rate --format json
```

### Listing Configuration Keys

```bash
nlm-cli config list
nlm-cli config list --format table
nlm-cli config list --format json
```

### Validating Configuration

```bash
nlm-cli config validate
nlm-cli config validate --strict
nlm-cli config validate --fix
```

### Automatic Configuration Saving

```bash
nlm-cli config autosave checkpoints/config.json 1000 --enabled
```

## Advanced Configuration Usage

### Creating a Default Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Modify specific settings
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("neuromod.curiosity.enable", True)

# Save to file
config.saveToFile("my_config.json")

# Load from file
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")
```

### Configuration Validation

```python
# Validate configuration before use
config = pynlm.createDefaultConfig()

# Check if key exists
if config.has("brain.neuron_count"):
    neuron_count = config.get("brain.neuron_count").value()

# Get with default value
default_neurons = config.getOr("brain.neuron_count", 1000)

# List all keys
keys = config.getKeys()

# Get configuration summary
summary = config.summary()
```

### Command Line Arguments

```bash
# Load configuration from command line arguments
./nlm --config config.json --neuron-count 1500 --connection-probability 0.15

# Set configuration via command line
./nlm config set brain.neuron_count 2000 --source command-line

# Load configuration file via command line
./nlm --config my_config.yaml
```

## Configuration File Best Practices

1. **Use JSON for structured data**: JSON provides better parsing and validation.
2. **Use YAML for human-readable configuration**: YAML is easier to read and write.
3. **Use simple format for legacy compatibility**: The simple key=value format works well for quick scripting.
4. **Validate configuration on load**: Always validate configuration files to catch errors early.
5. **Use meaningful key names**: Use dot notation for nested configuration values.
6. **Include comments**: Add comments to configuration files to explain settings.
7. **Backup important configurations**: Always backup important configuration files.

## Configuration System Features

1. **Multiple formats**: Support for JSON, YAML, and simple key=value format.
2. **Auto-save**: Automatic configuration saving at specified intervals.
3. **Validation**: Configuration validation with optional auto-fix capabilities.
4. **Source tracking**: Track configuration sources (default, file, command-line, runtime).
5. **Type safety**: Strong typing with variant-based configuration values.
6. **Error handling**: Comprehensive error handling and reporting.
7. **Backward compatibility**: Support for legacy configuration formats.
8. **Documentation**: Built-in documentation and help system.

## Next Steps

1. **Try the examples**: Run the configuration examples to understand the system.
2. **Create your own configuration files**: Customize configuration for your specific needs.
3. **Explore the CLI**: Use the enhanced NLM CLI tool for configuration management.
4. **Integrate with your simulations**: Use configuration files in your NLM simulations.
5. **Extend the system**: Add custom configuration validation and parsing.

For more information, see the NLM documentation and examples in the `docs/` directory.