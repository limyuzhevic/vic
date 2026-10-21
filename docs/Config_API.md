# NLM Configuration API

## Config Class

The `Config` class provides a type-safe configuration system for NLM with JSON file support, command-line argument parsing, and runtime configuration management.

### Overview

The configuration system uses a key-value store with typed values and supports multiple sources (default, file, command-line, runtime). All configuration values are optional, with sensible defaults provided.

### Basic Usage

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Query configuration values
if config.has("brain.neuron_count"):
    neuron_count = config.get("brain.neuron_count")

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("random_seed", 42)

# Get with default values
max_energy = config.getOr("world.max_energy", 100.0)
neuron_count = config.getOr("brain.neuron_count", 1000)

# Save configuration to file
config.saveToFile("my_config.json")

# Load configuration from file
config.loadFromFile("my_config.json")
```

### Configuration Loading

The configuration can be loaded from multiple sources with the following precedence (runtime > command-line > file > default):

```python
config = pynlm.createDefaultConfig()

# Load from JSON file
config.loadFromFile("config.json")

# Load from command line arguments
# Usage: ./program --config.param=value --brain.neuron_count=2000
config.loadFromArgs(len(sys.argv), sys.argv)

# Set runtime values (override all previous)
config.set("brain.neuron_count", 3000, ConfigSource.Runtime)
```

### Configuration Keys and Types

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density (0-1) |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |
| `plasticity.stdp.enable` | bool | true | Enable STDP plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP potentiation time constant |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP depression time constant |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian plasticity |
| `plasticity.hebbian.learning_rate` | float | 0.001 | Hebbian learning rate |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.growth_rate` | float | 0.0001 | Synaptogenesis rate |
| `plasticity.structural.pruning_rate` | float | 0.00001 | Pruning rate |
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.rate` | float | 0.1 | Curiosity signal update rate |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.novelty.threshold` | float | 0.5 | Novelty detection threshold |

### Config Class Methods

#### Construction and Destruction

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Configuration is automatically destroyed when out of scope
```

#### File Operations

```python
# Load configuration from JSON file
# Returns True if successful, False if file not found or invalid
config.loadFromFile("config.json")

# Save configuration to JSON file
config.saveToFile("config.json")

# Example JSON config file
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "tau_mem": 20.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.001
        }
    }
}
```

#### Command Line Arguments

```python
import sys

# Usage: program --config.file=my_config.json --brain.neuron_count=2000
# or: program --config.key=value

args = ["program", "--config.file=proj_config.json", "--brain.neuron_count=2000"]
config.loadFromArgs(len(args), args)
```

#### Value Access

```python
# Check if key exists
if config.has("brain.neuron_count"):
    print("Neuron count configured")

# Get value with type checking
neuron_count = config.get("brain.neuron_count")
if neuron_count:
    print(f"Neuron count: {neuron_count}")

# Get value with default fallback
max_energy = config.getOr("world.max_energy", 100.0)
print(f"Max energy: {max_energy}")

# Get all keys
keys = config.getKeys()
print(f"Configured keys: {keys}")
```

#### Value Modification

```python
# Set values with explicit source
config.set("brain.neuron_count", 1500, ConfigSource.File)
config.set("brain.neuron_count", 2000, ConfigSource.CommandLine)
config.set("brain.neuron_count", 3000, ConfigSource.Runtime)

# Convenience methods for common types
config.set("brain.neuron_count", 1000)           # int
config.set("brain.synapse_density", 0.1)         # float
config.set("neuromod.curiosity.enable", true)    # bool
config.set("world.name", "MyWorld")              # string
config.set("brain.neuron_ids", [1, 2, 3])       # vector<int>
config.set("brain.weights", [0.1, 0.5, 0.9])    # vector<float>
```

#### Utility Methods

```python
# Get configuration summary
print(config.summary())

# Clear all configuration (use with caution)
config.clear()
```

### Configuration Validation

```python
import pynlm

config = pynlm.createDefaultConfig()

# Set basic configuration
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.enable", true)

# Validate configuration
errors = []

if config.has("brain.neuron_count"):
    neuron_count = config.get("brain.neuron_count")
    if neuron_count and neuron_count <= 0:
        errors.append("Neuron count must be positive")

