# NLM Configuration System Documentation
# =====================================

This document provides comprehensive information about the NLM configuration system, covering all configuration features, formats, and usage examples.

## Overview

The NLM configuration system provides flexible, multi-source configuration management with support for JSON, YAML, and legacy key=value formats. It includes validation, save/load functionality, and comprehensive error handling.

### Key Features

- **Multi-Format Support**: JSON, YAML, and legacy key=value formats
- **Multiple Sources**: Configuration from files, command line, and runtime
- **Type Safety**: Strong type checking and validation
- **Error Handling**: Comprehensive error reporting and recovery
- **Save/Load**: Persistent configuration management
- **Validation**: Schema and constraint validation
- **Diffing**: Configuration difference analysis

## Configuration Sources

### 1. Configuration File
**Format:** JSON, YAML, or legacy key=value

**Default Location:** `configs/default.cfg`

**Example (JSON):**
```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "log_level": "INFO"
}
```

**Example (YAML):**
```yaml
random_seed: 42
simulation_timestep: 0.001
neuron_count: 1000
region_count: 2
connection_probability: 0.15
stdp_ltp_weight: 0.02
stdp_ltd_weight: 0.015
log_level: INFO
```

**Example (Legacy):**
```ini
# configs/default.cfg
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
log_level = INFO
```

### 2. Command Line Arguments
**Format:** `--key value` or `--key=value`

**Examples:**
```bash
--random_seed=123
--neuron_count=2000
--connection_probability=0.2
--log_level=DEBUG
--stdp_ltp_weight=0.05
--duration=30.0
--steps=50000
```

### 3. Runtime Configuration
**Format:** Programmatic API calls

**Example:**
```cpp
auto config = std::make_shared<Config>();
config->set("random_seed", 42, ConfigSource::Runtime);
config->set("neuron_count", 1000, ConfigSource::Runtime);
```

## Configuration Format Support

### 1. JSON Support
**Enabled by:** `.json` extension or `application/json` content type

**Features:**
- Full JSON specification compliance
- Unicode support
- Complex nested structures
- Array and object support

**Example:**
```json
{
    "experiment": {
        "name": "demo",
        "duration": 10.0,
        "neurons": 500,
        "plasticity": {
            "hebbian": true,
            "stdp": true,
            "reward_modulated": false
        }
    },
    "output": {
        "format": "json",
        "filepath": "results.json"
    }
}
```

### 2. YAML Support
**Enabled by:** `.yaml`, `.yml` extension or `application/yaml` content type

**Features:**
- YAML 1.2 specification compliance
- Explicit and implicit mappings
- Multiline strings
- Flow style support

**Example:**
```yaml
experiment:
    name: demo
    duration: 10.0
    neurons: 500
    plasticity:
        hebbian: true
        stdp: true
        reward_modulated: false
output:
    format: json
    filepath: results.json
```

### 3. Legacy Key=Value Support
**Enabled by:** `.cfg`, `.txt` extension or no format detection

