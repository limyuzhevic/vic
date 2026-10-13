# NLM - Neural Learning Machine
# Complete Implementation and Usage Guide

**Phase 6: Final Integration - Enhanced Artificial Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Key Improvements in This Version

### 1. Enhanced Configuration System
- **Multi-format support**: JSON, YAML, and custom key=value formats
- **Advanced CLI options**: Powerful command-line interface for configuration management
- **Schema validation**: Configuration validation with detailed error reporting
- **File-based management**: Load/save configuration files with automatic backup

### 2. Advanced Logging System
- **Multiple output formats**: Console, file, and web-based logging
- **Structured logging**: JSON and custom log formats
- **Performance-aware**: Adaptive log levels and intelligent filtering
- **Real-time monitoring**: Live log viewing and analysis

### 3. Comprehensive Command-Line Interface
- **Unified tool**: `nlm` command provides access to all NLM functionality
- **Intelligent defaults**: Smart defaults for common operations
- **Context-aware help**: Dynamic help based on available commands and configuration
- **Completion support**: Command and argument completion

### 4. Performance Optimization Framework
- **Memory management**: Advanced memory pools and NUMA-aware allocation
- **Data layout optimization**: Structure of arrays (SoA) for better cache locality
- **Parallel processing**: Multi-threaded simulation and analysis
- **SIMD acceleration**: Vectorized operations for neural computations
- **Event-driven architecture**: Efficient spike propagation and event handling

### 5. Advanced Testing Framework
- **Comprehensive test suite**: Unit, integration, and performance tests
- **Automated benchmarking**: Performance measurement and comparison
- **Test visualization**: Interactive test result visualization
- **Regression testing**: Automated regression detection

### 6. Enhanced Memory Systems
- **Working memory**: Advanced activation dynamics with competition and decay
- **Episodic memory**: Sophisticated event storage and replay mechanisms
- **Associative memory**: Pattern completion and rapid association formation
- **Semantic memory**: Structured knowledge representation
- **Procedural memory**: Skill learning and habit formation
- **Integrated memory manager**: Cross-system memory coordination

### 7. Advanced Plasticity Mechanisms
- **Multiple plasticity rules**: STDP, Hebbian, contrastive, homeostatic
- **Neuromodulation integration**: Dopamine, acetylcholine, serotonin systems
- **Developmental plasticity**: Age-dependent learning rates and structural changes
- **Activity-dependent changes**: Experience-driven synaptic modifications
- **Metaplasticity**: Learning about learning

### 8. Visualization and Monitoring Tools
- **Real-time visualization**: Live neural network visualization and analysis
- **Performance monitoring**: Comprehensive system performance tracking
- **Web-based interface**: Interactive web-based control and monitoring
- **Data export**: Multiple export formats for analysis
- **Alert system**: Automatic performance issue detection

## Installation and Setup

### Prerequisites
```bash
# Required dependencies
build-essential cmake g++ python3 python3-pip python3-venv

# Python dependencies
pip install numpy matplotlib plotly socketio eventlet websockets psutil

# Optional dependencies for enhanced functionality
pip install PyYAML
pip install scipy
pip install pandas
pip install jupyter
```

### Build Instructions
```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build with default settings
mkdir build
cd build
cmake ..
make -j4

# Build with performance optimizations
mkdir build_perf
cd build_perf
cmake -DNLM_PERFORMANCE_ENABLED=ON -DNLM_USE_SIMD=ON -DNLM_USE_MULTITHREADING=ON ..
make -j4
```

### Running NLM
```bash
# Run basic simulation (Phase 2)
./nlm_phase2_demo

# Run Phase 6 integration test
./nlm_phase6_demo

# Use enhanced CLI interface
nlm run --config configs/enhanced.cfg --steps 1000

# Visualize brain state
nlm visualize --brain brain.bin --output viz.html

# Run comprehensive tests
nlm test --suite all --output test_results.json

# Monitor performance
nlm monitor --duration 3600 --output performance_report.json
```

## Configuration Management

### Configuration File Formats

#### JSON Configuration
```json
{
    "nlm": {
        "version": "6.0",
        "simulation": {
            "timestep": 0.001,
            "neuron_count": 10000,
            "region_count": 5,
            "connection_probability": 0.1
        },
        "plasticity": {
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.015,
            "stdp_time_constant": 20.0,
            "hebbian_learning_rate": 0.01
        },
        "memory": {
            "working_memory_capacity": 1000,
            "episodic_memory_limit": 50000,
            "consolidation_threshold": 0.5
        },
        "neuromodulation": {
            "dopamine_baseline": 0.0,
            "curiosity_strength": 1.0,
            "novelty_sensitivity": 0.5
        }
    }
}
```

