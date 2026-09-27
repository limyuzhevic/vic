# Configuration Module - NLM Configuration System

**NLM Configuration Module Documentation**

## Overview

The Configuration Module provides comprehensive configuration management for the NLM brain project. It supports multiple file formats, validation, environment variable substitution, and section-based organization for all brain system parameters.

### Key Features

- **Multi-Format Support**: JSON, YAML, and key=value formats
- **Type Safety**: Strong typing with automatic conversion
- **Source Tracking**: Configuration origin tracking for validation and persistence
- **Environment Variables**: Dynamic value substitution
- **Validation Framework**: Runtime configuration validation
- **Section Organization**: Hierarchical parameter organization
- **Auto-Save**: Persistent configuration management

## Core Components

### Config Class
Main configuration management class supporting all operations

### ConfigValue
Union type for all supported configuration value types

### ConfigEntry
Configuration entry with metadata (source, description, etc.)

### ConfigSource
Enumeration of configuration sources (Default, File, CommandLine, Runtime)

### ConfigFileFormat
Enumeration of supported file formats (JSON, YAML, key=value)

### ValidationResult
Validation outcome with detailed error messages

## Configuration Categories

### Brain Configuration
Neural system parameters and organization:
- `brain.regions`: Number of brain regions
- `brain.region.0.size`: Neuron count per region
- `brain.region.0.neuronType`: Neuron types (Excitatory, Inhibitory, etc.)
- `brain.region.0.connectionProbability`: Synaptic connectivity probability

### Memory Configuration
Memory system settings and capacities:
- `memory.working.capacity`: Working memory capacity
- `memory.working.decayRate`: Working memory decay rate
- `memory.episodic.encodingRate`: Episodic encoding rate
- `memory.episodic.maxDuration`: Maximum episode duration
- `memory.associative.associationStrength`: Pattern association strength

### Plasticity Configuration
Learning algorithm parameters:
- `plasticity.stdp.learningRate`: STDP learning rate
- `plasticity.stdp.timeWindow`: STDP time window
- `plasticity.stdp.maxWeightChange`: Maximum weight change per spike
- `plasticity.hebbian.associationStrength`: Hebbian association strength
- `plasticity.rewardModulated.learningRate`: Reward-modulated learning rate

### Neuromodulation Configuration
Neuromodulator behaviors and responses:
- `neuromodulation.dopamine.sensitivity`: Dopamine sensitivity
- `neuromodulation.curiosity.intensity`: Curiosity drive intensity
- `neuromodulation.novelty.threshold`: Novelty detection threshold
- `neuromodulation.predictionError.weight`: Prediction error weighting

### Prediction and Cognition Configuration
Prediction and cognitive system settings:
- `prediction.system.enabled`: Enable prediction system
- `prediction.horizon`: Prediction time horizon
- `cognition.planning.horizon`: Planning horizon
- `cognition.attention.focusStrength`: Attention focus strength

### Development Configuration
Developmental stage parameters:
- `development.initialPlasticity`: Initial plasticity rate
- `development.criticalPeriodStart`: Critical period start
- `development.criticalPeriodEnd`: Critical period end
- `development.maturationRate`: Maturation rate

### Simulation Configuration
Simulation control parameters:
- `simulation.timestep`: Simulation time step (ms)
- `simulation.duration`: Total simulation duration (ms)
- `simulation.maxSteps`: Maximum number of steps
- `simulation.randomSeed`: Random seed for reproducibility

### Logging Configuration
Logging and debugging settings:
- `logging.level`: Log level (Debug, Info, Warning, Error, Critical)
- `logging.file`: Log file path
- `logging.maxSize`: Maximum log file size
- `logging.rotate`: Log rotation enabled

## Configuration File Formats

### JSON Format
```json
{
  "brain": {
    "regions": 4,
    "region.0": {
      "size": 100,
      "neuronType": "Excitatory",
      "connectionProbability": 0.1
    }
  },
  "memory": {
    "working": {
      "capacity": 1000,
      "decayRate": 0.01
    }
  },
  "plasticity": {
    "stdp": {
      "learningRate": 0.01,
      "timeWindow": 20
    }
  }
}
```

### YAML Format
```yaml
brain:
  regions: 4
  region.0:
    size: 100
    neuronType: Excitatory
    connectionProbability: 0.1
memory:
  working:
    capacity: 1000
    decayRate: 0.01
  episodic:
    encodingRate: 0.5
    maxDuration: 3600
plasticity:
  stdp:
    learningRate: 0.01
    timeWindow: 20
  hebbian:
    associationStrength: 0.1
```

