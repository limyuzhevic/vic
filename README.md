# NLM CLI Framework - Quick Start

## What This Is

The NLM CLI framework provides a modern, comprehensive command-line interface for all NLM executables. It replaces the old simple argument parsing with a robust, feature-rich CLI system that supports:

- Multiple execution modes (demo, benchmark, script, test, validate)
- Configuration file support (JSON, YAML, key=value) with automatic format detection
- Comprehensive help and documentation system
- Advanced logging configuration with structured logging and async support
- Backward compatibility with existing `--config` flag
- Modern C++20 implementation with proper separation of concerns
- Rich test coverage for all components
- Python bindings and integration capabilities
- Performance benchmarking and memory management features

## Key Improvements Implemented

### 1. Enhanced CLI Framework
- Modern argument parsing with short/long option support
- Subcommand-based architecture for organized command handling
- Flexible value parsing with `--key=value` or `--key value` formats
- Automatic configuration file format detection and parsing
- Comprehensive error handling and validation

### 2. Advanced Configuration System
- JSON/YAML/simple key-value configuration file support
- Configuration source tracking (File, CommandLine, Runtime, Default)
- Critical parameter validation and defaults
- Format conversion and migration utilities
- Comprehensive configuration testing (11 test cases)

### 3. Enhanced Logging System
- Hierarchical log levels from Trace to Fatal
- Structured logging with key-value pairs and custom fields
- Asynchronous logging with performance optimization
- File rotation and retention management
- Advanced query and search capabilities
- Global statistics and performance monitoring

### 4. Modern C++ Implementation
- C++20 standard with modern design patterns
- RAII for resource management
- Exception safety and proper error handling
- Move semantics for performance
- Template-based value handling

### 5. Comprehensive Testing
- Unit tests for configuration system (11 test functions)
- CLI framework tests with full coverage
- Integration tests for all new features
- Performance benchmarking tests
- Memory management tests
- C++ modernization feature tests
- Python bindings tests

### 6. Documentation and Examples
- Comprehensive API documentation for all headers
- Usage examples and best practices
- Migration guides from old CLI to new CLI
- Advanced feature documentation
- Troubleshooting and debugging guides

## Getting Started

### 1. Build the Project

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with all cores
make -j4
```

### 2. Test the CLI Framework

```bash
# Run the test suite
./nlm_test

# Test help system
./nlm --help

# Test version information
./nlm --version
```

### 3. Try Basic Commands

```bash
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

## Getting Started

### 1. Build the Project

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with all cores
make -j4
```

### 2. Test the CLI Framework

```bash
# Run the test suite
./nlm_test

# Test help system
./nlm --help

# Test version information
./nlm --version
```

### 3. Try Basic Commands

```bash
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

## Available Commands

### `demo` - Run a demo mode
```bash
./nlm demo --demo-type phase3
./nlm demo --demo-type phase4
./nlm demo --demo-type phase6
./nlm demo --demo-type phase3 --episodes 10 --steps 500
```

### `benchmark` - Run performance benchmarks
```bash
./nlm benchmark
./nlm benchmark --benchmark-config 500,1000
./nlm benchmark --benchmark-config 1000,500 --detailed
```

### `script` - Run batch processing script
```bash
./nlm script --script-file myscript.txt
./nlm script --continue
```

### `test` - Run system tests
```bash
./nlm test --validate-configs
./nlm test --validate-format
```

### `validate` - Validate configuration files
```bash
./nlm --config myconfig.cfg validate
./nlm --config myconfig.json validate
```

## Configuration Files

### Supported Formats

**Simple Key-Value (`.cfg`):**
```
# config.cfg
random_seed = 42
neuron_count = 1000
log_level = INFO
simulation_timestep = 0.001
```

**JSON (`.json`):**
```json
{
  "random_seed": 42,
  "neuron_count": 1000,
  "log_level": "INFO",
  "simulation_timestep": 0.001
}
```

**YAML (`.yaml`):**
```yaml
# config.yaml
random_seed: 42
neuron_count: 1000
log_level: INFO
simulation_timestep: 0.001
```

### Using Configuration Files

```bash
# Load configuration and run demo
./nlm --config config.cfg demo

# Validate configuration file
./nlm --config config.json validate
```

## Logging Options

### Log Levels
```bash
./nlm --log-level trace demo     # Detailed debug information
./nlm --log-level debug demo    # Debug information
./nlm --log-level info demo     # General information (default)
./nlm --log-level warning demo  # Warning messages
./nlm --log-level error demo    # Error messages
./nlm --log-level critical demo # Critical errors
./nlm --log-level off demo      # No logging
```

