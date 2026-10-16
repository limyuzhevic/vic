# Unified CLI Tool for NLM Demos

## Overview

A unified command-line interface that replaces the multiple demo executables (main.cpp, Phase4Demo.cpp, Phase6Demo.cpp) with a modern, modular CLI that supports subcommands, phases, and configuration loading.

## Features

1. **Subcommands**: demo, experiment, interactive
2. **Phases**: phase1, phase2, phase3, phase4, phase5, phase6
3. **Configuration Loading**: configs/default.cfg, JSON/YAML
4. **Consistent Interface**: Same interface across all demos
5. **Backward Compatibility**: Existing config file format maintained
6. **Command-line Options**: Common settings via CLI
7. **Advanced Options**: Power user features
8. **Argument Parsing**: Modern C++ approach
9. **Configuration Merging**: Files, CLI, defaults
10. **Logging**: Verbosity control
11. **Quick Execution**: Testing modes
12. **Programmatic Configuration**: Power user API
13. **Help System**: Comprehensive usage documentation

## Design

### CLI Architecture

```
Unified CLI with subcommands:
├── demo [phase] [--config file] [--trials N] [--steps N] [--verbosity level]
├── experiment [type] [--config file] [--duration N] [--output file] [--verbose]
└── interactive [--config file] [--continue] [--log file] [--log-level level]
```

### Configuration System

Configuration is loaded in order of precedence:
1. Default configuration values
2. Configuration file (configs/default.cfg)
3. Command-line arguments
4. Runtime overrides

### Command-line Shortcuts

Quick execution modes:
```bash
# Phase 4 quick test
nlm unified-cli demo phase4 --trials 10 --quick

# Phase 3 interactive
nlm unified-cli interactive --phase phase3 --continue --log verbose

# Custom experiment
nlm unified-cli experiment custom --config my_config.json --output results.json
```

## Usage Examples

### Basic Usage

```bash
# Run Phase 2 with defaults
nlm unified-cli demo phase2

# Run Phase 2 with custom config
nlm unified-cli demo phase2 --config /path/to/config.json

# Run Phase 4 with trials
nlm unified-cli demo phase4 --trials 100

# Run Phase 6 integration test
nlm unified-cli demo phase6

# Quick test mode
nlm unified-cli demo phase4 --quick
```

### Interactive Mode

```bash
# Start interactive session
nlm unified-cli interactive

# Continue previous session
nlm unified-cli interactive --continue --load previous_session.json

# Configure logging
nlm unified-cli interactive --log my_session.log --log-level debug
```

### Experiment Mode

```bash
# Run standard experiment
nlm unified-cli experiment standard --duration 3600

# Run with custom configuration
nlm unified-cli experiment standard --config experiment_config.json --output results.csv

# Run with advanced settings
nlm unified-cli experiment standard --settings "neuron_count=2000,plasticity_rate=0.05"
```

### Configuration File Format

#### JSON Configuration
```json
{
    "phase": "phase4",
    "neuron_count": 1000,
    "trials": 50,
    "plasticity": {
        "stdp_enabled": true,
        "hebbian_enabled": true
    },
    "logging": {
        "level": "info",
        "console": true,
        "file": "nlm.log"
    }
}
```

#### YAML Configuration
```yaml
phase: phase4
neuron_count: 1000
trials: 50
plasticity:
    stdp_enabled: true
    hebbian_enabled: true
logging:
    level: info
    console: true
    file: nlm.log
```

#### Config File (configs/default.cfg)
```
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
```

## Command Reference

### demo [phase] [options]

Run a specific phase demo with unified interface.

**Arguments:**
- `phase`: One of [phase1, phase2, phase3, phase4, phase5, phase6]

**Options:**
- `--config <file>`: Load configuration from file
- `--trials <N>`: Number of trials/episodes (default: 50)
- `--steps <N>`: Number of steps per trial (default: 200)
- `--quick`: Quick execution mode (shorter trials)
- `--continuous`: Continuous execution without summary
- `--verbosity <level>`: Log verbosity (quiet, normal, verbose, debug)
- `--duration <N>`: Execution duration in seconds (0 for unlimited)
- `--output <file>`: Save results to file

**Examples:**
```bash
nlm unified-cli demo phase4 --trials 100 --steps 500 --quick
nlm unified-cli demo phase6 --config custom.json
```

### experiment [type] [options]

Run experiments with standardized interface.

**Arguments:**
- `type`: One of [standard, custom, benchmark, validation]

**Options:**
- `--config <file>`: Load configuration from file
- `--duration <N>`: Experiment duration in seconds
- `--output <file>`: Save results to file (JSON, CSV, YAML)
- `--overwrite`: Overwrite output file if exists
- `--settings <string>`: JSON string of configuration overrides
- `--parallel <N>`: Number of parallel runs (0 for sequential)
- `--seed <N>`: Random seed for reproducibility
- `--verbose`: Enable verbose logging

