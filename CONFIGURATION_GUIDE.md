# Configuration Examples and Best Practices

This document provides practical examples and best practices for working with NLM configuration files.

## Configuration File Formats

### Simple Key=Value Format

The simplest configuration format uses `key = value` pairs:

```
# Basic NLM Configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15

# STDP parameters ( Spike-Timing-Dependent Plasticity )
stdp_ltp_weight = 0.02    # Long-Term Potentiation weight
stdp_ltd_weight = 0.015   # Long-Term Depression weight
stdp_tau = 20.0           # STDP time constant (ms)

# Structural plasticity parameters
synaptogenesis_rate = 0.0001    # Rate of new synapse formation
pruning_rate = 0.00001          # Rate of synapse elimination

# Development parameters
development_enabled = true
critical_period_start = 100
critical_period_end = 1000

# Neuromodulation parameters
dopamine_enabled = true
curiosity_enabled = true
novelty_enabled = true
```

### JSON Configuration Format

For more complex configurations, use JSON:

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.15,
    
    "stdp": {
        "ltp_weight": 0.02,
        "ltd_weight": 0.015,
        "tau": 20.0,
        "pre_trace_decay": 0.02,
        "post_trace_decay": 0.02
    },
    
    "structural_plasticity": {
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001,
        "max_synapses_per_neuron": 50
    },
    
    "development": {
        "enabled": true,
        "critical_period_start": 100,
        "critical_period_end": 1000,
        "plasticity_decay_rate": 0.001
    },
    
    "neuromodulation": {
        "dopamine": {
            "enabled": true,
            "strength": 1.0,
            "prediction_error_weight": 0.5
        },
        "curiosity": {
            "enabled": true,
            "exploration_strength": 0.3
        },
        "novelty": {
            "enabled": true,
            "detection_threshold": 0.5
        }
    },
    
    "memory": {
        "working_memory_capacity": 10,
        "episodic_memory_capacity": 1000,
        "consolidation_interval": 100
    },
    
    "environment": {
        "width": 640,
        "height": 480,
        "vision_width": 128,
        "vision_height": 128,
        "max_energy": 100.0,
        "energy_decay_rate": 0.01
    }
}
```

### Advanced Configuration Templates

#### Template 1: Small Educational System

```
# Quick testing configuration
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 100
region_count = 1
connection_probability = 0.2
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.008
stdp_tau = 15.0
synaptogenesis_rate = 0.001
pruning_rate = 0.0001
```

#### Template 2: Large Research System

```json
{
    "random_seed": 999,
    "simulation_timestep": 0.0005,
    "neuron_count": 5000,
    "region_count": 3,
    "connection_probability": 0.1,
    
    "stdp": {
        "ltp_weight": 0.025,
        "ltd_weight": 0.02,
        "tau": 25.0,
        "pre_trace_decay": 0.025,
        "post_trace_decay": 0.025
    },
    
    "structural_plasticity": {
        "synaptogenesis_rate": 0.0005,
        "pruning_rate": 0.00005,
        "max_synapses_per_neuron": 100
    },
    
    "development": {
        "enabled": true,
        "critical_period_start": 50,
        "critical_period_end": 500,
        "plasticity_decay_rate": 0.0005
    },
    
    "neuromodulation": {
        "dopamine": {
            "enabled": true,
            "strength": 1.5,
            "prediction_error_weight": 0.7
        },
        "curiosity": {
            "enabled": true,
            "exploration_strength": 0.5
        },
        "novelty": {
            "enabled": true,
            "detection_threshold": 0.3
        }
    }
}
```

#### Template 3: Agent Training Configuration

```
# Configuration for training autonomous agents
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.12
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Development for learning phases
synaptogenesis_rate = 0.001
pruning_rate = 0.0001
development_enabled = true
critical_period_start = 100

# Strong neuromodulation for learning
 dopamine_enabled = true
curiosity_enabled = true  # Exploration
novelty_enabled = true    # Novelty detection

# Environment for training
environment_width = 800
environment_height = 600
vision_width = 160
vision_height = 120
max_energy = 200.0
energy_decay_rate = 0.005
```

## Command Line Configuration Examples

### Using Configuration Files

```bash
# Use simple key=value format
./nlm --config=production.cfg

# Use JSON format
./nlm --config=advanced_config.json

