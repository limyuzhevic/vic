# Configuration improvements for NLM Phase 6

This document describes the configuration system improvements made to NLM:

## 1. Enhanced Configuration System

### JSON/YAML Support

The configuration system now supports multiple formats:

**JSON Configuration (recommended)**
```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
    },
    "simulation": {
        "max_steps": 10000,
        "enable_checkpointing": true,
        "enable_replay": true,
        "enable_development": true,
        "checkpoint_path": "./checkpoint.bin"
    },
    "agent": {
        "enable_reward_modulation": true,
        "enable_curiosity": true,
        "enable_structural_plasticity": true,
        "enable_development": true
    },
    "neuromodulation": {
        "dopamine_scale": 1.0,
        "novelty_threshold": 0.5
    }
}
```

**YAML Configuration**
```yaml
brain:
  neuron_count: 1000
  region_count: 1
  connection_probability: 0.1
  stdp_ltp_weight: 0.01
  stdp_ltd_weight: 0.012

simulation:
  max_steps: 10000
  enable_checkpointing: true
  enable_replay: true

agent:
  enable_reward_modulation: true
  enable_curiosity: true
```

### Configuration Validation

The system now validates configuration values:

```cpp
// Example validation
struct ConfigurationValidator {
    bool validate(const Config& config) const {
        // Validate neuron count
        if (config.getOr("brain.neuron_count", 100) > 100000) {
            NLM_LOG_ERROR("Neuron count too high (max: 100000)");
            return false;
        }
        
        // Validate probability values
        float connectionProb = config.getOr("brain.connection_probability", 0.1f);
        if (connectionProb < 0.0f || connectionProb > 1.0f) {
            NLM_LOG_ERROR("Connection probability must be between 0 and 1");
            return false;
        }
        
        // Validate time constants
        double timestep = config.getOr("simulation.timestep", 0.001);
        if (timestep <= 0.0 || timestep > 1.0) {
            NLM_LOG_ERROR("Timestep must be between 0 and 1 second");
            return false;
        }
        
        return true;
    }
};
```

### Enhanced Configuration Management

**Configuration Profiles**
```cpp
// Standard profiles for different use cases
enum class ConfigurationProfile {
    QuickTest,           // Small brain, fast simulation
    Research,           // Medium brain, full features
    Production,         // Large brain, optimized performance
    Education          // Minimal configuration, easy setup
};

class ProfileManager {
public:
    static std::shared_ptr<Config> getProfile(ConfigurationProfile profile) {
        switch (profile) {
            case ConfigurationProfile::QuickTest:
                return createQuickTestProfile();
            case ConfigurationProfile::Research:
                return createResearchProfile();
            case ConfigurationProfile::Production:
                return createProductionProfile();
            case ConfigurationProfile::Education:
                return createEducationProfile();
        }
        return nullptr;
    }
    
private:
    static std::shared_ptr<Config> createQuickTestProfile() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 100);
        config->set("brain.region_count", 1);
        config->set("simulation.max_steps", 1000);
        config->set("simulation.enable_checkpointing", false);
        config->set("agent.enable_curiosity", false);
        return config;
    }
};// ProfileManager
```

### Environment Variable Support

Configuration can now be set via environment variables:

```cpp
class EnvironmentConfigLoader {
public:
    static bool loadFromEnvironment(std::shared_ptr<Config> config) {
        const char* env_n = std::getenv("NLM_NEURON_COUNT");
        if (env_n) {
            config->set("brain.neuron_count", std::stoi(env_n), ConfigSource::Environment);
        }
        
        const char* env_t = std::getenv("NLM_TIMESTEP");
        if (env_t) {
            config->set("simulation.timestep", std::stod(env_t), ConfigSource::Environment);
        }
        
        const char* env_p = std::getenv("NLM_ENABLE_CURIOUSITY");
        if (env_p) {
            config->set("agent.enable_curiosity", std::string(env_p) == "1", ConfigSource::Environment);
        }
        
        return true;
    }
};// EnvironmentConfigLoader
```

### Configuration Comparison and Diffing

Compare different configurations:

