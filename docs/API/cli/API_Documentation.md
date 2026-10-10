# CLI API Documentation
# Auto-generated API Documentation for NLM CLI

## Overview

This documentation provides comprehensive API reference for the NLM Command Line Interface (CLI) framework. The CLI is built on a modular architecture that supports advanced command execution, option parsing, and help systems.

## Core Components

### Command Line Interface Framework

#### `CommandLine.hpp`
Main CLI framework that manages commands, options, and execution flow.

**Key Features**:
- Subcommand-based architecture
- Global option support
- Interactive help system
- Configuration file integration
- Environment variable support

**Classes**:

##### `CommandLine`
The main CLI interface class that manages command execution and help systems.

**Methods**:
- `void addCommand(std::unique_ptr<Command> cmd)` - Add a command to the CLI
- `bool execute(int argc, char** argv)` - Execute CLI with arguments
- `void showHelp(const std::string& commandName = "") const` - Display help
- `bool isInteractive() const` - Check if in interactive mode

##### `Command`
Base class for all CLI commands.

**Abstract Methods**:
- `virtual std::string getName() const = 0` - Get command name
- `virtual std::string getDescription() const = 0` - Get command description
- `virtual CommandCategory getCategory() const = 0` - Get command category
- `virtual bool execute(const std::vector<std::string>& args) = 0` - Execute command
- `virtual void showHelp() const = 0` - Display command help

**Concrete Methods**:
- `virtual bool validateArguments(const std::vector<std::string>& args) const` - Validate arguments
- `virtual std::string getUsage() const` - Get command usage string
- `virtual std::vector<std::string> getExamples() const` - Get command examples

#### `OptionParser.hpp`
Advanced option parsing system with comprehensive validation and error handling.

**Key Features**:
- Support for flag, string, integer, double, float, and boolean options
- Required option validation
- Value validation using custom functions
- Positional argument support
- Configuration file parsing
- Environment variable support

**Classes**:

##### `OptionParser`
Main option parser class.

**Methods**:
- `void addOption(const AdvancedOption& option)` - Add an option
- `void addFlag(const std::string& longName, char shortName, const std::string& description, bool defaultValue)` - Add a flag option
- `void addOptionWithValue(const std::string& longName, char shortName, const std::string& description, const std::string& defaultValue, bool required)` - Add an option with value
- `ParseResult parse(const std::vector<std::string>& args)` - Parse arguments
- `ParseResult parse(int argc, char** argv)` - Parse command line arguments
- `template<typename T> T getValue(const std::string& optionName, const T& defaultValue) const` - Get option value
- `bool hasOption(const std::string& optionName) const` - Check if option is set
- `bool validate() const` - Validate all options
- `std::vector<std::string> getValidationErrors() const` - Get validation errors

##### `AdvancedOption`
Represents a command line option with comprehensive metadata.

**Members**:
- `std::string longName` - Long option name (--option)
- `char shortName` - Short option name (-o)
- `std::string description` - Human-readable description
- `std::string defaultValue` - Default value
- `bool required` - Whether option is required
- `std::string value` - Parsed value
- `bool hasValue` - Whether option takes a value
- `std::vector<std::string> allowedValues` - Allowed values for the option
- `std::function<bool(const std::string&)> validator` - Custom validator function

#### `HelpSystem.hpp`
Comprehensive help system with interactive features and documentation generation.

**Key Features**:
- Interactive help mode
- Command categorization
- Examples and tips
- Search functionality
- Documentation generation (HTML, man pages, text)
- Theme support
- Text wrapping and formatting

**Classes**:

##### `HelpSystem`
Main help system class.

**Methods**:
- `void setProgramInfo(const std::string& name, const std::string& version, const std::string& description = "", const std::string& copyright = "", const std::string& website = "")` - Set program information
- `void addGlobalOption(const std::string& option)` - Add a global option
- `void addTip(const std::string& tip)` - Add a helpful tip
- `void addExample(const std::string& example)` - Add an example
- `void registerCommand(const std::string& name, const std::string& description, const std::string& usage, CommandCategory category)` - Register a command
- `void showHelp(const std::string& commandName = "", bool interactive = false)` - Show help
- `void showCommandHelp(const std::string& commandName)` - Show command-specific help
- `void startInteractiveHelp()` - Start interactive help mode
- `bool saveHelpToFile(const std::string& filePath) const` - Save help to file
- `std::vector<std::string> searchCommands(const std::string& query) const` - Search for commands
- `std::vector<std::string> getCommandSuggestions(const std::string& partialCommand) const` - Get command suggestions

