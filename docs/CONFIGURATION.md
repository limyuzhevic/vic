# NLM - Configuration System Documentation

## Overview

This document describes the improved configuration system for NLM (Neural Learning Machine), supporting both JSON and key=value format configurations.

## Configuration Formats

### JSON Configuration

JSON format provides structured, type-safe configuration with rich validation capabilities.

**Example JSON Configuration (`config.json`):**

```json
{
  "brain": {
    "neuron": {
      "count": 10000,
      "simulation": {
        "dt": 0.001,
        "maxTime": 600.0
      }
    }
  },
  "memory": {
    "working": {
      "capacity": 100
    },
    "episodic": {
      "capacity": 1000
    }
  },
  "neuromodulation": {
    "dopamine": {
      "level": 0.5
    },
    "curiosity": {
      "threshold": 0.3
    }
  },
  "development": {
    "stage": {
      "duration": 300.0
    }
  },
  "environment": {
    "world": {
      "size": 50.0
    },
    "vision": {
      "fov": 90.0
    }
  },
  "motor": {
    "actions": {
      "count": 6
    }
  }
}
```

### Key=Value Configuration

Key=value format provides a simple, human-readable configuration format.

**Example Key=Value Configuration (`config.cfg`):**

```
# NLM Configuration File
# Brain settings
brain.neuron.count = 10000
brain.simulation.dt = 0.001
brain.simulation.maxTime = 600.0

# Memory settings
memory.working.capacity = 100
memory.episodic.capacity = 1000

# Neuromodulation settings
neuromodulation.dopamine.level = 0.5
neuromodulation.curiosity.threshold = 0.3

# Development settings
development.stage.duration = 300.0

# Environment settings
environment.world.size = 50.0
environment.vision.fov = 90.0

# Motor settings
motor.actions.count = 6
```

## Configuration Helper API

The `ConfigHelper` class provides a unified interface for working with configuration files.

### Class Definition

```cpp
class ConfigHelper {
public:
    // Configuration source detection
    enum class SourceType {
        Unknown,
        KeyValue,
        JSON
    };
    
    ConfigHelper();
    ~ConfigHelper();
    
    // Disable copying, enable moving
    ConfigHelper(const ConfigHelper&) = delete;
    ConfigHelper& operator=(const ConfigHelper&) = delete;
    ConfigHelper(ConfigHelper&&) noexcept;
    ConfigHelper& operator=(ConfigHelper&&) noexcept;
    
    // Load configuration from file (auto-detect format)
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from string (auto-detect format)
    bool loadFromString(const std::string& content);
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get configuration value
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Set configuration value
    template<typename T>
    bool set(const std::string& key, const T& value);
    
    // Check if key exists
    bool has(const std::string& key) const;
    
    // Get all configuration keys
    std::vector<std::string> getKeys() const;
    
    // Get configuration source type
    SourceType getSourceType() const;
    
    // Get configuration file path
    std::string getFilepath() const;
    
    // Get configuration summary
    std::string getSummary() const;
    
    // Clear configuration
    void clear();
    
    // Convert configuration to JSON string
    std::string toJSON(bool pretty = false) const;
    
    // Validate configuration schema
    bool validateSchema(const std::string& schemaFile) const;
    
    // Merge with another configuration
    bool merge(const ConfigHelper& other);
    
    // Create default configuration
    static ConfigHelper createDefaultConfig();
    
    // Create example configuration
    static std::string createExampleConfig();
};
```

### Usage Examples

#### Loading from File

```cpp
#include "Config/ConfigHelper.hpp"
#include <iostream>

int main() {
    nlm::ConfigHelper config;
    
    // Load from JSON file
    if (config.loadFromFile("config.json")) {
        std::cout << "Loaded JSON configuration successfully" << std::endl;
    }
    
    // Or load from key=value file
    if (config.loadFromFile("config.cfg")) {
        std::cout << "Loaded key=value configuration successfully" << std::endl;
    }
    
    return 0;
}
```

#### Accessing Values

```cpp
// Get integer value
auto neuronCount = config.get<int>("brain.neuron.count");
if (neuronCount) {
    std::cout << "Neuron count: " << *neuronCount << std::endl;
}

// Get double value
auto dt = config.get<double>("brain.simulation.dt");
if (dt) {
    std::cout << "Simulation timestep: " << *dt << std::endl;
}

// Get boolean value
auto debugMode = config.get<bool>("debug.enabled");
if (debugMode) {
    std::cout << "Debug mode: " << (*debugMode ? "enabled" : "disabled") << std::endl;
}

// Get string value
auto configPath = config.get<std::string>("paths.config");
if (configPath) {
    std::cout << "Config path: " << *configPath << std::endl;
}
```

#### Setting Values

```cpp
config.set("brain.neuron.count", 15000);
config.set("brain.simulation.dt", 0.0005);
config.set("debug.enabled", true);
config.set("paths.config", "/custom/path/config.json");
```

#### Saving Configuration