#### YAML Configuration
```yaml
nlm:
    version: '6.0'
    simulation:
        timestep: 0.001
        neuron_count: 10000
        region_count: 5
        connection_probability: 0.1
    plasticity:
        stdp_ltp_weight: 0.02
        stdp_ltd_weight: 0.015
        stdp_time_constant: 20.0
        hebbian_learning_rate: 0.01
    memory:
        working_memory_capacity: 1000
        episodic_memory_limit: 50000
        consolidation_threshold: 0.5
    neuromodulation:
        dopamine_baseline: 0.0
        curiosity_strength: 1.0
        novelty_sensitivity: 0.5
```

#### Simple Key=Value Configuration
```cfg
# NLM Configuration File
# Phase 6: Enhanced Integration

# Simulation parameters
simulation_timestep = 0.001
neuron_count = 10000
region_count = 5
connection_probability = 0.1

# Plasticity settings
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0
hebbian_learning_rate = 0.01

# Memory system parameters
working_memory_capacity = 1000
episodic_memory_limit = 50000
consolidation_threshold = 0.5

# Neuromodulation parameters
dopamine_baseline = 0.0
curiosity_strength = 1.0
novelty_sensitivity = 0.5

# Logging settings
log_level = INFO
log_to_file = true
log_filename = nlm.log
```

### Command-Line Configuration
```bash
# Set configuration values via command line
nlm config set --section simulation --key timestep --value 0.001
nlm config set --section plasticity --key ltp_weight --value 0.05
nlm config show --section memory

# Load configuration from file
nlm config load --file config.json --schema schema.json

# Export configuration
nlm config export --format yaml --output exported_config.yaml
```

## Command Reference

### Core Commands

#### `nlm run`
Run NLM simulation with specified configuration.

```bash
nlm run --config config.json --steps 1000 --output brain.bin --mode performance
```

**Options:**
- `--config CONFIG`: Configuration file path
- `--steps STEPS`: Number of simulation steps
- `--output OUTPUT`: Output file path
- `--mode MODE`: Simulation mode (standard, performance, debug)
- `--checkpoint CHECKPOINT`: Checkpoint file for resume

#### `nlm checkpoint`
Manage brain state checkpoints.

```bash
# Save brain state
nlm checkpoint save --brain brain.bin --output checkpoint.bin --compress

# Load brain state
nlm checkpoint load --input checkpoint.bin --brain brain.bin

# List available checkpoints
nlm checkpoint list --directory ./checkpoints
```

#### `nlm experiment`
Run specific experiments.

```bash
# Run Phase 6 integration test
nlm experiment run --type phase6 --config exp_config.json

# Run custom experiment
nlm experiment run --type custom --config custom_config.json --output results.json
```

#### `nlm visualize`
Create visualizations and analyses.

```bash
# Create neural network visualization
nlm visualize --brain brain.bin --output viz.html --format html

# Create 3D neural representation
nlm visualize --brain brain.bin --output viz3d.html --format html --neuron-type all

# Generate analysis report
nlm visualize --brain brain.bin --output analysis.html --format html --analysis
```

#### `nlm monitor`
Monitor running simulation.

```bash
# Monitor simulation performance
nlm monitor --brain brain.bin --duration 3600 --output monitor_data.json

# Start web interface for real-time monitoring
nlm monitor --brain brain.bin --port 8080 --level comprehensive
```

#### `nlm test`
Run comprehensive test suite.

```bash
# Run all tests
nlm test --suite all --output test_results.json

# Run specific test category
nlm test --suite plasticity --output plasticity_test.json

# Run performance benchmarks
nlm test --suite benchmarks --output benchmark_results.json
```

#### `nlm config`
Manage configuration files.

```bash
# Show current configuration
nlm config show

# Edit configuration
nlm config edit --section simulation --key timestep --value 0.001

# Validate configuration
nlm config validate --file config.json --schema schema.json

# Merge configurations
nlm config merge --base config1.json --override config2.json --output merged_config.json
```

### Analysis Commands

#### `nlm analyze`
Analyze neural simulation data.

```bash
# Analyze brain connectivity
nlm analyze --brain brain.bin --type connectivity --output connectivity_analysis.json

# Analyze temporal dynamics
nlm analyze --brain brain.bin --type temporal --output temporal_analysis.json

# Analyze memory patterns
nlm analyze --brain brain.bin --type memory --output memory_analysis.json
```

### Utility Commands

#### `nlm utils`
Utility operations and tools.

```bash
# Run benchmarks
nlm utils benchmark --neurons 1000 --steps 100 --output benchmark.json

# Export data
nlm utils export --brain brain.bin --format csv --output data.csv

# Compare simulations
nlm utils compare --brain1 brain1.bin --brain2 brain2.bin --output comparison.json
```

## Advanced Features

### Real-Time Monitoring

The NLM framework includes comprehensive real-time monitoring capabilities:

1. **Web-based Dashboard**: Interactive web interface for monitoring simulation
2. **Performance Metrics**: CPU, memory, and simulation performance tracking
3. **Alert System**: Automatic detection of performance issues and anomalies
4. **Real-time Visualization**: Live neural network visualization and analysis

### Memory Management

