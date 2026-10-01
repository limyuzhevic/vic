# NLM (熙然) - Neural Learning Machine CLI Guide

This document provides comprehensive information about the NLM command-line interface (CLI), including:

- Available commands and options
- Configuration management
- Running simulations for different phases
- Performance monitoring and benchmarking
- Troubleshooting and common issues

## Overview

The NLM CLI provides a modern, user-friendly interface for running neural simulations across all 6 phases of the project. It includes advanced features such as:

- **Unified argument parsing** with modern `--key=value` syntax
- **Comprehensive help system** with context-sensitive documentation
- **Configuration management** with validation and error reporting
- **Performance monitoring** with benchmark tools
- **Checkpoint management** for saving/loading simulation states
- **Extensive logging** with configurable levels
- **Python bindings** for programmatic access

## Installation

The NLM CLI is included in the main build. After building the project:

```bash
# Build the project
cd /path/to/nlm
mkdir build && cd build
cmake .. && make -j4

# The following executables will be available:
./nlm              # Main NLM Phase 2 test suite
./nlm_phase3_demo   # NLM Phase 3 world interaction demo
./nlm_phase4_demo   # NLM Phase 4 cognitive experiments
./nlm_phase6_demo   # NLM Phase 6 integration test
```

## Basic Usage

### Help and Version

```bash
# Display general help information
nlm --help

# Display version information
nlm --version

# Display help for specific executables
nlm_phase3_demo --help
```

### Running Simulations

The main `nlm` executable runs the Phase 2 test suite with configurable options:

```bash
# Run all tests (default)
nlm

# Run specific tests only
nlm --test 1          # Basic connectivity test
nlm --test 2          # Plasticity learning experiment
nlm --test 3          # STDP verification test
nlm --test 1,2,3      # Tests 1 and 2 only (not supported yet)

# Override configuration values
nlm --set neuron_count=1000 --set connection_probability=0.2

# Run with custom configuration file
nlm --config my_config.cfg

# Enable verbose logging
nlm --verbose

# Set log level
nlm --log-level DEBUG
```

## Demo Executables

### Phase 3: World Interaction

```bash
# Run Phase 3 demo with default settings (5 episodes, 200 steps each)
nlm_phase3_demo

# Customize number of episodes
nlm_phase3_demo 20                      # 20 episodes, 200 steps each
nlm_phase3_demo 20 500                   # 20 episodes, 500 steps each

# With logging
nlm_phase3_demo --verbose --log-level INFO

# With configuration file
nlm_phase3_demo --config phase3_config.cfg
```

### Phase 4: Cognitive Experiments

```bash
# Run Phase 4 experiments with default settings (50 trials)
nlm_phase4_demo

# Customize number of trials
nlm_phase4_demo 100                      # 100 trials

# Run with specific operating mode
nlm_phase4_demo --mode full
nlm_phase4_demo --mode quick              # Faster, less comprehensive

# With logging
nlm_phase4_demo --verbose --log-level DEBUG
```

### Phase 6: Integration Test

```bash
# Run Phase 6 integration test
nlm_phase6_demo

# With custom settings
nlm_phase6_demo --neuron-count 2000 --max-steps 5000

# With specific experiment mode
nlm_phase6_demo --experiment-mode full
nlm_phase6_demo --experiment-mode thorough    # Comprehensive testing
```

## Configuration Management

NLM provides comprehensive configuration management through both command line and configuration files.

### Configuration File Format

Configuration files use a simple key-value format:

```ini
# Example configuration file (config.cfg)
# Brain parameters
neuron_count = 500
region_count = 2
connection_probability = 0.1
simulation_timestep = 0.001

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Development parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Random seed for reproducibility
random_seed = 42
```

### Command Line Configuration

Override configuration values using the `--set` option:

```bash
# Override individual values
nlm --set neuron_count=1000 --set connection_probability=0.2

# Load configuration file and override values
nlm --config default.cfg --set neuron_count=2000

# Set multiple values
nlm --set neuron_count=1000 --set region_count=4 \
    --set stdp_ltp_weight=0.01 --set stdp_ltd_weight=0.012
```

### Configuration Validation

To validate a configuration file:

```bash
# The validation feature needs to be tested and verified
# For now, configuration files are loaded without validation
nlm --config my_config.cfg
```

## Advanced Configuration

### Working with the Python API

