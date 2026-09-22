# NLM Configuration Schema Documentation

## Overview

The NLM (Neural Learning Machine) configuration system uses a structured format with nested sections to organize brain parameters, simulation settings, and system configurations. This document provides a comprehensive schema for all configuration parameters used in the NLM framework.

## Configuration File Structure

NLM configuration files use a hybrid format combining INI-style sections with dot-notation for nested parameters:

```ini
[section.name]
parameter.name = value
nested.parameter = value
```

Sections provide logical grouping of related parameters, while dot notation allows for deeper nesting and avoids naming conflicts.

## Configuration Sections

### General Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `random_seed` | int | 42 | Seed for random number generator (reproducible experiments) |
| `simulation_timestep` | double | 0.001 | Simulation timestep in seconds |
| `log_level` | string | "INFO" | Logging level: DEBUG, INFO, WARNING, ERROR, CRITICAL |
| `log_to_file` | bool | false | Whether to log to file |
| `log_filename` | string | "nlm.log" | Log file name when log_to_file is true |
| `checkpoint_dir` | string | "./checkpoints" | Directory for checkpoint files |
| `checkpoint_interval` | int | 1000 | Simulation steps between checkpoints |

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons in the brain |
| `brain.region_count` | int | 1 | Number of neural regions |
| `brain.connection_probability` | float | 0.1 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable spike-timing dependent plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant for LTP (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant for LTD (ms) |
| `plasticity.stdp.ltp_weight` | float | 0.01 | LTP weight parameter |
| `plasticity.stdp.ltd_weight` | float | 0.012 | LTD weight parameter |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `plasticity.structural.pruning_rate` | float | 0.00001 | Rate of synapse elimination |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale factor |
| `neuromod.dopamine.baseline` | float | 0.1 | Baseline dopamine level |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.scale` | float | 1.0 | Curiosity modulation scale |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.novelty.threshold` | float | 0.5 | Novelty detection threshold |
| `neuromod.prediction_error.scale` | float | 1.0 | Prediction error modulation scale |

### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.enabled` | bool | true | Enable developmental processes |
| `development.initial_stage` | string | "Initial" | Initial developmental stage |
| `development.synaptogenesis_rate` | float | 0.001 | Synaptogenesis rate |
| `development.pruning_rate` | float | 0.0001 | Pruning rate |
| `development.replay_interval` | int | 100 | Memory replay interval (steps) |
| `development.consolidation_interval` | int | 1000 | Memory consolidation interval |
| `development.critical_period_duration` | float | 100.0 | Critical period duration |

### Reward Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `reward.discount_factor` | float | 0.99 | Reward discount factor for temporal difference learning |
| `reward.scaling` | float | 1.0 | Reward value scaling factor |
| `reward.sparsity` | float | 0.1 | Reward sparsity (probability of reward occurrence) |

### Environment Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `environment.name` | string | "GridWorld" | Environment type name |
| `environment.width` | int | 10 | Environment width (cells) |
| `environment.height` | int | 10 | Environment height (cells) |
| `environment.vision.width` | int | 8 | Agent vision width |
| `environment.vision.height` | int | 8 | Agent vision height |
| `environment.max_energy` | float | 100.0 | Maximum agent energy |
| `environment.energy_decay_rate` | float | 0.01 | Energy decay per timestep |
| `environment.obstacle_density` | float | 0.1 | Probability of obstacles |
| `environment.resource_density` | float | 0.2 | Probability of resources |

### Simulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation.max_steps` | int | 10000 | Maximum simulation steps |
| `simulation.time_limit` | float | 0.0 | Maximum simulation time (0 = unlimited) |
| `simulation.saver.enabled` | bool | true | Enable checkpoint saving |
| `simulation.saver.compression` | string | "balanced" | Compression level: none, fast, balanced, maximum |
| `simulation.saver.max_checkpoints` | int | 10 | Maximum checkpoints to keep |
| `simulation.trace.enabled` | bool | false | Enable detailed tracing |
| `simulation.trace.file` | string | "simulation.trace" | Trace file name |

### Visualization Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `visualization.enabled` | bool | false | Enable visualization |
| `visualization.update_rate` | float | 30.0 | Visualization update rate (Hz) |
| `visualization.window.width` | int | 1024 | Window width (pixels) |
| `visualization.window.height` | int | 768 | Window height (pixels) |
| `visualization.window.title` | string | "NLM Brain Simulation" | Window title |
| `visualization.neuron_radius` | float | 3.0 | Neuron display radius |
| `visualization.synapse_width` | float | 1.0 | Synapse line width |

## Configuration Parameter Types

### Integer Parameters
```cpp
// Examples
brain.neuron_count = 1000
brain.region_count = 4
simulation.max_steps = 50000
```

### Floating-Point Parameters
```cpp
// Examples
brain.connection_probability = 0.15
brain.v_thresh = -52.5
reward.discount_factor = 0.995
```

### Boolean Parameters
```cpp
// Examples
plasticity.stdp.enable = true
neuromod.curiosity.enable = false
environment.obstacle_density = 0.2
```

### String Parameters
```cpp
// Examples
environment.name = "ContinuousWorld"
log_filename = "brain_simulation.log"
environment.obstacle_type = "random"
```

### Vector Parameters
```cpp
// Examples (INI format with section brackets)
[environment.vision]
width = 8
height = 8
channels = 3

[plasticity.hebbian.weights]
min = -1.0
max = 1.0
center = 0.0
stddev = 0.1
```

## Configuration Loading Priority

Configuration values are loaded in the following priority order (higher priority wins):

1. **Command Line Arguments**: Override all other sources
   - `--section.key value` or `--section.key=value`

2. **Configuration File**: Override defaults
   - From default config file or specified file

3. **Runtime Settings**: Override file and command line
   - Programmatically set via `Config::set()`

4. **Defaults**: Built-in fallback values

## Validation Rules

### Type Validation
- Integer parameters must be valid integers
- Floating-point parameters must be valid floats
- Boolean parameters must be `true` or `false` (case-insensitive)
- String parameters can be any quoted or unquoted string

### Range Validation
```cpp
// Example validation
bool validateNeuronCount(const ConfigValue& value) {
    int neurons = std::get<int>(value);
    return neurons > 0 && neurons <= 1000000; // Reasonable limits
}

bool validateConnectionProbability(const ConfigValue& value) {
    double prob = std::get<double>(value);
    return prob >= 0.0 && prob <= 1.0;
}
```

### Dependency Validation
```cpp
// Example: STDP requires neurons > 0
bool validateSTDPWithNeurons(const Config& config) {
    int neurons = config.getOr<int>("brain.neuron_count", 1000);
    bool stdpEnabled = config.getOr<bool>("plasticity.stdp.enable", true);
    
    if (stdpEnabled && neurons == 0) {
        return false; // STDP without neurons doesn't make sense
    }
    return true;
}
```

## Example Configuration Files

### Minimal Configuration

```ini
# Minimal NLM configuration for basic testing
[general]
random_seed = 42
simulation_timestep = 0.001

[brain]
neuron_count = 100
region_count = 1

[plasticity.stdp]
enable = true
learning_rate = 0.01
```

### Research Configuration

```ini
# Configuration for research experiments
[general]
random_seed = 12345
simulation_timestep = 0.0005
log_level = INFO
log_to_file = true
log_filename = "research_simulation_001.log"

[brain]
neuron_count = 5000
region_count = 4
connection_probability = 0.05
v_thresh = -55.0
v_rest = -65.0

[plasticity]
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.018
stdp_time_constant = 25.0

[development]
synaptogenesis_rate = 0.0005
pruning_rate = 0.00005
replay_interval = 50
consolidation_interval = 500

[neuromodulation.dopamine]
scale = 1.5
baseline = 0.2

[neuromodulation.curiosity]
enable = true
scale = 2.0

[neuromodulation.novelty]
enable = true
threshold = 0.7

[environment]
name = "GridWorld"
width = 50
height = 50
vision.width = 12
vision.height = 12
max_energy = 200.0
energy_decay_rate = 0.005
obstacle_density = 0.15
resource_density = 0.3

[simulation]
max_steps = 100000
simulation.saver.enabled = true
simulation.saver.compression = "balanced"

[visualization]
enabled = true
update_rate = 10.0
```

### Performance Configuration

```ini
# High-performance configuration
[general]
random_seed = 999
simulation_timestep = 0.01  # Larger timestep for speed

[brain]
neuron_count = 20000  # Many neurons
region_count = 2
connection_probability = 0.01  # Sparse connections

[plasticity.stdp]
enable = true
learning_rate = 0.005  # Slower learning

[environment]
name = "ContinuousWorld"
width = 100
height = 100
vision.width = 16
vision.height = 16

[simulation]
max_steps = 50000

[visualization]
enabled = false  # Disable for performance
```

## Programmatic Configuration

### Creating Configuration Programmatically

```cpp
#include "nlm/core/Config/Config.hpp"
#include <memory>

void createConfiguration() {
    auto config = std::make_shared<nlm::Config>();
    
    // Set basic parameters
    config->set("brain.neuron_count", 1000);
    config->set("brain.region_count", 2);
    
    // Set nested parameters
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.stdp.learning_rate", 0.01);
    
    // Set development parameters
    config->set("development.synaptogenesis_rate", 0.001);
    config->set("development.pruning_rate", 0.0001);
    
    // Set neuromodulation
    config->set("neuromod.dopamine.scale", 1.0);
    config->set("neuromod.curiosity.enable", true);
    
    // Set environment
    config->set("environment.width", 20);
    config->set("environment.height", 20);
    config->set("environment.vision.width", 8);
    config->set("environment.vision.height", 8);
    
    // Load from file (overwrites defaults)
    config->loadFromFile("my_config.ini");
    
    // Override with command line
    config->loadFromArgs(3, (char*[]){"program", "--environment.width", "30"});
    
    // Validate configuration
    std::vector<nlm::ConfigParameter> parameters = {
        nlm::ConfigParameter("", "brain.neuron_count", "Number of neurons", "1000", true),
        nlm::ConfigParameter("plasticity.stdp", "enable", "STDP enabled", "true"),
        nlm::ConfigParameter("environment", "width", "Environment width", "10"),
        nlm::ConfigParameter("environment", "height", "Environment height", "10"),
    };
    
    if (config->validateAllParameters(parameters)) {
        // Configuration is valid
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
    } else {
        // Show errors
        auto errors = config->getErrors();
        for (const auto& error : errors) {
            std::cout << "Error: " << error << std::endl;
        }
    }
}
```

### Configuration Helper Functions

```cpp
// Common configuration presets
nlm::Config createResearchConfig() {
    auto config = std::make_shared<nlm::Config>();
    
    // Research-oriented settings
    config->set("brain.neuron_count", 10000);
    config->set("brain.region_count", 5);
    config->set("brain.connection_probability", 0.02);
    
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.stdp.learning_rate", 0.001);
    config->set("plasticity.stdp.tau_plus", 25.0);
    config->set("plasticity.stdp.tau_minus", 25.0);
    
    config->set("development.synaptogenesis_rate", 0.0002);
    config->set("development.pruning_rate", 0.00002);
    config->set("development.replay_interval", 200);
    
    config->set("neuromod.dopamine.scale", 1.0);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    
    config->set("environment.name", "GridWorld");
    config->set("environment.width", 100);
    config->set("environment.height", 100);
    
    config->set("simulation.max_steps", 200000);
    
    return config;
}

nlm::Config createPerformanceConfig() {
    auto config = std::make_shared<nlm::Config>();
    
    // Performance-oriented settings
    config->set("brain.neuron_count", 5000);
    config->set("brain.region_count", 2);
    config->set("brain.connection_probability", 0.01);
    
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.stdp.learning_rate", 0.005);
    
    config->set("environment.width", 50);
    config->set("environment.height", 50);
    config->set("environment.vision.width", 6);
    config->set("environment.vision.height", 6);
    
    config->set("simulation.max_steps", 50000);
    config->set("visualization.enabled", false);
    
    return config;
}
```

## Configuration Schema Validation

### Automatic Validation

The configuration system includes built-in validation for common parameter types:

```cpp
bool validate(const Config& config) {
    // Validate neuron count
    auto neuronCount = config.getOr<int>("brain.neuron_count", 0);
    if (neuronCount <= 0) {
        return false; // Error: neuron_count must be positive
    }
    
    // Validate connection probability
    auto connProb = config.getOr<double>("brain.connection_probability", 1.0);
    if (connProb < 0.0 || connProb > 1.0) {
        return false; // Error: connection_probability must be 0-1
    }
    
    // Validate STDP dependencies
    auto stdpEnabled = config.getOr<bool>("plasticity.stdp.enable", true);
    if (stdpEnabled && neuronCount == 0) {
        return false; // Error: STDP requires neurons
    }
    
    return true;
}
```

### Custom Validation with Callbacks

```cpp
// Define custom validators
std::function<bool(const ConfigValue&)> neuronCountValidator =
    [](const ConfigValue& value) {
        int neurons = std::get<int>(value);
        return neurons > 0 && neurons <= 1000000;
    };

std::function<bool(const ConfigValue&)> connectionValidator =
    [](const ConfigValue& value) {
        double prob = std::get<double>(value);
        return prob >= 0.0 && prob <= 1.0;
    };

std::function<bool(const ConfigValue&)> visionValidator =
    [](const ConfigValue& value) {
        // For nested vision parameters, need special handling
        return true;
    };

// Create parameter schema with validators
std::vector<ConfigParameter> schema = {
    ConfigParameter("", "brain.neuron_count", "Number of neurons", "1000", true, neuronCountValidator),
    ConfigParameter("", "brain.connection_probability", "Connection probability", "0.1", connectionValidator),
    ConfigParameter("environment.vision", "width", "Vision width", "8", visionValidator),
    ConfigParameter("environment.vision", "height", "Vision height", "8", visionValidator),
};
```

## Configuration File Format Reference

### Key-Value Pairs

```ini
# Section header
[section]

# Simple key-value pair
key = value

# Quoted string values
quoted_string = "value with spaces"
boolean = true
number = 42
float = 3.14159

# Nested parameters using dot notation
brain.neuron_count = 1000
environment.vision.width = 8
```

### Comments and Formatting

```ini
# Comments start with #
; Comments also start with ;

# Empty lines are ignored
[section]

key = value  # Inline comment

# Multiple values can be specified with different keys
[plasticity.stdp]
lr = 0.001
tau_plus = 20.0
tau_minus = 20.0
```

### Advanced Features

```ini
# Include other configuration files
#include "base_config.ini"
[overrides]
# This section overrides values from included files
brain.neuron_count = 2000

# Variable substitution (if supported by parser)
base_seed = 42
random_seed = ${base_seed}

# Environment variable substitution
env_seed = $SEED
random_seed = ${env_seed}
```

## Migration Guide

### From Old Format (Phase 1)

The old configuration format used simple key-value pairs without sections:

```ini
# Old format (Phase 1)
random_seed = 42
neuron_count = 1000
region_count = 2
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
```

Migrate to new format:

```ini
# New format
[general]
random_seed = 42

[brain]
neuron_count = 1000
region_count = 2
connection_probability = 0.1

[plasticity.stdp]
lpt_weight = 0.01
ltd_weight = 0.012
```

### Adding New Sections

To add new configuration sections:

1. **Define the section**: Create logical grouping for related parameters
2. **Add parameters**: Use dot notation for nested parameters
3. **Update documentation**: Document new parameters
4. **Add validation**: Include validation rules for new parameters

Example adding a new "reward" section:

```cpp
// Define reward configuration parameters
std::vector<ConfigParameter> rewardParameters = {
    ConfigParameter("reward", "discount_factor", "Reward discount factor", "0.99"),
    ConfigParameter("reward", "scaling", "Reward scaling factor", "1.0"),
    ConfigParameter("reward", "sparsity", "Reward sparsity probability", "0.1"),
    ConfigParameter("reward", "exploration_bonus", "Exploration bonus", "0.0"),
};
```

## Configuration File Validation

### Schema Validation

Validate configuration against a predefined schema:

```cpp
class ConfigSchema {
public:
    bool validate(const Config& config) const {
        // Check required parameters
        if (!config.has("brain.neuron_count")) {
            return false;
        }
        
        // Check parameter ranges
        auto neuronCount = config.getOr<int>("brain.neuron_count", 0);
        if (neuronCount <= 0 || neuronCount > 1000000) {
            return false;
        }
        
        // Check consistency between parameters
        auto regionCount = config.getOr<int>("brain.region_count", 1);
        if (regionCount <= 0) {
            return false;
        }
        
        return true;
    }
    
    std::vector<std::string> getErrors() const {
        return errors;
    }
    
private:
    std::vector<std::string> errors;
};
```

### Runtime Validation

Validate configuration during runtime:

```cpp
class RuntimeValidator {
public:
    bool validate(const Config& config) {
        errors.clear();
        
        if (!validateBrainParameters(config)) {
            return false;
        }
        
        if (!validatePlasticityParameters(config)) {
            return false;
        }
        
        if (!validateDevelopmentParameters(config)) {
            return false;
        }
        
        return true;
    }
    
    std::vector<std::string> getErrors() const {
        return errors;
    }
    
private:
    bool validateBrainParameters(const Config& config) {
        // Validate brain parameters
        auto neuronCount = config.getOr<int>("brain.neuron_count", 1000);
        if (neuronCount <= 0) {
            errors.push_back("brain.neuron_count must be positive");
            return false;
        }
        
        return true;
    }
    
    // ... other validation methods
    
    std::vector<std::string> errors;
};
```

## Performance Considerations

### Configuration Caching

Cache parsed configuration to avoid repeated file I/O:

```cpp
class ConfigCache {
public:
    std::shared_ptr<Config> getConfig(const std::string& filepath) {
        auto it = cache.find(filepath);
        if (it != cache.end()) {
            return it->second;
        }
        
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(filepath)) {
            cache[filepath] = config;
            return config;
        }
        
        return nullptr;
    }
    
private:
    std::unordered_map<std::string, std::shared_ptr<Config>> cache;
};
```

### Lazy Loading

Load configuration parameters only when needed:

```cpp
class LazyConfig {
public:
    int getNeuronCount() {
        if (!hasNeuronCount_) {
            // Load from file only if needed
            loadFromFile("config.ini");
        }
        return neuronCount_;
    }
    
private:
    void loadFromFile(const std::string& filepath) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        
        // Extract specific parameters
        neuronCount_ = config->getOr<int>("brain.neuron_count", 1000);
        hasNeuronCount_ = true;
    }
    
    int neuronCount_ = 1000;
    bool hasNeuronCount_ = false;
};
```

## Best Practices

### 1. Use Sections for Logical Grouping

Group related parameters in sections:

```cpp
// Good
config->set("plasticity.stdp.learning_rate", 0.001);
config->set("plasticity.hebbian.enable", true);
config->set("plasticity.structural.synaptogenesis_rate", 0.001);

// Bad (no logical grouping)
config->set("learning_rate", 0.001);
config->set("hebbian_enabled", true);
config->set("synapthogenesis_rate", 0.001);
```

### 2. Use Descriptive Parameter Names

Use clear, descriptive names:

```cpp
// Good
define("plasticity.stdp.enable", "Enable STDP", "true")
define("environment.vision.width", "Vision width in cells", "8")

// Avoid cryptic names
define("stdp_lr", "Short for STDP learning rate", "0.001")
```

### 3. Validate Configuration Early

Validate configuration as early as possible:

```cpp
void initializeSimulation() {
    auto config = std::make_shared<Config>();
    
    // Load configuration
    if (!config->loadFromFile("config.ini")) {
        std::cerr << "Failed to load configuration" << std::endl;
        return 1;
    }
    
    // Validate configuration
    if (!validateConfiguration(config)) {
        std::cerr << "Configuration validation failed:" << std::endl;
        for (const auto& error : config->getErrors()) {
            std::cerr << "  " << error << std::endl;
        }
        return 1;
    }
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Continue with simulation...
}
```

### 4. Document Configuration Parameters

Document all configuration parameters:

```cpp
// Parameter documentation
const std::unordered_map<std::string, std::string> parameterDocs = {
    {"brain.neuron_count", "Number of neurons in the brain (1-1,000,000)"},
    {"brain.connection_probability", "Probability of connection between neurons (0-1)"},
    {"plasticity.stdp.learning_rate", "STDP learning rate (typically 0.001)"},
    {"development.synaptogenesis_rate", "Rate of new synapse formation"},
    {"environment.width", "Environment width in grid cells"},
    {"environment.height", "Environment height in grid cells"},
};
```

### 5. Use Consistent Naming Conventions

Follow consistent naming conventions:

```cpp
// Use snake_case for nested parameters
config->set("brain.neuron_count", 1000);
config->set("environment.vision.width", 8);
config->set("neuromodulation.curiosity.scale", 1.0);

// Use CamelCase for section names
[Environment.Vision]
[Neuromodulation.Curiosity]
```

## Configuration File Templates

### Complete Configuration Template

```ini
# NLM Configuration Template
# ================================

[general]
# Basic settings
random_seed = 42
simulation_timestep = 0.001
log_level = INFO
log_to_file = true
log_filename = "nlm_simulation.log"

[general.performance]
# Performance tuning
simulation_max_steps = 100000
checkpoint_interval = 1000
checkpoint_compression = "balanced"

[execution]
# Execution control
max_steps = 10000
save_checkpoints = true
load_checkpoint = ""
enable_visualization = false

[brain]
# Core brain parameters
neuron_count = 1000
region_count = 2
connection_probability = 0.1
initial_weight_mean = 0.5
initial_weight_std = 0.1
v_thresh = -50.0
v_rest = -70.0
v_reset = -75.0
tau_mem = 20.0
tau_ref = 2.0

[plasticity.stdp]
# Spike-timing dependent plasticity
enable = true
learning_rate = 0.001
tau_plus = 20.0
tau_minus = 20.0
ltp_weight = 0.01
ltd_weight = 0.012

[plasticity.hebbian]
# Hebbian learning
enable = true
learning_rate = 0.01

[plasticity.structural]
# Structural plasticity
enable = true
synaptogenesis_rate = 0.001
pruning_rate = 0.0001
max_synapses_per_neuron = 50

[development]
# Development processes
enabled = true
initial_stage = "Initial"
synaptogenesis_rate = 0.001
pruning_rate = 0.0001
replay_interval = 100
consolidation_interval = 1000

[neuromodulation.dopamine]
# Dopamine system
scale = 1.0
baseline = 0.1
plasticity_factor = 1.0

[neuromodulation.curiosity]
# Curiosity system
enable = true
scale = 1.0
exploration_bonus = 0.0

[neuromodulation.novelty]
# Novelty detection
enable = true
threshold = 0.5
adaptation_rate = 0.01

[neuromodulation.prediction_error]
# Prediction error
scale = 1.0

[environment]
# Simulation environment
name = "GridWorld"
width = 10
height = 10
vision.width = 8
vision.height = 8
vision.channels = 3
max_energy = 100.0
energy_decay_rate = 0.01
obstacle_density = 0.1
resource_density = 0.2
hazard_density = 0.05

[reward]
# Reward system
discount_factor = 0.99
scaling = 1.0
sparsity = 0.1
exploration_bonus = 0.0

[simulation.saver]
# Checkpoint saving
save_interval = 1000
compression = "balanced"
max_checkpoints = 10

[visualization]
# Visualization settings
enabled = false
update_rate = 30.0
window.width = 1024
window.height = 768
window.title = "NLM Brain Simulation"
neuron_radius = 3.0
synapse_width = 1.0

[logging]
# Logging configuration
level = "INFO"
console_output = true
file_output = true
file = "nlm.log"
max_file_size = 10485760  # 10MB
backup_count = 5
```

### Minimal Configuration Template

```ini
# Minimal NLM Configuration
[general]
random_seed = 42

[brain]
neuron_count = 100
region_count = 1

[plasticity.stdp]
enable = true
learning_rate = 0.01

[environment]
width = 10
height = 10
vision.width = 8
vision.height = 8
```

### Research Configuration Template

```ini
# Research Configuration
[general]
random_seed = 12345
simulation_timestep = 0.0005
log_level = DEBUG
log_to_file = true

[brain]
neuron_count = 10000
region_count = 5
connection_probability = 0.02

[plasticity.stdp]
enable = true
learning_rate = 0.001
tau_plus = 25.0
tau_minus = 25.0

[plasticity.structural]
synaptogenesis_rate = 0.0002
pruning_rate = 0.00002

[development]
synaptogenesis_rate = 0.0005
pruning_rate = 0.00005
replay_interval = 200

[neuromodulation.dopamine]
scale = 1.5

[neuromodulation.curiosity]
enable = true
scale = 2.0

[environment]
name = "GridWorld"
width = 100
height = 100
vision.width = 12
vision.height = 12
max_energy = 200.0
```

## Configuration Validation Examples

### 1. Validate Configuration Schema

```cpp
#include <nlm/core/Config/Config.hpp>

bool validateConfiguration(const nlm::Config& config) {
    // Check required parameters
    if (!config.has("brain.neuron_count")) {
        config.addError("Missing required parameter: brain.neuron_count");
        return false;
    }
    
    if (!config.has("environment.width")) {
        config.addError("Missing required parameter: environment.width");
        return false;
    }
    
    // Validate parameter ranges
    auto neuronCount = config.getOr<int>("brain.neuron_count", 0);
    if (neuronCount <= 0) {
        config.addError("brain.neuron_count must be positive");
        return false;
    }
    
    auto connProb = config.getOr<double>("brain.connection_probability", 1.0);
    if (connProb < 0.0 || connProb > 1.0) {
        config.addError("brain.connection_probability must be between 0 and 1");
        return false;
    }
    
    auto width = config.getOr<int>("environment.width", 0);
    auto height = config.getOr<int>("environment.height", 0);
    if (width <= 0 || height <= 0) {
        config.addError("Environment dimensions must be positive");
        return false;
    }
    
    return true;
}
```

### 2. Validate Configuration Dependencies

```cpp
bool validateConfigurationDependencies(const nlm::Config& config) {
    // If STDP is enabled, ensure neurons > 0
    auto stdpEnabled = config.getOr<bool>("plasticity.stdp.enable", true);
    auto neuronCount = config.getOr<int>("brain.neuron_count", 1000);
    
    if (stdpEnabled && neuronCount == 0) {
        config.addError("STDP requires at least one neuron");
        return false;
    }
    
    // If curiosity is enabled, ensure dopamine baseline is reasonable
    auto curiosityEnabled = config.getOr<bool>("neuromod.curiosity.enable", true);
    if (curiosityEnabled) {
        auto dopamineBaseline = config.getOr<double>("neuromod.dopamine.baseline", 0.1);
        if (dopamineBaseline < 0.0 || dopamineBaseline > 10.0) {
            config.addError("Neuromod.dopamine.baseline should be between 0 and 10");
            return false;
        }
    }
    
    return true;
}
```

### 3. Validate Configuration Against Schema

```cpp
class ConfigurationSchema {
public:
    bool validate(const nlm::Config& config) const {
        return validateGeneral(config) &&
               validateBrain(config) &&
               validatePlasticity(config) &&
               validateEnvironment(config);
    }
    
    std::vector<std::string> getErrors() const {
        return errors;
    }
    
private:
    bool validateGeneral(const nlm::Config& config) const {
        // Validate general parameters
        auto randomSeed = config.getOr<int64_t>("random_seed", 42);
        if (randomSeed < 0) {
            errors.push_back("random_seed must be non-negative");
            return false;
        }
        
        return true;
    }
    
    bool validateBrain(const nlm::Config& config) const {
        // Validate brain parameters
        auto neuronCount = config.getOr<int>("brain.neuron_count", 1000);
        if (neuronCount <= 0 || neuronCount > 1000000) {
            errors.push_back("brain.neuron_count must be between 1 and 1,000,000");
            return false;
        }
        
        return true;
    }
    
    bool validatePlasticity(const nlm::Config& config) const {
        // Validate plasticity parameters
        auto stdpLTP = config.getOr<double>("plasticity.stdp.ltp_weight", 0.01);
        if (stdpLTP <= 0.0) {
            errors.push_back("plasticity.stdp.ltp_weight must be positive");
            return false;
        }
        
        return true;
    }
    
    bool validateEnvironment(const nlm::Config& config) const {
        // Validate environment parameters
        auto visionWidth = config.getOr<int>("environment.vision.width", 8);
        auto visionHeight = config.getOr<int>("environment.vision.height", 8);
        if (visionWidth <= 0 || visionHeight <= 0) {
            errors.push_back("environment.vision dimensions must be positive");
            return false;
        }
        
        return true;
    }
    
    mutable std::vector<std::string> errors;
};
```

## Configuration File Handling

### Loading Configuration Files

```cpp
// Load configuration from file
#include <nlm/core/Config/Config.hpp>

std::shared_ptr<nlm::Config> loadConfiguration(const std::string& filepath) {
    auto config = std::make_shared<nlm::Config>();
    
    if (config->loadFromFile(filepath)) {
        return config;
    } else {
        // Handle error
        auto errors = config->getErrors();
        for (const auto& error : errors) {
            std::cerr << "Error loading configuration: " << error << std::endl;
        }
        return nullptr;
    }
}
```

### Saving Configuration Files

```cpp
// Save configuration to file
bool saveConfiguration(const nlm::Config& config, const std::string& filepath) {
    return config.saveToFile(filepath);
}
```

### Merging Configuration Files

```cpp
// Merge configuration files (command line overrides file, file overrides defaults)
std::shared_ptr<nlm::Config> mergeConfigurations(
    const std::string& defaultConfigPath,
    const std::string& userConfigPath,
    int argc, char** argv) {
    
    auto config = std::make_shared<nlm::Config>();
    
    // Load default configuration
    config->loadFromFile(defaultConfigPath);
    
    // Override with user configuration
    if (!userConfigPath.empty()) {
        config->loadFromFile(userConfigPath);
    }
    
    // Override with command line arguments
    config->loadFromArgs(argc, argv);
    
    return config;
}
```

### Configuration File Paths

```cpp
// Configuration file path management
class ConfigPaths {
public:
    static std::string getDefaultConfigPath() {
        return "configs/default.cfg";
    }
    
    static std::string getUserConfigPath() {
        // Check environment variable first
        const char* envPath = std::getenv("NLM_CONFIG_PATH");
        if (envPath != nullptr && std::strlen(envPath) > 0) {
            return envPath;
        }
        
        // Check user directory
        const char* home = std::getenv("HOME");
        if (home != nullptr) {
            return std::string(home) + "/.config/nlm/config.ini";
        }
        
        return ".nlm_config";
    }
    
    static std::string getEnvironmentVariable(const std::string& name) {
        const char* value = std::getenv(name.c_str());
        return value != nullptr ? value : "";
    }
    
    static bool setEnvironmentVariable(const std::string& name, const std::string& value) {
        return std::setenv(name.c_str(), value.c_str(), 1) == 0;
    }
};
```

## Configuration Parameter Reference

### General Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `random_seed` | int | 42 | Random seed for reproducibility |
| `simulation_timestep` | double | 0.001 | Simulation timestep |

### Brain Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.region_count` | int | 1 | Number of neural regions |
| `brain.connection_probability` | float | 0.1 | Connection probability |

### Plasticity Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP LTP time constant |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP LTD time constant |

### Neuromodulation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine scale factor |
| `neuromod.dopamine.baseline` | float | 0.1 | Dopamine baseline |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity |

### Development Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.synaptogenesis_rate` | float | 0.001 | Synaptogenesis rate |
| `development.pruning_rate` | float | 0.0001 | Pruning rate |

### Environment Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `environment.name` | string | "GridWorld" | Environment name |
| `environment.width` | int | 10 | Environment width |
| `environment.height` | int | 10 | Environment height |
| `environment.vision.width` | int | 8 | Vision width |
| `environment.vision.height` | int | 8 | Vision height |

### Simulation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation.max_steps` | int | 10000 | Maximum simulation steps |
| `simulation.time_limit` | float | 0.0 | Time limit |

### Visualization Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `visualization.enabled` | bool | false | Enable visualization |
| `visualization.update_rate` | float | 30.0 | Update rate |

## Configuration Migration Guide

### From Phase 1 to Phase 6

Configuration parameters have evolved from Phase 1 to Phase 6:

#### Phase 1 Parameters (Deprecated)

```ini
# Old Phase 1 format
random_seed = 42
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
```

#### Phase 6 Parameters (New Format)

```ini
# New Phase 6 format
[general]
random_seed = 42

[brain]
neuron_count = 1000
region_count = 2
connection_probability = 0.1

[plasticity.stdp]
learning_rate = 0.01
ltp_weight = 0.01
ltd_weight = 0.012
```

#### Migration Strategy

1. **Update parameter names**: Convert `neuron_count` to `brain.neuron_count`
2. **Add sections**: Group related parameters in sections
3. **Add new parameters**: Include new Phase 6 parameters
4. **Update validation**: Add validation for new parameter types

### Sample Migration Script

```cpp
// Configuration migration script
void migrateConfiguration(const nlm::Config& oldConfig, nlm::Config& newConfig) {
    // Copy general parameters
    if (oldConfig.has("random_seed")) {
        newConfig.set("general", "random_seed", 
                     oldConfig.getOr<int>("random_seed", 42));
    }
    
    // Copy brain parameters
    if (oldConfig.has("neuron_count")) {
        newConfig.set("brain", "neuron_count",
                     oldConfig.getOr<int>("neuron_count", 1000));
    }
    
    if (oldConfig.has("region_count")) {
        newConfig.set("brain", "region_count",
                     oldConfig.getOr<int>("region_count", 1));
    }
    
    if (oldConfig.has("connection_probability")) {
        newConfig.set("brain", "connection_probability",
                     oldConfig.getOr<double>("connection_probability", 0.1));
    }
    
    // Copy plasticity parameters
    if (oldConfig.has("plasticity_learning_rate")) {
        newConfig.set("plasticity.stdp", "learning_rate",
                     oldConfig.getOr<double>("plasticity_learning_rate", 0.01));
    }
    
    if (oldConfig.has("stdp_ltp_weight")) {
        newConfig.set("plasticity.stdp", "ltp_weight",
                     oldConfig.getOr<double>("stdp_ltp_weight", 0.01));
    }
    
    if (oldConfig.has("stdp_ltd_weight")) {
        newConfig.set("plasticity.stdp", "ltd_weight",
                     oldConfig.getOr<double>("stdp_ltd_weight", 0.012));
    }
    
    // Add new Phase 6 parameters with defaults
    newConfig.set("plasticity.stdp", "enable", true);
    newConfig.set("development", "synaptogenesis_rate", 0.001);
    newConfig.set("development", "pruning_rate", 0.0001);
}
```

## Configuration Best Practices

### 1. Use Descriptive Parameter Names

Use clear, descriptive names that indicate the parameter's purpose:

```cpp
// Good
config->set("plasticity.stdp.learning_rate", 0.001);
config->set("environment.vision.width", 8);
config->set("neuromod.dopamine.scale", 1.0);

// Avoid cryptic names
config->set("lr", 0.001);  // Too vague
```

### 2. Document All Parameters

Document all configuration parameters with clear descriptions:

```cpp
const std::unordered_map<std::string, std::string> parameterDocumentation = {
    {"general.random_seed", "Random seed for reproducible experiments"},
    {"brain.neuron_count", "Total number of neurons in the brain"},
    {"brain.region_count", "Number of neural regions"},
    {"brain.connection_probability", "Probability of connection between neurons"},
    {"plasticity.stdp.learning_rate", "STDP learning rate"},
    {"plasticity.stdp.tau_plus", "STDP LTP time constant (ms)"},
    {"plasticity.stdp.tau_minus", "STDP LTD time constant (ms)"},
    {"development.synaptogenesis_rate", "Rate of new synapse formation"},
    {"development.pruning_rate", "Rate of synapse elimination"},
    {"environment.name", "Environment name"},
    {"environment.width", "Environment width in cells"},
    {"environment.height", "Environment height in cells"},
    {"environment.vision.width", "Vision width in cells"},
    {"environment.vision.height", "Vision height in cells"},
};
```

### 3. Validate Configuration Early

Validate configuration early in the application:

```cpp
bool validateConfiguration(const nlm::Config& config) {
    // Validate general parameters
    if (!validateGeneral(config)) {
        return false;
    }
    
    // Validate brain parameters
    if (!validateBrain(config)) {
        return false;
    }
    
    // Validate plasticity parameters
    if (!validatePlasticity(config)) {
        return false;
    }
    
    // Validate environment parameters
    if (!validateEnvironment(config)) {
        return false;
    }
    
    return true;
}
```

### 4. Use Consistent Naming Conventions

Follow consistent naming conventions:

```cpp
// Use snake_case for parameter keys
config->set("random_seed", 42);
config->set("simulation_timestep", 0.001);

// Use CamelCase for section names
[General]
[Environment.Vision]
[Plasticity.STDP]
```

### 5. Test Configuration Loading

Test configuration loading with various scenarios:

```cpp
void testConfigurationLoading() {
    // Test loading from file
    auto config1 = std::make_shared<nlm::Config>();
    assert(config1->loadFromFile("valid_config.ini"));
    
    // Test loading from command line
    auto config2 = std::make_shared<nlm::Config>();
    assert(config2->loadFromArgs(3, (char*[]){"program", "--brain.neuron_count", "500"}));
    
    // Test merging configurations
    auto config3 = std::make_shared<nlm::Config>();
    config3->loadFromFile("base.ini");
    config3->loadFromArgs(3, (char*[]){"program", "--brain.neuron_count", "1000"}));
    
    assert(config3->getOr<int>("brain.neuron_count", 0) == 1000);
}
```

### 6. Handle Configuration Errors Gracefully

Handle configuration errors gracefully:

```cpp
bool safeLoadConfiguration(const std::string& filepath) {
    auto config = std::make_shared<nlm::Config>();
    
    if (!config->loadFromFile(filepath)) {
        std::cerr << "Failed to load configuration from " << filepath << ":" << std::endl;
        
        auto errors = config->getErrors();
        for (const auto& error : errors) {
            std::cerr << "  " << error << std::endl;
        }
        
        return false;
    }
    
    return true;
}
```

## Configuration Examples

### Complete Configuration Example

```ini
# Complete NLM Configuration Example
# ================================

[general]
random_seed = 12345
simulation_timestep = 0.0005
log_level = DEBUG
log_to_file = true
log_filename = "nlm_simulation.log"

[general.performance]
simulation_max_steps = 100000
checkpoint_interval = 1000

[execution]
max_steps = 10000
save_checkpoints = true

[brain]
neuron_count = 10000
region_count = 5
connection_probability = 0.02
initial_weight_mean = 0.5
initial_weight_std = 0.1
v_thresh = -55.0
v_rest = -65.0
v_reset = -70.0
tau_mem = 20.0
tau_ref = 5.0

[plasticity.stdp]
enable = true
learning_rate = 0.001
tau_plus = 25.0
tau_minus = 25.0
ltp_weight = 0.015
ltd_weight = 0.018

[plasticity.hebbian]
enable = true
learning_rate = 0.01

[plasticity.structural]
enable = true
synaptogenesis_rate = 0.0002
pruning_rate = 0.00002
max_synapses_per_neuron = 50

[development]
enabled = true
initial_stage = "Initial"
synaptogenesis_rate = 0.0005
pruning_rate = 0.00005
replay_interval = 200
consolidation_interval = 500

[neuromodulation.dopamine]
scale = 1.5
baseline = 0.2
plasticity_factor = 1.0

[neuromodulation.curiosity]
enable = true
scale = 2.0
exploration_bonus = 0.0

[neuromodulation.novelty]
enable = true
threshold = 0.7
adaptation_rate = 0.01

[neuromodulation.prediction_error]
scale = 1.0

[environment]
name = "GridWorld"
width = 100
height = 100
vision.width = 12
vision.height = 12
vision.channels = 3
max_energy = 200.0
energy_decay_rate = 0.005
obstacle_density = 0.15
resource_density = 0.3

[reward]
discount_factor = 0.99
scaling = 1.0
sparsity = 0.1
exploration_bonus = 0.0

[simulation.saver]
save_interval = 1000
compression = "balanced"
max_checkpoints = 10

[visualization]
enabled = true
update_rate = 10.0
window.width = 1280
window.height = 720
window.title = "NLM Brain Simulation"
neuron_radius = 3.0
synapse_width = 1.0
```

### Minimal Configuration Example

```ini
# Minimal NLM Configuration
[general]
random_seed = 42

[execution]
max_steps = 1000

[brain]
neuron_count = 100
region_count = 1

[plasticity.stdp]
enable = true
learning_rate = 0.01

[environment]
width = 20
height = 20
vision.width = 8
vision.height = 8
```

## Configuration Schema Validation

### Schema Definition

```cpp
struct ConfigurationSchema {
    std::vector<ConfigParameter> parameters;
    
    bool validate(const nlm::Config& config) const {
        for (const auto& param : parameters) {
            if (!validateParameter(config, param)) {
                return false;
            }
        }
        return true;
    }
    
private:
    bool validateParameter(const nlm::Config& config, const ConfigParameter& param) const {
        // Check if parameter exists
        if (param.required && !config.has(param.section, param.key)) {
            errors.push_back("Missing required parameter: " + param.section + "." + param.key);
            return false;
        }
        
        // Validate parameter type and range
        if (config.has(param.section, param.key)) {
            auto value = config.get<std::string>(param.section, param.key);
            if (!validateValue(value, param)) {
                return false;
            }
        }
        
        return true;
    }
    
    bool validateValue(const std::string& value, const ConfigParameter& param) const {
        // Validate value against type and constraints
        if (param.validator) {
            // Convert value to appropriate type (simplified)
            if (param.defaultValue.find('.') != std::string::npos) {
                double doubleValue = std::stod(value);
                return param.validator(doubleValue);
            } else {
                int intValue = std::stoi(value);
                return param.validator(intValue);
            }
        }
        return true;
    }
    
    mutable std::vector<std::string> errors;
};
```

### Creating Configuration Schema

```cpp
// Create configuration schema for validation
ConfigurationSchema createConfigurationSchema() {
    ConfigurationSchema schema;
    
    // Required parameters
    schema.parameters.push_back({
        "", "brain.neuron_count", "Number of neurons", "1000", true,
        [](const ConfigValue& v) {
            int neurons = std::get<int>(v);
            return neurons > 0 && neurons <= 1000000;
        }
    });
    
    // Optional parameters with defaults
    schema.parameters.push_back({
        "brain", "connection_probability", "Connection probability", "0.1",
        false, [](const ConfigValue& v) {
            double prob = std::get<double>(v);
            return prob >= 0.0 && prob <= 1.0;
        }
    });
    
    // Nested parameters
    schema.parameters.push_back({
        "environment.vision", "width", "Vision width", "8", false
    });
    
    schema.parameters.push_back({
        "environment.vision", "height", "Vision height", "8", false
    });
    
    return schema;
}
```

## Configuration Testing

### Unit Testing Configuration

```cpp
#include <nlm/core/Config/Config.hpp>
#include <gtest/gtest.h>

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<nlm::Config>();
    }
    
    std::shared_ptr<nlm::Config> config;
};

TEST_F(ConfigTest, DefaultConfig) {
    ASSERT_EQ(config->getOr<int>("brain.neuron_count", 0), 1000);
    ASSERT_EQ(config->getOr<int>("brain.region_count", 0), 1);
    ASSERT_EQ(config->getOr<double>("brain.connection_probability", 0.0), 0.1);
}

TEST_F(ConfigTest, SetAndGet) {
    config->set("brain.neuron_count", 500);
    EXPECT_EQ(config->getOr<int>("brain.neuron_count", 0), 500);
}

TEST_F(ConfigTest, Has) {
    config->set("test.param", "value");
    EXPECT_TRUE(config->has("test.param"));
    EXPECT_FALSE(config->has("nonexistent.param"));
}

TEST_F(ConfigTest, Remove) {
    config->set("test.param", "value");
    config->remove("test.param");
    EXPECT_FALSE(config->has("test.param"));
}

TEST_F(ConfigTest, GetKeys) {
    config->set("param1", "value1");
    config->set("param2", "value2");
    
    auto keys = config->getKeys();
    EXPECT_EQ(keys.size(), 2);
    EXPECT_TRUE(std::find(keys.begin(), keys.end(), "param1") != keys.end());
    EXPECT_TRUE(std::find(keys.begin(), keys.end(), "param2") != keys.end());
}

TEST_F(ConfigTest, SectionSupport) {
    config->set("brain", "neuron_count", 2000);
    config->set("environment", "width", 30);
    
    EXPECT_EQ(config->getOr<int>("brain.neuron_count", 0), 2000);
    EXPECT_EQ(config->getOr<int>("environment.width", 0), 30);
    
    auto brainKeys = config->getKeys("brain");
    EXPECT_EQ(brainKeys.size(), 1);
    EXPECT_EQ(brainKeys[0], "neuron_count");
    
    auto envKeys = config->getKeys("environment");
    EXPECT_EQ(envKeys.size(), 1);
    EXPECT_EQ(envKeys[0], "width");
}

TEST_F(ConfigTest, LoadFromFile) {
    // Create a temporary config file
    std::ofstream file("test_config.ini");
    file << "[brain]\n";
    file << "neuron_count = 500\n";
    file << "region_count = 2\n";
    file.close();
    
    ASSERT_TRUE(config->loadFromFile("test_config.ini"));
    EXPECT_EQ(config->getOr<int>("brain.neuron_count", 0), 500);
    EXPECT_EQ(config->getOr<int>("brain.region_count", 0), 2);
    
    // Clean up
    std::remove("test_config.ini");
}

TEST_F(ConfigTest, SaveToFile) {
    config->set("brain.neuron_count", 1000);
    config->set("brain.region_count", 3);
    
    ASSERT_TRUE(config->saveToFile("test_save.ini"));
    
    // Load back and verify
    auto config2 = std::make_shared<nlm::Config>();
    ASSERT_TRUE(config2->loadFromFile("test_save.ini"));
    EXPECT_EQ(config2->getOr<int>("brain.neuron_count", 0), 1000);
    EXPECT_EQ(config2->getOr<int>("brain.region_count", 0), 3);
    
    // Clean up
    std::remove("test_save.ini");
}
```

### Integration Testing

```cpp
// Integration test for configuration with Brain
#include <nlm/core/Brain.hpp>

TEST(ConfigurationIntegration, BrainWithConfig) {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.neuron_count", 100);
    config->set("brain.region_count", 1);
    
    // Create brain with configuration
    auto brain = std::make_shared<nlm::Brain>(config);
    ASSERT_TRUE(brain->initialize());
    
    // Verify brain configuration matches
    EXPECT_EQ(brain->getConfig()->getOr<int>("brain.neuron_count", 0), 100);
    EXPECT_EQ(brain->getConfig()->getOr<int>("brain.region_count", 0), 1);
    
    // Verify brain statistics
    EXPECT_EQ(brain->getTotalNeuronCount(), 100);
    EXPECT_EQ(brain->getRegionCount(), 1);
}
```

## Configuration Migration Examples

### Migrate from Old Configuration Format

```cpp
// Migration script for old configuration format
class OldToNewConfigConverter {
public:
    static std::shared_ptr<nlm::Config> migrate(const std::string& oldConfigPath) {
        auto config = std::make_shared<nlm::Config>();
        
        // Load old configuration
        auto oldConfig = std::make_shared<nlm::Config>();
        oldConfig->loadFromFile(oldConfigPath);
        
        // Migrate general parameters
        if (oldConfig->has("random_seed")) {
            config->set("general", "random_seed",
                       oldConfig->getOr<int64_t>("random_seed", 42));
        }
        
        // Migrate brain parameters
        if (oldConfig->has("neuron_count")) {
            config->set("brain", "neuron_count",
                       oldConfig->getOr<int>("neuron_count", 1000));
        }
        
        if (oldConfig->has("region_count")) {
            config->set("brain", "region_count",
                       oldConfig->getOr<int>("region_count", 1));
        }
        
        if (oldConfig->has("connection_probability")) {
            config->set("brain", "connection_probability",
                       oldConfig->getOr<double>("connection_probability", 0.1));
        }
        
        // Migrate plasticity parameters
        if (oldConfig->has("plasticity_learning_rate")) {
            config->set("plasticity.stdp", "learning_rate",
                       oldConfig->getOr<double>("plasticity_learning_rate", 0.01));
        }
        
        if (oldConfig->has("stdp_ltp_weight")) {
            config->set("plasticity.stdp", "ltp_weight",
                       oldConfig->getOr<double>("stdp_ltp_weight", 0.01));
        }
        
        if (oldConfig->has("stdp_ltd_weight")) {
            config->set("plasticity.stdp", "ltd_weight",
                       oldConfig->getOr<double>("stdp_ltd_weight", 0.012));
        }
        
        // Migrate development parameters
        if (oldConfig->has("development_synaptogenesis_rate")) {
            config->set("development", "synaptogenesis_rate",
                       oldConfig->getOr<double>("development_synaptogenesis_rate", 0.001));
        }
        
        if (oldConfig->has("development_pruning_rate")) {
            config->set("development", "pruning_rate",
                       oldConfig->getOr<double>("development_pruning_rate", 0.0001));
        }
        
        // Add new Phase 6 parameters with defaults
        config->set("plasticity.stdp", "enable", true);
        config->set("plasticity.hebbian", "enable", true);
        config->set("plasticity.structural", "enable", true);
        config->set("development", "enabled", true);
        config->set("neuromodulation.curiosity", "enable", true);
        
        return config;
    }
};
```

## Conclusion

The NLM configuration system provides a flexible, structured approach to managing brain simulation parameters. Key improvements include:

1. **Section-based organization**: Parameters are logically grouped for better maintainability
2. **Type safety**: Strong type checking with validation
3. **Backward compatibility**: Support for old configuration formats
4. **Extensibility**: Easy addition of new parameters and sections
5. **Validation**: Comprehensive parameter validation with error reporting
6. **Testing**: Full test coverage for configuration functionality

The configuration system is now ready for production use in Phase 6 and beyond, supporting the complex requirements of modern neural simulation research.
