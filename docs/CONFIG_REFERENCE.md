# NLM Configuration Reference

## Overview

This document provides a comprehensive reference for NLM configuration options, covering all settings for brain simulation, plasticity, neuromodulation, environment interaction, and logging. Configuration can be specified in both simple `key=value` format (legacy) and modern JSON format.

## Configuration Files

### Simple Format (`.cfg` files)

```cfg
# NLM Configuration File
# This is the default configuration for Phase 1 skeleton

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

### JSON Format (`.json` files)

```json
{
  "brain": {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1
  },
  "plasticity": {
    "learning_rate": 0.01,
    "stdp": {
      "ltp_weight": 0.02,
      "ltd_weight": 0.015,
      "time_constant": 20.0
    },
    "synaptogenesis_rate": 0.001,
    "pruning_rate": 0.0001
  },
  "neuromodulation": {
    "dopamine_baseline": 0.1,
    "curiosity": {
      "threshold": 0.05,
      "decay": 0.99
    },
    "novelty": {
      "decay": 0.98
    },
    "prediction_error": {
      "scaling": 1.0
    }
  },
  "reward": {
    "discount_factor": 0.99,
    "exploration_bonus": 0.1
  },
  "environment": {
    "name": "GridWorld",
    "dimensions": {
      "width": 10,
      "height": 10
    },
    "sensory": {
      "vision": {
        "width": 8,
        "height": 8
      }
    }
  },
  "logging": {
    "level": "INFO",
    "to_file": false,
    "filename": "nlm.log",
    "file_rotations": 5,
    "max_size_mb": 100
  },
  "simulation": {
    "max_steps": 10000,
    "time_limit_seconds": 0.0,
    "checkpoint_interval_steps": 1000
  },
  "visualization": {
    "enabled": false,
    "update_rate_hz": 30.0,
    "show_neurons": true,
    "show_synapses": true,
    "window_width": 1280,
    "window_height": 720
  }
}
```

## Configuration Categories

### Brain Configuration

#### Core Brain Parameters
| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `neuron_count` | integer | 1000 | Total number of neurons | [100, 100000] |
| `region_count` | integer | 1 | Number of brain regions | [1, 100] |
| `connection_probability` | float | 0.1 | Probability of random connections | [0.0, 1.0] |
| `simulation_timestep` | float | 0.001 | Simulation timestep in seconds | [0.0001, 0.01] |

#### Neural Dynamics Parameters
| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `lif_leak_coefficient` | float | 0.1 | Membrane potential leak coefficient | [0.01, 1.0] |
| `lif_threshold` | float | -55.0 | Firing threshold (mV) | [-100.0, -20.0] |
| `lif_reset_potential` | float | -70.0 | Reset potential after spike (mV) | [-80.0, -60.0] |
| `lif_refractory_period` | float | 0.002 | Refractory period duration (s) | [0.001, 0.01] |

### Plasticity Configuration

#### Hebbian Plasticity
| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `plasticity_learning_rate` | float | 0.01 | Base Hebbian learning rate | [0.001, 0.1] |
| `hebbian_weight_increment` | float | 0.01 | Weight increment per co-activation | [0.001, 0.05] |
| `hebbian_weight_decay` | float | 0.995 | Weight decay per timestep | [0.99, 0.999] |

#### STDP (Spike-Timing-Dependent Plasticity)
| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `stdp_ltp_weight` | float | 0.02 | LTP weight change magnitude | [0.01, 0.05] |
| `stdp_ltd_weight` | float | 0.015 | LTD weight change magnitude | [0.01, 0.03] |
| `stdp_time_constant` | float | 20.0 | STDP time constant (ms) | [10.0, 50.0] |
| `stdp_trace_decay` | float | 0.95 | STDP trace decay rate | [0.9, 0.99] |
| `stdp_pre_trace_duration` | float | 100.0 | Pre-synaptic trace duration (ms) | [50.0, 200.0] |
| `stdp_post_trace_duration` | float | 80.0 | Post-synaptic trace duration (ms) | [40.0, 160.0] |

#### Structural Plasticity
| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `synaptogenesis_rate` | float | 0.001 | Probability of new synapse formation | [0.0001, 0.01] |
| `pruning_rate` | float | 0.0001 | Probability of synapse removal | [0.00001, 0.001] |
| `min_synapse_weight` | float | 0.01 | Minimum weight for synapse survival | [0.001, 0.05] |
| `max_synapse_weight` | float | 1.0 | Maximum weight for synapse | [0.5, 2.0] |

### Development Configuration

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `development_synaptogenesis_rate` | float | 0.001 | Age-dependent synaptogenesis rate | [0.0001, 0.01] |
| `development_pruning_rate` | float | 0.0001 | Age-dependent pruning rate | [0.00001, 0.001] |
| `development_maturation_speed` | float | 1.0 | Rate of developmental maturation | [0.1, 10.0] |
| `critical_period_start` | float | 100.0 | Age when critical period starts (steps) | [10.0, 500.0] |
| `critical_period_end` | float | 1000.0 | Age when critical period ends (steps) | [100.0, 5000.0] |
| `plasticity_during_critical_period` | float | 2.0 | Plasticity multiplier during critical period | [1.0, 10.0] |

### Neuromodulation Configuration

#### Dopamine System
| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `dopamine_baseline` | float | 0.1 | Baseline dopamine level | [0.0, 1.0] |
| `dopamine_reward_gain` | float | 1.0 | Reward prediction error gain | [0.5, 2.0] |
| `dopamine_error_decay` | float | 0.95 | Dopamine error decay rate | [0.9, 0.99] |
| `dopamine_phasing` | float | 0.1 | Dopamine signal phasing (s) | [0.05, 0.2] |

#### Curiosity System
| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `curiosity_threshold` | float | 0.05 | Threshold for curiosity-driven exploration | [0.01, 0.1] |
| `curiosity_decay_rate` | float | 0.99 | Curiosity signal decay rate | [0.95, 0.999] |
| `curiosity_exponent` | float | 2.0 | Exponent for curiosity computation | [1.0, 5.0] |
| `curiosity_prediction_weight` | float | 0.5 | Weight of prediction error in curiosity | [0.1, 1.0] |
| `curiosity_novelty_weight` | float | 0.5 | Weight of novelty in curiosity | [0.1, 1.0] |

#### Novelty Detection
| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `novelty_decay` | float | 0.98 | Novelty signal decay rate | [0.95, 0.999] |
| `novelty_sensitivity` | float | 1.0 | Sensitivity to novelty | [0.5, 2.0] |
| `novelty_window_size` | integer | 100 | Number of previous states for novelty comparison | [10, 500] |

#### Prediction Error
| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `prediction_error_scaling` | float | 1.0 | Scaling factor for prediction error | [0.1, 10.0] |
| `prediction_error_decay` | float | 0.9 | Prediction error decay rate | [0.8, 0.95] |
| `prediction_confidence_threshold` | float | 0.5 | Threshold for prediction confidence | [0.1, 0.9] |

### Reward Configuration

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `reward_discount_factor` | float | 0.99 | Discount factor for future rewards | [0.9, 0.999] |
| `reward_exploration_bonus` | float | 0.1 | Bonus for exploration behavior | [0.0, 1.0] |
| `reward_punishment_factor` | float | 0.5 | Weight of punishments vs rewards | [0.1, 2.0] |
| `reward_stability` | float | 0.1 | Reward signal stability | [0.01, 0.5] |

### Environment Configuration

#### Grid World Parameters
| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `environment_width` | integer | 10 | Width of grid environment | [5, 50] |
| `environment_height` | integer | 10 | Height of grid environment | [5, 50] |
| `environment_obstacle_density` | float | 0.1 | Density of obstacles | [0.0, 0.5] |
| `environment_reward_for_goal` | float | 1.0 | Reward for reaching goal | [0.0, 10.0] |
| `environment_reward_for_obstacle` | float | -0.1 | Penalty for hitting obstacle | [-1.0, 0.0] |

#### Sensory Processing
| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `vision_width` | integer | 8 | Width of visual field | [4, 20] |
| `vision_height` | integer | 8 | Height of visual field | [4, 20] |
| `vision_field_of_view` | float | 180.0 | Field of view angle (degrees) | [90.0, 360.0] |
| `vision_noise_level` | float | 0.01 | Noise level in sensory input | [0.0, 0.1] |
| `audio_sensitivity` | float | 0.5 | Sensitivity to audio input | [0.1, 1.0] |

### Logging Configuration

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `log_level` | string | "INFO" | Minimum log level | "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL" |
| `log_to_file` | boolean | false | Whether to log to file | true, false |
| `log_filename` | string | "nlm.log" | Log file name | Any valid filename |
| `log_file_rotations` | integer | 5 | Number of rotated log files to keep | [1, 100] |
| `log_max_size_mb` | integer | 100 | Maximum log file size before rotation (MB) | [10, 1000] |
| `log_include_timestamps` | boolean | true | Include timestamps in log messages | true, false |
| `log_include_thread_ids` | boolean | true | Include thread IDs in log messages | true, false |

### Simulation Configuration

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `max_simulation_steps` | integer | 10000 | Maximum number of simulation steps | [100, 1000000] |
| `simulation_time_limit` | float | 0.0 | Maximum simulation time (seconds) | [0.0, 3600.0] |
| `checkpoint_interval_steps` | integer | 1000 | Interval between checkpoint saves | [100, 10000] |
| `checkpoint_save_time` | boolean | true | Include simulation time in checkpoints | true, false |
| `checkpoint_compress` | boolean | true | Compress checkpoint files | true, false |
| `simulation_random_seed` | integer | 42 | Random seed for simulation | [0, 2^31-1] |

### Visualization Configuration

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `visualization_enabled` | boolean | false | Enable/disable visualization | true, false |
| `visualization_update_rate_hz` | float | 30.0 | Visualization update rate (Hz) | [1.0, 120.0] |
| `show_neurons` | boolean | true | Display neurons in visualization | true, false |
| `show_synapses` | boolean | true | Display synapses in visualization | true, false |
| `show_spikes` | boolean | true | Display spike activity | true, false |
| `show_weights` | boolean | true | Display synaptic weights | true, false |
| `show_memory` | boolean | true | Display memory systems | true, false |
| `window_width` | integer | 1280 | Visualization window width | [640, 3840] |
| `window_height` | integer | 720 | Visualization window height | [480, 2160] |
| `neuron_radius` | float | 3.0 | Radius of neuron visualization | [1.0, 10.0] |
| `synapse_line_width` | float | 1.0 | Width of synapse visualization | [0.5, 3.0] |

## Configuration File Locations

### Default Configuration
- **Simple format**: `configs/default.cfg`
- **JSON format**: `configs/default.json`

### Example Configurations
- **Minimal setup**: `configs/minimal.json`
- **Complex simulation**: `configs/complex.json`
- **Development testing**: `configs/development.json`
- **Performance testing**: `configs/performance.json`

### Configuration Overrides
- **Command line**: Use `--config filename.cfg` or `--config filename.json`
- **Command line parameters**: Use `--key=value` or `--key value`
- **Environment variables**: `NLM_CONFIG_PATH` points to configuration directory

## Configuration Validation

### Type Validation
All configuration values are validated for type compatibility:

```cpp
// Example validation function
bool Config::validate(const std::string& key, const ConfigValue& value) {
    // Check type compatibility with expected parameter
    if (key == "neuron_count" && !std::holds_alternative<int64_t>(value)) {
        return false;
    }
    if (key == "connection_probability" && !std::holds_alternative<double>(value)) {
        return false;
    }
    // Additional validation...
    return true;
}
```

### Range Validation
Configuration values are validated for reasonable ranges:

```cpp
// Example range validation
bool Config::validateRange(const std::string& key, int64_t value) {
    if (key == "neuron_count" && (value < 100 || value > 100000)) {
        return false;
    }
    if (key == "region_count" && (value < 1 || value > 100)) {
        return false;
    }
    return true;
}

