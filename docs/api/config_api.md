# NLM Configuration System API Reference

## Overview

The Configuration System API provides access to NLM's comprehensive configuration management capabilities. NLM uses a hierarchical configuration system that controls all aspects of the neural simulation, from basic brain parameters to advanced feature settings.

## Configuration System Architecture

### Configuration Hierarchy

NLM uses a hierarchical configuration system with the following structure:

```
brain:
  neuron_count: 1000
  region_count: 2
  connection_probability: 0.1
  v_thresh: -50.0
  v_rest: -70.0
  v_reset: -75.0
  tau_mem: 20.0
  tau_ref: 2.0

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
    tau_plus: 20.0
    tau_minus: 20.0
    weight_max: 1.0
    weight_min: -0.5
  hebbian:
    enable: true
    learning_rate: 0.0001
  structural:
    enable: true
    growth_rate: 0.01
    pruning_rate: 0.005

neuromodulation:
  dopamine:
    scale: 1.0
    decay_rate: 0.01
  curiosity:
    enable: true
    threshold: 0.5
    decay: 0.02
  novelty:
    enable: true
    sensitivity: 1.0
    decay: 0.01
  attention:
    enable: true
    selection_strength: 0.8
    inhibition_strength: 0.6

memory:
  working:
    enable: true
    capacity: 50
    decay_rate: 0.01
  episodic:
    enable: true
    max_episodes: 100
    replay_interval: 100
  associative:
    enable: true
    pattern_threshold: 0.7
    learning_rate: 0.001

prediction:
  enable: true
  horizon: 10
  confidence_threshold: 0.5
  prediction_decay: 0.05
  error_gain: 1.0

development:
  enable: true
  stage_duration: 100
  plasticity_decay: 0.1
  critical_period: 50
  maturation_rate: 0.01
```

## Config Class

### Config Class

The `Config` class (in `src/core/Config/Config.hpp` and `src/core/Config/Config.cpp`) provides configuration management for NLM.

#### Config()

**Description**: Default constructor.

**Example**:
```cpp
Config config;
```

#### Config(const std::string& filename)

**Description**: Constructor that loads configuration from file.

**Parameters**:
- `filename`: File path to load configuration from

**Example**:
```cpp
Config config("config.json");
```

### Configuration Methods

#### void set(const std::string& key, size_t value)

**Description**: Sets an integer configuration value.

**Parameters**:
- `key`: Configuration key (dot-separated path, e.g., "brain.neuron_count")
- `value`: Integer value to set

**Example**:
```cpp
config.set("brain.neuron_count", 1000);
config.set("brain.region_count", 2);
```

#### void set(const std::string& key, float value)

**Description**: Sets a float configuration value.

**Parameters**:
- `key`: Configuration key
- `value`: Float value to set

**Example**:
```cpp
config.set("plasticity.stdp.learning_rate", 0.001f);
config.set("neuromod.dopamine.scale", 1.5f);
```

#### void set(const std::string& key, bool value)

**Description**: Sets a boolean configuration value.

**Parameters**:
- `key`: Configuration key
- `value`: Boolean value to set

**Example**:
```cpp
config.set("neuromod.curiosity.enable", true);
config.set("memory.episodic.enable", false);
```

#### void set(const std::string& key, const std::string& value)

**Description**: Sets a string configuration value.

**Parameters**:
- `key`: Configuration key
- `value`: String value to set

**Example**:
```cpp
config.set("learning.algorithm", "STDP");
```

#### bool has(const std::string& key) const

**Description**: Checks if configuration has the specified key.

**Parameters**:
- `key`: Configuration key to check

**Returns**: `true` if key exists, `false` otherwise

**Example**:
```cpp
if (config.has("brain.neuron_count")) {
    size_t neurons = config.get<size_t>("brain.neuron_count");
}
```

#### template<typename T> T get(const std::string& key) const

**Description**: Gets configuration value as specified type.

**Parameters**:
- `key`: Configuration key

**Returns**: Configuration value as type T

