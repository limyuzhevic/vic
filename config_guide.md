# NLM Configuration Guide

This document provides comprehensive guidance for working with NLM configuration files, including:

## Configuration File Format

### Current Format (Phase 1)
NLM uses a simple key=value format for configuration:

```ini
# NLM Configuration File
# This is the default configuration for Phase 1 skeleton

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
dopamine_baseline = 0.1
reward_discount_factor = 0.99
environment_name = GridWorld
environment_width = 10
environment_height = 10
log_level = INFO
log_to_file = false
log_filename = nlm.log
max_simulation_steps = 10000
simulation_time_limit = 0.0
visualization_enabled = false
visualization_update_rate = 30.0
```

### Proposed Format (Phase 2+)
Future versions will support JSON/YAML format:

```json
{
  "nlm": {
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
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
      "dopamine_baseline": 0.1
    },
    "reward": {
      "discount_factor": 0.99
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
}
```

## Configuration Categories

### Core Settings
| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `random_seed` | int | 42 | Random seed for reproducible experiments |
| `simulation_timestep` | double | 0.001 | Simulation timestep in seconds |
| `neuron_count` | int | 1000 | Total number of neurons in the brain |
| `region_count` | int | 2 | Number of brain regions |
| `connection_probability` | double | 0.1 | Probability of connection between neurons |

### Plasticity Settings
| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `plasticity_learning_rate` | double | 0.01 | General plasticity learning rate |
| `stdp_ltp_weight` | double | 0.01 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | double | 0.012 | STDP long-term depression weight |
| `stdp_time_constant` | double | 20.0 | STDP time constant (ms) |

### Development Settings
| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `development_synaptogenesis_rate` | double | 0.001 | Rate of new synapse formation |
| `development_pruning_rate` | double | 0.0001 | Rate of synapse removal |

### Neuromodulation Settings
| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `dopamine_baseline` | double | 0.0 | Baseline dopamine level |

### Reward Settings
| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `reward_discount_factor` | double | 0.99 | Reward discount factor (gamma) |

### Environment Settings
| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `environment_name` | string | GridWorld | Name of environment |
| `environment_width` | int | 10 | Environment width |
| `environment_height` | int | 10 | Environment height |

### Logging Settings
| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `log_level` | string | INFO | Log level (Debug, Info, Warning, Error, Critical) |
| `log_to_file` | bool | false | Whether to log to file |
| `log_filename` | string | nlm.log | Log file name |

### Simulation Settings
| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `max_simulation_steps` | int | 10000 | Maximum simulation steps |
| `simulation_time_limit` | double | 0.0 | Simulation time limit (seconds) |

### Visualization Settings
| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `visualization_enabled` | bool | false | Whether visualization is enabled |
| `visualization_update_rate` | double | 30.0 | Visualization update rate (Hz) |

## Configuration Loading

### From File
```cpp
bool loadFromFile(const std::string& filepath);
```

**Current implementation**: Simple key=value parser with basic error handling
**Future implementation**: JSON/YAML parser with schema validation

### From Command Line
```bash
./nlm --neuron_count=500 --random_seed=123 --plasticity_learning_rate=0.001
```

Or with space-separated values:
```bash
./nlm -neuron_count 500 -random_seed 123
```

### Programmatically
```cpp
config->set("neuron_count", 500);
config->set("plasticity_learning_rate", 0.001);
```

## Configuration Validation

**Current State**: No validation
**Proposed**: Comprehensive validation with helpful error messages

### Validation Rules
1. **Neuron Count**: Must be positive (e.g., 1-10000)
2. **Time Steps**: Must be positive (e.g., > 0)
3. **Connections**: Must be between 0 and 1
4. **Learning Rates**: Must be non-negative
5. **File Paths**: Must be valid
6. **Log Levels**: Must be one of: Debug, Info, Warning, Error, Critical

### Example Validation
```cpp
class ConfigValidator {
public:
    static std::vector<std::string> validate(const Config& config);
    static bool isValid(const Config& config);
    static void setDefaults(Config& config);
};
```

