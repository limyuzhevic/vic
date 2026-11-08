# NLM — Documentation Improvements

This document provides comprehensive documentation for the NLM (Neural Learning Machine) project, including information about the improvements made during Phase 2 and later phases.

## Overview

NLM is an experimental artificial developmental brain project that implements real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

## Improvements Summary

This document covers the following major improvements made to the NLM codebase:

1. **Enhanced Main Demo (`src/main.cpp`)**
   - Advanced test patterns for plasticity, STDP, and neuromodulation
   - Comprehensive user interface with command-line options
   - Support for multiple demo modes (basic, plasticity, stdp, all, benchmark)
   - Enhanced performance monitoring and reporting

2. **Configuration Management**
   - Comprehensive configuration file examples in `configs/` directory
   - Support for different use cases (small, large, learning, development)
   - Command-line override capabilities

3. **Logging and Error Handling**
   - Enhanced logging system with error callbacks and recovery modes
   - Log rotation and directory management
   - Improved error handling and reporting

4. **Code Organization**
   - Improved Brain.cpp code organization
   - Removal of commented-out experimental code
   - Better documentation and comments

5. **Advanced Demo Modes**
   - Sophisticated plasticity tests
   - Performance benchmarking capabilities
   - Memory system testing
   - Neuromodulation system testing

## Usage Guide

### Running NLM

#### Basic Usage

Run the enhanced NLM demo with default settings:

```bash
./nlm_advanced
```

This runs all available tests in the default demo mode.

#### Command Line Options

```bash
./nlm_advanced --help
```

Shows all available command-line options.

#### Demo Modes

The enhanced NLM supports multiple demo modes:

- `basic`: Basic neural connectivity tests
- `plasticity`: Advanced plasticity learning experiments
- `stdp`: STDP (Spike-Timing-Dependent Plasticity) verification
- `benchmark`: Performance benchmarking
- `memory`: Memory systems testing
- `neuromod`: Neuromodulation systems testing
- `all`: Run all tests (default)

Example usage:

```bash
# Run plasticity demo with custom settings
./nlm_advanced --demo-mode plasticity --steps 1000 --neurons 200

# Run benchmark
./nlm_advanced --demo-mode benchmark --steps 5000

# Run memory test
./nlm_advanced --demo-mode memory --neurons 100
```

#### Configuration Files

NLM supports configuration files located in the `configs/` directory:

- `configs/default.cfg`: Standard configuration
- `configs/small.cfg`: Resource-constrained configuration
- `configs/large.cfg`: High-performance configuration
- `configs/learning.cfg`: Intensive learning configuration
- `configs/development.cfg`: Development/debug configuration

To use a specific configuration file:

```bash
./nlm_advanced --config configs/learning.cfg
```

#### Command Line Overrides

Command-line arguments can override configuration file settings:

```bash
./nlm_advanced --config configs/small.cfg --neurons 200 --seed 777 --demo-mode plasticity
```

### Advanced Features

#### 1. Performance Monitoring

The enhanced NLM includes comprehensive performance monitoring:

- Steps per second calculation
- Total spike counts
- Average firing rates
- Excitation/inhibition ratios
- Memory system statistics
- Neuromodulator levels

#### 2. Memory Systems Testing

Test the integrated memory systems:

- **Working Memory**: Transient active information storage
- **Episodic Memory**: Experience and event recording
- **Associative Memory**: Pattern associations

#### 3. Neuromodulation Systems

Test the neuromodulation systems:

- **Dopamine**: Reward and reinforcement
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection
- **Prediction Error**: Learning signals

#### 4. Enhanced Logging

The enhanced logging system provides:

- Detailed log entries with source location
- Log level filtering (Debug, Info, Warning, Error, Critical)
- Color-coded console output
- File output with rotation
- Error callbacks for error handling
- Session management

#### 5. Error Handling

Enhanced error handling includes:

- Error callbacks for custom error handling
- Recovery modes for graceful error recovery
- Comprehensive error reporting
- System error stream redirection

## Configuration File Reference

