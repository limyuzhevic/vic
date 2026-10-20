# NLM Command-Line Interface

## Overview

The NLM (Neural Learning Machine) command-line interface has been significantly enhanced to provide:

- **Argparse-Style Interface**: Modern command-line argument parsing
- **Configuration Override**: Command-line override of configuration values
- **Help System**: Comprehensive help documentation
- **Validation**: Configuration validation via CLI
- **Flexibility**: Multiple argument formats and options
- **Integration**: Seamless integration with configuration system
- **Error Handling**: Detailed error messages and validation
- **Documentation**: Built-in help and version information

## Argument Parsing Style

The NLM CLI uses a modern, argparse-style interface with the following principles:

### Positional Arguments

```bash
./nlm [OPTIONS] [CONFIG_FILE]
```

### Long Options

```bash
--key=value              Set configuration key-value pair
--key value              Set configuration key-value pair (alternative format)
--help                   Show help message and exit
--version                Show version information and exit
```

### Short Options

```bash
-h, --help               Show help message and exit
-v, --verbose            Enable verbose output
-c, --config FILE        Specify configuration file
```

## Configuration File Detection

The CLI automatically detects and loads configuration files:

### Automatic Detection

```bash
# Positional argument as config file
./nlm custom.cfg

# Explicit option
./nlm --config config.json

# With other options
./nlm --verbose --config production.cfg
```

### File Format Detection

```bash
# JSON configuration
./nlm --config simulation.json

# YAML configuration
./nlm --config settings.yaml

# Key=value format
./nlm --config parameters.cfg
```

## Configuration Override Priority

1. **Command-line arguments** (highest priority)
2. **Configuration file** (medium priority)
3. **Default values** (lowest priority)

```bash
# Command-line overrides everything
./nlm --neuron_count=5000

# Config file overrides defaults
./nlm --config myconfig.cfg --neuron_count=5000

# Command-line and config file merged
./nlm --config base.cfg --neuron_count=5000 --region_count=3
```

## Command-Line Options

### Help and Version

```bash
./nlm --help
./nlm -h
./nlm --version
```

### Configuration Options

```bash
./nlm --config FILE|-c FILE     Specify configuration file
custom.cfg                     (positional config file)
```

### Simulation Control

```bash
./nlm --steps N                 Set maximum simulation steps
./nlm --time-limit SECONDS      Set time limit in seconds
```

### Logging and Output

```bash
./nlm --verbose|-v              Enable verbose output
./nlm --json-logging            Enable JSON logging format
./nlm --no-colors               Disable colored output
./nlm --log-file FILE           Set log output file
```

### Validation

```bash
./nlm --validate-only          Validate configuration and exit
```

### Configuration Key-Value Pairs

#### Long Format

```bash
./nlm --neuron_count=2000
./nlm --connection_probability=0.2
./nlm --random_seed=12345
```

#### Short Format

```bash
./nlm -n 2000
./nlm -c 0.2
./nlm -r 12345
```

#### Mixed Format

```bash
./nlm --verbose --config config.cfg --neuron_count=3000 --stdp_tau=25.0
```

## Argument Parsing Examples

### Basic Usage

```bash
# Basic simulation
./nlm --config default.cfg

# With verbose output
./nlm --verbose --config custom.cfg

# JSON logging
./nlm --json-logging --config settings.json
```

### Configuration Override

```bash
# Override configuration values
./nlm --neuron_count=5000 --region_count=3 --stdp_ltp_weight=0.03

# Mix config file and command-line
./nlm --config base.cfg --neuron_count=5000 --region_count=3

# Use only command-line
./nlm -n 5000 -r 3
```

### Advanced Usage

```bash
# Validation only
./nlm --validate-only --config production.cfg

# Custom configuration
./nlm --neuron_count=2000 --connection_probability=0.15 --random_seed=42

# Performance tuning
./nlm --steps 10000 --time-limit 60.0 --verbose

# Multiple configuration files
./nlm --config base.cfg --config overrides.cfg

# Disabled features
./nlm --visualization_enabled=false --log_to_file=true
```

## Argument Parser Implementation

### Class: CLIConfig

```cpp
struct CLIConfig {
    std::string configFile = "configs/default.cfg";
    bool verbose = false;
    bool jsonLogging = false;
    bool validateOnly = false;
    int simulationSteps = 0;
    double timeLimit = 0.0;
    std::vector<std::string> customSettings;
    
    bool parseArgs(int argc, char** argv);
    void printHelp() const;
    void printVersion() const;
    bool validateConfiguration(const nlm::Config& config) const;
};
```

