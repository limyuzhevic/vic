# NLM Phase 2 Enhanced: Development Guide

## Quick Start

Welcome to the NLM Phase 2 Enhanced documentation! This guide provides comprehensive information for developing with the enhanced NLM framework.

## What’s New in Phase 2 Enhanced

This version of NLM Phase 2 introduces significant enhancements:

1. **Enhanced Error Handling**: Comprehensive exception hierarchy with specific error types
2. **AgentBrain v2.0**: Improved encapsulation with fluent API and expert features
3. **Type-Safe Configuration**: Enhanced configuration system with validation and schema
4. **Comprehensive Testing**: Extensive test coverage for both original and enhanced features
5. **Advanced Features**: Custom plasticity rules, batch processing, and performance monitoring
6. **Backward Compatibility**: 100% compatibility with original implementation

## Getting Started

### Prerequisites

- C++ compiler with C++17 support
- CMake 3.16 or higher
- Python 3.8 or higher (for testing)

### Building the Project

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4
```

### Running Tests

```bash
# Run all tests (original + enhanced)
./test_nlm

# Run only enhanced tests
./test_nlm --enhanced

# Run specific test suites
./test_nlm --neuron    # Neuron tests
./test_nlm --config    # Configuration tests
./test_nlm --agent     # Agent tests
```

### Example Usage

#### Basic Example

```cpp
#include "include/nlm/NLM.h"
#include "include/nlm/AgentBrain.hpp"

int main() {
    // Initialize NLM framework
    nlm::initialize();
    
    // Create brain configuration
    auto config = std::make_shared<nlm::Config>();
    config->setInt("neuron_count", 1000);
    config->setDouble("simulation_timestep", 0.001);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create agent brain with enhanced configuration
    nlm::AgentBrainConfig agentConfig;
    agentConfig.enableCuriosity = true;
    agentConfig.curiosityThreshold = 0.3f;
    
    nlm::AgentBrain agentBrain(brain, agentConfig);
    agentBrain.initialize(world);
    
    // Main simulation loop
    while (simulationSteps < maxSteps) {
        // Get observation
        auto percept = world.observe(brain->getRegions()[0].get());
        
        // Process sensory input
        agentBrain.process(percept);
        
        // Get action
        auto action = agentBrain.getAction();
        
        // Apply action to world
        world.applyAction(brain->getRegions()[0].get(), action);
        
        // Update brain
        brain->step(simulationSteps, simulationSteps * 0.001);
        
        simulationSteps++;
    }
    
    return 0;
}
```

#### Advanced Example with Expert Features

```cpp
#include "include/nlm/AgentBrain.hpp"

// Custom plasticity rule for expert users
auto customPlasticityRule = [](float dopamine) -> float {
    // Custom rule: exponential response to dopamine
    return std::exp(dopamine * 2.0f) * 0.1f;
};