## Configuration Management

### Priority System
Configuration values can be set with different priority levels:

1. **Default**: Initial configuration values
2. **File**: Loaded from configuration files
3. **CommandLine**: Set via command line arguments
4. **Runtime**: Set programmatically during execution

### Priority Resolution
Later priorities override earlier ones:
- Runtime values override command line
- Command line overrides file values
- File values override defaults

### Example Priority Override
```cpp
// Default: neuron_count = 1000
// Command line: --neuron_count=500  
// Result: neuron_count = 500 (command line wins)

// Default: neuron_count = 1000
// File: neuron_count = 2000
// Runtime: set("neuron_count", 1500)
// Result: neuron_count = 1500 (runtime wins)
```

## Configuration Access

### Getting Values
```cpp
auto neuronCount = config->getOr<int>("neuron_count", 1000);
auto timestep = config->get<double>("simulation_timestep");
```

### Type Safety
```cpp
// Safe access - returns nullopt if key doesn't exist
auto optValue = config->get<int>("neuron_count");
if (optValue.has_value()) {
    int neurons = optValue.value(); // Use the value
}

// Convenience method with default
int neurons = config->getOr<int>("neuron_count", 1000);
```

### Key Information
```cpp
// Get all keys
auto keys = config->getKeys();

// Check if key exists
if (config->has("neuron_count")) {
    // Value exists
}

// Remove key
config->remove("unused_key");
```

## Configuration Examples

### Minimal Configuration
```cpp
config->set("random_seed", 42);
config->set("neuron_count", 500);
brain->initialize();
```

### Complete Configuration
```cpp
config->set("random_seed", 123);
config->set("simulation_timestep", 0.001);
config->set("neuron_count", 1000);
config->set("region_count", 2);
config->set("connection_probability", 0.15);
config->set("plasticity_learning_rate", 0.01);
config->set("stdp_ltp_weight", 0.02);
config->set("stdp_ltd_weight", 0.015);
config->set("stdp_time_constant", 20.0);
config->set("development_synaptogenesis_rate", 0.0005);
config->set("development_pruning_rate", 0.00005);
config->set("dopamine_baseline", 0.1);
config->set("reward_discount_factor", 0.95);
config->set("environment_name", "GridWorld");
config->set("environment_width", 20);
config->set("environment_height", 15);
config->set("log_level", "INFO");
config->set("log_to_file", false);
config->set("max_simulation_steps", 50000);
```

## Configuration Best Practices

### 1. Use Defaults When Possible
```cpp
// Good - use defaults
brain->initialize();

// Bad - override everything unnecessarily
config->set("neuron_count", 1000); // Default is already 1000
```

### 2. Set Random Seeds for Reproducibility
```cpp
// Good - reproducible experiments
config->set("random_seed", 42);

// Bad - non-reproducible results
config->set("random_seed", time(nullptr));
```

### 3. Validate Configuration Values
```cpp
// Always validate critical parameters
int neuronCount = config->getOr<int>("neuron_count", 1000);
if (neuronCount <= 0 || neuronCount > 100000) {
    throw std::invalid_argument("Invalid neuron_count: must be 1-100000");
}
```

### 4. Document Configuration Choices
```cpp
// Document why you're choosing these values
config->set("neuron_count", 500); // Small brain for faster testing
config->set("simulation_timestep", 0.01); // Larger timestep for faster simulation
```

### 5. Use Command Line for Development
```bash
# Development: Small, fast brain
development: --neuron_count=100 --simulation_timestep=0.01

# Production: Large, accurate brain  
production: --neuron_count=5000 --simulation_timestep=0.001
```

## Configuration Migration Guide

### Version 0.1 to 0.2
If upgrading from an older version, this guide explains configuration changes:

#### New Configuration Options
- `plasticity_learning_rate` (renamed from `plasticity.stdp.learning_rate`)
- `stdp_time_constant` (added)
- `dopamine_baseline` (fix from duplicate values)

