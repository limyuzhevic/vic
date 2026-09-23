# Tutorial 2: Advanced Configuration

## Overview

This tutorial introduces you to advanced NLM (Neural Learning Machine) configuration features. It covers:

- Schema-based configuration with validation
- Factory presets for common configurations
- Configuration file I/O (JSON, TOML, YAML)
- Configuration validation and error handling
- Performance optimization through configuration

This tutorial is designed for users who want to get the most out of NLM's powerful configuration system.

## Prerequisites

Before starting this tutorial, you should:

1. **Be familiar with Tutorial 1**: Basic brain creation and simulation
2. **Understand configuration concepts**: Key-value pairs, types, validation
3. **Have NLM installed**: As per the main installation instructions

## What You'll Learn

By the end of this tutorial, you will:

- Understand the NLM configuration system architecture
- Create schemas for configuration validation
- Use factory presets for common configurations
- Load and save configuration files
- Validate configuration with comprehensive error handling
- Optimize configuration for performance

## Setting Up Your Environment

The same as Tutorial 1. Ensure NLM is installed:

```bash
pip install pynlm
```

## NLM Configuration System

### Overview

NLM's configuration system is a modern, type-safe configuration management framework that provides:

1. **Type Safety**: Compile-time type checking and type-safe APIs
2. **Validation**: Comprehensive validation with custom validators
3. **Schema Support**: Full schema definition and validation
4. **Clean API**: Simplified, modern C++ interface
5. **Backward Compatibility**: Existing code continues to work
6. **Extensibility**: Easy to add new types, validators, and features

### Configuration Architecture

```
NLM Configuration System

┌─────────────────────────────────────────────────────────┐
│                    Global Configuration                │
├─────────────────────────────────────────────────────────┤
│  ┌─────────────────┐  ┌─────────────────┐            │
│  │   Config       │  │   ConfigSchema  │            │
│  │   (Runtime)    │  │   (Definition)  │            │
│  └─────────────────┘  └─────────────────┘            │
│         │                      │                   │
│   ┌─────▼─────┐        ┌─────────▼─────────┐           │
│   │ Values    │        │ Validation Rules  │           │
│   │ (Runtime) │        │ (Schema-based)   │           │
│   └───────────┘        └───────────────────┘           │
│                                │                   │
│   ┌─────────────────┐  ┌────────▼────────┐          │
│   │ Validators      │  │  Error Reporting │          │
│   │ (Custom Rules)  │  │  (Detailed)      │          │
│   └─────────────────┘  └───────────────────┘          │
└─────────────────────────────────────────────────────────┘
```

## Configuration System Components

### 1. Config Class

The main configuration class with type-safe API:

```python
class Config:
    """Configuration management for NLM brain."""
    
    def __init__(self):
        """Initialize empty configuration."""
    
    @staticmethod
    def createDefaultConfig() -> 'Config':
        """Create default configuration."""
    
    def set(self, key: str, value: Any) -> None:
        """Set configuration value."""
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value."""
    
    def has(self, key: str) -> bool:
        """Check if configuration has key."""
    
    def getKeys(self) -> List[str]:
        """Get all configuration keys."""
    
    def loadFromFile(self, filename: str) -> bool:
        """Load configuration from file."""
    
    def saveToFile(self, filename: str) -> bool:
        """Save configuration to file."""
    
    def clear(self) -> None:
        """Clear all configuration values."""
```

### 2. ConfigSchema Class

Schema definition for configuration validation:

```python
class ConfigSchema:
    """Schema definition for configuration validation."""
    
    def __init__(self, name: str, type: Type, required: bool, 
                 defaultValue: Any = None, description: str = ""):
        """Initialize schema entry."""
    
    @staticmethod
    def createBuilder() -> 'ConfigSchemaBuilder':
        """Create schema builder."""
    
    def withRange(self, min: float, max: float) -> 'ConfigSchema':
        """Add range validation."""
    
    def withLengthRange(self, min: int, max: int) -> 'ConfigSchema':
        """Add string length validation."""
    
    def addValidator(self, validator: 'IValidator') -> 'ConfigSchema':
        """Add custom validator."""
```

### 3. ConfigValidator Class

Validation logic for configuration values:

```python
class ConfigValidator:
    """Validation logic for configuration values."""
    
    def validateValue(self, schema: ConfigSchema, value: Any) -> bool:
        """Validate single value against schema."""
    
    def validateAll(self, config: Config, schema: ConfigSchema) -> bool:
        """Validate all configured values."""
    
    def getValidationErrorMessage(self, schema: ConfigSchema, value: Any) -> str:
        """Get detailed error message for validation failure."""
```

