# NLM CLI Tool - Command Line Interface for NLM Brain Simulation

The NLM CLI Tool provides a comprehensive command-line interface for NLM (Neural Learning Machine) brain simulation with advanced configuration support.

## Overview

The CLI tool enables researchers and developers to:
- Create and manage brain simulations
- Run experiments and demos
- Save/load brain states and configurations
- Configure brain parameters via command line
- Import/export configurations in multiple formats
- Monitor system performance

## Installation

The CLI tool is built as part of the NLM project. After building with CMake:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

The `nlm-cli` executable will be installed to the bin directory.

## Usage

### Basic Usage

```bash
nlm-cli [command] [options]
```

### Commands

#### create
Create a new brain simulation with specified parameters.

**Syntax:**
```bash
nlm-cli create --neurons <count> --regions <count> --timestep <timestep>
```

**Examples:**
```bash
# Create brain with 500 neurons in 1 region
nlm-cli create --neurons 500 --regions 1 --timestep 0.001

# Create with custom timestep
nlm-cli create --neurons 1000 --regions 2 --timestep 0.01
```

#### step
Run simulation steps.

**Syntax:**
```bash
nlm-cli step --steps <count>
```

**Examples:**
```bash
# Run 100 simulation steps
nlm-cli step --steps 100

# Run 500 steps for longer simulation
nlm-cli step --steps 500
```

#### save
Save brain state and configuration to files.

**Syntax:**
```bash
nlm-cli save --filepath <path>
```

**Examples:**
```bash
# Save to default brain state
nlm-cli save

# Save to custom location
nlm-cli save --filepath /path/to/brain_state
```

#### load
Load brain state and configuration from files.

**Syntax:**
```bash
nlm-cli load --filepath <path>
```

**Examples:**
```bash
# Load from default brain state
nlm-cli load

# Load from custom location
nlm-cli load --filepath /path/to/brain_state
```

#### reset
Reset brain to initial state.

**Syntax:**
```bash
nlm-cli reset
```

#### demo
Run a demonstration simulation showcasing NLM capabilities.

**Syntax:**
```bash
nlm-cli demo
```

#### experiment
Run an experiment with specified configuration.

**Syntax:**
```bash
nlm-cli experiment --name <experiment_name>
```

#### config
View and manage configuration.

**Syntax:**
```bash
nlm-cli config [--save --filepath <path>]
```

**Examples:**
```bash
# View current configuration
nlm-cli config

# Save current configuration
nlm-cli config --save --filepath custom_config.cfg
```

#### import
Import configuration from file.

**Syntax:**
```bash
nlm-cli import --filepath <path>
```

**Examples:**
```bash
# Import from JSON file
nlm-cli import --filepath config.json

# Import from YAML file
nlm-cli import --filepath config.yaml
```

#### export
Export configuration to file.

**Syntax:**
```bash
nlm-cli export --filepath <path>
```

**Examples:**
```bash
# Export to JSON file
nlm-cli export --filepath config.json

# Export to YAML file
nlm-cli export --filepath config.yaml
```

### Global Options

#### --config <file>
Load configuration from specified file.

**Example:**
```bash
nlm-cli --config my_config.cfg demo
```

#### --verbose
Enable verbose output showing detailed progress information.

#### --log-level <level>
Set logging level. Options: trace, debug, info, warn, error, critical.

**Example:**
```bash
nlm-cli --log-level debug demo
```

#### --help
Show help message.

## Configuration Files

The CLI tool supports multiple configuration file formats:

### Simple Config (.cfg)
Key-value pairs format (default):
```
# Simulation parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1

# Neuron parameters
neuron_membrane_resistance = 10000.0
neuron_threshold = -55.0

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
```

### JSON Configuration (.json)
Hierarchical JSON format:
```json
{
    "nlm_config": {
        "simulation": {
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "neuron_count": 500,
            "region_count": 1
        },
        "neuron": {
            "membrane_resistance": 10000.0,
            "threshold": -55.0
        }
    }
}
```

### YAML Configuration (.yaml)
Human-readable YAML format:
```yaml
nlm_config:
  simulation:
    random_seed: 42
    simulation_timestep: 0.001
    neuron_count: 500
    region_count: 1
  
  neuron:
    membrane_resistance: 10000.0
    threshold: -55.0
```

## Environment Variables