##### `GlobalHelpInfo`
Holds global help information for the entire program.

**Members**:
- `std::string programName` - Program name
- `std::string version` - Program version
- `std::string description` - Program description
- `std::string usage` - Usage string
- `std::string copyright` - Copyright information
- `std::string website` - Website URL
- `std::vector<std::string> globalOptions` - Global options
- `std::vector<std::string> tips` - Helpful tips
- `std::vector<std::string> examples` - Usage examples

##### `CommandHelpInfo`
Holds detailed help information for a specific command.

**Members**:
- `std::string name` - Command name
- `std::string description` - Command description
- `std::string usage` - Usage string
- `std::string detailedDescription` - Detailed description
- `CommandCategory category` - Command category
- `std::vector<std::string> examples` - Usage examples
- `std::vector<std::string> options` - Available options
- `std::vector<std::string> notes` - Additional notes
- `std::string seeAlso` - Related commands

### Configuration Management

#### `ConfigManager`
Configuration file and environment variable management system.

**Key Features**:
- Load configuration from files
- Load configuration from environment variables
- Merge configuration from multiple sources
- Type-safe value access with defaults
- Validation and error handling

**Methods**:
- `bool loadFromFile(const std::string& file)` - Load configuration from file
- `bool saveToFile(const std::string& file) const` - Save configuration to file
- `template<typename T> T getValue(const std::string& key, const T& defaultValue) const` - Get configuration value
- `void setValue(const std::string& key, const std::variant<std::string, int64_t, double, bool>& value)` - Set configuration value
- `void mergeWithCommandLine(const std::vector<std::string>& args)` - Merge with command line arguments
- `void loadFromEnvironment()` - Load from environment variables

#### `ArgumentMerger`
Merges configuration from files, environment variables, and command line arguments.

**Key Features**:
- Precedence resolution (command line > environment > file)
- Comprehensive error reporting
- Type-safe value access
- Support for all option types

**Methods**:
- `bool merge(int argc, char** argv)` - Merge all configuration sources
- `template<typename T> T getFinalValue(const std::string& optionName, const T& defaultValue) const` - Get merged value
- `bool hasOption(const std::string& optionName) const` - Check if option is set
- `std::string getErrors() const` - Get all errors from merging process

### Utility Classes

#### `ProgressIndicator`
Visual progress indicator for long-running operations.

**Key Features**:
- Configurable progress display
- Indeterminate mode support
- Progress calculation and display
- Integration with status reporter

**Methods**:
- `void start(const std::string& msg, int total, bool indeterminate = false)` - Start progress indicator
- `void update(int step)` - Update progress
- `void update()` - Update progress (increment step)
- `void complete()` - Complete progress indicator
- `void disable()` - Disable progress indicator

#### `StatusReporter`
Centralized status reporting with different log levels.

**Key Features**:
- Multiple log levels (info, warning, error, success, progress)
- Global configuration support
- Colored output support
- Integration with progress indicators

**Static Methods**:
- `static void info(const std::string& message)` - Report info message
- `static void warning(const std::string& message)` - Report warning
- `static void error(const std::string& message)` - Report error
- `static void success(const std::string& message)` - Report success
- `static void progress(const std::string& message)` - Report progress

#### `Environment`
Environment variable access and manipulation.

**Key Features**:
- Type-safe environment variable access
- Support for string, integer, double, and boolean values
- List support with custom separators
- Cross-platform compatibility

**Static Methods**:
- `static std::string getVar(const std::string& name, const std::string& defaultValue = "")` - Get string environment variable
- `static int getVar(const std::string& name, int defaultValue)` - Get integer environment variable
- `static double getVar(const std::string& name, double defaultValue)` - Get double environment variable
- `static bool getVar(const std::string& name, bool defaultValue)` - Get boolean environment variable
- `static std::vector<std::string> getVarList(const std::string& name, char separator = ',')` - Get list environment variable

#### `CommandLineHelp`
Integration between CLI and help system with documentation generation.

**Key Features**:
- Automatic help system integration
- Documentation generation (HTML, man pages, text)
- API documentation generation
- Examples documentation generation
- Configuration documentation generation

