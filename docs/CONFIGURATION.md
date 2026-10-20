# NLM (Neural Learning Machine) - Configuration System Documentation

## Overview

The NLM configuration system provides comprehensive parameter management for the neural brain simulation framework. It supports typed configuration parameters, validation, history tracking, and file I/O operations. This document provides detailed information about configuration categories, parameters, and best practices.

## Configuration System Architecture

### Core Components

1. **Config Class**: Main configuration management interface
2. **ConfigMetadata**: Schema definition for each parameter
3. **ConfigurationValidator**: Parameter validation system
4. **ConfigHistoryManager**: Parameter change tracking and rollback
5. **SessionManager**: Isolated configuration sessions
6. **FileIO**: JSON configuration file support

### Configuration Categories

The NLM configuration system organizes parameters into logical categories:

| Category | Description | Key Parameters |
|----------|-------------|----------------|
| `brain` | Neural network configuration | neuron_count, synapse_density, v_thresh |
| `plasticity` | Learning mechanism parameters | stdp_learning_rate, hebbian_threshold |
| `neuromod` | Neuromodulation signals | dopamine_scale, curiosity_threshold |
| `development` | Developmental stages | critical_period_duration, maturity_rate |
| `world` | Environment interface | width, height, vision_range |
| `performance` | Performance optimization | enable_optimization, parallel_threads |
| `logging` | Debug and monitoring | log_level, log_file, enable_profiling |

## Configuration Parameters

### Brain Configuration

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons in the brain | 100-10000 |
| `brain.region_count` | int | 1 | Number of brain regions | 1-50 |
| `brain.connection_probability` | double | 0.05 | Connection probability between neurons | 0.001-0.5 |
| `brain.synapse_density` | double | 0.1 | Average synapse density | 0.01-1.0 |
| `brain.initial_weight_mean` | double | 0.5 | Mean initial synaptic weight | 0.0-1.0 |
| `brain.initial_weight_std` | double | 0.1 | Standard deviation of initial weights | 0.01-0.5 |
| `brain.v_thresh` | double | -50.0 | Neuronal threshold potential (mV) | -100 to -10 |
| `brain.v_rest` | double | -70.0 | Resting potential (mV) | -90 to -60 |
| `brain.v_reset` | double | -75.0 | Reset potential after spike (mV) | -90 to -60 |
| `brain.tau_mem` | double | 20.0 | Membrane time constant (ms) | 1.0-100.0 |
| `brain.tau_ref` | double | 2.0 | Refractory period (ms) | 1.0-10.0 |
| `brain.max_rate` | double | 200.0 | Maximum firing rate (Hz) | 10.0-1000.0 |
| `brain.simulation_timestep` | double | 0.001 | Simulation timestep (s) | 0.0001-0.01 |

### Plasticity Configuration

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity | - |
| `plasticity.stdp.learning_rate` | double | 0.001 | STDP learning rate | 0.0001-0.1 |
| `plasticity.stdp.tau_plus` | double | 20.0 | STDP LTP time constant (ms) | 5.0-100.0 |
| `plasticity.stdp.tau_minus` | double | 20.0 | STDP LTD time constant (ms) | 5.0-100.0 |
| `plasticity.stdp.ltp_threshold` | double | 0.5 | STDP LTP threshold | 0.1-1.0 |
| `plasticity.stdp.ltd_threshold` | double | 0.5 | STDP LTD threshold | 0.1-1.0 |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning | - |
| `plasticity.hebbian.learning_rate` | double | 0.01 | Hebbian learning rate | 0.001-0.1 |
| `plasticity.hebbian.threshold` | double | 0.5 | Hebbian learning threshold | 0.1-1.0 |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity | - |
| `plasticity.structural.synaptogenesis_rate` | double | 0.0001 | Synapse formation rate | 0.00001-0.001 |
| `plasticity.structural.pruning_rate` | double | 0.00001 | Synapse removal rate | 0.000001-0.0001 |
| `plasticity.structural.min_weight` | double | 0.0 | Minimum synaptic weight | 0.0-0.5 |
| `plasticity.structural.max_weight` | double | 1.0 | Maximum synaptic weight | 0.5-2.0 |