bool Config::validateRange(const std::string& key, double value) {
    if (key == "connection_probability" && (value < 0.0 || value > 1.0)) {
        return false;
    }
    if (key == "simulation_timestep" && (value < 0.0001 || value > 0.01)) {
        return false;
    }
    return true;
}
```

### Conflict Detection
Configuration conflicts are detected:

```cpp
// Example conflict detection
bool Config::detectConflicts() {
    // Check for contradictory settings
    if (has("log_level", "DEBUG") && has("log_max_size_mb", 10)) {
        // Debug logging with very small log files may cause performance issues
        logWarning("Debug logging with small log files may impact performance");
    }
    
    // Check for incompatible simulation settings
    if (has("max_simulation_steps", 10000) && has("checkpoint_interval_steps", 1000)) {
        // This configuration creates many checkpoints
        logInfo("Configuration will create many checkpoint files");
    }
    
    return true;
}
```

## Configuration Examples

### Minimal Configuration

```json
{
  "brain": {
    "neuron_count": 100,
    "region_count": 1,
    "connection_probability": 0.05
  },
  "environment": {
    "name": "GridWorld",
    "width": 5,
    "height": 5
  },
  "logging": {
    "level": "WARNING",
    "to_file": false
  }
}
```

### Complex Configuration

```json
{
  "brain": {
    "neuron_count": 5000,
    "region_count": 10,
    "connection_probability": 0.15,
    "lif_leak_coefficient": 0.05,
    "lif_threshold": -50.0,
    "lif_reset_potential": -65.0
  },
  "plasticity": {
    "learning_rate": 0.005,
    "stdp": {
      "ltp_weight": 0.03,
      "ltd_weight": 0.02,
      "time_constant": 25.0
    },
    "synaptogenesis_rate": 0.002,
    "pruning_rate": 0.0002
  },
  "neuromodulation": {
    "dopamine_baseline": 0.2,
    "curiosity": {
      "threshold": 0.03,
      "decay_rate": 0.98
    }
  },
  "environment": {
    "name": "GridWorld",
    "width": 20,
    "height": 20,
    "vision": {
      "width": 12,
      "height": 12
    }
  },
  "simulation": {
    "max_steps": 50000,
    "checkpoint_interval_steps": 5000
  },
  "visualization": {
    "enabled": true,
    "update_rate_hz": 60
  }
}
```

### Development Configuration

```cfg
debug_mode = true
log_level = DEBUG
log_to_file = true
log_filename = nlm_debug.log
log_file_rotations = 10
log_max_size_mb = 500