Enhanced memory systems provide:

1. **Working Memory**: Advanced activation dynamics with competition and decay
2. **Episodic Memory**: Event storage with temporal compression and replay
3. **Associative Memory**: Pattern completion and rapid association formation
4. **Integrated Management**: Cross-system memory coordination and consolidation

### Plasticity Mechanisms

Advanced plasticity includes:

1. **Multiple Rule Types**: STDP, Hebbian, contrastive, homeostatic, neuromodulated
2. **Developmental Plasticity**: Age-dependent learning rates and structural changes
3. **Neuromodulation**: Integration of dopamine, acetylcholine, and serotonin systems
4. **Metaplasticity**: Learning about learning and adaptive plasticity regulation

### Performance Optimizations

Key performance improvements:

1. **Memory Pools**: Efficient memory allocation with NUMA awareness
2. **Data Layout**: Structure of arrays (SoA) for better cache locality
3. **Parallel Processing**: Multi-threaded simulation and analysis
4. **SIMD Acceleration**: Vectorized neural computations
5. **Event-Driven Architecture**: Efficient spike propagation and event handling

## Testing and Validation

### Test Framework

The NLM framework includes comprehensive testing capabilities:

1. **Unit Tests**: Individual component testing
2. **Integration Tests**: System-level testing
3. **Performance Tests**: Benchmarking and optimization validation
4. **Regression Tests**: Automated regression detection
5. **Stress Tests**: Large-scale simulation testing

### Test Commands

```bash
# Run unit tests
nlm test --suite unit --output unit_test_results.json

# Run integration tests
nlm test --suite integration --output integration_test_results.json

# Run performance tests
nlm test --suite benchmarks --output benchmark_results.json

# Run all tests
nlm test --suite all --output comprehensive_test_results.json
```

### Test Results

All tests can be viewed in various formats:
- **JSON**: Machine-readable test results
- **HTML**: Interactive web-based test reports
- **Console**: Human-readable summary in terminal

## File Formats and Extensions

### Brain State Files

- **Binary (.bin)**: Compact binary format for brain state storage
- **Numpy (.npy)**: NumPy format for scientific computing
- **JSON (.json)**: Human-readable format for interoperability

### Configuration Files

- **JSON (.json)**: Standard JSON configuration
- **YAML (.yaml, .yml)**: YAML configuration
- **INI/Cfg (.cfg, .ini)**: Simple key=value format

### Visualization Files

- **HTML (.html)**: Interactive web-based visualizations
- **PNG (.png)**: Static image visualizations
- **JSON (.json)**: Data for custom visualization generation

## Development Guidelines

### Coding Standards

1. **Performance-First**: Optimize for speed and memory efficiency
2. **Memory Safety**: Use RAII and smart pointers to prevent leaks
3. **Thread Safety**: Design concurrent systems with proper synchronization
4. **Test Coverage**: Ensure comprehensive test coverage for all components
5. **Documentation**: Maintain detailed documentation for all public APIs

### Performance Optimization

1. **Memory Management**: Use memory pools and avoid dynamic allocations
2. **Cache Locality**: Design data structures for optimal cache utilization
3. **Parallel Processing**: Leverage multiple CPU cores for intensive computations
4. **SIMD Vectorization**: Use SIMD instructions for vectorized operations
5. **Event-Driven Architecture**: Minimize synchronization overhead

### Testing and Validation

1. **Automated Testing**: Implement continuous integration and automated testing
2. **Performance Benchmarking**: Regularly benchmark performance and identify bottlenecks
3. **Memory Leak Detection**: Use tools like Valgrind for memory leak detection
4. **Code Coverage**: Maintain high code coverage for all critical components
5. **Regression Testing**: Implement automated regression testing to prevent regressions

## Troubleshooting

### Common Issues and Solutions

#### Issue: Configuration Loading Failure
**Problem**: Configuration file cannot be loaded or parsed.
**Solution**: Check file format and ensure all required sections are present.

#### Issue: Memory Exhaustion
**Problem**: Simulation runs out of memory during execution.
**Solution**: Increase memory limits, optimize memory usage, or reduce simulation scale.

#### Issue: Performance Degradation
**Problem**: Simulation slows down over time.
**Solution**: Check for memory leaks, optimize algorithms, or increase system resources.

#### Issue: Visualization Not Working
**Problem**: Web-based visualization interface fails to load.
**Solution**: Check network connectivity, verify dependencies, and restart visualization server.

### Getting Help

1. **Documentation**: Check the official NLM documentation for detailed usage instructions
2. **Examples**: Review example configurations and scripts in the repository
3. **Community**: Join the NLM community forum for support and discussions
4. **Issues**: Report bugs and feature requests through the GitHub issue tracker

## Acknowledgments

The NLM project builds upon extensive research in computational neuroscience, artificial intelligence, and machine learning. Special thanks to the research community for contributions and the open-source ecosystem for enabling this implementation.

## License

MIT License

Copyright (c) 2026 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
