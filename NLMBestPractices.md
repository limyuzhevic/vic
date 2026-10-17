# NLM — Best Practices Guide

This document provides comprehensive best practices for the NLM codebase, covering error handling, configuration management, simulation, development, and performance.

---

## Overview

This best practices guide provides recommendations for using, developing, and maintaining the NLM codebase. It covers error handling, configuration management, simulation execution, and performance optimization.

## Error Handling Best Practices

### 1. Always Use NLM Error Handling Macros

**Why:** Comprehensive error checking prevents crashes and provides detailed error messages.

**How:** Use NLM_CHECK_NULL, NLM_CHECK_CONDITION, and other error handling macros throughout the codebase.

**Example:**
```cpp
// Before
if (!region) {
    std::cerr << "Region is null" << std::endl;
    return;
}

// After
NLM_CHECK_NULL(region, "region");
```

### 2. Validate All Inputs

**Why:** Prevents undefined behavior and provides clear error messages.

**How:** Use validation macros and configuration validation for all inputs.

**Example:**
```cpp
// Configuration validation
if (!config.has("brain.neuron_count")) {
    throw ValidationError("Neuron count not specified", 
                         "brain.neuron_count", 
                         ValidationError::ErrorType::MissingRequired);
}

int neuron_count = config.getInt("brain.neuron_count");
NLM_CHECK_CONDITION(neuron_count > 0,
                   "Neuron count must be positive",
                   "brain.neuron_count",
                   ValidationError::ErrorType::InvalidRange);
```

### 3. Handle File Operations Gracefully

**Why:** File operations can fail due to permissions, disk space, or file corruption.

**How:** Use try-catch blocks and proper error handling for all file operations.

**Example:**
```cpp
try {
    config.loadFromFile("config.json");
    config.saveToFile("backup_config.json");
} catch (const FileError& e) {
    std::cerr << "File operation failed: " << e.what() << std::endl;
    return 1;
}
```

### 4. Use Result Types for Error Handling

**Why:** Result types provide a clean interface for functions that can fail.

**How:** Use the Result template for public API functions that can fail.

**Example:**
```cpp
template<typename T>
class Result {
public:
    bool isSuccess() const;
    T get() const;
    const Error& getError() const;
    static Result success(T value);
    static Result failure(Error error);
};

// Usage
Result<int> getNeuronCount() {
    if (invalid_config) {
        return Result<int>::failure(ConfigError("Invalid configuration", "brain.neuron_count"));
    }
    return Result<int>::success(1000);
}
```

### 5. Log Errors Appropriately

**Why:** Proper error logging helps with debugging and monitoring.

**How:** Use the Logger system for all errors and provide detailed context.

**Example:**
```cpp
void processSensoryInput(const Percept& percept) {
    try {
        // Process input
    } catch (const ValidationError& e) {
        NLM_LOG_ERROR("Failed to process sensory input: " << e.what()
                     << " (field: " << e.getField() << ")");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error processing sensory input: " << e.what());
    }
}
```

## Configuration Best Practices

### 1. Use Presets for Common Configurations

**Why:** Presets provide optimized configurations for common use cases.

**How:** Use built-in presets for specific scenarios.

**Example:**
```bash
# For production environments
./nlm --preset=production

# For debugging
./nlm --preset=debug

# For simple testing
./nlm --preset=basic
```

### 2. Validate Configuration Before Use

**Why:** Invalid configurations can cause crashes or unexpected behavior.

**How:** Always validate configuration before running simulations.

**Example:**
```bash
# Validate current configuration
./nlm --validate-config

# Validate specific configuration file
./nlm --validate-config --config=production.cfg

# Validate with schema file
./nlm --validate-config --schema=custom_schema.json
```

### 3. Use Environment Variables for Configuration

**Why:** Environment variables provide a flexible way to override configuration.

**How:** Set NLM_ prefixed environment variables.

**Example:**
```bash
# Set environment variables
export NLM_brain_neuron_count=1000
export NLM_brain_synapse_density=0.1
export NLM_log_level=DEBUG

# Override via CLI
./nlm --config=basic.cfg --brain_neuron_count=500
```

### 4. Use Templates for Parameterized Configurations

**Why:** Templates allow you to create parameterized configurations.

**How:** Use configuration templates with overrides.

