# NLM Configuration Reference
# Neural Learning Machine - Phase 6: Integrated Artificial Brain

This document provides comprehensive reference for NLM configuration parameters, validation rules, and usage guidelines.

## Configuration Overview

NLM uses a flexible configuration system supporting multiple sources:
- **Command-line arguments** (highest priority)
- **Configuration files** (`.cfg` format)
- **Runtime overrides**
- **Built-in defaults** (lowest priority)

Configuration parameters are organized into logical categories for easy management and documentation.

## Parameter Categories

### 1. Core Simulation Parameters

#### Simulation Control
| Parameter | Type | Default | Description | Range | Example |
|-----------|------|---------|-------------|-------|---------|
| `random_seed` | uint64_t | 42 | Seed for reproducible experiments | Any non-negative | `random_seed = 12345` |
| `simulation_timestep` | double | 0.001 | Neural simulation time step (seconds) | (0, ∞) | `simulation_timestep = 0.002` |
| `neuron_count` | size_t | 1000 | Total number of neurons | [1, ∞) | `neuron_count = 2000` |
| `region_count` | size_t | 1 | Number of brain regions | [1, ∞) | `region_count = 4` |
| `connection_probability` | float | 0.1 | Probability of synaptic connection | [0.0, 1.0] | `connection_probability = 0.15` |

#### Simulation Limits
| Parameter | Type | Default | Description | Range | Example |
|-----------|------|---------|-------------|-------|---------|
| `max_simulation_steps` | size_t | 10000 | Maximum simulation steps | [1, ∞) | `max_simulation_steps = 50000` |
| `simulation_time_limit` | double | 0.0 | Absolute time limit (0 = unlimited) | [0, ∞) | `simulation_time_limit = 300.0` |
| `replay_interval` | size_t | 100 | Memory replay interval (steps) | [1, ∞) | `replay_interval = 500` |
| `consolidation_interval` | size_t | 1000 | Memory consolidation interval | [1, ∞) | `consolidation_interval = 2000` |

### 2. Plasticity and Learning Parameters

#### STDP Parameters
| Parameter | Type | Default | Description | Range | Example |
|-----------|------|---------|-------------|-------|---------|
| `stdp_ltp_weight` | float | 0.01 | Long-term potentiation weight | (-1.0, 1.0) | `stdp_ltp_weight = 0.02` |
| `stdp_ltd_weight` | float | 0.012 | Long-term depression weight | (-1.0, 1.0) | `stdp_ltd_weight = 0.015` |
| `stdp_tau` | float | 20.0 | STDP eligibility trace time | (0, ∞) | `stdp_tau = 25.0` |
| `plasticity_learning_rate` | float | 0.01 | Overall plasticity rate | [0.0, 1.0] | `plasticity_learning_rate = 0.02` |

#### Structural Plasticity
| Parameter | Type | Default | Description | Range | Example |
|-----------|------|---------|-------------|-------|---------|
| `synaptogenesis_rate` | float | 0.0001 | Synapse formation rate | [0.0, ∞) | `synaptogenesis_rate = 0.0002` |
| `pruning_rate` | float | 0.00001 | Synapse elimination rate | [0.0, ∞) | `pruning_rate = 0.00003` |

### 3. Memory System Parameters

#### Working Memory
| Parameter | Type | Default | Description | Range | Example |
|-----------|------|---------|-------------|-------|---------|
| `working_memory_capacity` | size_t | neuron_count/10 | Working memory capacity | [1, ∞) | `working_memory_capacity = 100` |

#### Episodic Memory
| Parameter | Type | Default | Description | Range | Example |
|-----------|------|---------|-------------|-------|---------|
| `episodic_memory_max_episodes` | size_t | 1000 | Maximum stored episodes | [1, ∞) | `episodic_memory_max_episodes = 5000` |
| `episodic_memory_recall_window` | size_t | 100 | Recall search window | [1, ∞) | `episodic_memory_recall_window = 200` |

### 4. Neuromodulation Parameters

#### Dopamine System
| Parameter | Type | Default | Description | Range | Example |
|-----------|------|---------|-------------|-------|---------|
| `dopamine_baseline` | float | 0.1 | Baseline dopamine level | (-1.0, 1.0) | `dopamine_baseline = 0.15` |
| `dopamine_curiosity_scale` | float | 2.0 | Curiosity modulation strength | [0.0, ∞) | `dopamine_curiosity_scale = 3.0` |