```cpp
// Save as JSON
if (config.saveToFile("new_config.json")) {
    std::cout << "Configuration saved as JSON" << std::endl;
}

// Save as key=value
if (config.saveToFile("new_config.cfg")) {
    std::cout << "Configuration saved as key=value" << std::endl;
}
```

#### Working with Default Configuration

```cpp
// Create default configuration
nlm::ConfigHelper defaultConfig = nlm::ConfigHelper::createDefaultConfig();

// Get summary
defaultConfig.getSummary();

// Convert to JSON
std::string jsonOutput = defaultConfig.toJSON(true); // pretty print
```

## Configuration File Examples

### Example 1: Production Configuration

```json
{
  "nlm": {
    "version": "6.0.0",
    "brain": {
      "neuron": {
        "count": 50000,
        "plasticity": {
          "initialRate": 1.0,
          "decayRate": 0.8
        }
      },
      "simulation": {
        "dt": 0.001,
        "maxTime": 3600.0
      }
    },
    "memory": {
      "working": {
        "capacity": 500,
        "replay": true
      },
      "episodic": {
        "capacity": 5000,
        "consolidation": "aggressive"
      }
    },
    "neuromodulation": {
      "dopamine": {
        "baseline": 0.1,
        "burstSensitivity": 2.0
      },
      "curiosity": {
        "threshold": 0.2,
        "decay": 0.9
      }
    },
    "environment": {
      "world": {
        "width": 100,
        "height": 100,
        "obstacleDensity": 0.1
      }
    }
  }
}
```

### Example 2: Development Configuration

```cfg
# Development Configuration
# Brain settings
brain.neuron.count = 1000
brain.simulation.dt = 0.01
brain.simulation.maxTime = 300.0

# Memory settings
memory.working.capacity = 50
memory.episodic.capacity = 200

# Neuromodulation settings
neuromodulation.dopamine.level = 0.8
neuromodulation.curiosity.threshold = 0.5

# Environment settings
environment.world.size = 20.0
environment.vision.fov = 120.0
environment.vision.resolution = "low"

# Debug settings
debug.enabled = true
debug.verbose = true
debug.logLevel = "debug"

# Performance settings
performance.enableProfiling = true
performance.maxMemoryUsage = "2GB"
```

## Configuration Key Reference

### Brain Configuration Keys

| Key | Type | Description |
|-----|------|-------------|
| `brain.neuron.count` | int | Number of neurons in the brain |
| `brain.simulation.dt` | double | Simulation timestep (seconds) |
| `brain.simulation.maxTime` | double | Maximum simulation time (seconds) |
| `brain.plasticity.initialRate` | double | Initial plasticity rate |
| `brain.plasticity.decayRate` | double | Plasticity decay rate |

### Memory Configuration Keys

| Key | Type | Description |
|-----|------|-------------|
| `memory.working.capacity` | int | Working memory capacity |
| `memory.working.replay` | bool | Enable working memory replay |
| `memory.episodic.capacity` | int | Episodic memory capacity |
| `memory.episodic.consolidation` | string | Consolidation strategy |

### Neuromodulation Configuration Keys

| Key | Type | Description |
|-----|------|-------------|
| `neuromodulation.dopamine.level` | double | Dopamine baseline level |
| `neuromodulation.dopamine.burstSensitivity` | double | Dopamine burst sensitivity |
| `neuromodulation.curiosity.threshold` | double | Curiosity activation threshold |
| `neuromodulation.curiosity.decay` | double | Curiosity decay rate |

### Environment Configuration Keys

| Key | Type | Description |
|-----|------|-------------|
| `environment.world.size` | double | World size (assuming square) |
| `environment.world.width` | double | World width |
| `environment.world.height` | double | World height |
| `environment.world.obstacleDensity` | double | Obstacle density |
| `environment.vision.fov` | double | Vision field of view (degrees) |
| `environment.vision.resolution` | string | Vision resolution |

### Development Configuration Keys

| Key | Type | Description |
|-----|------|-------------|
| `debug.enabled` | bool | Enable debug mode |
| `debug.verbose` | bool | Enable verbose output |
| `debug.logLevel` | string | Logging level |

### Performance Configuration Keys

| Key | Type | Description |
|-----|------|-------------|
| `performance.enableProfiling` | bool | Enable performance profiling |
| `performance.maxMemoryUsage` | string | Maximum memory usage |

## Error Handling

The configuration system includes comprehensive error handling with the following error types:

### Configuration Errors

- `ConfigFileNotFound`: Configuration file not found
- `ConfigParseError`: Failed to parse configuration file
- `ConfigInvalidValue`: Invalid configuration value

### Usage Examples

```cpp
#include "Config/ConfigHelper.hpp"
#include "ErrorHandling/ErrorHandling.hpp"

int main() {
    nlm::ConfigHelper config;
    
    try {
        if (!config.loadFromFile("nonexistent.json")) {
            NLM_THROW_ERROR(NLMErrors::ConfigFileNotFound, "Cannot load configuration file");
        }
    } catch (const nlm::NLMException& e) {
        std::cerr << "Configuration error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

## Integration with Other Components

### Integration with Neuromodulation

The configuration system integrates seamlessly with neuromodulation components:

```cpp
// Configure neuromodulation parameters
config.set("neuromodulation.dopamine.level", 0.7);
config.set("neuromodulation.curiosity.threshold", 0.3);

