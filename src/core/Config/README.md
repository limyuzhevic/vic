# NLM Configuration System Documentation

## Overview

The NLM (Neural Learning Machine) configuration system provides a comprehensive, type-safe configuration framework for the neuromorphic neural network simulation. This document covers configuration management, validation, presets, and advanced features.

## Configuration System Architecture

### Core Components

1. **Config Class** - Main configuration manager
2. **ConfigEntry** - Individual configuration entry with metadata
3. **ConfigSource** - Configuration source enumeration
4. **Validation** - Input validation and schema checking
5. **Presets** - Configuration preset management
6. **CLI Parser** - Command-line argument parsing

### Configuration Sources

```cpp
enum class ConfigSource {
    Default,      // Built-in default values
    File,         // Loaded from configuration file
    CommandLine,  // Passed via command line
    Runtime,      // Set at runtime
    Preset,       // Loaded from preset
    Environment,  // From environment variables
    Compilation    // Compiled into the application
};
```

## Configuration File Format

### Basic Format
```ini
# Configuration file example
# Comment lines start with #

# Basic settings
neuron_count = 1000
region_count = 1
connection_probability = 0.1

# Float values
simulation_timestep = 0.001
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0

# Boolean values
enable_plasticity = true
verbose_logging = false

# String values
checkpoint_dir = ./checkpoints
config_file = configs/default.cfg

# Vector values
neuron_types = [Sensory, Internal, Motor]
learning_rates = [0.01, 0.005, 0.001]
```

### Advanced Format with Metadata
```ini
# Advanced configuration with validation

# Neuron count with validation
neuron_count = 500
# Validation: range:1,10000
# Description: Number of neurons in the network
# Default: 100
# Required: Yes
# Read-only: No

# STDP parameters with validation
stdp_ltp_weight = 0.015
# Validation: range:0.001,0.1
# Description: STDP Long-Term Potentiation weight
# Default: 0.01
# Required: Yes

# Connection probability with validation
connection_probability = 0.15
# Validation: range:0,1
# Description: Probability of connections between neurons
# Default: 0.1
# Required: Yes
```

## Configuration Validation

### Validation Rules

Configuration values can be validated using various rules:

```cpp
// Range validation (for numeric types)
range:0,100
range:1.0,10.0
range:2022,2030

// String length validation
length:1,100
pattern:^[a-zA-Z0-9_]+$

// Enum validation
enum:SYNAPTIC_PLASTICITY,HEBBIAN,STDP

// Array size validation
size:1,100
```

### Example Validation Rules

```cpp
config.set("neuron_count", 1000, 
           "Number of neurons",
           ConfigSource::Default,
           "range:1,10000",    // Range validation
           "100",             // Default value
           true,              // Required
           false);            // Read-only

config.set("learning_rate", 0.01,
           "Learning rate",
           ConfigSource::Default,
           "range:0.001,0.1",
           "0.01",
           true,
           false);
```

## Presets

### Preset Management

The configuration system includes built-in presets for common use cases:

#### Default Preset
- 1000 neurons
- 1 region
- 0.1 connection probability
- Standard STDP parameters
- Standard structural plasticity

#### Performance Preset
- 500 neurons (smaller network)
- Optimized for speed
- Reduced plasticity rates

#### Accuracy Preset
- 5000 neurons (larger network)
- More regions
- Higher connection probability
- Enhanced plasticity

#### Minimal Preset
- 100 neurons
- Single region
- Minimal configuration
- Fast startup

### Using Presets

```cpp
// Save current configuration as a preset
Config config;
config.set("neuron_count", 500);
config.set("region_count", 1);
config.saveAsPreset("my_preset");

// Load a preset
Config loadedConfig;
loadedConfig.loadPreset("default");  // Built-in preset
loadedConfig.loadPreset("my_preset");  // User-defined preset
```

### Available Built-in Presets

```cpp
Config::createDefaultPreset();     // Create default preset
Config::createPerformancePreset(); // Create performance preset
Config::createAccuracyPreset();     // Create accuracy preset
Config::createMinimalPreset();      // Create minimal preset

std::vector<std::string> presets = Config::getAvailablePresets();
```

