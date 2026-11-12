# NLM Unified CLI Architecture

## Overview

The NLM project has been refactored to provide a unified command-line interface that replaces multiple phase-specific executables with a single `nlm` command supporting subcommands. This architecture provides:

- Single entry point: `nlm <command> [options] [args]`
- Six main subcommands: `run`, `analyze`, `export`, `visualize`, `config`, `tools`
- Command registration system for extensibility
- Backward compatibility with existing demo executables
- Clean separation between CLI interface and core logic

## Architecture

### Main CLI Entry Point

The main entry point is `nlm.cpp` which:

1. Parses subcommands using a command registry pattern
2. Delegates execution to appropriate command handlers
3. Provides unified help and usage documentation
4. Supports backward compatibility for existing argument patterns

### Command Interface

All commands implement the `Command` abstract base class:

- `execute()`: Execute the command with given arguments
- `getName()`: Get the command name
- `getDescription()`: Get the command description
- `getHelp()`: Get detailed help text
- `requiresRoot()`: Check if command requires root privileges
- `isInternal()`: Check if command is for internal use

### Command Registration System

Commands are registered using the `CommandRegistry` singleton:

- Commands can be dynamically registered
- Provides automatic help generation
- Supports extensibility for third-party commands
- Maintains command metadata (description, help text, etc.)

### Subcommands

#### 1. `nlm run`

**Purpose**: Run demonstrations and experiments

**Available modes**:
- `phase3`: Phase 3 world interaction demo
- `phase4`: Phase 4 cognitive emergence demo  
- `phase5`: Phase 5 personality/sandbox demo
- `phase6`: Phase 6 integration demo
- `benchmark`: Performance benchmarks

**Backward compatibility**: Maintains compatibility with original demo executable behavior

#### 2. `nlm analyze`

**Purpose**: Analyze brain state and activity

**Analysis modes**:
- `connectivity`: Neural connectivity patterns
- `plasticity`: Synaptic plasticity analysis
- `stdp`: STDP mechanism verification
- `learning`: Learning progress analysis
- `memory`: Memory systems analysis
- `all`: Run all analyses

#### 3. `nlm export`

**Purpose**: Export data and results

**Formats**:
- `json`: JSON format
- `csv`: CSV format
- `protobuf`: Protocol Buffers format
- `txt`: Text format

**Targets**:
- `brain`: Brain state export
- `config`: Configuration export
- `metrics`: Performance metrics export
- `all`: Export all data

#### 4. `nlm visualize`

**Purpose**: Create visualizations of brain activity and results

**Visualization types**:
- `neurons`: Neuron activity visualization
- `connections`: Neural connection visualization
- `spikes`: Spike pattern visualization
- `memory`: Memory trace visualization
- `plasticity`: Synaptic change visualization
- `all`: All visualizations

**Output formats**:
- `--plot`: Interactive plots
- `--video`: Animation videos
- `--svg`: SVG export
- `--html`: Web-based visualization

#### 5. `nlm config`

**Purpose**: Manage configuration settings

**Subcommands**:
- `list`: List all configuration options
- `get <key>`: Get configuration value
- `set <key> <val>`: Set configuration value
- `reset`: Reset to default configuration
- `load <file>`: Load configuration from file
- `save <file>`: Save configuration to file
- `validate`: Validate current configuration

#### 6. `nlm tools`

**Purpose**: Advanced tools and utilities

**Available tools**:
- `--checkpoint`: Create brain checkpoint
- `--replay`: Replay from checkpoint
- `--perturb`: Apply perturbations
- `--stimulus`: Apply external stimuli
- `--analyze`: Run detailed analysis
- `--simulate`: Run simulation with custom settings

## Backward Compatibility

### Phase-Specific Executables

The original phase-specific executables are still maintained for backward compatibility:

- `nlm_phase3_demo`: Phase 3 demo (now also accessible via `nlm run phase3`)
- `nlm_phase4_demo`: Phase 4 demo (now also accessible via `nlm run phase4`)
- `nlm_phase5_demo`: Phase 5 demo (now also accessible via `nlm run phase5`)
- `nlm_phase6_demo`: Phase 6 demo (now also accessible via `nlm run phase6`)
- `nlm_phase6_text_demo`: Phase 6 text demo
- `nlm_benchmark`: Benchmark executable (now also accessible via `nlm run benchmark`)

### Argument Patterns

