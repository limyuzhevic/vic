# Enhanced Configuration System for NLM

## Overview

The NLM Configuration System has been enhanced from a basic key-value storage system to a robust, self-documenting configuration framework with comprehensive validation, parameterization, and template support. This document outlines the new features and provides examples of usage.

## Key Enhancements

### 1. Parameter Validation
- **Type checking**: Validates that configuration values match expected types (int, double, bool, string, vectors)
- **Range validation**: Ensures values stay within sensible ranges
- **Custom validation**: Framework supports adding custom validation rules for specific parameters
- **Error reporting**: Clear error messages indicating what went wrong

### 2. Better Defaults
- **Sensible defaults**: All configuration parameters have sensible default values
- **Template configurations**: Pre-defined templates for common use cases
- **Auto-completion**: Get valid parameter names for a given group

### 3. Configuration Schema
- **Structured metadata**: Each parameter has type, range, description, and group information
- **Parameter groups**: Organized into logical groups (brain, plasticity, neuromodulation, development)
- **Self-documenting**: Configuration system documents itself through schema

### 4. Parameter Groups
The enhanced configuration system organizes parameters into logical groups:

#### Brain Group
- `brain.region_count`: Number of brain regions
- `brain.neurons_per_region`: Average neurons per region
- `brain.inhibitory_ratio`: Ratio of inhibitory neurons (0-1)
- `brain.excitatory_strength`: Default excitatory synapse strength

#### Plasticity Group
- `plasticity.stdp.learning_rate`: STDP learning rate
- `plasticity.stdp.time_window`: STDP time window in ms
- `plasticity.hebbian.rate`: Hebbian plasticity rate

#### Neuromodulation Group
- `neuromodulation.reward.modality`: Reward signal type
- `neuromodulation.reward.strength`: Reward modulation strength
- `neuromodulation.curiosity.epsilon`: Curiosity exploration parameter

#### Development Group
- `development.pruning.threshold`: Activity threshold for pruning
- `development.maturation.rate`: Neural maturation rate
- `development.synaptogenesis.initial_density`: Initial synapse density

### 5. Validation Methods
- `validate()`: Validate entire configuration
- `validate(key)`: Validate specific parameter
- `isValid(key)`: Quick check for parameter validity
- `getValidationErrors(key)`: Get error messages
- `getValidationWarnings(key)`: Get warning messages

### 6. Auto-completion
- `getGroups()`: Get all available parameter groups
- `getParametersInGroup(group)`: Get parameters in a specific group
- `getParameterDescription(key)`: Get detailed parameter information

### 7. Parameter Ranges
All parameters have defined ranges:
- **Numeric parameters**: Minimum and maximum values
- **Discrete parameters**: Allowed integer options
- **String parameters**: Valid options (e.g., reward modalities)

### 8. Configuration Templates
Predefined templates for common use cases:

#### Simple Brain (`simple_brain`)
Basic configuration suitable for basic neural network simulations
- 4 brain regions
- 100 neurons per region
- Standard plasticity and neuromodulation settings

#### Agent Learning (`agent_learning`)
Configuration optimized for reinforcement learning agents
- 6 brain regions for more complex behavior
- Higher neuromodulation levels
- Enhanced plasticity for learning

#### Research (`research`)
Comprehensive configuration for research and experimentation
- Maximum configurability
- All neuromodulation systems enabled
- Detailed development settings

#### Production (`production`)
Optimized for production use
- Streamlined configuration
- Performance optimizations
- Logging and monitoring enabled

## API Reference

### C++ API

#### Configuration Class
```cpp
nlm::Config config;

// Set values with validation
config.set("brain.region_count", 4);
config.set("plasticity.stdp.learning_rate", 0.01);

// Get values
int regions = config.getOr<int>("brain.region_count", 4);
double learningRate = config.getOr<double>("plasticity.stdp.learning_rate", 0.01);

// Validation
if (!config.isValid("brain.region_count")) {
    std::string errors = config.getValidationErrors("brain.region_count");
    // Handle error...
}

// Groups and Schema
auto groups = config.getGroups();
auto brainParams = config.getParametersInGroup("brain");
std::string desc = config.getParameterDescription("brain.region_count");

// Templates
config.loadFromTemplate("simple_brain");
auto templates = nlm::Config::getAvailableTemplates();
```

#### Schema Class
```cpp
nlm::ConfigSchema schema;

// Add parameter with metadata
nlm::ParameterMetadata param(
    "brain.region_count",
    "Region Count",
    "Number of brain regions in the simulation",
    nlm::ParamType::Int,
    4,  // default value
    "brain",  // group
    false,  // required
    1      // order
);

schema.addParameter(param);

// Validate parameter
auto result = schema.validateParameter("brain.region_count", std::any(4));
if (!result.isValid) {
    std::string errors = result.getErrors();
    // Handle error...
}
```

### Python API

The enhanced configuration system is fully exposed to Python:

```python
import pynlm

# Create configuration
config = pynlm.Config()

# Set values with validation
config.set("brain.region_count", 4)
config.set("plasticity.stdp.learning_rate", 0.01)

# Get values
regions = config.getInt("brain.region_count", 4)
learning_rate = config.getDouble("plasticity.stdp.learning_rate", 0.01)

# Validation
if not config.isValid("brain.region_count"):
    errors = config.getValidationErrors("brain.region_count")
    # Handle error...

# Groups and Schema
groups = config.getGroups()
brain_params = config.getParametersInGroup("brain")
desc = config.getParameterDescription("brain.region_count")

# Templates
config.loadFromTemplate("simple_brain")
templates = pynlm.Config.getAvailableTemplates()

# Configuration summary
print(config.summary())
```

## Example Usage Patterns

### 1. Basic Usage
```cpp
#include "core/Config/Config.hpp"

void setupConfiguration(nlm::Config& config) {
    // Use template for basic setup
    config.loadFromTemplate("simple_brain");
    
    // Override specific values
    config.set("brain.inhibitory_ratio", 0.3);
    config.set("plasticity.stdp.learning_rate", 0.02);
    
    // Validate everything
    auto validation = config.validate();
    if (!validation.isValid) {
        std::cerr << "Configuration errors: " << validation.getErrors() << std::endl;
    }
}
```

### 2. Advanced Usage with Schema
```cpp
#include "core/Config/ConfigEnhanced.hpp"

void createCustomConfiguration() {
    nlm::Config config;
    
    // Create custom schema
    nlm::ConfigSchema schema;
    
    // Add custom parameter
    nlm::ParameterMetadata customParam(
        "simulation.max_steps",
        "Max Steps",
        "Maximum number of simulation steps",
        nlm::ParamType::Int,
        1000,
        "simulation",
        false,
        1
    );
    customParam.range = nlm::ParameterRange(nlm::ParamType::Int, 100, 10000);
    schema.addParameter(customParam);
    
    // Apply schema to config
    config.setSchema(std::make_shared<nlm::ConfigSchema>(schema));
    
    // Set values
    config.set("simulation.max_steps", 5000);
}
```

### 3. Python Usage
```python
import pynlm

# Create configuration with template
config = pynlm.Config()
config.loadFromTemplate("agent_learning")

# Modify values
config.set("brain.region_count", 8)
config.set("neuromodulation.reward.strength", 2.0)

# Validate
if config.isValid("brain.region_count"):
    print("Valid configuration")

# Save to file
config.saveToFile("agent_config.json")

# Load and validate from file
config2 = pynlm.Config()
config2.loadFromFile("agent_config.json")
validation = config2.validate()
if validation.isValid:
    print("Loaded configuration is valid")
else:
    print("Validation errors:", validation.getErrors())
```

### 4. Configuration Templates Example
```cpp
void setupResearchConfiguration(nlm::Config& config) {
    // Start with research template for maximum flexibility
    config.loadFromTemplate("research");
    
    // Adjust specific parameters for research needs
    config.set("neuromodulation.novelty.sensitivity", 2.0);
    config.set("development.maturation.rate", 0.2);
    
    // Add custom parameters
    config.set("research.exploration_rate", 0.5);
    config.set("research.adaptation_window", 100);
}
```

## Migration Guide

### From Old to New Configuration System

The enhanced configuration system maintains backward compatibility while adding new features:

#### Old Configuration (Basic Key-Value)
```cpp
// Old way - basic key-value storage
config.set("param1", "value1", nlm::ConfigSource::Runtime);
config.set("param2", 42, nlm::ConfigSource::Runtime);
config.set("param3", true, nlm::ConfigSource::Runtime);

// Get values
std::string val1 = config.getOr<std::string>("param1", "default");
int val2 = config.getOr<int>("param2", 0);
bool val3 = config.getOr<bool>("param3", false);
```

#### New Configuration (Enhanced with Validation)
```cpp
// New way - with validation and schema
config.set("param1", "value1");  // Automatically validates
config.set("param2", 42);        // Automatically validates
config.set("param3", true);      // Automatically validates

// Same get methods work as before
std::string val1 = config.getOr<std::string>("param1", "default");
int val2 = config.getOr<int>("param2", 0);
bool val3 = config.getOr<bool>("param3", false);

// Plus new features
if (config.isValid("param1")) {
    // Parameter is valid
}

auto schema = config.getSchema();
auto groups = config.getGroups();
```

## Benefits

1. **Self-documenting**: Configuration parameters come with descriptions and validation
2. **Type safety**: Prevents type mismatches at configuration time
3. **Range checking**: Prevents invalid parameter values
4. **Template support**: Quick setup for common use cases
5. **Group organization**: Logical organization of related parameters
6. **Validation feedback**: Clear error messages for invalid configurations
7. **Backward compatibility**: Existing code continues to work
8. **Python integration**: Full Python API for configuration

## Performance Considerations

- Schema validation adds minimal overhead
- Templates provide fast initial setup
- Parameter groups reduce search time
- Validation can be skipped for performance-critical sections

## Future Enhancements

The configuration system is designed for extensibility:
- Custom validation functions
- Dynamic parameter addition
- Configuration inheritance
- Version control support
- Hot configuration reloading

## Conclusion

The enhanced configuration system transforms the basic NLM configuration from a simple key-value store into a comprehensive configuration management framework that supports validation, documentation, and organization. The system is backward compatible, well-documented, and provides both C++ and Python APIs for maximum flexibility.
