# NLM Documentation
# Comprehensive Guide for Neural Learning Machine

This document provides comprehensive documentation for the NLM (Neural Learning Machine) project, including all new improvements and features.

## Table of Contents

1. [Introduction](#introduction)
2. [Project Overview](#project-overview)
3. [Architecture](#architecture)
4. [Configuration Management](#configuration-management)
5. [Command-Line Interface](#command-line-interface)
6. [Error Handling](#error-handling)
7. [Performance Monitoring](#performance-monitoring)
8. [Neural Computation](#neural-computation)
9. [Memory Systems](#memory-systems)
10. [Neuromodulation](#neuromodulation)
11. [Development and Plasticity](#development-and-plasticity)
12. [Integration and Testing](#integration-and-testing)
13. [Performance Optimization](#performance-optimization)
14. [Migration Guide](#migration-guide)
15. [Troubleshooting](#troubleshooting)
16. [API Reference](#api-reference)
17. [Best Practices](#best-practices)
18. [Contributing](#contributing)
19. [License](#license)

## Introduction

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

### Key Features

- **Phase-based Development**: 6-phase developmental architecture (Phase 1: Basic setup, Phase 6: Final integration)
- **Real Spiking Neural Computation**: Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation
- **Integrated Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, prediction error
- **Prediction and Planning**: Neural prediction system and concept formation
- **Developmental Plasticity**: Synaptogenesis, pruning, structural plasticity
- **Checkpointing and Replay**: Save/load brain state, memory replay
- **Performance Monitoring**: Comprehensive metrics collection and analysis
- **Enhanced Configuration**: Environment variables, validation, error handling
- **Improved Command-Line Interface**: Help system, argument validation, presets

## Project Overview

### Core Components

1. **Brain Class**: Central coordinator of the neural system
2. **Neural Regions**: Organization of neurons into functional regions
3. **Memory Systems**: Working, episodic, and associative memory
4. **Neuromodulation Systems**: Chemical signals affecting neural activity
5. **Prediction System**: Forward and inverse models
6. **Cognition Systems**: Planning, attention, concept formation
7. **Development System**: Ontogenetic changes and maturation
8. **Plasticity Rules**: STDP and Hebbian learning
9. **Spike System**: Event-driven spike propagation

### Development Phases

- **Phase 1**: Project skeleton, core utilities, neural interfaces
- **Phase 2**: Real LIF neuron dynamics, plasticity rules
- **Phase 3**: World interaction loop, sensory/motor systems
- **Phase 4**: Prediction system, working memory, episodic memory
- **Phase 5**: Performance optimizations, SIMD, parallel processing
- **Phase 6**: Final integration of all systems

## Configuration Management

### Configuration Files

NLM supports multiple configuration sources with hierarchical precedence:

1. **Default Configuration**: Built-in defaults
2. **Configuration File**: JSON or key=value format
3. **Environment Variables**: NLM_ prefix
4. **Command Line Arguments**: --key=value format
5. **Runtime Configuration**: Programmatic API

### Configuration File Format (JSON)

```json
{
  "version": "0.1.0",
  "neuron_count": 1000,
  "region_count": 1,
  "connection_probability": 0.1,
  "simulation_timestep": 0.001,
  "random_seed": 42,
  "enable_development": true,
  "enable_checkpointing": true,
  "checkpoint_dir": "./checkpoints",
  "verbose": false,
  "debug": false,
  "phase": "6"
}
```

### Configuration File Format (Legacy)

```
# Configuration file for NLM
# Comments start with # or /

neuron_count = 1000
region_count = 1
connection_probability = 0.1
simulation_timestep = 0.001
random_seed = 42
enable_development = true
enable_checkpointing = true
checkpoint_dir = ./checkpoints
verbose = false
debug = false
phase = 6
```

### Environment Variables

All configuration keys can be set via environment variables with NLM_ prefix:

```bash
export NLM_NEURON_COUNT=2000
export NLM_SIMULATION_TIMESTEP=0.0005
export NLM_ENABLE_DEVELOPMENT=true
export NLM_VERBOSE=true
export NLM_DEBUG=false
export NLM_PHASE=6
```

### Validation Rules

Configuration values are validated against the following rules:

- **Neuron Count**: Must be >= 1, <= 100000
- **Region Count**: Must be >= 1, <= 100
- **Connection Probability**: Must be between 0.0 and 1.0
- **Simulation Timestep**: Must be > 0.0, <= 1.0
- **Random Seed**: Must be >= 0
- **Phase**: Must be 1, 2, 3, 4, 5, or 6

## Command-Line Interface

### Basic Usage

```bash
# Show help
nlm --help
nlm -h

# Show version information
nlm --version
nlm -v

# Enable verbose output
nlm --verbose
nlm -V

# Enable debug output
nlm --debug
nlm -d

# Set configuration values
nlm --neuron_count=2000 --simulation_timestep=0.0005
nlm -n 2000 -t 0.0005

# Load configuration from file
nlm config_file.json
nlm config_file.cfg

# Environment variables override configuration file
NLM_NEURON_COUNT=5000 ./nlm config_file.json
```

### Command-Line Options

#### General Options

| Option | Short Form | Description |
|--------|-------------|-------------|
| `--help`, `-h` | -h | Show help message |
| `--version`, `-v` | -v | Show version information |
| `--verbose`, `-V` | -V | Enable verbose output |
| `--debug`, `-d` | -d | Enable debug output |

#### Configuration Options

| Option | Short Form | Description |
|--------|-------------|-------------|
| `--neuron_count=N` | `-n N` | Number of neurons |
| `--region_count=N` | -r N | Number of regions |
| `--connection_probability=X` | -c X | Connection probability |
| `--simulation_timestep=X` | `-t X` | Simulation timestep |
| `--random_seed=N` | -seed N | Random seed |
| `--enable_development=true` | -dev true | Enable developmental plasticity |
| `--enable_checkpointing=true` | -ckpt true | Enable checkpoint saving |
| `--checkpoint_dir=PATH` | -dir PATH | Checkpoint directory |
| `--phase=N` | -phase N | Development phase (1-6) |
| `--config=FILE` | -f FILE | Load configuration from file |

#### Advanced Options

| Option | Description |
|--------|-------------|
| `--preset=NAME` | Apply preset configuration |
| `--help-config` | Show configuration help |
| `--list-config` | List all configuration options |
| `--experiment=NAME` | Run specific experiment |
| `--experiment-config=FILE` | Load experiment configuration |
| `--max_steps=N` | Maximum simulation steps |
| `--enable_replay=true` | Enable memory replay |
| `--enable_novelty=true` | Enable novelty detection |
| `--enable_curiosity=true` | Enable curiosity system |
| `--enable_prediction=true` | Enable prediction system |

### Configuration Presets

| Preset | Description |
|--------|-------------|
| `quick` | Quick test mode (small scale) |
| `standard` | Standard configuration |
| `production` | Production configuration |
| `research` | Research configuration (larger scale) |

```bash
# Apply preset configuration
nlm --preset=quick --neuron_count=100
nlm --preset=research --neuron_count=10000 --enable_development=true
```

## Error Handling

### Error Categories

NLM uses a comprehensive error handling system with the following categories:

1. **System**: System-level errors (memory, OS, etc.)
2. **Configuration**: Configuration errors
3. **Neural**: Neural network errors
4. **Simulation**: Simulation-specific errors
5. **Memory**: Memory management errors
6. **IO**: Input/output errors
7. **Network**: Network/communication errors
8. **Plasticity**: Plasticity rule errors
9. **Development**: Development system errors
10. **Neuromodulation**: Neuromodulation errors
11. **MemorySystem**: Memory system errors
12. **Prediction**: Prediction system errors
13. **Cognition**: Cognition system errors
14. **Validation**: Data validation errors
15. **Performance**: Performance monitoring errors
16. **Security**: Security-related errors
17. **File**: File system errors
18. **Resource**: Resource management errors
19. **Plugin**: Plugin/extension errors
20. **User**: User input errors
21. **Experimental**: Experimental/unknown errors

### Error Severity Levels

1. **Debug**: Debug information
2. **Info**: Informational messages
3. **Warning**: Warning conditions
4. **Error**: Error conditions
5. **Critical**: Critical errors that may cause failure
6. **Fatal**: Fatal errors that require immediate attention

### Error Recovery Actions

1. **None**: No recovery action
2. **Retry**: Retry the operation
3. **RetryWithBackoff**: Retry with exponential backoff
4. **Skip**: Skip the problematic operation
5. **GracefulDegrade**: Degrade functionality gracefully
6. **Revert**: Revert to previous state
7. **Shutdown**: Shutdown system
8. **ReportAndExit**: Report error and exit
9. **Continue**: Continue with degraded functionality
10. **LogAndContinue**: Log error and continue
11. **Alert**: Send alert notification
12. **Restart**: Restart affected component
13. **Fallforward**: Continue with next available path

### Error Handling Example

```cpp
#include "core/Error/ErrorHandling.hpp"

void performOperation() {
    try {
        // Your code here
        if (someCondition) {
            throw std::runtime_error("Invalid configuration detected");
        }
    } catch (const std::exception& e) {
        // Enhanced error reporting with context
        addSystemError(e, "Failed to initialize neural system");
        
        // Or use convenience functions
        logError("Operation failed: " + std::string(e.what()), 
                ErrorCategory::Neural, 
                ErrorSeverity::Error,
                ErrorRecoveryAction::GracefulDegrade);
    }
}
```

### Error Reporting

Errors can be reported in various formats:

1. **Console**: Text output to standard error
2. **JSON**: Structured error reports for programmatic consumption
3. **Log Files**: Persistent error logging
4. **Alert Systems**: Email, webhook, or other notification systems

## Performance Monitoring

### Performance Metrics

NLM collects comprehensive performance metrics:

#### System Metrics
- **CPU Usage**: Percentage of CPU utilization
- **Memory Usage**: Percentage of memory utilization
- **Network I/O**: Upload and download speeds
- **Disk I/O**: Read and write speeds
- **Temperature**: System temperature
- **Voltage**: CPU voltage
- **Frequency**: CPU operating frequency
- **Load Average**: System load averages

#### Neural Metrics
- **Neuron Count**: Number of active neurons
- **Synapse Count**: Number of active synapses
- **Spike Count**: Number of spikes generated
- **Firing Rate**: Average firing rate (spikes per second)
- **Computation Time**: Simulation time per step

### Performance Monitoring API

```cpp
#include "core/Performance/PerformanceMonitoring.hpp"

void setupPerformanceMonitoring() {
    // Start monitoring
    startMonitoring();
    
    // Add alert listener
    addPerformanceAlertListener([](const PerformanceAlert& alert) {
        std::cout << "PERFORMANCE ALERT: " << alert.message << std::endl;
        if (alert.type == PerformanceAlert::AlertType::CPUUsageHigh) {
            std::cout << "CPU usage is too high: " << alert.value << "% > " << alert.threshold << "%" << std::endl;
        }
    });
    
    // Record performance sample (called periodically)
    PerformanceMetrics metrics;
    // ... populate metrics ...
    recordSample(metrics);
}

void performanceWorker() {
    PerformanceMetrics metrics = getCurrentMetrics();
    
    // Do neural computation
    // ...
    
    // Record performance
    recordSample(metrics);
}
```

### Performance Benchmarks

Track performance of specific operations:

```cpp
void benchmarkNeuralComputation() {
    // Start benchmark
    PerformanceBenchmark bench = startBenchmark("Neural Computation");
    
    // Perform neural computation
    for (int i = 0; i < 1000; ++i) {
        // ... neural computation ...
    }
    
    // End benchmark
    endBenchmark(bench);
    
    // Report results
    std::cout << "Benchmark: " << bench.benchmarkName << std::endl;
    std::cout << "Duration: " << bench.duration << "s" << std::endl;
    std::cout << "FPS: " << bench.getFPS() << std::endl;
    std::cout << "Average Metric: " << bench.getAverageMetric("cpu_usage") << std::endl;
}
```

### Performance Profiling

Profile specific functions for optimization:

```cpp
void profileExpensiveFunction() {
    // Start profiling
    startProfiling("ExpensiveFunction");
    
    // Perform expensive operations
    // ...
    
    // Stop profiling
    stopProfiling();
}

void analyzePerformance() {
    // Get profile data
    auto profiles = getProfileData();
    
    // Print profile report
    printProfileReport();
    
    // Find most expensive function
    ProfileData mostExpensive = profiles[0];
    for (const auto& profile : profiles) {
        if (profile.duration > mostExpensive.duration) {
            mostExpensive = profile;
        }
    }
    
    std::cout << "Most expensive function: " << mostExpensive.functionName 
              << " (" << mostExpensive.duration << "s)" << std::endl;
}
```

## Neural Computation

### Neuron Dynamics

NLM implements Leaky Integrate-and-Fire (LIF) neurons with the following dynamics:

```math
τ * dV/dt = - (V - V_reset) + I
```

Where:
- V = membrane potential
- τ = membrane time constant
- V_reset = reset potential
- I = input current

### Spike Propagation

Spikes are propagated through synapses with delay:

```math
V_synapse(t) = w * spike(t - delay)
```

Where:
- w = synaptic weight
- delay = synaptic delay

### Plasticity Rules

#### Spike-Timing-Dependent Plasticity (STDP)

STDP modifies synaptic strength based on the timing between pre-synaptic and post-synaptic spikes:

```math
dW/dt = 
\begin{cases}
A_+ * exp(-(t - t_post)/τ_+) & \text{if } t_post > t_pre \\
-A_- * exp(-(t - t_pre)/τ_-) & \text{if } t_pre > t_post
\end{cases}
```

#### Hebbian Learning

Hebbian learning strengthens synapses based on co-activation:

```math
dW/dt = learningRate * preSpike * postSpike
```

## Memory Systems

### Working Memory

Working memory maintains transient information with competitive dynamics:

- **Capacity**: Limited capacity (configurable)
- **Maintenance**: Persistent activity through recurrent connections
- **Competition**: Mutual inhibition between memory traces
- **Update**: New information can overwrite old information

### Episodic Memory

Episodic memory stores experiences as episodes:

- **Encoding**: Current brain state stored as episode
- **Storage**: Episodes stored in temporal order
- **Replay**: Important episodes replayed during rest
- **Consolidation**: Weak episodes removed, strong episodes strengthened

### Associative Memory

Associative memory forms pattern associations:

- **Pattern Completion**: Partial patterns retrieve complete memories
- **Pattern Separation**: Similar patterns become distinct
- **Rapid Learning**: Fast learning of relationships

## Neuromodulation

### Dopamine System

Dopamine modulates neural excitability and plasticity:

- **Reward Prediction Error**: Signals prediction errors in reward
- **Plasticity Factor**: Scales learning rates
- **Excitability Modulation**: Changes neural responsiveness

### Curiosity System

Curiosity drives exploration behavior:

- **Novelty Detection**: Detects unexpected sensory input
- **Exploration Motivation**: Increases during novelty detection
- **Intrinsic Motivation**: Self-directed exploration

### Novelty Detection

Novelty detection identifies unexpected events:

- **Surprise**: Measures prediction error magnitude
- **Uncertainty**: Evaluates uncertainty in predictions
- **Attention**: Increases attention to novel events

### Prediction Error

Prediction error signals when predictions don't match reality:

- **Forward Model**: Generates predictions
- **Inverse Model**: Computes actions to achieve predictions
- **Error Signals**: Adjusts internal models based on errors

## Development and Plasticity

### Developmental Stages

Neural development follows distinct stages:

1. **Initial**: High plasticity, rapid learning
2. **CriticalPeriod**: Sensitive to environmental input
3. **Maturation**: Stabilization of connections
4. **Adult**: Stable, refined functionality

### Structural Plasticity

#### Synaptogenesis

Formation of new synapses:

```math
probability = baseRate * developmentalFactor * activityFactor
```

#### Pruning

Elimination of weak synapses:

```math
probability = pruningRate * activityDependency
```

### Developmental Dynamics

Development affects neural properties over time:

- **Plasticity Rate**: Changes with developmental stage
- **Connectivity**: Evolution of neural connections
- **Excitability**: Age-dependent changes in responsiveness
- **Maturation**: Progressive refinement of function

## Integration and Testing

### Phase 6 Integration Test

Phase 6 demonstrates integration of all brain systems:

```bash
# Run Phase 6 integration test
./nlm_phase6_demo

# Run with custom configuration
./nlm_phase6_demo --neuron_count=500 --max_steps=2000
```

### Integration Verification

The integration test verifies:

1. **Memory Systems**: Working memory, episodic memory
2. **Neuromodulation**: Dopamine, curiosity, novelty
3. **Prediction**: Prediction system integration
4. **Development**: Development system functionality
5. **Replay**: Memory replay mechanisms
6. **Checkpointing**: Save/load capabilities

### Testing Strategy

1. **Unit Tests**: Individual component testing
2. **Integration Tests**: System interaction testing
3. **Performance Tests**: Speed and scalability testing
4. **Regression Tests**: Change validation testing

## Performance Optimization

### Optimization Techniques

NLM employs several optimization strategies:

1. **Memory Pools**: Reuse memory allocations
2. **Event-Driven Simulation**: Process only active events
3. **SIMD Vectorization**: Parallel processing of data
4. **Multi-threading**: Parallel neural computation
5. **Caching**: Store frequently accessed data
6. **Lazy Evaluation**: Defer expensive computations

### Performance Monitoring Setup

Enable comprehensive performance monitoring:

```cpp
void setupPerformanceMonitoring() {
    // Start monitoring
    startMonitoring();
    
    // Configure sampling interval
    setPerformanceSamplingInterval(0.1); // 100ms
    
    // Add custom alert thresholds
    addPerformanceAlertListener([](const PerformanceAlert& alert) {
        if (alert.type == PerformanceAlert::AlertType::CPUUsageHigh) {
            // Handle high CPU usage
            std::cout << "High CPU usage alert: " << alert.value << "% > " << alert.threshold << "%" << std::endl;
        }
    });
    
    // Start performance profiling
    startProfiling("MainSimulationLoop");
}
```

### Performance Analysis

Analyze performance data for optimization:

```cpp
void analyzePerformance() {
    // Get current performance metrics
    PerformanceMetrics current = getCurrentMetrics();
    
    // Get performance samples
    std::vector<PerformanceSample> samples = getPerformanceSamples(1000); // Last 1000 samples
    
    // Analyze trends
    PerformanceTrend trend = analyzePerformanceTrend(samples);
    
    // Print performance report
    std::cout << "Performance Report:" << std::endl;
    std::cout << "  Current CPU Usage: " << current.cpuUsage << "%" << std::endl;
    std::cout << "  Current Memory Usage: " << current.memoryUsage << "%" << std::endl;
    std::cout << "  Current Firing Rate: " << current.firingRate << " Hz" << std::endl;
    std::cout << "  Computation Time: " << current.computationTime << "s" << std::endl;
    
    // Print trend analysis
    std::cout << "Trends:" << std::endl;
    std::cout << "  CPU Trend: " << (trend.cpuTrend > 0 ? "Increasing" : "Decreasing") 
              << " (" << trend.cpuTrend << ")" << std::endl;
    std::cout << "  Memory Trend: " << (trend.memoryTrend > 0 ? "Increasing" : "Decreasing") 
              << " (" << trend.memoryTrend << ")" << std::endl;
    std::cout << "  Computation Trend: " << (trend.computationTrend > 0 ? "Increasing" : "Decreasing") 
              << " (" << trend.computationTrend << ")" << std::endl;
    std::cout << "  Firing Rate Trend: " << (trend.firingTrend > 0 ? "Increasing" : "Decreasing") 
              << " (" << trend.firingTrend << ")" << std::endl;
}
```

## Migration Guide

### From Previous Versions

This document describes changes and migration path from previous NLM versions:

#### Configuration Changes

1. **JSON Support**: New JSON configuration file format
2. **Environment Variables**: Enhanced support for environment variables
3. **Validation**: Added configuration validation
4. **Presets**: New preset system

#### Code Changes

1. **Error Handling**: Comprehensive error handling system
2. **Performance Monitoring**: New performance monitoring features
3. **Configuration**: Enhanced configuration API
4. **Command Line**: Improved command-line interface

#### Migration Steps

1. **Update Configuration Files**: Convert to JSON format if using legacy format
2. **Update Environment Variables**: Use NLM_ prefix
3. **Update Command Line**: Use new options
4. **Test Performance**: Use performance monitoring
5. **Update Error Handling**: Use new error handling system

## Troubleshooting

### Common Issues and Solutions

#### Configuration Errors

**Issue**: Configuration file not found or invalid
**Solution**: Use `--help-config` for configuration help, ensure file exists and is readable

**Issue**: Invalid configuration values
**Solution**: Use `--list-config` for valid options, check configuration file

#### Performance Issues

**Issue**: High CPU usage
**Solution**: 
1. Increase sampling interval
2. Check for memory leaks
3. Optimize neural computation
4. Monitor system resources

**Issue**: High memory usage
**Solution**:
1. Increase system memory
2. Optimize memory usage in code
3. Check for memory leaks
4. Use memory-efficient data structures

#### Neural Computation Issues

**Issue**: No spikes generated
**Solution**:
1. Check neuron parameters
2. Verify input currents
3. Check connectivity
4. Verify plasticity rules

**Issue**: Unexpected spike patterns
**Solution**:
1. Check synaptic weights
2. Verify spike timing
3. Check neuromodulation effects
4. Verify prediction system

### Debugging Tips

1. **Enable Verbose Mode**: Use `--verbose` or `--debug` flag
2. **Enable Profiling**: Use performance monitoring
3. **Check Logs**: Review log files for errors
4. **Use Debugging Tools**: Use debugger for debugging
5. **Monitor Performance**: Use performance monitoring for bottlenecks

## API Reference

### Core Classes

#### Config
Configuration management with validation and environment variable support.

#### EnhancedConfig
Enhanced configuration with comprehensive features.

#### CommandLineInterface
Command-line interface with help and validation.

#### ErrorHandling
Comprehensive error handling with recovery mechanisms.

#### PerformanceMonitor
Performance monitoring and metrics collection.

#### PerformanceProfiler
Function profiling for optimization.

### Configuration API

```cpp
// Set configuration value
config->set("neuron_count", 1000);

// Get configuration value
int neuronCount = config->getOr<int>("neuron_count", 1000);

// Check if key exists
if (config->has("enable_development")) {
    bool devEnabled = config->getOr<bool>("enable_development", false);
}
```

### Command Line API

```cpp
// Parse command line arguments
bool success = cli->parseArguments(argc, argv);

// Get configuration
const auto& config = cli->getConfig();

// Show help
showHelp();

// Show version
showVersion();
```

### Error Handling API

```cpp
// Log error
logError("Operation failed", ErrorCategory::Neural, ErrorSeverity::Error);

// Log warning
logWarning("High memory usage detected");

// Log info
logInfo("System initialized successfully");

// Handle exception
addSystemError(std::runtime_error("Invalid configuration"), "During initialization");
```

### Performance Monitoring API

```cpp
// Start monitoring
startMonitoring();

// Record performance sample
PerformanceMetrics metrics;
// ... populate metrics ...
recordSample(metrics);

// Get current metrics
PerformanceMetrics current = getCurrentMetrics();

// Add alert listener
addPerformanceAlertListener([](const PerformanceAlert& alert) {
    // Handle alert
});

// Get alerts
std::vector<PerformanceAlert> alerts = getPerformanceAlerts();

// Start benchmark
PerformanceBenchmark bench = startBenchmark("Neural Computation");
// ... perform benchmark ...
endBenchmark(bench);
```

### Performance Profiling API

```cpp
// Start profiling
startProfiling("ExpensiveFunction");

// Stop profiling
stopProfiling();

// Get profile data
std::vector<ProfileData> profiles = getProfileData();

// Print profile report
printProfileReport();
```

## Best Practices

### Configuration Best Practices

1. **Use Presets**: Use presets for common configurations
2. **Validate Values**: Always validate configuration values
3. **Document Configuration**: Document configuration options
4. **Test Configuration**: Test configuration thoroughly
5. **Use Environment Variables**: Use environment variables for flexibility

### Performance Best Practices

1. **Profile Early**: Profile early in development
2. **Optimize Critical Paths**: Identify and optimize critical paths
3. **Use Efficient Data Structures**: Use memory-efficient data structures
4. **Monitor Performance**: Continuously monitor performance
5. **Use Caching**: Cache frequently accessed data

### Error Handling Best Practices

1. **Catch Specific Exceptions**: Catch specific exceptions, not general exceptions
2. **Provide Context**: Provide context for errors
3. **Recover Gracefully**: Recover gracefully from errors
4. **Log Errors**: Log errors for debugging
5. **Validate Inputs**: Validate inputs before processing

### Neural Computation Best Practices

1. **Use Realistic Parameters**: Use realistic neural parameters
2. **Validate Connections**: Validate neural connections
3. **Test Plasticity**: Test plasticity rules
4. **Monitor Performance**: Monitor neural performance
5. **Use Appropriate Data Types**: Use appropriate data types for neural simulation

## Contributing

### Contribution Guidelines

1. **Fork the Repository**: Create a fork of the NLM repository
2. **Create Feature Branch**: Create a feature branch for your changes
3. **Follow Code Style**: Follow the existing code style
4. **Add Tests**: Add tests for new functionality
5. **Update Documentation**: Update documentation
6. **Submit Pull Request**: Submit a pull request

### Code Style

- **C++20**: Use C++20 features
- **Google Style**: Follow Google C++ style guide
- **Kappa Rules**: Follow Kilo code style
- **Documentation**: Document all public APIs
- **Error Handling**: Use comprehensive error handling

### Testing

1. **Unit Tests**: Write unit tests for individual components
2. **Integration Tests**: Write integration tests for system interactions
3. **Performance Tests**: Write performance tests
4. **Regression Tests**: Write regression tests
5. **Continuous Integration**: Use continuous integration

### Documentation

1. **README**: Update README for new features
2. **Documentation**: Update documentation
3. **API Reference**: Update API reference
4. **Tutorials**: Add tutorials
5. **Examples**: Add examples

## License

NLM is licensed under the MIT License.

```
MIT License

Copyright (c) 2026 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

This documentation provides comprehensive information about the NLM project, including all new improvements and features. For more detailed information, please refer to the specific sections and API documentation.