### Key=Value Format
```
# brain section
brain.regions = 4
brain.region.0.size = 100
brain.region.0.neuronType = Excitatory
brain.region.0.connectionProbability = 0.1

# memory section
memory.working.capacity = 1000
memory.working.decayRate = 0.01
memory.episodic.encodingRate = 0.5

# plasticity section
plasticity.stdp.learningRate = 0.01
plasticity.stdp.timeWindow = 20
```

## Configuration Operations

### Loading
```cpp
Config config;

// Load from file (auto-detects format)
config.loadFromFile("config.json");

// Load from specific format
config.loadFromFile("config.yaml", ConfigFileFormat::YAML);
config.loadFromFile("config.keyval", ConfigFileFormat::KEY_VALUE);

// Load from JSON string
config.loadFromJSON("{\"brain\": {\"regions\": 4}}");

// Load from YAML string
config.loadFromYAML("brain:\n  regions: 4");

// Load from command line arguments
config.loadFromArgs(argc, argv);
```

### Saving
```cpp
// Save to file
config.saveToFile("output.json");
config.saveToFile("output.yaml", ConfigFileFormat::YAML);

// Save to string
std::string jsonStr = config.saveToString(ConfigFileFormat::JSON);
std::string yamlStr = config.saveToString(ConfigFileFormat::YAML);
```

### Accessing Values
```cpp
Config config;
config.loadFromFile("config.json");

// Type-safe access with optional return
std::optional<int> regions = config.get<int>("brain.regions");
std::optional<double> rate = config.get<double>("memory.working.decayRate");
std::optional<std::string> type = config.get<std::string>("brain.region.0.neuronType");

// Default value access
int defaultRegions = config.getOr<int>("brain.regions", 4);
double defaultRate = config.getOr<double>("memory.working.decayRate", 0.01);
```

### Setting Values
```cpp
Config config;

// Set with specific source
config.set("brain.regions", 4, ConfigSource::File);
config.set("memory.working.capacity", 1000, ConfigSource::Runtime);

// Type-specific setters
config.set("brain.region.0.size", 100);
config.set("memory.working.decayRate", 0.01);
config.set("plasticity.stdp.learningRate", 0.01);
config.set("neuromodulation.dopamine.sensitivity", 1.0);

// Override with validation
config.setValidated("brain.regions", 4, ConfigSource::Runtime, 
                   [](const std::string& key, const ConfigValue& value) {
   if (value.get<int>() < 1) {
       return ValidationResult(false, "Regions must be at least 1");
   }
   return ValidationResult(true);
});
```

### Section Management
```cpp
Config config;

// Check section existence
if (config.hasSection("brain")) {
    std::vector<std::string> keys = config.getKeys("brain");
}

// Get section value
ConfigValue value = config.getSectionValue("brain", "regions");

// Remove section
config.removeSection("brain");

// Get section entries
std::vector<ConfigEntry> entries = config.getSectionEntries("brain");
```

### Validation and Locking
```cpp
Config config;

// Set custom validator
bool validator(const std::string& key, const ConfigValue& value) {
    if (key == "brain.regions" && value.get<int>() < 1) {
        return false;
    }
    return true;
}

config.setValidator(validator);

// Lock configuration key
config.lock("brain.regions", true);

// Check if key is locked
bool isLocked = config.isLocked("brain.regions");
```

### Environment Variable Substitution
```cpp
Config config;

// Environment variables in configuration strings
config.set("brain.randomSeed", "${RANDOM_SEED:-42}");
config.set("memory.working.capacity", "${MEMORY_CAPACITY:-1000}");

// Static substitution (without runtime)
std::string substituted = Config::substituteVariables("Value: ${VAR:-default}");
```

## Validation System

### Validation Framework
The configuration system includes a comprehensive validation framework:

1. **Built-in Validations**: Type checking, range validation
2. **Custom Validators**: User-defined validation functions
3. **Validation Results**: Detailed error reporting with messages
4. **Validation Errors**: Configuration rollback on validation failure

### Validation Examples
```cpp
// Range validation
auto rangeValidator = [](const std::string& key, const ConfigValue& value) {
    if (key == "brain.regions" && value.get<int>() < 1) {
        return ValidationResult(false, "Regions must be at least 1");
    }
    if (key == "memory.working.capacity" && value.get<int>() > 10000) {
        return ValidationResult(false, "Working memory capacity too large");
    }
    return ValidationResult(true, "Validation passed");
};

config.setValidator(rangeValidator);

// Key existence validation
auto existenceValidator = [](const std::string& key, const ConfigValue& value) {
    std::vector<std::string> requiredKeys = {
        "brain.regions",
        "memory.working.capacity",
        "plasticity.stdp.learningRate"
    };
    
    if (std::find(requiredKeys.begin(), requiredKeys.end(), key) == requiredKeys.end()) {
        return ValidationResult(true); // Not required, skip validation
    }
    
    if (!value.has_value()) {
        return ValidationResult(false, "Required key missing: " + key);
    }
    
    return ValidationResult(true);
};
```

