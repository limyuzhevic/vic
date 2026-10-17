# NLM CLI Framework Usage

This document provides practical examples and usage guidelines for the NLM Command Line Interface framework.

## Quick Start

### Installation and Build

The NLM CLI framework is included in the standard NLM build process. No additional installation is required.

```bash
# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# The CLI framework is now integrated into all NLM executables
```

### Basic Usage

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
```

## Detailed Usage

### Help System

The CLI framework provides comprehensive help documentation:

```bash
# General help
./nlm --help

# Command-specific help
./nlm demo --help
./nlm benchmark --help
./nlm script --help
./nlm test --help
./nlm validate --help

# Show examples
./nlm --help  # Includes usage examples
```

### Version System

```bash
# Show NLM version
./nlm --version
```

## Command Reference

### Demo Command

The `demo` command runs various NLM demo modes.

#### Syntax
```bash
./nlm demo [options]
```

#### Options
```bash
--demo-type <type>    Demo type: phase3, phase4, phase6 (default: phase6)
--episodes <n>        Number of episodes (default: 5)
--steps <n>           Steps per episode (default: 200)
--help, -h            Show demo help
```

#### Examples
```bash
# Run Phase 3 demo
./nlm demo --demo-type phase3

# Run Phase 3 demo with custom settings
./nlm demo --demo-type phase3 --episodes 10 --steps 500

# Run Phase 4 demo
./nlm demo --demo-type phase4

# Run Phase 6 demo
./nlm demo --demo-type phase6

# Show demo help
./nlm demo --help
```

### Benchmark Command

The `benchmark` command runs performance benchmarks.

#### Syntax
```bash
./nlm benchmark [options]
```

#### Options
```bash
--benchmark-config <neurons,steps>  Specify benchmark configuration
--benchmark-output <file>           Save benchmark results to file
--detailed                           Run detailed benchmark analysis
--help, -h                          Show benchmark help
```

#### Examples
```bash
# Run default benchmarks
./nlm benchmark

# Run specific benchmark
./nlm benchmark --benchmark-config 500,1000

# Run detailed benchmark
./nlm benchmark --benchmark-config 1000,500 --detailed

# Save benchmark results
./nlm benchmark --benchmark-config 2000,200 --benchmark-output results.txt

# Show benchmark help
./nlm benchmark --help
```

#### Benchmark Configurations

Predefined benchmark configurations:
- `100,1000`: 100 neurons, 1000 steps
- `500,1000`: 500 neurons, 1000 steps  
- `1000,500`: 1000 neurons, 500 steps
- `2000,200`: 2000 neurons, 200 steps

### Script Command

The `script` command runs batch processing scripts.

#### Syntax
```bash
./nlm script [options]
```

#### Options
```bash
--script-file <file>  Load commands from script file
--continue            Continue from last checkpoint
--help, -h           Show script help
```

#### Examples
```bash
# Run script from file
./nlm script --script-file myscript.txt

# Continue from checkpoint
./nlm script --continue

# Show script help
./nlm script --help
```

### Test Command

The `test` command runs system tests.

#### Syntax
```bash
./nlm test [options]
```

#### Options
```bash
--validate-configs    Validate configuration files
--validate-format     Validate configuration file formats
--help, -h           Show test help
```

#### Examples
```bash
# Run tests with validation
./nlm test --validate-configs

# Show test help
./nlm test --help
```

### Validate Command

The `validate` command validates configuration files.

#### Syntax
```bash
./nlm validate [options]
```

#### Options
```bash
--config <file>      Configuration file to validate
--help, -h          Show validate help
```

#### Examples
```bash
# Validate configuration file
./nlm --config myconfig.cfg validate

# Validate JSON configuration
./nlm --config settings.json validate

# Show validate help
./nlm validate --help
```

## Configuration File Support

### Supported Formats

1. **Simple Key-Value** (`.cfg`)
2. **JSON** (`.json`)  
3. **YAML** (`.yaml`, `.yml`)

### Format Detection

The CLI framework automatically detects the format:

```bash
# The framework detects format from:
# - File extension (.cfg, .json, .yaml, .yml)
# - File content analysis
```

### Example Configuration Files

**Simple Key-Value (`config.cfg`):**
```
# NLM Configuration
random_seed = 42
neuron_count = 1000
region_count = 2
log_level = INFO
simulation_timestep = 0.001
```

**JSON (`config.json`):**
```json
{
  "random_seed": 42,
  "neuron_count": 1000,
  "region_count": 2,
  "log_level": "INFO",
  "simulation_timestep": 0.001
}
```

**YAML (`config.yaml`):**
```yaml
# NLM Configuration
random_seed: 42
neuron_count: 1000
region_count: 2
log_level: INFO
simulation_timestep: 0.001
```

### Using Configuration Files

```bash
# Load configuration and run demo
./nlm --config config.cfg demo

