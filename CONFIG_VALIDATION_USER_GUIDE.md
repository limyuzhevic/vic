# NLM Configuration Validation and Error Handling - User Guide

## Introduction

This document provides a comprehensive guide to the enhanced configuration validation and error handling system in the NLM (Neural Language Model) project. The system provides robust validation, error reporting, and automatic repair capabilities for NLM configurations.

## Quick Start

### 1. Basic Configuration Validation

```cpp
#include "Brain.hpp"
#include "Config.hpp"

int main() {
    // Create configuration object
    auto config = std::make_shared<Config>();
    
    // Load configuration from file
    config->loadFromFile("my_config.cfg");
    
    // Basic validation - check for errors
    if (!config->validate()) {
        auto errors = config->getValidationErrors();
        std::cout << "Configuration validation failed with " 
                  << errors.size() << " errors:" << std::endl;
        
        for (const auto& error : errors) {
            std::cout << "  " << error.key << ": " << error.errorMessage << std::endl;
        }
    }
    
    // Attempt automatic repair for common issues
    if (config->fixCommonIssues()) {
        std::cout << "Configuration repaired successfully" << std::endl;
        // Re-validate after repair
        if (!config->validate()) {
            std::cout << "Warning: Repair did not fix all issues" << std::endl;
        }
    }
    
    // Create brain with validation
    auto brain = std::make_shared<Brain>(config);
    
    // Initialize with comprehensive validation
    if (brain->initializeWithValidation()) {
        std::cout << "Brain initialized successfully with validation" << std::endl;
    } else {
        std::cout << "Brain initialization failed - check configuration" << std::endl;
        return 1;
    }
    
    return 0;
}
```

### 2. Schema-Based Validation

```cpp
// Include schema header
#include "ConfigSchema.hpp"

int main() {
    auto config = std::make_shared<Config>();
    config->loadFromFile("config.cfg");
    
    // Create custom schema
    ConfigSchema schema;
    
    // Add validation rules for important parameters
    schema.addRangeRule("learning_rate", 0.0, 1.0, false);  // Must be between 0 and 1
    schema.addRangeRule("threshold", 0.0, 1.0, false);     // Must be between 0 and 1  
    schema.addNumericRule("neuron_count", typeid(size_t), 1, 1000000, true);  // Required, 1-1M neurons
    schema.addStringRule("model_name", 1, 100, "^[a-zA-Z0-9_-]+$", false);  // Alphanumeric, 1-100 chars
    
    // Validate against custom schema
    ValidationResult result = config->validateAgainstSchema(schema);
    
    if (!result.isValid) {
        std::cout << "Schema validation failed:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  " << error.key << ": " << error.errorMessage << std::endl;
        }
    } else {
        std::cout << "Schema validation passed!" << std::endl;
    }
    
    return 0;
}
```

### 3. Configuration Health Monitoring

```cpp
int main() {
    auto config = std::make_shared<Config>();
    config->loadFromFile("config.cfg");
    
    // Enable health monitoring
    config->enableHealthMonitoring();
    
    // Check configuration health
    if (!config->isConfigurationHealthy()) {
        ValidationResult health = config->validateHealth();
        std::cout << "Configuration health check failed:" << std::endl;
        
        if (health.hasErrors()) {
            std::cout << "  Errors:" << std::endl;
            for (const auto& error : health.errors) {
                std::cout << "    " << error.key << ": " << error.errorMessage << std::endl;
            }
        }
        
        if (health.hasWarnings()) {
            std::cout << "  Warnings:" << std::endl;
            for (const auto& warning : health.warnings) {
                std::cout << "    " << warning << std::endl;
            }
        }
    } else {
        std::cout << "Configuration health check passed!" << std::endl;
    }
    
    return 0;
}
```

### 4. Brain Integration with Validation