### 4. ConfigFactory Class

Factory class for creating common configuration presets:

```python
class ConfigFactory:
    """Factory class for creating common configuration presets."""
    
    @staticmethod
    def createNeuralNetworkSchema() -> ConfigSchema:
        """Create schema for neural network configurations."""
    
    @staticmethod
    def createSimulationSchema() -> ConfigSchema:
        """Create schema for simulation configurations."""
    
    @staticmethod
    def createLoggingSchema() -> ConfigSchema:
        """Create schema for logging configurations."""
    
    @staticmethod
    def createMemorySchema() -> ConfigSchema:
        """Create schema for memory configurations."""
```

## Schema-Based Configuration

### Creating a Schema

```python
import pynlm

# Create schema with ConfigBuilder
auto schema = pynlm.Config::createBuilder()
    .add("port", pynlm.ConfigSchema("port", pynlm.ConfigSchema::Type::Integer, true))
        .withRange(1, 65535)
        .withDefault(8080)
        .withDescription("Network port for simulation server")
    .add("host", pynlm.ConfigSchema("host", pynlm.ConfigSchema::Type::String, false))
        .withLengthRange(1, 255)
        .withDefault("localhost")
        .withDescription("Host address for simulation server")
    .add("learning_rate", pynlm.ConfigSchema("learning_rate", pynlm.ConfigSchema::Type::Double, false))
        .withRange(0.0, 1.0)
        .withDefault(0.01)
        .withDescription("Learning rate for neural network")
    .add("neuron_count", pynlm.ConfigSchema("neuron_count", pynlm.ConfigSchema::Type::Integer, true))
        .withRange(100, 1000000)
        .withDefault(1000)
        .withDescription("Number of neurons in the network");

# Initialize configuration with schema
config.initializeWithSchema(schema.build());
```

### Schema Validation

```python
# Create schema
auto schema = pynlm::Config::createBuilder()
    .add("neuron_count", pynlm::ConfigSchema("neuron_count", pynlm::ConfigSchema::Type::Integer, true))
        .withRange(100, 1000000)
        .withDefault(1000)
    .add("learning_rate", pynlm::ConfigSchema("learning_rate", pynlm::ConfigSchema::Type::Double, false))
        .withRange(0.0, 1.0)
        .withDefault(0.01)
    .build();

config.initializeWithSchema(schema);

// Set valid value
config.setInt("neuron_count", 5000);

// Try to set invalid value (will throw exception)
try {
    config.setInt("neuron_count", 2000);  // Exceeds schema range
} catch (const pynlm::ConfigError& e) {
    std::cout << "Validation error: " << e.what() << std::endl;
}

// Check validity without throwing
if (config.validateAgainstSchema(schema)) {
    std::cout << "Configuration is valid" << std::endl;
} else {
    std::cout << "Validation errors: " << config.getValidationErrors() << std::endl;
}
```

## Factory Presets

### Neural Network Schema

```json
{
  "brain.neuron_count": 1000,
  "brain.synapse_density": 0.1,
  "brain.connection_probability": 0.05,
  "brain.initial_weight_mean": 0.5,
  "brain.initial_weight_std": 0.1,
  "brain.v_thresh": -50.0,
  "brain.v_rest": -70.0,
  "brain.v_reset": -75.0,
  "brain.tau_mem": 20.0,
  "brain.tau_ref": 2.0,
  "plasticity.stdp.enable": true,
  "plasticity.stdp.learning_rate": 0.001,
  "plasticity.stdp.tau_plus": 20.0,
  "plasticity.stdp.tau_minus": 20.0,
  "plasticity.hebbian.enable": true,
  "plasticity.structural.enable": true,
  "neuromod.dopamine.scale": 1.0,
  "neuromod.curiosity.enable": true,
  "neuromod.novelty.enable": true
}
```

### Simulation Schema

```json
{
  "simulation.timestep": 0.1,
  "simulation.max_steps": 10000,
  "simulation.random_seed": 42,
  "world.width": 20,
  "world.height": 20,
  "world.vision_width": 8,
  "world.vision_height": 8,
  "agent.max_energy": 100.0,
  "agent.energy_decay": 0.01
}
```

### Logging Schema

```json
{
  "logging.level": "INFO",
  "logging.file.enabled": true,
  "logging.file.path": "logs/nlm_$(date).log",
  "logging.console.enabled": true,
  "logging.buffer_size": 8192,
  "logging.flush_interval_ms": 100
}
```

### Memory Schema