# Validate configuration file
./nlm --config config.json validate

# Run with configuration
./nlm --config settings.yaml benchmark
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

### Logging Options

```bash
# Set log level
./nlm --log-level debug demo

# Log to file
./nlm --log-level info --log-file nlm.log demo

# Quiet mode (suppress output)
./nlm --quiet demo

# Verbose mode (detailed output)
./nlm --verbose demo
```

## Backward Compatibility

The CLI framework maintains full backward compatibility with the existing `--config` flag:

```bash
# Old format still works
./nlm --config configs/default.cfg

# New format with commands
./nlm --config configs/default.cfg demo

# Old argument parsing still works
./nlm_phase3_demo 10 200
```

## Common Use Cases

### 1. Running Demos

```bash
# Phase 3 demo (world interaction)
./nlm demo --demo-type phase3

# Phase 4 demo (emerging cognition)
./nlm demo --demo-type phase4

# Phase 6 demo (integration test)
./nlm demo --demo-type phase6
```

### 2. Performance Testing

```bash
# Run benchmarks
./nlm benchmark

# Detailed benchmark analysis
./nlm benchmark --benchmark-config 1000,500 --detailed
```

### 3. Configuration Validation

```bash
# Validate configuration file
./nlm --config production.cfg validate

# Validate format
./nlm --config settings.yaml --validate-format
```

### 4. Custom Configuration

```bash
# Run with custom configuration
./nlm --config myconfig.cfg demo

# Override configuration values
./nlm --config config.cfg --neuron_count 2000 demo
```

## Troubleshooting

### Common Issues

**Issue: Command not found**
```bash
# Check if command exists
./nlm --help

# Use correct command syntax
./nlm demo --help
```

**Issue: Configuration file not found**
```bash
# Verify file exists
ls -la config.cfg

# Use absolute path if needed
./nlm --config /path/to/config.cfg demo
```

**Issue: Invalid arguments**
```bash
# Get help for correct usage
./nlm --help

# Use valid options
./nlm demo --demo-type phase3
```

**Issue: Permission denied**
```bash
# Check file permissions
ls -la executable_name

# Make executable if needed
chmod +x executable_name
```

### Debug Mode

```bash
# Run with debug logging
./nlm --log-level debug demo

# Run with verbose output
./nlm --verbose benchmark
```

## Advanced Usage

### Combining Commands

```bash
# Load config and run demo
./nlm --config myconfig.cfg demo

# Run benchmark with specific configuration
./nlm benchmark --benchmark-config 500,1000

# Validate and run
./nlm --config test.cfg validate && ./nlm --config test.cfg demo
```

### Script Mode

Create a script file (`script.txt`):
```bash
# script.txt content
echo "Running Phase 3 demo"
./nlm demo --demo-type phase3 --episodes 1

echo "Running benchmark"
./nlm benchmark --benchmark-config 100,100
```

Run the script:
```bash
./nlm script --script-file script.txt
```

### Batch Processing

```bash
# Create batch script
cat > batch.sh << 'EOF'
#!/bin/bash
./nlm demo --demo-type phase3 --episodes 1
./nlm benchmark --benchmark-config 100,100
./nlm --config config.cfg validate
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
./nlm_phase3_demo 10 200

# With config
./nlm_phase3_demo --config configs/default.cfg
```

**After (New CLI):**
```bash
# Phase 3 demo
./nlm demo --demo-type phase3 --episodes 10 --steps 200

# With config
./nlm --config configs/default.cfg demo
```

The new CLI provides the same functionality with:
- More intuitive command structure
- Better help and documentation
- Enhanced configuration support
- Improved error handling
- Backward compatibility

## Conclusion

The NLM CLI framework provides a modern, comprehensive command-line interface for all NLM executables. It combines the power of modern argument parsing with the flexibility of configuration file support, making it easy to run demos, benchmarks, scripts, and tests.

Key benefits:
- **Unified interface**: All NLM executables use the same CLI framework
- **Backward compatibility**: Existing scripts continue to work
- **Rich documentation**: Comprehensive help and examples
- **Flexible configuration**: Support for multiple file formats
- **Extensible**: Easy to add new commands and options

Get started today and discover the power of the NLM CLI framework!