```cpp
int main() {
    // Load configuration
    auto config = std::make_shared<Config>();
    config->loadFromFile("brain_config.cfg");
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    // Enable health monitoring
    brain->enableHealthMonitoring(true);
    
    // Initialize with comprehensive validation
    std::cout << "Initializing brain with validation..." << std::endl;
    if (brain->initializeWithValidation()) {
        std::cout << "✓ Brain initialization successful" << std::endl;
        
        // Check configuration health after initialization
        if (brain->isConfigurationHealthy()) {
            std::cout << "✓ Configuration health check passed" << std::endl;
        } else {
            std::cout << "⚠ Configuration health check failed - monitoring active" << std::endl;
        }
        
        // Get validation statistics
        auto stats = brain->getValidationStatistics();
        std::cout << "Validation statistics:" << std::endl;
        for (const auto& pair : stats) {
            std::cout << "  " << pair.first << ": " << pair.second << std::endl;
        }
        
    } else {
        std::cout << "✗ Brain initialization failed" << std::endl;
        return 1;
    }
    
    return 0;
}
```

## Configuration File Format

### Basic Key-Value Format
```
# Configuration file for NLM brain simulation
neuron_count = 10000
region_count = 4
connection_probability = 0.15
learning_rate = 0.01
threshold = 0.5
model_name = "spiking_neural_net"

# Optional parameters with comments
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
simulation_timestep = 0.001
random_seed = 12345
```

### Command Line Arguments
```bash
./brain_simulation \
    --neuron_count=5000 \
    --region_count=2 \
    --learning_rate=0.05 \
    --threshold=0.3 \
    -model_name "deep_network" \
    -stdp_ltp_weight 0.02
```

## Configuration Types

### Numeric Types
```cpp
// Integer values
config->set("neuron_count", 10000);

// Floating point values  
config->set("learning_rate", 0.01);

// Size types
config->set("replay_interval", 1000U);
```

### String Types
```cpp
// String values
config->set("model_name", "spiking_network");
config->set("checkpoint_dir", "./checkpoints");
```

### Boolean Types
```cpp
// Boolean values
config->set("enable_plasticity", true);
config->set("use_gpu", false);
```

### List Types
```cpp
// Vector of integers
config->set("hidden_layers", std::vector<int>{64, 128, 64});

// Vector of doubles
config->set("learning_rates", std::vector<double>{0.01, 0.001, 0.0001});

// Vector of strings
config->set("layer_types", std::vector<std::string>{"sensory", "internal", "motor"});
```

## Validation Error Types

### Common Configuration Errors

| Error Type | Description | Example |
|------------|-------------|---------|
| `Empty key` | Configuration key is empty | Key "": Missing description |
| `Value must be between 0.0 and 1.0` | Threshold/rate out of range | Threshold "threshold": 1.5 |
| `Count/max must be non-negative` | Negative count value | Count "neuron_count": -100 |
| `Rate/level must be non-negative` | Negative rate value | Rate "learning_rate": -0.01 |

### Schema Validation Errors

| Error Type | Description | Example |
|------------|-------------|---------|
| `Required configuration key is missing` | Missing required parameter | Key "neuron_count": Not found |
| `Value does not match schema validation rule` | Type or format mismatch | String "model_name": "invalid@name" |

## Automatic Configuration Repair

The system automatically repairs common configuration issues:

1. **Empty Key Repair:** Generates unique keys for empty configuration keys
2. **Value Range Repair:** Clamps threshold, rate, and level values to valid ranges
3. **Default Value Injection:** Injects default values from schema for missing required keys
4. **Type Conversion:** Attempts to convert values to appropriate types

### Example Repair Scenario
```cpp
// Before repair:
config->set("", "invalid_value");  // Empty key
config->set("threshold", 1.5);      // Out of range
config->set("learning_rate", "high"); // Wrong type

// After fixCommonIssues():
// Key becomes "unnamed_12345" (unique)
// Threshold becomes 1.0 (clamped)
// Learning_rate remains "high" (cannot convert automatically)
```

## Performance Monitoring

### Validation Statistics
The system tracks and reports validation performance:

```cpp
auto stats = brain->getValidationStatistics();
for (const auto& pair : stats) {
    std::cout << pair.first << ": " << pair.second << std::endl;
}
```