### Neuromodulation Configuration

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `neuromod.dopamine.scale` | double | 1.0 | Dopamine modulation scale | 0.1-5.0 |
| `neuromod.dopamine.decay_rate` | double | 0.1 | Dopamine decay rate | 0.01-0.5 |
| `neuromod.dopamine.baseline` | double | 0.01 | Baseline dopamine level | 0.001-0.1 |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration | - |
| `neuromod.curiosity.threshold` | double | 0.3 | Curiosity activation threshold | 0.1-0.5 |
| `neuromod.curiosity.learning_rate` | double | 0.01 | Curiosity learning rate | 0.001-0.1 |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection | - |
| `neuromod.novelty.sensitivity` | double | 0.5 | Novelty detection sensitivity | 0.1-1.0 |
| `neuromod.novelty.decay_rate` | double | 0.05 | Novelty decay rate | 0.01-0.2 |
| `neuromod.prediction_error.scale` | double | 1.0 | Prediction error scaling | 0.5-2.0 |
| `neuromod.prediction_error.threshold` | double | 0.2 | Prediction error threshold | 0.1-0.5 |

### Development Configuration

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `development.enabled` | bool | true | Enable development system | - |
| `development.stage_duration` | double | 100.0 | Duration of each developmental stage (ms) | 10.0-500.0 |
| `development.critical_period_start` | double | 20.0 | Start of critical period (ms) | 5.0-100.0 |
| `development.critical_period_end` | double | 80.0 | End of critical period (ms) | 50.0-150.0 |
| `development.maturation_rate` | double | 0.1 | Rate of neural maturation | 0.01-0.5 |
| `development.plasticity_decay` | double | 0.05 | Overall plasticity decay rate | 0.01-0.2 |
| `development.min_plasticity` | double | 0.001 | Minimum plasticity rate | 0.0001-0.01 |

### World Configuration

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `world.width` | int | 100 | World width (cells) | 10-1000 |
| `world.height` | int | 100 | World height (cells) | 10-1000 |
| `world.vision_width` | int | 8 | Vision sensor width | 4-32 |
| `world.vision_height` | int | 8 | Vision sensor height | 4-32 |
| `world.vision_range` | int | 10 | Vision sensing range | 5-50 |
| `world.cell_size` | double | 1.0 | Size of each world cell | 0.1-5.0 |
| `world.max_energy` | double | 100.0 | Maximum agent energy | 10.0-1000.0 |
| `world.temperature` | double | 25.0 | World temperature (arbitrary units) | 0.0-100.0 |
| `world.random_seed` | unsigned int | 42 | Random seed for reproducibility | 0-4294967295 |

### Performance Configuration

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `performance.enable_optimization` | bool | true | Enable performance optimizations | - |
| `performance.parallel_threads` | int | 0 | Number of parallel threads (0 = auto) | 0-32 |
| `performance.memory_pool_size` | size_t | 0 | Memory pool size in MB | 0-1024 |
| `performance.simulation_speed` | double | 1.0 | Simulation speed factor | 0.1-10.0 |
| `performance.enable_profiling` | bool | false | Enable profiling output | - |
| `performance.checkpoint_interval` | int | 1000 | Checkpoint save interval (steps) | 100-10000 |

### Logging Configuration

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `logging.level` | string | "info" | Log level (trace, debug, info, warning, error) | - |
| `logging.file` | string | "" | Log file path (empty = stdout) | - |
| `logging.max_file_size` | size_t | 10485760 | Maximum log file size (bytes) | 1048576-104857600 |
| `logging.max_files` | int | 5 | Maximum number of log files | 1-10 |
| `logging.enable_colors` | bool | true | Enable colored log output | - |
| `logging.enable_timestamps` | bool | true | Enable timestamps in log output | - |

