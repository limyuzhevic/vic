# NLM Configuration Guide

## Overview

The NLM configuration system is a powerful, type-safe parameter management framework that controls all aspects of the neural simulation. It provides validation, multiple configuration sources, and section-based organization for managing complex brain parameters.

## Configuration Architecture

### Core Components

1. **Config Class** - Main configuration manager
2. **ConfigValue Type** - Union for parameter values (int, double, string, bool, arrays)
3. **ParameterMeta** - Parameter metadata with validation and defaults
4. **ConfigSource** - Source enumeration (Default, File, CommandLine, Runtime)

### Configuration Sources

| Source | Description | Priority |
|--------|-------------|----------|
| Default | Built-in default values | Highest |
| File | Loaded from configuration files | Medium |
| CommandLine | Command line arguments | High |
| Runtime | Set during program execution | Lowest |

### Configuration Hierarchy

```
Brain
  ├── neuron.*
  ├── synapse.*
  ├── plasticity.*
  ├── neuromod.*
  ├── memory.*
  ├── prediction.*
  ├── cognitive.*
  └── development.*

World
  ├── simple_world.*
  └── environment.*

Experiments
  ├── phase3.*
  ├── phase4.*
  └── phase6.*
```

## Configuration System Features

### Type Safety

- Strongly typed parameter storage
- Automatic validation against type constraints
- Template-based getter/setter methods

### Validation

- Range validation (float, int)
- Custom validator functions
- Required parameter checking

### Multiple Input Sources

- File loading with simple key=value format
- Command line parsing (--key=value, -k value)
- Runtime configuration during execution

### Serialization

- Save configuration to files
- Load configuration from files
- Summary generation for debugging

## Configuration Parameters

### Brain Configuration

#### Neuron Parameters

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `neuron.membrane_capacitance` | float | 1.0f | Membrane capacitance (nF) | [0.1f, 10.0f] |
| `neuron.time_constant` | float | 20.0f | Membrane time constant (ms) | [1.0f, 100.0f] |
| `neuron.max_spike_history` | int | 100 | Maximum spike history entries | [10, 1000] |
| `neuron.resting_potential_default` | float | -70.0f | Default resting potential (mV) | [-90.0f, -50.0f] |
| `neuron.threshold_default` | float | -55.0f | Default firing threshold (mV) | [-80.0f, -40.0f] |
| `neuron.reset_potential_default` | float | -70.0f | Reset potential (mV) | [-90.0f, -50.0f] |
| `neuron.leak_conductance_default` | float | 10.0f | Default leak conductance (nS) | [1.0f, 100.0f] |
| `neuron.adaptation_rate` | float | 0.01f | Spike-frequency adaptation rate | [0.001f, 0.1f] |
| `neuron.refractory_period_default` | int | 5 | Default refractory period (steps) | [1, 50] |

#### Synapse Parameters

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `synapse.weight_min` | float | -1.0f | Minimum synaptic weight | [-10.0f, 0.0f] |
| `synapse.weight_max` | float | 1.0f | Maximum synaptic weight | [0.0f, 10.0f] |
| `synapse.stp_facilitation_tau` | float | 100.0f | STP facilitation time constant (ms) | [10.0f, 500.0f] |
| `synapse.stp_depression_tau` | float | 200.0f | STP depression time constant (ms) | [50.0f, 500.0f] |
| `synapse.stp_u_max` | float | 1.0f | Short-term plasticity maximum utilization | [0.1f, 5.0f] |
| `synapse.max_spike_history` | int | 100 | Maximum spike history entries | [10, 1000] |
| `synapse.efficiency_range_min` | float | 0.5f | Synaptic efficacy minimum range | [0.1f, 1.0f] |
| `synapse.efficiency_range_max` | float | 2.0f | Synaptic efficacy maximum range | [1.0f, 5.0f] |

### Plasticity Configuration

#### STDP Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP learning |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau` | float | 20.0f | STDP time constant (ms) |

#### Hebbian Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.01f | Hebbian learning rate |
| `plasticity.hebbian.max_weight` | float | 1.0f | Maximum weight for Hebbian |

