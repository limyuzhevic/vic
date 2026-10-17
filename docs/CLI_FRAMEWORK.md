# NLM CLI Framework

This document describes the NLM Command Line Interface (CLI) framework, a comprehensive solution for modern command-line argument parsing and unified interface management across all NLM executables.

## Overview

The NLM CLI framework provides:
- Modern argument parsing with support for short and long options
- Multiple execution modes (demo, benchmark, script, test, validate)
- Configuration file support with automatic format detection
- Comprehensive help and documentation
- Backward compatibility with existing `--config` flag
- Logging configuration and management

## Architecture

```
NLM CLI Framework
├── CLIParser
│   ├── parse(int argc, char** argv, CLIConfig& config)
│   ├── printHelp()
│   ├── printVersion()
│   ├── validate()
│   └── command utilities
│
├── CLIFactory
│   ├── createParser()
│   ├── createConfig()
│   ├── createLogger()
│   ├── executeCommand()
│   └── loadConfiguration()
│
├── CLIAdapter
│   ├── adaptMain()
│   ├── adaptPhase3Demo()
│   ├── adaptPhase4Demo()
│   ├── adaptPhase6Demo()
│   └── adaptBenchmark()
│
└── CLIMacros
    ├── helper functions
    ├── constants
    └── utility functions
```

## Configuration

### CLIConfig Structure

The `CLIConfig` structure contains all command-line configuration options:

```cpp
struct CLIConfig {
    // General options
    bool help = false;
    bool version = false;
    bool dry_run = false;
    std::string config_file;
    std::string log_level;
    std::string log_file;
    bool quiet = false;
    bool verbose = false;
    
    // Command
    std::string command;
    std::vector<std::string> command_args;
    
    // Mode flags
    bool benchmark_mode = false;
    bool script_mode = false;
    bool test_mode = false;
    bool validate_mode = false;
    bool debug_mode = false;
    
    // Demo mode options
    std::string demo_type;
    int demo_episodes = 5;
    int demo_steps = 200;
    
    // Benchmark mode options
    std::vector<std::pair<size_t, size_t>> benchmark_configs;
    std::string benchmark_output;
    bool benchmark_detailed = false;
    
    // Script mode options
    std::string script_file;
    bool script_continue = false;
    
    // Validation mode options
    std::string config_schema;
    bool validate_configs = false;
    bool validate_format = false;
};
```

## Command Support

### Available Commands

1. **demo**: Run a demo mode (default)
   - `--demo-type <type>`: Demo type (phase3, phase4, phase6)
   - `--episodes <n>`: Number of episodes (default: 5)
   - `--steps <n>`: Steps per episode (default: 200)

2. **benchmark**: Run performance benchmarks
   - `--benchmark-config <neurons,steps>`: Specify benchmark configuration
   - `--benchmark-output <file>`: Save results to file
   - `--detailed`: Run detailed analysis

3. **script**: Run batch processing script
   - `--script-file <file>`: Load commands from script file
   - `--continue`: Continue from last checkpoint

4. **test**: Run system tests
   - `--validate-configs`: Validate configuration files
   - `--validate-format`: Validate configuration file formats

5. **validate**: Validate configuration files
   - `--config <file>`: Configuration file to validate

### General Options

- `--help, -h`: Show help message
- `--version`: Show version information
- `--config <file>`: Load configuration from file
- `--log-level <level>`: Set logging level
- `--log-file <file>`: Log to file
- `--quiet, -q`: Quiet mode
- `--verbose, -v`: Verbose mode
- `--dry-run`: Don't run commands, just show what would be executed

## Configuration File Support

### Supported Formats

1. **Simple Key-Value** (`.cfg`)
   ```
   random_seed = 42
   neuron_count = 1000
   log_level = INFO
   ```

2. **JSON** (`.json`)
   ```json
   {
     "random_seed": 42,
     "neuron_count": 1000,
     "log_level": "INFO"
   }
   ```

3. **YAML** (`.yaml`, `.yml`)
   ```yaml
   random_seed: 42
   neuron_count: 1000
   log_level: INFO
   ```

### Format Detection

The framework automatically detects configuration file format:
- File extension (`.json`, `.yaml`, `.yml`, `.cfg`)
- Content analysis for ambiguous cases

## Usage Examples

### Basic Usage

```bash
# Run Phase 6 demo with defaults
nlm

# Run Phase 3 demo with custom settings
nlm demo --demo-type phase3 --episodes 10 --steps 500

# Run Phase 4 demo
nlm demo --demo-type phase4

# Run Phase 6 demo
nlm demo --demo-type phase6
```

### Benchmark Mode

```bash
# Run default benchmarks
nlm benchmark

# Run specific benchmark
nlm benchmark --benchmark-config 500,1000

# Run detailed benchmark
nlm benchmark --benchmark-config 1000,500 --detailed

# Save benchmark results
nlm benchmark --benchmark-config 2000,200 --benchmark-output results.txt
```

### Script Mode

```bash
# Run script
nlm script --script-file myscript.txt

# Continue from checkpoint
nlm script --continue
```

### Validation Mode

```bash
# Validate configuration file
nlm --config myconfig.cfg validate

# Validate format
nlm --config myconfig.json --validate-format
```

### Logging Configuration

```bash
# Set log level
nlm --log-level debug demo
nlm --log-level info benchmark

# Log to file
nlm --log-file nlm.log demo

# Quiet mode (no output)
nlm --quiet demo

# Verbose mode (detailed output)
nlm --verbose demo
```

### Help and Version

