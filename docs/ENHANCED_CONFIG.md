# Enhanced NLM Configuration System

This document describes the enhanced configuration system for NLM (Neural Learning Machine), which provides power users with granular control over the brain simulation systems.

## Overview

The traditional flat key-value configuration has been enhanced with a nested section-based structure that organizes configuration options by brain system (memory, neuromodulation, prediction, plasticity, etc.). This makes configuration more intuitive and easier to manage.

## Key Features

### 1. Nested Configuration Sections

Configuration is organized into logical sections:

- **memory**: Working memory, episodic memory, semantic memory, procedural memory, associative memory
- **neuromodulation**: Dopamine, acetylcholine, norepinephrine, serotonin, reward prediction error
- **prediction**: Neural prediction, action consequence prediction, prediction error signal
- **plasticity**: STDP, Hebbian learning, structural plasticity
- **development**: Critical periods, maturation, synaptogenesis, pruning
- **performance**: Checkpoint intervals, memory usage, simulation speed
- **cognition**: Planning, attention, concept formation, self-model
- **system**: Global settings, random seed, simulation parameters

### 2. Schema Validation

Each configuration value can have validation rules:

```cpp
// Integer validation (range check)
auto intValidator = std::make_shared<IntegerValidator>(0, 1000);
config.set<size_t>("capacity", 500, "Capacity", intValidator, true);

// Float validation
auto floatValidator = std::make_shared<FloatValidator>(0.0f, 1.0f);
config.set<float>("learning_rate", 0.5f, "Learning rate", floatValidator);

// String validation
auto stringValidator = std::make_shared<StringValidator>(1, 100);
config.set<std::string>("name", "brain", "Name", stringValidator);
```

### 3. Advanced Command Line Interface

The enhanced CLI supports both traditional and new options:

```bash
# Traditional options (backward compatible)
./nlm --memory-capacity=1000 --neuron-count=2000 --simulation-timestep=0.001

# New structured options
./nlm --section.memory.capacity=1000 --section.neuromodulation.dopamine-base=0.1

# Enable/disable features
./nlm --enable-reward-modulation --enable-structural-plasticity

# Show help
./nlm --help
```

### 4. Multiple File Format Support

Configuration files can now be in JSON or YAML format:

**JSON example (`config.json`):**

```json
{
    "memory": {
        "capacity": 1000,
        "decay_rate": 0.01,
        "working_memory_size": 100
    },
    "neuromodulation": {
        "dopamine_base": 0.1,
        "reward_modulation_enabled": true,
        "structural_plasticity_enabled": true
    },
    "system": {
        "random_seed": 42,
        "simulation_timestep": 0.001
    }
}
```

**YAML example (`config.yaml`):**

```yaml
memory:
  capacity: 1000
  decay_rate: 0.01
  working_memory_size: 100
neuromodulation:
  dopamine_base: 0.1
  reward_modulation_enabled: true
  structural_plasticity_enabled: true
system:
  random_seed: 42
  simulation_timestep: 0.001
```

## Configuration API

### Creating Default Configuration

```cpp
#include "core/Config/Config.hpp"
#include <memory>

// Create a default configuration with all brain systems
auto config = nlm::Config::createDefaultConfig();

// Validate all configuration values
config.validateAll();
```

### Accessing Configuration Values

**By Section (Recommended):**

```cpp
// Access memory system
auto& memorySection = config.addSection("memory"); // or get existing section
auto capacity = memorySection.get<size_t>("capacity");
memorySection.set<size_t>("capacity", 2000);

// Access neuromodulation system
auto& neuromodSection = config.addSection("neuromodulation");
auto dopamineLevel = neuromodSection.get<float>("dopamine_base");
neuromodSection.set<bool>("reward_modulation_enabled", true);
```

**Backward Compatibility (Flat Access):**

```cpp
// Traditional flat key-value access (still supported)
config.set("neuron_count", 2000);
auto neurons = config.get<size_t>("neuron_count");

// With source tracking
config.set("neuron_count", 2000, nlm::ConfigSource::CommandLine);
```

### Validation and Error Handling

```cpp
// Validate individual value
if (auto validator = memorySection.getValidator("capacity")) {
    if (!validator->validate(value)) {
        std::cerr << "Error: " << validator->getErrorMessage(value) << std::endl;
    }
}

// Validate all configuration
void validateAll() {
    try {
        config.validateAll();
        std::cout << "Configuration is valid!" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Configuration error: " << e.what() << std::endl;
    }
}
```

## Command Line Usage

### Basic Usage

```bash
# Create brain with default configuration
./nlm

# Load configuration from file
./nlm --config=config.json

# Override specific values
./nlm --neuron-count=2000 --memory-capacity=1500 --simulation-timestep=0.0005

# Enable/disable features
./nlm --enable-reward-modulation --disable-structural-plasticity

# Show current configuration
./nlm --show-config

# Save configuration to file
./nlm --save-config=custom_config.json
```

### Advanced Options

```bash
# Configure multiple sections
./nlm \
  --section.memory.capacity=2000 \
  --section.neuromodulation.dopamine-base=0.2 \
  --section.plasticity.stdp-ltp-weight=0.02 \
  --section.performance.checkpoint-interval=500 \
  --section.system.random-seed=123

# Batch operations
./nlm --enable-all --disable-reward-modulation

# Configuration validation
./nlm --validate-config
```

## File I/O Examples

### Saving and Loading Configuration

