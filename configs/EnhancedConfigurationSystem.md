# Configuration System Enhancement for NLM

## Overview

This document describes the enhanced configuration system for NLM (Neural Learning Machine). The configuration system has been significantly improved to provide:

1. **Schema validation** - Ensures configuration values meet type and range requirements
2. **Type safety** - Better handling of configuration value types
3. **Documentation support** - Built-in parameter descriptions and examples
4. **Validation rules** - Custom validation for important parameters
5. **Import/export** - Save/load configuration with validation

## Configuration Schema

The NLM configuration system uses JSON format and supports the following parameters:

### Core Simulation Parameters

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `random_seed` | int64 | 42 | Random seed for reproducible experiments | `>= 0` |
| `simulation_timestep` | double | 0.001 | Simulation timestep in seconds | `> 0.0, < 1.0` |
| `neuron_count` | size_t | 1000 | Number of neurons in the brain | `>= 10, <= 100000` |
| `region_count` | size_t | 1 | Number of brain regions | `>= 1, <= 10` |
| `connection_probability` | float | 0.1 | Connection probability for random connectivity | `[0.0, 1.0]` |

### Plasticity Configuration

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `plasticity_learning_rate` | float | 0.01 | General plasticity learning rate | `>= 0.0, <= 1.0` |
| `stdp_ltp_weight` | float | 0.01 | STDP LTP weight | `>= 0.0, <= 1.0` |
| `stdp_ltd_weight` | float | 0.012 | STDP LTD weight | `>= 0.0, <= 1.0` |
| `stdp_time_constant` | float | 20.0 | STDP time constant | `>= 1.0, <= 100.0` |

### Development Configuration

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `development_synaptogenesis_rate` | float | 0.001 | Synaptogenesis rate | `[0.0, 0.01]` |
| `development_pruning_rate` | float | 0.0001 | Pruning rate | `[0.0, 0.001]` |

### Neuromodulation Configuration

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `dopamine_baseline` | float | 0.1 | Baseline dopamine level | `[0.0, 1.0]` |
| `curiosity_decay_rate` | float | 0.05 | Curiosity decay rate | `[0.0, 0.2]` |
| `novelty_threshold` | float | 0.5 | Novelty detection threshold | `[0.0, 1.0]` |

### Reward Configuration

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `reward_discount_factor` | float | 0.99 | Reward discount factor | `[0.5, 1.0]` |

### Environment Configuration

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `environment_name` | string | "GridWorld" | Environment name | Non-empty |
| `environment_width` | size_t | 10 | Environment width | `>= 5, <= 100` |
| `environment_height` | size_t | 10 | Environment height | `>= 5, <= 100` |

### Logging Configuration

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `log_level` | string | "INFO" | Logging level | ["DEBUG", "INFO", "WARN", "ERROR"] |
| `log_to_file` | bool | false | Log to file | |
| `log_filename` | string | "nlm.log" | Log filename | |

### Simulation Configuration

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `max_simulation_steps` | size_t | 10000 | Maximum simulation steps | `>= 100, <= 1000000` |
| `simulation_time_limit` | double | 0.0 | Simulation time limit | `>= 0.0` |

### Visualization Configuration

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `visualization_enabled` | bool | false | Enable visualization | |
| `visualization_update_rate` | float | 30.0 | Visualization update rate | `>= 1.0, <= 1000.0` |

## Python Configuration API

The enhanced configuration system provides a comprehensive Python API:

### Basic Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 3)
config.set("brain.connection_probability", 0.15)

# Get configuration values
neuron_count = config.getOr("brain.neuron_count", 1000)
connection_probability = config.getOr("brain.connection_probability", 0.1)

# Check if configuration exists
if config.has("brain.plasticity_learning_rate"):
    learning_rate = config.getOr("brain.plasticity_learning_rate", 0.01)
```

### Advanced Configuration Operations

```python
# Load configuration from file
config.loadFromFile("my_config.json")