// Create neuromodulators with configuration
auto dopamine = std::make_unique<nlm::Dopamine>();
dopamine->setLevel(config.get<double>("neuromodulation.dopamine.level").value_or(0.5));
```

### Integration with Memory Systems

```cpp
// Configure memory systems
config.set("memory.working.capacity", 500);
config.set("memory.episodic.capacity", 5000);

// Initialize memory systems with configuration
auto workingMemory = std::make_unique<nlm::WorkingMemory>(config.get<int>("memory.working.capacity").value_or(100));
auto episodicMemory = std::make_unique<nlm::EpisodicMemory>(config.get<int>("memory.episodic.capacity").value_or(1000));
```

### Integration with Development System

```cpp
// Configure development system
config.set("development.stage.duration", 300.0);

// Initialize development system
auto developmentSystem = std::make_unique<nlm::DevelopmentSystem>(config.get<double>("development.stage.duration").value_or(300.0));
```

## Performance Considerations

### Configuration Loading Performance

- **JSON files**: Slower to parse but support validation and schema checking
- **Key=value files**: Faster to parse, lightweight and human-readable
- **Auto-detection**: Automatically detects file format based on content

### Memory Usage

- **JSON parsing**: Higher memory usage due to string parsing and validation
- **Key=value parsing**: Lower memory usage, direct string parsing
- **Caching**: Configuration files can be cached for repeated access

### Best Practices

1. **Use key=value format for production** when performance is critical
2. **Use JSON format for development** when validation and schema checking are needed
3. **Cache frequently accessed configurations** to improve performance
4. **Validate configuration files** before loading to catch errors early
5. **Use environment variables** for overriding configuration values

## Migration Guide

### From Previous Version (Phase 5)

The configuration system has been completely rewritten from Phase 5 to provide:

1. **Enhanced format support**: Added JSON format alongside key=value format
2. **Better error handling**: Comprehensive error handling with specific error types
3. **Improved validation**: Schema validation and value validation
4. **Enhanced API**: More intuitive and powerful API methods
5. **Integration with other systems**: Better integration with neuromodulation, memory, and development systems

### Migration Steps

1. **Update configuration file format** from key=value to JSON if needed
2. **Update configuration loading code** to use `ConfigHelper`
3. **Update configuration access** to use the new API methods
4. **Add error handling** around configuration loading
5. **Test configuration loading** with new error handling

### Example Migration

**Before (Phase 5):**

```cpp
// Load configuration
Config config;
config.loadFromFile("config.cfg");

// Access values
int neuronCount = config.getOr<int>("brain.neuron.count", 10000);
double dt = config.getOr<double>("brain.simulation.dt", 0.001);
```

**After (Phase 6):**

```cpp
// Load configuration
ConfigHelper config;
if (!config.loadFromFile("config.json")) {
    // Handle error
}

// Access values
auto neuronCount = config.get<int>("brain.neuron.count");
if (neuronCount) {
    // Use value
}

auto dt = config.get<double>"brain.simulation.dt";
if (dt) {
    // Use value
}
```

## Configuration Validation

### Schema Validation

The configuration system supports JSON schema validation to ensure configuration files conform to expected structure:

```cpp
bool validateSchema(const std::string& schemaFile) const;
```

### Value Validation

- **Type checking**: Ensures values are of the correct type
- **Range validation**: Ensures values are within acceptable ranges
- **Required key checking**: Ensures required configuration keys are present
- **Format validation**: Ensures values follow expected formats

## Future Enhancements

### Planned Features

1. **YAML support**: Add support for YAML configuration files
2. **Environment variables**: Support for environment variable overrides
3. **Command-line integration**: Direct command-line configuration
4. **Hot reload**: Support for reloading configuration files at runtime
5. **Distributed configuration**: Support for distributed configuration management
6. **Configuration history**: Track changes to configuration over time

### Performance Optimizations

1. **Parallel loading**: Load multiple configuration files in parallel
2. **Lazy loading**: Load configuration values only when needed
3. **Compression**: Support for compressed configuration files
4. **Caching**: Advanced caching strategies for frequently accessed values

## Conclusion

The improved configuration system in NLM Phase 6 provides:

- **Flexible format support**: JSON and key=value formats with auto-detection
- **Comprehensive error handling**: Specific error types and detailed error messages
- **Powerful API**: Intuitive methods for configuration manipulation
- **Integration with other systems**: Seamless integration with brain, memory, neuromodulation, and development systems
- **Performance optimization**: Efficient parsing and loading strategies
- **Extensibility**: Easy to extend with new features and validation rules

This configuration system provides a solid foundation for configuring and running NLM simulations with improved flexibility, error handling, and user experience.