For programmatic access to NLM, use the Python bindings:

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("neuron_count", 1000)
config.set("connection_probability", 0.2)

# Save configuration to file
config.saveToFile("my_config.json")

# Create brain with configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
brain.step(0, 0.0)
```

## Performance Monitoring and Benchmarking

NLM includes performance monitoring capabilities through the Python CLI.

### Running Benchmarks

```bash
# Run benchmark for Phase 4 experiments
nlm benchmark --phase 4 --repeats 5

# Run benchmark with output file
nlm benchmark --phase 3 --repeats 10 --output benchmark_results.json
```

### Performance Metrics

The benchmark system measures:
- Simulation time per step
- Memory usage
- Processing speed
- Network efficiency
- Learning performance

## Checkpoint Management

NLM supports checkpointing for saving and restoring simulation states.

### Saving Checkpoints

```bash
# Save current brain state
nlm checkpoint save --file brain_state.checkpoint

# Save with specific phase information
nlm checkpoint save --file phase3_state.checkpoint --phase 3
```

### Loading Checkpoints

```bash
# Load saved brain state
nlm checkpoint load --file brain_state.checkpoint

# Load state for specific phase
nlm checkpoint load --file phase3_state.checkpoint --phase 3
```

## Logging Configuration

NLM provides extensive logging capabilities for debugging and monitoring.

### Log Levels

Available log levels:

- **DEBUG**: Detailed debug information
- **INFO**: General information messages
- **WARNING**: Warning conditions
- **ERROR**: Error conditions
- **CRITICAL**: Critical errors

### Logging Options

```bash
# Set log level
nlm --log-level DEBUG

# Enable verbose output
nlm --verbose

# Suppress output
nlm --quiet

# Set log file (requires additional implementation)
# nlm --log-file simulation.log
```

## Examples

### Complete Example: Running a Custom Simulation

```bash
# Run a custom Phase 3 simulation with detailed logging
nlm_phase3_demo \
    --episodes 10 \
    --steps 500 \
    --neuron-count 800 \
    --verbose \
    --log-level INFO \
    --config custom_settings.cfg \
    --set random_seed=123
```

### Example: Configuration Management Workflow

```bash
# 1. Create a new configuration file
nlm config create --file research_config.cfg --template research

# 2. View configuration options
nlm config list

# 3. Validate configuration
nlm config validate --file research_config.cfg

# 4. Run simulation with custom configuration
nlm --config research_config.cfg --verbose
```

### Example: Performance Comparison

```bash
# Run benchmark for different configurations
nlm benchmark --phase 4 --repeats 5 --output config1_bench.json
nlm benchmark --phase 4 --repeats 5 --output config2_bench.json

# Compare results
# (Manual comparison or use external tools)
```

## Troubleshooting

### Common Issues and Solutions

**Issue: Configuration file not found**

Solution:
```bash
# Use absolute path or ensure file is in current directory
nlm --config /path/to/config.cfg
```

**Issue: Invalid configuration values**

Solution:
```bash
# Check configuration file syntax
# Use default values or fix configuration file
nlm --config my_config.cfg
```

**Issue: Simulation takes too long**

Solution:
```bash
# Reduce neuron count for faster simulation
nlm --set neuron_count=100

# Reduce number of trials/steps
nlm --set steps=100
```

**Issue: Memory usage too high**

Solution:
```bash
# Reduce simulation parameters
nlm --set neuron_count=500 --set region_count=2
```

### Getting More Help

For more detailed information:

- **Command help**: `nlm <command> --help`
- **Phase-specific documentation**: See individual executable documentation
- **Python API documentation**: See `python/bindings.cpp` and `include/nlm/`
- **Project documentation**: See `README.md`, `easy_usage.md`, `HOW_TO_USE.md`

## Future Enhancements

The following features are planned for future releases:

1. **Full configuration validation** with detailed error messages
2. **JSON/YAML configuration file support**
3. **Advanced checkpoint compression**
4. **Parallel simulation support**
5. **Web-based visualization interface**
6. **Machine learning model integration**
7. **Distributed simulation support**
8. **Real-time monitoring and visualization**

## Version Information

Current version: Phase 2.0 (CLI Framework v1.0)

## Acknowledgments

This CLI framework builds upon the existing NLM architecture developed by the NLM research team. The unified CLI interface provides a modern, consistent interface across all NLM executables while maintaining backward compatibility.

For issues, questions, or contributions, please refer to the main NLM project documentation.