# Save configuration to file
config.saveToFile("experiment_config.json")

# Get all configuration keys
keys = config.getKeys()

# Configuration validation
def validate_config(config):
    # Required parameters
    required_params = ["brain.neuron_count", "brain.region_count"]
    for param in required_params:
        if not config.has(param):
            raise ValueError(f"Missing required parameter: {param}")
    
    # Range validation
    neuron_count = config.getOr("brain.neuron_count", 0)
    if neuron_count < 10 or neuron_count > 100000:
        raise ValueError(f"Invalid neuron_count: {neuron_count}")
    
    return True

# Validate configuration
validate_config(config)
```

### Configuration Templates

```python
# Common configuration templates

def get_high_performance_config():
    """Configuration optimized for high performance"""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 5000)
    config.set("brain.region_count", 4)
    config.set("brain.connection_probability", 0.2)
    config.set("simulation.timestep", 0.0005)
    config.set("visualization.enabled", True)
    config.set("log.level", "WARN")  # Reduce log noise
    return config

def get_research_config():
    """Configuration optimized for research"""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 10000)
    config.set("brain.region_count", 5)
    config.set("brain.connection_probability", 0.15)
    config.set("plasticity.learning_rate", 0.005)
    config.set("plasticity.stdp_ltp_weight", 0.02)
    config.set("plasticity.stdp_ltd_weight", 0.018)
    config.set("development.synaptogenesis_rate", 0.002)
    config.set("development.pruning_rate", 0.0002)
    config.set("neuromodulation.dopamine_baseline", 0.2)
    config.set("neuromodulation.curiosity_decay_rate", 0.03)
    config.set("neuromodulation.novelty_threshold", 0.7)
    config.set("reward.discount_factor", 0.95)
    return config

def get_reproduction_config():
    """Configuration optimized for production"""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.region_count", 1)
    config.set("brain.connection_probability", 0.05)
    config.set("simulation.max_steps", 1000)
    config.set("visualization.enabled", False)
    config.set("log.level", "INFO")
    config.set("checkpoint.enabled", True)
    config.set("checkpoint.path", "./checkpoints")
    return config
```

### Configuration Validation Examples

```python
# Comprehensive configuration validation
def validate_complete_config(config):
    errors = []
    
    # Check required parameters
    required = [
        ("brain.neuron_count", "Neuron count"),
        ("brain.region_count", "Region count"),
        ("brain.connection_probability", "Connection probability"),
        ("simulation.timestep", "Simulation timestep")
    ]
    
    for key, name in required:
        if not config.has(key):
            errors.append(f"Missing required parameter: {name}")
    
    # Check parameter values
    if config.has("brain.neuron_count"):
        neuron_count = config.getOr("brain.neuron_count", 0)
        if neuron_count < 10 or neuron_count > 100000:
            errors.append(f"Invalid neuron_count: {neuron_count}")
    
    if config.has("brain.region_count"):
        region_count = config.getOr("brain.region_count", 0)
        if region_count < 1 or region_count > 10:
            errors.append(f"Invalid region_count: {region_count}")
    
    if config.has("brain.connection_probability"):
        conn_prob = config.getOr("brain.connection_probability", 0.0)
        if conn_prob < 0.0 or conn_prob > 1.0:
            errors.append(f"Invalid connection_probability: {conn_prob}")
    
    if config.has("simulation.timestep"):
        timestep = config.getOr("simulation.timestep", 0.0)
        if timestep <= 0.0 or timestep >= 1.0:
            errors.append(f"Invalid simulation.timestep: {timestep}")
    
    # Check consistency between parameters
    if config.has("brain.neuron_count") and config.has("brain.region_count"):
        neuron_count = config.getOr("brain.neuron_count", 0)
        region_count = config.getOr("brain.region_count", 0)
        if region_count > 0 and neuron_count / region_count < 10:
            errors.append("Neuron count per region too low")
    
    if errors:
        raise ValueError("Configuration validation failed:\n" + "\n".join(errors))
    
    return True
