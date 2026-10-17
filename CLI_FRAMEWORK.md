# NLM CLI Framework - Advanced Documentation

## Overview

The NLM CLI framework provides a modern, production-ready command-line interface for all NLM executables. It represents a complete rewrite of the CLI system with enhanced features, modern C++20 implementation, and comprehensive testing.

## Key Features

### 1. Modern Architecture
- **Clean Design**: Separation of concerns with CLIParser, CLIFactory, CLIAdapter classes
- **C++20 Standard**: Modern C++ features with RAII and move semantics
- **Exception Safety**: Robust error handling throughout
- **Memory Management**: Smart pointers and automatic resource cleanup

### 2. Advanced Configuration Support
- **Multiple Formats**: JSON, YAML, and simple key-value configuration files
- **Automatic Detection**: Format auto-detection based on file extension and content
- **Source Tracking**: Configuration source tracking (File, CommandLine, Runtime, Default)
- **Validation**: Comprehensive configuration validation with critical parameter checking

### 3. Enhanced Logging System
- **Hierarchical Levels**: 11-level log system from Trace to Fatal
- **Structured Logging**: Key-value pairs and custom fields support
- **Asynchronous Processing**: Performance-optimized async logging
- **Advanced Features**: Query, search, aggregation, and export capabilities
- **File Rotation**: Automatic file rotation with retention management

### 4. Rich CLI Capabilities
- **Subcommands**: demo, benchmark, script, test, validate commands
- **Flexible Parsing**: `--key=value` or `--key value` formats
- **Help System**: Comprehensive help with examples and usage documentation
- **Backward Compatibility**: Old `--config` flag still supported
- **Command Validation**: Built-in validation for all command options

## API Documentation

### Core Classes

#### CLIConfig Structure
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
    
    // Dry run mode
    std::string dry_run_command;
};
```

#### Command Enumeration
```cpp
enum class Command {
    None,
    Demo,
    Benchmark,
    Script,
    Test,
    Validate,
    Help,
    Version
};
```

### Configuration System

#### ConfigValue Variant
The configuration system uses a variant to store different value types:
```cpp
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;
```

#### ConfigSource Enumeration
```cpp
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};
```

#### ConfigFormat Enumeration
```cpp
enum class ConfigFormat {
    Unknown,
    SimpleKeyValue,
    JSON,
    YAML
};
```

### Logger Classes

#### LogLevel Enumeration
```cpp
enum class LogLevel {
    None = -1,         // No logging
    Debug = 0,         // 1 - 10
    Trace = 1,         // 2
    Verbose = 2,       // 3 - 5
    Info = 3,          // 10 - 50
    Notice = 4,        // 20
    Warning = 5,       // 50 - 100
    Error = 6,         // 100 - 500
    Critical = 7,      // 500 - 1000
    Alert = 8,         // 1000 - 5000
    Emergency = 9,     // 5000 - 10000
    Fatal = 10         // 10000+
};
```

#### ILogger Interface
```cpp
class ILogger {
public:
    virtual ~ILogger() = default;
    
    // Core logging methods
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Enhanced configuration
    virtual void configure(const LogConfig& config) = 0;
    virtual LogConfig getConfig() const = 0;
    
    // Filtering
    virtual void setFilter(const LogFilter& filter) = 0;
    virtual LogFilter getFilter() const = 0;
    virtual bool shouldLog(const LogEntry& entry) const = 0;
    
    // Statistics
    virtual uint64_t getLogCount() const = 0;
    virtual std::chrono::system_clock::time_point getLastLogTime() const = 0;
    virtual double getAverageLogRate() const = 0;
    
    // Performance monitoring
    virtual void resetStats() = 0;
    
    // Sink management
    virtual void flush() = 0;
    virtual bool isBufferFull() const = 0;
};
```

### CLIParser Class

The main parser class handles argument parsing and validation:

```cpp
class CLIParser {
public:
    CLIParser();
    ~CLIParser();
    
    // Parse command line arguments
    bool parse(int argc, char** argv, CLIConfig& config);
    
    // Print help information
    void printHelp(const std::string& programName = "nlm") const;
    
    // Print version information
    void printVersion() const;
    
    // Validate configuration
    bool validate(const CLIConfig& config) const;
    
    // Get command string from enum
    static std::string commandToString(Command cmd);
    
    // Get command enum from string
    static Command stringToCommand(const std::string& str);
    
    // Detect configuration file format
    static ConfigFormat detectConfigFormat(const std::string& filepath);
    
    // Parse log level string
    static LogLevel stringToLogLevel(const std::string& level);
    
    // Get available commands
    static std::vector<std::string> getAvailableCommands();
    