### Argument Format Support

The CLI supports multiple argument formats:

#### Long Format (--key=value)
```cpp
if (arg.substr(0, 2) == "--") {
    size_t pos = arg.find('=');
    if (pos != std::string::npos) {
        std::string key = arg.substr(2, pos - 2);
        std::string value = arg.substr(pos + 1);
        // Process key-value pair
    }
}
```

#### Short Format (-k value)
```cpp
else if (arg[0] == '-' && arg.size() > 1 && std::isalpha(arg[1])) {
    std::string key = arg.substr(1);
    if (i + 1 < argc && argv[i + 1][0] != '-') {
        std::string value = argv[++i];
        // Process key-value pair
    }
}
```

#### Positional Arguments
```cpp
else if (!configFile.empty() && key.empty()) {
    // Treat as config file
    configFile = arg;
}
```

## Configuration Value Parsing

### Type Conversion

The CLI automatically converts string values to appropriate types:

```cpp
// Boolean values
"true" -> true
"false" -> false
"1" -> true (partial)
"0" -> false (partial)

// Integer values
"123" -> 123
"0x1F" -> 31 (hex, if supported)
"1.5e2" -> 150 (double, if not int)

// String values (with or without quotes)
"hello" -> "hello"
"'world'" -> "world"
'"test"' -> "test"
```

### Error Handling

```cpp
// Unknown flags
./nlm --unknown-option
Error: Unknown flag: --unknown-option

// Missing values
./nlm --neuron_count
Error: Missing value for --neuron_count

// Invalid configuration
./nlm --neuron_count=abc
Error: Invalid configuration value for --neuron_count
```

## Help System

### Help Message Format

```bash
Usage: nlm [OPTIONS] [CONFIG_FILE]

Options:
  -h, --help                    Show this help message and exit
  --version                     Show version information and exit
  -v, --verbose                 Enable verbose output
  --json-logging               Enable JSON logging format
  --validate-only              Validate configuration and exit
  -c, --config FILE            Specify configuration file (default: configs/default.cfg)
  --steps N                    Set maximum simulation steps
  --time-limit SECONDS         Set time limit in seconds
  --key=value                  Set configuration key-value pair
  -k value                     Set configuration key-value pair (shorthand)

Examples:
  nlm --config custom.cfg --verbose
  nlm --steps 5000 --time-limit 10.0
  nlm --neuron_count=2000 --connection_probability=0.2
  nlm -c config.json --json-logging
```

### Version Information

```bash
$ ./nlm --version
NLM - Neural Learning Machine
Version: 0.1.0 (Phase 2)
Built with C++20 support
Author: NLM Research Team
```

## Validation System

### Configuration Validation

The CLI provides configuration validation via:

```bash
./nlm --validate-only --config myconfig.cfg
```

### Validation Rules

```cpp
bool validateConfiguration(const Config& config) {
    // Check for required configuration keys
    std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count",
        "connection_probability"
    };
    
    // Validate numeric ranges
    if (config.has("neuron_count")) {
        auto count = config.getOr<int64_t>("neuron_count", 0);
        if (count <= 0 || count > 100000) {
            std::cerr << "ERROR: neuron_count must be between 1 and 100000" << std::endl;
            return false;
        }
    }
    
    // Additional validation rules...
}
```

## Integration with Configuration System

### Configuration Loading

```cpp
int main(int argc, char** argv) {
    // Parse command-line arguments
    CLIConfig cliConfig;
    if (!cliConfig.parseArgs(argc, argv)) {
        return 1;
    }
    
    // Load configuration file
    auto config = std::make_shared<nlm::Config>();
    if (cliConfig.configFile != "configs/default.cfg" || 
        std::find(argv, argv + argc, "--verbose") != argv + argc) {
        
        if (!config->loadFromFile(cliConfig.configFile)) {
            std::cerr << "Warning: Could not load configuration from " 
                      << cliConfig.configFile << std::endl;
        }
    }
    
    // Override with command-line arguments
    config->loadFromArgs(argc, argv);
    
    // Apply custom settings from CLI
    for (const auto& setting : cliConfig.customSettings) {
        size_t pos = setting.find('=');
        if (pos != std::string::npos) {
            std::string key = setting.substr(0, pos);
            std::string value = setting.substr(pos + 1);
            
            // Parse and set configuration value
            config->set(key, value, nlm::ConfigSource::CommandLine);
        }
    }
    
    // Validate configuration if requested
    if (cliConfig.validateOnly) {
        if (!validateConfiguration(*config)) {
            std::cerr << "Configuration validation failed." << std::endl;
            return 1;
        }
        return 0;
    }
    
    // Continue with application logic...
}
```