**Features:**
- Simple key=value parsing
- Comment support (lines starting with # or //)
- Basic type conversion
- Quote handling

**Example:**
```ini
# configs/default.cfg
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
log_level = INFO
```

## Configuration Value Types

### Supported Types

| Type | JSON | YAML | Legacy | Example |
|------|------|------|--------|---------|
| Integer | ✅ | ✅ | ✅ | `"value": 42` |
| Float | ✅ | ✅ | ✅ | `"value": 3.14` |
| Boolean | ✅ | ✅ | ❌ | `"value": true` |
| String | ✅ | ✅ | ✅ | `"value": "hello"` |
| String Array | ✅ | ✅ | ❌ | `"value": ["a", "b", "c"]` |
| Float Array | ✅ | ✅ | ❌ | `"value": [1.0, 2.0, 3.0]` |
| Integer Array | ✅ | ✅ | ❌ | `"value": [1, 2, 3]` |

### Type Conversion

**Automatic Type Detection:**
- Numbers without quotes → Number
- Numbers with quotes → String
- `true`/`false` → Boolean
- Arrays → Array type

**Manual Type Conversion:**
```cpp
// Get integer value
auto intValue = config->get<int>("key");

// Get float value
auto floatValue = config->get<double>("key");

// Get string value
auto stringValue = config->get<std::string>("key");

// Get boolean value
auto boolValue = config->get<bool>("key");

// Get array value
auto arrayValue = config->get<std::vector<double>>("key");
```

## Configuration Validation

### 1. Schema Validation
**Type Checking:**
- Integer values for integer fields
- Float values for float fields
- String values for string fields
- Array values for array fields

**Constraint Validation:**
- Range checking for numeric values
- Enum validation for string values
- Format validation for special fields

**Example:**
```cpp
// Validate configuration schema
bool validateSchema(const Config& config) {
    // Check required fields
    assert(config.has("random_seed"));
    assert(config.has("neuron_count"));
    assert(config.has("region_count"));
    
    // Check type correctness
    assert(config.get<int>("random_seed").has_value());
    assert(config.get<int64_t>("neuron_count").has_value());
    assert(config.get<int64_t>("region_count").has_value());
    
    // Check value ranges
    int seed = config.getOr<int>("random_seed", 0);
    assert(seed >= 0 && seed <= INT_MAX);
    
    int64_t neurons = config.getOr<int64_t>("neuron_count", 100);
    assert(neurons > 0 && neurons <= 100000);
    
    int64_t regions = config.getOr<int64_t>("region_count", 1);
    assert(regions > 0 && regions <= 1000);
    
    return true;
}
```

### 2. Configuration Diffing
**Difference Analysis:**
- Detect value changes
- Identify new keys
- Find removed keys
- Report source changes

**Example:**
```cpp
// Compare two configurations
void compareConfigurations(const Config& config1, const Config& config2) {
    auto diff = config1.diff(config2);
    
    // Report value changes
    for (const auto& change : diff.valueChanges) {
        std::cout << "Value changed: " << change.key 
                  << " (" << change.oldValue << " -> " << change.newValue << ")" 
                  << " (source: " << static_cast<int>(change.source) << ")" 
                  << std::endl;
    }
    
    // Report new keys
    for (const auto& key : diff.newKeys) {
        std::cout << "New key: " << key << std::endl;
    }
    
    // Report removed keys
    for (const auto& key : diff.removedKeys) {
        std::cout << "Removed key: " << key << std::endl;
    }
}
```

## Configuration Sources Priority

### Source Priority
1. **Command Line** (Highest priority)
2. **Configuration File** (Medium priority)
3. **Runtime** (Lowest priority)
4. **Default** (Fallback)

### Source Precedence
```cpp
// Configuration loading order
// Command line arguments override configuration file
// Configuration file overrides runtime settings
// Runtime settings override defaults

auto config = std::make_shared<Config>();

// 1. Load from file
config->loadFromFile("config.json");

// 2. Load from command line
config->loadFromArgs(argc, argv);

// 3. Set defaults
config->set("random_seed", 42, ConfigSource::Default);
config->set("simulation_timestep", 0.001, ConfigSource::Default);
config->set("neuron_count", 1000, ConfigSource::Default);
```

### Source Tracking
Each configuration value is associated with a source:

| Source | Description | Priority |
|--------|-------------|----------|
| Default | Built-in defaults | Lowest |
| Runtime | Programmatic calls | Low |
| File | Configuration file | Medium |
| CommandLine | Command line arguments | High |

**Example:**
```cpp
// Check source of a configuration value
auto seed = config->get<int>("random_seed");
if (seed.has_value()) {
    auto entry = config->getEntry("random_seed");
    std::cout << "Source: " << static_cast<int>(entry.source) << std::endl;
}
```

## Configuration Management

### 1. Loading Configuration
**File Loading:**
```cpp
// Load from specific file
bool success = config->loadFromFile("my_config.json");

// Load from default location
bool success = config->loadFromFile("configs/default.cfg");

// Check load success
if (success) {
    std::cout << "Configuration loaded successfully" << std::endl;
} else {
    std::cout << "Failed to load configuration" << std::endl;
}
```

**Command Line Loading:**
```cpp
// Load from command line
config->loadFromArgs(argc, argv);
```

### 2. Saving Configuration
**File Saving:**
```cpp
// Save in default format
bool saved = config->saveToFile("output.cfg");

// Save in JSON format
bool saved = config->saveToFile("output.json", Format::JSON);

// Save in YAML format
bool saved = config->saveToFile("output.yaml", Format::YAML);

// Check save success
if (saved) {
    std::cout << "Configuration saved successfully" << std::endl;
} else {
    std::cout << "Failed to save configuration" << std::endl;
}
```

**Configuration Summary:**
```cpp
// Get configuration summary
std::string summary = config->summary();
std::cout << summary << std::endl;
```

### 3. Configuration Modification
**Value Setting:**
```cpp
// Set value with specific source
config->set("random_seed", 123, ConfigSource::CommandLine);
config->set("neuron_count", 500, ConfigSource::Runtime);
config->set("neuron_count", 1000, ConfigSource::Default);
```

**Value Getting:**
```cpp
// Get value with type checking
auto intValue = config->get<int>("random_seed");
if (intValue.has_value()) {
    int seed = *intValue;
    std::cout << "Random seed: " << seed << std::endl;
}

// Get value with default
int seed = config->getOr<int>("random_seed", 42);
```

**Value Checking:**
```cpp
// Check if value exists
bool hasRandomSeed = config->has("random_seed");

// Get all keys
auto keys = config->getKeys();
for (const auto& key : keys) {
    std::cout << "Key: " << key << std::endl;
}
```

### 4. Configuration Cleanup
**Value Removal:**
```cpp
// Remove specific value
config->remove("random_seed");

// Clear all values
config->clear();
```

## Configuration Error Handling

### 1. Error Types
| Error Type | Description | Example |
|------------|-------------|---------|
| ParseError | File parsing errors | "Invalid JSON format" |
| ValidationError | Type/validation errors | "Expected integer, got string" |
| FileError | File I/O errors | "File not found" |
| ConfigError | General configuration errors | "Invalid configuration source" |

### 2. Error Reporting
```cpp
// Configuration error handling
class ConfigError {
public:
    enum class ErrorCode {
        ParseError,
        ValidationError,
        FileError,
        ConfigError
    };
    
    std::string message;
    ErrorCode code;
    std::string filepath;
    int line;
};

// Error handling
void handleConfigError(const ConfigError& error) {
    switch (error.code) {
        case ConfigError::ErrorCode::ParseError:
            std::cerr << "Parse error in " << error.filepath << ":" << error.line 
                      << ": " << error.message << std::endl;
            break;
        case ConfigError::ErrorCode::ValidationError:
            std::cerr << "Validation error: " << error.message << std::endl;
            break;
        case ConfigError::ErrorCode::FileError:
            std::cerr << "File error in " << error.filepath << ": " << error.message << std::endl;
            break;
        case ConfigError::ErrorCode::ConfigError:
            std::cerr << "Configuration error: " << error.message << std::endl;
            break;
    }
}
```

### 3. Error Recovery
```cpp
// Graceful error recovery
try {
    config->loadFromFile("config.json");
} catch (const ConfigError& e) {
    // Try default configuration
    config->set("random_seed", 42, ConfigSource::Default);
    config->set("neuron_count", 100, ConfigSource::Default);
    std::cout << "Using default configuration" << std::endl;
}
```

## Configuration Examples

### 1. Complete Configuration File
**File:** `configs/complete.cfg`

```json
{
    "experiment": {
        "name": "demo",
        "duration": 10.0,
        "steps": 5000,
        "neurons": 500,
        "regions": 2,
        "plasticity": {
            "hebbian": true,
            "stdp": true,
            "reward_modulated": false,
            "learning_rate": 0.01
        }
    },
    "simulation": {
        "timestep": 0.001,
        "random_seed": 42,
        "max_steps": 10000,
        "time_limit": 0.0
    },
    "network": {
        "connection_probability": 0.15,
        "neuron_type": "excitatory",
        "synapse_type": "chemical"
    },
    "neuromodulation": {
        "dopamine_baseline": 0.0,
        "curiosity": true,
        "reward_prediction_error": true
    },
    "memory": {
        "working_memory_size": 100,
        "episodic_memory_enabled": true,
        "semantic_memory_enabled": true
    },
    "output": {
        "format": "json",
        "filepath": "results.json",
        "verbose": true,
        "save_intermediate": false
    },
    "visualization": {
        "enabled": false,
        "update_rate": 30.0,
        "output_file": "visualization.png"
    }
}
```

### 2. Command-Line Configuration Examples

```bash
# Basic configuration
./nlm --duration 30.0 --steps 50000 --neurons 1000

# Advanced configuration
./nlm \
    --config config.json \
    --experiment demo \
    --save experiment.cfg \
    --load experiment.cfg \
    --validate \
    --verbose \
    --debug

# Benchmark configuration
./nlm \
    --test config \
    --benchmark neuron_performance \
    --duration 10.0 \
    --neurons 2000
```

### 3. Programmatic Configuration

```cpp
// C++ API example
#include "core/Config/Config.hpp"

void configureBrain() {
    auto config = std::make_shared<Config>();
    
    // Set basic parameters
    config->set("random_seed", 42, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", 1000, ConfigSource::Default);
    config->set("region_count", 2, ConfigSource::Default);
    
    // Override with file
    config->loadFromFile("my_config.json");
    
    // Override with command line
    config->loadFromArgs(argc, argv);
    
    // Use in brain
    Brain brain(config);
    brain.initialize();
}
```

## Configuration Best Practices

### 1. Configuration Management
- **Use descriptive keys**: Use descriptive keys with underscores
- **Document configuration**: Document all configuration options
- **Validate early**: Validate configuration before use
- **Test configurations**: Test different configuration combinations

### 2. Performance Considerations
- **Use lazy loading**: Load configuration only when needed
- **Cache configuration**: Cache frequently used values
- **Minimize I/O**: Reduce file I/O operations
- **Use efficient formats**: Use JSON or YAML for complex configurations

### 3. Error Handling
- **Graceful degradation**: Provide sensible defaults
- **Clear error messages**: Provide detailed error messages
- **Configuration validation**: Validate configuration before use
- **Recovery mechanisms**: Implement recovery mechanisms

### 4. Security Considerations
- **Configuration security**: Secure sensitive configuration values
- **Access control**: Control access to configuration files
- **Audit trails**: Log configuration changes
- **Version control**: Track configuration changes

## Conclusion

The NLM configuration system provides comprehensive configuration management with support for multiple formats, multiple sources, validation, and error handling. It is designed for flexibility, reliability, and performance.

Key features:
- Multi-format support (JSON, YAML, legacy)
- Multiple configuration sources (file, command line, runtime)
- Type safety and validation
- Comprehensive error handling
- Save/load functionality
- Configuration diffing and comparison
- Performance optimization

The configuration system is essential for the NLM project, providing a solid foundation for neural simulation experiments and research.