neuron_count = 500
region_count = 2
connection_probability = 0.08

plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.02
stdp_time_constant = 15.0

development_synaptogenesis_rate = 0.003
development_pruning_rate = 0.0003
development_maturation_speed = 1.5

environment_width = 8
environment_height = 8
vision_width = 6
vision_height = 6

max_simulation_steps = 20000
checkpoint_interval_steps = 2000
simulation_time_limit = 300.0

visualization_enabled = false
```

## Configuration Command Line Interface

### Setting Configuration via Command Line

```bash
# Load configuration from file
./nlm --config configs/default.cfg

# Override specific configuration values
./nlm --config configs/default.cfg --neuron_count=2000 --log_level=DEBUG

# Use JSON configuration
./nlm --config configs/complex.json

# Set multiple configuration values
./nlm --neuron_count=1000 --region_count=5 --log_level=INFO

# Use short form for single-dash options
./nlm -n 2000 -r 5 -l INFO
```

### Configuration File Format for Command Line

```bash
# Using equals sign
./nlm --random_seed=42 --simulation_timestep=0.01

# Using separate values
./nlm --random_seed 42 --simulation_timestep 0.01

# Using short options
./nlm -rs 123 -st 0.005

# Using environment variables
NLM_CONFIG_PATH=./configs/
./nlm
```

## Configuration Tools

### Configuration File Validator

```cpp
// Example configuration validator
class ConfigValidator {
public:
    bool validate(const std::string& configFile) {
        auto config = JSONParser::parseFile(configFile);
        if (!config) {
            logError("Failed to parse configuration file: " + configFile);
            return false;
        }
        
        return validateConfig(*config);
    }
    
private:
    bool validateConfig(const JSONValue& config) {
        if (!std::holds_alternative<std::map<std::string, JSONValue>>(config)) {
            logError("Configuration must be a JSON object");
            return false;
        }
        
        const auto& configObj = std::get<std::map<std::string, JSONValue>>(config);
        
        // Validate required sections
        std::vector<std::string> requiredSections = {"brain", "environment"};
        for (const auto& section : requiredSections) {
            if (configObj.find(section) == configObj.end()) {
                logWarning("Configuration section missing: " + section);
            }
        }
        
        // Validate individual values
        return validateBrainConfig(configObj) &&
               validateEnvironmentConfig(configObj) &&
               validatePlasticityConfig(configObj);
    }
};
```

### Configuration Generator

```cpp
// Example configuration generator
class ConfigGenerator {
public:
    std::string generateComplexConfig(const ConfigGenerationParams& params) {
        std::map<std::string, JSONValue> config;
        
        // Brain configuration
        config["brain"] = generateBrainConfig(params);
        
        // Plasticity configuration
        config["plasticity"] = generatePlasticityConfig(params);
        
        // Neuromodulation configuration
        config["neuromodulation"] = generateNeuromodulationConfig(params);
        
        // Environment configuration
        config["environment"] = generateEnvironmentConfig(params);
        
        // Logging configuration
        config["logging"] = generateLoggingConfig(params);
        
        // Simulation configuration
        config["simulation"] = generateSimulationConfig(params);
        
        // Visualization configuration
        config["visualization"] = generateVisualizationConfig(params);
        
        return JSONParser::serialize(config, 0);
    }
    
private:
    // Implementation of individual configuration generators...
};
```

## Configuration Migration

### Migrating from Simple to JSON Format

```cpp
// Example migration function
bool migrateConfig(const std::string& oldConfigFile, const std::string& newConfigFile) {
    // Parse old configuration
    auto oldConfig = JSONParser::parseFile(oldConfigFile);
    if (!oldConfig) {
        // Fall back to simple format parsing
        return parseSimpleConfigToJSON(oldConfigFile, newConfigFile);
    }
    
    // Transform to new format
    auto newConfig = transformToNewFormat(*oldConfig);
    
    // Write new configuration
    std::ofstream file(newConfigFile);
    if (!file.is_open()) {
        return false;
    }
    
    file << JSONParser::serialize(newConfig, 0);
    return true;
}
```

## Configuration Performance Considerations

### Large Configuration Files

```cpp
// Configuration optimization for large networks
struct OptimizedConfig {
    // Use integer IDs for large numbers to reduce memory
    int64_t neuron_count;
    