#### Reward Processing
| Parameter | Type | Default | Description | Range | Example |
|-----------|------|---------|-------------|-------|---------|
| `reward_discount_factor` | float | 0.99 | Reward discounting factor | [0.0, 1.0] | `reward_discount_factor = 0.999` |
| `reward_prediction_error` | float | 0.0 | Initial prediction error | (-∞, ∞) | `reward_prediction_error = 0.0` |

### 5. Environment Parameters

#### World Configuration
| Parameter | Type | Default | Description | Range | Example |
|-----------|------|---------|-------------|-------|---------|
| `environment_name` | string | GridWorld | Type of environment | String | `environment_name = "ComplexWorld"` |
| `environment_width` | size_t | 10 | Environment width | [1, ∞) | `environment_width = 20` |
| `environment_height` | size_t | 10 | Environment height | [1, ∞) | `environment_height = 20` |

### 6. Logging and Output Parameters

#### Logging Configuration
| Parameter | Type | Default | Description | Range | Example |
|-----------|------|---------|-------------|-------|---------|
| `log_level` | string | INFO | Logging severity level | [Debug, Info, Warning, Error, Critical] | `log_level = "Debug"` |
| `log_to_file` | bool | false | Log to file in addition to console | [true, false] | `log_to_file = true` |
| `log_filename` | string | "nlm.log" | Log file name | String | `log_filename = "nlm_production.log"` |
| `log_colors` | bool | true | Enable colored console output | [true, false] | `log_colors = false` |

### 7. Performance and Optimization Parameters

#### Performance Tuning
| Parameter | Type | Default | Description | Range | Example |
|-----------|------|---------|-------------|-------|---------|
| `simulation_speed` | double | 1.0 | Simulation speed multiplier | (0, ∞) | `simulation_speed = 2.0` |
| `enable_parallel_processing` | bool | true | Enable multi-core processing | [true, false] | `enable_parallel_processing = false` |
| `enable_simd_acceleration` | bool | true | Enable SIMD vectorization | [true, false] | `enable_simd_acceleration = true` |
| `checkpoint_interval` | size_t | 1000 | Checkpoint save interval | [1, ∞) | `checkpoint_interval = 500` |
| `checkpoint_dir` | string | "./checkpoints" | Checkpoint directory | String | `checkpoint_dir = "./backups"` |

### 8. Experimental Parameters

#### Experiment Control
| Parameter | Type | Default | Description | Range | Example |
|-----------|------|---------|-------------|-------|---------|
| `experimental_phase` | string | "all" | Which experimental phase to run | [phase3, phase4, phase5, phase6, all] | `experimental_phase = "phase4"` |
| `target_accuracy` | float | 0.85 | Target accuracy for experiments | [0.0, 1.0] | `target_accuracy = 0.90` |
| `memory_capacity` | size_t | neuron_count/10 | Memory capacity for experiments | [1, ∞) | `memory_capacity = 200` |
| `planning_depth` | size_t | 5 | Planning depth for cognition | [1, ∞) | `planning_depth = 3` |
| `attention_selectivity` | float | 0.5 | Attention system selectivity | [0.0, 1.0] | `attention_selectivity = 0.8` |

## Configuration File Format

### Basic Syntax
```
# Comment or configuration description
key = value

# Multi-line comments
# This is a comment
# Another comment
# key = value

# Quotes for string values
message = "Hello, World!"
path = '/usr/local/bin'

# Boolean values
enabled = true
disabled = false
```

### Data Types

#### Numeric Types
```
# Integers
integer_value = 42
large_integer = 1000000
negative = -123

# Floating point numbers
float_value = 3.14
double_value = 2.718281828
scientific = 1.23e-4

# Special numeric values
inf = 1.0 / 0.0  # Positive infinity
nan = 0.0 / 0.0  # Not a number
```

#### String Types
```
# Simple strings
simple_string = "hello"

# Strings with quotes
quoted = "He said: \"Hello!\""

# File paths
path = "/home/user/documents"
relative_path = "./config/file.cfg"
```