## Command Line Interface

### Basic Usage

```bash
# Run with default configuration
./nlm

# Use custom configuration file
./nlm --config myconfig.cfg

# Enable verbose logging
./nlm --verbose

# Run specific experiment
./nlm --experiment plasticity:5000

# Exclude specific tests
./nlm --exclude-test stdp --exclude-test advanced

# Enable advanced features
./nlm --enable-advanced

# Enable metrics
./nlm --metrics

# Disable progress reporting
./nlm --no-progress
```

### Advanced Command Line Options

```bash
# Set configuration value
./nlm --neuron_count=2000
./nlm --region_count=2
./nlm --stdp_ltp_weight=0.02

# Multiple values
./nlm --neuron_types=Sensory,Internal,Motor

# Validation range
./nlm --validation-range neuron_count:1,10000

# Required keys
./nlm --required-key random_seed

# Additional arguments
./nlm --arg custom_key custom_value
```

### Help System

```bash
# Show help
./nlm --help
./nlm -h

# Show version
./nlm --version

# Show examples
./nlm --help=examples
```

## Logging Configuration

### Log Levels

```cpp
enum class LogLevel {
    Trace = 0,      // Detailed debugging information
    Debug = 1,      // Debug information
    Info = 2,       // Informational messages
    Warning = 3,    // Warning messages
    Error = 4,      // Error messages
    Critical = 5,   // Critical error messages
    Fatal = 6       // Fatal error messages
};
```

### Log Categories

```cpp
enum class LogCategory {
    System,         // Core system operations
    Brain,          // Neural network operations
    Memory,         // Memory management
    Plasticity,     // Synaptic plasticity
    Neuromodulation, // Neuromodulators
    Sensory,        // Sensory processing
    Motor,          // Motor control
    Learning,       // Learning algorithms
    Development,    // Development processes
    Prediction,     // Prediction system
    Cognition,      // Higher-level cognition
    Experiment,     // Experimental results
    Performance,    // Performance metrics
    Configuration,  // Configuration and setup
    Network,        // Network topology
    ErrorHandling,  // Error recovery
    User,           // User interactions
    Interface,      // API/interface
    Debug          // Low-level debugging
};
```

### Log Format Examples

```ini
# Console logging format
console_format = "%timestamp [%level] %message"

# File logging format
file_format = "%timestamp [%level] [%category] %message"

# Structured logging format
structured_format = "{\"timestamp\": \"%timestamp\", \"level\": \"%level\", \"message\": \"%message\"}"
```

## Performance and Monitoring

### Performance Metrics

The configuration system provides extensive performance monitoring:

```cpp
struct PerformanceMetrics {
    size_t totalOperations;     // Total configuration operations
    double averageOperationTime; // Average time per operation
    size_t cacheHits;          // Cache hit statistics
    size_t cacheMisses;         // Cache miss statistics
    size_t memoryUsage;         // Memory consumption
    std::chrono::system_clock::time_point lastActivity;
};
```

### Statistics Collection

```cpp
struct Statistics {
    size_t totalEntries;               // Total configuration entries
    size_t entriesBySource[6];          // Entries by source type
    size_t entriesByType[4];            // Entries by data type
    size_t validationErrors;            // Validation error count
    size_t validationWarnings;          // Validation warning count
    double memoryUsage;                 // Estimated memory usage
    std::chrono::system_clock::time_point lastModified;
};
```

## Error Handling

### Error Types

The configuration system categorizes errors for better handling:

```cpp
enum class ConfigError {
    InvalidKey,        // Invalid configuration key
    InvalidValue,      // Invalid configuration value
    MissingRequired,    // Missing required configuration
    FileNotFound,      // Configuration file not found
    ParseError,        // Configuration file parsing error
    ValidationError,   // Configuration validation error
    MemoryError,       // Memory allocation error
    PermissionError    // Permission error
};
```

### Error Recovery

```cpp
// Example error recovery
Config config;
try {
    config.loadFromFile("config.cfg");
} catch (const ConfigError& e) {
    std::cerr << "Configuration error: " << errorToString(e) << std::endl;
    // Fallback to defaults
    config.set("neuron_count", 100, ConfigSource::Default);
} catch (const std::exception& e) {
    std::cerr << "Unexpected error: " << e.what() << std::endl;
    // Return error code
    return 1;
}
```

