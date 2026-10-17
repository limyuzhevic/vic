# NLM Configuration System Documentation

## Overview

The NLM configuration system provides a comprehensive and flexible way to control all aspects of neural network simulation. It supports multiple file formats, provides type safety, validation, and inheritance mechanisms for complex configuration management.

## Configuration System Architecture

### Configuration Hierarchy

```
Configuration System
├── Core Configuration (neuron count, simulation parameters)
├── Brain Configuration (architecture, connectivity)
├── Plasticity Configuration (learning rules, weights)
├── Neuromodulation Configuration (behavioral control)
├── Development Configuration (ontogeny, maturation)
├── Memory Configuration (cognitive architectures)
├── Performance Configuration (optimization settings)
└── Environment Configuration (simulation parameters)
```

### Configuration Files

The configuration system supports multiple file formats:

#### Plain Text (.cfg)
```
# Comment
key = value
```

#### JSON (.json)
```json
{
    "key": "value"
}
```

#### YAML (.yaml)
```yaml
key: value
```

#### TOML (.toml)
```toml
key = "value"
```

#### INI (.ini)
```ini
[section]
key = value
```

#### Properties (.properties)
```properties
key = value
```

## Configuration Categories

### 1. Core Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `random_seed` | int64 | - | Random seed for reproducibility |
| `simulation_timestep` | float | 0.001 | Time step for simulation |
| `neuron_count` | uint64 | 1000 | Number of neurons in the brain |
| `region_count` | uint32 | 10 | Number of neural regions |
| `connection_probability` | float | 0.1 | Probability of synaptic connections |

### 2. Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain_type` | string | "standard" | Type of brain architecture |
| `enable_plasticity` | bool | true | Enable synaptic plasticity |
| `plasticity_rule` | string | "stdp" | Plasticity rule (stdp, hebbian) |
| `initial_weight_mean` | float | 0.1 | Initial synaptic weight mean |
| `initial_weight_std` | float | 0.05 | Initial synaptic weight standard deviation |

### 3. Plasticity Settings

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `stdp_ltp_weight` | float | 0.01 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.01 | STDP long-term depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant |
| `synaptogenesis_rate` | float | 0.001 | Rate of new synapse formation |
| `pruning_rate` | float | 0.0001 | Rate of synapse pruning |

### 4. Neuromodulation Settings

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `enable_neuromodulation` | bool | true | Enable neuromodulation system |
| `curiosity_strength` | float | 1.0 | Strength of curiosity-driven learning |
| `novelty_threshold` | float | 0.5 | Threshold for novelty detection |
| `reward_decay` | float | 0.99 | Reward decay factor |
| `dopamine_strength` | float | 1.0 | Dopamine signal strength |

### 5. Simulation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `max_steps` | uint64 | 10000 | Maximum simulation steps |
| `enable_checkpointing` | bool | false | Enable checkpoint saving |
| `checkpoint_interval` | uint64 | 1000 | Checkpoint saving interval |
| `enable_profiling` | bool | false | Enable performance profiling |

### 6. Output Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `output_dir` | string | "./results" | Directory for output files |
| `save_weights` | bool | true | Save neural network weights |
| `log_level` | string | "info" | Log level (debug, info, warning, error) |
| `enable_visualization` | bool | false | Enable visualization output |

## Configuration Management

### Configuration Loading

```cpp
ArgsParser parser;
parser.parse(argc, argv);  // Parse command line

// Load from configuration file
parser.loadConfiguration("my_config.cfg");

// Access configuration values
int64_t neuronCount = parser.get<int64_t>("neuron_count");
float timestep = parser.get<float>("simulation_timestep");
bool plasticityEnabled = parser.get<bool>("enable_plasticity");
```

### Configuration Validation

```cpp
// Get default configuration schema
ConfigurationSchema schema = parser.getDefaultSchema();

// Validate current configuration
bool isValid = parser.validate(schema);

// Get validation errors
std::vector<ValidationError> errors = parser.getValidationErrors();
```

### Configuration Inheritance

Configuration inheritance allows child configurations to extend parent configurations:

```cpp
// Parent configuration
parent_config = {
    "neuron_count": 1000,
    "enable_plasticity": true,
    "plasticity_rule": "stdp"
}

// Child configuration (inherits parent, overrides specific values)
child_config = {
    "neuron_count": 2000,  // Override
    "enable_neuromodulation": true,  // Add new value
    "reward_decay": 0.95  // Override
}

// Merged configuration will have all parent values with child overrides
```

