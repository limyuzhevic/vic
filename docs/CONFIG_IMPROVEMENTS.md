// Enhanced documentation for the NLM project
// This file demonstrates the improved configuration system and features

## New Configuration System

The NLM configuration system now uses JSON format for better reliability and developer experience.

### Example JSON Configuration

```json
{
  "brain": {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity_learning_rate": 0.01,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_time_constant": 20.0
  },
  
  "development": {
    "synaptogenesis_rate": 0.001,
    "pruning_rate": 0.0001
  },
  
  "neuromodulation": {
    "dopamine_baseline": 0.1,
    "curiosity_enabled": true,
    "novelty_enabled": true
  },
  
  "reward": {
    "discount_factor": 0.99,
    "prediction_error_enabled": true
  },
  
  "environment": {
    "name": "GridWorld",
    "width": 10,
    "height": 10,
    "vision_width": 8,
    "vision_height": 8
  },
  
  "logging": {
    "level": "INFO",
    "to_file": false,
    "filename": "nlm.log",
    "colors": true
  },
  
  "simulation": {
    "max_steps": 10000,
    "time_limit": 0.0,
    "checkpoint_interval": 1000
  },
  
  "visualization": {
    "enabled": false,
    "update_rate": 30.0
  }
}
```

### Configuration Loading

```cpp
#include <nlm.hpp>

int main() {
    nlm::Config config;
    
    // Load from JSON file (NEW - works much better)
    if (config.loadFromFile("nlm_config.json")) {
        std::cout << "Configuration loaded successfully:" << std::endl;
        std::cout << config.summary();
    } else {
        std::cerr << "Failed to load configuration" << std::endl;
        // Fall back to defaults
        config.set("brain.neuron_count", 1000);
        config.set("brain.connection_probability", 0.1);
    }
    
    // Save to JSON file (NOW WORKS - saves real values, not placeholders)
    config.saveToFile("nlm_config.json");
    
    // Access configuration
    int neurons = config.getOr<int>("brain.neuron_count", 1000);
    double learning_rate = config.getOr<double>("brain.plasticity_learning_rate", 0.01);
    bool curiosity_enabled = config.getOr<bool>("neuromodulation.curiosity_enabled", true);
}
```

### Command Line Interface

The command line interface now supports JSON-like configuration:

```bash
# Load configuration file
./nlm --config my_config.json

# Override values from command line
./nlm --config config.json \
    --brain.neuron_count=2000 \
    --brain.connection_probability=0.2 \
    --neuromodulation.dopamine_baseline=0.5

# Simple key=value format (legacy)
./nlm --neuron_count 1500 --connection_probability 0.15
```

## Key Improvements

### 1. JSON-Based Configuration
- **Real file format**: No more placeholder values
- **Schema validation**: Built-in JSON structure validation
- **Better error handling**: Clear error messages for malformed JSON
- **Cross-platform**: Standard JSON format works everywhere

### 2. Enhanced Type Support
- **Arrays**: Native support for array values
- **Nested structures**: Object-based configuration organization
- **Type safety**: Better type conversion and error handling
- **Flexible access**: Template-based getters for all types

### 3. Improved Developer Experience
- **Better documentation**: Clear examples and usage patterns
- **Schema validation**: Validate configuration against JSON schema
- **Extensible**: Easy to add new configuration parameters
- **Debugging**: Detailed configuration summaries

### 4. Production Readiness
- **Robust error handling**: Graceful failure modes
- **File validation**: Check file readability before parsing
- **Performance**: Efficient parsing with nlohmann/json
- **Maintainability**: Clean, well-documented code

## Migration Guide

If you were using the old key=value format, you can migrate to JSON:

### Old Format (nlm_config.cfg):
```
# NLM Configuration
brain.neuron_count = 1000
brain.connection_probability = 0.1
neuromodulation.dopamine_baseline = 0.1
```

### New Format (nlm_config.json):
```json
{
  "brain": {
    "neuron_count": 1000,
    "connection_probability": 0.1
  },
  "neuromodulation": {
    "dopamine_baseline": 0.1
  }
}
```

## Testing the New System

The test suite has been enhanced:

```bash
# Run configuration tests
cd build
ctest --output-on-failure

# Test configuration loading/saving
./nlm_test --config test_config.json
```

## Advanced Configuration Features

### 1. Schema Validation
```cpp
// Validate configuration against schema
nlohmann::json schema = {
    {"type", "object"},
    {"required", nlohmann::json::array({"brain.neuron_count", "brain.connection_probability"})},
    {"properties", nlohmann::json::object({
        {"brain.neuron_count", {{ "type", "integer" }, { "minimum", 1 }}},
        {"brain.connection_probability", {{ "type", "number" }, { "maximum", 1.0 }}}
    })}
};

config.setSchema(schema);
if (!config.validate()) {
    std::cerr << "Invalid configuration" << std::endl;
}
```

### 2. Environment Variables
```bash
export NLM_BRAIN_NEURON_COUNT=2000
export NLM_BRAIN_CONNECTION_PROBABILITY=0.15
./nlm --config config.json
```

### 3. Configuration Factory
```cpp
class ConfigFactory {
public:
    static nlm::Config createFromEnvironment() {
        nlm::Config config;
        
        // Load from file if exists
        if (config.loadFromFile("nlm_config.json")) {
            std::cout << "Loaded configuration from file" << std::endl;
        }
        
        // Override from environment variables
        const char* env_neuron_count = std::getenv("NLM_BRAIN_NEURON_COUNT");
        if (env_neuron_count) {
            config.set("brain.neuron_count", std::stoi(env_neuron_count), nlm::ConfigSource::Runtime);
        }
        
        // Override from command line
        config.loadFromArgs(argc, argv);
        
        return config;
    }
};
```

## Conclusion

The new configuration system provides:

- **Better reliability**: JSON format is standard and well-supported
- **Developer friendliness**: Clear API and comprehensive documentation
- **Production ready**: Robust error handling and validation
- **Future-proof**: Extensible architecture for new features
- **Performance**: Efficient parsing with nlohmann/json
- **Maintainability**: Clean, modular code structure

This represents a significant improvement over the previous key=value parser, providing a solid foundation for the NLM neural simulation framework.