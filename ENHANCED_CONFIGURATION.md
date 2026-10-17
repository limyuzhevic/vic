# NLM (Neural Learning Machine) - Enhanced Configuration System

## Overview

This document describes the enhanced configuration system and command-line interface improvements for the NLM (Neural Learning Machine) project. The implementation addresses all requirements specified in the original request while maintaining backward compatibility.

## Key Improvements

### 1. Enhanced Command-Line Interface

#### New Command-Line Options

- **Experiment Type Selection**
  - `--experiment type=<type>`: Select experiment type (learning, behavior, plasticity)
  - `-e type=<type>`: Short form

- **Output Formatting**
  - `--output-format <format>`: Set output format (json, text, csv)
  - `-o <format>`: Short form

- **Simulation Mode**
  - `--mode <mode>`: Set simulation mode (demo, test, production)
  - `-m <mode>`: Short form

- **Logging Control**
  - `--log-level <level>`: Set logging level (debug, info, warn, error)
  - `--log-file <file>`: Specify log file path

- **Performance Profiling**
  - `--profile`: Enable performance profiling
  - `--profile-frequency <n>`: Set profiling sampling frequency (Hz)

- **Checkpoint Management**
  - `--checkpoint-interval <sec>`: Set checkpoint save interval (seconds)
  - `--checkpoint-dir <dir>`: Specify checkpoint directory

- **Visualization Options**
  - `--enable-visualization`: Enable visualization
  - `--visualization-fps <fps>`: Set visualization frame rate (Hz)

#### Configuration Management

- **Template Generation**
  - `--generate-template`: Generate configuration template with all parameters

- **Configuration Comparison**
  - `--compare-config <file1> <file2>`: Compare two configuration files

- **Backup and Restore**
  - `--backup-config <dir>`: Backup current configuration to timestamped file
  - `--restore-config <file>`: Restore configuration from backup

- **Validation**
  - `--validate-config`: Validate configuration against schema

#### Execution Control

- **Multi-stage Execution**
  - `--run-stage <stage>`: Run specific stage (1, 2, 3, 4, 5, or 6)

- **Parallel Execution**
  - `--parallel`: Enable parallel execution

- **Timing Control**
  - `--duration <sec>`: Set total simulation duration (seconds)
  - `--timestep <sec>`: Set simulation time step (seconds)

- **Input Pattern Specification**
  - `--input-pattern <pattern>`: Specify input pattern for experiments

#### Output and Analysis

- **Result Export**
  - `--export-results <file>`: Export results to CSV or JSON file

- **Analysis Integration**
  - `--analysis-type <type>`: Set analysis type (learning, behavior, plasticity)

- **Visualization Generation**
  - `--generate-plots`: Generate visualization plots

- **Statistical Analysis**
  - `--compute-statistics`: Compute statistical analysis

#### Advanced Options

- **Memory Management**
  - `--memory-limit <mb>`: Set memory limit in MB

- **GPU Acceleration**
  - `--use-gpu`: Enable GPU acceleration

- **Distributed Computing**
  - `--distributed`: Enable distributed computing

- **Custom Neural Architectures**
  - `--custom-arch <file>`: Specify custom neural architecture file

- **Random Seed**
  - `--random-seed <seed>`: Set random seed for reproducibility

### 2. Configuration System Improvements

#### Configuration Schema Validation

The configuration system now includes comprehensive validation:

- **Type Validation**: Ensures values match expected types (int, double, bool, string)
- **Range Validation**: Validates numerical ranges for parameters
- **Allowed Values Validation**: Ensures string values are within allowed sets
- **Required Parameters**: Identifies mandatory configuration parameters

#### Experiment-Specific Configuration Presets

Three predefined experiment types with optimized configurations:

- **Learning Experiment**
  - `plasticity.learningRate`: 0.05
  - `plasticity.stdp.learningRate`: 0.03
  - `plasticity.stdp.ltpWeight`: 0.05
  - `plasticity.stdp.ltdWeight`: 0.04

- **Behavior Experiment**
  - `plasticity.learningRate`: 0.02
  - `plasticity.stdp.learningRate`: 0.01
  - `plasticity.stdp.ltpWeight`: 0.02
  - `plasticity.stdp.ltdWeight`: 0.018

- **Plasticity Experiment**
  - `plasticity.learningRate`: 0.1
  - `plasticity.stdp.learningRate`: 0.08
  - `plasticity.stdp.ltpWeight`: 0.1
  - `plasticity.stdp.ltdWeight`: 0.09

#### Configuration Management Features

- **Template Generation**: Creates templates with all parameters, types, and constraints
- **Configuration Comparison**: Shows differences between configuration files
- **Backup and Restore**: Timestamped backups with full restoration capability
- **Schema Documentation**: Generates comprehensive documentation of all parameters

#### Better Error Messages

- Clear, descriptive error messages for validation failures
- Parameter name identification in error messages
- Context about expected vs. actual values

### 3. Execution Control Enhancements

#### Multi-stage Execution