The unified CLI maintains compatibility with original argument patterns:

1. **Original demo execution**: `nlm_phase3_demo [episodes] [steps]`
   - Now equivalent to: `nlm run phase3 --episodes N --steps M`

2. **Original benchmark execution**: `nlm_benchmark`
   - Now equivalent to: `nlm run benchmark`

3. **Original config file loading**: `--config file.cfg`
   - Now supported through `nlm config` commands

## Usage Examples

### Running Demos

```bash
# Run Phase 3 demo with custom settings
nlm run phase3 --episodes 10 --steps 500

# Run Phase 4 demo with default settings
nlm run phase4

# Run Phase 5 demo
nlm run phase5

# Run Phase 6 integration demo
nlm run phase6

# Run benchmark
nlm run benchmark
```

### Analysis

```bash
# Analyze neural connectivity
nlm analyze connectivity

# Verify STDP mechanism
nlm analyze stdp

# Run all analyses
nlm analyze all
```

### Export Data

```bash
# Export brain state to JSON
nlm export brain --output brain_state.json --format json

# Export metrics to CSV
nlm export metrics --output performance.csv --format csv

# Export all data
nlm export all --output results/ --format json
```

### Visualization

```bash
# Create neuron activity plots
nlm visualize neurons --plot

# Export connections as SVG
nlm visualize connections --svg

# Create all visualizations
nlm visualize all --output ./results
```

### Configuration

```bash
# List all configuration options
nlm config list

# Get a specific configuration value
nlm config get neuron_count

# Set a configuration value
nlm config set neuron_count 2000

# Load configuration from file
nlm config load myconfig.cfg

# Save configuration to file
nlm config save myconfig.cfg
```

### Tools

```bash
# Create a checkpoint
nlm tools --checkpoint

# Replay from checkpoint
nlm tools --replay

# Apply a perturbation
nlm tools --perturb --type=synaptic --magnitude=0.5

# Run simulation
nlm tools --simulate --steps=5000 --duration=50.0
```

## Help System

Each command provides comprehensive help information:

```bash
# General help
nlm

# Command help
nlm run --help
nlm analyze --help
nlm export --help
nlm visualize --help
nlm config --help
nlm tools --help

# Subcommand help (for commands with subcommands)
nlm config --help
```

## Building

Build the unified CLI using CMake:

```bash
mkdir build
cd build
cmake ..
make
```

The `nlm` executable will be created in the build directory along with all the original phase-specific executables for backward compatibility.

## Extensions

The command registration system makes it easy to add new commands:

1. Implement the `Command` interface
2. Register the command with `CommandRegistry::getInstance().registerCommand()`
3. The command will be automatically available in the CLI

This architecture supports third-party extensions and custom commands without modifying the core CLI code.

## Migration Guide

### For Users

If you're using the original phase-specific executables, you can migrate to the unified CLI:

1. Replace `nlm_phase3_demo 5 200` with `nlm run phase3 --episodes 5 --steps 200`
2. Replace `nlm_phase4_demo` with `nlm run phase4`
3. Replace `nlm_benchmark` with `nlm run benchmark`

The unified CLI provides all the original functionality with additional features and better organization.

### For Developers

If you're extending the NLM project:

1. Add new commands by implementing the `Command` interface
2. Use the command registration system for extensibility
3. Maintain backward compatibility by keeping the old executables
4. Follow the existing command patterns for consistency

## File Structure

```
/src/cli/
    NLMCLI.cpp              # Main CLI entry point
    SubcommandsRegistration.cpp # Command registration
    Subcommands/
        RunCommand.hpp/.cpp
        AnalyzeCommand.hpp/.cpp
        ExportCommand.hpp/.cpp
        VisualizeCommand.hpp/.cpp
        ConfigCommand.hpp/.cpp
        ToolsCommand.hpp/.cpp
/include/nlm/
    CliCommand.hpp          # Base command interface
    Subcommands/*.hpp        # Command interface headers
```

## Future Enhancements

The architecture can be easily extended with:

1. **Additional subcommands**: For new functionality
2. **Command aliases**: Multiple names for the same command
3. **Command groups**: Organize related commands
4. **Command completion**: Tab completion support
5. **Command scripting**: Support for command scripts
6. **Plugin architecture**: Load third-party commands dynamically

This unified CLI architecture provides a solid foundation for the NLM project's future development while maintaining full backward compatibility with existing code and workflows.