#### Boolean Types
```
# Boolean values
enabled = true
disabled = false

# Case insensitive
ENABLED = TRUE
enabled = FALSE
```

## Configuration Validation

### Validation Rules

#### Required Parameters
**Always present and validated:**
- `random_seed`: Must be non-negative integer
- `simulation_timestep`: Must be positive float
- `neuron_count`: Must be positive integer
- `region_count`: Must be positive integer

#### Conditional Validation
**Validation depends on other parameters:**

```cpp
// If connection_probability is set, it must be in [0,1]
if (config.hasKey("connection_probability")) {
    float prob = config.getOr<float>("connection_probability", 0.0f);
    if (prob < 0.0f || prob > 1.0f) {
        // Validation error
    }
}

// If neuron_count is set, it must be reasonable
if (config.hasKey("neuron_count")) {
    size_t neurons = config.getOr<size_t>("neuron_count", 0);
    if (neurons == 0 || neurons > 1000000) {
        // Validation error
    }
}
```

#### Cross-Parameter Validation
**Validation that checks relationships between parameters:**

```cpp
// Ensure neuron_count >= region_count
if (config.hasKey("neuron_count") && config.hasKey("region_count")) {
    size_t neurons = config.getOr<size_t>("neuron_count", 0);
    size_t regions = config.getOr<size_t>("region_count", 0);
    
    if (neurons == 0) {
        errors.push_back("neuron_count must be positive");
    } else if (regions == 0) {
        errors.push_back("region_count must be positive");
    } else if (neurons < regions) {
        errors.push_back("neuron_count must be >= region_count");
    }
}
```

## Configuration Source Priority

### Source Hierarchy

1. **Command-line arguments** (highest priority)
   - `--key=value` format
   - `-key value` format

2. **Configuration file**
   - Loaded via `--config` or default `configs/default.cfg`
   - Parsed as `key = value` pairs

3. **Runtime overrides**
   - Programmatically set via `Config::set()`
   - Applied after file loading

4. **Built-in defaults** (lowest priority)
   - Programmatically set by NLM
   - Applied last if parameters not set

### Merging Rules

```cpp
// Loading sequence:
// 1. Apply built-in defaults
// 2. Load configuration file (overrides defaults)
// 3. Apply command-line arguments (overrides all)
// 4. Apply runtime overrides (overrides all)

// Example:
// Built-in defaults: { "random_seed": 42, "neuron_count": 1000 }
// Config file: { "neuron_count": 500 }
// Command-line: { "region_count": 4 }
// Runtime: { "simulation_timestep": 0.002 }

// Final result:
// {
//   "random_seed": 42,           // from defaults
//   "neuron_count": 500,         // from file (overrides defaults)
//   "region_count": 4,           // from command-line (overrides all)
//   "simulation_timestep": 0.002, // from runtime (overrides all)
//   ... other defaults ...
// }
```

## Command-Line Interface

### Basic Usage

#### Help and Version
```bash
# Show help information
./nlm --help

# Show version information
./nlm --version
```

#### Configuration Selection
```bash
# Use default configuration
./nlm

# Specify custom configuration file
./nlm --config=my_config.cfg

# Use environment-specific configuration
./nlm --config=configs/production.cfg

# List available configurations
./nlm --config=help
```

#### Parameter Override
```bash
# Override single parameter
./nlm --neuron_count=2000

# Override multiple parameters
./nlm \
  --neuron_count=2000 \
  --region_count=4 \
  --timestep=0.002

# Use key=value format (preferred)
./nlm \
  --config=configs/default.cfg \
  --neuron_count=2000 \
  --region_count=4 \
  --simulation_timestep=0.002
```

#### Special Options
```bash
# Enable debugging
./nlm --log_level=Debug --log_colors=false

# Set checkpoint directory
./nlm --checkpoint_dir=./my_backups

# Enable experimental mode
./nlm --experimental_mode=phase4

# Run benchmark
./benchmark --neurons="100,500,1000,2000"
```

## Configuration Examples

### Example 1: Basic Configuration

```
// File: configs/basic.cfg
# NLM Configuration for Basic Demonstration

# Core simulation parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 2
connection_probability = 0.1

# Logging
log_level = INFO
log_to_file = false
```

### Example 2: High-Performance Configuration

