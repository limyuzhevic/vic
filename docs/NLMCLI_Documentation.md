# NLM CLI Documentation

## Overview

The NLM Command Line Interface (CLI) provides a comprehensive and intuitive interface for controlling the Neural Learning Machine (NLM) brain simulation project. It offers powerful capabilities for running simulations, managing configurations, executing experiments, generating visualizations, and analyzing performance.

## System Architecture

### Core Components

1. **CommandRunner Framework**
   - Plugin-like architecture for extensible command system
   - Command registration and execution management
   - Interactive mode support
   - Tab completion for improved UX

2. **Configuration Management**
   - Integration with existing NLM Config system
   - Support for multiple configuration sources (CLI, file, environment, defaults)
   - Configuration validation and error handling
   - Export/import capabilities

3. **Command Implementations**
   - **Run**: Execute simulations with customizable scenarios
   - **Config**: Create, edit, validate, and manage configuration files
   - **Experiment**: Run predefined experiments with parameters
   - **Visualize**: Generate various visualization outputs
   - **Benchmark**: Performance benchmarking and analysis
   - **Status**: System information and diagnostics
   - **Help**: Comprehensive command documentation

4. **Output Formatting**
   - Support for JSON, CSV, and human-readable formats
   - Progress reporting for long-running operations
   - Error handling and recovery messages
   - Progress indicators and status updates

5. **Logging Integration**
   - Integration with existing NLM Logger system
   - Configurable log levels (debug, info, warning, error, critical)
   - Console and file logging support
   - Color-coded output for improved readability

### File Structure

```
workspace/app/src/cli/
├── NLMCLI.hpp          # Main CLI interface headers
├── NLMCLI.cpp          # CLI implementation
├── NLMCLIMain.cpp      # Main entry point
├── Commands/
│   ├── RunCommand.hpp      # Run simulation command
│   ├── RunCommand.cpp      # Run simulation implementation
│   ├── ConfigCommand.hpp    # Configuration management command
│   ├── ConfigCommand.cpp    # Configuration management implementation
│   ├── ExperimentCommand.hpp # Experiment execution command
│   ├── ExperimentCommand.cpp # Experiment execution implementation
│   ├── VisualizeCommand.hpp  # Visualization command
│   ├── VisualizeCommand.cpp  # Visualization implementation
│   ├── BenchmarkCommand.hpp  # Benchmark command
│   ├── BenchmarkCommand.cpp  # Benchmark implementation
│   ├── StatusCommand.hpp     # Status command
│   ├── StatusCommand.cpp     # Status implementation
│   ├── HelpCommand.hpp       # Help command
│   └── HelpCommand.cpp       # Help implementation
├── include/nlm/          # Embedded C++ interface
│   └── NLMCLI.h         # C++ CLI interface
workspace/app/tests/        # Test suite
├── test_cli.cpp           # Basic CLI tests
├── test_cli_integration.cpp # Integration tests
workspace/app/examples/      # Usage examples
└── cli_examples.cpp        # CLI usage demonstrations
```

## Command Reference

### Global Options

All commands accept the following global options:

```bash
--config <file>        Configuration file path
--output <format>      Output format (json, csv, text)
--log-level <level>    Logging level (debug, info, warning, error)
--verbose, -v          Verbose output
--quiet, -q            Quiet output
--help                 Show help
--version              Show version
```

### Available Commands

#### `run <scenario> [options]`

**Description:** Run a neural simulation with the specified scenario and configuration.

**Arguments:**
- `<scenario>`: Scenario name or configuration file path

**Options:**
- `--steps <n>`: Number of simulation steps (default: 1000)
- `--neuron_count <n>`: Number of neurons (default: 1000)
- `--region_count <n>`: Number of brain regions (default: 1)
- `--random_seed <n>`: Random seed for reproducibility (default: 42)
- `--output <format>`: Output format (json, csv, text)
- `--log-level <level>`: Logging level
- `--verbose, -v`: Verbose output
- `--quiet, -q`: Quiet output

**Examples:**
```bash
nlm-cli run movie_test --neuron_count=1000 --steps=2000
nlm-cli run custom_config.cfg --output=json
```

#### `config <action> [key=value...]`

**Description:** Configuration management (create, edit, validate, save/load).

**Actions:**
- `create <file>`: Create a new configuration file
- `load <file>`: Load configuration from file
- `save <file>`: Save configuration to file
- `show [key]`: Show configuration values
- `set <key> <value>`: Set a configuration value
- `remove <key>`: Remove a configuration key
- `validate`: Validate configuration
- `list`: List all configuration keys
- `template <name>`: Load configuration template
- `export <format> <file>`: Export configuration to different format
- `import <format> <file>`: Import configuration from different format
- `merge <file>`: Merge configuration from file