```

## Configuration File Format

### JSON Configuration File

```json
{
    "version": "1.0",
    "description": "NLM Configuration for Research Experiment",
    
    "brain": {
        "neuron_count": 5000,
        "region_count": 3,
        "connection_probability": 0.15,
        "plasticity": {
            "learning_rate": 0.01,
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.018,
            "stdp_time_constant": 20.0
        },
        "development": {
            "synaptogenesis_rate": 0.001,
            "pruning_rate": 0.0001
        },
        "neuromodulation": {
            "dopamine_baseline": 0.1,
            "curiosity_decay_rate": 0.05,
            "novelty_threshold": 0.5
        }
    },
    
    "simulation": {
        "timestep": 0.001,
        "max_steps": 10000,
        "time_limit": 0.0,
        "checkpoint": {
            "enabled": true,
            "path": "./checkpoints",
            "max_checkpoints": 10,
            "compress": true
        }
    },
    
    "environment": {
        "name": "GridWorld",
        "width": 20,
        "height": 20,
        "vision_width": 8,
        "vision_height": 8
    },
    
    "reward": {
        "discount_factor": 0.99,
        "max_value": 1.0
    },
    
    "logging": {
        "level": "INFO",
        "to_file": true,
        "filename": "nlm_experiment.log",
        "async": true
    },
    
    "visualization": {
        "enabled": true,
        "update_rate": 30.0,
        "port": 8080,
        "host": "localhost"
    }
}
```

### Command Line Arguments

The configuration system also supports command line arguments:

```bash
# Basic usage
./nlm --config my_config.json

# Override specific parameters
./nlm --config base_config.json \
      --neuron_count 2000 \
      --region_count 4 \
      --connection_probability 0.2

# Command line argument format
--param_name value
--param_name=value

# Examples
./nlm --neuron_count 5000 --region_count 3 --connection_probability 0.15 \
      --plasticity_learning_rate 0.01 --stdp_ltp_weight 0.02 \
      --stdp_ltd_weight 0.018 --stdp_time_constant 20.0 \
      --development_synaptogenesis_rate 0.001 --development_pruning_rate 0.0001
```

## Configuration Validation Rules

### Custom Validation Functions

```python
def custom_validation_rules(config):
    """Custom validation rules specific to your experiment"""
    
    # Rule 1: Check memory requirements
    neuron_count = config.getOr("brain.neuron_count", 0)
    required_memory_mb = neuron_count * 100 / 1024 / 1024  # Rough estimate
    print(f"Estimated memory requirement: {required_memory_mb:.2f} MB")
    
    # Rule 2: Check connectivity patterns
    region_count = config.getOr("brain.region_count", 0)
    if region_count > 3:
        conn_prob = config.getOr("brain.connection_probability", 0.0)
        if conn_prob < 0.05:
            print("Warning: Low connection probability with many regions")
    
    # Rule 3: Check plasticity settings
    plasticity_rate = config.getOr("brain.plasticity.learning_rate", 0.0)
    if plasticity_rate < 0.001:
        print("Warning: Very low plasticity learning rate")
    
    # Rule 4: Check for balanced excitation/inhibition
    e_i_ratio = config.getOr("brain.e_i_ratio", 1.0)
    if e_i_ratio < 0.5 or e_i_ratio > 2.0:
        print("Warning: Unbalanced excitation/inhibition ratio")
    
    return True

# Apply custom validation
config = pynlm.createDefaultConfig()
custom_validation_rules(config)
```

## Configuration Export/Import

### Advanced Configuration Export

```python
import json