## Integration Examples

### Example 1: Advanced Configuration with Validation

```cpp
#include "core/Config/Config.hpp"

void setupAdvancedConfiguration() {
    Config config;
    
    // Load default configuration
    config.loadFromFile("configs/default.cfg");
    
    // Override with command line arguments
    int argc = 4;
    char* argv[] = { (char*)"nlm", (char*)"--neuron_count", (char*)"2000",
                    (char*)"--stdp_ltp_weight", (char*)"0.015" };
    config.loadFromArgs(argc, argv);
    
    // Validate configuration
    auto validationResult = config.validate();
    if (!validationResult.isValid) {
        std::cerr << "Configuration validation failed:" << std::endl;
        for (const auto& error : validationResult.errors) {
            std::cerr << "  ERROR: " << error << std::endl;
        }
        return;
    }
    
    // Apply configuration
    size_t neuronCount = config.getOr<size_t>("neuron_count", 1000);
    float stdpLtpWeight = config.getOr<float>("stdp_ltp_weight", 0.01f);
    
    // Initialize brain with validated configuration
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
    }
}
```

### Example 2: Preset-based Configuration

```cpp
#include "core/Config/Config.hpp"

void setupWithPreset() {
    // Check available presets
    std::vector<std::string> presets = Config::getAvailablePresets();
    
    std::cout << "Available presets:" << std::endl;
    for (const auto& preset : presets) {
        std::cout << "  " << preset << std::endl;
    }
    
    // Select preset based on user input
    std::string selectedPreset;
    std::cout << "Enter preset name: ";
    std::cin >> selectedPreset;
    
    Config config;
    if (config.loadPreset(selectedPreset)) {
        std::cout << "Loaded preset: " << selectedPreset << std::endl;
        
        // Apply configuration
        size_t neuronCount = config.getOr<size_t>("neuron_count", 1000);
        size_t regionCount = config.getOr<size_t>("region_count", 1);
        
        std::cout << "Configuration: " << neuronCount << " neurons, " 
                  << regionCount << " regions" << std::endl;
    } else {
        std::cerr << "Failed to load preset: " << selectedPreset << std::endl;
    }
}
```

### Example 3: Configuration Search and Filtering

```cpp
#include "core/Config/Config.hpp"

void searchAndFilterConfiguration() {
    Config config;
    config.loadFromFile("config.cfg");
    
    // Search for keys containing "neuron"
    std::vector<std::string> neuronKeys = config.findKeysContaining("neuron");
    std::cout << "Keys containing 'neuron':" << std::endl;
    for (const auto& key : neuronKeys) {
        std::cout << "  " << key << " = " << config.get<std::string>(key).value() << std::endl;
    }
    
    // Search for keys by type
    std::vector<std::string> intKeys = config.getKeysByType("int");
    std::cout << "Integer keys:" << std::endl;
    for (const auto& key : intKeys) {
        std::cout << "  " << key << " = " << config.get<int>(key).value() << std::endl;
    }
    
    // Filter by source
    std::vector<ConfigEntry> fileEntries = config.filterBySource(ConfigSource::File);
    std::cout << "File configuration entries:" << std::endl;
    for (const auto& entry : fileEntries) {
        std::cout << "  " << entry.key << " = " << entry.getStringValue() << " (source: " 
                  << Config::getSourceString(entry.source) << ")" << std::endl;
    }
}
```

## Configuration File Templates

### Basic Configuration Template
```ini
# NLM Configuration File
# Generated by: Your Tool
# Generated on: 2026-10-17

# Basic network parameters
neuron_count = 1000
region_count = 1
connection_probability = 0.1

# STDP parameters
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0

# Structural plasticity parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Simulation parameters
simulation_timestep = 0.001
replay_interval = 100
consolidation_interval = 1000

# Random seed
random_seed = 42

# Logging configuration
log_level = Info
log_file = nlm.log
enable_colors = true

# Performance settings
checkpoint_dir = ./checkpoints
max_checkpoints = 10
compression_level = Balanced
```

