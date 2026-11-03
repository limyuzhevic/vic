# NLM API Documentation

This document provides comprehensive API documentation for the NLM (Neural Learning Machine) framework.

## Overview

NLM is a computational brain simulation project that implements neural dynamics, learning, memory, and cognition based on biologically-inspired principles.

## Configuration API

### Config Class

The `nlm::Config` class provides a unified interface for managing configuration settings.

#### Constructor
```cpp
Config(); // Default constructor creates empty configuration
```

#### Core Methods

**Loading Configuration**
```cpp
// Load from plain text file (key=value format)
bool loadFromFile(const std::string& filepath);

// Load from JSON file
bool loadFromJSONFile(const std::string& filepath);

// Load from YAML file
bool loadFromYAMLFile(const std::string& filepath);

// Load from command line arguments
bool loadFromArgs(int argc, char** argv);

// High-level configuration methods
void configureFromFile(const std::string& filepath);
void configureFromJSONFile(const std::string& filepath);
void configureFromYAMLFile(const std::string& filepath);
void configureFromCommandLine(int argc, char** argv);
```

**Value Access**
```cpp
// Generic template-based access
template<typename T>
std::optional<T> get(const std::string& key) const;

// Convenience method with default value
template<typename T>
T getOr(const std::string& key, const T& defaultValue) const;

// Specific type access (convenience wrappers)
int getInt(const std::string& key) const;
int64_t getInt64(const std::string& key) const;
double getDouble(const std::string& key) const;
bool getBool(const std::string& key) const;
std::string getString(const std::string& key) const;
std::vector<int> getIntVector(const std::string& key) const;
std::vector<double> getDoubleVector(const std::string& key) const;
std::vector<std::string> getStringVector(const std::string& key) const;
```

**Value Setting**
```cpp
// Set values with type safety
void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);

// Convenience methods
void setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
void setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
```

**Query and Management**
```cpp
// Check if key exists
bool has(const std::string& key) const;

// Remove key
void remove(const std::string& key);

// Get all keys
std::vector<std::string> getKeys() const;

// Clear all configuration
void clear();

// Get configuration summary
std::string summary() const;

// Get formatted configuration report with validation
std::string getConfigurationReport() const;

// Configuration validation
bool validate() const;
bool validateEntry(const ConfigEntry& entry) const;
```

**File I/O**
```cpp
// Save to plain text file
bool saveToFile(const std::string& filepath) const;

// Save configuration with proper formatting
void exportToJSON(const std::string& filepath) const;
void exportToYAML(const std::string& filepath) const;
```

#### Configuration Sources

The `ConfigSource` enum defines different configuration sources:

```cpp
enum class ConfigSource {
    Default,      // Default configuration
    File,         // File-based configuration
    CommandLine,  // Command-line configuration
    Runtime       // Runtime configuration
};
```

Configuration sources are ordered by precedence (later sources override earlier ones):

1. Default configuration
2. File configuration
3. Command-line configuration
4. Runtime configuration

#### Supported File Formats

**Plain Text (default.cfg)**
```ini
# Comments start with # or /
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
dopamine_baseline = 0.1
reward_discount_factor = 0.99
environment_name = GridWorld
environment_width = 10
environment_height = 10
log_level = INFO
log_to_file = false
max_simulation_steps = 10000
```

**JSON Example (config.json)**
```json
{
    "simulation": {
        "timestep": 0.001,
        "max_steps": 10000,
        "neuron_count": 1000,
        "region_count": 2
    },
    "plasticity": {
        "learning_rate": 0.01,
        "stdp": {
            "ltp_weight": 0.01,
            "ltd_weight": 0.012,
            "time_constant": 20.0
        }
    },
    "development": {
        "synaptogenesis_rate": 0.001,
        "pruning_rate": 0.0001
    },
    "neuromodulation": {
        "dopamine_baseline": 0.1,
        "reward_discount_factor": 0.99
    },
    "environment": {
        "name": "GridWorld",
        "width": 10,
        "height": 10
    },
    "logging": {
        "level": "INFO",
        "to_file": false,
        "filename": "nlm.log"
    }
}
```

