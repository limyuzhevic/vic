# Configuration Documentation - NLM/熙然

## Overview

This document provides comprehensive documentation for NLM configuration options, including both **command-line interface (CLI) options** and **configuration file formats**. NLM uses a flexible configuration system that supports key-value pairs, JSON files, and command-line arguments.

The configuration system allows users to customize:
- Brain parameters (neurons, synapses, plasticity)
- Neuromodulation settings
- Development stages
- World simulation parameters
- Performance settings
- Logging and debugging options

## Configuration System Architecture

NLM supports multiple configuration sources with this priority order:

1. **Command-line arguments** (highest priority)
2. **Configuration file** (`.json` format)
3. **Default values** (lowest priority)

All configuration is stored as **key-value pairs** with hierarchical keys using dot notation:

```
brain.neuronCount = 1000
neuromod.dopamine.scale = 1.0
world.width = 20
```

## Configuration File Format

### Basic Structure

NLM configuration files use **JSON format** with hierarchical keys:

```json
{
    "brain": {
        "neuronCount": 1000,
        "synapseDensity": 0.1,
        "plasticity": {
            "stdp": {
                "enabled": true,
                "learningRate": 0.001
            },
            "hebbian": {
                "enabled": true
            }
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enabled": true
        }
    },
    "world": {
        "width": 20,
        "height": 20,
        "visionWidth": 8,
        "visionHeight": 8
    },
    "simulation": {
        "timeStep": 0.1,
        "maxSteps": 1000
    }
}
```

### Configuration File Examples

#### minimal.json (Basic Configuration)

```json
{
    "brain": {
        "neuronCount": 100,
        "regionCount": 1
    },
    "simulation": {
        "timeStep": 0.1
    }
}
```

#### standard.json (Recommended for Research)

```json
{
    "brain": {
        "neuronCount": 1000,
        "regionCount": 5,
        "plasticity": {
            "stdp": {
                "enabled": true,
                "learningRate": 0.001
            },
            "hebbian": {
                "enabled": true
            },
            "structural": {
                "enabled": true
            }
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enabled": true
        },
        "novelty": {
            "enabled": true
        }
    },
    "world": {
        "width": 20,
        "height": 20,
        "visionWidth": 8,
        "visionHeight": 8,
        "maxEnergy": 100.0
    },
    "development": {
        "enabled": true,
        "stageDuration": 1000
    },
    "simulation": {
        "timeStep": 0.1,
        "maxSteps": 1000
    }
}
```

#### research.json (Full Research Configuration)

```json
{
    "brain": {
        "neuronCount": 5000,
        "regionCount": 10,
        "plasticity": {
            "stdp": {
                "enabled": true,
                "learningRate": 0.001,
                "tauPlus": 20.0,
                "tauMinus": 20.0
            },
            "hebbian": {
                "enabled": true,
                "learningRate": 0.0005
            },
            "structural": {
                "enabled": true,
                "synaptogenesisInterval": 100,
                "pruningInterval": 100
            }
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0,
            "tonicLevel": 0.1
        },
        "curiosity": {
            "enabled": true,
            "noveltyWeight": 0.7,
            "predictionErrorWeight": 0.3
        },
        "novelty": {
            "enabled": true,
            "decayRate": 0.95
        },
        "predictionError": {
            "enabled": true,
            "learningRate": 0.01
        }
    },
    "development": {
        "enabled": true,
        "stageDurations": {
            "initial": 100,
            "criticalPeriod": 500,
            "maturation": 1000,
            "adult": -1
        },
        "plasticityModifiers": {
            "initial": 1.0,
            "criticalPeriod": 1.5,
            "maturation": 1.2,
            "adult": 1.0
        }
    },
    "world": {
        "width": 50,
        "height": 50,
        "visionWidth": 12,
        "visionHeight": 12,
        "maxEnergy": 200.0,
        "energyDecayRate": 0.01,
        "resourceValue": 10.0,
        "hazardSeverity": 5.0
    },
    "performance": {
        "memoryPoolSize": "large",
        "eventQueueSize": "large",
        "parallelThreads": 4,
        "enableSIMD": true
    },
    "simulation": {
        "timeStep": 0.05,
        "maxSteps": 5000,
        "enableCheckpointing": true,
        "checkpointInterval": 1000
    },
    "logging": {
        "level": "info",
        "enablePerformanceMetrics": true,
        "enableMemoryTracking": true
    }
}
```