```json
{
  "memory.neuron_pool_size": 1000000,
  "memory.synapse_pool_size": 10000000,
  "memory.event_queue_capacity": 16384,
  "memory.checkpoint_interval": 1000,
  "memory.enable_compression": true
}
```

## Configuration File I/O

### Loading Configuration

```python
import pynlm

# Load configuration from JSON file
config = pynlm.createDefaultConfig()

if config.loadFromFile("config.json"):
    print("Configuration loaded successfully")
else:
    print("Failed to load configuration")

# Load configuration from file with error handling
try:
    config = pynlm.createDefaultConfig()
    if config.loadFromFile("config.json") {
        print("Configuration loaded successfully")
        // Use configuration
    } else {
        std::cout << "Failed to load configuration from config.json" << std::endl;
        // Handle error
    }
} catch (const std::exception& e) {
    std::cout << "Error loading configuration: " << e.what() << std::endl;
}
```

### Saving Configuration

```python
import pynlm

# Create and populate configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("learning_rate", 0.01)
config.set("enable_curiosity", True)

# Save to JSON file
if config.saveToFile("output_config.json"):
    print("Configuration saved successfully")
else:
    print("Failed to save configuration")

# Save to file with error handling
try {
    config.saveToFile("output_config.json");
    std::cout << "Configuration saved to output_config.json" << std::endl;
} catch (const std::exception& e) {
    std::cout << "Error saving configuration: " << e.what() << std::endl;
}
```

### JSON Schema Generation

```python
import pynlm

// Create schema
auto schema = pynlm::Config::createBuilder()
    .add("neuron_count", pynlm::ConfigSchema("neuron_count", pynlm::ConfigSchema::Type::Integer, true))
        .withRange(100, 1000000)
        .withDefault(1000)
    .add("learning_rate", pynlm::ConfigSchema("learning_rate", pynlm::ConfigSchema::Type::Double, false))
        .withRange(0.0, 1.0)
        .withDefault(0.01)
    .add("enable_curiosity", pynlm::ConfigSchema("enable_curiosity", pynlm::ConfigSchema::Type::Boolean, false))
        .withDefault(true)
    .build();

// Initialize with schema
config.initializeWithSchema(schema);

// Generate JSON schema
std::string jsonSchema = config.generateJSONSchema();
std::cout << "JSON Schema:" << std::endl;
std::cout << jsonSchema << std::endl;
```

## Configuration Validation

### Basic Validation

```python
import pynlm

// Create schema
auto schema = pynlm::Config::createBuilder()
    .add("neuron_count", pynlm::ConfigSchema("neuron_count", pynlm::ConfigSchema::Type::Integer, true))
        .withRange(100, 1000000)
        .withDefault(1000)
    .add("learning_rate", pynlm::ConfigSchema("learning_rate", pynlm::ConfigSchema::Type::Double, false))
        .withRange(0.0, 1.0)
        .withDefault(0.01)
    .build();

config.initializeWithSchema(schema);

// Set valid values
config.setInt("neuron_count", 5000);
config.setDouble("learning_rate", 0.01);

// Check validation
if (config.validateAgainstSchema(schema)) {
    std::cout << "Configuration is valid" << std::endl;
} else {
    std::cout << "Configuration has errors: " << config.getValidationErrors() << std::endl;
}
```

### Custom Validators

```cpp
#include "core/Config/ConfigValidator.hpp"

// Create custom validator
class PositiveNumberValidator : public pynlm::IValidator {
public:
    bool validate(const std::any& value) const override {
        if (auto val = std::any_cast<double>(&value)) {
            return *val > 0.0;
        }
        return false;
    }
    
    std::string getErrorMessage(const std::any& value) const override {
        return "Value must be positive";
    }
};

// Create schema with custom validator
auto schema = pynlm::Config::createBuilder()
    .add("learning_rate", pynlm::ConfigSchema("learning_rate", pynlm::ConfigSchema::Type::Double, false))
        .withRange(0.0, 1.0)
        .withDefault(0.01)
        .addValidator(std::make_unique<PositiveNumberValidator>())
    .build();

config.initializeWithSchema(schema);
```

### Validation Error Handling

```python
import pynlm

// Create schema
auto schema = pynlm::Config::createBuilder()
    .add("port", pynlm::ConfigSchema("port", pynlm::ConfigSchema::Type::Integer, true))
        .withRange(1, 65535)
        .withDefault(8080)
    .add("host", pynlm::ConfigSchema("host", pynlm::ConfigSchema::Type::String, false))
        .withLengthRange(1, 255)
        .withDefault("localhost")
    .build();

config.initializeWithSchema(schema);

// Set configuration values
config.setInt("port", 8080);
config.setString("host", "localhost");

// Check validation
if (!config.validateAgainstSchema(schema)) {
    std::cout << "Configuration has validation errors:" << std::endl;
    std::vector<pynlm::ConfigError> errors = config.getValidationErrors();
    for (const auto& error : errors) {
        std::cout << "  - " << error.what() << std::endl;
    }
}
```

