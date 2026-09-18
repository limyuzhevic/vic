# NLM Configuration Documentation

## Overview

The NLM (Neural Learning Machine) project uses a flexible configuration system to control neural simulation parameters, system behavior, and experimental settings. The configuration system allows runtime modification of simulation parameters without code changes.

## Configuration System Architecture

### Core Components

1. **Config Class** (`src/core/Config/Config.hpp`)
   - Provides key-value storage with type support
   - Supports multiple configuration sources: default, file, command-line, runtime
   - Type-safe access with `get<T>()` and `getOr<T>()` methods
   - Automatic type conversion and validation

2. **Brain Integration**
   - Brain uses Config for simulation parameters
   - Configurable neuron count, region count, connection probability
   - Adjustable plasticity rules and learning rates
   - Configurable memory system parameters

3. **Agent Configuration**
   - Sensory dimensions (vision, touch, internal, proprioception)
   - Neuromodulation parameters
   - Development stage timing
   - Curiosity and novelty thresholds

## Available Configuration Parameters

### Neural System Parameters

#### Basic Simulation
- `neuron_count` (int, default: 1000)
  - Total number of neurons in the simulation
  - Affects memory capacity and computational load

- `region_count` (int, default: 1)
  - Number of neural regions
  - Enables multi-region connectivity

- `connection_probability` (float, default: 0.1f)
  - Probability of synapse formation between neurons
  - Controls network connectivity density

- `simulation_timestep` (double, default: 0.001)
  - Time step duration in seconds
  - Affects simulation accuracy and speed

### Plasticity Rules

#### STDP (Spike-Timing-Dependent Plasticity)
- `stdp_ltp_weight` (float, default: 0.01f)
  - Long-Term Potentiation weight
  - Stronger LTP = faster learning for synchronous spikes

- `stdp_ltd_weight` (float, default: 0.012f)
  - Long-Term Depression weight
  - Stronger LTD = faster forgetting for asynchronous spikes

- `stdp_tau` (float, default: 20.0f)
  - STDP time constant (ms)
  - Controls spike timing window

#### Structural Plasticity
- `synaptogenesis_rate` (float, default: 0.0001f)
  - Rate of new synapse formation
  - Higher = more exploratory connectivity

- `pruning_rate` (float, default: 0.00001f)
  - Rate of synapse elimination
  - Higher = more aggressive optimization

### Memory System

#### Working Memory
- `working_memory_capacity` (int, default: neuron_count / 10)
  - Number of active traces maintained
  - Affects attention and active processing

#### Episodic Memory
- `episodic_memory_max_episodes` (int, default: 1000)
  - Maximum number of stored experiences
  - Limits replay capability

- `replay_interval` (size_t, default: 100)
  - Steps between memory replay events
  - Controls consolidation timing

- `consolidation_interval` (size_t, default: 1000)
  - Steps between consolidation events
  - Affects memory strength updates

### Sensory Processing

#### Vision System
- `sensory_vision_width` (size_t, default: 16)
  - Vision sensor grid width
  - Determines visual resolution

- `sensory_vision_height` (size_t, default: 16)
  - Vision sensor grid height
  - Determines visual resolution

#### Other Sensory Modalities
- `sensory_touch_count` (size_t, default: 8)
- `sensory_internal_count` (size_t, default: 4)
- `sensory_proprioception_count` (size_t, default: 6)

### Neuromodulation

#### Reward Processing
- `reward_modulation_enabled` (bool, default: true)
  - Enable/disable reward-based learning

#### Curiosity
- `curiosity_enabled` (bool, default: true)
  - Enable/disable exploratory behavior

- `curiosity_threshold` (float, default: 0.3f)
  - Novelty level required for exploration

#### Novelty Detection
- `novelty_decay_factor` (float, default: 0.99f)
  - Rate of novelty decay over time

### Development

#### Developmental Stages
- `development_enabled` (bool, default: true)
  - Enable/disable developmental progression

- `critical_period_duration` (double, default: 300.0)
  - Duration of critical learning period

- `maturation_period_duration` (double, default: 900.0)
  - Duration of skill maturation period

### Checkpoint System

- `checkpoint_dir` (string, default: "./checkpoints")
  - Directory for brain state saves

- `max_checkpoints` (int, default: 10000)
  - Maximum number of checkpoints to retain