## Performance Considerations

### Argument Parsing

- **Time Complexity**: O(n) where n is number of arguments
- **Memory Usage**: Proportional to number of arguments
- **Validation**: Early validation for invalid arguments

### Configuration Loading

- **File I/O**: Asynchronous file reading for large configurations
- **Memory**: Configuration values stored in memory
- **Caching**: Configuration values cached for fast access

## Error Handling

### Command-Line Errors

```cpp
// Invalid argument format
if (arg.substr(0, 2) == "--") {
    // Parse long format
} else if (arg[0] == '-' && std::isalpha(arg[1])) {
    // Parse short format
} else {
    // Positional argument or unknown
    std::cerr << "Error: Invalid argument: " << arg << std::endl;
    return 1;
}
```

### File Not Found

```bash
./nlm --config nonexistent.json
Error: Could not open configuration file: nonexistent.json
```

## Testing and Validation

### Unit Testing

```cpp
void testCLIConfigParsing() {
    CLIConfig cliConfig;
    
    // Test long format
    char* argv[] = { (char*)"nlm", (char*)"--neuron_count=5000", (char*)"--verbose" };
    int argc = 3;
    assert(cliConfig.parseArgs(argc, argv));
    assert(cliConfig.customSettings.size() == 1);
    assert(cliConfig.customSettings[0] == "neuron_count=5000");
    assert(cliConfig.verbose == true);
}

void testHelpSystem() {
    CLIConfig cliConfig;
    char* argv[] = { (char*)"nlm", (char*)"--help" };
    int argc = 2;
    assert(cliConfig.parseArgs(argc, argv));
}
```

### Integration Testing

```cpp
void testCLIWithConfig() {
    CLIConfig cliConfig;
    char* argv[] = { (char*)"nlm", (char*)"--config", (char*)"test.cfg", 
                    (char*)"--neuron_count=2000" };
    int argc = 4;
    assert(cliConfig.parseArgs(argc, argv));
    
    // Load configuration
    auto config = std::make_shared<nlm::Config>();
    config->loadFromFile(cliConfig.configFile);
    config->loadFromArgs(argc, argv);
    
    assert(config->getOr<int64_t>("neuron_count", 0) == 2000);
}
```

## Migration Guide

### From Old to New CLI

1. **Update argument format**: Use `--key=value` instead of `-k value`
2. **Add new options**: Use `--verbose`, `--json-logging`
3. **Configuration loading**: Use `--config` option
4. **Validation**: Add `--validate-only` for validation
5. **Help system**: Use `--help` for documentation

### Backward Compatibility

The new CLI maintains backward compatibility:

```bash
# Old format still works
./nlm -n 2000 -v

# New format recommended
./nlm --neuron_count=2000 --verbose
```

## Limitations

1. **Argument Parsing**: Limited to simple key-value pairs
2. **File Format**: Detects based on extension, not content
3. **Validation**: Requires schema definition
4. **Error Messages**: Basic error reporting
5. **Performance**: O(n) parsing for large argument lists

## Future Enhancements

1. **Subcommands**: Support for `nlm config`, `nlm run`, etc.
2. **Argument Completion**: Tab completion for options
3. **Configuration Templates**: Built-in configuration templates
4. **Dynamic Validation**: Runtime validation rules
5. **Configuration History**: Command history and logging

## Examples

### Basic Usage

```bash
# Run with default configuration
./nlm

# Load specific configuration
./nlm --config production.cfg

# Override configuration values
./nlm --neuron_count=5000 --connection_probability=0.2

# Enable verbose output
./nlm --verbose
```

### Advanced Usage

```bash
# Full control over simulation
./nlm --config config.json --steps 10000 --time-limit 60.0 --json-logging

# Development mode
./nlm --verbose --validate-only --config development.cfg

# Production mode
./nlm --config production.cfg --neuron_count=10000
```

## References

- [argparse Documentation](https://docs.python.org/3/library/argparse.html)
- [GetOpt Documentation](https://www.gnu.org/software/getopt/)
- [Command Line Argument Standards](https://www.gnu.org/software/bash/manual/html_node/Arguments.html)

---
*This documentation is auto-generated from the NLM source code comments and CLI implementation.*
*Last updated: 2026-10-20*
*Version: 0.1.0*
*Author: NLM Research Team*