# Override specific parameters
./nlm --config=base.cfg --neuron_count=2000 --region_count=3
```

### Runtime Configuration

```bash
# All configuration can be overridden via command line
./nlm \
    --random_seed=12345 \
    --simulation_timestep=0.002 \
    --neuron_count=1500 \
    --region_count=2 \
    --connection_probability=0.2 \
    --stdp_ltp_weight=0.03 \
    --stdp_ltd_weight=0.025 \
    --stdp_tau=30.0
```

### Help and Information

```bash
# Show help information
./nlm --help

# Show configuration summary (from file)
./nlm --config=my_config.cfg --show_config

# Show default configuration
./nlm --show_defaults
```

## Best Practices

### 1. Configuration Organization

- **Environment-specific configs**: Create separate configs for development, testing, and production
- **Template + override**: Use a base template and override specific parameters
- **Validation**: Always validate configuration before use
- **Comments**: Use comments to document parameter meanings

### 2. Performance Optimization

- **For testing**: Use smaller `neuron_count` and `region_count`
- **For production**: Use larger networks with optimized parameters
- **For research**: Enable development and neuromodulation for learning
- **For real-time**: Use smaller `simulation_timestep` for accuracy

### 3. Parameter Tuning

- **STDP parameters**: Adjust `stdp_ltp_weight`, `stdp_ltd_weight`, and `stdp_tau` for different learning regimes
- **Development parameters**: Set `critical_period_start` and `critical_period_end` for developmental stages
- **Neuromodulation**: Enable/disable based on whether you want exploration vs. exploitation
- **Memory parameters**: Balance `working_memory_capacity` vs. `episodic_memory_capacity` based on task complexity

### 4. Configuration Validation

Always validate your configuration before running simulations:

```python
import pynlm

config = pynlm.createDefaultConfig()
config.loadFromFile("my_config.json")

try:
    config.validate()
    print("Configuration is valid")
    
    # Or use non-strict validation
    config.validate(strict=False)
    print("Configuration passed with warnings")
    
except Exception as e:
    print(f"Configuration error: {e}")
    # Fix configuration and try again
```

### 5. Configuration Templates

Create reusable configuration templates:

#### Template Manager (Python)

```python
import pynlm
import json
import os

class ConfigTemplate:
    def __init__(self, name):
        self.name = name
        self.parameters = {}
    
    def add_parameter(self, key, value, description=""):
        self.parameters[key] = {"value": value, "description": description}
    
    def save(self, directory="configs/"):
        os.makedirs(directory, exist_ok=True)
        filepath = os.path.join(directory, f"{self.name}.cfg")
        with open(filepath, 'w') as f:
            f.write(f"# Template: {self.name}\n")
            for key, param in self.parameters.items():
                f.write(f"# {param['description']}\n")
                f.write(f"{key} = {param['value']}\n")
            f.write("\n")
        return filepath
    
    def create_config(self, overrides=None):
        config = pynlm.createDefaultConfig()
        for key, param in self.parameters.items():
            config.set(key, param['value'])
        
        if overrides:
            for key, value in overrides.items():
                config.set(key, value)
        
        return config

# Create common templates
templates = {
    "basic": ConfigTemplate("basic"),
    "research": ConfigTemplate("research"),
    "agent_training": ConfigTemplate("agent_training")
}

# Setup basic template
templates["basic"].add_parameter("random_seed", 42, "Random seed for reproducible results")
templates["basic"].add_parameter("neuron_count", 500, "Number of neurons in the network")
templates["basic"].add_parameter("region_count", 1, "Number of brain regions")
templates["basic"].add_parameter("simulation_timestep", 0.001, "Simulation time step in seconds")

# Save templates
for name, template in templates.items():
    template.save()
```

## Configuration Debugging

### Common Issues and Solutions

#### Issue 1: Invalid Configuration Values

**Problem**: Simulation fails due to invalid configuration values.

**Solution**: Use configuration validation:

```bash
# Show configuration with validation
./nlm --config=problematic.cfg --validate
```

```python
# In Python with validation
config = pynlm.createDefaultConfig()
config.loadFromFile("problematic.cfg")

try:
    config.validate()
except Exception as e:
    print(f"Validation failed: {e}")
    # Fix the configuration
    config.set("neuron_count", 100)  # Example fix
    config.validate()  # Try again
