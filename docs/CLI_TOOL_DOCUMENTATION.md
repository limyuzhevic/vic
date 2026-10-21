# NLM CLI Tool - Complete System Documentation

This document provides comprehensive documentation for the NLM CLI tool, which replaces the previous hardcoded test system with a full-featured command-line interface.

## Overview

The NLM CLI tool provides a unified interface for:
- Running simulations and experiments
- Managing brain states and checkpoints
- Analyzing performance and metrics
- Exporting results in multiple formats
- Running comprehensive benchmarks and tests
- Monitoring system resources

## Command Structure

### Core Commands

#### `nlm --help, -h`
**Description**: Display help information and command usage
**Usage**: `nlm [--help] [--version]`
**Output**: Displays all available commands and their usage

#### `nlm --version`
**Description**: Display NLM version information
**Usage**: `nlm --version`
**Output**: Shows NLM Phase 6.0 version and build information

#### `nlm --simulate [options]`
**Description**: Run NLM integration simulation
**Usage**: `nlm --simulate [options]`
**Options**:
- `--config <file>`: Configuration file (default: `configs/default.cfg`)
- `--steps <N>`: Number of simulation steps (default: 10000)
- `--neurons <N>`: Number of neurons (default: 1000)
- `--regions <N>`: Number of brain regions (default: 1)
- `--headless`: Run without interactive output
- `--checkpoint <file>`: Save final state to file

**Examples**:
```bash
# Basic simulation
nlm --simulate

# Simulation with custom configuration
nlm --simulate --config configs/production.cfg

# Large simulation with checkpointing
nlm --simulate --neurons 2000 --steps 5000 --checkpoint experiment1.bin
```

#### `nlm --benchmark [options]`
**Description**: Run performance benchmarks
**Usage**: `nlm --benchmark [options]`
**Options**:
- `--range <mode>`: Scale of benchmark: `small|medium|large`
- `--output <file>`: Save results to file (JSON format)
- `--duration <seconds>`: Maximum benchmark duration

**Examples**:
```bash
# Medium scale benchmark
nlm --benchmark --range medium

# Large benchmark with output
nlm --benchmark --range large --output benchmark_results.json

# Short benchmark
nlm --benchmark --range small --duration 60
```

#### `nlm --test [options]`
**Description**: Run system tests
**Usage**: `nlm --test [options]`
**Options**:
- `--config <file>`: Test configuration file
- `--output <file>`: Save test results to file

**Available Tests**:
- `basic`: Basic connectivity and spike propagation
- `plasticity`: Synaptic plasticity and learning
- `stdp`: Spike-Timing-Dependent Plasticity
- `integration`: Phase 6 integration verification
- `all`: Run all tests

**Examples**:
```bash
# Run all tests
nlm --test all

# Run specific test
nlm --test plasticity

# Run tests with output
nlm --test integration --output test_results.json
```

#### `nlm --analyze [options]`
**Description**: Analyze brain state and performance
**Usage**: `nlm --analyze [options]`
**Options**:
- `--brain-file <file>`: Load brain state from file
- `--metrics <type>`: Metrics to analyze: `basic|detailed|full`
- `--output <file>`: Save analysis results
- `--format <type>`: Output format: `text|json|csv`

**Examples**:
```bash
# Basic analysis
nlm --analyze --brain-file checkpoint.bin

# Detailed analysis with JSON output
nlm --analyze --brain-file checkpoint.bin --metrics full --format json --output analysis.json
```

#### `nlm --export [options]`
**Description**: Export NLM data and results
**Usage**: `nlm --export [options]`
**Options**:
- `--format <type>`: Export format: `json|csv|matlab|xml`
- `--output <file>`: Output file path
- `--brain-file <file>`: Brain state file to export
- `--data <type>`: Data to export: `neurons|synapses|memory|all`
- `--compress`: Compress output file

**Examples**:
```bash
# Export all data in JSON format
nlm --export --format json --data all --output results.json

# Export neurons in CSV format
nlm --export --format csv --data neurons --output neurons.csv

# Export with compression
nlm --export --format json --data all --output compressed_results.json --compress
```

### Advanced Commands

#### `nlm --config <file>`
**Description**: Load and validate configuration file
**Usage**: `nlm --config <file>`
**Output**: Configuration summary and validation results

