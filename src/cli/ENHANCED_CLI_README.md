# NLM Enhanced Command Line Interface

This document describes the enhanced command-line interface for the NLM (Neural Learning Machine) project.

## Overview

The enhanced CLI provides a comprehensive interface for interacting with the NLM brain system through multiple modes and advanced features:

- **Demo Mode**: Basic neural computation demonstration
- **Integration Mode**: Comprehensive integration testing
- **Profiling Mode**: Performance profiling and analysis
- **Experiment Mode**: Custom experiment execution
- **Config Mode**: Configuration management
- **Checkpoint Mode**: Brain state save/load
- **Visualization Mode**: Text-based visualization
- **Analysis Mode**: Brain state analysis

## Installation

### Build System

The enhanced CLI is built using CMake. After building the main project, the `nlm_cli` executable will be available.

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running

```bash
./nlm_cli [command] [options]
```

## Commands

### Demo Mode

```bash
nlm_cli demo [--steps <num>] [--neurons <num>] [--seed <num>]
```

Runs a demonstration of basic neural computation with spiking neurons.

**Options:**
- `--steps <num>`: Number of simulation steps (default: 1000)
- `--neurons <num>`: Number of neurons (default: 1000)
- `--seed <num>`: Random seed for reproducibility

### Integration Mode

```bash
nlm_cli integration [--neurons <num>] [--steps <num>]
```

Verifies that all brain systems work together correctly.

**Options:**
- `--neurons <num>`: Number of neurons for integration test
- `--steps <num>`: Number of simulation steps

### Profiling Mode

```bash
nlm_cli profile [--duration <sec>] [--samples <num>] [--simple]
```

Runs performance profiling of the brain system.

**Options:**
- `--duration <sec>`: Profiling duration in seconds (default: 30)
- `--samples <num>`: Number of samples to collect (default: 1000)
- `--simple`: Simple profiling without detailed output

### Experiment Mode

```bash
nlm_cli experiment [--name <str>] [--duration <num>] [--repetitions <num>] [--no-save]
```

Runs custom experiments with configurable parameters.

**Options:**
- `--name <str>`: Experiment name (default: custom_experiment)
- `--duration <num>`: Duration in steps (default: 10000)
- `--repetitions <num>`: Number of repetitions (default: 5)
- `--no-save`: Disable automatic checkpoint saving

### Config Mode

```bash
nlm_cli config [--show|-s] [--set|-t <key> <value>] [--get|-g <key>] [--export|-e]
```

Manages and displays configuration settings.

**Options:**
- `--show, -s`: Show current configuration
- `--set, -t <key> <value>`: Set configuration value
- `--get, -g <key>`: Get configuration value
- `--export, -e`: Export configuration to file

### Checkpoint Mode

```bash
save, -s               Save current brain state
sload, -l              Load brain state from file
--file <path>          Specify file path (default: checkpoints/default_checkpoint.nlm)
```

Saves and loads brain checkpoints for experiments.

### Visualization Mode

```bash
nlm_cli viz [--detailed|-d] [--continuous|-c] [--update-rate <sec>]
```

Runs visualization modes (text-based).

**Options:**
- `--detailed, -d`: Enable detailed visualization
- `--continuous, -c`: Continuous visualization mode
- `--update-rate <sec>`: Visualization update rate in seconds (default: 10)

### Analysis Mode

```bash
nlm_cli analysis
```

Analyzes brain state and neural dynamics.

### Help and Information

```bash
nlm_cli help                     # Show all available commands
nlm_cli help <command>           # Show help for specific command
nlm_cli version                   # Display version information
nlm_cli exit                      # Exit the application
```

## Configuration

### Configuration File

The CLI uses configuration files for settings. The default configuration is located at `configs/default_cli.cfg` and includes:

```ini
# General CLI settings
cli_help_enabled = true
cli_version_output = true
cli_progress_indicators = true
cli_checkpoint_format = json

# Logging configuration
cli_log_level = INFO
cli_log_to_file = false
cli_log_filename = nlm_cli.log
cli_log_rotation = 10485760
cli_log_colors = true

# Default mode
cli_default_mode = demo
```

### Command Line Configuration

Configuration can also be set via command-line arguments:

```bash
nlm_cli demo --steps 2000 --seed 123
nlm_cli config --set neuron_count 5000
```

## Features

### Advanced Help System

The enhanced CLI provides detailed help for each command, including:
- Command descriptions
- Usage examples
- Available options
- Parameter explanations

### Multiple Execution Modes

Each mode provides specialized functionality:

1. **Demo Mode**: Educational demonstrations of neural computation
2. **Integration Mode**: Comprehensive system verification
3. **Profiling Mode**: Performance analysis and optimization
4. **Experiment Mode**: Flexible experiment execution
5. **Config Mode**: Configuration management tools
6. **Checkpoint Mode**: Brain state persistence
7. **Visualization Mode**: Interactive text-based displays
8. **Analysis Mode**: Deep brain state inspection

### Checkpoint Management

The CLI supports saving and loading brain checkpoints:
- Automatic checkpoint saving (configurable)
- Multiple checkpoint formats (JSON, binary)
- Checkpoint compression (configurable)
- Checkpoint directory management

### Text-based Visualization

The visualization mode provides:
- Real-time brain state displays
- Progress indicators with ETA
- Detailed neural activity information
- Configurable update rates

### Logging Control