int main() {
    // Create brain
    auto brain = createBrain();
    
    // Configure agent with expert features
    nlm::AgentBrainConfig expertConfig;
    expertConfig.enableAllFeatures();  // Enable all advanced features
    expertConfig.optimizeForPerformance();  // Optimize for speed
    
    nlm::AgentBrain agentBrain = nlm::AgentBrainBuilder::create(brain)
        .withConfig(expertConfig)
        .build();
    
    // Set custom plasticity rule
    agentBrain.setCustomPlasticityRule(customPlasticityRule);
    
    // Enable batch processing for high performance
    agentBrain.enableBatchProcessing(true);
    
    // Register custom callbacks
    agentBrain.setSensoryCallback([](const std::vector<float>& sensoryData) {
        // Custom sensory processing logic
    });
    
    // Run simulation
    runSimulation(agentBrain);
    
    return 0;
}
```

## Configuration

### Configuration Files

NLM supports multiple configuration file formats:

1. **JSON Configuration**
2. **INI/CFG Configuration**
3. **Environment Variables**
4. **Command Line Arguments**

#### JSON Configuration Example

```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.2,
    "simulation_timestep": 0.001,
    
    "neuron_dynamics": {
        "lif_resting_potential": -70.0,
        "lif_threshold": -55.0,
        "lif_refractory_period": 10
    },
    
    "plasticity": {
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015
    }
}
```

#### Environment Variables

```bash
# Set environment variables
export NLM_NEURON_COUNT=1000
export NLM_CONNECTION_PROBABILITY=0.2
export NLM_SIMULATION_TIMESTEP=0.001
export NLM_ENABLE_DEBUG_LOGGING=true
```

#### Command Line Arguments

```bash
./nlm_app --neuron_count=1000 --connection_probability=0.2 --simulation_timestep=0.001
```

### Configuration Schema

The enhanced configuration system includes comprehensive schema validation:

#### Core Configuration Keys

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `neuron_count` | int | 500 | Number of neurons |
| `region_count` | int | 1 | Number of regions |
| `connection_probability` | float | 0.15 | Connection probability |
| `simulation_timestep` | double | 0.001 | Simulation timestep |
| `random_seed` | int64_t | 42 | Random seed |

#### Neuron Dynamics Configuration

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `lif_resting_potential` | float | -70.0 | Resting potential (mV) |
| `lif_threshold` | float | -55.0 | Firing threshold (mV) |
| `lif_refractory_period` | int | 5 | Refractory period (steps) |

#### Advanced Configuration

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `enable_custom_plasticity` | bool | false | Enable custom plasticity rules |
| `enable_advanced_prediction` | bool | false | Enable advanced prediction |
| `enable_debug_logging` | bool | false | Enable debug logging |
| `enable_performance_metrics` | bool | false | Enable performance metrics |

## Error Handling

### Exception Hierarchy

The enhanced error handling system provides:

#### Base Exception Classes

- **NLMException**: Base exception for all NLM errors
- **NLMConfigException**: Configuration-specific errors
- **NLMNeuralException**: Neural system errors
- **NLMSimulationException**: Simulation-specific errors

#### Error Codes

| Error Code | Description |
|------------|-------------|
| `MemoryAllocationFailed` | Memory allocation failed |
| `ConfigValidationFailed` | Configuration validation failed |
| `NeuronNotFound` | Neuron not found |
| `SynapseNotFound` | Synapse not found |
| `SimulationNotRunning` | Simulation not running |
| `InvalidParameter` | Invalid parameter value |
| `Timeout` | Operation timed out |

### Error Handling Examples

#### Basic Error Handling

```cpp
#include "include/nlm/NLMErrors.hpp"

// Basic error handling
try {
    config.setInt("neuron_count", -5);
} catch (const nlm::NLMConfigException& e) {
    std::cerr << "Configuration error: " << e.what() << std::endl;
    std::cerr << "Key: " << e.getContext() << std::endl;
}
```

#### Neural System Error Handling

```cpp
// Neural system error handling
try {
    neuron = brain->getNeuron(42);
} catch (const nlm::NLMNeuralException& e) {
    std::cerr << "Neural system error: " << e.what() << std::endl;
    std::cerr << "Entity: " << e.getContext() << std::endl;
}
```

#### Simulation Error Handling

```cpp
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

## Testing

### Running Tests

```bash
# Run all tests
./test_nlm

# Run only enhanced tests
./test_nlm --enhanced

# Run specific test suites
./test_nlm --neuron    # Neuron tests
./test_nlm --config    # Configuration tests
./test_nlm --agent     # Agent tests
```

### Test Structure

The test framework includes:

1. **Original Tests**: Backward compatibility maintained
2. **Enhanced Neuron Tests**: Comprehensive error handling tests
3. **Enhanced Configuration Tests**: Type safety and validation tests
4. **Error Handling Tests**: Exception and recovery tests
5. **Performance Tests**: Benchmarking and optimization tests

### Adding New Tests