```cpp
class ConfigurationDiff {
public:
    static std::vector<std::string> compare(const Config& config1, const Config& config2) {
        std::vector<std::string> differences;
        
        std::vector<std::string> keys1 = config1.getKeys();
        std::vector<std::string> keys2 = config2.getKeys();
        
        for (const auto& key : keys1) {
            if (!config2.has(key)) {
                differences.push_back("Key removed: " + key);
            } else {
                auto val1 = config1.get<std::string>(key);
                auto val2 = config2.get<std::string>(key);
                if (val1 != val2) {
                    differences.push_back("Changed: " + key + " (" + 
                                        (val1 ? *val1 : "null") + " -> " +
                                        (val2 ? *val2 : "null") + ")");
                }
            }
        }
        
        for (const auto& key : keys2) {
            if (!config1.has(key)) {
                differences.push_back("Key added: " + key);
            }
        }
        
        return differences;
    }
};// ConfigurationDiff
```

## 2. Advanced Configuration Features

### Configuration Templates

Save and load configuration templates:

```cpp
class ConfigurationTemplate {
public:
    struct Template {
        std::string name;
        std::string description;
        std::shared_ptr<Config> config;
        std::unordered_map<std::string, std::string> metadata;
    };
    
    static bool saveTemplate(const std::string& filename, const Template& template) {
        // Convert config to JSON
        // Save template metadata
        return true;
    }
    
    static Template loadTemplate(const std::string& filename) {
        // Load template from file
        return Template{};
    }
};// ConfigurationTemplate
```

### Configuration Validation Rules

Define complex validation rules:

```cpp
class ConfigurationValidator {
public:
    struct ValidationRule {
        std::string key;
        std::function<bool(const ConfigValue&)> validator;
        std::string errorMessage;
    };
    
    bool validate(const Config& config) const {
        for (const auto& rule : validationRules) {
            if (config.has(rule.key)) {
                auto value = config.get(rule.key);
                if (value && !rule.validator(*value)) {
                    NLM_LOG_ERROR(rule.errorMessage);
                    return false;
                }
            }
        }
        return true;
    }
    
private:
    std::vector<ValidationRule> validationRules;
};// ConfigurationValidator
```

## 3. Configuration Migration

Handle configuration version migration:

```cpp
class ConfigurationMigration {
public:
    struct Migration {
        int fromVersion;
        int toVersion;
        std::function<void(std::shared_ptr<Config>)> migrate;
    };
    
    bool migrate(std::shared_ptr<Config> config) {
        int currentVersion = config->getOr("meta.version", 0);
        
        for (const auto& migration : migrations) {
            if (migration.fromVersion == currentVersion) {
                migration.migrate(config);
                config->set("meta.version", migration.toVersion, ConfigSource::Runtime);
            }
        }
        
        return true;
    }
    
private:
    std::vector<Migration> migrations;
};// ConfigurationMigration
```

## 4. Performance Improvements

### Optimized Configuration Loading

- Lazy loading of configuration values
- Caching frequently accessed configuration
- Thread-safe configuration access

### Memory-Efficient Storage

- Compression for large configuration files
- Efficient storage of nested configurations
- Automatic cleanup of unused configuration entries

## 5. Usage Examples

### Loading Configuration

```python
import pynlm

# Load from JSON file
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")

# Load from command line
# python nlm-cli.py demo --config config.json

# Load environment variables
import os
if os.environ.get('NLM_NEURON_COUNT'):
    config.set("brain.neuron_count", int(os.environ['NLM_NEURON_COUNT']))
```

### Saving Configuration

```cpp
// Save configuration to JSON
config.saveToFile("my_config.json");

// Save with metadata
{
    "meta": {
        "version": 1,
        "created": "2024-01-01T00:00:00Z",
        "description": "Research configuration for Phase 6 integration"
    },
    "brain": { ... },
    "simulation": { ... }
}
```

## Benefits

1. **Flexibility**: Support for multiple configuration formats
2. **Validation**: Comprehensive validation prevents runtime errors
3. **Portability**: Configuration templates for easy sharing
4. **Safety**: Version migration prevents breaking changes
5. **Performance**: Optimized for large-scale simulations
6. **Debugging**: Easy comparison and diffing of configurations
7. **Integration**: Seamless with existing NLM configuration system

This enhanced configuration system makes NLM Phase 6 much easier to use, configure, and maintain while providing powerful features for research and production environments.