**Example**:
```cpp
size_t neurons = config.get<size_t>("brain.neuron_count");
float learning_rate = config.get<float>("plasticity.stdp.learning_rate");
bool enable = config.get<bool>("neuromod.curiosity.enable");
```

#### std::vector<std::string> getKeys() const

**Description**: Gets list of all configuration keys.

**Returns**: List of all configuration keys

**Example**:
```cpp
std::vector<std::string> keys = config.getKeys();
for (const auto& key : keys) {
    std::cout << key << std::endl;
}
```

#### void loadFromFile(const std::string& filename)

**Description**: Loads configuration from file, replacing current configuration.

**Parameters**:
- `filename`: File path to load from

**Example**:
```cpp
config.loadFromFile("config.json");
```

#### void saveToFile(const std::string& filename) const

**Description**: Saves current configuration to file.

**Parameters**:
- `filename`: File path to save to

**Example**:
```cpp
config.saveToFile("my_config.json");
```

#### void clear()

**Description**: Clears all configuration values.

**Example**:
```cpp
config.clear();
```

#### size_t size() const

**Description**: Gets number of configuration keys.

**Returns**: Number of configuration keys

**Example**:
```cpp
size_t count = config.size();
```

### Configuration Key Patterns

#### Brain Configuration

```cpp
// Core brain parameters
brain.neuron_count              // Total number of neurons
brain.region_count              // Number of brain regions
brain.connection_probability    // Probability of neural connections
brain.v_thresh                 // Neuron threshold potential (mV)
brain.v_rest                   // Resting potential (mV)
brain.v_reset                  // Reset potential after spike (mV)
brain.tau_mem                  // Membrane time constant (ms)
brain.tau_ref                  // Refractory period (ms)
brain.simulation_timestep       // Simulation timestep (s)
brain.random_seed              // Random number generator seed
```

#### Plasticity Configuration

```cpp
// Plasticity parameters
plasticity.stdp.enable             // Enable STDP plasticity
plasticity.stdp.learning_rate     // STDP learning rate
plasticity.stdp.tau_plus          // STDP time constant (positive)
plasticity.stdp.tau_minus         // STDP time constant (negative)
plasticity.stdp.weight_max       // Maximum synaptic weight
plasticity.stdp.weight_min       // Minimum synaptic weight
plasticity.hebbian.enable         // Enable Hebbian learning
plasticity.hebbian.learning_rate  // Hebbian learning rate
plasticity.structural.enable      // Enable structural plasticity
plasticity.structural.growth_rate  // Structural growth rate
plasticity.structural.pruning_rate // Structural pruning rate
```

#### Neuromodulation Configuration

```cpp
// Neuromodulation parameters
neuromod.dopamine.scale          // Dopamine modulation scale
neuromod.dopamine.decay_rate      // Dopamine decay rate
neuromod.curiosity.enable         // Enable curiosity system
neuromod.curiosity.threshold      // Curiosity activation threshold
neuromod.curiosity.decay          // Curiosity decay rate
neuromod.novelty.enable           // Enable novelty detection
neuromod.novelty.sensitivity       // Novelty detection sensitivity
neuromod.novelty.decay           // Novelty decay rate
neuromod.attention.enable         // Enable attention system
neuromod.attention.selection_strength // Attention selection strength
neuromod.attention.inhibition_strength // Attention inhibition strength
```

#### Memory Configuration

```cpp
// Memory system parameters
memory.working.enable            // Enable working memory
memory.working.capacity          // Working memory capacity
memory.working.decay_rate        // Working memory decay rate
memory.episodic.enable           // Enable episodic memory
memory.episodic.max_episodes     // Maximum episodic episodes
memory.episodic.replay_interval   // Memory replay interval
memory.episodic.replay_strength   // Memory replay strength
memory.associative.enable        // Enable associative memory
memory.associative.pattern_threshold // Pattern matching threshold
memory.associative.learning_rate // Association learning rate
memory.concept_formation.enable   // Enable concept formation
memory.concept_formation.threshold // Concept learning threshold
```