**YAML Example (config.yml)**
```yaml
simulation:
  timestep: 0.001
  max_steps: 10000
  neuron_count: 1000
  region_count: 2

plasticity:
  learning_rate: 0.01
  stdp:
    ltp_weight: 0.01
    ltd_weight: 0.012
    time_constant: 20.0

development:
  synaptogenesis_rate: 0.001
  pruning_rate: 0.0001

neuromodulation:
  dopamine_baseline: 0.1
  reward_discount_factor: 0.99

environment:
  name: GridWorld
  width: 10
  height: 10

logging:
  level: INFO
  to_file: false
  filename: "nlm.log"
```

#### Command Line Usage

**Basic command line arguments:**
```bash
# Set individual parameters
./nlm --simulation_timestep=0.001 --neuron_count=1000 --region_count=2

# Set multiple parameters
./nlm --plasticity_learning_rate=0.01 --stdp_ltp_weight=0.01 --stdp_ltd_weight=0.012

# Short form (requires space-separated values)
./nlm -simulation_timestep 0.001 -neuron_count 1000 -region_count 2

# File-based configuration
./nlm --config_file=config.json
```

#### Advanced Configuration

**Configuration Templates**
```cpp
// Example: Create specialized configuration templates
class ConfigTemplates {
public:
    static Config getHighPerformanceConfig() {
        Config config;
        config.configureFromFile("configs/high_perf.cfg");
        config.setInt("neuron_count", 5000, ConfigSource::Runtime);
        config.setDouble("plasticity_learning_rate", 0.001, ConfigSource::Runtime);
        return config;
    }
    
    static Config getScientificConfig() {
        Config config;
        config.configureFromJSONFile("configs/scientific_config.json");
        config.setInt("random_seed", 42, ConfigSource::Runtime);
        config.setBool("log_to_file", true, ConfigSource::Runtime);
        return config;
    }
};
```

**Configuration Validation**
```cpp
void validateConfiguration(const Config& config) {
    // Check for required parameters
    if (!config.has("simulation_timestep")) {
        throw std::runtime_error("Missing required parameter: simulation_timestep");
    }
    
    // Validate parameter ranges
    double timestep = config.getOr("simulation_timestep", 0.001);
    if (timestep <= 0.0 || timestep > 1.0) {
        throw std::runtime_error("Invalid simulation_timestep: must be (0, 1.0]");
    }
    
    int neuronCount = config.getOr("neuron_count", 1000);
    if (neuronCount <= 0 || neuronCount > 100000) {
        throw std::runtime_error("Invalid neuron_count: must be (0, 100000]");
    }
    
    // Print configuration report
    std::cout << config.getConfigurationReport();
}
```

## Plasticity API

### STDP Class

**Constructor**
```cpp
STDP(); // Default STDP with standard parameters
```

**Configuration**
```cpp
void configure(float ltpWeight, float ltdWeight, float tau);
void setLTPWeight(float weight);
void setLTDWeight(float weight);
void setTimeConstant(float tau);
float getLTPWeight() const;
float getLTDWeight() const;
float getTimeConstant() const;
```

**Advanced Features**
```cpp
void setRewardFactor(float factor);
float getRewardFactor() const;
void setRewardTraceDecay(float decay);
float getRewardTraceDecay() const;
float getEligibilityTrace() const;
```

### RewardModulatedSTDP Class

**Constructor**
```cpp
RewardModulatedSTDP(); // Reward-modulated STDP
```

**Reward Configuration**
```cpp
void setLearningRate(float rate);
void setDiscountFactor(float factor);
void setTraceDecay(float decay);
void applyReward(float reward);
float getReward() const;
void resetReward();
```

### TripletSTDP Class

**Constructor**
```cpp
TripletSTDP(); // Triplet-based STDP
```

**Triplet Configuration**
```cpp
void setAplus(float a);                    // Triple potentiation parameter
void setAminus(float a);                   // Triple depression parameter
void setTauPlus(float tau);                // Triple potentiation time constant
void setTauMinus(float tau);               // Triple depression time constant
void setBaselineLTPWeight(float w);        // Baseline LTP weight
void setBaselineLTDWeight(float w);        // Baseline LTD weight
```

## Usage Examples

### Basic Configuration

