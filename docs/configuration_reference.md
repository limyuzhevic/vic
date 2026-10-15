# NLM - Configuration Reference Guide

## Overview

This comprehensive reference documents all configuration options available for NLM (Neural Learning Machine), including default values, description, and usage examples. This guide covers all configuration parameters for Brain, World, Agent, and Advanced settings.

## Configuration File Format

NLM uses a JSON-based configuration system that supports nested parameters. Configuration can be:

```python
import pynlm

# Create configuration
config = pynlm.createResearchConfig()

# Set individual values
config.set("brain.neuron_count", 1500)
config.set("brain.synapse_density", 0.12)

# Load from file
config.loadFromFile("my_config.json")

# Save to file
config.saveToFile("my_config.json")
```

## Brain Configuration

### Core Brain Parameters

| Parameter | Type | Default | Description | Range/Options |
|-----------|------|---------|-------------|---------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons in the brain | 100-100000 |
| `brain.synapse_density` | float | 0.1 | Probability of connection between neurons | 0.0-1.0 |
| `brain.connection_probability` | float | 0.05 | Additional connectivity factor | 0.0-1.0 |
| `brain.initial_weight_mean` | float | 0.5 | Mean of initial synaptic weights | 0.0-1.0 |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights | 0.0-1.0 |
| `brain.v_thresh` | float | -50.0 | Neuron firing threshold (mV) | -100.0 to -10.0 |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) | -80.0 to -60.0 |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) | -80.0 to -70.0 |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) | 5.0-50.0 |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) | 1.0-10.0 |

### Synapse and Plasticity Parameters

| Parameter | Type | Default | Description | Range/Options |
|-----------|------|---------|-------------|---------------|
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity | true/false |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate | 0.0-0.01 |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP potentiation time constant (ms) | 10.0-50.0 |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP depression time constant (ms) | 10.0-50.0 |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning | true/false |
| `plasticity.hebbian.learning_rate` | float | 0.001 | Hebbian learning rate | 0.0-0.01 |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity | true/false |
| `plasticity.structural.rate` | float | 0.0001 | Rate of synapse formation/removal | 0.00001-0.001 |

### Neuromodulation Parameters

| Parameter | Type | Default | Description | Range/Options |
|-----------|------|---------|-------------|---------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale factor | 0.0-3.0 |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration | true/false |
| `neuromod.curiosity.rate` | float | 0.05 | Curiosity decay rate | 0.0-0.1 |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection | true/false |
| `neuromod.novelty.sensitivity` | float | 0.8 | Novelty sensitivity threshold | 0.0-1.0 |

### Development System Parameters

| Parameter | Type | Default | Description | Range/Options |
|-----------|------|---------|-------------|---------------|
| `development.enable` | bool | true | Enable developmental learning | true/false |
| `development.maturation_rate` | float | 0.8 | Brain maturation rate | 0.1-1.0 |
| `development.critical_period.enabled` | bool | true | Enable critical period | true/false |
| `development.critical_period.duration` | int | 1000 | Critical period duration (steps) | 100-10000 |
| `development.critical_period.plasticity_boost` | float | 2.0 | Plasticity boost during critical period | 1.0-5.0 |

## World Configuration

### SimpleWorld Parameters

| Parameter | Type | Default | Description | Range/Options |
|-----------|------|---------|-------------|---------------|
| `world.width` | int | 20 | World width (cells) | 10-200 |
| `world.height` | int | 20 | World height (cells) | 10-200 |
| `world.vision_width` | int | 8 | Agent vision width (cells) | 4-32 |
| `world.vision_height` | int | 8 | Agent vision height (cells) | 4-32 |
| `world.max_energy` | float | 100.0 | Maximum energy for resources | 10.0-1000.0 |
| `world.reward_scale` | float | 1.0 | Reward magnitude scale | 0.1-10.0 |

### World Object Types

| Object Type | Parameters | Description |
|-------------|------------|-------------|
| Resource | `value` (float), `max_value` (float) | Food, energy source |
| Hazard | `damage` (float), `duration` (float) | Dangerous objects |
| Wall | `radius` (float), `passable` (bool) | Physical barriers |
| Marker | `id` (str), `data` (dict) | Special points of interest |

## Agent Configuration

### AgentBrain Parameters