    // Use compressed strings for long parameters
    std::string environment_name;
    
    // Use efficient data structures
    std::vector<std::pair<std::string, JSONValue>> custom_settings;
    
    // Use lazy loading for large files
    bool load_on_demand;
};
```

### Memory Efficiency

```cpp
// Memory-efficient configuration access
class MemoryEfficientConfig {
public:
    std::optional<JSONValue> get(const std::string& key) const {
        // Use move semantics for large values
        if (cached_values.find(key) != cached_values.end()) {
            return cached_values.at(key);
        }
        
        // Load on demand for large values
        if (shouldLoadLazy(key)) {
            auto value = loadFromFileLazy(key);
            cached_values[key] = std::move(value);
            return cached_values[key];
        }
        
        return std::nullopt;
    }
    
private:
    std::map<std::string, JSONValue> cached_values;
    std::set<std::string> lazy_load_keys;
};
```

## Configuration Security Considerations

### Sensitive Configuration Data

```cpp
// Configuration security considerations
struct SecureConfig {
    // Store sensitive data in environment variables
    std::string getDatabasePassword() {
        const char* envPassword = std::getenv("NLM_DATABASE_PASSWORD");
        return envPassword ? std::string(envPassword) : "";
    }
    
    // Use encrypted configuration files
    bool encryptConfigFile(const std::string& inputPath, const std::string& outputPath) {
        // Implement encryption/decryption
        return true;
    }
    