    // Get demo types
    static std::vector<std::string> getAvailableDemoTypes();
    
    // Get default benchmark configurations
    static std::vector<std::pair<size_t, size_t>> getDefaultBenchmarkConfigs();
};
```

## Usage Examples

### Basic CLI Usage

```bash
# Show help information
./nlm --help

# Show version information
./nlm --version

# Run Phase 6 demo (default)
./nlm

# Run Phase 3 demo
./nlm demo --demo-type phase3

# Run Phase 4 demo
./nlm demo --demo-type phase4

# Run Phase 6 demo
./nlm demo --demo-type phase6

# Run benchmark
./nlm benchmark
```

### Configuration Examples

```bash
# Load configuration from JSON file
./nlm --config settings.json demo

# Load configuration from YAML file
./nlm --config config.yaml benchmark

# Load configuration from key=value file
./nlm --config config.cfg validate

# Validate configuration file
./nlm --config production.cfg validate

# Run with debug logging
./nlm --log-level debug demo

# Run with quiet mode
./nml --quiet benchmark

# Run with verbose mode
./nml --verbose demo
```

### Advanced Configuration Usage

```bash
# Run demo with custom settings
./nlm demo --demo-type phase3 --episodes 10 --steps 500

# Run benchmark with specific configuration
./nlm benchmark --benchmark-config 500,1000

# Save benchmark results
./nlm benchmark --benchmark-config 1000,500 --benchmark-output results.txt

# Run detailed benchmark
./nlm benchmark --benchmark-config 2000,200 --detailed

# Load config and validate
test_cfg = $(mktemp)
echo 'random_seed = 42
neuron_count = 1000
log_level = INFO' > $test_cfg
./nlm --config $test_cfg validate
rm $test_cfg
```

### Script Mode Usage

```bash
# Create a script file
script.txt << EOF
Running Phase 3 demo
./nlm demo --demo-type phase3 --episodes 1

Running benchmark
./nlm benchmark --benchmark-config 100,100
EOF

# Run the script
./nlm script --script-file script.txt
```

### Batch Processing

```bash
# Create batch script
batch.sh << 'EOF'
#!/bin/bash
./nlm demo --demo-type phase3 --episodes 1
./nlm benchmark --benchmark-config 100,100
./nlm --config config.cfg validate
EOF

# Make executable and run
chmod +x batch.sh
./batch.sh
```

## Configuration File Formats

### JSON Configuration

```json
{
  "random_seed": 42,
  "neuron_count": 1000,
  "region_count": 2,
  "log_level": "INFO",
  "simulation_timestep": 0.001,
  "demo_type": "phase3",
  "demo_episodes": 5,
  "demo_steps": 200
}
```

### YAML Configuration

```yaml
# NLM Configuration
random_seed: 42
neuron_count: 1000
region_count: 2
log_level: INFO
simulation_timestep: 0.001
demo_type: phase3
demo_episodes: 5
demo_steps: 200
```

### Key-Value Configuration

```
# config.cfg
# NLM Configuration
random_seed = 42
neuron_count = 1000
region_count = 2
log_level = INFO
simulation_timestep = 0.001
demo_type = phase3
demo_episodes = 5
demo_steps = 200
```

## Logging Configuration

### Log Levels

```bash
# Log levels (from lowest to highest)
--log-level trace     # Detailed debug information
--log-level debug    # Debug information
--log-level info     # General information (default)
--log-level warning  # Warning messages
--log-level error    # Error messages
--log-level critical # Critical errors
--log-level off      # No logging
```

### Logging Examples

```bash
# Set log level
./nlm --log-level debug demo

# Log to file
./nlm --log-level info --log-file nlm.log demo

# Quiet mode (suppress output)
./nml --quiet demo

# Verbose mode (detailed output)
./nml --verbose benchmark