## Command-Line Interface Options

### Basic Usage

```bash
# Run with default configuration
./nlm

# Run with specific configuration file
./nlm --config my_config.json

# Run with command line overrides
./nlm --brain.neuronCount=2000 --world.width=30

# Run with verbose output
./nlm --verbose

# Run with custom simulation time
./nlm --simulation.timeStep=0.05 --simulation.maxSteps=2000
```

### CLI Argument Reference

#### Core Options

```bash
--help, -h              Show help message and exit
--version               Show version information and exit
--config FILE          Load configuration from file
--output FILE          Save output to file
--verbose, -v          Enable verbose output
--quiet, -q            Suppress informational messages
--log-level LEVEL      Set logging level (debug, info, warning, error)
--log-file FILE        Log output to file
```

#### Brain Configuration Options

```bash
--brain.neuronCount N          Total number of neurons
--brain.regionCount N          Number of brain regions
--brain.synapseDensity F       Synapse connectivity density (0-1)
--brain.connectionProbability F Probability of synapse between neurons (0-1)
--brain.initialWeightMean F    Mean initial synaptic weight
--brain.initialWeightStd F     Standard deviation of initial weights
--brain.v_thresh F            Neuron threshold potential (mV)
--brain.v_rest F              Resting potential (mV)
--brain.v_reset F             Reset potential after spike (mV)
--brain.tau_mem F             Membrane time constant (ms)
--brain.tau_ref F             Refractory period (ms)

# Plasticity options
--brain.plasticity.stdp.enabled           Enable STDP
--brain.plasticity.stdp.learningRate F    STDP learning rate
--brain.plasticity.stdp.tauPlus F        STDP time constant (ms)
--brain.plasticity.stdp.tauMinus F       STDP time constant (ms)
--brain.plasticity.hebbian.enabled        Enable Hebbian learning
--brain.plasticity.hebbian.learningRate F Hebbian learning rate
--brain.plasticity.structural.enabled    Enable structural plasticity
```

#### Neuromodulation Options

```bash
--neuromod.dopamine.scale F             Dopamine modulation scale
--neuromod.curiosity.enabled             Enable curiosity-driven exploration
--neuromod.curiosity.noveltyWeight F     Weight of novelty in curiosity
--neuromod.curiosity.predictionErrorWeight F Weight of prediction error in curiosity
--neuromod.novelty.enabled               Enable novelty detection
--neuromod.novelty.decayRate F           Novelty decay rate (0-1)
--neuromod.predictionError.enabled       Enable prediction error computation
--neuromod.predictionError.learningRate F Prediction error learning rate
```

#### Development Options

```bash
--development.enabled                Enable developmental plasticity
--development.stageDuration F        Duration of each developmental stage
--development.initialPlasticity F     Plasticity modifier for Initial stage
--development.criticalPeriodPlasticity F Plasticity modifier for CriticalPeriod
--development.maturationPlasticity F  Plasticity modifier for Maturation
--development.adultPlasticity F       Plasticity modifier for Adult
```

#### World Configuration Options

```bash
--world.width F                      World width in units
--world.height F                     World height in units
--world.visionWidth F                 Agent vision width
--world.visionHeight F                Agent vision height
--world.maxEnergy F                   Maximum agent energy
--world.energyDecayRate F             Energy decay rate per timestep
--world.resourceValue F               Value of resource objects
--world.hazardSeverity F              Severity of hazard objects
--world.randomSeed N                  Random seed for reproducible simulations
```

#### Simulation Options