```
// File: configs/performance.cfg
# NLM Configuration for High-Performance Systems

# Core simulation parameters
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 5000
region_count = 8
connection_probability = 0.05

# Performance optimizations
simulation_speed = 1.5
enable_parallel_processing = true
enable_simd_acceleration = true

# Memory management
max_simulation_steps = 20000
replay_interval = 500
consolidation_interval = 2000

# Logging for performance monitoring
log_level = Warning
log_to_file = true
log_filename = "nlm_performance.log"
log_colors = false
```

### Example 3: Experimental Configuration

```
// File: configs/experimental.cfg
# NLM Configuration for Cognitive Experiments

# Core simulation parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 3
connection_probability = 0.1

# Experimental parameters
experimental_phase = phase4
target_accuracy = 0.85
memory_capacity = 100
planning_depth = 3
attention_selectivity = 0.8

# Neuromodulation for learning
enable_plasticity = true
plasticity_learning_rate = 0.02
enable_curiosity = true
dopamine_curiosity_scale = 2.5

# Environment
environment_name = ExperimentalWorld
environment_width = 15
environment_height = 15

# Results collection
output_dir = "./experimental_results"
output_format = "json"
record_video = false
```

## Configuration Validation Examples

### 1. Validation Function Example

```cpp
// Config validation example
bool validateConfig(const Config& config, std::vector<std::string>& errors) {
    bool isValid = true;
    
    // Check required parameters
    if (!config.has("random_seed")) {
        errors.push_back("Missing required parameter: random_seed");
        isValid = false;
    }
    
    if (!config.has("simulation_timestep")) {
        errors.push_back("Missing required parameter: simulation_timestep");
        isValid = false;
    }
    
    if (!config.has("neuron_count")) {
        errors.push_back("Missing required parameter: neuron_count");
        isValid = false;
    }
    
    // Validate parameter ranges
    if (config.has("simulation_timestep")) {
        double timestep = config.getOr<double>("simulation_timestep", 0.001);
        if (timestep <= 0.0) {
            errors.push_back("simulation_timestep must be positive");
            isValid = false;
        }
    }
    
    if (config.has("neuron_count")) {
        size_t neurons = config.getOr<size_t>("neuron_count", 0);
        if (neurons == 0) {
            errors.push_back("neuron_count must be positive");
            isValid = false;
        } else if (neurons > 1000000) {
            errors.push_back("neuron_count too large (max: 1,000,000)");
            isValid = false;
        }
    }
    
    if (config.has("region_count")) {
        size_t regions = config.getOr<size_t>("region_count", 0);
        if (regions == 0) {
            errors.push_back("region_count must be positive");
            isValid = false;
        }
    }
    
    if (config.has("connection_probability")) {
        float prob = config.getOr<float>("connection_probability", 0.0f);
        if (prob < 0.0f || prob > 1.0f) {
            errors.push_back("connection_probability must be in [0, 1]");
            isValid = false;
        }
    }
    
    // Cross-parameter validation
    if (config.has("neuron_count") && config.has("region_count")) {
        size_t neurons = config.getOr<size_t>("neuron_count", 0);
        size_t regions = config.getOr<size_t>("region_count", 0);
        
        if (neurons < regions) {
            errors.push_back("neuron_count must be >= region_count");
            isValid = false;
        }
    }
    
    return isValid;
}
```

### 2. Safe Configuration Application

```cpp
// Safe configuration application with error handling
bool initializeBrainWithValidation(const std::shared_ptr<Config>& config) {
    // Validate configuration
    std::vector<std::string> errors;
    if (!validateConfig(*config, errors)) {
        std::cerr << "Configuration validation failed:" << std::endl;
        for (const auto& error : errors) {
            std::cerr << "  ERROR: " << error << std::endl;
        }
        return false;
    }
    
    // Apply validated configuration
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return false;
    }
    
    // Configuration successful
    std::cout << "Brain initialized successfully with validated configuration." << std::endl;
    brain->logStatus();
    
    return true;
}
```

## Configuration File Best Practices

### 1. File Organization

```
configs/
├── default.cfg              # Default configuration
├── development.cfg          # Development settings
├── production.cfg           # Production settings
├── experimental/            # Experiment-specific configs
│   ├── phase4_cognitive.cfg
│   ├── phase5_developmental.cfg
│   └── phase6_integration.cfg
└── examples/                # Usage examples
    ├── basic.cfg
    ├── performance.cfg
    └── research.cfg
```

