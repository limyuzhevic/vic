# NLM Phase 2 Enhanced: API Documentation

## Overview

This document provides comprehensive documentation for the enhanced NLM Phase 2 implementation, including improved error handling, enhanced AgentBrain APIs, type-safe configuration system, and advanced features for expert users.

## Table of Contents

1. [Enhanced Error Handling](#enhanced-error-handling)
2. [AgentBrain v2.0 API](#agentbrain-v20-api)
3. [Type-Safe Configuration System](#type-safe-configuration-system)
4. [Testing Framework](#testing-framework)
5. [Advanced Features for Expert Users](#advanced-features-for-expert-users)
6. [Performance Considerations](#performance-considerations)
7. [Migration Guide](#migration-guide)

## Enhanced Error Handling

### NLM Exception Hierarchy

The enhanced error handling system provides a comprehensive exception hierarchy:

#### Base Classes

- **NLMException**: Base exception for all NLM errors
  - Properties: `ErrorCode`, `message`, `context`
  - Error codes: `MemoryAllocationFailed`, `ConfigValidationFailed`, `NeuronNotFound`, etc.

- **NLMConfigException**: Configuration-specific errors
  - Additional property: `context` (key name)

- **NLMNeuralException**: Neural system errors
  - Additional property: `entity` (neuron, synapse, region name)

- **NLMSimulationException**: Simulation-specific errors
  - Additional property: `step` (simulation step number)

#### Error Code Reference

| Error Code | Description | When to Use |
|------------|-------------|-------------|
| `MemoryAllocationFailed` | Memory allocation failed | When memory allocation fails |
| `ConfigValidationFailed` | Configuration validation failed | When configuration is invalid |
| `NeuronNotFound` | Neuron not found | When neuron doesn't exist |
| `SynapseNotFound` | Synapse not found | When synapse doesn't exist |
| `SimulationNotRunning` | Simulation not running | When trying to step while not running |
| `InvalidParameter` | Invalid parameter value | When parameter value is out of range |
| `Timeout` | Operation timed out | When operation exceeds time limit |

#### Error Handling Examples

```cpp
#include "include/nlm/NLMErrors.hpp"

// Basic error handling
try {
    config.setInt("neuron_count", -5);
} catch (const nlm::NLMConfigException& e) {
    std::cerr << "Configuration error: " << e.what() << std::endl;
    std::cerr << "Key: " << e.getContext() << std::endl;
}

// Neural system error handling
try {
    neuron = brain->getNeuron(42);
} catch (const nlm::NLMNeuralException& e) {
    std::cerr << "Neural system error: " << e.what() << std::endl;
    std::cerr << "Entity: " << e.getContext() << std::endl;
}

// Simulation error handling
try {
    brain->step(step, 0.001);
} catch (const nlm::NLMSimulationException& e) {
    std::cerr << "Simulation error: " << e.what() << std::endl;
    if (e.getCode() == nlm::NLMException::ErrorCode::SimulationNotRunning) {
        // Handle restart
        brain->initialize();
    }
}
```

### Error Recovery Strategies

1. **Recoverable Errors**: Errors that don't prevent system operation
   - Examples: `SimulationNotRunning`, `InvalidParameter`
   - Strategy: Retry operation or use default values

2. **Non-Recoverable Errors**: Errors that prevent system operation
   - Examples: `MemoryAllocationFailed`, `FileSystemError`
   - Strategy: Shutdown or request system intervention

3. **Validation Errors**: Configuration or parameter validation errors
   - Examples: `ConfigValidationFailed`, `InvalidParameter`
   - Strategy: Rollback changes and prompt for valid input

## AgentBrain v2.0 API

### AgentBrain Configuration

```cpp
struct AgentBrainConfig {
    // Neuromodulation settings
    bool enableRewardModulation = true;
    bool enableStructuralPlasticity = true;
    bool enableDevelopment = true;
    bool enableCuriosity = true;
    bool enableNoveltyDetection = true;
    
    // Curiosity parameters
    float curiosityThreshold = 0.3f;
    float explorationProbability = 0.2f;
    
    // Novelty parameters
    float noveltyDecayRate = 0.99f;
    
    // Development parameters
    double developmentalDuration = 900.0;
    
    // Performance optimizations
    bool enableCaching = true;
    bool enableBatchProcessing = false;
    
    // Debugging and monitoring
    bool enableDebugLogging = false;
    bool enablePerformanceMetrics = false;
    
    // Advanced features for expert users
    bool enableCustomPlasticityRules = false;
    bool enableAdvancedPrediction = false;
    
    // Custom callbacks
    std::function<void(const std::vector<float>&)> onSensoryProcessed;
    std::function<void(MotorCommand)> onActionSelected;
    std::function<void(float, float)> onRewardReceived;
};
```

### Constructor and Initialization

```cpp
// Basic constructor
AgentBrain agentBrain(brain);  // Default configuration

// Enhanced constructor with configuration
AgentBrain agentBrain(brain, config);

// Fluent API for configuration
AgentBrain agentBrain = AgentBrainBuilder::create(brain)
    .withConfig(config)
    .enableAllFeatures()
    .optimizeForPerformance()
    .build();

// Initialize with world
agentBrain.initialize(world);
```

### Core Processing API

```cpp
// Process sensory input and update internal state
void process(const SensoryPercept& percept, double timestep = 0.001);

// Get current action (includes curiosity-based exploration)
MotorCommand getAction() const;

// Apply reward modulation
void applyReward(float reward, float predictedReward = 0.0f);

// Update development system
void updateDevelopment(double timestep);
```

### State Access API

```cpp
// Neuromodulation state
float getNeuromodulationLevel() const;    // Dopamine level
float getCuriosityLevel() const;          // Curiosity drive
float getNoveltyLevel() const;             // Novelty detection
float getPredictionError() const;         // Prediction error

// Developmental state
DevelopmentalStage getDevelopmentalStage() const;

// Performance metrics
struct PerformanceMetrics {
    size_t sensoryProcessingTime;
    size_t actionSelectionTime;
    size_t neuromodulationTime;
    float memoryUsage;
    size_t neuronGroupsCached;
};
const PerformanceMetrics& getPerformanceMetrics() const;
```

### Expert Features API

```cpp
// Custom plasticity rules for expert users
void setCustomPlasticityRule(std::function<float(SynapticWeight)> rule) {
    config_.enableCustomPlasticityRules = true;
    customPlasticityRule_ = rule;
}

// Batch processing for high-performance scenarios
void enableBatchProcessing(bool enable) {
    config_.enableBatchProcessing = enable;
    if (enable) {
        batchBuffer_.reserve(1000);
    }
}

void startBatchProcessing();
void addToBatch(const SensoryPercept& percept);
std::vector<MotorCommand> executeBatch(double timestep = 0.001);
void endBatchProcessing();

// Callbacks for expert users
void setSensoryCallback(std::function<void(const std::vector<float>&)> callback);
void setActionCallback(std::function<void(MotorCommand)> callback);
void setRewardCallback(std::function<void(float, float)> callback);
```

### Debug and Monitoring

```cpp
// Get detailed debug information
std::string getDebugInfo() const;

// Enable debugging
void enableDebugLogging(bool enable);
void enablePerformanceMetrics(bool enable);

// Memory management
void clearCaches();
size_t getCacheSize() const;
```

### Configuration Examples

#### Basic Configuration
```cpp
AgentBrainConfig config;
config.enableRewardModulation = true;
config.enableCuriosity = true;
config.curiosityThreshold = 0.5f;

AgentBrain agentBrain(brain, config);
```

#### Expert Configuration
```cpp
AgentBrainConfig expertConfig;
expertConfig.enableAllFeatures();  // Enable all advanced features
expertConfig.optimizeForPerformance();  // Optimize for speed
expertConfig.enableCustomPlasticityRules = true;
expertConfig.enableAdvancedPrediction = true;
expertConfig.enableDebugLogging = true;

AgentBrain agentBrain = AgentBrainBuilder::create(brain)
    .withConfig(expertConfig)
    .build();
```

## Type-Safe Configuration System

### Configuration Schema

The enhanced configuration system provides a comprehensive schema with validation:

#### Core Configuration Keys

| Key | Type | Default | Description | Unit |
|-----|------|---------|-------------|------|
| `neuron_count` | int | 500 | Number of neurons | neurons |
| `region_count` | int | 1 | Number of regions | regions |
| `connection_probability` | float | 0.15 | Connection probability | - |
| `simulation_timestep` | double | 0.001 | Simulation timestep | ms |
| `random_seed` | int64_t | 42 | Random seed | seed |

#### Neuron Dynamics Configuration

| Key | Type | Default | Description | Unit |
|-----|------|---------|-------------|------|
| `lif_resting_potential` | float | -70.0 | Resting potential | mV |
| `lif_threshold` | float | -55.0 | Firing threshold | mV |
| `lif_refractory_period` | int | 5 | Refractory period | steps |

#### Configuration Loading

```cpp
#include "core/Config/Config.hpp"

// Load from configuration file (JSON or INI format)
bool success = config.loadFromFile("config.json");

// Load from command line arguments
config.loadFromArgs(argc, argv);

// Load from environment variables
config.loadFromEnv("NLM_");

// Type-safe access with validation
int neuronCount = config.getWithValidation<int>("neuron_count", 500);
double timestep = config.getOr<double>("simulation_timestep", 0.001);
bool enableDebug = config.getOr<bool>("enable_debug", false);
```

### JSON Configuration Example

```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.2,
    "simulation_timestep": 0.001,
    "random_seed": 12345,
    
    "neuron_dynamics": {
        "lif_resting_potential": -70.0,
        "lif_threshold": -55.0,
        "lif_refractory_period": 10
    },
    
    "plasticity": {
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
    },
    
    "advanced": {
        "enable_custom_plasticity": true,
        "enable_advanced_prediction": true,
        "enable_debug_logging": true
    }
}
```

### Environment Variable Configuration

Environment variables use the prefix `NLM_`:

```bash
export NLM_NEURON_COUNT=1000
export NLM_CONNECTION_PROBABILITY=0.2
export NLM_SIMULATION_TIMESTEP=0.001
export NLM_ENABLE_DEBUG_LOGGING=true
```

### Configuration Validation

The configuration system validates all values against schema constraints:

```cpp
// This will throw NLMConfigException if validation fails
config.setInt("neuron_count", -5);  // Invalid: negative

// Safe validation with default fallback
int safeValue = config.getWithValidation<int>("neuron_count", 500);
```

## Testing Framework

### Enhanced Test Coverage

The enhanced testing framework includes:

1. **Original Tests**: Backward compatibility maintained
2. **Enhanced Neuron Tests**: Comprehensive error handling tests
3. **Enhanced Configuration Tests**: Type safety and validation tests
4. **Error Handling Tests**: Exception and recovery tests
5. **Performance Tests**: Benchmarking and optimization tests

### Test Structure

```cpp
// Original tests (backward compatibility)
namespace test_neuron {
    void testNeuronCreation();
    void testMembranePotential();
    // ... original tests
}

// Enhanced tests (new functionality)
namespace test_neuron_enhanced {
    void testNeuronErrorHandling();
    void testNeuronErrorHandlingConstructor();
    void testNeuronErrorHandlingMembranePotential();
    // ... enhanced error handling tests
}

// Configuration tests
namespace test_config {
    void testConfigCreation();
    // ... original config tests
}

namespace test_config_enhanced {
    void testConfigurationTypeSafety();
    void testConfigurationValidation();
    void testConfigurationJSON();
    // ... enhanced config tests
}
```

### Test Execution

```bash
# Build and run all tests
mkdir build && cd build
cmake ..
make -j4
./test_nlm  # Runs all original and enhanced tests

# Run specific test suites
./test_nlm --neuron  # Only neuron tests
./test_nlm --config  # Only configuration tests
./test_nlm --enhanced  # Only enhanced tests
```

### Test Results Summary

```
=== NLM Phase 2 Enhanced Tests ===
Testing Real Neural Computation with Error Handling:
  - LIF Neuron Dynamics (Enhanced)
  - Synaptic Transmission
  - STDP Plasticity
  - Enhanced Configuration System
  - Comprehensive Error Handling

Running Types tests...
  PASSED
Running Original Config tests...
  PASSED
Running Enhanced Config tests...
  PASSED
Running Random tests...
  PASSED
Running Clock tests...
  PASSED
Running Original Neuron tests...
  PASSED
Running Enhanced Neuron tests...
  PASSED

=== ALL TESTS PASSED ===
Phase 2 Enhanced: Real Neural Computation with Error Handling
Features: Enhanced APIs, Type-Safe Configuration, Comprehensive Error Handling
```

## Advanced Features for Expert Users

### 1. Custom Plasticity Rules

```cpp
// Define custom plasticity rule
auto customPlasticityRule = [](float dopamine) -> float {
    // Custom rule: stronger plasticity for moderate dopamine
    if (dopamine < 0.0f) return 0.5f + dopamine * 0.5f;  // Less plastic for negative
    return 1.0f + dopamine * 0.3f;  // More plastic for positive
};

agentBrain.setCustomPlasticityRule(customPlasticityRule);
```

### 2. Batch Processing

```cpp
// Enable batch processing for high-performance scenarios
agentBrain.enableBatchProcessing(true);

// Collect multiple sensory inputs
agentBrain.startBatchProcessing();
agentBrain.addToBatch(percept1);
agentBrain.addToBatch(percept2);
agentBrain.addToBatch(percept3);

// Process all at once
std::vector<MotorCommand> actions = agentBrain.executeBatch();
agentBrain.endBatchProcessing();
```

### 3. Custom Callbacks

```cpp
// Register custom callbacks for monitoring and debugging
agentBrain.setSensoryCallback([](const std::vector<float>& sensoryData) {
    std::cout << "Processed sensory data of size: " << sensoryData.size() << std::endl;
});

agentBrain.setActionCallback([](MotorCommand action) {
    std::cout << "Selected action: " << static_cast<int>(action) << std::endl;
});

agentBrain.setRewardCallback([](float reward, float predictedReward) {
    float error = reward - predictedReward;
    std::cout << "Reward prediction error: " << error << std::endl;
});
```

### 4. Performance Monitoring

```cpp
// Enable performance metrics
agentBrain.enablePerformanceMetrics(true);

// Access performance data
const auto& metrics = agentBrain.getPerformanceMetrics();
std::cout << "Sensory processing time: " << metrics.sensoryProcessingTime << " μs" << std::endl;
std::cout << "Action selection time: " << metrics.actionSelectionTime << " μs" << std::endl;
std::cout << "Cache hits: " << metrics.neuronGroupsCached << std::endl;
```

### 5. Memory Management

```cpp
// Clear caches when needed
agentBrain.clearCaches();

// Check memory usage
std::cout << "Current cache size: " << agentBrain.getCacheSize() << " bytes" << std::endl;
```

## Performance Considerations

### Optimization Strategies

1. **Caching**: Enable caching for frequently accessed data
2. **Batch Processing**: Process multiple inputs together for efficiency
3. **SIMD Support**: Use SIMD instructions for vectorized operations
4. **Multi-threading**: Utilize multiple CPU cores for parallel processing
5. **Memory Pools**: Reuse memory allocations to reduce garbage collection

### Performance Recommendations

#### For High-Performance Applications

```cpp
AgentBrainConfig perfConfig;
perfConfig.enableCaching = true;
perfConfig.enableBatchProcessing = true;
perfConfig.enablePerformanceMetrics = true;
perfConfig.enableCustomPlasticityRules = true;

AgentBrain agentBrain = AgentBrainBuilder::create(brain)
    .withConfig(perfConfig)
    .optimizeForPerformance()
    .build();
```

#### For Development and Debugging

```cpp
AgentBrainConfig debugConfig;
debugConfig.enableDebugLogging = true;
debugConfig.enablePerformanceMetrics = true;
debugConfig.enableCustomPlasticityRules = false;

AgentBrain agentBrain = AgentBrainBuilder::create(brain)
    .withConfig(debugConfig)
    .build();
```

## Migration Guide

### From Original to Enhanced

#### Configuration

**Original:**
```cpp
int neuronCount = config.getOr<int>("neuron_count", 500);
```

**Enhanced:**
```cpp
int neuronCount = config.getWithValidation<int>("neuron_count", 500);
```

#### Error Handling

**Original:**
```cpp
try {
    // Some operation
} catch (...) {
    // Generic error handling
}
```

**Enhanced:**
```cpp
try {
    // Some operation
} catch (const nlm::NLMException& e) {
    // Specific error handling
    if (e.getCode() == nlm::NLMException::ErrorCode::NeuronNotFound) {
        // Handle neuron not found
    }
}
```

#### AgentBrain Usage

**Original:**
```cpp
AgentBrain agentBrain(brain);
MotorCommand action = agentBrain.getAction();
```

**Enhanced:**
```cpp
AgentBrain agentBrain = AgentBrainBuilder::create(brain)
    .enableAllFeatures()
    .build();
MotorCommand action = agentBrain.getAction();
```

#### Testing

**Original Tests:**
```bash
./test_nlm
```

**Enhanced Tests:**
```bash
./test_nlm --enhanced
```

### Backward Compatibility

The enhanced implementation maintains 100% backward compatibility:

1. **Original APIs**: All original APIs continue to work unchanged
2. **Original Tests**: All original tests pass without modification
3. **Original Configuration**: Original configuration keys work as before
4. **Original Error Handling**: Original error handling patterns continue to work

### Recommended Migration Path

1. **Gradual Migration**: Start with configuration enhancements
2. **Error Handling**: Add error handling gradually as needed
3. **Performance**: Optimize performance when necessary
4. **Advanced Features**: Use expert features when required

## Conclusion

The NLM Phase 2 Enhanced implementation provides:

### Key Improvements

1. **Enhanced Error Handling**: Comprehensive exception hierarchy with specific error types
2. **AgentBrain v2.0**: Improved encapsulation with fluent API and expert features
3. **Type-Safe Configuration**: Enhanced configuration system with validation and schema
4. **Comprehensive Testing**: Extensive test coverage for both original and enhanced features
5. **Advanced Features**: Custom plasticity rules, batch processing, and performance monitoring
6. **Backward Compatibility**: 100% compatibility with original implementation

### Benefits

- **Reliability**: Robust error handling prevents crashes and provides detailed diagnostics
- **Flexibility**: Enhanced APIs support a wide range of use cases from basic to expert
- **Performance**: Optimized for high-performance applications with caching and batch processing
- **Maintainability**: Clear separation of concerns and comprehensive documentation
- **Extensibility**: Easy to add new features and capabilities

### Next Steps

1. **Try Enhanced Features**: Start with basic enhancements
2. **Add Error Handling**: Implement error handling for critical operations
3. **Optimize Performance**: Use performance features as needed
4. **Extend with Expert Features**: Add advanced features for specialized use cases

The NLM Phase 2 Enhanced implementation represents a significant step forward in neural simulation frameworks, providing the foundation for next-generation artificial brain research and development.
