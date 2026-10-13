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

## Files

### Core Components

- `src/core/cli/CommandLineInterface.hpp` - Main header file with the CommandLineInterface class
- `src/core/cli/CommandLineInterface.cpp` - Implementation file
- `src/core/cli/CommandLineInterface::Impl` - Internal implementation details

### Demo Programs

- `src/demo_cli_main.cpp` - Demo program showing CommandLineInterface usage
- `demo_cli` - Compiled demo executable

### Documentation

- `docs/CLI_USER_GUIDE.md` - Comprehensive user guide

### Examples

- `examples/` - Example usage and configuration files

## Building

### Using CMake

The project uses CMake for building. To build the CommandLineInterface:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Individual Components

You can build individual components:

```bash
# Build just the CommandLineInterface library
cmake --build . --target nlm_cli

# Build the demo program
cmake --build . --target demo_cli

# Build the main NLM program
cmake --build . --target nlm
```

## Usage

### Quick Start

1. **Include the header**:

```cpp
#include "core/cli/CommandLineInterface.hpp"
```

2. **Create an instance**:

```cpp
nlm::CommandLineInterface cli;
```

3. **Initialize**:

```cpp
if (!cli.initialize(argc, argv)) {
    // Handle initialization error
}
```

4. **Process commands**:

```cpp
if (!cli.runCommand("--help")) {
    // Handle command error
}
```

5. **Shutdown**:

```cpp
cli.shutdown();
```

### Command Line Arguments

The CommandLineInterface supports the following command line arguments:

- `--help`, `-h` - Show help information
- `--version`, `-v` - Show version information
- `--config <file>` - Load configuration from file
- `--interactive` - Start in interactive mode
- `--script <file>` - Run script file

### Examples

#### Basic Usage

```cpp
#include <iostream>
#include "core/cli/CommandLineInterface.hpp"

int main(int argc, char** argv) {
    nlm::CommandLineInterface cli;
    
    if (!cli.initialize(argc, argv)) {
        std::cerr << "Failed to initialize CommandLineInterface" << std::endl;
        return 1;
    }
    
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            if (!cli.runCommand(argv[i])) {
                std::cerr << "Command failed: " << argv[i] << std::endl;
            }
        }
    } else {
        std::cout << cli.getHelp() << std::endl;
    }
    
    cli.shutdown();n
    return 0;
}
```

#### Interactive Mode

```bash
$ ./nlm --interactive
NLM CommandLineInterface (interactive mode)
Type 'help' for available commands or 'exit' to quit.
> help
Available commands: help, version, config, simulate, run, export, import, status, benchmark, performance, logging, visualization, experiment, batch, compare, pause, resume, step, stop, reset, script, load, save, history, clear, exit, quit
> simulate phase2 --config config.cfg
Simulation started...
> pause
Simulation paused
> resume
Simulation resumed
> step
Simulation step completed
> stop
Simulation stopped
> exit
```

#### Script Mode

Create a script file `commands.nlm`:

```
# NLM CommandLineInterface script
# This script demonstrates various commands
config load default.cfg
simulate phase2 --steps 1000
export brain_state.json --format json
benchmark --type stress
experiment phase3 --config exp3.cfg
exit
```

Run the script:

```bash
$ ./nlm --script commands.nlm
```

#### Configuration Management

Load and save configuration files:

```bash
$ ./nlm --config config.cfg
simulate phase2 --config config.cfg
config show
simulate phase4 --steps 2000 --timestep 0.001
```

#### Output Formatting

Export results in different formats:

```bash
$ nlm export brain_state.json --format json
$ nlm export experiment_results.csv --format csv
$ nlm export model.xml --format xml
$ nlm export config.yaml --format yaml
```

#### Performance Monitoring

Monitor system performance:

```bash
$ nlm performance --start
$ nlm benchmark --type stress
$ nlm benchmark --type memory --config benchmark.cfg
```

#### Experiment Management

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

#### Logging Control

Control logging configuration:

```bash
$ nlm logging level info
$ nlm logging add application.log
$ nlm logging remove application.log
$ nlm logging show
```

#### Visualization

Control visualization:

```bash
$ nlm visualization neural_net --config visualization.cfg
$ nlm visualization update data.csv
$ nlm visualization close
```

## Command Reference

### Core Commands

#### help
Display help information for available commands or specific commands.

**Usage:**
```bash
$ nlm help
$ nlm help <command>
```

#### version
Display version information.

**Usage:**
```bash
$ nlm version
```

#### config
Manage configuration settings.

**Usage:**
```bash
$ nlm config show
$ nlm config load <file>
$ nlm config save <file>
```

#### simulate
Start/stop simulations.

**Usage:**
```bash
$ nlm simulate <type> [--config <file>] [--steps <count>] [--timestep <time>]
```