```cpp
// Save configuration to JSON file
config.saveToFile("my_brain_config.json");

// Load configuration from JSON file
nlm::Config config2;
config2.loadFromFile("my_brain_config.json");

// Save and load with error handling
try {
    config.saveToFile("production_config.json");
    config2.loadFromFile("production_config.json");
    config2.validateAll();
    std::cout << "Configuration loaded and validated successfully!" << std::endl;
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
}
```

### Checkpoint Configuration

```cpp
// Configure checkpoint system
config.set<size_t>("checkpoint_interval", 1000);
config.set<std::string>("checkpoint_dir", "./checkpoints");
config.set<size_t>("max_checkpoints", 10);
```

## Performance Configuration

### Simulation Speed Control

```cpp
// Control simulation speed
config.set<float>("simulation_speed", 2.0f);  // 2x real time
config.set<float>("simulation_speed", 0.5f); // 0.5x real time
```

### Memory Management

```cpp
// Configure memory usage limits
config.set<size_t>("max_memory_usage", 2000000000); // 2GB
config.set<size_t>("max_neurons", 50000);
config.set<size_t>("max_synapses", 500000);
```

### Checkpoint Configuration

```cpp
// Configure checkpointing behavior
config.set<size_t>("checkpoint_interval", 5000);    // Save every 5000 steps
config.set<size_t>("max_checkpoints", 20);          // Keep 20 checkpoints
config.set<bool>("checkpoint_compressed", true);   // Compress checkpoints
config.set<std::string>("checkpoint_dir", "./backups");
```

## Development and Testing

### Unit Tests

Run the enhanced configuration tests:

```bash
./build/tests/test_config
```

The test suite includes:
- Creation and section management
- Set/get operations for all types
- Validation and error handling
- Default configuration creation
- File I/O operations
- Backward compatibility

### Integration Tests

For full integration with brain simulation:

```bash
# Run with custom configuration
./nlm --config=advanced_config.json --show-config

# Test specific configuration scenarios
./nlm --test-scenario=learning --config=learning_scenario.json
```

## Migration Guide

### From Flat to Nested Configuration

If you have existing configuration files using the flat key=value format, you can migrate by:

1. **Manual migration**: Add section prefixes to keys
   ```
   neuron_count -> section.system.neuron_count
   memory_capacity -> section.memory.capacity
   reward_modulation_enabled -> section.neuromodulation.reward_modulation_enabled
   ```

2. **Automatic migration tool**: Use the provided migration script (to be developed)

### Backward Compatibility

The enhanced configuration system maintains full backward compatibility:

```cpp
// Old code still works
nlm::Config config;
config.set("neuron_count", 1000, nlm::ConfigSource::File);
auto neurons = config.get<size_t>("neuron_count");

// New code for advanced users
nlm::Config config = nlm::Config::createDefaultConfig();
config.set<size_t>("neuron_count", 2000);
config.validateAll();
```

## Best Practices

### 1. Use Sections for Related Settings

```cpp
// Good - related settings in one section
config.addSection("memory")
    .set<size_t>("capacity", 1000)
    .set<float>("decay_rate", 0.01)
    .set<size_t>("working_memory_size", 100);

// Bad - scattered settings
config.set<size_t>("capacity", 1000);
config.set<float>("decay_rate", 0.01);
config.set<size_t>("working_memory_size", 100);
```

### 2. Validate Critical Configuration

```cpp
// Always validate configuration that affects system stability
config.validateAll();  // Throws exception on invalid values
```

### 3. Use Appropriate Validators

```cpp
// Use validators for critical parameters
config.addSection("system")
    .set<size_t>("random_seed", 42, "Random seed", 
                 std::make_shared<IntegerValidator>(0, 2147483647), true);
```

### 4. Test Configuration Changes

```cpp
// Test configuration changes before applying
auto testConfig = nlm::Config::createDefaultConfig();
testConfig.set<size_t>("neuron_count", 2000);
testConfig.validateAll();
// If validation passes, apply to real config
config = testConfig;
```

## Troubleshooting

### Common Issues and Solutions

**Issue 1: Configuration key not found**
```cpp
// Solution: Check if key exists before accessing
if (config.has("neuron_count")) {
    auto neurons = config.get<size_t>("neuron_count");
} else {
    auto neurons = config.getOr<size_t>("neuron_count", 1000);
}
```

**Issue 2: Validation errors**
```cpp
// Solution: Catch validation exceptions
try {
    config.validateAll();
} catch (const std::runtime_error& e) {
    std::cerr << "Configuration error: " << e.what() << std::endl;
    // Fix invalid configuration and try again
}
```

**Issue 3: File I/O errors**
```cpp
// Solution: Handle file access errors
if (!config.loadFromFile("config.json")) {
    std::cerr << "Failed to load configuration from config.json" << std::endl;
}
```

## Future Enhancements

The configuration system is designed for future extensibility:

1. **Dynamic configuration** - Runtime configuration changes
2. **Configuration inheritance** - Base configurations with overrides
3. **Configuration templates** - Predefined configuration sets
4. **Configuration monitoring** - Runtime configuration monitoring and alerting
5. **Configuration versioning** - Configuration history and rollback

## Conclusion

The enhanced configuration system provides power users with comprehensive control over NLM brain simulation parameters. Whether you're configuring a simple learning experiment or a complex cognitive architecture, the nested section structure, validation, and multiple file format support make it easy to create and manage sophisticated configurations.

For the complete API reference and implementation details, see the `Config.hpp` and `Config.cpp` files in the `src/core/Config/` directory.