    // Validate configuration integrity
    bool validateConfigIntegrity(const std::string& configFile) {
        // Compute hash and verify
        return true;
    }
};
```

## Configuration Monitoring and Analytics

### Configuration Usage Tracking

```cpp
// Configuration usage monitoring
class ConfigMonitor {
public:
    void trackConfigUsage(const std::string& configFile, const ConfigUsage& usage) {
        // Log configuration access patterns
        usage_log_.push_back({std::chrono::system_clock::now(), configFile, usage});
    }
    
    std::vector<ConfigUsage> getConfigUsage(const std::string& configFile, 
                                          const std::chrono::system_clock::time_point& start,
                                          const std::chrono::system_clock::time_point& end) {
        // Analyze configuration usage over time
        return analyzeUsage(usage_log_, configFile, start, end);
    }
    
private:
    std::vector<ConfigUsage> usage_log_;
};
```

## Configuration Testing

### Configuration Unit Tests

```cpp
// Example configuration tests
TEST(ConfigTest, JSONParsing) {
    std::string jsonConfig = R"({"neuron_count": 1000, "log_level": "INFO"})";
    JSONValue config;
    EXPECT_TRUE(JSONParser::parse(jsonConfig, config));
    
    EXPECT_TRUE(std::holds_alternative<int64_t>(std::get<std::map<std::string, JSONValue>>(config)["neuron_count"]));
    EXPECT_EQ(1000, std::get<std::map<std::string, JSONValue>>(config)["neuron_count"].get<int64_t>());
    
    EXPECT_TRUE(std::holds_alternative<std::string>(std::get<std::map<std::string, JSONValue>>(config)["log_level"]));
    EXPECT_EQ("INFO", std::get<std::map<std::string, JSONValue>>(config)["log_level"].get<std::string>());
}