### Configuration Keys

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `random_seed` | int64_t | 42 | Random number generator seed |
| `simulation_timestep` | double | 0.001 | Time step duration in seconds |
| `neuron_count` | int64_t | 500 | Number of neurons in the brain |
| `region_count` | int64_t | 1 | Number of neural regions |
| `connection_probability` | float | 0.15 | Probability of synapse creation |
| `stdp_ltp_weight` | float | 0.02 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.015 | STDP long-term depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant |
| `synaptogenesis_rate` | float | 0.0001 | Rate of new synapse creation |
| `pruning_rate` | float | 0.00001 | Rate of synapse elimination |
| `replay_interval` | int64_t | 100 | Steps between memory replay cycles |
| `consolidation_interval` | int64_t | 1000 | Steps between memory consolidation |
| `checkpoint_dir` | string | "./checkpoints" | Directory for saving brain state |
| `checkpoint_interval` | int64_t | 10000 | Steps between automatic checkpoints |

### Configuration File Examples

#### Small Configuration (`configs/small.cfg`)

Optimized for systems with limited memory and processing power:

```
random_seed = 123
simulation_timestep = 0.001
neuron_count = 100
region_count = 1
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 15.0
synaptogenesis_rate = 0.00005
pruning_rate = 0.000005
replay_interval = 50
consolidation_interval = 500
checkpoint_dir = ./checkpoints_small
checkpoint_interval = 5000
```

#### Large Configuration (`configs/large.cfg`)

Designed for systems with substantial computational resources:

```
random_seed = 999
simulation_timestep = 0.0005
neuron_count = 2000
region_count = 2
connection_probability = 0.2
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.02
stdp_tau = 25.0
synaptogenesis_rate = 0.0002
pruning_rate = 0.00002
replay_interval = 200
consolidation_interval = 2000
checkpoint_dir = ./checkpoints_large
checkpoint_interval = 20000
```

#### Learning Configuration (`configs/learning.cfg`)

Optimized for maximum learning and memory consolidation:

```
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 1
connection_probability = 0.18
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.025
stdp_tau = 30.0
synaptogenesis_rate = 0.0003
pruning_rate = 0.00001
replay_interval = 50
consolidation_interval = 200
checkpoint_dir = ./checkpoints_learning
checkpoint_interval = 2000
```

## Advanced Usage

### 1. Custom Demo Scripts

Create custom demo scripts that leverage NLM's advanced features:

```bash
#!/bin/bash

# Custom demo script for NLM

echo "=== NLM Custom Demo ==="

# Run plasticity test with custom settings
./nlm_advanced --demo-mode plasticity --steps 2000 --neurons 800 \
              --config configs/learning.cfg

# Run memory test
./nlm_advanced --demo-mode memory --neurons 300

# Run neuromodulation test
./nlm_advanced --demo-mode neuromod

echo "=== Demo Complete ==="
```

### 2. Integration with Other Systems

NLM can be integrated with other systems through its logging and error handling interfaces:

```cpp
// Example error callback
auto errorCallback = [](const LogEntry& entry) {
    std::cerr << "[CUSTOM ERROR] " << entry.message << std::endl;
    // Additional error handling logic here
};

// Set up enhanced logging
EnhancedLogger::InitializeEnhancedLogging("./logs");
EnhancedLogger::getGlobal().setErrorHandler(errorCallback);
```

### 3. Performance Tuning

For optimal performance, consider these tuning parameters:

- **Small systems (≤100 neurons)**: Use `configs/small.cfg`
- **Medium systems (100-1000 neurons)**: Use `configs/default.cfg`
- **Large systems (>1000 neurons)**: Use `configs/large.cfg`
- **Learning-focused experiments**: Use `configs/learning.cfg`

### 4. Development and Debugging

For development and debugging:

- Use `configs/development.cfg` for rapid iteration
- Enable verbose logging with `--verbose`
- Use `--quiet` mode for production runs
- Monitor log files in the log directory

## Development Guide

### 1. Adding New Demo Modes

To add a new demo mode:

1. Create a new function in `src/main.cpp`
2. Add the function to the demo mode handling logic
3. Update the help text with the new option

Example:

```cpp
void runCustomTest(std::shared_ptr<Brain> brain, size_t steps) {
    // Custom test implementation
}
```

### 2. Extending Configuration Options

To add new configuration options:

1. Add the option to `Config` class
2. Update the configuration file examples
3. Add command-line argument parsing
4. Update the default configuration

### 3. Improving Logging

To enhance logging:

1. Add new log levels if needed
2. Extend log entry structure
3. Add log filtering options
4. Implement log analytics

### 4. Adding Error Handling

To add error handling:

1. Implement error callbacks
2. Add recovery modes
3. Extend error reporting
4. Add error recovery strategies

## Troubleshooting

