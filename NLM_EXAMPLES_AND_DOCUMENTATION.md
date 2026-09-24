# NLM Examples and Documentation

This document provides examples and documentation for the improved NLM (熙然) system, featuring enhanced configuration management, command-line interface, and testing capabilities.

## Table of Contents

1. [Introduction](#introduction)
2. [Configuration System](#configuration-system)
3. [Command-Line Interface](#command-line-interface)
4. [Phase 6 Integration Demo](#phase-6-integration-demo)
5. [Testing and Validation](#testing-and-validation)
6. [Best Practices](#best-practices)
7. [Troubleshooting](#troubleshooting)

## Introduction

NLM (熙然) is an experimental artificial developmental brain simulation that has evolved through multiple phases. This version represents the culmination of Phase 6: Final Integration, featuring a complete, coherent brain system with all subsystems properly integrated.

### Key Improvements

- **Enhanced Configuration System**: Full JSON/YAML support, validation, proper save/load
- **Comprehensive Error Handling**: Robust exception handling with detailed error reporting
- **Improved Command-Line Interface**: Phase selection, argument validation, help system
- **Advanced Testing Framework**: Unit tests for all components including config validation
- **Better Documentation**: Complete phase documentation and usage examples

## Configuration System

The NLM configuration system has been significantly enhanced from the original Phase 2 implementation.

### Configuration File Format

#### Key=Value Format (Original)
```
# Simple configuration file
neuron_count = 1000
connection_probability = 0.05
random_seed = 42
enable_multithreading = true
```

#### JSON Format (Enhanced)
```json
{
    "brain": {
        "neuron_count": 1000,
        "connection_probability": 0.05,
        "random_seed": 42
    },
    "plasticity": {
        "stdp_enable": true,
        "hebbian_enable": true,
        "structural_enable": true
    },
    "neuromodulation": {
        "dopamine_scale": 1.0,
        "curiosity_enable": true,
        "novelty_enable": true
    }
}
```

### Configuration Management Features

#### Setting Values
```cpp
// Using the enhanced Config class
auto config = std::make_shared<Config>();

// Set various types of values
config->set("int_param", 42);
config->set("double_param", 3.14159);
config->set("string_param", "Hello World");
config->set("bool_param", true);
config->set("vector_param", std::vector<int>({1, 2, 3, 4, 5}));

// With source tracking
config->set("param", value, ConfigSource::Runtime);
```

#### Getting Values
```cpp
// Get values with type safety
auto intValue = config->get<int>("int_param");
if (intValue.has_value()) {
    int value = intValue.value();
}

// Get with default value
int value = config->getOr<int>("int_param", 100);

// Check existence
if (config->has("required_param")) {
    // Use the parameter
}
```

#### Validation
```cpp
// Add validation for specific keys
config->addValidator("positive_value", [](const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value) > 0;
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value) > 0.0;
    }
    return false;
});

// Set a value (validation happens automatically)
config->set("positive_value", 42); // Success
config->set("positive_value", -5); // Validation fails, value not set
```

#### Save and Load
```cpp
// Save configuration to file
bool saveSuccess = config->saveToFile("myconfig.json");
if (saveSuccess) {
    std::cout << "Configuration saved successfully!" << std::endl;
}

// Load configuration from file
bool loadSuccess = config->loadFromFile("myconfig.json");
if (loadSuccess) {
    std::cout << "Configuration loaded successfully!" << std::endl;
}
```

## Command-Line Interface

The command-line interface has been significantly enhanced to support various operations and configurations.

### Basic Usage

```bash
# Run Phase 6 integration demo (default)
nlm

# Show help information
nlm --help
nlm -h

# Show version information
nlm --version
nlm -v

# Run quick test only
nlm --test

# Enable verbose logging
nlm --verbose
nlm -V

# Suppress informational output
nlm --quiet
nlm -q
```

### Advanced Options

```bash
# Load configuration from file
nlm --config myconfig.json
nlm -c myconfig.json

# Save results to file
nlm --output results.txt
nlm -o results.txt

# Measure execution time
nlm --time

# Custom simulation steps
nlm --steps 5000

# Custom number of neurons
nlm --neurons 2000

# Combined options
nlm --config myconfig.json --output results.txt --verbose --time
```

### Command Line Parsing Examples

```cpp
// In your main.cpp
parseCommandLineArgs(argc, argv, quiet, verbose, 
                   configFile, outputFile, 
                   measureTime, steps, neurons, 
                   runDemo, runQuickTest);

// Use the parsed values
if (runDemo) {
    runPhase6IntegrationDemo(steps, neurons);
} else if (runQuickTest) {
    runQuickIntegrationTest();
}
```

## Phase 6 Integration Demo

The Phase 6 integration demo demonstrates the complete NLM brain system with all subsystems properly connected.

### Demo Overview

The demo performs the following steps:

1. **Integration Verification**: Quick check that all systems are properly connected
2. **Memory Integration Test**: Verifies memory systems work with neural processing
3. **Neuromodulation Integration Test**: Tests reward and curiosity systems
4. **Checkpoint Test**: Validates save/load functionality
5. **Replay Test**: Verifies memory replay and consolidation
6. **Full Integration Experiment**: Complete lifetime simulation

### Running the Demo

```bash
# Basic demo
nlm

# Demo with custom settings
nlm --steps 3000 --neurons 750 --verbose

# Quick test only (faster)
nlm --test --quiet

# Demo with output file
nlm --output demo_results.txt
```

### Expected Output

```
==========================================
NLM — 熙然
Neural Learning Machine
==========================================

Phase 6: Final Integration Demo
Demonstrating complete brain system integration.

=== Integration Verification ===
Integration verification passed!

--- Memory Integration Test ---
Memory integration test completed.

--- Neuromodulation Integration Test ---
Neuromodulation integration test completed.

--- Checkpoint Test ---
Checkpoint test completed.

--- Replay Test ---
Replay test completed.

=== FULL INTEGRATION EXPERIMENT ===
Total reward: 127.45
Average firing rate: 8.23
Memory episodes stored: 45
Dopamine level: 0.87

=== PHASE 6 INTEGRATION STATUS ===
Memory Systems: CONNECTED ✓
Episodic Memory: CONNECTED ✓
Neuromodulation: CONNECTED ✓
Prediction: CONNECTED ✓
Development: CONNECTED ✓
Checkpointing: WORKING ✓
Replay: FUNCTIONAL ✓

=== PHASE 6 SUMMARY ===
Phase 6 demonstrates:
  ✓ Memory systems integrated with neural processing
  ✓ Neuromodulation affects plasticity and dynamics
  ✓ Prediction system as central organizing principle
  ✓ Development affects plasticity rates
  ✓ Checkpoint save/load working
  ✓ Replay and consolidation functional

The NLM brain is now a complete artificial neural substrate
capable of emerging cognition through integrated systems.

SUCCESS: All brain systems are properly integrated and functional!
```

## Testing and Validation

The NLM system includes comprehensive testing capabilities to ensure reliability and correctness.

### Test Framework

The testing framework includes:

#### Core System Tests
- **Types Tests**: Type system and data structure tests
- **Config Tests**: Configuration system validation
- **Random Tests**: Random number generator tests
- **Clock Tests**: Time management tests

#### Neural Network Tests
- **Neuron Tests**: LIF neuron dynamics and spike generation
- **Synapse Tests**: Synaptic transmission and plasticity
- **STDP Tests**: Spike-timing dependent plasticity
- **Brain Tests**: Complete brain integration and functionality

#### Advanced Config Tests
- **Basic Config Operations**: Setting and getting values
- **Validation Tests**: Validator functionality
- **Save/Load Tests**: File I/O operations
- **Key=Value Parsing**: Simple configuration format
- **Command Line Parsing**: Argument handling

### Running Tests

```bash
# Run all tests
./nlm_test

# Run specific test category
# (Note: Individual test category execution may not be directly supported)
```

### Test Results

When all tests pass, you should see:

```
=== NLM Phase 6 Tests ===
Testing Enhanced Neural Learning Machine:
  - LIF Neuron Dynamics
  - Synaptic Transmission
  - STDP Plasticity
  - Enhanced Configuration Management

Running Types tests... PASSED
Running Config tests... PASSED
Running Advanced Config tests... PASSED
Running Random tests... PASSED
Running Clock tests... PASSED
Running Neuron tests... PASSED
Running Synapse tests... PASSED
Running STDP tests... PASSED
Running Brain tests... PASSED

=== All Tests PASSED ===
```

## Best Practices

### Configuration Management

1. **Always validate input values**: Use validators to ensure configuration values meet requirements.

2. **Use descriptive parameter names**: Choose meaningful keys that describe their purpose.

3. **Document configuration options**: Provide clear descriptions for each configuration parameter.

4. **Use appropriate data types**: Choose the most specific type for each parameter.

5. **Validate before using**: Check if required parameters are present before processing.

### Error Handling

1. **Catch exceptions at appropriate levels**: Handle specific exceptions rather than using broad catch blocks.

2. **Provide informative error messages**: Include context and helpful information in error messages.

3. **Fail gracefully**: Clean up resources and exit with appropriate error codes.

4. **Log errors for debugging**: Use the logging system to record errors for troubleshooting.

### Command-Line Usage

1. **Always provide help**: Implement a `--help` option to guide users.

2. **Validate arguments**: Check that required arguments are present and valid.

3. **Use sensible defaults**: Provide reasonable defaults for most options.

4. **Support both short and long options**: Offer `-h` and `--help` for flexibility.

5. **Validate parameter ranges**: Check that numeric parameters are within valid ranges.

### Testing Practices

1. **Test all public interfaces**: Ensure all public methods are covered by tests.

2. **Test boundary conditions**: Test minimum, maximum, and edge cases.

3. **Test error conditions**: Verify that error handling works correctly.

4. **Test with invalid input**: Ensure the system handles malformed input gracefully.

5. **Test with valid input**: Ensure the system works correctly with valid input.

## Troubleshooting

### Common Issues and Solutions

#### Issue: Configuration File Not Found

**Symptom**: `ERROR: Failed to load configuration from file`

**Solution**: 
- Check that the file path is correct
- Verify file permissions
- Ensure the file is not corrupted
- Try using absolute paths

#### Issue: Invalid Configuration Values

**Symptom**: Configuration validation fails or unexpected behavior

**Solution**:
- Check that configuration values are of the correct type
- Verify that values are within expected ranges
- Use descriptive parameter names
- Check configuration file format

#### Issue: Missing Dependencies

**Symptom**: Compilation errors related to missing headers or libraries

**Solution**:
- Ensure all required dependencies are installed
- Check that CMake can find the dependencies
- Install missing system libraries
- Verify build tools are properly configured

#### Issue: Memory or Performance Issues

**Symptom**: Slow performance, excessive memory usage, or crashes

**Solution**:
- Reduce configuration complexity
- Use more efficient algorithms
- Optimize configuration loading
- Check for memory leaks in the configuration system

#### Issue: Command-Line Parsing Problems

**Symptom**: Command-line arguments not recognized or misinterpreted

**Solution**:
- Check argument order and syntax
- Verify that both short and long options work
- Ensure required arguments are provided
- Check for typos in argument names

### Debugging Techniques

#### Using Logging

Enable verbose logging to debug issues:

```bash
nlm --verbose --quiet  # Enable verbose but suppress info messages
```

#### Checking Error Messages

The NLM system provides detailed error messages. Look for:

- File paths that cannot be opened
- Invalid parameter values
- Missing required dependencies
- Memory allocation errors

#### Running Tests

Run tests to identify configuration or functionality issues:

```bash
./nlm_test
```

#### Manual Configuration

If automated configuration fails, create a minimal configuration manually:

```json
{
    "brain": {
        "neuron_count": 100,
        "connection_probability": 0.1
    }
}
```

## Conclusion

The NLM Phase 6 system represents a significant advancement in artificial brain simulation, featuring:

1. **Complete System Integration**: All subsystems work together seamlessly
2. **Enhanced Configuration Management**: Robust configuration system with validation
3. **Comprehensive Error Handling**: Detailed error reporting and recovery
4. **Improved User Experience**: Intuitive command-line interface
5. **Thorough Testing**: Extensive test coverage for all components
6. **Comprehensive Documentation**: Complete documentation and examples

The system demonstrates how complex neural architectures can emerge from simple building blocks through experience and interaction with the environment. The enhanced configuration, testing, and user interface features make it easier than ever to experiment with and understand the NLM brain system.

For further information and research papers, visit the NLM documentation and research repositories.