#### Structural Plasticity Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.synaptogenesis_rate` | float | 0.0001f | Synapse formation rate |
| `plasticity.structural.pruning_rate` | float | 0.00001f | Synapse removal rate |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0f | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### Memory Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working.enable` | bool | true | Enable working memory |
| `memory.working.capacity` | int | 100 | Working memory capacity |
| `memory.episodic.enabled` | bool | true | Enable episodic memory |
| `memory.associative.enabled` | bool | true | Enable associative memory |
| `memory.concept.enabled` | bool | true | Enable concept formation |

### Prediction Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `prediction.neural.enabled` | bool | true | Enable neural prediction |
| `prediction.action.enabled` | bool | true | Enable action prediction |
| `prediction.horizon` | int | 10 | Prediction time horizon |

### Cognitive Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `cognitive.attention.enabled` | bool | true | Enable attention system |
| `cognitive.attention.competition_strength` | float | 1.0f | Attention competition strength |
| `cognitive.planning.enabled` | bool | true | Enable planning system |
| `cognitive.planning.horizon` | int | 5 | Planning horizon |
| `cognitive.selfmodel.enabled` | bool | true | Enable self-model |
| `cognitive.social.enabled` | bool | true | Enable social learning |

### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.stage` | int | 1 | Initial developmental stage |
| `development.plasticity_modulation` | bool | true | Modulate plasticity with development |
| `development.critical_period_end` | int | 100 | End of critical period |
| `development.adult_maturation` | bool | true | Mature into adult state |

### World Configuration

#### SimpleWorld Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `world.simple.width` | int | 20 | World width (cells) |
| `world.simple.height` | int | 20 | World height (cells) |
| `world.simple.vision_width` | int | 8 | Vision width (cells) |
| `world.simple.vision_height` | int | 8 | Vision height (cells) |
| `world.simple.max_energy` | float | 100.0f | Maximum world energy |

### Experiment Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `experiment.phase` | int | 2 | Current experiment phase |
| `experiment.steps` | int | 1000 | Simulation steps |
| `experiment.record_metrics` | bool | true | Record performance metrics |

## Configuration Usage

### Creating and Using Config

#### Basic Usage

```cpp
#include <nlm/core/Config/Config.hpp>

// Create configuration
auto config = std::make_shared<Config>();

// Set values
config->set("neuron_count", 1000, ConfigSource::Default);
config->set("plasticity.stdp.enable", true, ConfigSource::File);
config->set("synapse.weight_min", -5.0f, ConfigSource::CommandLine);

// Get values
int neuronCount = config->getOr<int>("neuron_count", 1000);
float learningRate = config->getOr<float>("plasticity.stdp.learning_rate", 0.001f);

// Check existence
if (config->has("neuron_count")) {
    std::cout << "Neuron count: " << config->getOr<int>("neuron_count", 0) << std::endl;
}
```

#### Loading from File

```cpp
// Create configuration
auto config = std::make_shared<Config>();

// Load from file (simple key=value format)
if (config->loadFromFile("configs/my_config.cfg")) {
    std::cout << "Configuration loaded successfully" << std::endl;
} else {
    std::cout << "Using default configuration" << std::endl;
}

// File format example (configs/my_config.cfg)
# NLM Configuration File
# -----------------------

# Brain configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 1

# Neuron parameters
neuron.time_constant = 20.0f
neuron.resting_potential_default = -70.0f
neuron.threshold_default = -55.0f

# Plasticity parameters
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.hebbian.enable = true
plasticity.hebbian.learning_rate = 0.01f

# Neuromodulation parameters
neuromod.dopamine.scale = 1.0f
neuromod.curiosity.enable = true
```

#### Loading from Command Line

```bash
# Command line arguments
./nlm --neuron_count=2000 --plasticity.stdp.learning_rate=0.002 \
     --world.simple.width=30 --world.simple.height=30

// Program code to parse
int main(int argc, char** argv) {
    auto config = std::make_shared<Config>();
    
    // Parse command line arguments
    config->loadFromArgs(argc, argv);
    
    // Can override with runtime settings
    config->set("world.simple.vision_width", 16, ConfigSource::Runtime);
    
    // Initialize brain with configuration
    auto brain = std::make_shared<Brain>(config);
}
```

### Configuration Validation

#### Parameter Validation

```cpp
auto config = std::make_shared<Config>();

// Set value (will validate if parameter is registered)
config->set("neuron.time_constant", 50.0f, ConfigSource::Runtime);

// Check if parameter is valid
if (config->has("neuron.time_constant")) {
    float tau = config->getOr<float>("neuron.time_constant", 20.0f);
    std::cout << "Time constant: " << tau << " ms" << std::endl;
}

// Ensure required parameters
config->ensureRequiredParameters("neuron");
```