TEST(ConfigTest, ConfigValidation) {
    Config config;
    config.set("neuron_count", 1000);
    config.set("connection_probability", 0.1);
    
    EXPECT_TRUE(config.has("neuron_count"));
    EXPECT_TRUE(config.has("connection_probability"));
    EXPECT_FALSE(config.has("unknown_parameter"));
    
    EXPECT_EQ(1000, config.getOr<int64_t>("neuron_count", 0));
    EXPECT_DOUBLE_EQ(0.1, config.getOr<double>("connection_probability", 0.0));
    EXPECT_EQ("default", config.getOr<std::string>("unknown_key", "default"));
}

TEST(ConfigTest, ConfigFileIO) {
    Config config;
    config.set("neuron_count", 500);
    config.set("log_level", "DEBUG");
    
    std::string testFile = "/tmp/test_config.json";
    EXPECT_TRUE(config.saveToFile(testFile));
    
    Config config2;
    EXPECT_TRUE(config2.loadFromFile(testFile));
    EXPECT_EQ(500, config2.getOr<int64_t>("neuron_count", 0));
    EXPECT_EQ("DEBUG", config2.getOr<std::string>("log_level", "INFO"));
    
    std::remove(testFile.c_str());
}
```

## Configuration Best Practices

### 1. Use Consistent Naming Conventions

```cpp
// Use consistent naming for configuration parameters
struct Config {
    // Use snake_case for all parameter names
    int64_t neuron_count;
    float connection_probability;
    std::string log_level;
    
    // Use descriptive names that indicate purpose
    bool enable_curiosity;
    double plasticity_learning_rate;
};// Consistent naming makes configuration files easier to read and maintain
```

### 2. Document All Configuration Parameters

```cpp
// Document all configuration parameters
struct ConfigDocumentation {
    static const std::map<std::string, std::string> parameterDocumentation {
        {"neuron_count", "Total number of neurons in the brain [100, 100000]"},
        {"connection_probability", "Probability of random connections between neurons [0.0, 1.0]"},
        {"log_level", "Minimum logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)"},
        {"simulation_timestep", "Simulation timestep in seconds [0.0001, 0.01]"},
        // Add all other parameters...
    };
    