## Presets

The configuration system includes several predefined presets:

### Standard Preset
- Balanced configuration for general use
- Includes core brain functionality
- Moderate performance

```json
{
    "neuron_count": 1000,
    "enable_plasticity": true,
    "plasticity_rule": "stdp",
    "enable_neuromodulation": true,
    "enable_curiosity": true,
    "enable_novelty": true,
    "max_steps": 10000,
    "output_dir": "./results"
}
```

### Fast Preset
- Optimized for speed
- Reduced accuracy
- Fewer plasticity rules

```json
{
    "neuron_count": 500,
    "enable_plasticity": true,
    "plasticity_rule": "hebbian",  // Simpler rule
    "enable_neuromodulation": false,  // Disable for speed
    "enable_curiosity": false,
    "enable_novelty": false,
    "max_steps": 5000,
    "enable_profiling": false
}
```

### High Precision Preset
- Optimized for accuracy
- Enhanced learning capabilities
- Slower performance

```json
{
    "neuron_count": 5000,
    "enable_plasticity": true,
    "plasticity_rule": "stdp",
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.01,
    "stdp_tau": 50.0,
    "enable_neuromodulation": true,
    "curiosity_strength": 2.0,
    "novelty_threshold": 0.3,
    "max_steps": 50000,
    "enable_checkpointing": true,
    "output_dir": "./high_precision_results"
}
```

### Debug Preset
- Enhanced debugging output
- Detailed logging
- Performance profiling enabled

```json
{
    "neuron_count": 100,
    "enable_plasticity": true,
    "enable_neuromodulation": true,
    "enable_curiosity": true,
    "enable_novelty": true,
    "enable_prediction_error": true,
    "max_steps": 1000,
    "log_level": "debug",
    "debug_neurons": [1, 2, 3, 4, 5],
    "enable_profiling": true,
    "enable_visualization": true
}
```

## Command Line Configuration

### Core Options

| Option | Short | Long | Type | Description |
|--------|-------|------|------|-------------|
| `-n` | `--neuron-count` | int64 | Number of neurons |
| `-t` | `--simulation-timestep` | float | Simulation time step |
| `-p` | `--enable-plasticity` | bool | Enable/disable plasticity |
| `-v` | `--verbose` | bool | Enable verbose output |
| `-h` | `--help` | - | - | Show help information |

### Configuration Options

| Option | Long | Type | Description |
|--------|------|------|-------------|
| `--config=<file>` | Configuration file |
| `--preset=<name>` | string | Apply predefined preset |
| `--random-seed=<value>` | int64 | Random seed |
| `--output-dir=<dir>` | string | Output directory |

### Override Configuration

```bash
# Use base configuration
./build/nlm --config=base_config.cfg

# Override specific parameters
./build/nlm --config=base_config.cfg --neuron-count=2000 --enable-plasticity=true

# Use preset and override
./build/nlm --preset=debug --neuron-count=200
```

## Python Configuration API

```python
import pynlm

# Create advanced brain configuration
brain_config = pynlm.createAdvancedBrainConfig({
    'neuron_count': 2000,
    'simulation_timestep': 0.001,
    'enable_plasticity': True,
    'enable_neuromodulation': True,
    'enable_curiosity': True,
    'enable_novelty': True,
    'enable_prediction_error': True,
    'enable_structural_plasticity': True,
    'enable_development': True,
    'learning_rate': 0.001,
    'discount_factor': 0.99
})

# Validate configuration
schema = pynlm.python.AdvancedBrainConfiguration.getDefaultBrainSchema()
is_valid = pynlm.python.AdvancedBrainConfiguration.validateConfig(brain_config, schema)

# Apply preset
specialized_config = pynlm.python.AdvancedBrainConfiguration.applyPreset("research")

# Export configuration
pynlm.python.AdvancedBrainConfiguration.exportConfiguration("brain_config.json")

# Import configuration
imported_config = pynlm.python.AdvancedBrainConfiguration.importConfiguration("brain_config.json")
```

## Configuration Validation

### Schema Validation