#### Custom Validation

```cpp
// Register custom parameter with validation
config->registerParameter("brain.custom", ParameterMeta(
    "brain.custom.value",
    "Custom brain parameter with range validation",
    0.5f,
    config_validation::createFloatRangeValidator(0.0f, 1.0f),  // Custom validator
    true  // Required parameter
));

// This will throw exception if validation fails
try {
    config->set("brain.custom.value", 2.0f, ConfigSource::Runtime);
} catch (const std::runtime_error& e) {
    std::cerr << "Validation error: " << e.what() << std::endl;
}
```

### Configuration Sections

The configuration system supports hierarchical sections for organization:

```cpp
// Register section-specific parameters
config->registerParameter("neuron.advanced", ParameterMeta(
    "neuron.advanced.adaptation_rate",
    "Advanced neuron adaptation rate parameter",
    0.05f,
    config_validation::createFloatRangeValidator(0.001f, 0.1f),
    false
));

// Set section-specific parameter
config->setParameter("neuron.advanced", "adaptation_rate", 0.05f, ConfigSource::File);
```

## Common Configuration Patterns

### Pattern 1: Reproducible Experiments

```cpp
// Ensure reproducible results
config->set("random_seed", 12345, ConfigSource::Default);
config->set("simulation_timestep", 0.001, ConfigSource::Default);
config->set("neuron_count", 1000, ConfigSource::Default);
config->set("connection_probability", 0.1f, ConfigSource::Default);

// Load additional settings from file
config->loadFromFile("configs/experiment_base.cfg");

// Override with command line if provided
config->loadFromArgs(argc, argv);
```

### Pattern 2: Development vs Production

```cpp
// Development configuration (smaller, faster)
config->set("neuron_count", 100, ConfigSource::Default);
config->set("region_count", 1, ConfigSource::Default);
config->set("plasticity.stdp.enable", true, ConfigSource::Default);

// Production configuration (larger, more realistic)
config->set("neuron_count", 10000, ConfigSource::Default);
config->set("region_count", 5, ConfigSource::Default);
config->set("plasticity.stdp.enable", true, ConfigSource::Default);
config->set("plasticity.structural.enable", true, ConfigSource::Default);
```

### Pattern 3: Performance Tuning

```cpp
// Performance-optimized configuration
config->set("neuron.time_constant", 50.0f, ConfigSource::Default);  // Slower dynamics
config->set("neuron.refractory_period_default", 10, ConfigSource::Default);  // Longer refractory
config->set("plasticity.hebbian.learning_rate", 0.001f, ConfigSource::Default);  // Slower learning
config->set("plasticity.stdp.learning_rate", 0.0005f, ConfigSource::Default);  // Slower STDP
```

## Configuration File Examples

### Minimal Configuration

```
# Minimal NLM Configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
```

### Detailed Configuration

```
# NLM Configuration - Detailed Settings

# Basic settings
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 5000
region_count = 3
connection_probability = 0.05

# Neuron dynamics
neuron.time_constant = 20.0f
neuron.resting_potential_default = -70.0f
neuron.threshold_default = -55.0f
neuron.refractory_period_default = 5
neuron.adaptation_rate = 0.01f

# Plasticity rules
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.stdp.tau = 20.0f
plasticity.hebbian.enable = true
plasticity.hebbian.learning_rate = 0.01f

# Neuromodulation
neuromod.dopamine.scale = 1.0f
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

# Memory systems
memory.working.enabled = true
memory.episodic.enabled = true
memory.associative.enabled = true

# World settings
world.simple.width = 30
world.simple.height = 30
world.simple.vision_width = 16
world.simple.vision_height = 16
world.simple.max_energy = 200.0f
```

### Experiment-Specific Configuration

```
# Phase 3 Integration Experiment
experiment.phase = 3
experiment.steps = 5000
experiment.record_metrics = true

# Optimized for integration testing
neuron_count = 2000
region_count = 2
connection_probability = 0.08

# Memory focused
memory.working.capacity = 500
memory.episodic.enabled = true
memory.associative.enabled = true
memory.concept.enabled = true

# Learning emphasis
plasticity.stdp.enable = true
plasticity.hebbian.enable = true
plasticity.structural.enable = true
neuromod.curiosity.enable = true
```