### 2. Configuration Comments

```
# configs/basic.cfg

# ============================================================================
# NLM Configuration for Basic Neural Network Demonstration
# ============================================================================

# Random seed for reproducible experiments
# Set to a fixed value for debugging, or use system time for variability
random_seed = 42

# Neural simulation timestep in seconds
# Smaller values give more accurate dynamics but require more computation
simulation_timestep = 0.001

# Network architecture parameters
neuron_count = 500           # Total number of neurons
region_count = 2             # Number of modular brain regions
connection_probability = 0.1 # Probability of synaptic connections

# Plasticity parameters for learning
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0

# Logging configuration
log_level = INFO
log_to_file = false
log_filename = "nlm.log"

# Simulation limits
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10
```

### 3. Configuration Validation Script

```bash
#!/bin/bash
# scripts/validate_config.sh

# Validate configuration file
validate_config() {
    local config_file="$1"
    
    if [[ ! -f "$config_file" ]]; then
        echo "Error: Configuration file '$config_file' not found!"
        return 1
    fi
    
    echo "Validating configuration file: $config_file"
    
    # Check for required parameters
    local required_params=("random_seed" "simulation_timestep" "neuron_count" "region_count")
    
    for param in "${required_params[@]}"; do
        if ! grep -q "^$param\s*=" "$config_file"; then
            echo "Warning: Required parameter '$param' not found in configuration"
        fi
    done
    
    # Check parameter syntax
    if ! grep -q "^random_seed\s*=[[:space:]]*[0-9]\+$" "$config_file"; then
        echo "Error: Invalid random_seed format in configuration"
        return 1
    fi
    
    if ! grep -q "^simulation_timestep\s*=[[:space:]]*[0-9]\+\.?[0-9]*\s*$" "$config_file"; then
        echo "Error: Invalid simulation_timestep format in configuration"
        return 1
    fi
    
    echo "Configuration validation passed!"
    return 0
}

# Main script
if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <config_file>"
    echo "Validates NLM configuration file syntax"
    exit 1
fi

validate_config "$1"
```

## Configuration Migration Guide

### 1. From Phase 1 to Phase 6

**Phase 1 → Phase 6 Configuration Migration:**

#### Phase 1 Configuration
```
# Phase 1: Basic neural network
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 1
connection_probability = 0.1
```

#### Phase 6 Configuration
```
# Phase 6: Integrated artificial brain
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2  # Added modular regions
connection_probability = 0.1
replay_interval = 100                    # New: memory replay
simulation_time_limit = 0.0              # New: time limit
enable_plasticity = true                 # New: plasticity control
synaptogenesis_rate = 0.0001f           # New: structural plasticity
```

#### Migration Notes
- Add new parameters that become essential in Phase 6
- Update parameter descriptions to reflect new functionality
- Add validation for new parameter constraints
- Update configuration files to include experimental features

### 2. Configuration Upgrade Guide

**Upgrade from Version X to Y:**

```bash
# Upgrade script example
#!/bin/bash
# scripts/upgrade_config.sh

OLD_CONFIG="$1"
NEW_CONFIG="$2"

# Backup old configuration
cp "$OLD_CONFIG" "${OLD_CONFIG}.backup.$(date +%Y%m%d_%H%M%S)"

# Create new configuration based on old
cat > "$NEW_CONFIG" << EOF
# Migrated configuration from Phase 1 to Phase 6
random_seed = $(grep "^random_seed" "$OLD_CONFIG" | cut -d'=' -f2 | tr -d ' ')

# Copy existing parameters with updates
simulation_timestep = $(grep "^simulation_timestep" "$OLD_CONFIG" | cut -d'=' -f2 | tr -d ' ')
neuron_count = $(grep "^neuron_count" "$OLD_CONFIG" | cut -d'=' -f2 | tr -d ' ')
region_count = $(grep "^region_count" "$OLD_CONFIG" | cut -d'=' -f2 | tr -d ' ' 2>/dev/null || echo "2")

# Add Phase 6 specific parameters
connection_probability = $(grep "^connection_probability" "$OLD_CONFIG" | cut -d'=' -f2 | tr -d ' ' 2>/dev/null || echo "0.1")
replay_interval = 100
simulation_time_limit = 0.0
enable_plasticity = true
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
EOF

echo "Configuration upgraded from $OLD_CONFIG to $NEW_CONFIG"
```