```

#### Issue 2: Configuration File Not Found

**Problem**: Configuration file specified but not found.

**Solution**: Check file paths and use absolute paths:

```bash
# Use absolute path
./nlm --config=/home/user/configs/production.cfg

# Create default config if missing
./nlm --config=nonexistent.cfg --create_default
```

#### Issue 3: Parameter Conflicts

**Problem**: Configuration parameters conflict with each other.

**Solution**: Use configuration templates and override only specific parameters:

```python
# Load base template
base_config = pynlm.createDefaultConfig()
base_config.loadFromFile("configs/research_template.cfg")

# Override only what you need
base_config.set("neuron_count", 2000)
base_config.set("random_seed", 123)

# Validate before use
base_config.validate()
```

### Configuration Comparison Tool

Compare two configuration files:

```python
import pynlm
import json

def compare_configs(file1, file2):
    config1 = pynlm.createDefaultConfig()
    config2 = pynlm.createDefaultConfig()
    
    config1.loadFromFile(file1)
    config2.loadFromFile(file2)
    
    keys1 = config1.getKeys()
    keys2 = config2.getKeys()
    
    all_keys = set(keys1) | set(keys2)
    
    print(f"Configuration Comparison: {file1} vs {file2}")
    print("=" * 50)
    
    for key in sorted(all_keys):
        val1 = config1.get(key)
        val2 = config2.get(key)
        
        if val1 != val2:
            print(f"DIFFERENT: {key}")
            print(f"  {file1}: {val1}")
            print(f"  {file2}: {val2}")
        else:
            print(f"SAME: {key} = {val1}")

# Usage
compare_configs("configs/baseline.cfg", "configs/optimized.cfg")
```

## Configuration Migration Guide

### From Simple Format to JSON

If you're migrating from simple `key=value` files to JSON:

1. **Convert existing configs**:

```python
import configparser

def convert_to_json(simple_file, json_file):
    config = configparser.ConfigParser()
    config.read(simple_file)
    
    json_data = {}
    for section in config.sections():
        for key, value in config.items(section):
            # Convert string values to appropriate types
            try:
                if value.lower() in ('true', 'false'):
                    json_data[key] = value.lower() == 'true'
                elif '.' in value:
                    json_data[key] = float(value)
                elif value.isdigit():
                    json_data[key] = int(value)
                else:
                    json_data[key] = value
            except ValueError:
                json_data[key] = value
    
    with open(json_file, 'w') as f:
        json.dump(json_data, f, indent=4)

# Usage
convert_to_json("old_config.cfg", "new_config.json")
```

2. **Validation and testing**:

```bash
# Test both formats produce same results
./nlm --config=old_config.cfg --output=old_results.json
./nlm --config=new_config.json --output=new_results.json

# Compare results
python compare_results.py old_results.json new_results.json
```

### From JSON to Optimized Simple Format

For production use, convert JSON to optimized simple format:

```python
import json

def optimize_json_to_simple(json_file, simple_file):
    with open(json_file, 'r') as f:
        config = json.load(f)
    
    # Only include non-default values
    default_values = {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.15,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "stdp_tau": 20.0,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001,
        "development_enabled": true,
        "dopamine_enabled": true,
        "curiosity_enabled": true,
        "novelty_enabled": true
    }
    
    with open(simple_file, 'w') as f:
        for key, value in sorted(config.items()):
            if key in default_values:
                if value != default_values[key]:
                    f.write(f"{key} = {value}\n")
            else:
                f.write(f"{key} = {value}\n")

# Usage
optimize_json_to_simple("full_config.json", "optimized_config.cfg")
```

## Configuration Best Practices Checklist

- [ ] Always validate configuration before running simulations
- [ ] Use absolute paths for configuration files in production
- [ ] Document all custom parameters with comments
- [ ] Test configurations with small networks first
- [ ] Use different configurations for development, testing, and production
- [ ] Backup important configuration files
- [ ] Keep configuration management in version control
- [ ] Use environment variables for sensitive configuration values
- [ ] Validate configuration ranges (e.g., probabilities 0-1, positive numbers)
- [ ] Test configuration reload capabilities
- [ ] Verify configuration serialization/deserialization

This document provides comprehensive guidance for working with NLM configurations, from basic usage to advanced optimization techniques.