```bash
# Show general help
nlm --help

# Show command-specific help
nlm demo --help

# Show version
nlm --version
```

## Integration with Existing Executables

The CLI framework provides adapters for existing executables:

### Phase 3 Demo

**Old pattern:**
```cpp
int main(int argc, char* argv[]) {
    int numEpisodes = 5;
    if (argc > 1) numEpisodes = std::atoi(argv[1]);
    
    if (argc > 2) stepsPerEpisode = std::atoi(argv[2]);
    
    // Run demo
    Phase3Demo demo;
    demo.run(numEpisodes, stepsPerEpisode);
}
```

**New pattern:**
```cpp
int main(int argc, char* argv[]) {
    return nlm::CLIAdapter::adaptPhase3Demo(argc, argv);
}
```

### Phase 4 Demo

**Old pattern:**
```cpp
int main(int argc, char* argv[]) {
    size_t numTrials = 50;
    if (argc > 1) numTrials = std::stoi(argv[1]);
    
    // Run demo
    Phase4Demo demo;
    demo.run(numTrials);
}
```

**New pattern:**
```cpp
int main(int argc, char* argv[]) {
    return nlm::CLIAdapter::adaptPhase4Demo(argc, argv);
}
```

### Phase 6 Demo

**Old pattern:**
```cpp
int main(int argc, char* argv[]) {
    // No arguments, just run integration test
    Phase6Demo demo;
    demo.run(Phase6Config{});
}
```

**New pattern:**
```cpp
int main(int argc, char* argv[]) {
    return nlm::CLIAdapter::adaptPhase6Demo(argc, argv);
}
```

### Main Executable

**Old pattern:**
```cpp
int main(int argc, char** argv) {
    // Manual argument parsing
    // Run Phase 2 tests
}
```

**New pattern:**
```cpp
int main(int argc, char** argv) {
    return nlm::CLIAdapter::adaptMain(argc, argv);
}
```

## Development Guidelines

### Adding New Commands

To add a new command:
1. Add command to `Command` enum in `CLIFramework.hpp`
2. Update `CLIParser::parseCommand()` to handle the command
3. Add help text in `CLIParser::initializeHelpTexts()`
4. Implement command execution in `CLIFactory::executeCommand()`

### Adding New Options

To add a new option:
1. Add to `CLIConfig` structure in `CLIFramework.hpp`
2. Update `CLIParser::parseOptions()` to handle the option
3. Update help text in `CLIParser::initializeHelpTexts()`
4. Implement validation in `CLIParser::validate()`

### Configuration File Validation

Configuration files are validated during loading:
1. Format detection (`.json`, `.yaml`, `.yml`, `.cfg`)
2. Content parsing based on detected format
3. Validation of required fields
4. Type checking and range validation

## Testing

### Manual Testing

1. **Help System**
   ```bash
   nlm --help
   nlm demo --help
   nlm benchmark --help
   ```

2. **Version System**
   ```bash
   nlm --version
   ```

3. **Command Execution**
   ```bash
   nlm demo --demo-type phase3 --episodes 1 --steps 10
   nlm benchmark --benchmark-config 100,100
   nlm --config configs/default.cfg validate
   ```

4. **Error Handling**
   ```bash
   nlm --unknown-option
   nlm --demo-type unknown
   ```

### Automated Testing

The CLI framework is designed to work with existing testing infrastructure. Integration tests should:
1. Test argument parsing
2. Test configuration file loading
3. Test command execution
4. Test error handling
5. Test backward compatibility

## Migration Guide

### From Old to New CLI

**Step 1: Update main() function**
```cpp
// OLD
int main(int argc, char* argv[]) {
    // Manual parsing
    // ...
}

// NEW
int main(int argc, char* argv[]) {
    return nlm::CLIAdapter::adaptMain(argc, argv);
}
```

**Step 2: Update demo executables**
```cpp
// OLD (Phase3Demo.cpp)
int main(int argc, char* argv[]) {
    int numEpisodes = 5;
    if (argc > 1) numEpisodes = std::atoi(argv[1]);
    
    if (argc > 2) stepsPerEpisode = std::atoi(argv[2]);
    
    // Run demo
    Phase3Demo demo;
    demo.run(numEpisodes, stepsPerEpisode);
}

// NEW (Phase3Demo.cpp)
int main(int argc, char* argv[]) {
    return nlm::CLIAdapter::adaptPhase3Demo(argc, argv);
}
```

**Step 3: Test thoroughly**
1. Run `./nlm --help` to verify help system
2. Run `./nlm --version` to verify version system
3. Test all demo modes with `--help`
4. Test actual command execution
5. Test error cases

## Known Limitations

1. **Script Mode**: Script execution is basic and may need enhancement for complex scripts
2. **Configuration Validation**: Comprehensive schema validation is not yet implemented
3. **Command History**: Command history is not persisted
4. **Tab Completion**: Command-line completion is not implemented (optional)

## Future Enhancements

1. **Tab Completion**: Implement command-line completion support
2. **Command History**: Add command history with up/down arrow navigation
3. **Configuration Schemas**: Add support for JSON/YAML configuration schemas
4. **Advanced Scripting**: Enhance script mode with more advanced features
5. **Plugin System**: Add plugin system for extending CLI functionality
6. **Internationalization**: Add support for non-English help texts

## Conclusion

The NLM CLI framework provides a modern, comprehensive solution for command-line interface management across all NLM executables. It maintains backward compatibility while providing powerful new features for configuration management, logging, and command execution.

The framework is designed to be easily extensible and maintainable, with clear separation of concerns between parsing, configuration, and execution components.