**Common Statistics:**
- `average_validation_time_ms`: Average time per validation
- `total_validations`: Total number of validations performed
- `total_errors`: Total number of validation errors detected
- `total_warnings`: Total number of warnings issued
- `success_rate`: Percentage of successful validations

### Performance Metrics
```cpp
// Get detailed performance metrics
auto metrics = brain->getValidationPerformanceMetrics();
for (const auto& pair : metrics) {
    std::cout << pair.first << ": " 
              << pair.second.count() << " ms" << std::endl;
}
```

## Integration with Existing Code

### For Existing Users
If you already use NLM but want to add validation:

1. **Minimal Changes:** Wrap existing initialization with validation:
```cpp
// Old code:
brain->initialize();

// New code with validation:
if (brain->initializeWithValidation()) {
    // Success
} else {
    // Handle failure
}
```

2. **Gradual Adoption:** Use `fixCommonIssues()` to handle existing configuration files:
```cpp
config->fixCommonIssues();  // Auto-fix common issues
config->validate();        // Check remaining issues
```

### Configuration Migration

#### From Legacy Format
```cpp
// Legacy format may have issues
config->loadFromFile("legacy.cfg");

// New validation will catch issues and attempt repair
if (!config->validate()) {
    config->fixCommonIssues();  // Auto-fix common problems
    config->validate();        // Check if fixed
}
```

## Error Handling Best Practices

### 1. Always Validate Before Use
```cpp
// Always validate configuration before using it
if (!config->validate()) {
    std::cerr << "Invalid configuration - aborting" << std::endl;
    return 1;
}
```

### 2. Use Automatic Repair Wisely
```cpp
// Use automatic repair for known safe configurations
if (config->fixCommonIssues()) {
    std::cout << "Configuration auto-fixed" << std::endl;
    // Still need to validate after repair
    if (!config->validate()) {
        std::cerr << "Auto-fix did not resolve all issues" << std::endl;
        return 1;
    }
}
```

### 3. Handle Validation Failures Gracefully
```cpp
// Provide helpful error messages
if (!config->validate()) {
    auto errors = config->getValidationErrors();
    std::cerr << "Configuration validation failed:" << std::endl;
    
    for (const auto& error : errors) {
        std::cerr << "  " << error.key << ": " << error.errorMessage << std::endl;
        std::cerr << "    Suggested fix: Check value range/type for " << error.key << std::endl;
    }
    
    std::cerr << "\nRun 'nlm-validate' to see full validation details" << std::endl;
    return 1;
}
```

## Command Line Tools

### Validation Utility
The project includes a command-line validation utility:

```bash
# Validate configuration file
nlm-validate config.cfg

# Validate and attempt repair
nlm-validate --repair config.cfg

# Validate with custom schema
custom_schema.xml
nlm-validate --schema schema.xml config.cfg

# Check configuration health
nlm-validate --health config.cfg

# Get validation statistics
nlm-validate --stats config.cfg
```

### Configuration Tools
```bash
# List all configuration keys
nlm-config list

# Show configuration value
nlm-config get learning_rate

# Set configuration value
nlm-config set learning_rate 0.01

# Show configuration summary
nlm-config summary

# Backup current configuration
nlm-config backup

# Restore configuration
nlm-config restore backup.cfg
```

## Troubleshooting

### Common Issues and Solutions

| Issue | Command | Description |
|-------|---------|-------------|
| `Validation failed` | `nlm-validate --repair config.cfg` | Auto-repair common issues |
| `Missing required key` | Check config file | Add required parameters |
| `Value out of range` | `nlm-validate --schema schema.xml config.cfg` | Validate with schema |
| `Brain initialization fails` | `nlm-validate --health config.cfg` | Check configuration health |
| `Performance slow` | `nlm-validate --stats config.cfg` | Check validation statistics |

### Error Code Reference

| Error Code | Description | Resolution |
|------------|-------------|------------|
| CV001 | Configuration validation error | Check configuration values |
| CV002 | Schema validation error | Validate against schema |
| CV003 | Health check failed | Check configuration health |
| CV004 | Integration validation failed | Check system integration |
| CV005 | Repair failed | Manual configuration required |