#### Prediction Configuration

```cpp
// Prediction system parameters
prediction.enable                 // Enable prediction system
prediction.horizon               // Prediction time horizon
prediction.confidence_threshold    // Minimum confidence for action selection
prediction.prediction_decay        // Prediction decay rate
prediction.error_gain             // Prediction error gain
prediction.learning_rate          // Prediction learning rate
```

#### Development Configuration

```cpp
// Development system parameters
development.enable                  // Enable development system
development.stage_duration         // Developmental stage duration
development.plasticity_decay       // Plasticity decay with development
development.critical_period        // Critical period length
development.maturation_rate        // Maturation rate
```

### Configuration Templates

#### createDefaultConfig()

**Description**: Creates default configuration with recommended settings for standard use.

**Returns**: Default configuration

**Example**:
```cpp
Config config = Config::createDefaultConfig();
```

#### createResearchConfig()

**Description**: Creates configuration optimized for research experiments.

**Returns**: Research-oriented configuration

**Example**:
```cpp
Config config = Config::createResearchConfig();
```

#### createHighPerformanceConfig()

**Description**: Creates configuration optimized for high-performance simulation.

**Returns**: High-performance configuration

**Example**:
```cpp
Config config = Config::createHighPerformanceConfig();
```

#### createMinimalConfig()

**Description**: Creates minimal configuration for basic functionality testing.

**Returns**: Minimal configuration

**Example**:
```cpp
Config config = Config::createMinimalConfig();
```

### Configuration Validation

#### bool validate() const

**Description**: Validates current configuration for consistency.

**Returns**: `true` if configuration is valid, `false` otherwise

**Example**:
```cpp
if (!config.validate()) {
    std::cerr << "Invalid configuration!" << std::endl;
    return 1;
}
```

#### std::vector<std::string> getValidationErrors() const

**Description**: Gets configuration validation errors.

**Returns**: List of validation error messages

**Example**:
```cpp
std::vector<std::string> errors = config.getValidationErrors();
for (const auto& error : errors) {
    std::cerr << "Error: " << error << std::endl;
}
```

#### bool checkParameter(const std::string& key, const std::string& type, const std::string& min, const std::string& max) const

**Description**: Checks if parameter meets type and range requirements.

**Parameters**:
- `key`: Parameter key
- `type`: Expected type ("int", "float", "bool", "string")
- `min`: Minimum allowed value (empty for no minimum)
- `max`: Maximum allowed value (empty for no maximum)

**Returns**: `true` if parameter is valid, `false` otherwise

**Example**:
```cpp
if (!config.checkParameter("brain.neuron_count", "int", "100", "10000")) {
    std::cerr << "Invalid neuron count!" << std::endl;
}
```

### Configuration File Format

#### JSON Configuration

Configuration can be saved and loaded in JSON format:

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.0001
        }
    },
    "neuromodulation": {
        "dopamine": {
            "scale": 1.0,
            "decay_rate": 0.01
        },
        "curiosity": {
            "enable": true,
            "threshold": 0.5,
            "decay": 0.02
        }
    }
}
```

#### Configuration File Extensions

Common configuration file extensions:
- `.json` - JSON format
- `.yml` - YAML format
- `.yaml` - YAML format
- `.xml` - XML format
- `.ini` - INI format
- `.cfg` - Configuration format

## Python Configuration API

### Config Class

The Python `Config` class provides Python-friendly configuration management.

#### createDefaultConfig()

**Description**: Creates a default configuration with recommended settings.

**Returns**: Default configuration object

**Example**:
```python
import pynlm

config = pynlm.createDefaultConfig()
```

#### loadFromFile(filename)

**Description**: Loads configuration from file.

**Parameters**:
- `filename`: File path to load from

**Example**:
```python
config.loadFromFile("config.json")
```

#### saveToFile(filename)

**Description**: Saves configuration to file.

**Parameters**:
- `filename`: File path to save to

**Example**:
```python
config.saveToFile("my_config.json")
```

#### has(key)

**Description**: Checks if configuration has the specified key.

**Parameters**:
- `key`: Configuration key (dot-separated path)

**Returns**: `True` if key exists, `False` otherwise

**Example**:
```python
if config.has("brain.neuron_count"):
    neurons = config.get("brain.neuron_count")
