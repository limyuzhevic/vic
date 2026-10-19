# This file is maintained by Kilo - please do not edit manually

# Configuration for enhanced NLM (Neural Learning Machine)

This document describes the configuration options available for NLM after the recent improvements.

## Overview

NLM has been enhanced with several new configuration features to make it more powerful and easier to use for both beginners and advanced users.

## Configuration Formats

### JSON Configuration

NLM now supports JSON format configuration files. This is the recommended format for complex configurations.

**Example JSON config (config.json):**
```json
{
    "brain": {
        "neuron_count": 2000,
        "connection_probability": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "tau_mem": 20.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        },
        "hebbian": {
            "enable": true,
            "strength": 0.5
        }
    },
    "neuromodulation": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true,
            "exploration_rate": 0.3
        }
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 10000,
        "enable_checkpointing": true,
        "checkpoint_path": "/tmp/nlm_checkpoint.bin"
    },
    "debug": {
        "level": 2,
        "neurons": {
            "enable": true,
            "track_connections": true
        },
        "synapses": {
            "enable": true,
            "track_dynamics": true
        }
    }
}
```

### YAML Configuration

NLM also supports YAML configuration files for human-readable configuration.

**Example YAML config (config.yaml):**
```yaml
brain:
  neuron_count: 2000
  connection_probability: 0.1
  v_thresh: -50.0
  v_rest: -70.0
  tau_mem: 20.0

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
  hebbian:
    enable: true
    strength: 0.5

neuromodulation:
  dopamine:
    scale: 1.0
  curiosity:
    enable: true
    exploration_rate: 0.3

simulation:
  timestep: 0.001
  max_steps: 10000
  enable_checkpointing: true
  checkpoint_path: /tmp/nlm_checkpoint.bin

debug:
  level: 2
  neurons:
    enable: true
    track_connections: true
  synapses:
    enable: true
    track_dynamics: true
```

### Command Line Configuration

You can override configuration values using command-line arguments:

```bash
./nlm --config config.json --neurons 5000 --steps 20000 --enable-development --output results.json
```

Common command-line options:
- `--config <file>` - Load configuration from JSON/YAML file
- `--neurons <count>` - Set number of neurons
- `--steps <count>` - Set simulation steps
- `--timestep <time>` - Set simulation timestep
- `--random-seed <seed>` - Set random seed
- `--enable-development` - Enable developmental stages
- `--save-checkpoint <file>` - Save brain state to file
- `--load-checkpoint <file>` - Load brain state from file

## Debug Configuration

NLM now includes advanced debugging capabilities:

### Debug Levels

1. **Off (0)** - No debugging
2. **Error (1)** - Only errors
3. **Warning (2)** - Errors and warnings
4. **Info (3)** - Basic information
5. **Debug (4)** - Detailed debug information
6. **Verbose (5)** - Very detailed information
7. **Trace (6)** - Maximum detail (includes timing)

### Debug Configuration Options

```yaml
debug:
  level: 4  # Debug level
  neurons:
    enable: true
    track_connections: true
    track_firing_patterns: true
  synapses:
    enable: true
    track_dynamics: true
    track_plasticity: true
  memory:
    enable: true
    track_allocations: true
    profile_usage: true
  performance:
    enable: true
    track_timing: true
    profile_memory: true
```

### Using Debugger from Python

```python
import pynlm

# Create brain and enable debugging
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Configure debug settings
config.set("debug.level", 4)
config.set("debug.neurons.enable", True)
config.set("debug.synapses.enable", True)

# Initialize debugger (in real usage, this would be integrated with C++)
# debugger = pynlm.createDebugger(config)
# debugger.startDebugSession("research_experiment")

# Run simulation
for step in range(1000):
    brain.step(step)
    
    # Get debug information
    # debug_info = debugger.getDebugInfo()
    # active_neurons = debug_info.getActiveNeurons()
    
print(f"Average firing rate: {brain.getAverageFiringRate()}")
print(f"Network connectivity: {brain.getTotalSynapseCount()} synapses")
```

## Advanced Configuration Features

### Structural Plasticity Configuration

```json
"plasticity": {
    "structural": {
        "enable": true,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001,
        "min_weight": 0.1,
        "max_weight": 10.0,
        "max_synapses_per_neuron": 100
    }
}
```

### Neuromodulation Configuration

```json
"neuromodulation": {
    "dopamine": {
        "baseline": 0.1,
        "scale": 1.0,
        "decay_rate": 0.01
    },
    "curiosity": {
        "enable": true,
        "exploration_rate": 0.3,
        "novelty_sensitivity": 0.5
    },
    "prediction_error": {
        "learning_rate": 0.01,
        "prediction_threshold": 0.05
    }
}
```

### Memory System Configuration

```json
"memory": {
    "working_memory": {
        "capacity": 100,
        "decay_rate": 0.95,
        "trace_length": 1000
    },
    "episodic_memory": {
        "max_episodes": 1000,
        "consolidation_threshold": 0.8,
        "replay_frequency": 100
    }
}
```

## Configuration Validation

NLM includes configuration validation to catch errors before simulation:

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Validate configuration (in real usage, this would be integrated)
# validator = pynlm.createConfigValidator(config)
# issues = validator.validate()

if issues:
    print("Configuration issues found:")
    for issue in issues:
        print(f"  - {issue}")
else:
    print("Configuration is valid")
```

## Performance Configuration

NLM supports performance optimization through configuration:

```json
"performance": {
    "memory_pool": {
        "enable": true,
        "neuron_pool_size": 10000,
        "synapse_pool_size": 50000
    },
    "threading": {
        "enable": true,
        "num_threads": 4
    },
    "simd": {
        "enable": true,
        "prefer_avx512": true
    },
    "caching": {
        "enable": true,
        "cache_size": "256MB"
    }
}
```

## Configuration Templates

### Template 1: Research Simulation
```yaml
name: "Research Simulation"
brain:
  neuron_count: 10000
  connection_probability: 0.1
  region_count: 5
plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
  hebbian:
    enable: true
    strength: 0.5
  structural:
    enable: true
    synaptogenesis_rate: 0.0001
    pruning_rate: 0.00001
neuromodulation:
  curiosity:
    enable: true
    exploration_rate: 0.5
  prediction_error:
    learning_rate: 0.01
simulation:
  timestep: 0.001
  max_steps: 50000
  enable_checkpointing: true
debug:
  level: 3
  neurons:
    enable: true
  synapses:
    enable: true
```

### Template 2: Fast Testing
```json
{
    "brain": {
        "neuron_count": 500,
        "connection_probability": 0.05
    },
    "simulation": {
        "timestep": 0.01,
        "max_steps": 1000
    },
    "debug": {
        "level": 1
    }
}
```

## Configuration File Examples

See the following example configuration files in the examples/ directory:
- `examples/simple_config.json` - Basic configuration
- `examples/research_config.yaml` - Research setup
- `examples/debug_config.json` - Debug-focused configuration
- `examples/performance_config.json` - Performance optimized

## Next Steps

1. Choose your preferred configuration format (JSON or YAML)
2. Start with the basic template and customize based on your needs
3. Use the enhanced command-line interface to override specific settings
4. Enable debugging and profiling for complex experiments
5. Use the configuration validation to catch errors early

For more detailed information, refer to the NLM documentation and examples.