## Configuration File Format

### JSON Format

NLM configuration files use standard JSON format with the following structure:

```json
{
    "version": "1.0",
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "v_thresh": -50.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.01
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0,
            "baseline": 0.01
        }
    },
    "world": {
        "width": 100,
        "height": 100,
        "vision_width": 8,
        "vision_height": 8
    }
}
```

### Configuration Validation

The configuration system validates parameters against schema definitions:

1. **Type Validation**: Ensures parameter types match expected types
2. **Range Validation**: Checks parameter values against valid ranges
3. **Dependency Validation**: Ensures required parameters are set
4. **Schema Validation**: Validates configuration against category schemas

**Example Validation Errors**:

```cpp
// Invalid type
config.set("brain.neuron_count", "invalid"); // Error: type mismatch

// Out of range
config.set("brain.neuron_count", 20000); // Error: exceeds maximum

// Missing required parameter
config.set("world.width", 100);  // Error: missing world.height
```

## Session Management

### Configuration Sessions

The NLM configuration system supports isolated sessions:

```cpp
// Start new session
config.startNewSession("experiment_1");

// Make changes in session
config.set("brain.neuron_count", 1000);

// End session and save
config.endSession();

// Rollback to previous session
config.undoLastChange();
```

### Session Features

1. **Session Isolation**: Changes are isolated to specific sessions
2. **Rollback Support**: Can undo changes within a session
3. **History Tracking**: Complete parameter change history
4. **Snapshot Capability**: Save session state for later restoration

## File I/O Operations

### Loading Configuration

```cpp
Config config;

// Load from JSON file
if (!config.loadFromFile("brain_config.json")) {
    std::cerr << "Failed to load configuration" << std::endl;
    return 1;
}

// Load with validation
if (!config.loadFromFile("brain_config.json", true)) {
    std::cerr << "Configuration validation failed" << std::endl;
    return 1;
}
```

### Saving Configuration

```cpp
Config config;

// Save with default options
if (!config.saveToFile("default_config.json")) {
    std::cerr << "Failed to save configuration" << std::endl;
    return 1;
}

// Save with custom formatting
config.set("brain.neuron_count", 1500);
config.set("plasticity.stdp_learning_rate", 0.01);
config.saveToFile("custom_config.json");
```

### Configuration Templates

NLM provides built-in configuration templates:

```cpp
// Create default configuration
Config config = Config::createDefaultConfig();

// Create optimized configuration for large simulations
Config config = Config::createOptimizedConfig(5000, true);

// Create development configuration
Config config = Config::createDevelopmentConfig();

// Create production configuration
Config config = Config::createProductionConfig();
```

## Configuration API Reference

### Core Methods

#### Config Constructor/Destructor

```cpp
Config::Config()
Config::~Config()
```

**Description**: Creates a new configuration object with default parameters.

#### Parameter Management

```cpp
template<typename T>
bool Config::set(const std::string& key, const T& value)

T Config::get(const std::string& key) const

bool Config::has(const std::string& key) const

std::vector<std::string> Config::getKeys() const

std::vector<std::string> Config::getKeys(const std::string& category) const
```

**Parameters**:
- `key`: Configuration parameter key (e.g., "brain.neuron_count")
- `value`: Parameter value to set

**Return Values**:
- `bool`: True if operation successful, false otherwise
- `T`: Parameter value

**Examples**:

```cpp
Config config;

// Set parameters
config.set("brain.neuron_count", 1000);
config.set("plasticity.stdp_learning_rate", 0.001);

// Get parameters
int neurons = config.get("brain.neuron_count");
double rate = config.get("plasticity.stdp_learning_rate");

// Check existence
if (config.has("brain.neuron_count")) {
    std::cout << "Brain neuron count is set" << std::endl;
}

// List all keys
auto keys = config.getKeys();
for (const auto& key : keys) {
    std::cout << key << std::endl;
}
```