| Parameter | Type | Default | Description | Range/Options |
|-----------|------|---------|-------------|---------------|
| `agent.reward_modulation_enabled` | bool | true | Enable reward modulation | true/false |
| `agent.curiosity_enabled` | bool | true | Enable curiosity-driven exploration | true/false |
| `agent.development_enabled` | bool | true | Enable developmental learning | true/false |
| `agent.structural_plasticity_enabled` | bool | true | Enable structural plasticity | true/false |
| `agent.novelty_detection_enabled` | bool | true | Enable novelty detection | true/false |

### Agent Learning Parameters

| Parameter | Type | Default | Description | Range/Options |
|-----------|------|---------|-------------|---------------|
| `agent.prediction_horizon` | int | 3 | Number of future steps to predict | 1-10 |
| `agent.learning_rate` | float | 0.01 | Learning rate for updates | 0.001-0.1 |
| `agent.exploration_rate` | float | 0.1 | Random exploration probability | 0.0-0.5 |
| `agent.exploitation_rate` | float | 0.9 | Exploitation probability | 0.5-1.0 |

## Configuration Presets

### Research Configuration

```python
config = pynlm.createResearchConfig()
# Neurons: 2000
# Synapse Density: 0.15
# All plasticity enabled
# Enhanced neuromodulation
# Development enabled
```

### Development Configuration

```python
config = pynlm.createDevelopmentConfig()
# Neurons: 1500
# Focused on training and learning
# Maturation rate: 0.8
# Similar to research but optimized for training
```

### Testing Configuration

```python
config = pynlm.createTestingConfig()
# Neurons: 100 (minimal)
# All advanced features disabled
# Predictable behavior for unit testing
```

### Production Configuration

```python
config = pynlm.createProductionConfig()
# Neurons: 1000
# Stable moderate plasticity
# Curiosity disabled for predictability
# Development disabled for stability
```

### Default Configuration

```python
config = pynlm.createDefaultConfig()
# Original settings (neuron_count: 1000)
# Backward compatibility
```

## Configuration Validation

### Validation Rules

| Parameter | Validation Rule | Error Message |
|-----------|----------------|---------------|
| `brain.neuron_count` | Must be positive integer | "Neuron count must be a positive integer" |
| `brain.synapse_density` | Must be between 0.0 and 1.0 | "Synapse density must be between 0.0 and 1.0" |
| `plasticity.stdp.learning_rate` | Must be positive | "STDP learning rate must be positive" |
| `neuromod.dopamine.scale` | Must be positive | "Dopamine scale must be positive" |
| `development.maturation_rate` | Must be between 0.1 and 1.0 | "Maturation rate must be between 0.1 and 1.0" |

### Validation Example

```python
import pynlm

# Create configuration
config = pynlm.createResearchConfig()

# Validate configuration
errors, warnings = pynlm.validate_config(config)

if errors:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")

if warnings:
    print("Configuration warnings:")
    for warning in warnings:
        print(f"  - {warning}")
```

## Advanced Configuration Examples

### Custom Configuration Template