```cpp
#include <nlm.h>
#include <iostream>

int main() {
    try {
        // Create and configure
        nlm::Config config;
        config.configureFromFile("configs/default.cfg");
        
        // Validate configuration
        if (!config.validate()) {
            std::cerr << "Configuration validation failed!" << std::endl;
            return 1;
        }
        
        // Access configuration values
        double timestep = config.getDouble("simulation_timestep");
        int neuronCount = config.getInt("neuron_count");
        std::string envName = config.getString("environment_name");
        
        std::cout << "Configuration loaded successfully:" << std::endl;
        std::cout << "  Simulation timestep: " << timestep << std::endl;
        std::cout << "  Neuron count: " << neuronCount << std::endl;
        std::cout << "  Environment: " << envName << std::endl;
        
        // Save configuration
        config.saveToFile("output_config.cfg");
        
        // Print configuration report
        std::cout << "\n" << config.getConfigurationReport();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

### Advanced Configuration with Multiple Sources

```cpp
#include <nlm.h>

int main(int argc, char** argv) {
    nlm::Config config;
    
    // Load default configuration
    config.configureFromFile("configs/default.cfg");
    
    // Override with command line arguments
    config.configureFromCommandLine(argc, argv);
    
    // Add runtime settings
    config.setDouble("simulation_time_limit", 10.0, nlm::ConfigSource::Runtime);
    config.setBool("visualization_enabled", true, nlm::ConfigSource::Runtime);
    
    // Get configuration report
    std::cout << config.getConfigurationReport();
    
    return 0;
}
```

### Configuration Validation and Error Handling

```cpp
#include <nlm.h>
#include <stdexcept>

void validateSimulationConfig(const nlm::Config& config) {
    // Required parameters
    if (!config.has("simulation_timestep")) {
        throw std::runtime_error("Missing simulation_timestep");
    }
    
    if (!config.has("neuron_count")) {
        throw std::runtime_error("Missing neuron_count");
    }
    
    if (!config.has("region_count")) {
        throw std::runtime_error("Missing region_count");
    }
    
    // Validate parameter values
    double timestep = config.getDouble("simulation_timestep");
    if (timestep <= 0.0 || timestep > 1.0) {
        throw std::runtime_error("Invalid simulation_timestep");
    }
    
    int neuronCount = config.getInt("neuron_count");
    if (neuronCount <= 0) {
        throw std::runtime_error("neuron_count must be positive");
    }
    
    int regionCount = config.getInt("region_count");
    if (regionCount <= 0) {
        throw std::runtime_error("region_count must be positive");
    }
    
    // Check connectivity consistency
    double connectionProb = config.getOr("connection_probability", 0.1);
    if (connectionProb < 0.0 || connectionProb > 1.0) {
        throw std::runtime_error("connection_probability must be in [0, 1]");
    }
}

