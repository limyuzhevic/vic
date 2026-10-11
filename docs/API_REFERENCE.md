# NLM — 熙然
# Neural Learning Machine API Documentation

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project designed to create neural systems that learn, remember, and adapt through interaction with environments. NLM implements real spiking neural computation with biological-inspired mechanisms.

## Table of Contents

1. [Introduction](#introduction)
2. [System Architecture](#system-architecture)
3. [Core Components](#core-components)
4. [Configuration Management](#configuration-management)
5. [Command-Line Interface](#command-line-interface)
6. [Testing and Validation](#testing-and-validation)
7. [Performance Monitoring](#performance-monitoring)
8. [API Reference](#api-reference)
9. [Best Practices](#best-practices)
10. [Migration Guide](#migration-guide)

## Introduction

### What is NLM?

NLM is a **brain simulator** for computers that enables the creation of virtual brains capable of:

- **Learning** from experience through synaptic plasticity
- **Memory** formation and retrieval
- **Decision-making** through neural computation
- **Adaptation** to changing environments

### Key Features

#### Neural Computation
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- Multiple plasticity rules (STDP, Hebbian, structural)
- Network-level emergent behavior

#### Memory Systems
- Working memory with persistent activity
- Episodic memory with experience encoding
- Associative memory with pattern completion
- All memory systems interconnected with neural processing

#### Neuromodulation
- Reward prediction error (dopamine)
- Curiosity-driven exploration
- Novelty detection
- Integrated with learning and plasticity

#### Cognition
- Neural planning with action sequence evaluation
- Concept formation from experience patterns
- Competitive attention mechanisms
- Self-model and social learning

#### Development
- Developmental stages affecting plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

## System Architecture

### Core Components

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   WORLD        │    │ SENSORY INPUT  │    │ NEURAL PROCESSING│
│ (Environment)   │───▶ (Sensory)     │───▶ (LIF Neurons)  │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                        │                        │
         ▼                        ▼                        ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│ INTERNAL STATE  │◀───│ MEMORY / PREDICTION │◀───│ MOTIVATION / NEUROMODULATION │
│ (Working Memory)│    │ (Episodic Working)│    │ (Dopamine, Curiosity)    │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                        │                        │
         ▼                        ▼                        ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│ ACTION SELECTION │───▶ MOTOR OUTPUT │───▶ WORLD CONSEQUENCE │
│ (Neural Planner)│    │ (Motor System)  │    │ (Environment)      │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                        │                        │
         ▼                        ▼                        ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   FEEDBACK      │◀───│ REWARD / ERROR  │◀───│ PLASTICITY RULES │
│ (Prediction Err)│    │ (Prediction Err) │    │ (STDP, Hebbian)   │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                        │                        │
         ▼                        ▼                        ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│  MEMORY UPDATE  │───▶ DEVELOPMENT     │───▶ CHANGED BRAIN │
│ (Consolidation) │    │ (Plasticity)     │    │ (New Connections)│
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### Phase-Based Development

NLM evolved through six phases:

#### Phase 1: Project Skeleton (Complete)
- Core types and configuration
- Neural interfaces
- Basic architecture

#### Phase 2: Real Neural Computation (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

#### Phase 3: World Interaction (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

#### Phase 4: Advanced Cognition (Complete)
- Neural prediction system
- Working memory
- Episodic memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

#### Phase 5: Performance Optimizations (Complete)
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

#### Phase 6: Final Integration (Complete)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional

## Core Components

### Brain System

The Brain class is the central component implementing neural computation:

```cpp
class Brain {
public:
    // Core computation
    void step(SimulationStep step, double timestep);
    bool initialize();
    void reset();
    
    // Neural properties
    size_t getTotalNeuronCount() const;
    size_t getFiringNeuronCount() const;
    double getAverageFiringRate() const;
    size_t getTotalSpikeCount() const;
    
    // Region management
    NeuralRegion* getRegion(RegionId id) const;
    std::vector<NeuralRegion*> getAllRegions() const;
    
    // Synaptic operations
    size_t getTotalSynapseCount() const;
    void injectCurrentToNeurons(NeuronType type, float current);
    void injectCurrent(NeuronId neuronId, float current);
    
    // Learning and plasticity
    void enablePlasticity(bool hebbian, bool stdp, bool structural);
    void logStatus() const;
    
    // State management
    std::string getDevelopmentalStage() const;
    double getExcitability() const;
};
```

### Memory Systems

#### Working Memory
- Persistent activity buffer
- Item storage and retrieval
- Competitive dynamics
- Capacity limits

#### Episodic Memory
- Experience encoding
- Temporal indexing
- Similarity matching
- Consolidation mechanisms

#### Associative Memory
- Pattern associations
- Hebbian learning
- Pattern completion
- Predictive coding

### Neuromodulation

#### Reward Prediction Error
- Dopamine signaling
- Learning signal generation
- Reward-based plasticity

#### Curiosity
- Novelty detection
- Exploration drive
- Intrinsic motivation

#### Novelty Detection
- Change detection
- Surprise signaling
- Adaptive exploration

### Cognition Systems

#### Neural Planner
- Action sequence evaluation
- Predictive modeling
- Decision making
- Confidence scoring

#### Concept Formation
- Pattern abstraction
- Feature extraction
- Generalization
- Stability maintenance

#### Attention
- Competitive selection
- Salience mapping
- Resource allocation
- Distractor filtering

## Configuration Management

### Configuration System

NLM uses a comprehensive configuration system supporting JSON, YAML, and simple key=value formats:

```cpp
// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
};

// Configuration schema
class ConfigSchema {
public:
    ConfigValidationResult validate(const Config& config) const;
    std::string generateSummary(const Config& config) const;
    std::string getFieldDescription(const std::string& name) const;
};
```

### Default Configuration

```ini
# Core simulation parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15

# STDP parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Memory system parameters
working_memory_capacity = 1000
episodic_memory_capacity = 5000

# Development parameters
development_enabled = true
critical_period_duration = 100.0

# Neuromodulation parameters
reward_sensitivity = 1.0
curiosity_sensitivity = 1.0

# Environment parameters
environment_width = 20
environment_height = 20
```

### Configuration Validation

The configuration system validates values against schema constraints:

- Required field checking
- Type validation (int, float, string, vector)
- Range validation (min/max values)
- Enum validation (allowed values)
- Size validation (vector lengths)

## Command-Line Interface

### Basic Usage

```bash
# Run basic simulation
./nlm --run basic

# Run with verbose logging
./nlm --run all --verbose

# Load configuration from file
./nlm --config my_config.json --run plasticity

# Custom parameters
./nlm --neuron-count 1000 --timestep 0.0005 --run stdp
```

### Advanced Options

```bash
# Full help
./nlm --help

# List available tests
./nlm --list-tests

# Run all tests
./nlm --run-all

# Validate configuration
./nlm --validate-config

# Performance profiling
./nlm --profile

# Benchmark mode
./nlm --benchmark quick

# Debug mode
./nlm --debug --verbose

# Output to file
./nlm --run all --output results.csv
```

### Command-Line Arguments

| Option | Description | Example |
|--------|-------------|---------|
| `--help` | Show help message | `./nlm --help` |
| `--version` | Show version | `./nlm --version` |
| `--config <file>` | Load configuration | `./nlm --config config.json` |
| `--run <test>` | Run specific test | `./nlm --run basic` |
| `--run-all` | Run all tests | `./nlm --run-all` |
| `--verbose` | Enable verbose logging | `./nlm --verbose` |
| `--debug` | Enable debug logging | `./nlm --debug` |
| `--validate-config` | Validate configuration | `./nlm --validate-config` |
| `--profile` | Enable performance profiling | `./nlm --profile` |
| `--benchmark <mode>` | Run in benchmark mode | `./nlm --benchmark quick` |
| `--output <file>` | Write results to file | `./nlm --output results.csv` |

## Testing and Validation

### Test Framework

NLM provides a comprehensive test framework for validating functionality:

#### Basic Connectivity Test
- Injects current into neurons
- Verifies spike propagation
- Checks network connectivity

#### Plasticity Experiment
- Records initial synaptic state
- Applies learning stimuli
- Measures synaptic changes
- Validates learning detection

#### STDP Verification
- Tests pre-before-post timing
- Measures synaptic potentiation
- Validates LTD/LTP balance
- Checks plasticity accuracy

### Test Execution

```bash
# Run specific test
./nlm --run basic

# Run all tests
./nlm --run-all

# Run test with custom parameters
./nlm --run plasticity --neuron-count 750

# List available tests
./nlm --list-tests
```

### Test Results

Each test produces detailed output including:

- **Basic Connectivity Test**: Spike counts, membrane potentials, firing rates
- **Plasticity Experiment**: Weight changes, learning curves, stability metrics
- **STDP Verification**: Synaptic deltas, LTP/LTD balance, plasticity efficacy

## Performance Monitoring

### Performance Profiler

NLM includes advanced performance monitoring capabilities:

```cpp
// Performance metrics
struct PerformanceMetrics {
    double totalTime;           // Total execution time
    double cpuTime;             // CPU time
    double wallTime;            // Wall clock time
    size_t peakMemoryUsage;     // Peak memory usage
    size_t stepsExecuted;       // Number of simulation steps
    double stepsPerSecond;      // Throughput
    double stabilityScore;      // Network stability
    double efficiencyScore;     // Computational efficiency
};
```

### Benchmarking

```bash
# Run benchmark suite
./nlm --benchmark quick
./nlm --benchmark thorough
./nlm --benchmark stress

# Save benchmark results
./nlm --benchmark quick --output benchmark_results.json
```

### Visualization

Visualization data is automatically collected and can be exported:

```cpp
// Visualization data collector
class VisualizationDataCollector {
public:
    void recordStep(const std::vector<double>& neuronActivities,
                   const std::vector<double>& spikeTimes,
                   const std::vector<std::vector<double>>& synapticWeights,
                   const std::vector<double>& membranePotentials,
                   const std::vector<double>& learningSignals);
    
    void saveData(const std::string& filename = "visualization_data.json");
    void saveCSV(const std::string& filename = "visualization_data.csv");
    void savePythonPlot(const std::string& filename = "plot_data.py");
};
```

## API Reference

### Core Classes

#### nlm::Brain
- **Brain(std::shared_ptr<Config> config)**: Constructor
- **initialize()**: Initialize brain with configuration
- **reset()**: Reset brain to initial state
- **step(SimulationStep step, double timestep)**: Execute simulation step
- **getTotalNeuronCount()**: Get total neuron count
- **getFiringNeuronCount()**: Get firing neuron count
- **getAverageFiringRate()**: Get average firing rate
- **getTotalSpikeCount()**: Get total spike count
- **injectCurrentToNeurons(NeuronType type, float current)**: Inject current
- **injectCurrent(NeuronId neuronId, float current)**: Inject current to specific neuron

#### nlm::Config
- **Config()**: Constructor
- **loadFromFile(const std::string& filepath)**: Load from file
- **loadFromArgs(int argc, char** argv)**: Load from command line
- **set(const std::string& key, const ConfigValue& value, ConfigSource source)**: Set value
- **get(const std::string& key)**: Get value
- **getOr(const std::string& key, const T& defaultValue)**: Get with default
- **has(const std::string& key)**: Check if key exists
- **validate()**: Validate against schema

#### nlm::Logger
- **Logger()**: Constructor
- **addLogger(std::shared_ptr<ILogger> logger)**: Add logger
- **setLevel(LogLevel level)**: Set log level
- **info(const std::string& message)**: Log info
- **debug(const std::string& message)**: Log debug
- **warning(const std::string& message)**: Log warning
- **error(const std::string& message)**: Log error

### Utility Classes

#### nlm::HighResTimer
- **start()**: Start timer
- **stop()**: Stop timer
- **elapsedSeconds()**: Get elapsed seconds
- **elapsedMilliseconds()**: Get elapsed milliseconds

#### nlm::MemoryProfiler
- **trackAllocation(size_t size)**: Track memory allocation
- **trackDeallocation(size_t size)**: Track memory deallocation
- **getPeakMemoryUsage()**: Get peak memory usage
- **getCurrentMemoryUsage()**: Get current memory usage

#### nlm::SimulationProfiler
- **startStepProfiling()**: Start step profiling
- **recordStepMetrics()**: Record step metrics
- **getPerformanceMetrics()**: Get performance metrics

## Best Practices

### Development Guidelines

1. **Use Constants**: Always use named constants instead of magic numbers
2. **Error Handling**: Implement comprehensive error checking
3. **Logging**: Use the logger for debugging and monitoring
4. **Configuration**: Use configuration files for parameter management
5. **Testing**: Write unit tests for all new functionality
6. **Documentation**: Document public APIs and provide examples

### Configuration Best Practices

1. **Schema Validation**: Always validate configuration against schema
2. **Default Values**: Provide sensible defaults for all parameters
3. **Override Support**: Allow command-line overriding of configuration
4. **Documentation**: Document all configuration options
5. **Type Safety**: Use appropriate types for configuration values

### Performance Best Practices

1. **Profiling**: Use performance profiler for optimization
2. **Memory Management**: Track memory allocation and deallocation
3. **Caching**: Cache frequently used data
4. **Parallelization**: Use parallel processing for intensive computations
5. **Monitoring**: Monitor performance during runtime

## Migration Guide

### From Phase 5 to Phase 6

#### Key Changes

1. **Configuration System**: Enhanced JSON/YAML support
2. **Command-Line Interface**: Advanced argument parsing
3. **Error Handling**: Comprehensive error handling and recovery
4. **Performance Monitoring**: Advanced profiling and benchmarking
5. **Memory Management**: Improved memory tracking and management
6. **Documentation**: Comprehensive API documentation

#### Migration Steps

1. **Update Configuration**: Migrate from simple key=value to JSON/YAML format
2. **Update Command Line**: Use new command-line arguments
3. **Add Error Handling**: Implement try-catch blocks for error recovery
4. **Add Profiling**: Integrate performance profiling
5. **Update Documentation**: Document new APIs and features
6. **Test Migration**: Verify all functionality works with new system

### Backward Compatibility

The improved NLM framework maintains backward compatibility:

- **Configuration**: Supports old simple key=value format alongside JSON/YAML
- **APIs**: Existing APIs unchanged, new APIs added
- **Testing**: All existing tests continue to work
- **Command Line**: New arguments added, old arguments still supported

## Conclusion

NLM (熙然) is a comprehensive neural learning machine framework implementing real spiking neural computation with biological-inspired mechanisms. The framework has evolved through six phases, culminating in Phase 6: FINAL INTEGRATION, where all systems work together as a coherent artificial brain.

The improvements outlined in this document enhance:

1. **Code Quality**: Refactored main.cpp, improved organization, added constants
2. **Configuration System**: JSON/YAML parser, schema validation
3. **Command-Line Interface**: Advanced argument parsing, verbose/debug modes
4. **Performance Monitoring**: Profiling, benchmarking, visualization
5. **Error Handling**: Comprehensive error handling and recovery
6. **Code Organization**: Proper header files, consistent patterns
7. **API Documentation**: Complete API reference and examples
8. **Testing**: Enhanced test framework and validation

These improvements make NLM more maintainable, extensible, and easier to use while preserving backward compatibility with existing code.