```python
custom_config = {
    "brain": {
        "neuron_count": 3000,
        "synapse_density": 0.12,
        "plasticity": {
            "stdp": {
                "enable": True,
                "learning_rate": 0.002,
                "tau_plus": 25.0,
                "tau_minus": 25.0
            },
            "hebbian": {
                "enable": True,
                "learning_rate": 0.0015
            },
            "structural": {
                "enable": True,
                "rate": 0.0002
            }
        }
    },
    "neuromodulation": {
        "dopamine": {"scale": 1.5},
        "curiosity": {"enable": True, "rate": 0.06},
        "novelty": {"enable": True, "sensitivity": 0.9}
    },
    "development": {
        "enable": True,
        "maturation_rate": 0.7,
        "critical_period": {
            "enabled": True,
            "duration": 1500,
            "plasticity_boost": 3.0
        }
    }
}

# Convert to NLM config
config = pynlm.createDefaultConfig()

# Set all custom values
config.set("brain.neuron_count", custom_config["brain"]["neuron_count"])
config.set("brain.synapse_density", custom_config["brain"]["synapse_density"])
config.set("plasticity.stdp.enable", custom_config["brain"]["plasticity"]["stdp"]["enable"])
config.set("plasticity.stdp.learning_rate", custom_config["brain"]["plasticity"]["stdp"]["learning_rate"])
config.set("plasticity.stdp.tau_plus", custom_config["brain"]["plasticity"]["stdp"]["tau_plus"])
config.set("plasticity.stdp.tau_minus", custom_config["brain"]["plasticity"]["stdp"]["tau_minus"])
config.set("plasticity.hebbian.enable", custom_config["brain"]["plasticity"]["hebbian"]["enable"])
config.set("plasticity.hebbian.learning_rate", custom_config["brain"]["plasticity"]["hebbian"]["learning_rate"])
config.set("plasticity.structural.enable", custom_config["brain"]["plasticity"]["structural"]["enable"])
config.set("plasticity.structural.rate", custom_config["brain"]["plasticity"]["structural"]["rate"])
config.set("neuromod.dopamine.scale", custom_config["neuromodulation"]["dopamine"]["scale"])
config.set("neuromod.curiosity.enable", custom_config["neuromodulation"]["curiosity"]["enable"])
config.set("neuromod.curiosity.rate", custom_config["neuromodulation"]["curiosity"]["rate"])
config.set("neuromod.novelty.enable", custom_config["neuromodulation"]["novelty"]["enable"])
config.set("neuromod.novelty.sensitivity", custom_config["neuromodulation"]["novelty"]["sensitivity"])
config.set("development.enable", custom_config["development"]["enable"])
config.set("development.maturation_rate", custom_config["development"]["maturation_rate"])
config.set("development.critical_period.enabled", custom_config["development"]["critical_period"]["enabled"])
config.set("development.critical_period.duration", custom_config["development"]["critical_period"]["duration"])
config.set("development.critical_period.plasticity_boost", custom_config["development"]["critical_period"]["plasticity_boost"])
```

### Configuration File Operations

```python
import pynlm
import json

# Save configuration to JSON file
config = pynlm.createResearchConfig()
pynlm.save_config_to_file(config, "my_research_config.json")

# Load configuration from JSON file
loaded_config = pynlm.load_config_from_file("my_research_config.json")

# Example: Modify loaded configuration
loaded_config.set("brain.neuron_count", 1500)
loaded_config.set("plasticity.stdp.learning_rate", 0.002)
pynlm.save_config_to_file(loaded_config, "custom_config.json")
```

## Configuration Debugging

### Common Configuration Issues

| Issue | Symptoms | Solution |
|-------|----------|----------|
| Invalid neuron count | Segmentation fault or crash | Set positive integer (100+) |
| Negative plasticity rate | No learning or crashes | Set positive value |
| Invalid world dimensions | World creation fails | Set positive integers (10+) |
| Missing required keys | Configuration errors | Add all required parameters |

### Debugging Configuration

```python
import pynlm

def debug_configuration(config):
    """Debug configuration and identify issues."""
    
    print("Configuration Debug Report:")
    print("=" * 40)
    
    # Check for required keys
    required_keys = ["brain.neuron_count", "brain.synapse_density"]
    
    print("\nRequired keys check:")
    for key in required_keys:
        if config.has(key):
            value = config.get(key)
            print(f"  ✓ {key}: {value}")
        else:
            print(f"  ✗ {key}: MISSING")
    
    # Check parameter ranges
    print("\nParameter range checks:")
    
    # Neuron count
    if config.has("brain.neuron_count"):
        neuron_count = config.get("brain.neuron_count")
        if 100 <= neuron_count <= 100000:
            print(f"  ✓ brain.neuron_count ({neuron_count}): OK")
        else:
            print(f"  ✗ brain.neuron_count ({neuron_count}): OUT OF RANGE")
    
    # Synapse density
    if config.has("brain.synapse_density"):
        density = config.get("brain.synapse_density")
        if 0.0 <= density <= 1.0:
            print(f"  ✓ brain.synapse_density ({density}): OK")
        else:
            print(f"  ✗ brain.synapse_density ({density}): OUT OF RANGE")
    
    # Learning rates
    if config.has("plasticity.stdp.learning_rate"):
        stdp_rate = config.get("plasticity.stdp.learning_rate")
        if stdp_rate > 0:
            print(f"  ✓ plasticity.stdp.learning_rate ({stdp_rate}): OK")
        else:
            print(f"  ✗ plasticity.stdp.learning_rate ({stdp_rate}): MUST BE POSITIVE")
    
    if config.has("plasticity.hebbian.learning_rate"):
        hebbian_rate = config.get("plasticity.hebbian.learning_rate")
        if hebbian_rate > 0:
            print(f"  ✓ plasticity.hebbian.learning_rate ({hebbian_rate}): OK")
        else:
            print(f"  ✗ plasticity.hebbian.learning_rate ({hebbian_rate}): MUST BE POSITIVE")
    
    # Validate entire configuration
    print("\nConfiguration validation:")
    errors, warnings = pynlm.validate_config(config)
    
    if errors:
        print("  Errors:")
        for error in errors:
            print(f"    - {error}")
    
    if warnings:
        print("  Warnings:")
        for warning in warnings:
            print(f"    - {warning}")
    
    if not errors and not warnings:
        print("  ✓ Configuration is valid!")

# Example usage
config = pynlm.createResearchConfig()
debug_configuration(config)
```