## Configuration Testing

### 1. Configuration Test Suite

```cpp
// configs/test_config.cpp - Configuration validation tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

void testBasicConfig() {
    auto config = std::make_shared<Config>();
    
    // Set basic parameters
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    config->set("neuron_count", static_cast<int64_t>(1000));
    config->set("region_count", static_cast<int64_t>(2));
    
    // Test getOr functionality
    assert(config->getOr<int64_t>("random_seed", 0) == 42);
    assert(config->getOr<double>("simulation_timestep", 0.0) == 0.001);
    assert(config->getOr<int64_t>("neuron_count", 0) == 1000);
    assert(config->getOr<int64_t>("region_count", 0) == 2);
    
    // Test default values
    assert(config->getOr<int64_t>("missing_key", 999) == 999);
    
    // Test has functionality
    assert(config->has("random_seed"));
    assert(config->has("simulation_timestep"));
    assert(!config->has("missing_key"));
    
    std::cout << "✓ Basic configuration test passed" << std::endl;
}

void testConfigValidation() {
    auto config = std::make_shared<Config>();
    
    // Test validation with required keys
    std::vector<std::string> requiredKeys = {"random_seed", "simulation_timestep", "neuron_count"};
    
    assert(config->validate(requiredKeys));  // Should fail - missing keys
    
    // Add missing keys
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    config->set("neuron_count", static_cast<int64_t>(1000));
    
    assert(config->validate(requiredKeys));  // Should pass
    
    std::cout << "✓ Configuration validation test passed" << std::endl;
}

void testConfigFile() {
    auto config = std::make_shared<Config>();
    
    // Test loading from file
    bool loaded = config->loadFromFile("configs/default.cfg");
    assert(loaded);
    
    // Test specific parameters
    assert(config->has("random_seed"));
    assert(config->has("simulation_timestep"));
    assert(config->has("neuron_count"));
    assert(config->has("region_count"));
    
    std::cout << "✓ Configuration file loading test passed" << std::endl;
}

int main() {
    testBasicConfig();
    testConfigValidation();
    testConfigFile();
    
    std::cout << std::endl << "All configuration tests passed!" << std::endl;
    return 0;
}
```

### 2. Command-Line Configuration Testing

```bash
#!/bin/bash
# scripts/test_config_cli.sh

# Test command-line configuration
./nlm \
  --config=configs/default.cfg \
  --neuron_count=500 \
  --region_count=2 \
  --timestep=0.002

# Verify configuration
./nlm \
  --config=configs/default.cfg \
  --neuron_count=500 \
  --region_count=2 \
  --timestep=0.002 \
  --validate

# Test configuration validation script
./scripts/validate_config.sh configs/default.cfg

# Test configuration upgrade
./scripts/upgrade_config.sh configs/old.cfg configs/new.cfg
```

## Conclusion

The NLM configuration system provides a flexible, powerful framework for managing neural network parameters across different phases and use cases. This reference document covers:

### Key Sections:
1. **Parameter Categories**: All configuration parameters organized by function
2. **File Format**: Syntax and structure for configuration files
3. **Validation Rules**: Parameter validation and cross-parameter checking
4. **Source Priority**: How different configuration sources are merged
5. **Testing**: Validation and testing procedures
6. **Migration**: Upgrade paths and compatibility considerations

### Configuration Best Practices:
- Use descriptive comments for each parameter
- Organize parameters by logical groups
- Validate configuration before use
- Test configuration in target environment
- Document parameter ranges and constraints
- Provide migration paths for configuration upgrades

### Configuration Usage:
- **Simple usage**: `./nlm` for default configuration
- **Custom configuration**: `./nlm --config=my_config.cfg`
- **Parameter overrides**: `./nlm --neuron_count=1000 --timestep=0.001`
- **Advanced configuration**: `./nlm --experimental_mode=phase4 --target_accuracy=0.9`

The NLM configuration system is designed to support everything from simple demonstrations to large-scale research experiments, with comprehensive validation and testing capabilities to ensure reliable operation across all use cases.