```bash
--simulation.timeStep F              Time step for simulation (seconds)
--simulation.maxSteps N               Maximum number of simulation steps
--simulation.enableCheckpointing      Enable checkpoint saving
--simulation.checkpointInterval N     Checkpoint save interval (steps)
--simulation.enablePhysics            Enable physics simulation
--simulation.timeScale F              Time scale multiplier (1.0 = real time)
--simulation.pauseOnCompletion        Pause after simulation completion
```

#### Performance Options

```bash
--performance.memoryPoolSize SIZE      Memory pool size (small, medium, large)
--performance.eventQueueSize SIZE      Event queue size (small, medium, large)
--performance.parallelThreads N        Number of parallel threads
--performance.enableSIMD              Enable SIMD vectorization
--performance.enableMultithreading    Enable multithreading
--performance.profileHotPaths        Profile hot paths for optimization
```

#### Logging Options

```bash
--logging.level LEVEL               Logging level (debug, info, warning, error)
--logging.enableConsole             Enable console logging
--logging.enableFile                Enable file logging
--logging.file LOG_FILE             Log file path
--logging.enablePerformanceMetrics  Enable performance metrics collection
--logging.enableMemoryTracking      Enable memory usage tracking
```

### Example Command-Line Usage

#### Basic Run with Configuration File

```bash
# Load configuration and run
./nlm --config standard.json

# Override specific settings
./nlm --config standard.json --brain.neuronCount=2000 --world.width=30
```

#### Research Configuration with Performance Settings

```bash
# Create optimized research configuration
./nlm \
  --config research.json \
  --performance.memoryPoolSize large \
  --performance.eventQueueSize large \
  --performance.parallelThreads 4 \
  --performance.enableSIMD \
  --logging.level info \
  --logging.enablePerformanceMetrics
```

#### Development and Testing Configuration

```bash
# Development configuration with detailed logging
./nlm \
  --brain.neuronCount=100 \
  --brain.regionCount=2 \
  --development.enabled \
  --simulation.timeStep=0.05 \
  --simulation.maxSteps=500 \
  --world.width=10 \
  --world.height=10 \
  --logging.level debug \
  --logging.enableMemoryTracking \
  --verbose
```

#### Production Configuration

```bash
# Production-ready configuration
./nlm \
  --config standard.json \
  --performance.memoryPoolSize large \
  --performance.enableSIMD \
  --logging.level warning \
  --simulation.timeScale 2.0 \
  --simulation.enableCheckpointing
```

## Configuration Key Reference