## Configuration Performance Tips

### Performance Optimization

| Optimization | Description | Effect |
|--------------|-------------|--------|
| Use presets | Use `createResearchConfig()` etc. instead of custom | Faster setup, optimized defaults |
| Minimal testing | Use `createTestingConfig()` for performance testing | Better performance measurement |
| Batch configuration | Set multiple parameters at once | Reduced overhead |
| Reuse configurations | Keep config objects for multiple simulations | Memory efficiency |

### Memory-Efficient Configuration

```python
import pynlm
import gc

# Create configuration pool for reuse
class ConfigPool:
    def __init__(self, size=10):
        self.configs = []
        self.size = size
    
    def get_config(self, config_type="research"):
        if len(self.configs) < self.size:
            # Create new config
            if config_type == "research":
                config = pynlm.createResearchConfig()
            elif config_type == "development":
                config = pynlm.createDevelopmentConfig()
            elif config_type == "testing":
                config = pynlm.createTestingConfig()
            else:
                config = pynlm.createProductionConfig()
            
            self.configs.append(config)
        
        # Return oldest config (reuse)
        config = self.configs.pop(0)
        self.configs.append(config)
        return config
    
    def clear(self):
        """Clear all configurations."""
        self.configs.clear()
        gc.collect()

# Usage
config_pool = ConfigPool(size=5)

# Get configs (will reuse from pool)
for i in range(10):
    config = config_pool.get_config("research")
    print(f"Config {i}: {config.get('brain.neuron_count')} neurons")

config_pool.clear()
```

## Configuration Migration Guide

### From Old to New API

#### Old Configuration Pattern

```python
# Old configuration pattern
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)
```

#### New Configuration Pattern

```python
# New configuration pattern with enhanced features
config = pynlm.createResearchConfig()  # Use presets for research

# Optional: Load additional settings from file
config.loadFromFile("config.json")

# Set specific overrides
config.set("plasticity.stdp.learning_rate", 0.002)
```

### Migration Examples

```python
import pynlm

# Migration from simple to research configuration
old_config = pynlm.createDefaultConfig()
old_config.set("brain.neuron_count", 1000)

# Convert to research configuration
new_config = pynlm.createResearchConfig()
new_config.set("brain.neuron_count", old_config.get("brain.neuron_count", 1000))
new_config.set("brain.synapse_density", old_config.get("brain.synapse_density", 0.1))
new_config.set("plasticity.stdp.learning_rate", old_config.get("plasticity.stdp.learning_rate", 0.001))

print("Configuration migrated successfully!")
print(f"Old: {old_config.get('brain.neuron_count')} neurons")
print(f"New: {new_config.get('brain.neuron_count')} neurons")
```

## Conclusion

This Configuration Reference Guide provides comprehensive documentation for all NLM configuration options:

1. **Complete Parameter Reference**: All Brain, World, and Agent parameters with ranges
2. **Configuration Presets**: Research, Development, Testing, Production, and Default configurations
3. **Configuration Validation**: Rules and validation examples
4. **Debugging Tools**: Configuration debugging and troubleshooting
5. **Performance Optimization**: Memory-efficient configuration patterns
6. **Migration Guide**: From old to new API patterns
7. **Best Practices**: Configuration management and optimization tips

This guide serves as a comprehensive reference for researchers and developers working with NLM configurations, enabling efficient and effective use of the enhanced NLM Python API.

-----
*End of Configuration Reference Guide*