#### run
Execute commands or scripts.

**Usage:**
```bash
$ nlm run <command> [<args>...]
$ nlm run script <file>
```

#### export
Export brain states and experiment results.

**Usage:**
```bash
$ nlm export <source> --format <format> --output <file>
```

#### import
Import brain states and experiment results.

**Usage:**
```bash
$ nlm import <file> --format <format>
```

#### status
Display system status.

**Usage:**
```bash
$ nlm status
```

#### benchmark
Run benchmark tests.

**Usage:**
```bash
$ nlm benchmark [--type <type>] [--config <file>]
```

#### performance
Monitor performance metrics.

**Usage:**
```bash
$ nlm performance [--start] [--stop] [--report]
```

#### logging
Control logging configuration.

**Usage:**
```bash
$ nlm logging level <level>
$ nlm logging add <file>
$ nlm logging remove <file>
$ nlm logging show
```

#### visualization
Control visualization.

**Usage:**
```bash
$ nlm visualization <type> [--config <file>]
$ nlm visualization update <data>
$ nlm visualization close
```

#### experiment
Run experiments.

**Usage:**
```bash
$ nlm experiment <type> [--config <file>] [--steps <count>]
```

#### batch
Run batch experiments.

**Usage:**
```bash
$ nlm batch <file>
```

#### compare
Compare experiment results.

**Usage:**
```bash
$ nlm compare <experiment1> <experiment2>
```

#### pause, resume, step, stop
Control simulation execution.

**Usage:**
```bash
$ nlm pause
$ nlm resume
$ nlm step
$ nlm stop
```

#### reset
Reset the system.

**Usage:**
```bash
$ nlm reset
```

#### script
Run a script file.

**Usage:**
```bash
$ nlm script <file>
```

#### load, save
Load/save configuration.

**Usage:**
```bash
$ nlm load <file>
$ nlm save <file>
```

#### history
Display command history.

**Usage:**
```bash
$ nlm history
```

#### clear
Clear error history.

**Usage:**
```bash
$ nlm clear errors
```

#### exit, quit
Exit the application.

**Usage:**
```bash
$ nlm exit
$ nlm quit
```

## Configuration Files

Configuration files can be in JSON, YAML, or INI format. Examples:

### JSON Configuration

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
  },
  "experiment": {
    "type": "phase3",
    "config": "experiment.cfg"
  }
}
```

### YAML Configuration

```yaml
simulation:
  type: phase2
  steps: 1000
  timestep: 0.001

logging:
  level: info
  files:
    - application.log
    - debug.log

experiment:
  type: phase3
  config: experiment.cfg
```

### INI Configuration

```ini
[simulation]
type = phase2
steps = 1000
timestep = 0.001

[logging]
level = info
files = application.log,debug.log

[experiment]
type = phase3
config = experiment.cfg
```

## Output Formats

The CommandLineInterface supports multiple output formats:

### Text
Human-readable text output.

### JSON
Machine-readable JSON output.

### CSV
Spreadsheet-compatible CSV output.

### XML
Structured XML output.

### YAML
Human-readable YAML output.

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

### Running Tests

To run tests:

```bash
mkdir build
cd build
cmake ..
cmake --build . --target test
```

### Demo Program

To run the demo program:

```bash
cmake --build . --target demo_cli
./demo_cli
```

## Dependencies

### Required Libraries

- C++20 compiler
- Standard library
- CMake 3.16 or higher

### NLM Libraries

- ConfigurationManager
- Logger
- NLMErrors
- Experiment framework
- Phase 2-6 experiments

### Optional Dependencies

- Visualization libraries (for visualization support)
- Network libraries (for remote control)
- Database libraries (for result storage)

## Building with Dependencies

To build with additional dependencies:

```bash
mkdir build
cd build
cmake -DUSE_VISUALIZATION=ON -DUSE_NETWORK=ON -DUSE_DATABASE=ON ..
cmake --build .
```

## License

This code is part of the NLM project and is licensed under the MIT License.

## Contributing

Contributions are welcome! Please follow the project's contribution guidelines.

## Support

For support, please refer to the NLM documentation or contact the development team.

## Changelog

### Version 1.0.0
- Initial release
- Comprehensive command-line interface
- Advanced argument parsing
- Simulation control
- Output formatting
- Configuration management
- Experiment management

## Future Improvements

### Upcoming Features

- Web interface for remote control
- Plugin system for extending functionality
- Advanced scripting capabilities
- Integration with machine learning frameworks
- Cloud deployment support

### Known Issues

- Some visualization features require additional dependencies
- Network features may have platform-specific limitations
- Performance monitoring may have accuracy limitations

## Contact

For questions, issues, or feature requests, please refer to the NLM project documentation.
