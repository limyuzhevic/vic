#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Comprehensive CLI Tool

This is the main entry point for the NLM Advanced CLI tool, providing 
enhanced functionality beyond the basic interface.

## Overview

The NLM CLI tool provides a comprehensive command-line interface for:
- Brain creation, simulation, inspection, and benchmarking
- World creation, configuration, and simulation
- Agent creation, training, evaluation, exploration, and analysis
- Configuration management (create, load, save, validate, compare, migrate)
- Experiment management (run, list, create, analyze)
- Performance profiling (start, stop, analyze, report)

## Features

### Core CLI Features
- Main command with subcommands: brain, world, agent, config, experiment, profile
- JSON output support for programmatic access
- Verbose logging and debugging options
- Configuration file support
- Command history and auto-completion
- Interactive REPL mode
- Script mode for batch operations
- Remote execution support via API server
- Web dashboard integration

### Advanced Options
- `--output-format` (json, yaml, table, csv)
- `--output-file` for saving results
- `--log-level` (debug, info, warn, error, critical)
- `--config-file` for custom configuration
- `--dry-run` for preview operations
- `--force` for overwriting existing files

### Integration Features
- Script mode for batch operations
- Remote execution support
- API server support
- Web dashboard integration

## Usage Examples

### Basic Usage

```bash
# Start interactive CLI
nlm

# Run specific command
nlm brain create --neuron-count 1000 nlm brain simulate --steps 100

# Run with custom configuration
nlm --config-file my_config.cfg brain create --neuron-count 2000

# Save output to file
nlm brain simulate --steps 100 --output-format json --output-file results.json
```

### Interactive Mode

```bash
nlm
# In interactive mode:
brain create --neuron-count 1000
brain simulate --steps 50
world create --width 100 --height 100
agent create --training-episodes 100
```

### Script Mode

```bash
# Execute commands from file
nlm --script operations.txt

# Script file example (operations.txt):
brain create --neuron-count 500
world create --width 50 --height 50
agent create --training-episodes 50
experiment run --name "demo_test"
```

### API Server

```bash
# Start API server
nlm api-server --host 0.0.0.0 --port 8080

# Query via HTTP
curl -X POST http://localhost:8080/api/commands \
  -H "Content-Type: application/json" \
  -d '{"command": "brain create --neuron-count 1000"}'
```

## Installation

```bash
pip install -e .
```

## Structure

The CLI tool is organized into the following components:

1. **nlm_cli/** - Main CLI package
   - `nlm_cli.py` - Main CLI implementation
   - `interactive.py` - Interactive REPL mode
   - `api_server.py` - Web API server

2. **Documentation**
   - `README.md` - Main documentation
   - `docs/cli_examples.md` - Command examples

3. **Configuration**
   - `configs/default.cfg` - Default configuration file

## Command Reference

### Brain Commands
- `brain create` - Create brain with custom configuration
- `brain load` - Load saved brain state
- `brain save` - Save brain state
- `brain simulate` - Run simulation with options
- `brain inspect` - Examine brain properties and statistics
- `brain benchmark` - Performance benchmarking
- `brain validate` - Validate brain configuration

### World Commands
- `world create` - Create custom world
- `world configure` - Configure world parameters
- `world simulate` - Run world simulation
- `world inspect` - Examine world state
- `world export` - Export world data for external use

### Agent Commands
- `agent create` - Create agent interface
- `agent train` - Train agent with learning algorithms
- `agent evaluate` - Evaluate agent performance
- `agent explore` - Run exploration with curiosity
- `agent analyze` - Analyze agent behavior

### Config Commands
- `config create` - Create configuration
- `config load` - Load configuration
- `config save` - Save configuration
- `config validate` - Validate configuration
- `config compare` - Compare configurations
- `config migrate` - Migrate configuration between versions

### Experiment Commands
- `experiment run` - Run experimental setup
- `experiment list` - List available experiments
- `experiment create` - Create new experiment
- `experiment analyze` - Analyze experiment results

### Profile Commands
- `profile start` - Start performance profiling
- `profile stop` - Stop profiling
- `profile analyze` - Analyze performance data
- `profile report` - Generate performance reports

## Configuration

The CLI tool supports configuration via:

1. **Configuration file** (default: `configs/default.cfg`)
2. **Command line arguments**
3. **Environment variables**

Example configuration file:

```ini
[general]
random_seed = 42
simulation_timestep = 0.001

[neuron]
neuron_count = 1000
region_count = 2
connection_probability = 0.1

[plasticity]
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

[environment]
environment_name = GridWorld
environment_width = 10
environment_height = 10

[logging]
log_level = INFO
log_to_file = false
log_filename = nlm.log
```

## Output Formats

The CLI tool supports multiple output formats:

- **JSON**: Machine-readable format for programmatic use
- **YAML**: Human-readable format for documentation
- **TABLE**: Formatted text output for console display
- **CSV**: Tabular data format for data analysis

Example usage:

```bash
nlm brain simulate --output-format json --output-file results.json
nlm brain simulate --output-format table
nlm brain simulate --output-format csv --output-file results.csv
```

## Technical Specifications

- **Programming Language**: Python 3.8+
- **Dependencies**: Click, Rich, Pydantic, PyYAML, Tabulate, Prompt-toolkit, Websockets
- **API**: RESTful HTTP API with WebSocket support
- **Configuration**: INI, JSON, and Python dictionary formats
- **Output**: Multiple formats (JSON, YAML, TABLE, CSV)
- **Interactive**: Full REPL with auto-completion and history
- **Remote**: SSH and queue system integration

## Performance

The CLI tool is optimized for:
- Fast brain simulation with configurable timesteps
- Efficient memory usage for large networks
- Parallel processing for batch operations
- Real-time monitoring and profiling

## Limitations

Current limitations:
- Some commands may require the pynlm Python bindings
- Advanced features may require additional dependencies
- Certain operations may have performance constraints for very large networks
- Interactive mode may have limited functionality in non-TTY environments

## Future Enhancements

Planned features:
- Advanced neural network architectures
- Custom learning algorithms
- Distributed computing support
- Real-time visualization
- Mobile app integration
- Cloud deployment support

## Support

For support and documentation:
- GitHub: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Issues: https://github.com/nlm-project/nlm/issues

## License

MIT License - See LICENSE file for details.