#### Schema Management

```cpp
void Config::addCategory(const std::string& category)

void Config::addParameter(const std::string& category,
                         const std::string& key,
                         const ConfigMetadata& metadata)

ConfigMetadata Config::getParameterMetadata(const std::string& key) const
```

**Parameters**:
- `category`: Configuration category (e.g., "brain", "plasticity")
- `key`: Parameter key within the category
- `metadata`: Parameter metadata including description, type, range, etc.

#### File I/O

```cpp
bool Config::loadFromFile(const std::string& filename)
bool Config::saveToFile(const std::string& filename) const
```

**Parameters**:
- `filename`: Path to configuration file

**Return Values**:
- `bool`: True if operation successful, false otherwise

**Examples**:

```cpp
// Load configuration from file
Config config;
if (config.loadFromFile("my_config.json")) {
    std::cout << "Configuration loaded successfully" << std::endl;
}

// Save configuration to file
if (config.saveToFile("new_config.json")) {
    std::cout << "Configuration saved successfully" << std::endl;
}
```

#### Session Management

```cpp
void Config::startNewSession(const std::string& sessionId)
void Config::endSession()
std::string Config::getCurrentSessionId() const

bool Config::undoLastChange()
size_t Config::getHistorySize() const
```

**Parameters**:
- `sessionId`: Unique identifier for the configuration session

**Return Values**:
- `bool`: True if operation successful, false otherwise
- `size_t`: Number of entries in history

**Examples**:

```cpp
Config config;

// Start a new session
config.startNewSession("experiment_1");

// Make configuration changes
config.set("brain.neuron_count", 1000);
config.set("plasticity.stdp_learning_rate", 0.001);

// End session and save changes
config.endSession();

// Undo last change
if (config.undoLastChange()) {
    std::cout << "Last change undone" << std::endl;
}

// Check history size
std::cout << "History size: " << config.getHistorySize() << std::endl;
```

#### Statistics and Information

```cpp
size_t Config::getTotalChanges() const
size_t Config::getTotalRollbacks() const
```

**Examples**:

```cpp
Config config;

// Make some changes
config.set("brain.neuron_count", 1000);
config.set("plasticity.stdp_learning_rate", 0.001);

// Get statistics
std::cout << "Total changes: " << config.getTotalChanges() << std::endl;
std::cout << "Total rollbacks: " << config.getTotalRollbacks() << std::endl;
```

## Configuration Best Practices

### Parameter Naming

1. **Use Dot Notation**: Use category.parameter format for clear organization
2. **Be Descriptive**: Use clear, descriptive parameter names
3. **Consistency**: Use consistent naming conventions across categories
4. **No Spaces**: Parameter names should not contain spaces

**Good Examples**:

```cpp
config.set("brain.neuron_count", 1000);
config.set("plasticity.stdp_learning_rate", 0.001);
config.set("neuromod.dopamine_scale", 1.0);
config.set("development.critical_period_end", 80.0);
```

**Bad Examples**:

```cpp
config.set("neurons", 1000);  // Too vague
config.set("plasticity_stdp_learning_rate", 0.001);  // Inconsistent format
```

### Configuration Validation

1. **Always Validate**: Validate configuration before using it
2. **Check Dependencies**: Ensure required parameters are set
3. **Validate Ranges**: Check that values are within valid ranges
4. **Handle Errors Gracefully**: Provide meaningful error messages

**Example Validation**:

```cpp
Config config;

// Load and validate
if (!config.loadFromFile("config.json")) {
    std::cerr << "Configuration validation failed" << std::endl;
    return 1;
}

// Check for required parameters
if (!config.has("brain.neuron_count")) {
    std::cerr << "Missing required parameter: brain.neuron_count" << std::endl;
    return 1;
}

// Validate parameter ranges
int neuronCount = config.get("brain.neuron_count");
if (neuronCount < 100 || neuronCount > 10000) {
    std::cerr << "Invalid neuron count: " << neuronCount << std::endl;
    return 1;
}
```