Comprehensive logging control includes:
- Multiple log levels (Quiet, Error, Warning, Info, Debug, Verbose)
- Console and file output
- Log rotation and compression
- Color-coded output (when supported)

### Progress Indicators

All long-running operations display progress:
- Progress bars with percentage completion
- Estimated time remaining
- Descriptive status messages
- Real-time updates

## Integration with Existing Brain System

The enhanced CLI integrates seamlessly with the existing NLM brain system:

1. **Configuration Sharing**: Uses the same configuration system as the core
2. **Brain Initialization**: Properly initializes brain instances with correct parameters
3. **State Management**: Supports checkpoint save/load for brain persistence
4. **Visualization Support**: Integrates with existing visualization interfaces
5. **Performance Monitoring**: Provides profiling tools for brain system optimization

## Example Usage

### Basic Demo

```bash
nlm_cli demo
```

### Custom Experiment

```bash
nlm_cli experiment --name learning_experiment --duration 5000 --repetitions 3
```

### Integration Test with Profiling

```bash
nlm_cli integration --neurons 3000 --steps 2000
```

### Configuration Management

```bash
nlm_cli config --show
nlm_cli config --set simulation_timestep 0.01
nlm_cli config --get neuron_count
```

### Checkpoint Management

```bash
nlm_cli checkpoint save --file ./checkpoints/experiment_1.nlm
nlm_cli checkpoint load --file ./checkpoints/experiment_1.nlm
```

## Configuration File Reference

### General Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| cli_help_enabled | bool | true | Enable/disable help system |
| cli_version_output | bool | true | Enable/disable version output |
| cli_progress_indicators | bool | true | Enable/disable progress indicators |
| cli_checkpoint_format | string | json | Checkpoint file format |

### Logging Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| cli_log_level | string | INFO | Log level (QUIET, ERROR, WARNING, INFO, DEBUG, VERBOSE) |
| cli_log_to_file | bool | false | Enable file logging |
| cli_log_filename | string | nlm_cli.log | Log file name |
| cli_log_rotation | int | 10485760 | Log file rotation size (bytes) |
| cli_log_colors | bool | true | Enable colored output |

### Demo Mode Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| demo_mode_brain_size | int | 1000 | Number of neurons for demo mode |
| demo_mode_steps | int | 1000 | Number of steps for demo mode |
| demo_mode_seed | int | 42 | Random seed for demo mode |

### Integration Mode Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| integration_mode_brain_size | int | 5000 | Number of neurons for integration |
| integration_mode_steps | int | 5000 | Number of steps for integration |
| integration_mode_connection_probability | float | 0.15 | Connection probability |
| integration_mode_plasticity_enabled | bool | true | Enable plasticity in integration |

### Performance Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| performance_profiling_enabled | bool | true | Enable performance profiling |
| performance_memory_tracking | bool | true | Enable memory tracking |
| performance_thread_count | int | 1 | Number of threads for parallel processing |

### Visualization Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| cli_text_viz_enabled | bool | true | Enable text-based visualization |
| cli_text_viz_update_rate | float | 10.0 | Visualization update rate (seconds) |
| cli_text_viz_detailed | bool | false | Enable detailed visualization |

### Checkpoint Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| checkpoint_enabled | bool | true | Enable checkpointing |
| checkpoint_interval | int | 100 | Checkpoint save interval (steps) |
| checkpoint_dir | string | ./checkpoints | Checkpoint directory |
| checkpoint_compress | bool | true | Compress checkpoint files |

## Troubleshooting

### Common Issues

**Help not displaying:**
- Ensure CLI is properly installed
- Check if help files are accessible
- Try `nlm_cli help` for general help

**Commands not found:**
- Ensure all CLI files are compiled
- Check CMake configuration
- Verify executable permissions

**Configuration errors:**
- Check configuration file syntax
- Ensure required directories exist
- Verify file permissions

**Performance issues:**
- Reduce brain size for testing
- Disable logging for speed
- Use simpler modes for debugging

### Getting Help

For additional assistance:
1. Use `nlm_cli help` for command documentation
2. Check configuration files in `configs/` directory
3. Review the architecture documentation in `docs/` directory
4. Consult the main NLM documentation

## Development

### Adding New Commands

To add a new command:

1. Create a new command class in `CLICommands.hpp`
2. Implement the command in `CLICommands.cpp`
3. Register the command in `CommandRegistry`
4. Update help system if needed

### Adding Configuration Options

To add a new configuration option:

1. Add to `configs/default_cli.cfg`
2. Update `initializeDefaultConfig()` in `NLMCommandLine.cpp`
3. Update `setupLogging()` and other configuration parsing code

### Extending Visualization

To extend visualization capabilities:

1. Add new visualization modes to `VisualizationCommand`
2. Update `VisualizationInterface` integration
3. Add new display formats and update rates

## Compatibility

The enhanced CLI is compatible with:
- NLM Phase 6 (current implementation)
- All existing brain system components
- Standard configuration file formats
- POSIX-compliant systems
- UTF-8 text encoding

## Future Enhancements

Planned improvements include:
1. Graphical user interface (TUI/CLI hybrid)
2. Advanced visualization with matplotlib
3. Real-time web interface
4. Machine learning model integration
5. Enhanced experiment management
6. Distributed computing support
7. Cloud deployment options

## License

This enhanced command-line interface is part of the NLM project, licensed under the MIT License.

For more information, see the project documentation and source code.