    static std::string getDocumentation(const std::string& parameter) {
        auto it = parameterDocumentation.find(parameter);
        return it != parameterDocumentation.end() ? it->second : "Unknown parameter";
    }
};
```

### 3. Validate Configuration Parameters

```cpp
// Validate configuration parameters before use
bool validateConfig(const Config& config) {
    bool isValid = true;
    
    // Validate ranges
    if (config.neuron_count < 100 || config.neuron_count > 100000) {
        logError("Invalid neuron_count: " + std::to_string(config.neuron_count));
        isValid = false;
    }
    
    if (config.connection_probability < 0.0 || config.connection_probability > 1.0) {
        logError("Invalid connection_probability: " + std::to_string(config.connection_probability));
        isValid = false;
    }
    
    // Validate enum values
    std::vector<std::string> validLogLevels = {"DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"};
    if (std::find(validLogLevels.begin(), validLogLevels.end(), config.log_level) == validLogLevels.end()) {
        logError("Invalid log_level: " + config.log_level);
        isValid = false;
    }
    
    return isValid;
}
```

### 4. Use Default Values for Optional Parameters

```cpp
// Use sensible defaults for optional parameters
struct Config {
    // Required parameters
    int64_t neuron_count = 1000;
    
    // Optional parameters with defaults
    int64_t region_count = 1;
    float connection_probability = 0.1;
    std::string log_level = "INFO";
    float simulation_timestep = 0.001;
    bool visualization_enabled = false;
    
    // Optional parameters with more complex defaults
    int max_simulation_steps = 10000;
    float simulation_time_limit = 0.0;
    
    // Optional parameters with conditional defaults
    int checkpoint_interval_steps = max_simulation_steps / 10;
    
    // Validation for dependent parameters
    bool validate() const {
        if (region_count < 1 || region_count > 100) {
            return false;
        }
        if (connection_probability < 0.0 || connection_probability > 1.0) {
            return false;
        }
        if (simulation_timestep <= 0.0) {
            return false;
        }
        return true;
    }
};
```

### 5. Provide Configuration Examples

```cpp
// Provide configuration examples for common use cases
struct ConfigExamples {
    static Config getMinimalConfig() {
        Config config;
        config.neuron_count = 100;
        config.region_count = 1;
        config.connection_probability = 0.05;
        config.log_level = "WARNING";
        config.visualization_enabled = false;
        return config;
    }
    
    static Config getDevelopmentConfig() {
        Config config;
        config.neuron_count = 500;
        config.region_count = 2;
        config.connection_probability = 0.08;
        config.log_level = "DEBUG";
        config.log_to_file = true;
        config.log_filename = "nlm_debug.log";
        config.visualization_enabled = false;
        config.max_simulation_steps = 20000;
        config.simulation_time_limit = 300.0;
        return config;
    }
    
    static Config getProductionConfig() {
        Config config;
        config.neuron_count = 5000;
        config.region_count = 10;
        config.connection_probability = 0.15;
        config.log_level = "INFO";
        config.log_to_file = true;
        config.log_filename = "nlm.log";
        config.visualization_enabled = true;
        config.max_simulation_steps = 100000;
        config.simulation_time_limit = 3600.0;
        config.checkpoint_interval_steps = 10000;
        return config;
    }
};
```

## Conclusion

The NLM configuration system provides flexible, powerful configuration options for controlling brain simulation behavior. By supporting both simple and JSON formats, providing comprehensive validation, and including extensive documentation, users can easily configure NLM for a wide range of experimental scenarios.

Key improvements include:

1. **Enhanced Configuration Format**: JSON format support for better structure and tooling
2. **Comprehensive Documentation**: Complete reference for all configuration parameters
3. **Robust Validation**: Type and range validation for all configuration parameters
4. **Flexible Loading**: Support for both simple and JSON configuration formats
5. **Performance Optimization**: Memory-efficient configuration access and lazy loading
6. **Security Considerations**: Secure handling of sensitive configuration data
7. **Testing Support**: Comprehensive testing for configuration parsing and validation
8. **Best Practices**: Clear guidelines for configuration management

These improvements make NLM more accessible to users while maintaining the flexibility needed for advanced research scenarios.