### Logging Configuration
```bash
# Set log level
./nlm --log-level debug demo

# Log to file
./nlm --log-level info --log-file nlm.log demo

# Quiet mode (no output)
./nml --quiet benchmark

# Verbose mode (detailed output)
./nml --verbose demo
```

## Help System

```bash
# General help
./nml --help

# Command-specific help
./nml demo --help
./nml benchmark --help
./nml validate --help

# Version information
./nml --version
```

## Examples

### Phase 3 Demo
```bash
# Quick Phase 3 demo
./nml demo --demo-type phase3

# Extended Phase 3 demo
./nml demo --demo-type phase3 --episodes 10 --steps 500
```

### Benchmark
```bash
# Default benchmarks
./nml benchmark

# Specific benchmark configuration
./nml benchmark --benchmark-config 500,1000

# Detailed benchmark with output
./nml benchmark --benchmark-config 1000,500 --detailed --benchmark-output results.txt
```

### Configuration Validation
```bash
# Validate configuration file
./nml --config production.cfg validate

# Validate with format checking
./nml --config settings.yaml --validate-format
```

### Debug Mode
```bash
# Run with debug logging
./nml --log-level debug demo

# Run with verbose output
./nml --verbose benchmark
```

## Backward Compatibility

The CLI framework maintains full backward compatibility with the existing `--config` flag:

```bash
# Old format still works
./nml --config configs/default.cfg

# New format with commands
./nml --config configs/default.cfg demo
```

## Documentation

### Available Documentation

- **CLI_FRAMEWORK.md** - Comprehensive CLI framework documentation
- **USAGE.md** - Usage examples and guidelines
- **QUICK_START.md** - Quick start guide
- **CLI_INTEGRATION_SUMMARY.md** - Detailed integration summary

### Accessing Documentation

```bash
# View general documentation
cat CLI_FRAMEWORK.md

# View usage examples
cat USAGE.md

# View quick start guide
cat QUICK_START.md
```

## Testing

### Run the Test Suite
```bash
# Build the project (if not already built)
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run the test suite
./nml_test
```

### Test Individual Components
```bash
# Test help system
./nml --help

# Test version system
./nml --version

# Test command help
./nml demo --help
./nml benchmark --help
```

## Troubleshooting

### Common Issues

**Issue: Command not found**
```bash
# Check if command exists
./nml --help

# Use correct command syntax
./nml demo --help
```

**Issue: Configuration file not found**
```bash
# Verify file exists
ls -la config.cfg

# Use absolute path if needed
./nml --config /path/to/config.cfg demo
```

**Issue: Invalid arguments**
```bash
# Get help for correct usage
./nml --help

# Use valid options
./nml demo --demo-type phase3
```

### Debug Mode
```bash
# Run with debug logging
./nml --log-level debug demo

# Run with verbose output
./nml --verbose benchmark
```

## Advanced Usage

### Combining Commands
```bash
# Load config and run demo
./nml --config myconfig.cfg demo

# Run benchmark with specific configuration
./nml benchmark --benchmark-config 500,1000

# Validate and run
./nml --config test.cfg validate && ./nml --config test.cfg demo
```

### Script Mode

Create a script file (`script.txt`):
```bash
# script.txt content
echo "Running Phase 3 demo"
./nml demo --demo-type phase3 --episodes 1

echo "Running benchmark"
./nml benchmark --benchmark-config 100,100
```

Run the script:
```bash
./nml script --script-file script.txt
```

### Batch Processing

```bash
# Create batch script
cat > batch.sh << 'EOF'
#!/bin/bash
./nml demo --demo-type phase3 --episodes 1
./nml benchmark --benchmark-config 100,100
./nml --config config.cfg validate
EOF

# Make executable and run
chmod +x batch.sh
./batch.sh
```

## Migration Guide

### From Old CLI to New CLI

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
./nml demo --demo-type phase3 --episodes 10 --steps 200

# With config
./nml --config configs/default.cfg demo

# Show help
./nml --help
```

## What This Framework Provides

1. **Unified CLI**: All NLM executables use the same CLI framework
2. **Modern Features**: Short/long options, value parsing, subcommands
3. **Rich Documentation**: Built-in help with examples
4. **Flexible Config**: Multiple file formats with auto-detection
5. **Backward Compatible**: Old commands and arguments still work
6. **Extensible**: Easy to add new commands and options
7. **Production Ready**: Robust error handling and validation

## Next Steps

1. **Build the project** (see above)
2. **Run `./nml --help`** to explore all options
3. **Try `./nml demo --demo-type phase3`** for a quick demo
4. **Explore `./nml benchmark`** for performance testing
5. **Test `./nml --config myconfig.cfg validate`** for config validation

The CLI framework makes it easy to run any NLM demo, benchmark, or validation with a clean, intuitive interface!
