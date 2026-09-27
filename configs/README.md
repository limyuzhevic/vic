# NLM Configuration Templates

This directory contains ready-to-use configuration files for NLM experiments.

## Contents

### default.json
Complete configuration for a standard NLM brain simulation

### quickstart.json
Minimal configuration for beginners

### research.json
Configuration optimized for research experiments

### production.json
Configuration optimized for production use

### custom_template.json
Template for creating custom configurations

## Usage

### Load a configuration
```bash
./nlm --config default.json
```

### Python example
```python
import pynlm
import json

# Load configuration from JSON
with open('configs/default.json', 'r') as f:
    config_data = json.load(f)

config = pynlm.createDefaultConfig()
config.loadFromFile('configs/default.json')

brain = pynlm.createBrain(config)
brain.initialize()
```

### Create your own configuration
Use `custom_template.json` as a starting point and modify the values as needed.

## Configuration Parameters

### Brain Configuration
- `brain.neuron_count`: Number of neurons (int)
- `brain.region_count`: Number of brain regions (int)
- `brain.connection_probability`: Probability of synapse formation (float)

### Neuron Configuration
- `neuron.v_thresh`: Threshold potential (mV)
- `neuron.v_rest`: Resting potential (mV)
- `neuron.v_reset`: Reset potential (mV)
- `neuron.tau_mem`: Membrane time constant (ms)
- `neuron.tau_ref`: Refractory period (ms)

### Plasticity Configuration
- `plasticity.stdp.enable`: Enable STDP (bool)
- `plasticity.stdp.learning_rate`: STDP learning rate (float)
- `plasticity.hebbian.enable`: Enable Hebbian learning (bool)
- `plasticity.structural.enable`: Enable structural plasticity (bool)

### Neuromodulation Configuration
- `neuromod.dopamine.scale`: Dopamine modulation scale (float)
- `neuromod.curiosity.enable`: Enable curiosity (bool)
- `neuromod.novelty.enable`: Enable novelty detection (bool)

### Development Configuration
- `development.enable`: Enable developmental stages (bool)
- `development.critical_period_start`: Start of critical period (ms)
- `development.critical_period_end`: End of critical period (ms)

### Memory Configuration
- `memory.working_capacity`: Working memory capacity (int)
- `memory.episodic_capacity`: Episodic memory capacity (int)
- `memory.associative_strength`: Initial associative strength (float)

### World Configuration
- `world.width`: World width (int)
- `world.height`: World height (int)
- `world.vision_width`: Vision width (int)
- `world.vision_height`: Vision height (int)

## Sample Configuration Files

### default.json
```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1,
        "initial_weight_mean": 0.5,
        "initial_weight_std": 0.1
    },
    "neuron": {
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
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true,
            "exploration_rate": 0.1
        },
        "novelty": {
            "enable": true,
            "threshold": 0.5
        }
    },
    "development": {
        "enable": true,
        "critical_period_start": 100.0,
        "critical_period_end": 1000.0,
        "plasticity_decay_rate": 0.01
    },
    "memory": {
        "working": {
            "capacity": 100,
            "decay_rate": 0.01
        },
        "episodic": {
            "capacity": 1000,
            "replay_frequency": 0.1
        },
        "associative": {
            "strength": 0.5,
            "decay_rate": 0.001
        }
    },
    "world": {
        "width": 20,
        "height": 20,
        "vision_width": 8,
        "vision_height": 8,
        "max_energy": 100.0,
        "energy_decay_rate": 0.01
    }
}
```

### quickstart.json
```json
{
    "brain": {
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.05
    },
    "neuron": {
        "v_thresh": -50.0,
        "v_rest": -70.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        }
    },
    "world": {
        "width": 10,
        "height": 10,
        "vision_width": 8,
        "vision_height": 8
    }
}
```

### research.json
```json
{
    "brain": {
        "neuron_count": 5000,
        "region_count": 3,
        "connection_probability": 0.15
    },
    "neuron": {
        "v_thresh": -55.0,
        "v_rest": -65.0,
        "tau_mem": 15.0,
        "tau_ref": 3.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.002,
            "tau_plus": 15.0,
            "tau_minus": 15.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.02
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.0002,
            "pruning_rate": 0.00002
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 2.0
        },
        "curiosity": {
            "enable": true,
            "exploration_rate": 0.2
        },
        "novelty": {
            "enable": true,
            "threshold": 0.3
        }
    },
    "development": {
        "enable": true,
        "critical_period_start": 50.0,
        "critical_period_end": 500.0,
        "plasticity_decay_rate": 0.005
    },
    "world": {
        "width": 50,
        "height": 50,
        "vision_width": 16,
        "vision_height": 16
    }
}
```

### custom_template.json
```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1,
        "initial_weight_mean": 0.5,
        "initial_weight_std": 0.1
    },
    "neuron": {
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
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true,
            "exploration_rate": 0.1
        },
        "novelty": {
            "enable": true,
            "threshold": 0.5
        }
    },
    "development": {
        "enable": true,
        "critical_period_start": 100.0,
        "critical_period_end": 1000.0,
        "plasticity_decay_rate": 0.01
    },
    "memory": {
        "working": {
            "capacity": 100,
            "decay_rate": 0.01
        },
        "episodic": {
            "capacity": 1000,
            "replay_frequency": 0.1
        },
        "associative": {
            "strength": 0.5,
            "decay_rate": 0.001
        }
    },
    "world": {
        "width": 20,
        "height": 20,
        "vision_width": 8,
        "vision_height": 8,
        "max_energy": 100.0,
        "energy_decay_rate": 0.01
    }
}
```