int main() {
    nlm::Config config;
    
    try {
        // Load configuration
        config.configureFromFile("configs/default.cfg");
        
        // Validate
        validateSimulationConfig(config);
        
        // Save if valid
        config.saveToFile("validated_config.cfg");
        
        std::cout << "Configuration validated and saved successfully!" << std::endl;
        std::cout << config.getConfigurationReport();
        
    } catch (const std::exception& e) {
        std::cerr << "Configuration validation failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

## Migration Guide

### From Phase 1 to Current Version

**Key Changes:**

1. **Configuration System**: 
   - Old: Simple key=value file parsing only
   - New: Auto-detection of JSON/YAML/plain text formats

2. **STDP Implementation**:
   - Old: Placeholder implementation
   - New: Full STDP with reward modulation and triplet interactions

3. **Error Handling**:
   - Old: Minimal error checking
   - New: Comprehensive validation with detailed error messages

4. **API Design**:
   - Old: Limited, inconsistent methods
   - New: Consistent template-based API with type safety

**Migration Steps:**

```cpp
// OLD (Phase 1)
bool loadFromFile(const std::string& filepath);
void set(const std::string& key, const std::string& value);
std::string get(const std::string& key);

// NEW (Current)
bool loadFromFile(const std::string& filepath);
bool loadFromJSONFile(const std::string& filepath);
bool loadFromYAMLFile(const std::string& filepath);
void set(const std::string& key, const ConfigValue& value, ConfigSource source);
template<typename T>
std::optional<T> get(const std::string& key) const;
bool validate() const;
```

### Configuration File Migration

```ini
// OLD: default.cfg (Phase 1)
# Simple key=value pairs
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
dopamine_baseline = 0.0
dopamine_baseline = 0.1  // DUPLICATE KEY!
reward_discount_factor = 0.99
environment_name = GridWorld
environment_width = 10
environment_height = 10
log_level = INFO
log_to_file = false
max_simulation_steps = 10000
```

```json
// NEW: config.json (Current)
{
    "simulation": {
        "timestep": 0.001,
        "max_steps": 10000,
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1
    },
    "plasticity": {
        "learning_rate": 0.01,
        "stdp": {
            "ltp_weight": 0.01,
            "ltd_weight": 0.012,
            "time_constant": 20.0
        }
    },
    "development": {
        "synaptogenesis_rate": 0.001,
        "pruning_rate": 0.0001
    },
    "neuromodulation": {
        "dopamine_baseline": 0.1,
        "reward_discount_factor": 0.99
    },
    "environment": {
        "name": "GridWorld",
        "width": 10,
        "height": 10
    },
    "logging": {
        "level": "INFO",
        "to_file": false,
        "filename": "nlm.log"
    }
}
```

## Best Practices

### Configuration Management

1. **Use Configuration Templates**: Create specialized configuration templates for different use cases.

2. **Validate Early**: Validate configuration as early as possible to catch errors quickly.

3. **Use Configuration Sources Appropriately**:
   - Default: System-wide defaults
   - File: Environment-specific settings
   - Command-line: Runtime overrides
   - Runtime: Dynamic settings

4. **Document Configuration**: Keep configuration documentation in sync with actual parameters.

### Error Handling

```cpp
try {
    config.loadFromFile("config.json");
    config.validate();
} catch (const std::exception& e) {
    std::cerr << "Configuration error: " << e.what() << std::endl;
    return 1;
}
```

### Performance Considerations

1. **Lazy Loading**: Load configuration only when needed.
2. **Caching**: Cache frequently accessed configuration values.
3. **Validation**: Run validation in parallel with other startup tasks.

### Testing Configuration

```cpp
// Test configuration loading
TEST(ConfigTest, LoadFromFile) {
    nlm::Config config;
    EXPECT_TRUE(config.loadFromFile("configs/default.cfg"));
    EXPECT_TRUE(config.has("neuron_count"));
    EXPECT_EQ(config.getInt("neuron_count"), 1000);
}

TEST(ConfigTest, ValidateValidConfig) {
    nlm::Config config;
    config.setInt("neuron_count", 1000);
    config.setDouble("simulation_timestep", 0.001);
    EXPECT_TRUE(config.validate());
}

TEST(ConfigTest, ValidateInvalidConfig) {
    nlm::Config config;
    config.setDouble("simulation_timestep", -1.0); // Invalid
    EXPECT_FALSE(config.validate());
}
```

## Troubleshooting

### Common Issues

1. **Configuration not loading**:
   - Check file path and permissions
   - Ensure file format matches detected type
   - Check for syntax errors in JSON/YAML

2. **Missing configuration values**:
   - Use default values with `getOr()`
   - Validate configuration with `validate()`

3. **Parameter validation failures**:
   - Check configuration report for specific validation errors
   - Ensure parameter ranges are within valid bounds

### Debug Configuration

```cpp
void debugConfiguration(const nlm::Config& config) {
    std::cout << "=== Configuration Debug ===" << std::endl;
    std::cout << "Total entries: " << config.getKeys().size() << std::endl;
    
    std::cout << "\nAll keys:" << std::endl;
    for (const auto& key : config.getKeys()) {
        std::cout << "  - " << key << std::endl;
    }
    
    std::cout << "\nConfiguration summary:" << std::endl;
    std::cout << config.summary();
    
    std::cout << "\nValidation result:" << std::endl;
    std::cout << (config.validate() ? "PASSED" : "FAILED") << std::endl;
    
    if (!config.validate()) {
        std::cout << "\nFull report:" << std::endl;
        std::cout << config.getConfigurationReport();
    }
}
```