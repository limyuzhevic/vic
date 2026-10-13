# NLM CommandLineInterface

The NLM CommandLineInterface is a comprehensive command-line interface for the Neural Learning Machine (NLM) project. It provides advanced argument parsing, simulation control, output formatting, configuration management, and much more.

## Overview

The CommandLineInterface is designed to provide:

1. **Advanced argument parsing** with help, validation, and error handling
2. **Simulation control** with pause/resume/step features
3. **Multiple output formats** (text, JSON, CSV, XML)
4. **Configuration management** with command-line to configuration mapping
5. **Simulation options** for all simulation types (Phase 2, 3, 4, 5, 6)
6. **Export/import functionality** for brain states and experiment results
7. **Performance monitoring** and reporting
8. **Visualization system integration**
9. **Logging configuration management**
10. **Experiment management** for batch execution

## Installation

To use the CommandLineInterface, include `"CommandLineInterface.hpp"` in your code and create an instance:

```cpp
#include "core/cli/CommandLineInterface.hpp"

int main(int argc, char** argv) {
    nlm::CommandLineInterface cli;
    
    if (!cli.initialize(argc, argv)) {
        std::cerr << "Failed to initialize CommandLineInterface" << std::endl;
        return 1;
    }
    
    // Process commands
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            if (!cli.runCommand(argv[i])) {
                std::cerr << "Command failed: " << argv[i] << std::endl;
            }
        }
    }
    
    cli.shutdown();
    return 0;
}
```

## Key Features

### 1. Command Processing

The CommandLineInterface provides a comprehensive set of commands:

- **help** - Display help information for available commands
- **version** - Display version information
- **config** - Manage configuration settings
- **simulate** - Start/stop simulations
- **run** - Execute commands or scripts
- **export** - Export brain states and experiment results
- **import** - Import brain states and experiment results
- **status** - Display system status
- **benchmark** - Run benchmark tests
- **performance** - Monitor performance metrics
- **logging** - Control logging configuration
- **visualization** - Control visualization
- **experiment** - Run experiments
- **batch** - Run batch experiments
- **compare** - Compare experiment results
- **pause** - Pause simulations
- **resume** - Resume simulations
- **step** - Execute a single simulation step
- **stop** - Stop simulations
- **reset** - Reset the system
- **script** - Run a script file
- **load** - Load configuration
- **save** - Save configuration
- **history** - Display command history
- **clear** - Clear error history
- **exit** - Exit the application
- **quit** - Exit the application

### 2. Interactive Mode

The CommandLineInterface supports interactive mode:

```bash
$ nlm --interactive
NLM CommandLineInterface (interactive mode)
Type 'help' for available commands or 'exit' to quit.
> help
Available commands: help, version, config, simulate, run, export, import, status, benchmark, performance, logging, visualization, experiment, batch, compare, pause, resume, step, stop, reset, script, load, save, history, clear, exit, quit
> exit
```

### 3. Scripting Mode

You can run scripts containing NLM commands:

```bash
$ nlm --script myscript.nlm
```

Script format:
```
# This is a comment
command arg1 arg2
config load config.cfg
run simulate phase2 --config simulation.cfg
export brain_state.bin --format binary
exit
```

### 4. Configuration Management

The CommandLineInterface integrates with the existing ConfigurationManager:

```bash
$ nlm --config config.cfg
$ nlm config show
$ nlm config load my_config.cfg
$ nlm config save my_config.cfg
```

### 5. Simulation Control

Start simulations with the `simulate` command:

```bash
$ nlm simulate phase2
$ nlm simulate phase3 --config phase3_config.cfg
$ nlm simulate phase4 --steps 1000 --timestep 0.001
```

Control simulations with pause/resume/step commands:

```bash
$ nlm simulate phase2 --background
$ nlm pause
$ nlm step
$ nlm resume
$ nlm stop
```

### 6. Output Formatting

The CommandLineInterface supports multiple output formats:

```bash
$ nlm export brain_state.json --format json
$ nlm export experiment_results.csv --format csv
$ nlm export model.xml --format xml
$ nlm export config.yaml --format yaml
```

### 7. Performance Monitoring

Monitor system performance:

```bash
$ nlm performance --start
$ nlm benchmark --type stress
$ nlm benchmark --type memory --config benchmark.cfg
```

### 8. Experiment Management

Run experiments:

```bash
$ nlm experiment phase2
$ nlm experiment phase3 --config experiment.cfg
$ nlm experiment phase4 --steps 500
```

Compare experiment results:

```bash
$ nlm compare experiment1 experiment2
```

### 9. Logging Control

Control logging configuration:

```bash
$ nlm logging level info
$ nlm logging add application.log
$ nlm logging remove application.log
$ nlm logging show
```

### 10. Visualization

Control visualization:

```bash
$ nlm visualization neural_net --config visualization.cfg
$ nlm visualization update data.csv
$ nlm visualization close
```

## Command Line Arguments

The CommandLineInterface supports the following command line arguments:

- `--help`, `-h` - Show help information
- `--version`, `-v` - Show version information
- `--config <file>` - Load configuration from file
- `--interactive` - Start in interactive mode
- `--script <file>` - Run script file

## File Formats

### Configuration Files

Configuration files can be in JSON, YAML, or INI format:

```json
{
  "simulation": {
    "type": "phase2",
    "steps": 1000,
    "timestep": 0.001
  },
  "logging": {
    "level": "info",
    "files": ["application.log", "debug.log"]
  }
}
```

### Export/Import Formats

- **Binary**: Efficient for large data
- **JSON**: Human-readable and machine-parseable
- **CSV**: Spreadsheet-friendly
- **XML**: Structured data format
- **YAML**: Human-readable configuration format

## Integration with Existing Code

The CommandLineInterface integrates with the existing NLM codebase:

1. **ConfigurationManager**: Used for configuration management
2. **Logger**: Used for logging
3. **NLMErrors**: Used for error handling
4. **ExperimentBase**: Used for experiment management
5. **Phase 2-6 Experiments**: Supported through the experiment command

## Advanced Usage

### Command Aliases

Create command aliases:

```bash
$ nlm alias sim simulate
$ nlm sim phase2
```

### Configuration Templates

Load configuration templates:

```bash
$ nlm config template default
$ nlm config template phase2
$ nlm config template production
```

### Real-time Monitoring

Start real-time monitoring:

```bash
$ nlm monitor start --interval 100
$ nlm monitor stop
```

### Error Recovery

Recover from errors:

```bash
$ nlm recover from_error_context
```

## Error Handling

The CommandLineInterface provides comprehensive error handling:

- Command validation
- Configuration validation
- Simulation error recovery
- File I/O error handling
- Network error handling

Errors are logged and can be accessed through the error history:

```bash
$ nlm errors
$ nlm clear errors
```

## Testing

To test the CommandLineInterface, run the demo:

```bash
$ g++ -std=c++20 demo_cli.cpp src/core/cli/CommandLineInterface.cpp -o demo_cli
$ ./demo_cli
```

## License

This code is part of the NLM project and is licensed under the MIT License.

## Dependencies

- C++20 compiler
- Standard library
- NLM core libraries
- ConfigurationManager
- Logger
- Experiment framework

## Limitations

- Some features require additional libraries to be installed
- Certain output formats may have limitations
- Visualization requires additional dependencies
- Performance monitoring may have platform-specific limitations

## Future Improvements

Future improvements may include:

- Web interface for remote control
- Plugin system for extending functionality
- Advanced scripting capabilities
- Integration with machine learning frameworks
- Cloud deployment support