## Performance Optimization

### Memory-Efficient Configuration

```python
import pynlm

// Create memory-efficient configuration
auto config = pynlm::createDefaultConfig();

// Use smaller brain size for memory efficiency
config.setInt("neuron_count", 1000);
config.setDouble("learning_rate", 0.01);
config.setBool("enable_curiosity", false);

// Create brain
auto brain = pynlm::createBrain(config);
brain->initialize();
```

### Configuration-Based Optimization

```python
import pynlm

// Performance-optimized configuration
auto config = pynlm::createDefaultConfig();

// Optimize for real-time performance
config.setInt("neuron_count", 500);
config.setDouble("learning_rate", 0.1);
config.setBool("enable_curiosity", false);
config.setBool("enable_structural_plasticity", false);

// Create brain
auto brain = pynlm::createBrain(config);
brain->initialize();

// Process simulation
for (int step = 0; step < 10000; ++step) {
    brain->step(step);
}
```

## Configuration Migration

### From Legacy Config to New Config System

```python
// Legacy API (deprecated)
config.setValue("neuron_count", 1000);
config.getValue("neuron_count", &neuron_count);

// New API (recommended)
config.setInt("neuron_count", 1000);
auto neuron_count = config.getInt("neuron_count");
```

### Migration Strategy

1. **Identify Legacy Configuration**: Find all configuration files and code
2. **Update Configuration Files**: Convert to new format (JSON, YAML, etc.)
3. **Update Code**: Change configuration access patterns
4. **Test Migration**: Run tests to ensure compatibility
5. **Document Changes**: Document migration for future reference

## Testing Configuration

### Unit Testing

```python
import unittest
import pynlm

class TestConfiguration(unittest.TestCase):
    def setUp(self):
        self.config = pynlm.createDefaultConfig()
        self.config.set("neuron_count", 1000)
        self.config.set("learning_rate", 0.01)
    
    def testConfigCreation(self):
        self.assertIsNotNone(self.config)
    
    def testConfigSetGet(self):
        self.config.set("test_key", "test_value")
        self.assertEqual(self.config.get("test_key"), "test_value")
    
    def testConfigValidation(self):
        auto schema = pynlm::Config::createBuilder()
            .add("neuron_count", pynlm::ConfigSchema("neuron_count", pynlm::ConfigSchema::Type::Integer, true))
                .withRange(100, 1000000)
                .withDefault(1000)
            .build();
        
        self.config.initializeWithSchema(schema);
        self.assertTrue(self.config.validateAgainstSchema(schema));
    
    def testConfigFileIO(self):
        import tempfile
        import os
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            self.config.saveToFile(f.name);
            temp_file = f.name;
        
        auto new_config = pynlm::createDefaultConfig();
        self.assertTrue(new_config.loadFromFile(temp_file));
        self.assertEqual(new_config.getInt("neuron_count"), 1000);
        
        os.unlink(temp_file);
```

## Configuration Best Practices

### 1. Use Schema Validation
```python
// Always use schema validation for production configurations
auto schema = pynlm::Config::createBuilder()
    .add("neuron_count", pynlm::ConfigSchema("neuron_count", pynlm::ConfigSchema::Type::Integer, true))
        .withRange(100, 1000000)
    .add("learning_rate", pynlm::ConfigSchema("learning_rate", pynlm::ConfigSchema::Type::Double, false))
        .withRange(0.0, 1.0)
    .build();

config.initializeWithSchema(schema);
```

### 2. Provide Sensible Defaults
```python
// Always provide sensible defaults for optional configuration
auto schema = pynlm::Config::createBuilder()
    .add("neuron_count", pynlm::ConfigSchema("neuron_count", pynlm::ConfigSchema::Type::Integer, true))
        .withRange(100, 1000000)
        .withDefault(1000)
    .add("learning_rate", pynlm::ConfigSchema("learning_rate", pynlm::ConfigSchema::Type::Double, false))
        .withRange(0.0, 1.0)
        .withDefault(0.01)
    .add("enable_curiosity", pynlm::ConfigSchema("enable_curiosity", pynlm::ConfigSchema::Type::Boolean, false))
        .withDefault(true)
    .build();
```