## Configuration Debugging

### Configuration Summary

```cpp
auto config = std::make_shared<Config>();
// ... configure config ...

// Get configuration summary
std::string summary = config->summary();
std::cout << summary << std::endl;
```

Sample output:
```
Configuration (5 entries):
  random_seed = [42] (0)
  simulation_timestep = [0.001] (0)
  neuron_count = [1000] (0)
  plasticity.stdp.enable = [1] (0)
  neuron.time_constant = [20] (0)
```

### Validating Configuration

```cpp
bool validateAll(const std::shared_shared<Config>& config) {
    // Check required parameters
    try {
        config->ensureRequiredParameters("neuron");
        config->ensureRequiredParameters("synapse");
    } catch (const std::runtime_error& e) {
        std::cerr << "Missing required parameter: " << e.what() << std::endl;
        return false;
    }
    
    // Check parameter ranges
    float tau = config->getOr<float>("neuron.time_constant", 0.0f);
    if (tau < 1.0f || tau > 100.0f) {
        std::cerr << "Invalid time constant: " << tau << std::endl;
        return false;
    }
    
    int neuronCount = config->getOr<int>("neuron_count", 0);
    if (neuronCount < 10 || neuronCount > 100000) {
        std::cerr << "Invalid neuron count: " << neuronCount << std::endl;
        return false;
    }
    
    return true;
}
```

## Configuration Best Practices

### 1. Use Sections for Organization

- Group related parameters under logical sections
- Keep parameters within their domains
- Use consistent naming conventions

### 2. Document All Parameters

- Add comments for complex parameters
- Use meaningful parameter names
- Document units and expected ranges

### 3. Validate Inputs

- Always validate user-provided configuration
- Use sensible defaults for missing parameters
- Provide clear error messages

### 4. Support Multiple Sources

- Use file-based configuration for experiments
- Allow command-line overrides for debugging
- Keep runtime configuration simple and lightweight

### 5. Version Configuration

- Track configuration changes in version control
- Document significant configuration changes
- Use different config files for different experiment types

## Advanced Configuration

### Dynamic Configuration

```cpp
// Modify configuration during runtime
config->set("neuron_count", 2000, ConfigSource::Runtime);
config->set("plasticity.stdp.learning_rate", 0.002f, ConfigSource::Runtime);
```

### Configuration Templates

```cpp
// Create configuration from template
std::map<std::string, ConfigValue> templateConfig = {
    {"neuron_count", 1000},
    {"plasticity.stdp.enable", true},
    {"neuromod.curiosity.enable", true},
    {"memory.working.enabled", true}
};

for (const auto& pair : templateConfig) {
    config->set(pair.first, pair.second, ConfigSource::Default);
}
```

### Configuration Migration

```cpp
// Migrate old configuration format
void migrateConfig(const std::string& oldFile, const std::string& newFile) {
    auto oldConfig = std::make_shared<Config>();
    auto newConfig = std::make_shared<Config>();
    
    if (oldConfig->loadFromFile(oldFile)) {
        // Migrate specific parameters
        if (oldConfig->has("neuron_count")) {
            int count = oldConfig->getOr<int>("neuron_count", 0);
            // Map old to new parameter names
            newConfig->set("brain.neuron_count", count, ConfigSource::Default);
        }
        
        // Save migrated configuration
        newConfig->saveToFile(newFile);
    }
}
```

## Configuration Command Reference

### Useful Command Line Options

```bash
# Run with custom configuration
./nlm --config=custom_config.cfg

# Override specific parameters
./nlm --neuron_count=5000 --plasticity.stdp.learning_rate=0.002

# Enable debug logging
./nlm --log_level=Debug

# Performance mode
./nlm --neuron_count=100 --region_count=1
```

## Further Reading

- [API_REFERENCE.md](docs/API_REFERENCE.md) - Complete API documentation
- [TUTORIAL.md](docs/TUTORIAL.md) - Step-by-step tutorial
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PERFORMANCE.md](docs/PERFORMANCE.md) - Performance optimization

## References

1. NLM Phase 2 Main Implementation - src/main.cpp
2. Configuration System Design - src/core/Config/Config.hpp
3. Parameter Validation Implementation - src/core/Config/Config.cpp
4. Configuration File Format Examples - configs/

## License

See LICENSE file for copyright and license information.