### Development Configuration Template
```ini
# Development Configuration
# For debugging and development purposes

# Debug mode
neuron_count = 100
verbose_logging = true
enable_debug_features = true

# Reduced plasticity for faster development
synaptogenesis_rate = 0.001
pruning_rate = 0.0001

# Frequent checkpoints for debugging
checkpoint_dir = ./dev_checkpoints
max_checkpoints = 50

# Lower connection probability for faster startup
connection_probability = 0.05

# Enable detailed logging
log_level = Debug
log_format = "%timestamp [%level] [%category] %message"
```

### Production Configuration Template
```ini
# Production Configuration
# Optimized for performance and stability

# Network size
neuron_count = 5000
region_count = 2
connection_probability = 0.15

# Optimized STDP parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.018
stdp_tau = 15.0

# Reduced structural plasticity for stability
synaptogenesis_rate = 0.0005
pruning_rate = 0.00005

# Performance optimization
simulation_timestep = 0.002
replay_interval = 500
consolidation_interval = 5000

# Efficient checkpointing
checkpoint_dir = ./prod_checkpoints
max_checkpoints = 5
compression_level = Fast

# Production logging
log_level = Warning
log_file = ./logs/nlm.log
log_rotation = true
max_file_size = 100MB
max_files = 10

# Random seed for reproducibility
random_seed = 12345
```

## Migration Guide

### From Old Configuration System

If you're migrating from an older version of NLM, follow these steps:

1. **Update Configuration File Format**
   - Old format: `key = value`
   - New format: Add comments and validation
   ```ini
   # Old
   neuron_count = 1000
   
   # New
   neuron_count = 1000
   # Validation: range:1,10000
   # Description: Number of neurons
   ```

2. **Update Code to Use New APIs**
   ```cpp
   // Old
   config->set("neuron_count", 1000);
   
   // New
   config.set("neuron_count", 1000, 
              "Number of neurons",
              ConfigSource::Default,
              "range:1,10000",
              "100",
              true,
              false);
   ```

3. **Add Validation**
   ```cpp
   // Validate before use
   auto validation = config.validate();
   if (!validation.isValid) {
       // Handle validation errors
   }
   ```

4. **Use Presets**
   ```cpp
   // Load appropriate preset
   Config::createDefaultPreset();
   config.loadPreset("default");
   ```

### Configuration Migration Example

```cpp
// Migration helper function
Config migrateConfiguration(const std::string& oldConfigPath) {
    Config newConfig;
    
    // Load old configuration
    // (parse old format)
    
    // Map old keys to new keys
    std::map<std::string, std::string> keyMapping = {
        {"num_neurons", "neuron_count"},
        {"num_regions", "region_count"},
        {"conn_prob", "connection_probability"},
        {"stdp_ltp", "stdp_ltp_weight"},
        {"stdp_ltd", "stdp_ltd_weight"},
        {"stdp_tau_ms", "stdp_tau"},
        {"synapse_form", "synaptogenesis_rate"},
        {"synapse_prune", "pruning_rate"},
        {"time_step", "simulation_timestep"},
        {"seed", "random_seed"}
    };
    
    // Convert and migrate
    for (const auto& pair : oldConfig) {
        std::string newKey = keyMapping.count(pair.first) ? 
                            keyMapping[pair.first] : pair.first;
        
        // Add validation and metadata
        std::string description = "Migrated from " + pair.first;
        std::string validation = "auto;";
        
        newConfig.set(newKey, pair.second, description, 
                     ConfigSource::File, validation, "", true, false);
    }
    
    return newConfig;
}
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: Configuration File Not Found
**Symptom:** "Failed to load configuration file"

**Solution:**
```bash
# Check if file exists
ls -la configs/default.cfg

# Create directory if needed
mkdir -p configs

# Create basic config file
echo "neuron_count = 100" > configs/default.cfg
```

#### Issue 2: Invalid Configuration Values
**Symptom:** "Configuration validation failed"

**Solution:**
```bash
# Check validation errors
./nlm --config invalid.cfg 2>&1 | grep -i "validation"