```

#### get(key, default=None)

**Description**: Gets configuration value.

**Parameters**:
- `key`: Configuration key
- `default`: Default value if key doesn't exist (optional)

**Returns**: Configuration value

**Example**:
```python
neurons = config.get("brain.neuron_count", 1000)
learning_rate = config.get("plasticity.stdp.learning_rate", 0.001)
```

#### set(key, value)

**Description**: Sets configuration value.

**Parameters**:
- `key`: Configuration key
- `value`: Value to set

**Example**:
```python
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.005)
```

#### getKeys()

**Description**: Gets list of all configuration keys.

**Returns**: List of configuration keys

**Example**:
```python
keys = config.getKeys()
```

#### clear()

**Description**: Clears all configuration values.

**Example**:
```python
config.clear()
```

#### size()

**Description**: Gets number of configuration keys.

**Returns**: Number of configuration keys

**Example**:
```python
key_count = config.size()
```

### Python Configuration Examples

#### Basic Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 1000)
config.set("brain.region_count", 2)
config.set("plasticity.stdp.learning_rate", 0.005)

# Get configuration values
neurons = config.get("brain.neuron_count")
learning_rate = config.get("plasticity.stdp.learning_rate")

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")
```

#### Configuration Validation

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

# Validate configuration
if not config.validate():
    print("Configuration is invalid!")
    return 1

# Check specific parameter
if config.has("brain.neuron_count"):
    neurons = config.get("brain.neuron_count")
    if neurons < 100 or neurons > 10000:
        print("Invalid neuron count!")
        return 1
```

#### Research Configuration

```python
import pynlm

# Create research configuration
config = pynlm.createResearchConfig()

# Customize research configuration
config.set("brain.neuron_count", 2000)
config.set("memory.episodic.max_episodes", 500)
config.set("prediction.horizon", 20)
config.set("neuromod.curiosity.threshold", 0.3)

# Save research configuration
config.saveToFile("research_config.json")
```

## Configuration File I/O

### Reading Configuration Files

```cpp
// Read JSON configuration file
std::ifstream file("config.json");
if (!file.is_open()) {
    std::cerr << "Failed to open config.json!" << std::endl;
    return 1;
}

// Parse JSON and apply to config
// (Implementation depends on JSON library used)
```

### Writing Configuration Files

```cpp
// Write JSON configuration file
std::ofstream file("config.json");
if (!file.is_open()) {
    std::cerr << "Failed to create config.json!" << std::endl;
    return 1;
}

// Write configuration to file
// (Implementation depends on JSON library used)
```

### Configuration File Templates

#### Basic Configuration Template

```ini
# Basic NLM configuration
[brain]
neuron_count = 1000
region_count = 2
connection_probability = 0.1

[plasticity]
stdp.enable = true
stdp.learning_rate = 0.001
hebbian.enable = true

[neuromodulation]
dopamine.scale = 1.0
curiosity.enable = true
```

#### Research Configuration Template

```ini
# Research NLM configuration
[brain]
neuron_count = 2000
region_count = 3
connection_probability = 0.15

[plasticity]
stdp.enable = true
stdp.learning_rate = 0.005
hebbian.enable = true
structural.enable = true

[neuromodulation]
dopamine.scale = 1.5
curiosity.enable = true
curiosity.threshold = 0.3
novelty.enable = true

[memory]
working.enable = true
working.capacity = 100
episodic.enable = true
episodic.max_episodes = 500
associative.enable = true

[prediction]
enable = true
horizon = 20