#### Deprecated Options
- `plasticity.stdp.learning_rate` → `plasticity_learning_rate`
- `plasticity.stdp.tau_plus` → `stdp_ltp_weight`
- `plasticity.stdp.tau_minus` → `stdp_ltd_weight`

### Migration Example
```cpp
// Old config (0.1):
config->set("plasticity.stdp.learning_rate", 0.01);
config->set("plasticity.stdp.tau_plus", 20.0);

// New config (0.2):
config->set("plasticity_learning_rate", 0.01);
config->set("stdp_ltp_weight", 20.0);
config->set("stdp_time_constant", 20.0); // New option
```

## Configuration Troubleshooting

### Common Issues

#### 1. "Key not found"
```cpp
if (!config->has("neuron_count")) {
    config->set("neuron_count", 1000); // Provide default
}
```

#### 2. Invalid values
```cpp
int neurons = config->getOr<int>("neuron_count", 1000);
if (neurons <= 0) {
    std::cerr << "Error: neuron_count must be positive" << std::endl;
    return 1;
}
```

#### 3. File not found
```cpp
bool loaded = config->loadFromFile("configs/production.cfg");
if (!loaded) {
    std::cerr << "Warning: Could not load config file, using defaults" << std::endl;
}
```

#### 4. Command line parsing errors
```bash
# Bad: --neuron_count 500 --  (space after value)
# Good: --neuron_count=500 --random_seed=123
```

### Configuration Validation
```bash
# Check if config is valid
./nlm --validate-config configs/production.cfg

# Or use Python API
import pynlm
config = pynlm.createDefaultConfig()
config.loadFromFile("configs/production.cfg")
# Validation happens automatically
```

## Advanced Configuration

### Environment Variables
Environment variables can override configuration:

```bash
export NLM_NEURON_COUNT=1000
export NLM_SIMULATION_TIMESTEP=0.001
export NLM_RANDOM_SEED=42
```

Programmatic access:
```cpp
std::string envNeuronCount = std::getenv("NLM_NEURON_COUNT");
if (!envNeuronCount.empty()) {
    config->set("neuron_count", std::stoi(envNeuronCount), ConfigSource::Runtime);
}
```

### Configuration Profiles
```cpp
// Profile types
enum class ConfigProfile {
    Development,  // Small, fast, verbose
    Production,    // Large, accurate, quiet
    Research,      // Custom parameters
    Benchmark      // Optimized for performance testing
};

// Profile application
void applyProfile(Config& config, ConfigProfile profile) {
    switch (profile) {
        case ConfigProfile::Development:
            config->set("neuron_count", 100);
            config->set("log_level", "DEBUG");
            break;
        case ConfigProfile::Production:
            config->set("neuron_count", 5000);
            config->set("log_level", "INFO");
            break;
        // etc.
    }
}
```

## Future Improvements

### Planned Configuration Features
1. **JSON/YAML Parser** - Replace simple key=value with proper format
2. **Schema Validation** - Validate configuration against JSON schema
3. **Configuration Templates** - Pre-built configuration profiles
4. **Dynamic Configuration** - Runtime configuration changes
5. **Configuration History** - Track configuration changes over time
6. **Cloud Configuration** - Load configuration from cloud storage

### Research References
- "Configuration Management: A Practical Guide" by Mikko Kontio
- "Design Patterns: Elements of Reusable Object-Oriented Software" by Gamma et al.
- "The Pragmatic Programmer" by Andrew Hunt and David Thomas

## Conclusion

Configuration is fundamental to NLM's functionality. A well-designed configuration system:

1. **Enables flexibility** - Different scenarios need different parameters
2. **Ensures reproducibility** - Fixed random seeds and saved configurations
3. **Improves usability** - Clear documentation and validation
4. **Supports testing** - Easy switching between development and production settings
5. **Facilitates maintenance** - Clear structure and validation

The NLM configuration system is evolving from Phase 1 (simple key=value) towards Phase 2 (JSON/YAML with validation). This guide provides the foundation for that evolution while supporting the current implementation.