# Fix configuration file
# Check file syntax
echo "neuron_count = 1000" > configs/default.cfg
# Add validation comments
# neuron_count = 1000  # Validation: range:1,10000
```

#### Issue 3: Missing Required Keys
**Symptom:** "Required key is missing"

**Solution:**
```cpp
// Add required key
config.set("random_seed", 42, 
           "Random seed for reproducibility",
           ConfigSource::Default,
           "range:0,10000000000",
           "42",
           true,   // Required
           false); // Read-only
```

#### Issue 4: Performance Issues
**Symptom:** "Configuration initialization taking too long"

**Solution:**
```bash
# Use performance preset
./nlm --experiment performance

# Check statistics
nlm --metrics
```

## Best Practices

### 1. Always Validate Configuration
```cpp
try {
    config.loadFromFile("config.cfg");
    auto validation = config.validate();
    if (!validation.isValid) {
        throw std::runtime_error("Invalid configuration");
    }
} catch (const std::exception& e) {
    std::cerr << "Configuration error: " << e.what() << std::endl;
    return 1;
}
```

### 2. Use Presets for Common Configurations
```cpp
// Instead of writing the same configuration repeatedly
// Use appropriate presets

if (args.use_performance) {
    config.loadPreset("performance");
} else if (args.use_accuracy) {
    config.loadPreset("accuracy");
} else {
    config.loadPreset("default");
}
```

### 3. Log Configuration Changes
```cpp
// Log when configuration changes
void logConfigChange(const std::string& key, const ConfigValue& oldValue, 
                    const ConfigValue& newValue) {
    NLM_LOG_INFO(std::string("Configuration changed: ") + 
                key + " = " + getValueString(newValue));
}
```

### 4. Use Configuration Validation
```cpp
// Add validation rules
config.set("learning_rate", 0.01f,
           "Learning rate for neural plasticity",
           ConfigSource::Default,
           "range:0.001,0.1",
           "0.01",
           true,
           false);

// Validate before use
auto validation = config.validate();
if (!validation.isValid) {
    // Log validation errors
    for (const auto& error : validation.errors) {
        NLM_LOG_ERROR("Validation error: " + error);
    }
    // Use safe defaults
    config.set("learning_rate", 0.01f, ConfigSource::Runtime);
}
```

### 5. Handle Configuration Errors Gracefully
```cpp
try {
    config.loadFromFile("config.cfg");
    config.loadFromArgs(argc, argv);
    
    // Validate
    auto validation = config.validate();
    if (!validation.isValid) {
        // Use fallback configuration
        config.set("neuron_count", 100, ConfigSource::Runtime);
        config.set("region_count", 1, ConfigSource::Runtime);
        
        // Log warning
        for (const auto& warning : validation.warnings) {
            NLM_LOG_WARNING("Configuration warning: " + warning);
        }
    }
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
} catch (const std::exception& e) {
    std::cerr << "Fatal configuration error: " << e.what() << std::endl;
    return 1;
}
```

## Conclusion

The NLM configuration system provides a robust, flexible framework for managing neural network configurations. Key improvements include:

1. **Type Safety**: Compile-time type checking for configuration values
2. **Validation**: Comprehensive validation with detailed error messages
3. **Presets**: Built-in and user-defined configuration presets
4. **CLI Integration**: Full command-line interface support
5. **Error Handling**: Graceful error handling with recovery options
6. **Performance**: Optimized for performance with minimal overhead
7. **Documentation**: Comprehensive documentation and examples

This configuration system enables users to easily configure, validate, and manage NLM neural network simulations while providing advanced features for power users and ensuring reliability for production deployments.

## References

- NLM Configuration System Design
- C++20 Best Practices
- Configuration Management Patterns
- Neural Network Simulation Configuration

## See Also

- [`Config.hpp`](/app/src/core/Config/Config.hpp) - Configuration header
- [`Config.cpp`](/app/src/core/Config/Config.cpp) - Configuration implementation
- [`Logger.hpp`](/app/src/core/Logger/Logger.hpp) - Enhanced logging system
- [`Brain.cpp`](/app/src/brain/Brain.cpp) - Enhanced brain implementation
- [test_config.cpp](/app/tests/test_config.cpp) - Configuration test suite