```cpp
// Add enhanced neuron tests
namespace test_neuron_enhanced {
    void testNeuronErrorHandling();
    void testNeuronErrorHandlingConstructor();
    // ... other tests
}

// Add enhanced configuration tests
namespace test_config_enhanced {
    void testConfigurationTypeSafety();
    void testConfigurationValidation();
    // ... other tests
}
```

## Advanced Features

### Custom Plasticity Rules

```cpp
// Define custom plasticity rule
auto customPlasticityRule = [](float dopamine) -> float {
    // Custom rule: stronger plasticity for moderate dopamine
    if (dopamine < 0.0f) return 0.5f + dopamine * 0.5f;
    return 1.0f + dopamine * 0.3f;
};

agentBrain.setCustomPlasticityRule(customPlasticityRule);
```

### Batch Processing

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

### Performance Monitoring

```cpp
// Enable performance metrics
agentBrain.enablePerformanceMetrics(true);

// Access performance data
const auto& metrics = agentBrain.getPerformanceMetrics();
std::cout << "Sensory processing time: " << metrics.sensoryProcessingTime << " μs" << std::endl;
std::cout << "Action selection time: " << metrics.actionSelectionTime << " μs" << std::endl;
```

## Performance Optimization

### Optimization Strategies

1. **Caching**: Enable caching for frequently accessed data
2. **Batch Processing**: Process multiple inputs together for efficiency
3. **SIMD Support**: Use SIMD instructions for vectorized operations
4. **Multi-threading**: Utilize multiple CPU cores for parallel processing
5. **Memory Pools**: Reuse memory allocations to reduce garbage collection

### Performance Recommendations

#### For High-Performance Applications

```cpp
nlm::AgentBrainConfig perfConfig;
perfConfig.enableCaching = true;
perfConfig.enableBatchProcessing = true;
perfConfig.enablePerformanceMetrics = true;
perfConfig.enableCustomPlasticityRules = true;

nlm::AgentBrain agentBrain = nlm::AgentBrainBuilder::create(brain)
    .withConfig(perfConfig)
    .optimizeForPerformance()
    .build();
```

#### For Development and Debugging

```cpp
nlm::AgentBrainConfig debugConfig;
debugConfig.enableDebugLogging = true;
debugConfig.enablePerformanceMetrics = true;
debugConfig.enableCustomPlasticityRules = false;

nlm::AgentBrain agentBrain = nlm::AgentBrainBuilder::create(brain)
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

## Troubleshooting

### Common Issues and Solutions

#### 1. Configuration Validation Errors

**Problem**: Configuration validation fails
**Solution**: Use `getWithValidation()` for safe access:

```cpp
int safeValue = config.getWithValidation<int>("neuron_count", 500);
```

#### 2. Memory Allocation Issues

**Problem**: Memory allocation fails
**Solution**: Check system resources and increase memory limits:

```bash
ulimit -v unlimited  # Increase virtual memory limit
```

#### 3. Performance Issues

**Problem**: Simulation runs slowly
**Solution**: Enable performance optimizations:

```cpp
agentBrain.enableCaching(true);
agentBrain.enableBatchProcessing(true);
```

#### 4. Error Handling Not Working

**Problem**: Custom error handling not triggered
**Solution**: Ensure proper exception handling:

```cpp
try {
    // Some operation
} catch (const nlm::NLMException& e) {
    // Handle error
}
```

## Support and Community

### Getting Help

1. **Documentation**: Refer to this documentation for detailed information
2. **Issue Tracking**: Report issues on GitHub
3. **Community Forum**: Join the NLM community forum
4. **Technical Support**: Contact technical support for enterprise users

### Contributing

1. **Code Contributions**: Submit pull requests with comprehensive tests
2. **Documentation**: Contribute to documentation and examples
3. **Bug Reports**: Report bugs with detailed reproduction steps
4. **Feature Requests**: Suggest new features and capabilities

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