### Common Issues and Solutions

#### Issue: "Failed to initialize brain!"

**Solution**: Check that all required dependencies are properly initialized. Ensure that the configuration file is correctly formatted and contains all required fields.

#### Issue: Performance degradation with large neuron counts

**Solution**: Use `configs/large.cfg` for large systems and consider using systems with more memory.

#### Issue: Memory usage too high

**Solution**: Reduce `neuron_count` or use `configs/small.cfg`. Monitor memory usage during operation.

#### Issue: Log files not being created

**Solution**: Ensure the log directory exists and is writable. Set up proper error handling for directory creation failures.

#### Issue: Error handling not working

**Solution**: Check that error callbacks are properly registered and that error handlers return appropriate values.

### Debugging Commands

#### Verbose Logging

```bash
./nlm_advanced --verbose --demo-mode plasticity
```

#### Quiet Mode

```bash
./nlm_advanced --quiet --demo-mode benchmark
```

#### Debug Configuration

```bash
./nlm_advanced --config configs/development.cfg --verbose
```

## Migration Guide

### From Previous Versions

#### Upgrading from Phase 1/2

If upgrading from earlier NLM versions:

1. **Configuration Files**: Migrate settings from old configuration format to the new key-value format
2. **Command Line**: Update command-line arguments to use the new `--demo-mode` syntax
3. **Demo Options**: Add `--demo-mode` option for different test scenarios
4. **Logging**: Enable enhanced logging with `--verbose` option
5. **Error Handling**: Set up error callbacks for better error reporting

#### Configuration Migration Example

Old configuration format:

```
# old_config.txt
neuron_count = 500
region_count = 1
stdp_ltp_weight = 0.01
```

New configuration format:

```
# default.cfg
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
```

### Migration Steps

1. **Backup**: Backup existing configuration files and logs
2. **Test**: Test the new configuration with `--demo-mode basic`
3. **Compare**: Compare results between old and new configurations
4. **Adjust**: Adjust parameters as needed for desired behavior
5. **Deploy**: Deploy the new configuration for production use

## Best Practices

### 1. Configuration Management

- Use configuration files for different environments (development, testing, production)
- Keep configuration files in version control
- Document configuration options clearly
- Test configurations with different settings

### 2. Logging and Monitoring

- Enable appropriate log levels for different environments
- Set up log rotation for long-running processes
- Monitor error logs for potential issues
- Use structured logging for better analysis

### 3. Error Handling

- Implement error callbacks for custom error handling
- Use recovery modes for graceful error recovery
- Log errors with sufficient context for debugging
- Test error scenarios regularly

### 4. Performance Optimization

- Profile performance for large-scale simulations
- Use appropriate configuration for system capabilities
- Monitor memory and CPU usage
- Optimize configuration for specific use cases

### 5. Testing and Validation

- Test all demo modes before deployment
- Validate configurations with different settings
- Monitor performance over time
- Test error scenarios and recovery

## Future Enhancements

### 1. Advanced Features

- **Real-time visualization**: Interactive visualization of brain activity
- **Distributed computing**: Support for parallel and distributed computing
- **Cloud integration**: Integration with cloud computing platforms
- **API interface**: REST API for remote brain interaction

### 2. Performance Improvements

- **GPU acceleration**: Leverage GPU for faster computation
- **Optimization**: Further optimize performance for large-scale simulations
- **Memory management**: Improved memory management for large systems
- **Caching**: Implement caching for frequently accessed data

### 3. Research Applications

- **Neuroscience**: Apply to neuroscience research
- **AI integration**: Integrate with other AI systems
- **Robotics**: Apply to robotics control systems
- **Education**: Use for educational purposes

## Conclusion

The NLM project has been significantly enhanced with improved test patterns, advanced user interface, comprehensive configuration management, enhanced logging and error handling, and better code organization. These improvements make NLM more accessible, reliable, and suitable for both research and educational purposes.

The enhanced NLM provides:

- Better user experience with intuitive command-line interface
- Comprehensive testing capabilities with advanced demo modes
- Robust configuration management with flexible options
- Improved reliability with enhanced logging and error handling
- Better performance with optimized test patterns
- Comprehensive documentation for users and developers

These improvements position NLM as a state-of-the-art tool for neural simulation research and education.

---

*Last Updated: 2026-11-08*
*NLM Version: 0.1.0 (Phase 2 Enhanced)*
*Project: Neural Learning Machine (熙然)*