- `checkpoint_compression` (bool, default: true)
  - Enable/disable checkpoint compression

## Configuration File Format

### Simple Key=Value Format

```
# NLM Configuration File
# Format: key = value (value can be quoted if it contains spaces)

# Basic simulation parameters
neuron_count = 5000
region_count = 2
simulation_timestep = 0.002

# Plasticity rules
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.018
stdp_tau = 25.0
synaptogenesis_rate = 0.0002pruning_rate = 0.000015

# Memory system
working_memory_capacity = 500
episodic_memory_max_episodes = 2000
replay_interval = 200
consolidation_interval = 2000

# Sensory dimensions
sensory_vision_width = 32
sensory_vision_height = 32
sensory_touch_count = 12
sensory_internal_count = 6
sensory_proprioception_count = 8

# Neuromodulation
reward_modulation_enabled = true
curiosity_enabled = true
novelty_decay_factor = 0.95

# Development
development_enabled = true
critical_period_duration = 500
```

### JSON Format (Future Enhancement)

The Config system is designed to support JSON/YAML format in future versions, allowing more complex nested structures and data types.

## Command Line Configuration

### Program Arguments

```bash
./nlm_simulation \
  --neuron_count=10000 \
  --region_count=3 \
  --simulation_timestep=0.0005 \
  --sensory_vision_width=64 \
  --sensory_vision_height=64 \
  --stdp_ltp_weight=0.02 \
  --stdp_ltd_weight=0.025 \
  --checkpoint_dir=./brain_checkpoints
```

### Short Form Arguments

```bash
./nlm_simulation -n 10000 -r 3 -t 0.0005 -w 64 -h 64 -l 0.02 -d 0.025 -c ./brain_checkpoints
```

## Runtime Configuration

### Programmatic Access

```cpp
#include "Config.hpp"

auto config = std::make_shared<nlm::Config>();

// Set runtime values
config->set("neuron_count", 8000);
config->set("curiosity_enabled", false);
config->set("sensory_vision_width", 48);

// Get values with defaults
int neurons = config->getOr<int>("neuron_count", 1000);
float visionWidth = config->getOr<float>("sensory_vision_width", 16.0f);
bool curiosity = config->getOr<bool>("curiosity_enabled", true);
```

### Configuration Validation

The configuration system includes basic validation:

- Ensures positive values for counts and rates
- Validates ranges for probability values
- Checks for reasonable timestep values
- Verifies consistency between related parameters

## Implementation Notes

### Configuration Priority (Highest to Lowest)

1. **Runtime settings** (set() calls during execution)
2. **Command-line arguments** (--key=value)
3. **File configuration** (loaded from file)
4. **Default values** (built into Config class)

When a key exists in multiple sources, the highest priority source takes precedence.

### Type Safety

The configuration system uses type-safe access:

```cpp
// Correct: type matches
int value = config->getOr<int>("key", 0);

// Wrong: type mismatch - causes runtime error
float value = config->getOr<float>("key", 0.0f); // ERROR!
```

### Performance Considerations

- Configuration values are cached after first access
- File I/O is performed only when needed
- Runtime modifications are O(1) operation
- Memory usage is minimal (stores only configured values)

## Example: Complete Configuration

### File: `nlm_config.nlmc`

```
# Complete NLM Configuration
# Generated for advanced research simulation

# ========== BASIC SIMULATION ==========
neuron_count = 20000
region_count = 4
simulation_timestep = 0.0005

# ========== PLASTICITY ==========
stdp_ltp_weight = 0.018
stdp_ltd_weight = 0.022
stdp_tau = 30.0
synaptogenesis_rate = 0.00015
pruning_rate = 0.000008

# ========== MEMORY SYSTEMS ==========
working_memory_capacity = 2000
episodic_memory_max_episodes = 5000
replay_interval = 150
consolidation_interval = 1500

# ========== SENSORY PROCESSING ==========
sensory_vision_width = 64
sensory_vision_height = 64
sensory_touch_count = 16
sensory_internal_count = 8
sensory_proprioception_count = 12

# ========== NEUROMODULATION ==========
reward_modulation_enabled = true
curiosity_enabled = true
novelty_decay_factor = 0.97

# ========== DEVELOPMENT ==========
development_enabled = true
critical_period_duration = 600
maturation_period_duration = 1800

# ========== CHECKPOINTING ==========
checkpoint_dir = ./advanced_checkpoints
max_checkpoints = 20000
checkpoint_compression = true

# ========== RANDOM SEED ==========
random_seed = 12345
```