## Configuration File Format

### Simple Key=Value Format
For backward compatibility, you can use the simple key=value format:

```
# Configuration file in simple key=value format
brain.neuron_count = 1000
brain.region_count = 1
brain.connection_probability = 0.1
neuron.v_thresh = -50.0
neuron.v_rest = -70.0
neuron.v_reset = -75.0
neuron.tau_mem = 20.0
neuron.tau_ref = 2.0
```

### JSON Format
The recommended format is JSON, which supports:
- Nested objects for organization
- Comments (with // or #)
- Array values
- Proper data types

### YAML Format
You can also use YAML format:

```yaml
brain:
  neuron_count: 1000
  region_count: 1
  connection_probability: 0.1
neuron:
  v_thresh: -50.0
  v_rest: -70.0
  v_reset: -75.0
  tau_mem: 20.0
  tau_ref: 2.0
```

## Command Line Usage

### Load configuration from file
```bash
# Using JSON
./nlm --config configs/default.json

# Using simple format
./nlm --config configs/simple.cfg

# Using command line overrides
./nlm --config configs/default.json --brain.neuron_count=2000
```

### Python Usage
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Load from JSON file
config.loadFromFile('configs/default.json')

# Load from simple file (backward compatibility)
config.loadFromFile('configs/simple.cfg')

# Override command line arguments
config.loadFromArgs(['--config', 'configs/default.json', '--brain.neuron_count=2000'])

# Save configuration
config.saveToFile('my_config.json')
```

## Configuration Parameters Reference

### Brain Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| brain.neuron_count | int | 1000 | Total number of neurons |
| brain.region_count | int | 1 | Number of brain regions |
| brain.connection_probability | float | 0.1 | Probability of connection between neurons |
| brain.initial_weight_mean | float | 0.5 | Mean initial synaptic weight |
| brain.initial_weight_std | float | 0.1 | Standard deviation of initial weights |

### Neuron Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| neuron.v_thresh | float | -50.0 | Threshold potential (mV) |
| neuron.v_rest | float | -70.0 | Resting potential (mV) |
| neuron.v_reset | float | -75.0 | Reset potential after spike (mV) |
| neuron.tau_mem | float | 20.0 | Membrane time constant (ms) |
| neuron.tau_ref | float | 2.0 | Refractory period (ms) |

### Plasticity Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| plasticity.stdp.enable | bool | true | Enable STDP |
| plasticity.stdp.learning_rate | float | 0.001 | STDP learning rate |
| plasticity.stdp.tau_plus | float | 20.0 | STDP time constant (ms) |
| plasticity.stdp.tau_minus | float | 20.0 | STDP time constant (ms) |
| plasticity.hebbian.enable | bool | true | Enable Hebbian learning |
| plasticity.hebbian.learning_rate | float | 0.01 | Hebbian learning rate |
| plasticity.structural.enable | bool | true | Enable structural plasticity |
| plasticity.structural.synaptogenesis_rate | float | 0.0001 | Rate of new synapse formation |
| plasticity.structural.pruning_rate | float | 0.00001 | Rate of synapse removal |

### Neuromodulation Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| neuromod.dopamine.scale | float | 1.0 | Dopamine modulation scale |
| neuromod.curiosity.enable | bool | true | Enable curiosity-driven exploration |
| neuromod.curiosity.exploration_rate | float | 0.1 | Rate of curiosity-driven exploration |
| neuromod.novelty.enable | bool | true | Enable novelty detection |
| neuromod.novelty.threshold | float | 0.5 | Novelty detection threshold |

### Development Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| development.enable | bool | true | Enable developmental stages |
| development.critical_period_start | float | 100.0 | Start of critical period (ms) |
| development.critical_period_end | float | 1000.0 | End of critical period (ms) |
| development.plasticity_decay_rate | float | 0.01 | Rate of plasticity decay over time |

### Memory Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| memory.working.capacity | int | 100 | Working memory capacity |
| memory.working.decay_rate | float | 0.01 | Rate of working memory decay |
| memory.episodic.capacity | int | 1000 | Episodic memory capacity |
| memory.episodic.replay_frequency | float | 0.1 | Frequency of episodic replay |
| memory.associative.strength | float | 0.5 | Initial associative strength |
| memory.associative.decay_rate | float | 0.001 | Rate of associative memory decay |

### World Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| world.width | int | 20 | World width |
| world.height | int | 20 | World height |
| world.vision_width | int | 8 | Vision width |
| world.vision_height | int | 8 | Vision height |
| world.max_energy | float | 100.0 | Maximum energy |
| world.energy_decay_rate | float | 0.01 | Rate of energy decay |

## Configuration Validation

The configuration loader includes validation for:
- Positive values for neuron counts
- Valid probability ranges (0.0 - 1.0)
- Reasonable time constants
- Compatible memory capacities

Invalid values will result in warnings and fallback to default values.

## Advanced Configuration Options

For advanced users, you can also configure:
- Regional connectivity patterns
- Specific neuron population types
- Custom plasticity rules
- Specialized neuromodulation patterns

These require deeper knowledge of NLM's internal architecture.

## Configuration Persistence

Use the `saveToFile()` method to persist configurations:
```python
config = pynlm.createDefaultConfig()
# ... modify configuration ...
config.saveToFile('my_config.json')
```

The saved file will be in JSON format and can be reused with the `--config` option.