**Examples:**
```bash
nlm unified-cli experiment standard --duration 300 --output results.json
nlm unified-cli experiment benchmark --settings "neuron_count=5000,plasticity_rate=0.02"
```

### interactive [options]

Start interactive session with real-time control.

**Options:**
- `--config <file>`: Load initial configuration
- `--continue`: Continue from previous session
- `--load <file>`: Load session from file
- `--save <file>`: Save session to file
- `--log <file>`: Log interactive session
- `--log-level <level>`: Logging level (quiet, info, debug, trace)
- `--skip-welcome`: Skip welcome banner
- `--no-save`: Don't save session on exit

**Examples:**
```bash
nlm unified-cli interactive --log session.log --log-level verbose
nlm unified-cli interactive --continue --load saved_session.json
```

## Configuration Loading

Configuration is loaded and merged in this order:

1. **Default Configuration**: Built-in defaults for each phase
2. **Configuration File**: Loaded from specified file or configs/default.cfg
3. **Command-line Arguments**: Override file settings
4. **Runtime Configuration**: Programmatic overrides

### Configuration Sources

1. **Built-in Defaults**: Core parameters for each phase
2. **Config File**:
   - File format: .cfg, .json, .yaml, .yml
   - Path: Relative to working directory or absolute
   - Supported formats:
     ```
     configs/default.cfg
     configs/experiment.json
     configs/advanced_settings.yaml
     ```
3. **Command-line Arguments**:
   ```bash
   nlm unified-cli demo phase4 --trials 100 --neuron-count 2000
   ```
4. **Programmatic Configuration**:
   ```cpp
   config->set("neuron_count", size_t(5000), ConfigSource::Runtime);
   config->set("plasticity_rate", 0.02f, ConfigSource::Runtime);
   ```

### Configuration File Search Order

The tool searches for configuration files in this order:
1. File specified with `--config` option
2. `configs/default.cfg` in working directory
3. `configs/default.json` (if YAML parser available)
4. `configs/default.yaml`

## Logging System

The unified CLI provides comprehensive logging with configurable verbosity.

### Log Levels

1. **QUIET**: Only errors
2. **ERROR**: Errors only
3. **WARNING**: Errors and warnings
4. **INFO**: Normal operation (default)
5. **VERBOSE**: Detailed information
6. **DEBUG**: Debug information
7. **TRACE**: All information

### Logging Options

```bash
nlm unified-cli demo phase4 --verbosity verbose
nlm unified-cli interactive --log-level debug
nlm unified-cli experiment standard --output results.json --log advanced.log
```

## Quick Execution Modes

### Quick Test (--quick flag)

Reductions for faster testing:
- Fewer trials (10 instead of 50)
- Fewer neurons (500 instead of 1000)
- Shorter duration
- Simplified output

### Examples
```bash
nlm unified-cli demo phase4 --quick
# Equivalent to: --trials 10 --neuron-count 500
```

## Programmatic Configuration

Power users can configure the tool programmatically:

### Configuration API

```cpp
// Programmatic configuration
std::shared_ptr<Config> config = std::make_shared<Config>();

// Set configuration values
config->set("phase", "phase4", ConfigSource::Runtime);
config->set("neuron_count", size_t(2000), ConfigSource::Runtime);
config->set("trials", size_t(100), ConfigSource::Runtime);
config->set("plasticity_rate", 0.02f, ConfigSource::Runtime);
config->set("log_level", std::string("verbose"), ConfigSource::Runtime);

// Load from JSON
bool loaded = config->loadFromJsonFile("custom_config.json");

// Save to file
config->saveToFile("output_config.json");
```

### Command-line Generation

```cpp
// Generate command line for reproduction
std::string cmd = "nlm unified-cli demo phase4 --trials 100 --neuron-count 2000";
// Or generate with config:
cmd = config->toCommandLine();
```

## Error Handling

The unified CLI provides comprehensive error handling:

### Error Types

1. **Configuration Errors**: Invalid configuration values
2. **File Errors**: Missing or unreadable files
3. **Argument Errors**: Invalid command-line arguments
4. **Execution Errors**: Runtime failures during execution
5. **Validation Errors**: Failed validation of parameters

### Error Reporting

Errors are reported with detailed information:
- Clear error messages
- Suggested fixes
- Context information
- Stack traces for debugging

### Error Handling Examples

```bash
# Invalid configuration file
$ nlm unified-cli demo phase4 --config invalid.json
Error: Failed to load configuration file 'invalid.json'
Details: File not found or invalid JSON format

# Invalid arguments
$ nlm unified-cli demo invalid_phase
Error: Invalid phase 'invalid_phase'
Valid phases: [phase1, phase2, phase3, phase4, phase5, phase6]

# File permission error
$ nlm unified-cli experiment standard --output /root/results.json
Error: Permission denied writing to '/root/results.json'
```