**Examples:**
```bash
nlm-cli config create custom_config.cfg
nlm-cli config load custom_config.cfg
nlm-cli config show random_seed neuron_count
nlm-cli config set plasticity.learning_rate 0.001
nlm-cli config template performance > config.cfg
```

#### `experiment <name> [options]`

**Description:** Run predefined experiments.

**Arguments:**
- `<name>`: Experiment name (Phase3Demo, Phase4Demo, Phase5Experiment, Phase6Demo, etc.)

**Options:**
- `--steps <n>`: Number of simulation steps (default: 1000)
- `--repeats <n>`: Number of times to repeat experiment (default: 1)
- `--neuron_count <n>`: Number of neurons (default: 1000)
- `--region_count <n>`: Number of brain regions (default: 1)
- `--output <file>`: Output file for results
- `--record_metrics`: Record performance metrics

**Examples:**
```bash
nlm-cli experiment Phase3Demo --steps=5000 --repeats=3
nlm-cli experiment Phase4Demo --output=experiment_results.json
```

#### `visualize <output> [options]`

**Description:** Generate visualization outputs.

**Arguments:**
- `<output>`: Output file or directory for visualizations

**Options:**
- `--format <format>`: Visualization format (graphml, dot, png, svg)
- `--neuron_count <n>`: Number of neurons (default: 1000)
- `--region_count <n>`: Number of brain regions (default: 1)
- `--viz_type <type>`: Visualization type (network, activity, connectivity)
- `--resolution <width> <height>`: Output resolution

**Examples:**
```bash
nlm-cli visualize network_graph --format=dot --viz_type=network
nlm-cli visualize activity_data --format=csv --viz_type=activity
```

#### `benchmark [options]`

**Description:** Performance benchmarking.

**Options:**
- `--neurons <n>`: Number of neurons (default: 1000)
- `--steps <n>`: Number of steps (default: 1000)
- `--repeats <n>`: Number of benchmark runs (default: 4)
- `--output <file>`: Output file for results

**Examples:**
```bash
nlm-cli benchmark --neurons=2000 --steps=1000 --repeats=4
nlm-cli benchmark --neurons=5000 --steps=500 --output=benchmark_results.csv
```

#### `status [options]`

**Description:** Show system status and information.

**Options:**
- `--verbose, -v`: Show detailed status information

**Examples:**
```bash
nlm-cli status
nlm-cli status --verbose
```

#### `help [command]`

**Description:** Show detailed help for all commands or a specific command.

**Arguments:**
- `<command>`: Command name to show help for (optional)

**Examples:**
```bash
nlm-cli help
nlm-cli help run
nlm-cli help config
```

## Configuration Integration

### Configuration Sources

The NLM CLI supports multiple configuration sources with the following precedence:

1. **Command Line Arguments** (highest priority)
2. **Configuration Files** (medium priority)
3. **Environment Variables** (low priority)
4. **Default Values** (lowest priority)

### Configuration File Formats

The CLI supports multiple configuration file formats:

1. **INI format** (default, key=value)
2. **JSON format** (structured configuration)
3. **CSV format** (comma-separated values)

### Configuration Validation

The CLI provides comprehensive configuration validation:

- **Type validation**: Ensure values match expected types
- **Range validation**: Check values are within acceptable ranges
- **Required parameter validation**: Ensure essential parameters are set
- **Cross-parameter validation**: Check relationships between parameters

## Output Formats

### JSON Output

Formatted JSON output suitable for machine processing:

```json
{
  "command": "run",
  "scenario": "movie_test",
  "neurons": 1000,
  "steps": 2000,
  "success": true,
  "execution_time": 1.234,
  "results": {
    "total_spikes": 1500,
    "average_firing_rate": 3.5
  }
}
```

### CSV Output

Comma-separated values format suitable for spreadsheet analysis:

```csv
command,scenario,neurons,steps,success,execution_time,total_spikes,average_firing_rate
run,movie_test,1000,2000,true,1.234,1500,3.5
```

### Text Output

Human-readable output with formatting:

```
╔══════════════════════════════════════════════════════════════════════════════╗
║                    Simulation Results                                     ║
╠════════════════════════════════════════════════════════════════════════════════╣
║ Scenario: movie_test                                                      ║
║ Neurons: 1000                                                              ║
║ Steps: 2000                                                                ║
║ Total Spikes: 1500                                                         ║
║ Average Firing Rate: 3.5 Hz                                                ║
║ Execution Time: 1.234s                                                     ║
║ Success: Yes                                                               ║
╚══════════════════════════════════════════════════════════════════════════════╝
```