#### `nlm --check`
**Description**: Verify system integrity
**Usage**: `nlm --check`
**Output**: System validation and health check results

#### `nlm --interactive`
**Description**: Launch interactive brain control TUI
**Usage**: `nlm --interactive`
**Output**: Interactive terminal user interface for brain control

## Configuration Management

### Configuration File Formats

NLM supports multiple configuration file formats:

#### 1. Default Configuration (`configs/default.cfg`)
```ini
brain.neuron_count=500
brain.region_count=1
brain.connection_probability=0.15
simulation.timestep=0.001
simulation.steps=10000
memory.replay_enabled=true
neuromodulation.dopamine_baseline=0.5
plasticity.stdp_ltp_strength=0.02
plasticity.stdp_ltd_strength=0.015
development.enabled=true
```

#### 2. JSON Configuration (`configs/nlm_config.json`)
```json
{
    "brain": {
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.15
    },
    "simulation": {
        "timestep": 0.001,
        "steps": 10000,
        "seed": 42
    },
    "memory": {
        "replay_enabled": true,
        "working_memory_size": 500
    }
}
```

#### 3. YAML Configuration (`configs/nlm_config.yaml`)
```yaml
brain:
  neuron_count: 500
  region_count: 1
  connection_probability: 0.15

simulation:
  timestep: 0.001
  steps: 10000
  seed: 42

memory:
  replay_enabled: true
  working_memory_size: 500
```

### Configuration Override

Command line arguments can override configuration file settings:

```bash
# Override specific parameters
nlm --simulate \
    --config configs/research.cfg \
    --steps 20000 \
    --neurons 1000 \
    --checkpoint my_experiment.bin

# Enable advanced features
nlm --simulate --neurons 2000 --steps 5000 --enable-replay --enable-development
```

## Results Export

NLM supports multiple output formats for results:

### 1. JSON Export
- **File extension**: `.json`
- **Structure**: Hierarchical JSON with all data
- **Usage**: Data analysis, integration with other tools

### 2. CSV Export
- **File extension**: `.csv`
- **Structure**: Tabular data for spreadsheets
- **Usage**: Data analysis, visualization

### 3. MATLAB Export
- **File extension**: `.mat`
- **Structure**: MATLAB-compatible binary format
- **Usage**: Mathematical analysis, MATLAB integration

### 4. XML Export
- **File extension**: `.xml`
- **Structure**: Standard XML format
- **Usage**: Data exchange, documentation

## System Monitoring

### Resource Monitoring
The NLM CLI tool includes built-in system monitoring capabilities:

- **CPU Usage**: Real-time CPU utilization monitoring
- **Memory Usage**: Memory allocation and usage tracking
- **Disk I/O**: File system activity monitoring
- **Network I/O**: Network traffic monitoring
- **System Load**: Overall system load average

### Performance Profiling
NLM includes performance profiling tools:

- **Timeline Analysis**: Event-by-event timing analysis
- **Bottleneck Detection**: Identification of performance bottlenecks
- **Optimization Recommendations**: Suggestions for performance improvements
- **Resource Leak Detection**: Detection of memory and resource leaks

## Testing and Benchmarking

### Test Suite
NLM includes comprehensive testing capabilities:

#### Unit Tests
- Core functionality tests
- Memory system tests
- Neuromodulation tests
- Plasticity tests
- Development tests
- Prediction tests
- Cognition tests
- Sensory-motor tests

#### Integration Tests
- Component integration tests
- System integration tests
- Regression tests
- Validation tests

### Benchmarking
NLM includes comprehensive benchmarking:

#### Performance Benchmarks
- **Small Scale**: 100 neurons, 1000 steps
- **Medium Scale**: 500 neurons, 5000 steps
- **Large Scale**: 1000 neurons, 10000 steps

#### Scaling Analysis
- Linear scaling analysis
- Memory efficiency analysis
- Performance per watt analysis

#### System Resource Analysis
- CPU performance profiling
- Memory usage analysis
- Disk I/O performance
- Network performance

## Configuration Templates

NLM provides configuration templates for different use cases:

### 1. Beginner Configuration (`configs/templates/beginner.cfg`)
- **Purpose**: Learning NLM basics
- **Neurons**: 100
- **Steps**: 500
- **Features**: Minimal configuration