**Example:**
```bash
# Use default template with overrides
./nlm --template=default --override="neuron_count=1000,--plasticity_learning_rate=0.05"

# Use debug template with custom settings
./nlm --template=debug --override="neuron_count=100,--log_level=DEBUG"
```

### 5. Maintain Configuration Version Control

**Why:** Configuration version control helps track changes and revert when needed.

**How:** Save and version control configuration files.

**Example:**
```bash
# Create configuration versions
mkdir configs

# Save different configurations
cat > configs/basic.json << EOF
{
  "brain": {
    "neuron_count": 1000,
    "synapse_density": 0.1
  }
}
EOF

cat > configs/advanced.json << EOF
{
  "brain": {
    "neuron_count": 5000,
    "synapse_density": 0.05
  },
  "plasticity": {
    "stdp": { "enable": true, "learning_rate": 0.001 }
  }
}
EOF
```

## Simulation Best Practices

### 1. Enable Learning Features for Realistic Simulations

**Why:** Learning features make the simulation more realistic and interesting.

**How:** Enable all relevant learning features for your simulation.

**Example:**
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent
agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Initialize agent
agent.initialize(world)

# Enable all learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

### 2. Use Checkpoints for Long Simulations

**Why:** Checkpoints allow you to save and restore simulation state.

**How:** Save checkpoints periodically during long simulations.

**Example:**
```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with checkpointing
checkpoint_interval = 100

for step in range(0, 10000, checkpoint_interval):
    for i in range(checkpoint_interval):
        brain.step(step + i)
    
    # Save checkpoint
    checkpoint_file = f"checkpoint_step_{step + checkpoint_interval}.bin"
    brain.save(checkpoint_file)
    print(f"Saved checkpoint: {checkpoint_file}")
    
    # Monitor memory usage
    memory_usage = brain.getMemoryUsage()
    print(f"Memory usage: {memory_usage} bytes")

# Resume from checkpoint (if needed)
# brain.load("checkpoint_step_500.bin")
```

### 3. Monitor Simulation Statistics

**Why:** Monitoring helps you understand simulation behavior and detect issues.

**How:** Monitor key statistics periodically during simulation.

**Example:**
```python
import pynlm

# Create and run simulation
brain, agent, world = run_simulation(1000)

# Monitor statistics periodically
for step in range(0, 1000, 100):
    print(f"Step {step}:")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate()}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"  Curiosity level: {agent.getCuriosityLevel()}")
    print(f"  Novelty level: {agent.getNoveltyLevel()}")
    print(f"  Prediction error: {agent.getPredictionError()}")
    print(f"  Neuromodulation level: {agent.getNeuromodulationLevel()}")
```

### 4. Use Appropriate Simulation Parameters

**Why:** Appropriate parameters ensure simulation stability and performance.

**How:** Choose simulation parameters based on your goals.

**Example:**
```bash
# For short demonstrations
./nlm --preset=basic

# For research
./nlm --preset=advanced

# For debugging
./nlm --preset=debug

# For production
./nlm --preset=production
```

## Development Best Practices

### 1. Use Error Handling Macros

**Why:** Consistent error handling throughout the codebase.

**How:** Use NLM error handling macros for all pointer access and validation.

**Example:**
```cpp
// Use NLM macros for all critical operations
NLM_CHECK_NULL(region, "region");
NLM_CHECK_CONDITION(population_count > 0, 
                   "Population must have neurons",
                   "population_count",
                   ValidationError::ErrorType::InvalidRange);
```

### 2. Follow Configuration Schema

**Why:** Schema validation ensures configuration validity.

**How:** Validate configuration against schema.

**Example:**
```cpp
// Validate configuration schema
try {
    config.loadFromFile("config.json");
    config.validateSchema();
} catch (const ConfigError& e) {
    NLM_THROW_CONFIG("Invalid configuration schema", "schema");
}
```

### 3. Implement Proper Resource Management

**Why:** Proper resource management prevents memory leaks and crashes.

**How:** Use SafeResource for exception-safe resource cleanup.

**Example:**
```cpp
// Use SafeResource for exception-safe resource management
SafeResource<std::mutex> lock_guard(mutex);

// Use SafeResource for file operations
SafeResource<std::fstream> file("config.json", std::ios::in);
if (!file) {
    NLM_THROW_FILE("open", "config.json", "Failed to open file", FileError::ErrorType::NotFound);
}
```