## Configuration Examples

### Example 1: Minimal Valid Configuration
```
neuron_count = 1000
region_count = 1
learning_rate = 0.01
threshold = 0.5
model_name = "minimal_brain"
```

### Example 2: Advanced Configuration
```
neuron_count = 10000
region_count = 4
connection_probability = 0.15
learning_rate = 0.01
threshold = 0.5
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
simulation_timestep = 0.001
replay_interval = 1000
consolidation_interval = 5000
checkpoint_dir = "./checkpoints"
enable_gpu = true
model_name = "advanced_spiking_net"
```

### Example 3: Configuration with All Features
```
# Core parameters
neuron_count = 50000
region_count = 8
learning_rate = 0.001
threshold = 0.3

# Plasticity parameters
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Memory parameters
working_memory_capacity = 5000
max_episodes = 1000
replay_interval = 100
consolidation_interval = 1000

# Simulation parameters
simulation_timestep = 0.001
random_seed = 12345
checkpoint_dir = "./checkpoints"

# Model identification
model_name = "phase6_integrated_brain"
model_version = "1.0"
```

## Best Practices

### 1. Configuration Organization
```cpp
// Group related configuration parameters
struct NeuralConfig {
    // Core parameters
    size_t neuronCount;
    size_t regionCount;
    float learningRate;
    float threshold;
    
    // Plasticity parameters
    float stdpLtpWeight;
    float stdpLtdWeight;
    float stdpTau;
    
    // Memory parameters
    size_t workingMemoryCapacity;
    size_t maxEpisodes;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Simulation parameters
    double simulationTimestep;
    uint64_t randomSeed;
    std::string checkpointDir;
    
    // Model identification
    std::string modelName;
    std::string modelVersion;
};
```

### 2. Validation Strategy
```cpp
// Layered validation approach
class ConfigurationValidator {
public:
    // 1. Basic validation - check for errors
    bool validateBasic(const Config& config);
    
    // 2. Schema validation - check against rules
    bool validateSchema(const Config& config, const ConfigSchema& schema);
    
    // 3. Cross-validation - check consistency
    bool validateCross(const Config& config);
    
    // 4. Integration validation - check compatibility
    bool validateIntegration(const Config& config, const Brain& brain);
    
    // 5. Health validation - check ongoing health
    bool validateHealth(const Config& config);
};
```

### 3. Error Recovery
```cpp
// Comprehensive error recovery strategy
class ConfigurationErrorRecovery {
public:
    // Attempt to fix common issues
    bool fixCommonIssues(Config& config);
    
    // Repair based on error type
    bool repairBasedOnError(Config& config, const ConfigValidationError& error);
    
    // Apply configuration fixes
    bool applyConfigurationFixes(Config& config);
    
    // Validate recovery
    bool validateRecovery(const Config& config);
};
```

## Conclusion

The NLM configuration validation and error handling system provides a comprehensive approach to configuration management:

### Key Benefits

1. **Robust Validation:** Multiple validation levels ensure configuration correctness
2. **Automatic Repair:** Common issues are automatically fixed when possible
3. **Detailed Error Reporting:** Clear, actionable error messages help users fix issues
4. **Performance Monitoring:** Tracks validation performance for optimization
5. **Brain Integration:** Seamlessly integrates with brain initialization and operation
6. **Health Monitoring:** Provides ongoing configuration health monitoring
7. **Backward Compatibility:** Works with existing configuration files
8. **Extensible:** Easy to add new validation rules and custom schemas

### Getting Started

1. **Quick Start:** Use `initializeWithValidation()` for basic validation
2. **Custom Validation:** Create schemas for specific requirements
3. **Health Monitoring:** Enable health monitoring for ongoing checks
4. **Performance Tuning:** Use statistics to optimize validation performance
5. **Error Recovery:** Use automatic repair for common issues

This system makes it easier to develop, test, and deploy NLM neural network simulations with confidence in configuration validity and system reliability.