if config.has("brain.synapse_density"):
    density = config.get("brain.synapse_density")
    if density and (density < 0.0 or density > 1.0):
        errors.append("Synapse density must be between 0 and 1")

if errors:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")
else:
    print("Configuration is valid")
```

### Advanced Configuration Patterns

#### Parameter Groups
```python
# Configuration can be organized by functional groups
brain_config = {
    "neuron_count": 1000,
    "synapse_density": 0.1,
    "v_thresh": -50.0,
    "tau_mem": 20.0
}

plasticity_config = {
    "stdp": {
        "enable": true,
        "learning_rate": 0.001
    },
    "hebbian": {
        "enable": true,
        "learning_rate": 0.001
    }
}

neuromodulation_config = {
    "dopamine": {
        "scale": 1.0,
        "enable": true
    }
}
```

#### Dynamic Configuration
```python
config = pynlm.createDefaultConfig()

# Runtime parameter adjustment
for i in range(10):
    config.set("plasticity.stdp.learning_rate", 0.001 * (i + 1))
    # ... run simulation ...

# Save optimized configuration
config.saveToFile("optimized_config.json")
```

#### Multi-source Configuration
```python
config = pynlm.createDefaultConfig()

# Load base configuration
config.loadFromFile("defaults.json")

# Override with command line arguments
config.loadFromArgs(len(sys.argv), sys.argv)

# Apply runtime settings
config.set("brain.neuron_count", 500, ConfigSource.Runtime)

# Save final configuration
config.saveToFile("final_config.json")
```

### Common Configuration Patterns

#### High-Performance Configuration
```python
# Optimized for speed and efficiency
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)           # Fewer neurons
config.set("brain.synapse_density", 0.05)       # Lower connectivity
config.set("simulation_timestep", 0.01)         # Larger timestep
config.set("plasticity.stdp.enable", false)     # Disable STDP
config.set("plasticity.hebbian.enable", true)   # Keep basic Hebbian
config.set("neuromod.curiosity.enable", false)  # Disable curiosity
config.set("development.enabled", false)         # Disable development
```

#### Research Configuration
```python
# Optimized for scientific research and exploration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)           # Larger network
config.set("brain.synapse_density", 0.2)         # Higher connectivity
config.set("plasticity.stdp.enable", true)       # Full STDP
config.set("plasticity.stdp.learning_rate", 0.01)  # Stronger learning
config.set("plasticity.hebbian.enable", true)
config.set("plasticity.structural.enable", true)  # Structural plasticity
config.set("neuromod.dopamine.scale", 2.0)       # Stronger reward
config.set("neuromod.curiosity.enable", true)   # Encourage exploration
config.set("neuromod.novelty.enable", true)     # Novelty detection
config.set("development.enabled", true)          # Developmental plasticity
```

#### Minimal Configuration
```python
# Just enough for basic simulation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 10)            # Tiny network
config.set("brain.synapse_density", 0.8)        # High connectivity
config.set("plasticity.stdp.enable", false)     # No plasticity
config.set("plasticity.hebbian.enable", true)   # Basic Hebbian only
config.set("neuromod.curiosity.enable", false)  # No neuromodulation
```

### Configuration Best Practices

1. **Use Constants for Parameter Names**: Define string constants for configuration keys to avoid typos

2. **Validation Before Simulation**: Always validate configuration before running simulations

3. **Gradual Parameter Adjustment**: Start with simple configurations and add complexity

4. **Save Optimized Configurations**: Save working configurations as templates

5. **Environment-specific Configurations**: Use different configs for development, testing, and production

6. **Configuration Documentation**: Document configuration parameters with clear descriptions

### Error Handling

```python
def safe_simulation(config):
    try:
        # Load configuration
        if not config.loadFromFile("config.json"):
            print("Warning: Could not load config file, using defaults")
        
        # Validate configuration
        if not validate_config(config):
            raise ValueError("Invalid configuration")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(1000):
            brain.step(step)
            
    except Exception as e:
        print(f"Simulation error: {e}")
        # Cleanup
        if 'brain' in locals():
            brain.reset()
        raise
```

This comprehensive configuration system provides flexible, type-safe configuration management for NLM simulations with multiple loading sources and validation capabilities.