### 4. Write Comprehensive Tests

**Why:** Comprehensive tests ensure code quality and prevent regressions.

**How:** Write tests for all error handling scenarios and configuration validation.

**Example:**
```cpp
// Test error handling
void testErrorHandling() {
    // Test null pointer checking
    NLM_CHECK_NULL(nullptr, "test"); // Should throw
    
    // Test condition checking
    NLM_CHECK_CONDITION(false, "Test", "field", ValidationError::ErrorType::InvalidValue); // Should throw
    
    // Test configuration validation
    try {
        config.loadFromFile("invalid_config.json");
        fail("Should have thrown ConfigError");
    } catch (const ConfigError& e) {
        // Test passed
    }
}
```

## Performance Best Practices

### 1. Use Appropriate Configuration Presets

**Why:** Presets are optimized for specific use cases.

**How:** Use performance-optimized presets for performance-critical applications.

**Example:**
```bash
# For performance-critical applications
./nlm --preset=production
```

### 2. Enable Performance Optimizations

**Why:** Performance optimizations improve simulation speed.

**How:** Enable SIMD vectorization, parallel processing, and memory pools.

**Example:**
```cpp
// Enable performance optimizations
brain.enableSIMD(true);
brain.enableParallelProcessing(true);
brain.enableMemoryPools(true);
brain.enableEventQueues(true);
```

### 3. Monitor Resource Usage

**Why:** Resource monitoring helps identify performance bottlenecks.

**How:** Monitor memory usage and step times.

**Example:**
```python
import pynlm
import time

# Create and run simulation
brain, agent, world = run_simulation(1000)

# Monitor resource usage
start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()

elapsed_time = end_time - start_time
print(f"Simulation time: {elapsed_time:.2f} seconds")
print(f"Average step time: {elapsed_time/1000*1000:.2f} ms")
print(f"Memory usage: {brain.getMemoryUsage()} bytes")
```

### 4. Use Efficient Data Structures

**Why:** Efficient data structures improve performance and memory usage.

**How:** Use appropriate data structures for your use case.

**Example:**
```cpp
// Use vector for dynamic collections
std::vector<Neuron*> neurons;
neurons.reserve(1000);

// Use unordered_map for fast lookups
std::unordered_map<std::string, ConfigValue> config_map;

// Use map for ordered collections
std::map<std::string, ConfigValue> ordered_config;
```

## Best Practices Summary

### Error Handling

| Practice | Description |
|----------|-------------|
| Use macros | Always use NLM error handling macros |
| Validate inputs | Validate all function inputs |
| Handle files | Gracefully handle file operations |
| Log errors | Log errors appropriately |
| Use Result types | Use Result type for error handling |

### Configuration

| Practice | Description |
|----------|-------------|
| Use presets | Use built-in presets for common scenarios |
| Validate config | Always validate configuration |
| Use env vars | Use environment variables for overrides |
| Use templates | Use templates for parameterized configurations |
| Version control | Maintain configuration version control |

### Simulation

| Practice | Description |
|----------|-------------|
| Enable learning | Enable all relevant learning features |
| Use checkpoints | Use checkpoints for long simulations |
| Monitor stats | Monitor simulation statistics |
| Use appropriate params | Choose appropriate simulation parameters |

### Development

| Practice | Description |
|----------|-------------|
| Use macros | Use NLM error handling macros |
| Follow schema | Validate configuration against schema |
| Resource management | Use proper resource management |
| Write tests | Write comprehensive tests |

### Performance

| Practice | Description |
|----------|-------------|
| Use presets | Use performance-optimized presets |
| Enable optimizations | Enable SIMD, parallel processing, memory pools |
| Monitor resources | Monitor memory usage and performance |
| Use efficient data structures | Use appropriate data structures |

---

## References

1. **NLMUserGuide.md** - User guide and tutorials
2. **NLMApiReference.md** - API documentation
3. **NLMConfigurationReference.md** - Configuration documentation
4. **NLMExamples.md** - Additional examples
5. **NLMMigrationGuide.md** - Migration guide
6. **NLMTroubleshooting.md** - Troubleshooting guide

---

*Last Updated: Phase 6 Completion*
*Version: 1.0.0*