### Configuration Management

1. **Version Control**: Save configurations to version control
2. **Backup**: Regularly backup important configurations
3. **Template System**: Use configuration templates for common setups
4. **Documentation**: Document configuration parameters and their effects

### Performance Considerations

1. **Minimize Set Operations**: Reduce the number of configuration changes
2. **Use Batches**: Set multiple parameters in batch when possible
3. **Reuse Configurations**: Use the same configuration for multiple runs
4. **Profile Configuration**: Profile configuration loading time

### Debugging Configuration

1. **Enable Logging**: Enable logging to track configuration changes
2. **Use Checkpoints**: Use checkpoints to save and restore configurations
3. **Validate Before Use**: Always validate configuration before use
4. **Check History**: Use configuration history to debug issues

## Migration Guide

### From Previous Versions

This section provides guidance for migrating from previous NLM versions:

#### Version 0.1.x to 1.0.0

1. **Parameter Changes**:
   - `plasticity.stddev_learning_rate` → `plasticity.stdp.learning_rate`
   - `neuromod.curiosity.enabled` → `neuromod.curiosity.enable`
   - `development.critical_period` → `development.critical_period_end`

2. **New Categories**:
   - `performance` category added
   - `logging` category added

3. **File Format Changes**:
   - JSON schema updated
   - Additional metadata fields

**Migration Example**:

```cpp
// Old configuration (0.1.x)
config.set("plasticity.stddev_learning_rate", 0.001);
config.set("neuromod.curiosity.enabled", true);

// New configuration (1.0.0)
config.set("plasticity.stdp.learning_rate", 0.001);
config.set("neuromod.curiosity.enable", true);
```

## Configuration Examples

### Example 1: Basic Configuration

```cpp
Config config;

// Basic brain configuration
config.set("brain.neuron_count", 500);
config.set("brain.synapse_density", 0.1);

// Basic plasticity configuration
config.set("plasticity.stdp.enable", true);
config.set("plasticity.stdp.learning_rate", 0.001);

// Basic neuromodulation configuration
config.set("neuromod.dopamine.scale", 1.0);
config.set("neuromod.curiosity.enable", true);

// Save configuration
config.saveToFile("basic_config.json");
```

### Example 2: World Simulation Configuration

```cpp
Config config;

// World configuration
config.set("world.width", 200);
config.set("world.height", 200);
config.set("world.vision_width", 16);
config.set("world.vision_height", 16);

// Agent configuration
config.set("brain.neuron_count", 1000);
config.set("brain.region_count", 3);

// Performance configuration
config.set("performance.enable_optimization", true);
config.set("performance.parallel_threads", 4);

// Save configuration
config.saveToFile("world_config.json");
```

### Example 3: Development Configuration

```cpp
Config config;

// Development configuration
config.set("development.enabled", true);
config.set("development.stage_duration", 150.0);
config.set("development.critical_period_start", 30.0);
config.set("development.critical_period_end", 120.0);

// High plasticity configuration
config.set("plasticity.stdp.learning_rate", 0.01);
config.set("plasticity.hebbian.learning_rate", 0.02);
config.set("plasticity.structural.synaptogenesis_rate", 0.001);

// Save configuration
config.saveToFile("development_config.json");
```

## Configuration File Templates

### Template Files

Configuration templates are available in the `configs/` directory:

- `configs/basic_config.json` - Basic configuration for testing
- `configs/large_scale_config.json` - Configuration for large simulations
- `configs/development_config.json` - Configuration for developmental studies
- `configs/production_config.json` - Configuration for production use

### Using Templates

```cpp
// Load from template
Config config;
if (config.loadFromFile("configs/basic_config.json")) {
    // Use configuration
}

// Save custom configuration to template directory
config.saveToFile("configs/my_custom_config.json");
```

## Tools and Utilities

### Configuration Editor

A command-line tool for editing NLM configurations:

```bash
# List all parameters
nlm_config_editor list

# Show parameter details
nlm_config_editor show brain.neuron_count

# Set parameter
nlm_config_editor set brain.neuron_count 1000

# Load configuration
nlm_config_editor load config.json

# Save configuration
nlm_config_editor save my_config.json
```

### Configuration Validator

A tool for validating NLM configurations:

```bash
# Validate configuration
nlm_config_validator validate config.json

# Validate and fix
nlm_config_validator fix config.json

# Show validation errors
nlm_config_validator errors config.json
```

## Configuration Troubleshooting

### Common Issues and Solutions

**Issue**: Configuration file not found
**Solution**:

```cpp
// Check if file exists
std::ifstream file("config.json");
if (!file.good()) {
    std::cerr << "Configuration file not found: config.json" << std::endl;
}
```

**Issue**: Invalid JSON format
**Solution**:

```cpp
// Use a JSON parser to check syntax
std::ifstream file("config.json");
nlohmann::json jsonData;
try {
    file >> jsonData;
} catch (const std::exception& e) {
    std::cerr << "Invalid JSON format: " << e.what() << std::endl;
}
```

**Issue**: Parameter validation fails
**Solution**:

```cpp
// Enable detailed validation
Config config;
config.setValidationLevel(Config::ValidationLevel::DETAILED);

if (!config.loadFromFile("config.json")) {
    std::cerr << "Configuration validation failed" << std::endl;
    config.printValidationErrors();
}
```

## Configuration API Reference

### Class Config

```cpp
class Config {
public:
    // Constructors and Destructor
    Config();
    ~Config();
    
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Parameter management
    template<typename T>
    bool set(const std::string& key, const T& value);
    
    template<typename T>
    T get(const std::string& key) const;
    
    bool has(const std::string& key) const;
    
    // Schema management
    void addCategory(const std::string& category);
    void addParameter(const std::string& category,
                     const std::string& key,
                     const ConfigMetadata& metadata);
    
    // File I/O
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    
    // Session management
    void startNewSession(const std::string& sessionId);
    void endSession();
    std::string getCurrentSessionId() const;
    
    // History and rollback
    bool undoLastChange();
    size_t getHistorySize() const;
    
    // Statistics
    size_t getTotalChanges() const;
    size_t getTotalRollbacks() const;
    
    // Validation
    bool validate() const;
    void setValidationLevel(ValidationLevel level);
    
    // Statistics
    double getValidationScore() const;
    
    // Template methods
    static Config createDefaultConfig();
    static Config createOptimizedConfig(int neuronCount, bool enableOptimizations);
    static Config createDevelopmentConfig();
    static Config createProductionConfig();
};
```

### Class ConfigMetadata

```cpp
struct ConfigMetadata {
    std::string description;
    std::string category;
    std::string type;
    bool required;
    std::optional<std::pair<double, double>> range;
    std::optional<std::string> unit;
    std::optional<std::string> validationRegex;
    std::optional<std::string> defaultValue;
    std::optional<std::string> notes;
};
```

## Conclusion

The NLM configuration system provides a comprehensive framework for managing brain simulation parameters. It supports typed parameters, validation, history tracking, and file I/O operations. This document provides detailed information about configuration categories, parameters, and best practices.

For more information, refer to the API reference and other documentation files in the NLM documentation set.

## References

- [NLM API Reference](API_REFERENCE.md)
- [NLM User Guide](USER_GUIDE.md)
- [NLM Python Bindings](PYTHON_BINDINGS.md)
- [NLM Architecture Documentation](ARCHITECTURE.md)
- [NLM Testing Documentation](TESTING.md)
- [NLM Examples and Tutorials](EXAMPLES.md)
- [NLM Upgrading Guide](UPGRADING_GUIDE.md)

## See Also

- Configuration templates in the `configs/` directory
- Configuration validation tools
- Configuration history and rollback features
- Performance optimization with configuration

## License

MIT License - See LICENSE file for details.