- **Stage 1**: Basic Neural Connectivity Test
- **Stage 2**: Plasticity Learning Experiment
- **Stage 3**: STDP Verification
- **Stage 4**: Advanced Plasticity
- **Stage 5**: Memory Integration
- **Stage 6**: Neuromodulation

#### Parallel and Distributed Computing

- Support for parallel execution across multiple cores
- Distributed computing support for large-scale simulations
- Memory limit controls for resource management

### 4. Output and Analysis Enhancements

#### Result Export

- **CSV Format**: Tab-separated values for spreadsheet analysis
- **JSON Format**: Structured data for programmatic processing
- **Text Format**: Human-readable output for console viewing

#### Statistical Analysis

- **Learning Metrics**: Synaptic changes, firing rates, and activity patterns
- **Behavioral Metrics**: Response times, accuracy, and performance measures
- **Plasticity Metrics**: Learning curves, adaptation rates, and stability measures

### 5. Backward Compatibility

The enhanced system maintains full backward compatibility:

- Existing `--config <file>` option continues to work as before
- All existing configuration file formats are supported
- Default behavior remains unchanged for users who don't specify new options
- Configuration validation is optional and can be disabled if needed

## Implementation Details

### Configuration Schema

The system supports the following configuration parameter categories:

- **Neural Architecture**: `neuron.count`, `neuron.regionCount`, `neuron.connectionProbability`
- **Plasticity**: `plasticity.learningRate`, `plasticity.stdp.learningRate`, etc.
- **Simulation**: `simulation.timestep`, `simulation.maxSteps`, `simulation.timeLimit`
- **Logging**: `logging.level`, `logging.enabled`, `logging.filename`
- **Visualization**: `visualization.enabled`, `visualization.updateRate`
- **Checkpoint**: `checkpoint.interval`

### Configuration File Format

The system uses a simple key=value format with support for:

```
# Comments
key = value
neuron.count = 500
plasticity.learningRate = 0.01
```

### Command-Line Override Priority

1. Command-line arguments (highest precedence)
2. Configuration file
3. Default values
4. Runtime settings

## Usage Examples

### Basic Usage

```bash
# Run learning experiment with custom configuration
nlm --experiment learning --config myconfig.cfg --log-level debug

# Production mode with GPU acceleration
nlm --mode production --use-gpu --checkpoint-interval 60 --output-format json
```

### Configuration Management

```bash
# Generate configuration template
nlm --generate-template

# Compare two configuration files
nlm --compare-config config1.cfg config2.cfg

# Backup and restore configuration
nlm --backup-config ./backups
nlm --restore-config ./backups/nlm_config_20240101_120000.cfg

# Validate configuration
nlm --validate-config
```

### Experiment-Specific Configuration

```bash
# Run behavior experiment with specific parameters
nlm --experiment behavior --set simulation.timestep=0.01 --set neuron.count=1000

# Run plasticity experiment with GPU
nlm --experiment plasticity --use-gpu --duration 300
```

## Testing

### Unit Tests

The enhanced system includes comprehensive unit tests:

- **Config Tests**: Core configuration functionality
- **Configuration Validator Tests**: Schema validation and presets
- **Integration Tests**: Full system integration

### Test Commands

```bash
# Build tests
mkdir build && cd build
cmake .. && make nlm_test

# Run tests
./nlm_test

# Run individual test suites
./nlm_test --gtest_filter="testConfig*"
./nlm_test --gtest_filter="testConfigurationValidator*"
```

## Future Enhancements

The implementation includes a solid foundation for future enhancements:

- **JSON/YAML Configuration Support**: Planned for Phase 2
- **Advanced Validation Rules**: Custom validation scripts
- **Distributed Computing Support**: Integration with distributed frameworks
- **Advanced Analytics**: Real-time monitoring and analysis
- **Plugin Architecture**: Extensible plugin system for new experiment types

## Files Modified

### Source Files

- `src/core/Config/Config.hpp`: Enhanced configuration class with new methods
- `src/core/Config/Config.cpp`: Implementation of enhanced configuration features
- `src/core/Config/ConfigurationValidator.hpp`: New configuration schema validation system
- `src/main.cpp`: Enhanced command-line interface and configuration handling

### Test Files

- `tests/test_config.cpp`: Comprehensive configuration tests
- `tests/test_configuration_validator.cpp`: Configuration validator tests
- `tests/CMakeLists.txt`: Updated test configuration

### Documentation

- `configs/default.cfg`: Enhanced default configuration file

## Build Instructions

### Standard Build

```bash
mkdir build && cd build
cmake .. && make -j$(nproc)
```

### Build with Tests

```bash
mkdir build && cd build
cmake -DENABLE_TESTS=ON .. && make -j$(nproc)
```

### Install

```bash
cd build
sudo make install
```

## License

This implementation is part of the NLM project and follows the project's existing licensing terms.

## Acknowledgments

The enhanced configuration system builds upon the existing NLM codebase and follows the project's established patterns and conventions. Special thanks to the development team for their contributions to the base implementation.