### Brain Parameters

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `brain.neuronCount` | int | 1000 | Total number of neurons |
| `brain.regionCount` | int | 5 | Number of brain regions |
| `brain.synapseDensity` | float | 0.1 | Synapse connectivity density |
| `brain.connectionProbability` | float | 0.05 | Connection probability |
| `brain.initialWeightMean` | float | 0.5 | Initial synaptic weight mean |
| `brain.initialWeightStd` | float | 0.1 | Initial synaptic weight standard deviation |
| `brain.v_thresh` | float | -50.0 | Neuron threshold (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Parameters

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `plasticity.stdp.enabled` | bool | true | Enable spike-timing dependent plasticity |
| `plasticity.stdp.learningRate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tauPlus` | float | 20.0 | STDP time constant (positive)
| `plasticity.stdp.tauMinus` | float | 20.0 | STDP time constant (negative)
| `plasticity.hebbian.enabled` | bool | true | Enable Hebbian plasticity |
| `plasticity.hebbian.learningRate` | float | 0.0005 | Hebbian learning rate |
| `plasticity.structural.enabled` | bool | true | Enable structural plasticity |
| `plasticity.structural.synaptogenesisInterval` | int | 100 | Synapse creation interval |
| `plasticity.structural.pruningInterval` | int | 100 | Synapse removal interval |

### Neuromodulation Parameters

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.dopamine.tonicLevel` | float | 0.1 | Tonic dopamine level |
| `neuromod.curiosity.enabled` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.noveltyWeight` | float | 0.7 | Weight of novelty in curiosity |
| `neuromod.curiosity.predictionErrorWeight` | float | 0.3 | Weight of prediction error in curiosity |
| `neuromod.novelty.enabled` | bool | true | Enable novelty detection |
| `neuromod.novelty.decayRate` | float | 0.95 | Novelty decay rate |
| `neuromod.predictionError.enabled` | bool | true | Enable prediction error computation |
| `neuromod.predictionError.learningRate` | float | 0.01 | Prediction error learning rate |

### Development Parameters

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `development.enabled` | bool | true | Enable developmental stages |
| `development.stageDuration` | int | 1000 | Duration of each stage (steps) |
| `development.initialPlasticity` | float | 1.0 | Plasticity modifier for Initial stage |
| `development.criticalPeriodPlasticity` | float | 1.5 | Plasticity modifier for CriticalPeriod |
| `development.maturationPlasticity` | float | 1.2 | Plasticity modifier for Maturation |
| `development.adultPlasticity` | float | 1.0 | Plasticity modifier for Adult |

### World Parameters

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `world.width` | float | 20.0 | World width |
| `world.height` | float | 20.0 | World height |
| `world.visionWidth` | int | 8 | Agent vision width |
| `world.visionHeight` | int | 8 | Agent vision height |
| `world.maxEnergy` | float | 100.0 | Maximum agent energy |
| `world.energyDecayRate` | float | 0.01 | Energy decay rate per timestep |
| `world.resourceValue` | float | 10.0 | Resource object value |
| `world.hazardSeverity` | float | 5.0 | Hazard object severity |
| `world.randomSeed` | int | 0 | Random seed (0 = use system time) |

### Simulation Parameters

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `simulation.timeStep` | float | 0.1 | Simulation time step |
| `simulation.maxSteps` | int | 1000 | Maximum simulation steps |
| `simulation.enableCheckpointing` | bool | false | Enable checkpoint saving |
| `simulation.checkpointInterval` | int | 1000 | Checkpoint save interval |
| `simulation.enablePhysics` | bool | true | Enable physics simulation |
| `simulation.timeScale` | float | 1.0 | Time scale multiplier |
| `simulation.pauseOnCompletion` | bool | false | Pause after completion |

### Performance Parameters

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `performance.memoryPoolSize` | string | "medium" | Memory pool size (small, medium, large) |
| `performance.eventQueueSize` | string | "medium" | Event queue size (small, medium, large) |
| `performance.parallelThreads` | int | 0 | Number of parallel threads (0 = auto) |
| `performance.enableSIMD` | bool | true | Enable SIMD vectorization |
| `performance.enableMultithreading` | bool | false | Enable multithreading |
| `performance.profileHotPaths` | bool | false | Profile hot paths |

### Logging Parameters

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `logging.level` | string | "info" | Logging level (debug, info, warning, error) |
| `logging.enableConsole` | bool | true | Enable console logging |
| `logging.enableFile` | bool | false | Enable file logging |
| `logging.file` | string | "nlm.log" | Log file path |
| `logging.enablePerformanceMetrics` | bool | false | Enable performance metrics |
| `logging.enableMemoryTracking` | bool | false | Enable memory tracking |

## Configuration Validation

### Required Parameters

The following parameters are required for basic operation:

- `brain.neuronCount` (min: 10)
- `brain.regionCount` (min: 1)
- `world.width` (min: 1)
- `world.height` (min: 1)
- `simulation.timeStep` (min: 0.01)
- `simulation.maxSteps` (min: 1)

### Type Validation

Configuration values are validated for correct types:

```bash
# These would cause validation errors
--brain.neuronCount "abc"      # String instead of int
--brain.synapseDensity 2.0      # Out of range (0-1)
--world.width -5              # Negative value
```

### Range Validation

Numerical parameters have range limits:

- Neuron count: 10 to 100,000
- Synapse density: 0.0 to 1.0
- Connection probability: 0.0 to 1.0
- Time step: 0.01 to 1.0 seconds
- World dimensions: 1 to 1000 units

### Consistency Validation

Configuration parameters must be internally consistent:

- Vision dimensions must fit within world dimensions
- Number of regions cannot exceed neuron count
- Plasticity intervals must be positive integers

## Configuration Best Practices

### For Development

1. **Start Minimal**: Begin with minimal configuration to test basic functionality
2. **Incremental Addition**: Add complexity gradually to isolate issues
3. **Verbose Logging**: Use `--verbose` and `--logging.level debug` for development
4. **Configuration Comments**: Add comments to configuration files for clarity
5. **Version Control**: Track configuration changes in version control

### For Research

1. **Document Configuration**: Maintain detailed documentation of experimental configurations
2. **Reproducibility**: Save configuration files for reproducibility
3. **Parameter Sensitivity**: Test sensitivity to key parameters
4. **Performance Profiling**: Use `--performance.profileHotPaths` for optimization
5. **Validation Scripts**: Create validation scripts to check configuration consistency

### For Production

1. **Optimized Configuration**: Use high-performance settings
2. **Error Handling**: Implement robust error handling for configuration failures
3. **Monitoring**: Enable performance metrics and logging
4. **Backup**: Regularly backup important configurations
5. **Testing**: Thoroughly test configurations before deployment

### Configuration File Organization

```
/configs/
├── base/                    # Base configurations
│   ├── minimal.json
│   ├── standard.json
│   └── research.json
├── experiments/            # Experiment-specific configurations
│   ├── baseline.json
│   ├── mutation_1.json
│   └── mutation_2.json
├── production/             # Production configurations
│   ├── development.json
│   └── staging.json
├── templates/              # Configuration templates
│   └── template.json
└── backup/                 # Configuration backups
    ├── config_2024-01-01.json
    └── config_2024-02-01.json
```

## Configuration Tips and Tricks

### Combining Configuration Sources

```bash
# Command line overrides configuration file
./nlm --config config.json --brain.neuronCount=2000 --world.width=30

# Multiple configuration files (later files override earlier ones)
./nlm --config base.json --config experiment.json
```

### Creating Configuration Templates

```json
{
    "template": true,
    "brain": {
        "neuronCount": "{{NEURON_COUNT}}",
        "regionCount": "{{REGION_COUNT}}"
    },
    "world": {
        "width": "{{WORLD_WIDTH}}",
        "height": "{{WORLD_HEIGHT}}"
    }
}
```

### Environment-Specific Configurations

```bash
# Development environment
export NLM_CONFIG=dev_config.json

# Production environment
export NLM_CONFIG=prod_config.json

# Use environment variable in scripts
if [ -n "$NLM_CONFIG" ]; then
    ./nlm --config "$NLM_CONFIG"
else
    ./nlm
fi
```

## Configuration Migration Guide

### From Version 1.x to 2.x

1. **Configuration File Format**: JSON format changed from key-value to hierarchical
2. **Parameter Names**: Some parameter names have been standardized
3. **Default Values**: Some defaults have changed based on research findings

```bash
# Version 1.x style
--brain.neurons 1000
--world.vision_width 8

# Version 2.x style
--brain.neuronCount 1000
--world.visionWidth 8
```

## Conclusion

The NLM configuration system provides flexible and comprehensive configuration management for all aspects of neural simulation. Whether you're running simple tests, conducting research experiments, or deploying production systems, the configuration system can be adapted to meet your needs.

Key takeaways:

1. **Flexible Sources**: Support for configuration files, command-line arguments, and defaults
2. **Hierarchical Structure**: Easy-to-navigate key-value system with dot notation
3. **Type Safety**: Automatic type validation and conversion
4. **Consistency Checks**: Internal validation for parameter consistency
5. **Extensive Documentation**: Comprehensive parameter reference and examples

The configuration system is designed to support NLM's modular architecture while providing the flexibility needed for research, development, and production environments.