The CLI tool supports NLM_* environment variables for configuration:

- `NLM_RANDOM_SEED` - Random seed for reproducibility
- `NLM_SIMULATION_TIMESTEP` - Simulation timestep
- `NLM_NEURON_COUNT` - Number of neurons
- `NLM_REGION_COUNT` - Number of regions
- And many more...

**Example:**
```bash
export NLM_RANDOM_SEED=123
export NLM_NEURON_COUNT=1000
nlm-cli demo
```

## Examples

### Running a Complete Simulation

```bash
# Create simulation with 500 neurons
nlm-cli create --neurons 500 --regions 1 --timestep 0.001

# Run simulation steps
nlm-cli step --steps 200

# Save the state
nlm-cli save

# Reset and run another simulation
nlm-cli reset
```

### Experiment Configuration

```bash
# Run with custom configuration file
nlm-cli --config experiment_config.json demo

# Use environment variables
export NLM_NEURON_COUNT=1000
export NLM_SIMULATION_TIMESTEP=0.01
nlm-cli demo
```

### Brain State Management

```bash
# Save current simulation state
nlm-cli save --filepath my_brain_state

# Later, restore it
nlm-cli load --filepath my_brain_state

# Export configuration to JSON
nlm-cli export --filepath backup_config.json

# Import configuration back
nlm-cli import --filepath backup_config.json
```

## Configuration Validation

The CLI tool validates configuration files and parameters:

1. **Required Parameters**: Essential parameters must be provided or have valid defaults
2. **Type Checking**: Configuration values must match expected types (int, float, string, bool)
3. **Range Validation**: Values must be within acceptable ranges
4. **Cross-parameter Validation**: Dependencies between parameters are checked

## Configuration Backup and Recovery

The CLI tool supports automatic configuration backup:

- Automatic backups are created when saving brain states
- Previous configurations are preserved for recovery
- Backup files can be restored at any time

## Performance Monitoring

The CLI tool includes performance monitoring capabilities:

- Track simulation progress
- Monitor memory usage
- Log performance metrics
- Profile simulation execution

## Integration with NLM Systems

The CLI tool integrates with all NLM systems:

- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Prediction System**: Internal predictions and error signals
- **Cognition Systems**: Planning, attention, concept formation
- **Development**: Adaptive plasticity based on age/stage

## Scripting

For automation, the CLI tool supports scripting:

```bash
#!/bin/bash
# Run a complete experiment
nlm-cli create --neurons 500 --regions 1
nlm-cli step --steps 100
nlm-cli save --filepath experiment_result
nlm-cli export --filepath experiment_config.json

# Restore experiment
nlm-cli import --filepath experiment_config.json
nlm-cli load --filepath experiment_result
```

## Debugging and Troubleshooting

### Common Issues

1. **Configuration File Not Found**
   - Ensure file path is correct
   - Check file permissions
   - Verify file format is supported

2. **Invalid Parameters**
   - Use `--help` to see valid options
   - Check parameter ranges and types
   - Ensure required parameters are provided

3. **Brain Initialization Failed**
   - Check configuration parameters
   - Verify system resources are sufficient
   - Try with different configuration

### Debug Output

Enable debug logging with:
```bash
nlm-cli --log-level debug demo
```

## Advanced Usage

### Running Multiple Experiments

```bash
#!/bin/bash
# Run multiple experiments with different configurations
for seed in 42 123 456 789; do
    echo "Running experiment with seed $seed"
    nlm-cli create --neurons 500 --regions 1
    nlm-cli --config configs/seed_$seed.cfg step --steps 100
    nlm-cli save --filepath results/seed_$seed
    nlm-cli reset
    echo "Completed seed $seed"
done
```

### Batch Processing

```bash
# Process multiple files in batch
for config in configs/*.json; do
    echo "Processing $config"
    nlm-cli --config "$config" demo
    nlm-cli export --filepath "exported_$(basename "$config")"
done
```

## Conclusion

The NLM CLI Tool provides a powerful and flexible interface for NLM brain simulation, enabling researchers to:

- Easily configure and run simulations
- Manage brain states and configurations
- Run complex experiments with reproducible results
- Integrate with existing workflows and automation
- Monitor and debug simulations effectively

This CLI tool is essential for research, development, testing, and benchmarking NLM systems in various scenarios.