# Export configuration with full metadata
def export_config_with_metadata(config, filepath):
    export_data = {
        "version": "1.0",
        "export_timestamp": datetime.now().isoformat(),
        "nlm_version": pynlm.__version__,
        "config": {},
        "metadata": {
            "description": config.summary(),
            "validation_passed": True,
            "dependencies": ["numpy", "matplotlib"]  # Example dependencies
        }
    }
    
    # Flatten configuration for export
    for key in config.getKeys():
        export_data["config"][key] = config.get(key)
    
    with open(filepath, 'w') as f:
        json.dump(export_data, f, indent=2)

# Import configuration with validation
def import_config_with_validation(filepath):
    with open(filepath, 'r') as f:
        data = json.load(f)
    
    # Check version compatibility
    if data.get("version", "0.1") != "1.0":
        raise ValueError(f"Configuration version {data.get('version')} not compatible")
    
    # Import configuration
    config = pynlm.createDefaultConfig()
    for key, value in data.get("config", {}).items():
        config.set(key, value)
    
    # Validate imported configuration
    if not validate_config(config):
        raise ValueError("Imported configuration validation failed")
    
    return config
```

## Performance Considerations

### Large Configuration Files

```python
# Configuration for large-scale simulations
large_config = pynlm.createDefaultConfig()
large_config.set("brain.neuron_count", 100000)  # 100K neurons
large_config.set("brain.region_count", 10)     # 10 regions
large_config.set("simulation.max_steps", 1000000)  # 1M steps

# Efficient loading for large configurations
import time
start_time = time.time()
config = pynlm.createDefaultConfig()
# ... apply settings
end_time = time.time()
print(f"Configuration loaded in {end_time - start_time:.3f} seconds")
```

### Configuration Caching

```python
import pickle
import hashlib

class ConfigurationCache:
    def __init__(self, cache_dir="./config_cache"):
        self.cache_dir = cache_dir
        os.makedirs(cache_dir, exist_ok=True)
    
    def get_cache_key(self, config):
        """Generate cache key from configuration"""
        config_str = str(sorted(config.getKeys()))
        return hashlib.md5(config_str.encode()).hexdigest()
    
    def load_from_cache(self, config):
        """Load configuration from cache if available"""
        cache_key = self.get_cache_key(config)
        cache_file = os.path.join(self.cache_dir, f"{cache_key}.pkl")
        
        if os.path.exists(cache_file):
            with open(cache_file, 'rb') as f:
                return pickle.load(f)
        return None
    
    def save_to_cache(self, config):
        """Save configuration to cache"""
        cache_key = self.get_cache_key(config)
        cache_file = os.path.join(self.cache_dir, f"{cache_key}.pkl")
        
        with open(cache_file, 'wb') as f:
            pickle.dump(config, f)

# Usage example
cache = ConfigurationCache()
config = pynlm.createDefaultConfig()

# Try to load from cache
cached_config = cache.load_from_cache(config)
if cached_config is not None:
    config = cached_config
else:
    # Load new configuration
    config.set("brain.neuron_count", 1000)
    # Apply other settings...
    
    # Save to cache
    cache.save_to_cache(config)
```

## Configuration Best Practices

### 1. Configuration Management

```python
# Use configuration templates for common setups
CONFIG_TEMPLATES = {
    "small": {"neuron_count": 100, "region_count": 1},
    "medium": {"neuron_count": 1000, "region_count": 2},
    "large": {"neuron_count": 10000, "region_count": 5},
    "xlarge": {"neuron_count": 100000, "region_count": 10}
}

def get_config_by_size(size="medium"):
    """Get configuration by size category"""
    if size not in CONFIG_TEMPLATES:
        raise ValueError(f"Unknown config size: {size}")
    
    config = pynlm.createDefaultConfig()
    template = CONFIG_TEMPLATES[size]
    
    for key, value in template.items():
        config.set(f"brain.{key}", value)
    
    return config
```

### 2. Experiment Reproducibility

```python
# Ensure experiment reproducibility
import hashlib
import datetime