**Methods**:
- `bool autoGenerateHelp() const` - Generate all documentation
- `void integrateWithCommandLine(CommandLine& cli) const` - Integrate with CLI
- `bool generateHtmlHelp(const std::string& outputPath) const` - Generate HTML help
- `bool generateManPage(const std::string& outputPath) const` - Generate man page
- `bool generateTextHelp(const std::string& outputPath) const` - Generate text help

### Command Implementations

#### `SimulationCommand`
Implements the `simulation` command for running neural simulations.

**Features**:
- Advanced simulation parameter management
- Progress monitoring and reporting
- Output file generation in multiple formats
- Configuration file integration
- Performance profiling support

#### `ConfigCommand`
Implements the `config` command for configuration management.

**Features**:
- Configuration file listing and viewing
- Configuration file loading and validation
- Configuration file export
- Environment variable management

#### `DebugCommand`
Implements the `debug` command for debugging and profiling.

**Features**:
- Debugger attachment
- Performance profiling
- System validation
- Memory debugging
- Thread debugging

#### `ValidateCommand`
Implements the `validate` command for system validation.

**Features**:
- Memory validation
- Thread safety validation
- Configuration validation
- Plugin validation

#### `PluginCommand`
Implements the `plugin` command for plugin management.

**Features**:
- Plugin listing and discovery
- Plugin loading and unloading
- Plugin enabling and disabling
- Plugin information display

#### `ProfileCommand`
Implements the `profile` command for performance profiling.

**Features**:
- Benchmarking
- Profiling
- Analysis
- Output generation in multiple formats

#### `BenchmarkCommand`
Implements the `benchmark` command for performance benchmarking.

**Features**:
- Configurable benchmark scenarios
- Multi-threaded benchmarking
- Results output in multiple formats

#### `AnalyzeCommand`
Implements the `analyze` command for system analysis.

**Features**:
- Performance analysis
- Memory analysis
- Network analysis
- Custom analysis types

#### `UtilityCommand`
Implements the `utility` command for miscellaneous utilities.

**Features**:
- Format conversion
- Validation utilities
- Format information

## Command Categories

### `CommandCategory` Enum

```cpp
enum class CommandCategory {
    SIMULATION,      // Run neural simulations
    CONFIGURATION,   // Manage configuration
    DEBUGGING,       // Debug tools
    VALIDATION,      // System validation
    PLUGIN,          // Plugin management
    PERFORMANCE,     // Performance analysis
    ANALYSIS,       // System analysis
    UTILITIES        // Utility commands
};
```

### Category Descriptions

- **SIMULATION**: Commands for running neural simulations with configurable parameters
- **CONFIGURATION**: Commands for managing configuration files and settings
- **DEBUGGING**: Commands for debugging and profiling neural simulations
- **VALIDATION**: Commands for system validation and testing
- **PLUGIN**: Commands for plugin management and discovery
- **PERFORMANCE**: Commands for performance analysis and benchmarking
- **ANALYSIS**: Commands for system analysis and reporting
- **UTILITIES**: Commands for miscellaneous utility functions

## File Structure

### Include Files

```
src/cli/
├── CommandLine.hpp
├── Command.hpp
├── OptionParser.hpp
├── HelpSystem.hpp
└── AdvancedOptions.hpp
```

### Source Files

```
src/cli/
├── CommandLine.cpp
├── Command.cpp
├── OptionParser.cpp
├── HelpSystem.cpp
├── ConfigManager.cpp
└── Environment.cpp
```

### Examples

```
examples/cli/
├── basic_examples.sh
├── advanced_examples.sh
└── batch_scripts/
    ├── benchmark_suite.sh
    ├── debug_session.sh
    ├── plugin_test.sh
    └── config_workflow.sh
```

### Documentation

```
docs/cli/
├── CLI_Documentation.md
├── API/
│   ├── cli/
│   │   ├── CommandLine.md
│   │   ├── Command.md
│   │   ├── OptionParser.md
│   │   └── HelpSystem.md
│   └── configuration.md
└── examples.md
```

## Building the CLI Framework

### Prerequisites

- C++20 compiler
- CMake 3.16 or higher

### Build Instructions

```bash
mkdir build
cd build
cmake ..
make
```

### Installation

```bash
make install
```

### Running Examples

```bash
# Run basic examples
./examples/cli/basic_examples.sh

# Run advanced examples
./examples/cli/advanced_examples.sh

# Generate documentation
nlm help --generate-docs
```

## Configuration

### Configuration File Format