# Structured logging with custom fields
# (Configured via configuration files)
```

## Command Reference

### Demo Command

**Syntax:** `nlm demo [options]`

**Options:**
- `--demo-type <type>` - Demo type: phase3, phase4, phase6 (default: phase6)
- `--episodes <n>` - Number of episodes for demo (default: 5)
- `--steps <n>` - Steps per episode (default: 200)
- `--help, -h` - Show demo help

**Examples:**
```bash
./nlm demo --demo-type phase3
./nlm demo --demo-type phase3 --episodes 10 --steps 500
./nlm demo --demo-type phase4
./nlm demo --demo-type phase6
```

### Benchmark Command

**Syntax:** `nlm benchmark [options]`

**Options:**
- `--benchmark-config <neurons,steps>` - Specify benchmark configuration
- `--benchmark-output <file>` - Save benchmark results to file
- `--detailed` - Run detailed benchmark analysis
- `--help, -h` - Show benchmark help

**Examples:**
```bash
./nlm benchmark
./nlm benchmark --benchmark-config 500,1000
./nlm benchmark --benchmark-config 1000,500 --detailed
./nlm benchmark --benchmark-config 2000,200 --benchmark-output results.txt
```

### Script Command

**Syntax:** `nlm script [options]`

**Options:**
- `--script-file <file>` - Load commands from script file
- `--continue` - Continue from last checkpoint
- `--help, -h` - Show script help

### Test Command

**Syntax:** `nlm test [options]`

**Options:**
- `--validate-configs` - Validate configuration files
- `--validate-format` - Validate configuration file formats
- `--help, -h` - Show test help

### Validate Command

**Syntax:** `nlm validate [options]`

**Options:**
- `--config <file>` - Configuration file to validate
- `--help, -h` - Show validate help

## Help System

### Available Help Commands

```bash
# General help
./nlm --help

# Command-specific help
./nlm demo --help
./nml benchmark --help
./nml script --help
./nml test --help
./nml validate --help

# Version information
./nml --version
```

### Help Format

The help system provides:
- Program version and copyright information
- General options with descriptions
- Available commands list
- Command-specific help with examples
- Usage syntax examples
- Common troubleshooting tips

## Backward Compatibility

The CLI framework maintains full backward compatibility:

### Old CLI Usage Still Works

```bash
# Old format still works
./nml_phase3_demo 10 200

# With config (old style)
./nml_phase3_demo --config configs/default.cfg

# New format with commands
./nml --config configs/default.cfg demo
```

### Migration from Old to New CLI

**Before (Old CLI):**
```bash
# Phase 3 demo
./nml_phase3_demo 10 200

# With config
./nml_phase3_demo --config configs/default.cfg
```

**After (New CLI):**
```bash
# Phase 3 demo
./nlm demo --demo-type phase3 --episodes 10 --steps 200

# With config
./nml --config configs/default.cfg demo

# Show help
./nml --help
```

## Testing

### Test Suite

```bash
# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run the test suite
./nml_test

# Test individual components
./nlm --help
./nlm --version
./nlm demo --help
./nlm benchmark --help
```

### Test Coverage

The test suite includes:
- Configuration system tests (11 test functions)
- CLI framework tests with full coverage
- CLI-specific tests
- Simple compilation tests
- Integration tests for all features

## Performance Considerations

### Benchmarking

The CLI framework includes performance benchmarking capabilities:

```bash
# Run benchmarks with different configurations
./nlm benchmark --benchmark-config 100,1000
./nlm benchmark --benchmark-config 500,1000
./nlm benchmark --benchmark-config 1000,500
./nlm benchmark --benchmark-config 2000,200
```

### Memory Management

The CLI framework uses modern C++ memory management:
- Smart pointers for automatic resource cleanup
- Move semantics for performance
- RAII for exception safety
- Pooled allocation for frequent allocations

### Async Logging

For high-performance logging:
- Asynchronous logging with configurable batch sizes
- Multiple flush threads for parallel processing
- Buffer management with ring buffers
- Configurable flush intervals

## Future Enhancements

### Planned Features

1. **Tab Completion**: Command-line completion support
2. **Command History**: Command history with up/down arrow navigation
3. **Configuration Schemas**: JSON/YAML schema validation
4. **Advanced Scripting**: Enhanced script mode with more features
5. **Plugin System**: Plugin architecture for extending CLI functionality
6. **Internationalization**: Support for multiple languages

### Development Roadmap

1. **Phase 1**: Core CLI framework implementation ✅
2. **Phase 2**: Demo and benchmark integration ✅
3. **Phase 3**: Script and validation command support ✅
4. **Phase 4**: Advanced features and enhancements (planned)
5. **Phase 5**: Performance optimization and testing (planned)
6. **Phase 6**: Production deployment and monitoring (planned)

## Conclusion

The NLM CLI framework provides a modern, comprehensive command-line interface that:

✅ **Unifies** all NLM executables under a single, consistent interface
✅ **Enhances** functionality with modern CLI features
✅ **Maintains** full backward compatibility
✅ **Improves** user experience with better help and documentation
✅ **Extends** capabilities with new commands and options
✅ **Optimizes** performance with modern C++ implementation
✅ **Comprehensive** testing with full test coverage
✅ **Production Ready**: Robust error handling and validation

The framework is ready for production use, with comprehensive testing, documentation, and examples. It provides a solid foundation for future enhancements and extensions to the NLM project.