```cpp
// Define configuration schema
struct NeuronConfig {
    uint64_t count;
    float timestep;
    bool plasticity;
};

// Validate configuration
bool validateNeuronConfig(const NeuronConfig& config) {
    if (config.count == 0) return false;
    if (config.timestep <= 0.0) return false;
    if (config.timestep > 0.1) return false;
    return true;
}
```

### Type Safety

The configuration system provides type-safe argument parsing:

- Automatic type conversion (string to int, float, bool)
- Type validation with detailed error messages
- Default values for missing parameters
- Configuration schema checking

## Configuration Examples

### Example 1: Basic Configuration

```bash
./build/nlm --neuron-count=1000 --simulation-timestep=0.01 --max-steps=10000
```

### Example 2: Advanced Configuration with File

Create `advanced_config.json`:

```json
{
    "neuron_count": 5000,
    "simulation_timestep": 0.001,
    "enable_plasticity": true,
    "plasticity_rule": "stdp",
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.01,
    "enable_neuromodulation": true,
    "curiosity_strength": 1.0,
    "enable_novelty": true,
    "enable_prediction_error": true,
    "enable_checkpointing": true,
    "output_dir": "./advanced_results",
    "log_level": "info"
}
```

Run:

```bash
./build/nlm --config=advanced_config.json
```

### Example 3: Python Configuration

```python
import pynlm
import numpy as np

# Create configuration with validation
brain_config = pynlm.createAdvancedBrainConfig({
    'neuron_count': 3000,
    'enable_plasticity': True,
    'enable_neuromodulation': True,
    'enable_curiosity': True,
    'enable_novelty': True,
    'enable_prediction_error': True,
    'enable_structural_plasticity': False,  // Disable for this run
    'learning_rate': 0.001,
    'discount_factor': 0.99,
    'replay_buffer_size': 5000
})

# Validate configuration
schema = pynlm.python.AdvancedBrainConfiguration.getDefaultBrainSchema()
if not pynlm.python.AdvancedBrainConfiguration.validateConfig(brain_config, schema):
    raise ValueError("Invalid configuration")

# Export for sharing
pynlm.python.AdvancedBrainConfiguration.exportConfiguration("my_brain_config.json")

# Use in simulation
brain = pynlm.createBrain(brain_config)
```

## Configuration Best Practices

### DO

1. **Use Configuration Files**: Complex configurations should be in files
2. **Validate Configurations**: Always validate before use
3. **Document Configurations**: Add comments in configuration files
4. **Use Presets**: Start with presets and modify as needed
5. **Test Configurations**: Test configurations in small-scale first
6. **Version Control**: Keep configuration files in version control
7. **Environment Variables**: Use for environment-specific settings
8. **Logging**: Enable appropriate logging for debugging

### DON'T

1. **Hardcode Values**: Use configuration files for all parameters
2. **Skip Validation**: Always validate configuration before use
3. **Use Magic Numbers**: Use named constants or configuration
4. **Ignore Performance**: Consider performance implications of configuration
5. **Share Sensitive Data**: Don't commit sensitive configurations to version control
6. **Use Inconsistent Formats**: Stick to one configuration format
7. **Skip Testing**: Test configurations thoroughly
8. **Forget Documentation**: Document configuration usage

## Configuration Reference

### Command Line Reference

See `/workspace/app/docs/reference/command_line_reference.md` for detailed command line options and usage.

### Python API Reference

See `/workspace/app/docs/reference/api_reference.md` for detailed Python API documentation.

### Configuration Schema Reference

See `/workspace/app/docs/reference/configuration_reference.md` for detailed configuration schema documentation.

## Conclusion

The NLM configuration system provides a comprehensive, flexible, and type-safe way to control neural network simulations. With support for multiple file formats, validation, inheritance, and presets, it enables both simple and complex configuration management.

Key features include:
- Multiple configuration file formats (CFG, JSON, YAML, TOML, INI, Properties)
- Type-safe argument parsing with automatic conversion
- Comprehensive validation with detailed error messages
- Configuration inheritance for hierarchical management
- Predefined presets for common use cases
- Python API for programmatic configuration
- Command line overrides for easy testing and debugging

The configuration system is designed to support the full range of NLM capabilities, from simple demos to large-scale research simulations, while maintaining type safety, validation, and ease of use.

For more information, refer to the specific sections of this documentation or consult the Python API documentation for detailed implementation details.