```ini
# Configuration file example
neuron_count = 500
max_steps = 1000
timestep = 0.001
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
output_format = "json"
enable_profiling = true
```

### Environment Variables

```bash
export NLM_LOG_LEVEL=info
export NLM_VERBOSE=true
export NLM_CONFIG_FILE=configs/myconfig.cfg
export NLM_THREADS=4
export NLM_OUTPUT_FORMAT=json
```

## Usage Examples

### Basic Commands

```bash
# Run a simulation
nlm simulation --neuron-count 1000 --max-steps 500

# Run a simulation with output
nlm simulation --neuron-count 2000 --max-steps 1000 --output-file results.json --output-format json

# Run a simulation with profiling
nlm simulation --neuron-count 1000 --max-steps 1000 --enable-profiling --enable-validation
```

### Advanced Commands

```bash
# Run a comprehensive benchmark
nlm benchmark --neuron-count 1000 --steps 500 --output-file results.csv

# Run a detailed profile
nlm profile --benchmark --output-file perf.json

# Analyze system performance
nlm analyze --type performance --output-format json

# Validate system state
nlm validate --memory --threads --config --plugins
```

### Configuration Management

```bash
# List current configuration
nlm config --list

# Load configuration from file
nlm config --config-file myconfig.cfg --validate

# Export configuration
nlm config --output-file exported.cfg
```

### Help and Discovery

```bash
# Show general help
nlm help

# Show command-specific help
nlm help simulation
nlm simulation --help

# Search for commands
nlm help --search "simulation"

# List commands by category
nlm help --category simulation
```

## Error Handling

### Common Errors

#### Parse Errors
- Invalid option names
- Missing required option values
- Invalid option formats

#### Validation Errors
- Invalid option values
- Configuration file errors
- Plugin loading errors

#### Execution Errors
- Simulation errors
- File I/O errors
- Memory allocation errors

### Error Reporting

The CLI framework provides comprehensive error reporting with:

- Clear error messages
- Suggestions for corrections
- Context information
- Exit codes for scripting

## Testing

### Unit Tests

Run the unit tests with:

```bash
cd tests
./run_tests.sh
```

### Integration Tests

Run integration tests with:

```bash
make test
```

### Performance Tests

Run performance tests with:

```bash
nlm benchmark --performance
```

## Migration Guide

### From main.cpp to Enhanced CLI

The enhanced CLI framework replaces the basic CLI in `main.cpp` with a more sophisticated architecture. To migrate:

1. Replace `main.cpp` with the enhanced CLI implementation
2. Add command implementations as needed
3. Configure the help system
4. Set up configuration files
5. Test all commands

### Command Migration Examples

| Old Command | New Command | Notes |
|-------------|-------------|-------|
| Basic simulation | `nlm simulation` | Enhanced with many more options |
| Configuration loading | `nlm config --config-file file.cfg` | More flexible configuration |
| Debugging | `nlm debug` | Comprehensive debugging tools |
| Validation | `nlm validate` | System validation |
| Plugin management | `nlm plugin` | Advanced plugin system |
| Profiling | `nlm profile` | Performance profiling |
| Benchmarking | `nlm benchmark` | Performance benchmarking |
| Analysis | `nlm analyze` | System analysis |

## Future Enhancements

### Planned Features

1. **Web Interface**: Web-based CLI interface
2. **GUI Framework**: Graphical user interface
3. **Remote Control**: Remote CLI control
4. **Plugin Architecture**: Extensible plugin system
5. **Configuration Templates**: Predefined configuration templates
6. **Command Scripts**: Support for command scripts
7. **Integration APIs**: Integration with external tools
8. **Documentation Generation**: Automatic documentation generation

### Research Directions

- Interactive command completion
- Natural language command parsing
- Machine learning for command suggestions
- Distributed simulation execution
- Real-time monitoring and visualization

## References

### Related Documents

- `docs/ARCHITECTURE.md` - Overall NLM architecture
- `docs/SCIENCE.md` - Scientific background
- `docs/EXPERIMENTS.md` - Experimental validation
- `examples/demo/` - Demonstration examples

### External Resources

- [GitHub Repository](https://github.com/nlm/nlm) - NLM GitHub repository
- [Documentation](https://nlm.github.io/docs) - NLM documentation
- [API Reference](https://nlm.github.io/api) - API documentation

## Copyright and License

Copyright (c) 2026 NLM Team

This documentation is provided under the same license as the NLM project.
