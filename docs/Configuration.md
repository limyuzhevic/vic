# NLM Configuration System

## Overview

NLM uses a flexible configuration system that supports JSON, YAML, and INI formats. The configuration system manages brain parameters, simulation settings, and component-specific options. This guide covers the configuration schema, examples, and validation.

## Configuration File Formats

NLM supports three configuration file formats:

### JSON Configuration (`.json`)
```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        }
    }
}
```

### YAML Configuration (`.yaml`, `.yml`)
```yaml
brain:
  neuron_count: 1000
  region_count: 2
  connection_probability: 0.1

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
```

### INI Configuration (`.cfg`, `.ini`)
```ini
[brain]
neuron_count = 1000
region_count = 2
connection_probability = 0.1

[plasticity.stdp]
enable = true
learning_rate = 0.001
```

## Configuration Schema

### Core Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | integer | 1000 | Total number of neurons |
| `brain.region_count` | integer | 2 | Number of brain regions |
| `brain.connection_probability` | float | 0.1 | Probability of synapse formation |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron firing threshold (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Neural Dynamics Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `dynamics.neuron_types.excitatory` | boolean | true | Enable excitatory neurons |
| `dynamics.neuron_types.inhibitory` | boolean | true | Enable inhibitory neurons |
| `dynamics.neuron_types.sensory` | boolean | true | Enable sensory neurons |
| `dynamics.neuron_types.motor` | boolean | true | Enable motor neurons |
| `dynamics.neuron_types.modulatory` | boolean | true | Enable modulatory neurons |
| `dynamics.neuron_types.internal` | boolean | true | Enable internal neurons |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | boolean | true | Enable STDP learning |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | boolean | true | Enable Hebbian learning |
| `plasticity.structural.enable` | boolean | true | Enable structural plasticity |
| `plasticity.structural.synaptogenesis_rate` | float | 0.001 | Synapse formation rate |
| `plasticity.structural.pruning_rate` | float | 0.0001 | Synapse removal rate |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | boolean | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | boolean | true | Enable novelty detection |
| `neuromod.prediction_error.scale` | float | 1.0 | Prediction error impact |

### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.initial_plasticity` | float | 1.0 | Initial plasticity rate |
| `development.critical_period_duration` | float | 1000.0 | Critical period length |
| `development.maturation_rate` | float | 0.01 | Maturation progression rate |
| `development.synapse_maturation_rate` | float | 0.1 | Synapse maturation rate |

### Memory Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working_capacity` | integer | 100 | Working memory capacity |
| `memory.episodic_capacity` | integer | 1000 | Episodic memory capacity |
| `memory.associative_capacity` | integer | 500 | Associative memory capacity |
| `memory.replay_frequency` | integer | 100 | Replay cycle frequency |
| `memory.consolidation_threshold` | integer | 1000 | Consolidation trigger threshold |

### Prediction Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `prediction.horizon` | integer | 3 | Prediction time horizon |
| `prediction.confidence_decay` | float | 0.95 | Confidence decay factor |
| `prediction.error_threshold` | float | 0.1 | Prediction error threshold |

### Environment Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `environment.name` | string | GridWorld | Environment type |
| `environment.width` | integer | 20 | Environment width |
| `environment.height` | integer | 20 | Environment height |
| `environment.vision_width` | integer | 8 | Vision field of view width |
| `environment.vision_height` | integer | 8 | Vision field of view height |
| `environment.max_energy` | float | 100.0 | Maximum energy level |

### Logging Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `logging.level` | string | INFO | Log level (DEBUG, INFO, WARNING, ERROR) |
| `logging.to_file` | boolean | false | Enable file logging |
| `logging.filename` | string | nlm.log | Log file name |
| `logging.max_size` | integer | 10485760 | Maximum log file size (bytes) |
| `logging.backup_count` | integer | 5 | Number of backup log files |

## Complete Example Configuration

### JSON Example
```json
{
    "brain": {
        "neuron_count": 5000,
        "region_count": 3,
        "connection_probability": 0.05
    },
    "dynamics": {
        "neuron_types": {
            "excitatory": true,
            "inhibitory": true,
            "sensory": true,
            "motor": true
        }
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
            "learning_rate": 0.0005
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
            "enable": true
        },
        "novelty": {
            "enable": true
        },
        "prediction_error": {
            "scale": 1.0
        }
    },
    "development": {
        "initial_plasticity": 1.0,
        "critical_period_duration": 1000.0,
        "maturation_rate": 0.01,
        "synapse_maturation_rate": 0.1
    },
    "memory": {
        "working_capacity": 200,
        "episodic_capacity": 2000,
        "associative_capacity": 1000,
        "replay_frequency": 100,
        "consolidation_threshold": 1000
    },
    "prediction": {
        "horizon": 5,
        "confidence_decay": 0.95,
        "error_threshold": 0.1
    },
    "environment": {
        "name": "GridWorld",
        "width": 30,
        "height": 30,
        "vision_width": 10,
        "vision_height": 10,
        "max_energy": 200.0
    },
    "logging": {
        "level": "INFO",
        "to_file": true,
        "filename": "nlm_simulation.log",
        "max_size": 10485760,
        "backup_count": 5
    }
}
```

### YAML Example
```yaml
brain:
  neuron_count: 5000
  region_count: 3
  connection_probability: 0.05

dynamics:
  neuron_types:
    excitatory: true
    inhibitory: true
    sensory: true
    motor: true

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
    tau_plus: 20.0
    tau_minus: 20.0
  hebbian:
    enable: true
    learning_rate: 0.0005
  structural:
    enable: true
    synaptogenesis_rate: 0.001
    pruning_rate: 0.0001

neuromod:
  dopamine:
    scale: 1.0
  curiosity:
    enable: true
  novelty:
    enable: true
  prediction_error:
    scale: 1.0

development:
  initial_plasticity: 1.0
  critical_period_duration: 1000.0
  maturation_rate: 0.01
  synapse_maturation_rate: 0.1

memory:
  working_capacity: 200
  episodic_capacity: 2000
  associative_capacity: 1000
  replay_frequency: 100
  consolidation_threshold: 1000

prediction:
  horizon: 5
  confidence_decay: 0.95
  error_threshold: 0.1

environment:
  name: GridWorld
  width: 30
  height: 30
  vision_width: 10
  vision_height: 10
  max_energy: 200.0

logging:
  level: INFO
  to_file: true
  filename: nlm_simulation.log
  max_size: 10485760
  backup_count: 5
```

### INI Example
```ini
[brain]
neuron_count = 5000
region_count = 3
connection_probability = 0.05

[dynamics.neuron_types]
excitatory = true
inhibitory = true
sensory = true
motor = true

[plasticity.stdp]
enable = true
learning_rate = 0.001
tau_plus = 20.0
tau_minus = 20.0

[plasticity.hebbian]
enable = true
learning_rate = 0.0005

[plasticity.structural]
enable = true
synaptogenesis_rate = 0.001
pruning_rate = 0.0001

[neuromod.dopamine]
scale = 1.0

[neuromod.curiosity]
enable = true

[neuromod.novelty]
enable = true

[neuromod.prediction_error]
scale = 1.0

[development]
initial_plasticity = 1.0
critical_period_duration = 1000.0
maturation_rate = 0.01
synapse_maturation_rate = 0.1

[memory]
working_capacity = 200
episodic_capacity = 2000
associative_capacity = 1000
replay_frequency = 100
consolidation_threshold = 1000

[prediction]
horizon = 5
confidence_decay = 0.95
error_threshold = 0.1

[environment]
name = GridWorld
width = 30
height = 30
vision_width = 10
vision_height = 10
max_energy = 200.0

[logging]
level = INFO
to_file = true
filename = nlm_simulation.log
max_size = 10485760
backup_count = 5
```

## Preset Configurations

### Quick Start Configuration
```json
{
    "brain": { "neuron_count": 100 },
    "environment": { "width": 10, "height": 10 },
    "logging": { "level": "WARNING" }
}
```

### Research Configuration
```json
{
    "brain": { "neuron_count": 10000 },
    "plasticity": { "stdp": { "learning_rate": 0.0001 } },
    "development": { "critical_period_duration": 5000 },
    "logging": { "level": "INFO" }
}
```

### High Performance Configuration
```json
{
    "brain": { "neuron_count": 50000 },
    "plasticity": { "stdp": { "enable": false } },
    "development": { "critical_period_duration": 0 },
    "logging": { "level": "ERROR" },
    "environment": { "max_energy": 1000.0 }
}
```

## Configuration Validation

### Required Fields
All configurations must include:
- `brain.neuron_count`: Minimum 10 neurons
- `environment.name`: Environment type specification

### Type Validation
- Numeric fields must be positive
- Boolean fields must be true/false
- String fields must be non-empty

### Range Validation
- `brain.neuron_count`: 10-100000
- `plasticity.stdp.learning_rate`: 0.0-1.0
- `brain.tau_mem`: 1.0-1000.0
- `brain.tau_ref`: 0.1-100.0

## Configuration Files Location

### Default Configuration Paths
- `./config.json` - JSON configuration (highest priority)
- `./config.yaml` - YAML configuration (fallback)
- `./config.cfg` - INI configuration (fallback)
- `./default.json` - Package default JSON
- `./default.cfg` - Package default INI

### Command Line Override
```bash
./nlm --neuron_count 2000 --learning_rate 0.01
```

## Configuration Files Examples Directory

The project includes an `examples/` directory with working configurations:
- `examples/simple_brain.json`
- `examples/large_scale.json`
- `examples/development_stages.json`
- `examples/memory_focused.json`

## Configuration Migration

### From NLM v0.1 to v0.2
Changes include:
- Renamed `plasticity_learning_rate` to `plasticity.stdp.learning_rate`
- Added `development` section
- New `memory` section

Migration script available: `scripts/migrate_config.py`

## Advanced Configuration

### Dynamic Configuration
```json
{
    "brain": { "neuron_count": 1000 },
    "dynamic_settings": {
        "plasticity_learning_rate": "auto",
        "development_cooling_rate": "adaptive"
    }
}
```

### Configuration Templates
Templates allow creating standardized configurations for different research purposes:
- `templates/research.json`
- `templates/production.json`
- `templates/debug.json`

## Configuration Troubleshooting

### Common Issues

**Issue**: `brain.neuron_count` must be a positive integer
**Solution**: Use `neuron_count: 100` instead of `neuron_count: "100"`

**Issue**: Unknown configuration parameter
**Solution**: Check configuration schema documentation for valid parameters

**Issue**: Invalid file format
**Solution**: Ensure file extension matches format (json/.json, yaml/.yaml, cfg/.cfg)

### Validation Tools
Use `scripts/validate_config.py` to validate configuration files:
```bash
python scripts/validate_config.py config.json
```

## Configuration Schema Reference

A complete JSON schema is available in `schemas/config-schema.json` for IDE validation and automated checking.

## Configuration Versioning

Configuration files are versioned using semantic versioning. Always include a `version` field:
```json
{
    "version": "1.2.0",
    "brain": { ... }
}
```

Version-specific handling is available in `src/core/Configuration.cpp`.

## Related Documentation

- See [CLI-Reference.md](CLI-Reference.md) for command-line configuration options
- See [API-Guide.md](API-Guide.md) for programmatic configuration access
- See [Development-Guide.md](Development-Guide.md) for adding new configuration parameters