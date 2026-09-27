# User Manual

## Overview

NLM (熙然) is an experimental artificial developmental brain project. This manual provides step-by-step instructions for using NLM to run brain simulation experiments.

## Installation

### Prerequisites

- CMake 3.10 or higher
- C++ compiler with C++11 support
- 4+ GB RAM (recommended)

### Build Instructions

```bash
# Navigate to the project root
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4
```

### Quick Start

```bash
# Run the Phase 6 demo (integration test)
./nlm_phase6_demo

# Run with custom configuration
./nlm --config configs/default.json
```

## Basic Usage

### Running Experiments

NLM supports several built-in experiments. Here are the main commands:

| Command | Description | Configuration |
|---------|-------------|---------------|
| `nlm_phase6_demo` | Comprehensive integration test | Default config |
| `nlm` | General brain simulation | config file |
| `nlm --help` | Show available options | N/A |

### Example Experiment: Simple World Interaction

```bash
# Create a build directory if not exists
mkdir -p build && cd build

# Configure with custom parameters
cmake .. -DEXPERIMENT_TYPE=SimpleWorld -DBRAIN_SIZE=1024

# Build
make -j4

# Run the experiment
./nlm --config=../configs/simple_world.json
```

## Configuration

### Configuration File Format

Configuration is managed through JSON files. Here are the essential parameters:

```json
{
    "simulation": {
        "timestep": 0.001,
        "steps": 1000,
        "random_seed": 42
    },
    "brain": {
        "neuron_count": 1024,
        "region_count": 4,
        "plasticity_enabled": true
    },
    "neuromodulation": {
        "dopamine_enabled": true,
        "curiosity_enabled": true
    }
}
```

### Common Configuration Options

#### Simulation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation.timestep` | float | 0.001 | Simulation time step in seconds |
| `simulation.steps` | int | 1000 | Number of simulation steps |
| `simulation.random_seed` | int | 42 | Random seed for reproducibility |

#### Brain Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1024 | Number of neurons in the brain |
| `brain.region_count` | int | 4 | Number of brain regions |
| `brain.plasticity_enabled` | bool | true | Enable neural plasticity |

### Configuration Examples

#### Basic Configuration

See `configs/default.json` for the basic configuration.

#### Simple World Environment

See `configs/simple_world.json` for environment-specific settings.

## Command Line Interface

### Available Commands

NLM provides a flexible command-line interface. Use these commands to control brain simulation:

#### Core Commands

- `help` or `--help`: Display help information
- `version` or `--version`: Show NLM version
- `list-experiments`: List available experiments
- `status`: Show current simulation status

#### Simulation Control

- `run [steps]` or `--run [steps]`: Run simulation for specified steps
- `pause` or `--pause`: Pause current simulation
- `resume` or `--resume`: Resume paused simulation
- `stop` or `--stop`: Stop simulation

#### Configuration Management

- `load [file]` or `--load [file]`: Load configuration from file
- `save [file]` or `--save [file]`: Save current configuration
- `export [format]` or `--export [format]`: Export results

### Command Examples

```bash
# Display help information
nlm --help

# List available experiments
nlm list-experiments

# Run simulation for 100 steps with custom config
nlm run 100 --config=experiment.json

# Pause current simulation
nlm pause

# Resume simulation
nlm resume

# Save current configuration to file
nlm save current_config.json

# Export results as CSV
nlm export csv results.csv
```

## Working with Experiments

### Experiment Lifecycle

1. **Preparation**: Load configuration and initialize brain
2. **Execution**: Run simulation steps
3. **Monitoring**: Track brain state and performance
4. **Analysis**: Process results and generate insights

### Experiment States

| State | Description |
|-------|-------------|
| `INITIALIZING` | Loading configuration and setting up brain |
| `RUNNING` | Active simulation with processing |
| `PAUSED` | Simulation paused for analysis |
| `COMPLETED` | Simulation finished successfully |
| `ERROR` | Simulation terminated due to error |

### Real-time Monitoring

During simulation, you can monitor:

- **Spike Statistics**: Total spikes, spike rate
- **Memory Usage**: Working memory activity
- **Prediction Accuracy**: Prediction system performance
- **Plasticity**: Weight change statistics
- **Neuromodulation**: Dopamine and curiosity signals

## Troubleshooting

### Common Issues

#### Issue: "Configuration file not found"

**Solution:**
- Ensure the configuration file path is correct
- Verify file exists with `ls -l your_config.json`
- Use absolute paths or relative to working directory

#### Issue: "Memory allocation failed"

**Solution:**
- Increase available memory
- Reduce brain size parameters
- Check system resources with `free -h`

#### Issue: "Simulation hangs or becomes unresponsive"

**Solution:**
- Check for infinite loops in configuration
- Add timeout parameters
- Verify brain region connectivity

### Getting Help

If you encounter issues:

1. **Check Documentation**: Refer to this user manual
2. **Configuration Examples**: Review existing configs
3. **Experiment Logs**: Examine simulation output
4. **Community**: Join NLM discussion forums
5. **Development Team**: Submit detailed issue reports

### Contact Information

- **Documentation Issues**: Report at GitHub Issues
- **Technical Support**: Contact project maintainers
- **Bug Reports**: Include configuration and log output

## Future Enhancements

The NLM project is actively developed. Upcoming features include:

- Enhanced visualization tools
- Advanced configuration GUI
- Multi-brain comparison tools
- Web-based experiment monitoring
- Export to common data formats

## License

NLM is licensed under the MIT License. See LICENSE file for details.