## Interactive Mode

The NLM CLI provides an interactive mode for iterative exploration:

```bash
nlm-cli

>>> help
Available commands:
  run      : Run a neural simulation
  config   : Configuration management
  experiment: Run predefined experiments
  visualize: Generate visualization outputs
  benchmark: Performance benchmarking
  status   : Show system status and information
  help     : Show detailed help for all commands

>>> run movie_test --steps=100
Starting simulation: movie_test
Simulation running...
Simulation completed successfully

>>> config show random_seed neuron_count
random_seed = 42
neuron_count = 1000

>>> exit
```

## Tab Completion

The CLI supports tab completion for improved user experience:

```bash
nlm-cli run|  # Tab completion shows available scenarios
nlm-cli config|  # Tab completion shows available config actions
nlm-cli experiment|  # Tab completion shows available experiments
```

## Progress Reporting

The CLI provides real-time progress reporting for long-running operations:

```
Running simulation... (0%)
Progress: Simulation running... (25%)
Progress: Simulation running... (50%)
Progress: Simulation running... (75%)
Progress: Simulation running... (100%) - Completed
```

## Error Handling

The CLI provides comprehensive error handling with clear messages:

### Common Errors and Solutions

1. **Unknown Command**
   - Error: "Unknown command: invalid_command"
   - Solution: "nlm-cli help"
   - Shows all available commands

2. **Missing Required Arguments**
   - Error: "Missing scenario argument"
   - Solution: "nlm-cli help run"
   - Shows usage for the specific command

3. **Configuration File Not Found**
   - Error: "Failed to load configuration from: file.cfg"
   - Solution: "nlm-cli config create my_config.cfg"

4. **Invalid Parameter Value**
   - Error: "Configuration validation failed for key: neuron_count"
   - Solution: Check parameter range or use validation

## Integration with Existing NLM Components

### Config System

The CLI integrates seamlessly with the existing NLM Config system:

- **Type-safe parameters**: All configuration values are strongly typed
- **Validation support**: Parameters can have validation rules
- **Multiple sources**: Configuration can come from files, CLI, or runtime
- **Hierarchical organization**: Configuration is organized by sections

### Experiment Framework

The CLI provides access to the existing experiment framework:

- **Predefined experiments**: Phase3Demo, Phase4Demo, Phase5Experiment, Phase6Demo
- **Experiment parameters**: Customizable experiment settings
- **Result recording**: Automatic recording of experiment metrics
- **Experiment management**: Create, run, and manage experiments

### Brain System

The CLI integrates with the neural brain system:

- **Simulation control**: Start, stop, and monitor simulations
- **Configuration management**: Set brain parameters via CLI
- **Progress monitoring**: Real-time monitoring of simulation progress
- **Result analysis**: Access to brain simulation results

### Logging System

The CLI integrates with the NLM logging system:

- **Log levels**: Support for debug, info, warning, error, critical
- **Output formatting**: Colored console output for better readability
- **File logging**: Option to log to files
- **Log filtering**: Filter logs by level and component

## Performance Features

### Command Caching

Frequently used commands can be cached for faster execution:

```bash
# First run caches command
nlm-cli run movie_test --steps=100
# Subsequent runs are faster
nlm-cli run movie_test --steps=100
```

### Parallel Processing

The CLI supports parallel processing for batch operations:

```bash
# Run multiple scenarios in parallel
nlm-cli run scene1 --output=scene1_results.txt &
nlm-cli run scene2 --output=scene2_results.txt &
nlm-cli run scene3 --output=scene3_results.txt &
wait
```

### Performance Benchmarking

The CLI includes built-in benchmarking capabilities:

```bash
nlm-cli benchmark --neurons=1000 --steps=1000
Benchmark Results:
  Neurons: 1000
  Steps/sec: 1234
  Total Time: 2.345s
```

## Advanced Features

### Configuration Templates

The CLI provides predefined configuration templates:

```bash
nlm-cli config template minimal
nlm-cli config template performance
nlm-cli config template research
```

### Batch Processing

Run multiple configurations in batch:

```bash
# Process multiple scenarios
for scenario in scene1 scene2 scene3; do
    nlm-cli run $scenario --output=$scenario.results
    echo "Completed $scenario"
done
```

### Parameter Sweeps

Run parameter variations systematically:

```bash
# Vary neuron count
for neurons in 500 1000 2000; do
    nlm-cli run movie_test --neuron_count=$neurons
    echo "Completed with $neurons neurons"
done
```

## Development Guidelines

### Adding New Commands

To add a new command to the NLM CLI:

1. **Create command class**: Inherit from `ICommand`
2. **Implement required methods**: `getName()`, `getDescription()`, `getUsage()`, `validateArguments()`, `execute()`
3. **Register command**: Add to `CommandRunner` in `NLMCLIMain.cpp`
4. **Update documentation**: Add to this documentation file

### Testing CLI

The CLI includes comprehensive tests:

- **Unit tests**: Test individual command functionality
- **Integration tests**: Test command integration with NLM components
- **Configuration tests**: Test configuration loading and validation
- **Output format tests**: Test output formatting
- **Progress reporting tests**: Test progress indicators

### Performance Considerations

- **Command validation**: Validate arguments early to avoid unnecessary processing
- **Resource management**: Properly manage system resources
- **Error handling**: Handle errors gracefully without resource leaks
- **Memory management**: Use smart pointers for automatic memory management

## Best Practices

### Configuration Management

1. **Use descriptive parameter names**: Make configuration keys self-documenting
2. **Validate user input**: Always validate user-provided configuration
3. **Use sensible defaults**: Provide reasonable default values
4. **Document parameters**: Document the purpose and expected range of each parameter

### Command Usage

1. **Provide help**: Always provide helpful help text for commands
2. **Use consistent naming**: Use consistent naming conventions for commands and options
3. **Handle errors gracefully**: Provide clear error messages and suggestions
4. **Support both short and long options**: Support `-h` and `--help`, `-v` and `--verbose`

### Output Formatting

1. **Be consistent**: Use consistent formatting across all outputs
2. **Use colors wisely**: Use colors to improve readability but not overwhelming
3. **Provide progress feedback**: Show progress for long-running operations
4. **Handle large outputs**: Handle large outputs gracefully (e.g., CSV files)

### Performance Optimization

1. **Cache frequently accessed data**: Cache configuration files and command results
2. **Use efficient algorithms**: Use efficient algorithms for command processing
3. **Minimize I/O operations**: Minimize file I/O operations when possible
4. **Use concurrent processing**: Use concurrent processing for independent operations

## Troubleshooting

### Common Issues and Solutions

1. **CLI not found or not working**
   - Check if the CLI binary is in the PATH
   - Verify build configuration
   - Check for compilation errors

2. **Configuration file issues**
   - Verify file syntax
   - Check file permissions
   - Use simpler configuration files

3. **Performance problems**
   - Reduce simulation size
   - Use smaller configurations
   - Check system resources

4. **Output format problems**
   - Check output file permissions
   - Verify output format specifications
   - Use default formats

### Getting Help

- **Command help**: `nlm-cli help <command>`
- **General help**: `nlm-cli help`
- **Version information**: `nlm-cli --version`
- **Examples**: `nlm-cli examples/cli_examples.cpp`

## Migration Guide

### From Previous Versions

If migrating from a previous version of the NLM CLI:

1. **Configuration files**: Previous configuration files may need to be converted to the new format
2. **Command syntax**: Command syntax may have changed
3. **Output format**: Output format may have changed
4. **New features**: Take advantage of new CLI features

### Example Migration

```bash
# Old way (limited functionality)
nlm-cli --config=old_config.cfg --steps=1000

# New way (enhanced functionality)
nlm-cli run simulation1 --config=old_config.cfg --steps=1000 --output=json
```

## Future Enhancements

### Planned Features

1. **Plugin architecture**: True plugin architecture for command extensibility
2. **Web interface**: Web-based CLI interface
3. **GUI integration**: Graphical user interface integration
4. **Machine learning integration**: Integration with machine learning models
5. **Cloud support**: Cloud-based execution and storage
6. **API integration**: REST API for programmatic access

### Research Areas

1. **Natural language interface**: Command-line interface using natural language
2. **AI-powered suggestions**: AI-powered command and parameter suggestions
3. **Adaptive UI**: Adaptive user interface based on user behavior
4. **Performance optimization**: Advanced performance optimization techniques

## Conclusion

The NLM CLI provides a powerful and comprehensive command-line interface for the Neural Learning Machine project. It offers:

- **Intuitive command structure** with clear help and documentation
- **Flexible configuration management** with multiple sources and validation
- **Rich command set** for all NLM operations
- **Multiple output formats** for different use cases
- **Real-time progress reporting** for long-running operations
- **Comprehensive error handling** with clear messages
- **Interactive mode** for iterative exploration
- **Tab completion** for improved user experience
- **Full integration** with existing NLM components

The CLI is designed to be:

- **User-friendly**: Easy to learn and use
- **Powerful**: Capable of handling complex operations
- **Extensible**: Easy to add new commands and features
- **Maintainable**: Clean code structure and good documentation
- **Performant**: Optimized for performance-critical operations

This documentation provides comprehensive guidance for using, extending, and maintaining the NLM CLI system.