### 3. Use Factory Presets
```python
// Use factory presets for common configurations
auto nn_schema = pynlm::ConfigFactory::createNeuralNetworkSchema();
auto sim_schema = pynlm::ConfigFactory::createSimulationSchema();
auto logging_schema = pynlm::ConfigFactory::createLoggingSchema();
```

### 4. Validate Configuration
```python
// Always validate configuration before use
if (config.validateAgainstSchema(schema)) {
    // Configuration is valid, use it
} else {
    // Configuration has errors, fix them
    auto errors = config.getValidationErrors();
    // Handle errors
}
```

### 5. Document Configuration
```python
// Document configuration for users
auto schema = pynlm::Config::createBuilder()
    .add("neuron_count", pynlm::ConfigSchema("neuron_count", pynlm::ConfigSchema::Type::Integer, true))
        .withRange(100, 1000000)
        .withDefault(1000)
        .withDescription("Number of neurons in the brain")
    .add("learning_rate", pynlm::ConfigSchema("learning_rate", pynlm::ConfigSchema::Type::Double, false))
        .withRange(0.0, 1.0)
        .withDefault(0.01)
        .withDescription("Learning rate for neural network")
    .build();
```

## Configuration Examples

### Example 1: Basic Configuration

```python
import pynlm

// Create basic configuration
auto config = pynlm::createDefaultConfig();
config.setInt("neuron_count", 1000);
config.setDouble("learning_rate", 0.01);
config.setBool("enable_curiosity", true);

// Create brain
auto brain = pynlm::createBrain(config);
brain->initialize();
```

### Example 2: Advanced Configuration

```python
import pynlm

// Create schema
auto schema = pynlm::Config::createBuilder()
    .add("neuron_count", pynlm::ConfigSchema("neuron_count", pynlm::ConfigSchema::Type::Integer, true))
        .withRange(100, 1000000)
        .withDefault(1000)
        .withDescription("Number of neurons in the brain")
    .add("learning_rate", pynlm::ConfigSchema("learning_rate", pynlm::ConfigSchema::Type::Double, false))
        .withRange(0.0, 1.0)
        .withDefault(0.01)
        .withDescription("Learning rate for neural network")
    .add("synapse_density", pynlm::ConfigSchema("synapse_density", pynlm::ConfigSchema::Type::Double, false))
        .withRange(0.01, 0.5)
        .withDefault(0.1)
        .withDescription("Synapse connectivity density")
    .build();

// Initialize configuration
config.initializeWithSchema(schema);

// Create brain
auto brain = pynlm::createBrain(config);
brain->initialize();
```

### Example 3: Factory Preset Configuration

```python
import pynlm

// Create neural network configuration preset
auto nn_schema = pynlm::ConfigFactory::createNeuralNetworkSchema();
auto sim_schema = pynlm::ConfigFactory::createSimulationSchema();
auto logging_schema = pynlm::ConfigFactory::createLoggingSchema();

// Create brains with factory presets
auto nn_config = pynlm::createDefaultConfig();
nn_config.initializeWithSchema(nn_schema);
auto nn_brain = pynlm::createBrain(nn_config);

auto sim_config = pynlm::createDefaultConfig();
sim_config.initializeWithSchema(sim_schema);
auto sim_brain = pynlm::createBrain(sim_config);

auto logging_config = pynlm::createDefaultConfig();
logging_config.initializeWithSchema(logging_schema);
auto logging_brain = pynlm::createBrain(logging_config);
```

## Conclusion

This tutorial has covered advanced NLM configuration features, including:

1. **Schema-Based Configuration**: Creating schemas with validation rules
2. **Factory Presets**: Using predefined configuration presets
3. **Configuration File I/O**: Loading and saving configuration files
4. **Validation**: Comprehensive configuration validation
5. **Custom Validators**: Creating custom validation rules
6. **Error Handling**: Detailed error reporting and handling
7. **Performance Optimization**: Memory-efficient configuration
8. **Testing**: Unit testing and integration testing

By following these advanced configuration practices, you can:

- Create robust, type-safe configurations
- Prevent runtime configuration errors
- Optimize performance through configuration
- Maintain backward compatibility
- Improve developer experience

Next steps:
- **Tutorial 3**: Learn about batch processing and performance optimization
- **Tutorial 4**: Learn about performance profiling and analysis
- **Tutorial 5**: Learn advanced Python features and integration
- **Tutorial 6**: Learn real-world application development

The NLM configuration system provides a powerful foundation for building robust, efficient, and maintainable NLM applications. By leveraging these advanced features, you can create high-performance neural simulations with ease and confidence.
