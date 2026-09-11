# NLM Advanced Command System

## Overview

This document describes the NLM (Neural Learning Machine) Advanced Command System, a comprehensive set of expert-level commands for batch processing, debugging, configuration, analysis, and development.

## Implementation Summary

The NLM Advanced Command System adds powerful command-line interface capabilities to the existing NLM framework while maintaining full backward compatibility.

### Key Features

1. **Advanced Command Processor**: Main command processing infrastructure with plugin-like architecture
2. **Batch Processing**: Batch execution of multiple experiments with configurable variations
3. **Debug Tools**: Comprehensive debugging and monitoring capabilities
4. **Network Analysis**: Graph theory-based network connectivity analysis
5. **Expert Configuration**: Advanced configuration manipulation and tuning
6. **Analysis Commands**: Brain state and behavior analysis
7. **Development Tools**: Build system utilities and code generation

## Command Architecture

### Command Structure

All advanced commands follow the pattern:

```bash
nlm [common_options] <command> [command_options]
```

### Common Options

- `--help, -help`: Show help for a specific command or all commands
- `--verbose, -v`: Increase verbosity level
- `--debug, -d`: Enable debug mode with detailed logging
- `--config=<file>`: Load configuration from file
- `--output=<file>`: Output file for command results
- `--json`: Output results in JSON format

## Command Categories

### 1. Batch Processing Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `run` | Run simulation with configuration | `nlm run [--config=<file>] [--verbose] [--debug]` |
| `profile` | Profile performance and timing metrics | `nlm profile [--verbose]` |
| `batch` | Run batch of experiments with different configs | `nlm batch [--verbose]` |
| `export` | Export brain states for analysis | `nlm export [--output=<file>] [--verbose]` |

### 2. Debugging Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `debug` | Enable debug mode with detailed logging | `nlm debug [--verbose]` |
| `brain` | Inspect and analyze brain state | `nlm brain [--verbose]` |
| `anatomy` | Detailed brain structure and connectivity analysis | `nlm anatomy [--verbose]` |

### 3. Expert Configuration Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `config` | Advanced config manipulation | `nlm config [--set=<key>=<value>] [--load=<file>] [--save=<file>] [--validate=<key>] [--verbose]` |
| `tune` | Brain parameter tuning | `nlm tune [--optimize=<metric>] [--verbose]` |
| `topology` | Network topology exploration | `nlm topology [--verbose]` |

### 4. Analysis Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `analyze` | Analyze brain state and behavior | `nlm analyze [--verbose]` |
| `network` | Network connectivity metrics | `nlm network [--verbose]` |
| `metrics` | Learning statistics collection | `nlm metrics [--verbose]` |

### 5. Development Tools

| Command | Description | Usage |
|---------|-------------|-------|
| `build` | Build system utilities | `nlm build [--verbose]` |
| `generate` | Code generation for new neuron types | `nlm generate [--verbose]` |
| `test` | Test suite management | `nlm test [--verbose]` |

## Key Components

### 1. AdvancedCommandProcessor

Location: `src/NLMAdvancedCommandProcessor.hpp` and `src/NLMAdvancedCommandProcessor.cpp`

**Purpose**: Main command processing infrastructure

**Features**:
- Command registration system
- Flexible option parsing
- Context-sensitive help system
- Error handling and validation
- Plugin-like architecture for extensibility

### 2. BatchProcessor

Location: `src/BatchProcessor.hpp` and `src/BatchProcessor.cpp`

**Purpose**: Batch execution of multiple experiments

**Features**:
- Experiment management
- Configuration variation generation
- Batch execution with progress tracking
- Results aggregation and statistics
- Batch configuration saving/loading

### 3. DebugTools

Location: `src/DebugTools.hpp` and `src/DebugTools.cpp`

**Purpose**: Comprehensive debugging and monitoring

**Features**:
- Brain state snapshot capability
- Anomaly detection
- Real-time monitoring
- Performance profiling
- Memory inspection
- Debug logging and analysis

### 4. NetworkAnalysis

Location: `src/NetworkAnalysis.hpp` and `src/NetworkAnalysis.cpp`

**Purpose**: Graph theory-based network analysis

**Features**:
- Network graph extraction from brain
- Connectivity metrics computation
- Community detection
- Shortest path analysis
- Centrality measures
- Network visualization

### 5. Command Documentation

Location: `ADVANCED_COMMANDS.md`

**Purpose**: Complete command documentation and examples