def create_reproducible_config(seed=42, experiment_name="test"):
    """Create reproducible configuration for experiments"""
    config = pynlm.createDefaultConfig()
    
    # Set seed for reproducibility
    config.set("random_seed", seed)
    
    # Add experiment metadata
    config.set("experiment.name", experiment_name)
    config.set("experiment.date", datetime.datetime.now().isoformat())
    config.set("experiment.version", pynlm.__version__)
    
    # Generate hash for version control
    config_hash = hashlib.md5(str(sorted(config.getKeys())).encode()).hexdigest()
    config.set("experiment.config_hash", config_hash)
    
    return config

# Use reproducible configuration
config = create_reproducible_config(experiment_name="plasticity_study")
```

### 3. Configuration Versioning

```python
# Configuration versioning for iterative experiments
CONFIG_VERSIONS = {
    "v1.0": {"description": "Initial configuration", "neuron_count": 1000},
    "v2.0": {"description": "Enhanced plasticity", "neuron_count": 1000, "plasticity.learning_rate": 0.005},
    "v3.0": {"description": "Memory integration", "neuron_count": 1000, "memory.enabled": True}
}

def get_config_version(version):
    """Get configuration by version"""
    if version not in CONFIG_VERSIONS:
        raise ValueError(f"Unknown configuration version: {version}")
    
    config = pynlm.createDefaultConfig()
    template = CONFIG_VERSIONS[version]
    
    for key, value in template.items():
        config.set(key, value)
    
    config.set("experiment.version", version)
    return config

# Version control for experiments
config_v2 = get_config_version("v2.0")
config_v3 = get_config_version("v3.0")
```

### 4. Configuration Validation

```python
# Comprehensive validation framework
class ConfigValidator:
    def __init__(self):
        self.rules = []
        self.warnings = []
    
    def add_rule(self, rule_func, description):
        """Add validation rule"""
        self.rules.append((rule_func, description))
    
    def validate(self, config):
        """Validate configuration"""
        results = {"errors": [], "warnings": []}
        
        for rule_func, description in self.rules:
            try:
                error, warning = rule_func(config)
                if error:
                    results["errors"].append(f"{description}: {error}")
                if warning:
                    results["warnings"].append(f"{description}: {warning}")
            except Exception as e:
                results["errors"].append(f"{description}: {str(e)}")
        
        return results
    
    def get_summary(self, validation_results):
        """Get validation summary"""
        if not validation_results["errors"] and not validation_results["warnings"]:
            return "Configuration is valid"
        
        summary = []
        if validation_results["errors"]:
            summary.append(f"Errors: {len(validation_results['errors'])}")
        if validation_results["warnings"]:
            summary.append(f"Warnings: {len(validation_results['warnings'])}")
        
        return ", ".join(summary)

# Create validator and add rules
validator = ConfigValidator()

# Add common validation rules
validator.add_rule(
    lambda c: (
        None if c.getOr("brain.neuron_count", 0) >= 10 else "Neuron count too low",
        "Neuron count is very low"
    ),
    "Neuron count validation"
)

validator.add_rule(
    lambda c: (
        None if c.getOr("brain.region_count", 0) <= 10 else "Region count too high",
        None
    ),
    "Region count validation"
)

# Validate configuration
validation_results = validator.validate(config)
print(validator.get_summary(validation_results))
if validation_results["errors"]:
    print("Errors:", "\n".join(validation_results["errors"]))
if validation_results["warnings"]:
    print("Warnings:", "\n".join(validation_results["warnings"]))
```

## Conclusion

The enhanced configuration system provides:

1. **Robust validation** - Comprehensive parameter validation and error checking
2. **Flexible API** - Easy-to-use Python interface with advanced features
3. **Template support** - Pre-defined configuration templates for common use cases
4. **Validation framework** - Extensible validation system for custom rules
5. **Best practices** - Guidance for effective configuration management
6. **Performance optimization** - Efficient loading and caching for large configurations

This system ensures that configurations are valid, reproducible, and optimized for different experimental scenarios while providing a clean and intuitive interface for users at all levels.