### 2. Research Configuration (`configs/templates/research.cfg`)
- **Purpose**: Research-grade simulations
- **Neurons**: 5000
- **Steps**: 50000
- **Features**: Full configuration, advanced features

### 3. Production Configuration (`configs/templates/production.cfg`)
- **Purpose**: Production environments
- **Neurons**: 2000
- **Steps**: 20000
- **Features**: Performance optimized, checkpointing

## Error Handling and Diagnostics

### Common Error Messages

1. **Configuration File Not Found**
   ```
   ERROR: Configuration file not found: configs/my_config.cfg
   Using default configuration.
   ```

2. **Invalid Command**
   ```
   ERROR: Unknown command: --invalidcommand
   Available commands:
     --help, -h: Show help
     --version: Show version
     --simulate: Run simulation
     --benchmark: Run benchmarks
     --test: Run tests
     --analyze: Analyze brain state
     --export: Export results
   ```

3. **Invalid Argument**
   ```
   ERROR: Invalid value for --neurons: abc
   Usage: nlm --simulate --neurons <N>
   ```

4. **Export Error**
   ```
   ERROR: Failed to export results to: results.json
   Permission denied or invalid path.
   ```

5. **Benchmark Error**
   ```
   ERROR: Benchmark failed for configuration: research.cfg
   Reason: Memory limit exceeded.
   ```

### Diagnostic Commands

#### `nlm --check`
Verifies system integrity and checks for potential issues:

```bash
$ nlm --check
=== System Integrity Check ===
- Configuration files: OK
- Libraries and dependencies: OK
- Memory availability: OK
- Disk space: OK
- Network connectivity: OK

=== Configuration Validation ===
- Parameter ranges: OK
- Default values: OK
- File paths: OK

=== Performance Status ===
- CPU performance: Good
- Memory usage: Normal
- System resources: Available

=== Recommended Actions ===
- All systems operational
- No action required
```

## Scripting and Automation

### Command Line Arguments

NLM supports extensive command line arguments for scripting:

```bash
#!/bin/bash

# Run experiment with specific configuration
nlm --simulate \
    --config configs/research.cfg \
    --steps 10000 \
    --neurons 1000 \
    --checkpoint experiment1.bin

# Export results
nlm --export --format json --data all --output results.json

# Run analysis
nlm --analyze --brain-file experiment1.bin --metrics full --format json

# Run tests
nlm --test all --output test_results.json

# Run benchmarks
nlm --benchmark --range medium --output benchmark_results.json
```

### Environment Variables

NLM supports the following environment variables:

- `NLM_HOME`: NLM installation directory (default: current directory)
- `NLM_CONFIG`: Default configuration file path
- `NLM_LOG_LEVEL`: Logging level (debug, info, warning, error, critical)
- `NLM_VERBOSE`: Enable verbose output (true/false)
- `NLM_TIMEOUT`: Command timeout in seconds

```bash
export NLM_LOG_LEVEL=info
export NLM_VERBOSE=true
export NLM_TIMEOUT=300
```

## Troubleshooting Guide

### 1. Help Not Displayed
**Problem**: `nlm --help` shows nothing
**Solution**:
- Check if NLM is properly installed
- Verify executable permissions
- Check for environment variable conflicts

### 2. Configuration Not Loaded
**Problem**: Configuration file not found
**Solution**:
- Verify file path exists
- Check file permissions
- Use absolute paths for configuration files

### 3. Simulation Fails
**Problem**: Simulation crashes or hangs
**Solution**:
- Check available memory
- Reduce neuron count or steps
- Check for memory leaks
- Verify configuration parameters

### 4. Export Fails
**Problem**: Cannot export results
**Solution**:
- Check disk space
- Verify write permissions
- Check file path validity
- Try different export format

### 5. Benchmark Slow
**Problem**: Benchmark takes too long
**Solution**:
- Reduce scale of benchmark
- Increase timeout
- Check system resources
- Optimize configuration parameters

## Performance Optimization

### Configuration Optimization

1. **Memory Optimization**
   ```json
   {
       "performance": {
           "enable_vectorization": true,
           "use_memory_pools": true,
           "enable_parallel_processing": true
       }
   }
   ```