## Migration Guide

### From Phase 1 to Phase 6

The configuration system was enhanced across phases:

**Phase 1-3**: Basic key=value pairs only
**Phase 4**: Added type support and validation
**Phase 5**: Enhanced with memory system parameters
**Phase 6**: Full integration with agent and neuromodulation systems

### Configuration Evolution

| Phase | Added Configuration | Notable Changes |
|-------|-------------------|-----------------|
| 1 | `neuron_count`, `region_count` | Basic simulation control |
| 2 | `stdp_ltp_weight`, `stdp_ltd_weight` | Plasticity rule configuration |
| 3 | `sensory_vision_width`, `sensory_vision_height` | Sensory system configuration |
| 4 | `working_memory_capacity`, `episodic_memory_max_episodes` | Memory system configuration |
| 5 | `curiosity_enabled`, `novelty_decay_factor` | Neuromodulation configuration |
| 6 | All Phase 5 parameters + `checkpoint_dir` | Full system integration |

## Testing Configuration

### Unit Tests

The configuration system includes:

```bash
# Run configuration tests
./test_config --test_basic
./test_config --test_types
./test_config --test_validation
./test_config --test_priority
```

### Integration Tests

For full integration tests with the brain system:

```bash
# Test with default configuration
./nlm_phase6_demo

# Test with custom configuration
./nlm_phase6_demo --config_file=./nlm_config.nlmc

# Test with command line overrides
./nlm_phase6_demo --neuron_count=5000 --region_count=2 --checkpoint_dir=./test_checkpoints
```

## Troubleshooting

### Common Configuration Issues

#### "Key not found" Errors

**Solution**: Always use `getOr<T>()` with sensible defaults:

```cpp
// Wrong - crashes if key not found
int value = config->get<int>("my_key");

// Right - provides default if key not found
int value = config->getOr<int>("my_key", 100);
```

#### Type Mismatch Errors

**Solution**: Ensure the requested type matches the stored type:

```cpp
// If value was stored as string, use string getter
std::string value = config->getOr<std::string>("my_key", "default");

// If value was stored as int, use int getter
int value = config->getOr<int>("my_key", 0);
```

#### Invalid Configuration Values

**Solution**: Validate configuration before use:

```cpp
void validate_config(const Config& config) {
    int neurons = config.getOr<int>("neuron_count", 1000);
    if (neurons <= 0 || neurons > 100000) {
        throw std::invalid_argument("neuron_count must be between 1 and 100000");
    }
    
    float timestep = config.getOr<float>("simulation_timestep", 0.001f);
    if (timestep <= 0.0f || timestep > 1.0f) {
        throw std::invalid_argument("simulation_timestep must be between 0.0001s and 1.0s");
    }
}
```

### Configuration Performance Tips

1. **Set configuration early**: Set most configuration before `initialize()`
2. **Minimize runtime changes**: Runtime configuration is convenient but may have performance impact
3. **Use appropriate data types**: Choose int vs float vs string based on usage patterns
4. **Document configuration**: Add comments to configuration files for maintainability

## Future Enhancements

### Planned Features

1. **JSON/YAML Support**: Enhanced configuration file format
2. **Configuration Schema**: Type definitions and validation rules
3. **Configuration Inheritance**: Base configurations with environment-specific overrides
4. **Configuration Monitoring**: Runtime configuration change tracking
5. **Configuration Templates**: Presets for common simulation scenarios

### Research Extension Ideas

The configuration system enables numerous research possibilities:

- **Adaptive Configuration**: Self-modifying configuration based on simulation performance
- **Configuration Optimization**: Automated parameter tuning for specific tasks
- **Multi-scale Configuration**: Different configuration levels (coarse vs fine-grained)
- **Distributed Configuration**: Network-based configuration sharing across simulations

## Conclusion

The NLM configuration system provides a robust, flexible foundation for controlling the complex neural simulation across all phases of development. Its type-safe, priority-based architecture ensures reliable parameter management while supporting the full range of experimental configurations needed for advanced brain research.

The configuration system is integral to NLM's adaptive learning capabilities, enabling researchers to explore how different parameter sets affect neural development, learning, and behavior without requiring code modifications.