**Features**:
- Command descriptions
- Usage examples
- Option details
- Scripting support

## Usage Examples

### Basic Usage

```bash
# Run a simulation
nlm run --config=my_config.json --verbose

# Enable debug mode
nlm debug --verbose

# Analyze brain state
nlm brain --verbose

# Run batch processing
nlm batch --verbose
```

### Expert Usage

```bash
# Advanced configuration
nlm config --set=random_seed=123 --set=neuron_count=1000 --verbose

# Network analysis
nlm topology --verbose

# Performance profiling
nlm profile --verbose

# Export results
nlm export --output=brain_data.checkpoint --verbose
```

### Scripting Support

```bash
#!/bin/bash
# Run multiple experiments
for config in configs/*.json; do
    nlm batch --config=$config --output=result_${config}.json
    nlm analyze --verbose
    nlm export --output=brain_${config}.checkpoint
    echo "Completed $config"
done
```

### Batch Configuration

```bash
#!/bin/bash
# Create batch experiment
nlm batch --experiments=configs/experiment_list.txt
nlm batch --generate-variations --base=experiment1 --parameters="learning_rate=0.1,0.01,0.001"
```

## Implementation Details

### Command Processing

1. **Command Registration**: Commands are registered with handlers in the AdvancedCommandProcessor
2. **Option Parsing**: Flexible parsing of both short and long command-line options
3. **Help System**: Context-sensitive help for all commands
4. **Error Handling**: Comprehensive error messages and exit codes
5. **Validation**: Input validation before command execution

### Integration with NLM Systems

The advanced commands integrate with:

- **Brain class**: Direct access to neural systems for simulation and analysis
- **AgentBrain class**: High-level interface for agent control
- **ExperimentRunner class**: Batch experiment management
- **Performance class**: Performance monitoring and profiling
- **Config class**: Configuration management and manipulation
- **Logger class**: Logging and debugging support

### Performance Considerations

- **Real-time Monitoring**: Optional real-time output during long operations
- **Memory Efficiency**: Optimized for large-scale simulations
- **Parallel Processing**: Support for parallel batch execution
- **Caching**: Results caching for repeated operations

## Configuration File Support

The system supports multiple configuration formats:

- **JSON**: `.json` files
- **YAML**: `.yaml` or `.yml` files
- **Key-value**: `.cfg` or `.txt` files
- **Command line**: `--key=value` format

## Output Formats

Results can be output in multiple formats:

- **Standard output**: Console output
- **JSON format**: Machine-readable output
- **File output**: Save to files
- **Log files**: Detailed logging to files

## Error Handling

The advanced command processor provides:

- **Detailed error messages**: Clear indication of what went wrong
- **Exit codes**: Different exit codes for different error types
- **Recovery options**: Suggestions for fixing common errors
- **Validation**: Input validation before execution

## File Structure

```
/workspace/project/
├── src/
│   ├── NLMAdvancedCommandProcessor.hpp (Main command processor)
│   ├── NLMAdvancedCommandProcessor.cpp
│   ├── BatchProcessor.hpp (Batch processing system)
│   ├── BatchProcessor.cpp
│   ├── DebugTools.hpp (Debug and monitoring)
│   ├── DebugTools.cpp
│   ├── NetworkAnalysis.hpp (Network analysis)
│   ├── NetworkAnalysis.cpp
│   └── ... (Other NLM components)
├── ADVANCED_COMMANDS.md (Complete documentation)
└── configs/
    ├── default.json
    ├── default.cfg
    └── default.yaml
```

## Building and Testing

### Build Instructions

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Testing

```bash
# Run all tests
ctest --output-on-failure

# Run specific tests
ctest -R <test_name>

# Run advanced command tests
./nlm --help  # Test basic help
./nlm run --help  # Test specific command help
```

## Backward Compatibility

The NLM Advanced Command System maintains full backward compatibility:

- Existing `--config` flag still works
- Existing `--help` flag still works
- Existing command line parsing is preserved
- All existing functionality remains intact

## Future Enhancements

Planned features for future versions:

1. **REST API**: HTTP API for remote command execution
2. **Web interface**: Web-based command console
3. **GUI tools**: Graphical user interface
4. **Plugin architecture**: External command plugins
5. **Cloud integration**: Cloud-based execution

## Contributors

- NLM Development Team
- Phase 6 Integration Team
- Expert User Community

## License

MIT License

## Copyright

Copyright (c) 2026 NLM Project

This project is part of the NLM (熙然) - Neural Learning Machine research project.