2. **CPU Optimization**
   ```json
   {
       "brain": {
           "neuron_count": 1000,
           "region_count": 1
       },
       "performance": {
           "enable_simd": true,
           "optimize_for_speed": true
       }
   }
   ```

3. **I/O Optimization**
   ```json
   {
       "output": {
           "enable_checkpointing": true,
           "checkpoint_frequency": 1000,
           "export_format": "compressed"
       }
   }
   ```

### System Optimization

1. **Hardware Recommendations**
   - Multi-core processor with 8+ cores
   - Minimum 8GB RAM (16GB+ recommended)
   - SSD storage for faster I/O
   - 1Gbps network for distributed simulations

2. **Software Recommendations**
   - Modern C++ compiler (GCC 9+ or Clang 10+)
   - OpenMP support for parallel processing
   - HDF5 library for large file I/O (optional)

## Migration from Previous Version

### From Hardcoded Tests to CLI

**Old System** (main.cpp with hardcoded test functions):
```cpp
void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    // Hardcoded test implementation
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    // Hardcoded experiment implementation
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    // Hardcoded STDP verification
}
```

**New System** (CLI with command handlers):
```bash
# Run basic connectivity test
nlm --test basic

# Run plasticity experiment
nlm --test plasticity

# Run STDP verification
nlm --test stdp

# Run simulation
nlm --simulate

# Run experiment with custom config
nlm --simulate --config configs/research.cfg
```

### Benefits of the New System

1. **Flexibility**: Command-driven execution instead of hardcoded flow
2. **Extensibility**: Easy to add new commands and features
3. **User-Friendly**: Intuitive command structure and help system
4. **Scriptable**: Full support for automation and scripting
5. **Configurable**: Multiple configuration file formats
6. **Comprehensive**: Includes benchmarking, testing, and monitoring
7. **Documented**: Complete documentation and examples
8. **Maintainable**: Modular design with clear separation of concerns

## Examples and Use Cases

### 1. Educational Use
```bash
# Simple demonstration for students
nlm --simulate --neurons 100 --steps 500

# Step-by-step learning
nlm --test basic
nlm --test plasticity
nlm --test stdp
```

### 2. Research Work
```bash
# Research simulation with analysis
nlm --simulate --config configs/research.cfg --checkpoint research_run.bin
nlm --analyze --brain-file research_run.bin --metrics full --format json
nlm --export --format json --data all --output research_data.json
```

### 3. Production Deployment
```bash
# Production simulation with monitoring
nlm --simulate --config configs/production.cfg

# Background monitoring
nlm --check --continuous

# Automated reporting
nlm --analyze --brain-file production_run.bin --format json | tee report.json
```

### 4. Performance Tuning
```bash
# Performance benchmark suite
nlm --benchmark --range small
nlm --benchmark --range medium
nlm --benchmark --range large

# Scaling analysis
nlm --benchmark --analyze-scaling --output scaling_analysis.txt
```

### 5. Automated Testing
```bash
#!/bin/bash

# Run complete test suite
nlm --test all --output test_results.json

# Run regression tests
nlm --test regression --output regression_results.json

# Compare with baseline
python analyze_results.py baseline.json current.json --output comparison.html
```

## Future Enhancements

The NLM CLI system is designed to be extensible with planned enhancements:

1. **Web Interface**: RESTful API for web-based interaction
2. **Graphical Interface**: GUI for visual simulation control
3. **Distributed Computing**: Support for multi-node simulations
4. **Advanced Analytics**: Machine learning for pattern detection
5. **Cloud Integration**: AWS/GCP integration for cloud deployments
6. **Plugin System**: Extensible plugin architecture for custom features
7. **Advanced Visualization**: 3D brain visualization and interactive exploration

## Conclusion

The NLM CLI tool represents a significant improvement over the previous hardcoded test system. It provides:

- **User-Friendly Interface**: Intuitive command structure with comprehensive help
- **Flexible Execution**: Command-driven execution for various use cases
- **Comprehensive Documentation**: Complete documentation and examples
- **Advanced Features**: Benchmarking, testing, monitoring, and export capabilities
- **Extensible Architecture**: Modular design for future enhancements
- **Production Ready**: Robust error handling and system monitoring

This system makes NLM accessible to beginners while providing powerful features for advanced users and researchers.