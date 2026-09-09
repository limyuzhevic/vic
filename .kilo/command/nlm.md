# NLM Command Line Interface
# Improved command system for NLM (熙然)

## Overview
This document describes the command-line interface for NLM (熙然), an experimental artificial developmental brain project.

## Usage

### Basic Commands

`nlm run [options]` - Run a neural simulation
`nlm experiment [name] [options]` - Run a specific experiment
`nlm config [options]` - Manage configuration
`nlm status` - Show system status
`nlm help [command]` - Show help information
`nlm version` - Show version information

### Examples

Run a simple simulation:
```bash
nlm run --neurons 500 --steps 1000
```

Run with specific configuration:
```bash
nlm config my_config.cfg run --neurons 1000
```

Run experiment:
```bash
nlm experiment phase2_demo --quick
```

### Options

**Common Options:**
- `--neurons COUNT` - Set number of neurons (default: 500)
- `--steps COUNT` - Set number of simulation steps (default: 1000)
- `--config FILE` - Load configuration from file
- `--output FILE` - Save results to file
- `--verbose` - Enable verbose output
- `--headless` - Run without GUI
- `--debug` - Enable debug mode

**Experiment Options:**
- `--integration` - Run integration tests
- `--memory` - Test memory systems
- `--neuromod` - Test neuromodulation
- `--plasticity` - Test plasticity rules
- `--development` - Test developmental stages

## Command Reference

### nlm run
Run a neural simulation with configurable parameters.

**Arguments:**
- `--neurons COUNT` - Number of neurons (default: 500)
- `--steps COUNT` - Number of simulation steps (default: 1000)
- `--timestep FLOAT` - Simulation timestep (default: 0.001)

**Example:**
```bash
nlm run --neurons 1000 --steps 2000 --timestep 0.001
```

### nlm experiment
Run a specific NLM experiment.

**Available Experiments:**
- `phase2_demo` - Phase 2 neural computation demo
- `phase3_demo` - Phase 3 world interaction demo
- `phase4_demo` - Phase 4 cognition demo
- `phase6_integration` - Phase 6 full integration test
- `plasticity` - Plasticity mechanisms test
- `memory` - Memory systems test
- `neuromodulation` - Neuromodulation test

**Example:**
```bash
nlm experiment phase6_integration --neurons 500 --steps 2000
```

### nlm config
Manage configuration files.

**Subcommands:**
- `create [name]` - Create new configuration
- `load [file]` - Load configuration from file
- `save [file]` - Save configuration to file
- `list` - List available configurations
- `set KEY VALUE` - Set configuration value
- `get KEY` - Get configuration value

**Example:**
```bash
nlm config create my_config
nlm config set brain.neuron_count 1000
nlm config save my_config.cfg
```

### nlm status
Show system status and performance metrics.

**Example:**
```bash
nlm status
```

### nlm help
Show help information.

**Example:**
```bash
nlm help run
```

### nlm version
Show version information.

**Example:**
```bash
nlm version
```

## Configuration File Format

NLM configuration files use JSON format:

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "initial_weight_mean": 0.5,
        "initial_weight_std": 0.1
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
            "synaptogenesis_rate": 0.001,
            "pruning_rate": 0.0001
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
            "detection_threshold": 0.1
        }
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 10000,
        "random_seed": 42
    },
    "world": {
        "width": 10,
        "height": 10,
        "vision_width": 8,
        "vision_height": 8
    }
}
```

## Integration with CMake

For projects using CMake, NLM provides integration:

```cmake
find_package(NLM REQUIRED)

# Create executable
nlm_add_executable(demo src/demo.cpp)
nlm_target_link_libraries(demo nlm_core)
```

## Python API

NLM also provides a Python API via pybind11:

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Advanced Features

### Command Aliases

- `nlm simulate` - Alias for `nlm run`
- `nlm test` - Alias for running tests
- `nlm demo` - Run default demo

### Batch Processing

Run multiple simulations with different parameters:

```bash
for neurons in 100 500 1000; do
    nlm run --neurons $neurons --steps 500
    nlm save results_${neurons}.bin
    nlm config load default.cfg
    echo "Completed with ${neurons} neurons"
done
```

### Automation

Create shell scripts for common workflows:

```bash
#!/bin/bash
# run_experiment.sh - Run NLM experiment with monitoring

echo "Starting NLM experiment..."
nlm experiment phase6_integration --neurons 500 --steps 2000 --integration

if [ $? -eq 0 ]; then
    echo "Experiment completed successfully!"
    nlm status > experiment.log
    echo "Results saved to experiment.log"
else
    echo "Experiment failed!"
    exit 1
fi
```

## Configuration Management

### Environment Variables

- `NLM_CONFIG` - Default configuration file path
- `NLM_LOG_LEVEL` - Log level (DEBUG, INFO, WARN, ERROR)
- `NLM_OUTPUT_DIR` - Output directory for results
- `NLM_MAX_MEMORY` - Maximum memory usage (MB)

### File Locations

Configuration files are stored in:
- `~/.config/nlm/` - User configuration directory
- `./configs/` - Project configurations
- `./build/configs/` - Build-time configurations

### Backup and Restore

```bash
# Backup current configuration
nlm config save backup_$(date +%Y%m%d_%H%M%S).cfg

# Restore from backup
nlm config load backup_20240115_143022.cfg
```

## Troubleshooting

### Common Issues

**Error: Configuration file not found**
```bash
nlm config --config myconfig.cfg run
```

**Error: Invalid parameters**
```bash
nlm run --neurons 0 --steps -100
```
Correct usage:
```bash
nlm run --neurons 1000 --steps 1000
```

**Error: Permission denied**
Check file permissions and ensure you have write access to output directories.

### Debugging

Enable debug mode:
```bash
nlm run --debug --verbose --output debug.log
```

Check logs:
```bash
tail -f debug.log
```

### Support

For help with NLM commands:
1. Check the official documentation
2. Visit the NLM GitHub repository
3. Check the examples in the repository
4. Report issues with command-line interface

## License

MIT License. See LICENSE file for details.