## Performance Features

### Optimization Options

1. **Quick Execution**: Reduced parameters for faster testing
2. **Parallel Processing**: Multiple experiment runs
3. **Streaming Output**: Real-time results streaming
4. **Memory Management**: Efficient memory usage
5. **Lazy Loading**: Load only required components

### Performance Flags

```bash
nlm unified-cli experiment standard --parallel 4 --stream --memory-efficient
```

## Integration

The unified CLI integrates with:

### External Tools

1. **Build Systems**: CMake integration
2. **Testing Frameworks**: Integration with test runners
3. **Monitoring**: Integration with monitoring systems
4. **Version Control**: Configuration version control

### File Formats

1. **Configuration**: JSON, YAML, .cfg
2. **Output**: JSON, CSV, YAML, plain text
3. **Logging**: Plain text, JSON, structured logging

## Advanced Features

### Command-line Scripts

```bash
#!/bin/bash
# Phase 4 quick test script
nlm unified-cli demo phase4 --trials 50 --quick --output quick_results.json

# Custom experiment with configuration
nlm unified-cli experiment custom --config config.json --output results.csv

# Interactive debugging
nlm unified-cli interactive --log debug.log --log-level trace
```

### Configuration Templates

Template files for common configurations:

```yaml
# quick_test.yaml
phase: phase4
trials: 10
neuron_count: 500
quick: true
```

```json
# standard_experiment.json
{
    "phase": "phase4",
    "trials": 100,
    "neuron_count": 1000,
    "plasticity": {
        "stdp_enabled": true,
        "hebbian_enabled": true
    }
}
```

## Migration Guide

### From Multiple Executables to Unified CLI

#### Before

```bash
# Multiple executables
./nlm                                  # Phase 2
./nlm_phase3_demo 5 200               # Phase 3
./nlm_phase4_demo 50                  # Phase 4
./nlm_phase6_demo                     # Phase 6
```

#### After

```bash
# Unified interface
nlm unified-cli demo phase2          # Phase 2
nlm unified-cli demo phase3 --trials 5 --steps 200  # Phase 3
nlm unified-cli demo phase4 --trials 50   # Phase 4
nlm unified-cli demo phase6          # Phase 6
```

#### With Configuration

```bash
# Load configuration
nlm unified-cli demo phase4 --config /path/to/config.json

# Override configuration
nlm unified-cli demo phase4 --config config.json --trials 100 --neuron-count 2000
```

### Migration Benefits

1. **Single Entry Point**: One command for all demos
2. **Consistent Interface**: Same options across all demos
3. **Configuration Management**: Centralized configuration
4. **Scripting Support**: Easier script integration
5. **Testing**: Simplified test setup and execution
6. **Documentation**: Comprehensive help system

## Troubleshooting

### Common Issues

#### Issue: Configuration not loading

**Symptoms**: Default values used despite config file specified

**Solutions**:
1. Check file exists: `ls -la configs/default.cfg`
2. Verify file permissions
3. Use absolute path: `--config /full/path/config.json`

#### Issue: Invalid arguments

**Symptoms**: "Error: Invalid argument" message

**Solutions**:
1. Check help: `nlm unified-cli --help`
2. Use correct argument format
3. Verify argument types

#### Issue: Memory issues

**Symptoms**: "Out of memory" or crash

**Solutions**:
1. Use quick mode: `--quick`
2. Reduce trials: `--trials 10`
3. Check system memory

#### Issue: Permission denied

**Symptoms**: "Permission denied" error

**Solutions**:
1. Check file/directory permissions
2. Use different output location
3. Run with appropriate privileges

### Debugging Commands

```bash
# Enable debug logging
nlm unified-cli demo phase4 --verbosity debug

# Check configuration
nlm unified-cli demo phase4 --check-config

# Validate arguments
nlm unified-cli demo phase4 --validate
```

## Future Enhancements

### Planned Features

1. **Plugin Architecture**: Extensible plugin system
2. **Web Interface**: Web-based configuration and monitoring
3. **Docker Support**: Docker container for easy deployment
4. **Container Support**: Docker integration
5. **Distributed Computing**: Parallel experiment execution
6. **Real-time Monitoring**: Live system monitoring
7. **Machine Learning**: Automated parameter optimization
8. **Visualization**: Built-in visualization tools

### Future Development Roadmap

- **Phase 1**: Core CLI infrastructure (COMPLETED)
- **Phase 2**: Basic configuration and argument parsing
- **Phase 3**: Subcommand system and demo integration
- **Phase 4**: Configuration file support and error handling
- **Phase 5**: Advanced features and optimization
- **Phase 6**: Plugin architecture and web interface

## License

This unified CLI tool is part of the NLM project.
See LICENSE for details.

## Copyright

Copyright (c) 2026 NLM Project
All rights reserved.