## Auto-Save Configuration

### Auto-Save Features
```cpp
Config config;

// Enable auto-save
config.setAutoSave(true, "json");  // Auto-save to JSON format
config.setAutoSave(true, "yaml");  // Auto-save to YAML format

// Check auto-save status
bool isAutoSaveEnabled = config.isAutoSaveEnabled();
std::string saveFormat = config.getSaveFormat();
```

### Auto-Save Behavior
- Configuration saved on validation success
- File format specified by user
- Timestamped backup files
- Atomic write operations

## Merge Operations

### Configuration Merging
```cpp
Config config1, config2;

// Load configurations from different sources
config1.loadFromFile("brain.json");
config2.loadFromFile("memory.yaml");

// Merge configurations (config2 overrides config1)
config1.merge(config2);

// Deep merge with conflict resolution
// Source precedence: CommandLine > File > Default > Runtime
```

## Performance Characteristics

### Time Complexity
- **Key Access**: O(1) average case (hash table)
- **Key Existence Check**: O(1) average case
- **Section Operations**: O(n) where n is number of entries in section
- **Validation**: O(1) per validator function
- **Save/Load**: O(n) where n is total number of configuration entries

### Memory Usage
- **Configuration Storage**: O(n) for n configuration entries
- **Value Storage**: Variable based on type (integers: 4-8 bytes, strings: variable)
- **Validation Functions**: O(m) for m validator functions
- **Temporary Storage**: O(n) during file I/O operations

### Optimization Features
- **Memory Pools**: Efficient memory allocation
- **Lazy Loading**: On-demand configuration loading
- **Change Tracking**: Only write changed values
- **Compression**: Optional compression for large configurations

## Error Handling

### Common Error Conditions
1. **File Not Found**: Graceful fallback with warnings
2. **Invalid Format**: Detailed error messages with suggestions
3. **Validation Failure**: Rollback with descriptive errors
4. **Type Conversion**: Safe conversion with error reporting
5. **Memory Allocation**: Fallback to default values

### Error Recovery
```cpp
bool loadConfigWithRecovery(Config& config, const std::string& filepath) {
    try {
        config.loadFromFile(filepath);
        return true;
    } catch (const std::exception& e) {
        // Log error
        std::cerr << "Failed to load config: " << e.what() << std::endl;
        
        // Try to load from backup or default
        if (config.loadFromFile("config.default.json")) {
            std::cerr << "Loaded default configuration" << std::endl;
            return true;
        }
        
        return false;
    }
}
```

## Testing and Validation

### Unit Tests
```bash
# Run configuration unit tests
./nlm_config_test

# Test configuration loading
./nlm_config_test --load

# Test configuration validation
./nlm_config_test --validate

# Test configuration merging
./nlm_config_test --merge
```

### Integration Tests
```bash
# Test configuration with brain system
./nlm_integration --config=brain.json

# Test configuration persistence
./nlm_integration --persist=true

# Test configuration validation
./nlm_integration --validate
```

## Migration and Upgrading

### Version Migration
```cpp
// Migration helper for configuration changes
class ConfigMigrator {
public:
    static bool migrateOldConfig(Config& config) {
        // Handle old format changes
        if (config.has("brain.regions")) {
            // Migrate old field name
            int regions = config.getOr<int>("brain.regions", 4);
            config.set("brain.config.regions", regions);
            config.remove("brain.regions");
        }
        
        return true;
    }
};
```

## Future Enhancements

### Planned Features
1. **Schema Validation**: JSON Schema and XML Schema support
2. **Distributed Configuration**: Network-based configuration management
3. **Hot Reload**: Real-time configuration updates
4. **Configuration Encryption**: Secure configuration storage
5. **Configuration Monitoring**: Runtime configuration monitoring and alerts

## Troubleshooting

### Common Issues and Solutions

**Issue**: Configuration fails to load
**Solution**: Check file format, permissions, and syntax

**Issue**: Validation fails
**Solution**: Review validation functions and configuration values

**Issue**: Auto-save not working
**Solution**: Check file permissions and disk space

**Issue**: Environment variables not substituted
**Solution**: Verify variable names and default values

## License

MIT

## References

1. **Brain Configuration** (`src/brain/README.md`)
2. **Memory Systems** (`src/memory/README.md`)
3. **Plasticity Module** (`src/plasticity/README.md`)
4. **Neuromodulation Module** (`src/neuromodulation/README.md`)

---

*This documentation is continuously updated as the configuration system evolves. For the latest information, refer to inline source code documentation and recent commits.*

**Version**: Phase 6 Configuration
**Last Updated**: Current
**Status**: Production Ready