[development]
enable = true
stage_duration = 150
```

## Configuration Best Practices

### 1. Configuration Validation

1. **Always validate configuration** before use
2. **Check parameter types** and ranges
3. **Ensure configuration consistency** across related parameters
4. **Test configuration** with different scenarios

### 2. Configuration Management

1. **Use descriptive configuration names**
2. **Document configuration choices**
3. **Backup important configurations**
4. **Version control configuration files**
5. **Use environment-specific configurations**

### 3. Configuration Optimization

1. **Start with minimal configuration**
2. **Add complexity incrementally**
3. **Profile performance** with different configurations
4. **Balance accuracy and performance**
5. **Use configuration templates** for common scenarios

### 4. Configuration Testing

1. **Test with different configuration sizes**
2. **Validate configuration with various scenarios**
3. **Check for configuration conflicts**
4. **Test configuration loading/saving**
5. **Verify configuration persistence**

## Configuration Troubleshooting

### Common Issues

1. **"Invalid configuration"**
   - **Problem**: Configuration validation fails
   - **Solution**: Check configuration for errors, fix invalid parameters

2. **"File not found"**
   - **Problem**: Configuration file cannot be opened
   - **Solution**: Check file path, ensure file exists, check permissions

3. **"Type mismatch"**
   - **Problem**: Configuration parameter has wrong type
   - **Solution**: Use correct parameter type in configuration

4. **"Range error"**
   - **Problem**: Configuration parameter value out of range
   - **Solution**: Adjust parameter value to within valid range

### Troubleshooting Commands

```bash
# List configuration keys
pynlm config --list

# Validate configuration
pynlm config --validate

# Check configuration errors
pynlm config --errors

# Load configuration
pynlm config --load config.json

# Save configuration
pynlm config --save my_config.json
```

## Configuration API Reference Summary

| Category | Class | Key Methods |
|----------|-------|-------------|
| Core | `Config` | `createDefaultConfig()`, `loadFromFile()`, `saveToFile()`, `set()`, `get()`, `has()` |
| Validation | `Config` | `validate()`, `getValidationErrors()`, `checkParameter()` |
| Templates | `Config` | `createDefaultConfig()`, `createResearchConfig()`, `createHighPerformanceConfig()`, `createMinimalConfig()` |

## Python Configuration API Reference Summary

| Class | Key Methods | Description |
|-------|-------------|-------------|
| `Config` | `createDefaultConfig()`, `loadFromFile()`, `saveToFile()`, `set()`, `get()`, `has()`, `getKeys()`, `clear()`, `size()` | Configuration management |

## Next Steps

1. **Get Started**: Try the basic examples in `docs/examples/configuration.cpp`
2. **Explore Documentation**: Read `HOW_TO_USE.md` for Python configuration details
3. **Run Demos**: Execute configuration-specific demo programs
4. **Create Experiments**: Design configuration-specific NLM experiments
5. **Extend Configuration**: Add new configuration features to NLM

The Configuration System API provides comprehensive configuration management for NLM. It provides everything you need to configure, validate, and manage NLM simulations across different scenarios and use cases.

---

*Configuration System API Reference*
- **Location**: `docs/api/config_api.md`
- **Source**: `src/core/Config/Config.hpp`, `src/core/Config/Config.cpp`
- **Python Bindings**: Configuration API in Python bindings
- **Documentation**: Complete configuration API reference with examples

*Ready to start using the Configuration API?*

1. **Read the examples**: Check `docs/examples/configuration.cpp`
2. **Try Python API**: Use `HOW_TO_USE.md` for Python examples
3. **Run demo programs**: Execute the built-in demo programs
4. **Create your own**: Design custom configuration NLM experiments
5. **Contribute**: Share your configuration API usage with the community

The Configuration API is the foundation of NLM's configurability. It provides everything you need to configure NLM systems for different research needs, performance requirements, and experimental scenarios!

---

*Configuration API enables NLM to:
• Provide flexible configuration management
• Support multiple configuration formats
• Ensure configuration consistency
• Validate configuration correctness
• Maintain configuration persistence
• Enable environment-specific configurations

Your configuration API research contributions will help advance neural system configuration capabilities!

---