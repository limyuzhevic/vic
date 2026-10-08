# NLM Configuration Guide

This document provides detailed configuration options and examples for NLM, organized by system and parameter type.

---

## Overview

Configuration in NLM is managed through the `Config` class, which provides a key-value store for system parameters. Configuration can be loaded from JSON files, command line arguments, or set at runtime. This guide provides comprehensive documentation of all available configuration options.

---

## Configuration Structure

NLM configuration uses a hierarchical key-value system with nested parameters. Configuration keys use dot notation to represent nested structures:

```python
# Basic configuration example
config.set("brain.region_count", 4)
config.set("brain.neurons_per_region", 200)
config.set("memory.working_memory_size", 50)
config.set("visualization.enabled", True)
```

## Core Brain Configuration

### Brain Parameters

| Key | Type | Default | Description | Range |
|-----|------|---------|-------------|-------|
| `brain.region_count` | int | 1 | Number of neural regions | 1-10 |
| `brain.neurons_per_region` | int | 100 | Neurons per region | 10-1000 |
| `brain.initial_excitatory_ratio` | float | 0.8 | Ratio of excitatory neurons | 0.1-1.0 |
| `brain.initial_inhibitory_ratio` | float | 0.2 | Ratio of inhibitory neurons | 0.1-1.0 |
| `brain.synaptic_connection_probability` | float | 0.1 | Probability of random connections | 0.01-0.5 |
| `brain.synaptic_weight_mean` | float | 1.0 | Mean synaptic weight | 0.0-10.0 |
| `brain.synaptic_weight_variance` | float | 0.1 | Variance of synaptic weights | 0.0-1.0 |

### Developmental Configuration

| Key | Type | Default | Description | Range |
|-----|------|---------|-------------|-------|
| `development.enabled` | bool | True | Enable developmental processes | true/false |
| `development.start_critical_period` | int | 100 | Step when critical period starts | 0-1000 |
| `development.end_critical_period` | int | 400 | Step when critical period ends | 100-1000 |
| `development.maturation_rate` | float | 0.001 | Rate of developmental maturation | 0.0001-0.01 |
| `development.plasticity_modifier` | float | 1.0 | Modifier for plasticity during development | 0.1-5.0 |
| `development.structural_plasticity_enabled` | bool | True | Enable structural plasticity | true/false |

### Neuromodulation Configuration

| Key | Type | Default | Description | Range |
|-----|------|---------|-------------|-------|
| `neuromodulation.enable_dopamine` | bool | True | Enable dopamine system | true/false |
| `neuromodulation.dopamine_gain` | float | 1.0 | Dopamine influence on plasticity | 0.1-5.0 |
| `neuromodulation.enable_curiosity` | bool | True | Enable curiosity system | true/false |
| `neuromodulation.curiosity_threshold` | float | 0.5 | Threshold for curiosity activation | 0.0-1.0 |
| `neuromodulation.novelty_sensitivity` | float | 1.0 | Sensitivity to novel stimuli | 0.1-3.0 |
| `neuromodulation.enable_prediction_error` | bool | True | Enable prediction error system | true/false |
| `neuromodulation.prediction_error_gain` | float | 1.0 | Prediction error influence | 0.1-3.0 |

### Memory System Configuration

#### Working Memory

| Key | Type | Default | Description | Range |
|-----|------|---------|-------------|-------|
| `memory.working_memory_size` | int | 100 | Capacity of working memory | 10-1000 |
| `memory.working_memory_decay_rate` | float | 0.01 | Decay rate for working memory | 0.001-0.1 |
| `memory.working_memory_competition_strength` | float | 1.0 | Competition strength | 0.1-5.0 |

#### Episodic Memory

| Key | Type | Default | Description | Range |
|-----|------|---------|-------------|-------|
| `memory.episodic_capacity` | int | 1000 | Maximum episodic memories | 100-5000 |
| `memory.episodic_replay_frequency` | float | 0.1 | Replay frequency | 0.01-0.5 |
| `memory.episodic_consolidation_threshold` | float | 0.7 | Consolidation threshold | 0.1-1.0 |
| `memory.episodic_trace_decay` | float | 0.05 | Trace decay rate | 0.001-0.2 |

#### Associative Memory

| Key | Type | Default | Description | Range |
|-----|------|---------|-------------|-------|
| `memory.associative_pattern_size` | int | 50 | Pattern size for associations | 10-100 |
| `memory.associative_learning_rate` | float | 0.01 | Learning rate for associations | 0.001-0.1 |
| `memory.associative_forget_rate` | float | 0.001 | Forgetting rate | 0.0001-0.01 |

### Prediction System Configuration

| Key | Type | Default | Description | Range |
|-----|------|---------|-------------|-------|
| `prediction.prediction_horizon` | int | 20 | Lookahead window | 5-100 |
| `prediction.prediction_update_rate` | float | 0.5 | Prediction update frequency | 0.1-1.0 |
| `prediction.confidence_threshold` | float | 0.5 | Confidence threshold | 0.1-1.0 |
| `prediction.error_sensitivity` | float | 1.0 | Sensitivity to prediction error | 0.1-5.0 |

### Cognitive System Configuration

#### Neural Planner

| Key | Type | Default | Description | Range |
|-----|------|---------|-------------|-------|
| `cognition.planner_action_window` | int | 10 | Action planning horizon | 1-50 |
| `cognition.planner_evaluation_strength` | float | 1.0 | Planning evaluation strength | 0.1-5.0 |

#### Concept Formation

| Key | Type | Default | Description | Range |
|-----|------|---------|-------------|-------|
| `cognition.concept_learning_rate` | float | 0.01 | Concept learning rate | 0.001-0.1 |
| `cognition.concept_threshold` | float | 0.6 | Concept formation threshold | 0.1-1.0 |
| `cognition.concept_similarity` | float | 0.7 | Similarity threshold for concepts | 0.1-1.0 |

#### Attentional Selection

| Key | Type | Default | Description | Range |
|-----|------|---------|-------------|-------|
| `cognition.attention_focus_strength` | float | 1.0 | Attention focus strength | 0.1-5.0 |
| `cognition.attention_switch_rate` | float | 0.1 | Attention switching rate | 0.01-0.5 |

### Plasticity System Configuration

| Key | Type | Default | Description | Range |
|-----|------|---------|-------------|-------|
| `plasticity.stdp_learning_rate` | float | 0.01 | STDP learning rate | 0.001-0.1 |
| `plasticity.stdp_time_constant` | float | 20.0 | STDP time constant | 5.0-100.0 |
| `plasticity.hebbian_learning_rate` | float | 0.005 | Hebbian learning rate | 0.0001-0.05 |
| `plasticity.structural_plasticity_rate` | float | 0.001 | Structural plasticity rate | 0.0001-0.01 |
| `plasticity.weight_normalization` | bool | True | Enable weight normalization | true/false |

### World Configuration

| Key | Type | Default | Description | Range |
|-----|------|---------|-------------|-------|
| `world.width` | int | 20 | World width | 10-100 |
| `world.height` | int | 20 | World height | 10-100 |
| `world.vision_width` | int | 8 | Vision width | 4-20 |
| `world.vision_height` | int | 8 | Vision height | 4-20 |
| `world.max_energy` | float | 100.0 | Maximum agent energy | 10.0-1000.0 |
| `world.energy_decay_rate` | float | 0.01 | Energy decay rate | 0.001-0.1 |
| `world.object_spawn_rate` | float | 0.1 | Object spawn rate | 0.01-0.5 |

### Visualization Configuration

| Key | Type | Default | Description | Range |
|-----|------|---------|-------------|-------|
| `visualization.enabled` | bool | False | Enable visualization system | true/false |
| `visualization.update_interval` | float | 0.5 | Visualization update interval (seconds) | 0.1-5.0 |
| `visualization.network_layout` | str | "force-directed" | Network layout algorithm | "force-directed", "circular", "radial" |
| `visualization.performance_monitoring` | bool | True | Enable performance monitoring | true/false |
| `visualization.memory_visualization` | bool | True | Enable memory visualization | true/false |
| `visualization.real_time` | bool | False | Enable real-time updates | true/false |

### Performance Configuration

| Key | Type | Default | Description | Range |
|-----|------|---------|-------------|-------|
| `performance.checkpoint_interval` | int | 1000 | Checkpoint save interval | 100-10000 |
| `performance.max_memory_usage` | int | 512 | Maximum memory usage (MB) | 64-4096 |
| `performance.enable_profiling` | bool | False | Enable performance profiling | true/false |
| `performance.parallel_processing` | bool | True | Enable parallel processing | true/false |

## Configuration Loading Methods

### File-Based Configuration

Load configuration from a JSON file:

```python
config = pynlm.createDefaultConfig()
success = config.loadFromFile("my_config.json")
if success:
    print("Configuration loaded successfully")
else:
    print("Failed to load configuration")
```

**JSON File Format:**
```json
{
  "brain": {
    "region_count": 4,
    "neurons_per_region": 200
  },
  "memory": {
    "working_memory_size": 50,
    "episodic_capacity": 1000
  },
  "neuromodulation": {
    "enable_dopamine": true,
    "dopamine_gain": 2.0,
    "enable_curiosity": true,
    "curiosity_threshold": 0.3
  }
}
```

### Command Line Configuration

Load configuration from command line arguments:

```python
config = pynlm.createDefaultConfig()
success = config.loadFromArgs(len(sys.argv), sys.argv)
```

**Command Line Format:**
```bash
./my_program --brain.region_count 4 --memory.working_memory_size 50 \
           --neuromodulation.dopamine_gain 2.0 \
           --visualization.enabled --performance.enable_profiling
```

### Runtime Configuration

Set configuration parameters at runtime:

```python
config = pynlm.createDefaultConfig()

# Set basic parameters
config.set("brain.region_count", 6)
config.set("brain.neurons_per_region", 300)

# Set neuromodulation
config.set("neuromodulation.enable_dopamine", True)
config.set("neuromodulation.dopamine_gain", 1.5)
config.set("neuromodulation.enable_curiosity", True)
config.set("neuromodulation.curiosity_threshold", 0.2)

# Set memory systems
config.set("memory.working_memory_size", 100)
config.set("memory.episodic_capacity", 2000)

# Set visualization
config.set("visualization.enabled", True)
config.set("visualization.real_time", True)
```

## Configuration Best Practices

### 1. Configuration Organization

Organize configuration keys logically:

```python
# Brain configuration
brain_config = {
    "brain.region_count": 4,
    "brain.neurons_per_region": 200,
    "brain.initial_excitatory_ratio": 0.8
}

# Memory configuration
memory_config = {
    "memory.working_memory_size": 100,
    "memory.episodic_capacity": 1000,
    "memory.associative_pattern_size": 50
}

# Neuromodulation configuration
neuromod_config = {
    "neuromodulation.dopamine_gain": 1.5,
    "neuromodulation.curiosity_threshold": 0.3
}
```

### 2. Configuration Validation

Validate configuration parameters before use:

```python
def validate_config(config):
    # Validate brain parameters
    if config.has("brain.region_count"):
        regions = config.getOr("brain.region_count", 1)
        if regions < 1 or regions > 10:
            raise ValueError("brain.region_count must be between 1 and 10")
    
    # Validate memory parameters
    if config.has("memory.working_memory_size"):
        mem_size = config.getOr("memory.working_memory_size", 100)
        if mem_size < 10 or mem_size > 1000:
            raise ValueError("memory.working_memory_size must be between 10 and 1000")
    
    # Validate neuromodulation parameters
    if config.has("neuromodulation.dopamine_gain"):
        dopamine_gain = config.getOr("neuromodulation.dopamine_gain", 1.0)
        if dopamine_gain < 0.1 or dopamine_gain > 5.0:
            raise ValueError("neuromodulation.dopamine_gain must be between 0.1 and 5.0")
    
    return True
```

### 3. Configuration Templates

Create configuration templates for common scenarios:

```python
def create_research_config():
    """Create configuration for research scenarios."""
    config = pynlm.createDefaultConfig()
    
    # Research configuration
    config.set("brain.region_count", 8)
    config.set("brain.neurons_per_region", 500)
    config.set("memory.working_memory_size", 200)
    config.set("memory.episodic_capacity", 5000)
    config.set("neuromodulation.enable_dopamine", True)
    config.set("neuromodulation.dopamine_gain", 2.0)
    config.set("neuromodulation.enable_curiosity", True)
    config.set("neuromodulation.curiosity_threshold", 0.2)
    config.set("neuromodulation.enable_prediction_error", True)
    config.set("neuromodulation.prediction_error_gain", 1.5)
    config.set("development.enabled", True)
    config.set("development.maturation_rate", 0.001)
    config.set("plasticity.stdp_learning_rate", 0.02)
    config.set("plasticity.hebbian_learning_rate", 0.01)
    config.set("visualization.enabled", True)
    config.set("visualization.real_time", True)
    config.set("performance.enable_profiling", True)
    
    return config

def create_production_config():
    """Create configuration for production scenarios."""
    config = pynlm.createDefaultConfig()
    
    # Production configuration
    config.set("brain.region_count", 3)
    config.set("brain.neurons_per_region", 100)
    config.set("memory.working_memory_size", 50)
    config.set("memory.episodic_capacity", 500)
    config.set("neuromodulation.enable_dopamine", True)
    config.set("neuromodulation.dopamine_gain", 1.0)
    config.set("neuromodulation.enable_curiosity", False)
    config.set("neuromodulation.enable_prediction_error", False)
    config.set("development.enabled", False)
    config.set("visualization.enabled", False)
    config.set("performance.enable_profiling", False)
    config.set("performance.max_memory_usage", 256)
    
    return config
```

### 4. Configuration Monitoring

Monitor configuration changes during runtime:

```python
class ConfigMonitor:
    def __init__(self, config):
        self.config = config
        self.change_callbacks = []
    
    def add_change_callback(self, callback):
        """Add callback for configuration changes."""
        self.change_callbacks.append(callback)
    
    def set_with_monitoring(self, key, value):
        """Set configuration key with change notification."""
        old_value = None
        if self.config.has(key):
            old_value = self.config.get(key)
        
        self.config.set(key, value)
        
        # Notify callbacks
        for callback in self.change_callbacks:
            callback(key, old_value, value)
    
    def get_config_summary(self):
        """Get configuration summary for logging."""
        summary = self.config.summary()
        print(f"Configuration: {summary}")
        return summary
```

## Configuration Examples

### Example 1: Basic Brain

```python
import pynlm

# Basic brain configuration
config = pynlm.createDefaultConfig()
config.set("brain.region_count", 2)
config.set("brain.neurons_per_region", 50)
config.set("memory.working_memory_size", 20)
config.set("visualization.enabled", False)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Example 2: Advanced Research Setup

```python
import pynlm
import json

# Advanced research configuration
config = pynlm.createDefaultConfig()

# Enable all systems
config.set("development.enabled", True)
config.set("neuromodulation.enable_dopamine", True)
config.set("neuromodulation.dopamine_gain", 2.5)
config.set("neuromodulation.enable_curiosity", True)
config.set("neuromodulation.curiosity_threshold", 0.2)
config.set("neuromodulation.enable_prediction_error", True)
config.set("neuromodulation.prediction_error_gain", 1.8)

# Configure brain
config.set("brain.region_count", 6)
config.set("brain.neurons_per_region", 300)
config.set("brain.initial_excitatory_ratio", 0.7)

# Configure memory systems
config.set("memory.working_memory_size", 200)
config.set("memory.episodic_capacity", 3000)
config.set("memory.associative_pattern_size", 75)

# Configure plasticity
config.set("plasticity.stdp_learning_rate", 0.02)
config.set("plasticity.hebbian_learning_rate", 0.01)
config.set("plasticity.structural_plasticity_rate", 0.005)

# Configure visualization for research
config.set("visualization.enabled", True)
config.set("visualization.real_time", True)
config.set("visualization.performance_monitoring", True)
config.set("performance.enable_profiling", True)

# Initialize brain
brain = pynlm.createBrain(config)
brain.initialize()
```

### Example 3: High-Performance Configuration

```python
import pynlm

# High-performance configuration
config = pynlm.createDefaultConfig()

# Optimize for performance
config.set("brain.region_count", 4)
config.set("brain.neurons_per_region", 200)
config.set("performance.max_memory_usage", 1024)
config.set("performance.parallel_processing", True)
config.set("performance.checkpoint_interval", 5000)

# Enable essential systems only
config.set("development.enabled", True)
config.set("neuromodulation.enable_dopamine", True)
config.set("neuromodulation.enable_curiosity", False)  # Disable for performance
config.set("neuromodulation.enable_prediction_error", False)  # Disable for performance
config.set("visualization.enabled", False)  # Disable for performance

brain = pynlm.createBrain(config)
brain.initialize()
```

## Configuration File Format Reference

### JSON Configuration File

```json
{
  "brain": {
    "region_count": 4,
    "neurons_per_region": 200,
    "initial_excitatory_ratio": 0.8
  },
  "memory": {
    "working_memory_size": 100,
    "episodic_capacity": 1000,
    "associative_pattern_size": 50
  },
  "development": {
    "enabled": true,
    "maturation_rate": 0.001,
    "critical_period_start": 100,
    "critical_period_end": 400
  },
  "neuromodulation": {
    "enable_dopamine": true,
    "dopamine_gain": 1.5,
    "enable_curiosity": true,
    "curiosity_threshold": 0.3,
    "enable_prediction_error": true,
    "prediction_error_gain": 1.2
  },
  "plasticity": {
    "stdp_learning_rate": 0.01,
    "hebbian_learning_rate": 0.005,
    "structural_plasticity_rate": 0.001
  },
  "visualization": {
    "enabled": true,
    "update_interval": 0.5,
    "real_time": false
  },
  "performance": {
    "max_memory_usage": 512,
    "enable_profiling": false,
    "parallel_processing": true
  }
}
```

### Command Line Arguments

```bash
# Basic usage
./program --brain.region_count 4 --memory.working_memory_size 100

# With visualization
./program --visualization.enabled --visualization.update_interval 1.0

# With development
./program --development.enabled --development.maturation_rate 0.001

# Full research setup
./program --brain.region_count 6 --brain.neurons_per_region 300 \
         --memory.working_memory_size 200 --memory.episodic_capacity 3000 \
         --neuromodulation.enable_dopamine --neuromodulation.dopamine_gain 2.0 \
         --neuromodulation.enable_curiosity --neuromodulation.curiosity_threshold 0.2 \
         --neuromodulation.enable_prediction_error --neuromodulation.prediction_error_gain 1.5 \
         --development.enabled --development.maturation_rate 0.001 \
         --plasticity.stdp_learning_rate 0.02 --plasticity.hebbian_learning_rate 0.01 \
         --visualization.enabled --visualization.real_time \
         --performance.enable_profiling --performance.max_memory_usage 1024
```

## Configuration Troubleshooting

### Common Issues and Solutions

**Issue 1: Configuration not loading from file**

```python
try:
    config = pynlm.createDefaultConfig()
    if not config.loadFromFile("config.json"):
        print("Warning: Could not load config.json, using defaults")
except Exception as e:
    print(f"Error loading configuration: {e}")
    config = pynlm.createDefaultConfig()
```

**Issue 2: Invalid configuration values**

```python
def safe_config_set(config, key, value, validate_func=None):
    """Safely set configuration with validation."""
    try:
        if validate_func and not validate_func(value):
            raise ValueError(f"Invalid value for {key}: {value}")
        
        config.set(key, value)
        return True
    except Exception as e:
        print(f"Failed to set {key}: {e}")
        return False

# Usage
validate_neurons = lambda x: 10 <= x <= 1000
safe_config_set(config, "brain.neurons_per_region", 50, validate_neurons)
```

**Issue 3: Configuration performance issues**

```python
# Monitor configuration usage
import time

config = pynlm.createDefaultConfig()

# Time configuration operations
start_time = time.time()
config.set("brain.region_count", 4)
set_time = time.time() - start_time
print(f"Configuration set time: {set_time:.4f} seconds")

start_time = time.time()
value = config.getOr("brain.region_count", 1)
get_time = time.time() - start_time
print(f"Configuration get time: {get_time:.4f} seconds")
```

### Validation Functions

```python
def validate_brain_config(config):
    """Validate brain-related configuration."""
    errors = []
    
    if config.has("brain.region_count"):
        regions = config.getOr("brain.region_count", 1)
        if regions < 1 or regions > 10:
            errors.append(f"brain.region_count must be 1-10, got {regions}")
    
    if config.has("brain.neurons_per_region"):
        neurons = config.getOr("brain.neurons_per_region", 100)
        if neurons < 10 or neurons > 1000:
            errors.append(f"brain.neurons_per_region must be 10-1000, got {neurons}")
    
    return errors

def validate_memory_config(config):
    """Validate memory-related configuration."""
    errors = []
    
    if config.has("memory.working_memory_size"):
        mem_size = config.getOr("memory.working_memory_size", 100)
        if mem_size < 10 or mem_size > 1000:
            errors.append(f"memory.working_memory_size must be 10-1000, got {mem_size}")
    
    return errors

def validate_neuromod_config(config):
    """Validate neuromodulation configuration."""
    errors = []
    
    if config.has("neuromodulation.dopamine_gain"):
        gain = config.getOr("neuromodulation.dopamine_gain", 1.0)
        if gain < 0.1 or gain > 5.0:
            errors.append(f"neuromodulation.dopamine_gain must be 0.1-5.0, got {gain}")
    
    if config.has("neuromodulation.curiosity_threshold"):
        threshold = config.getOr("neuromodulation.curiosity_threshold", 0.5)
        if threshold < 0.0 or threshold > 1.0:
            errors.append(f"neuromodulation.curiosity_threshold must be 0.0-1.0, got {threshold}")
    
    return errors

def validate_all_config(config):
    """Validate all configuration sections."""
    all_errors = []
    all_errors.extend(validate_brain_config(config))
    all_errors.extend(validate_memory_config(config))
    all_errors.extend(validate_neuromod_config(config))
    
    if all_errors:
        print("Configuration validation errors:")
        for error in all_errors:
            print(f"  - {error}")
        return False
    return True
```

## Configuration Export/Import

### Export Configuration

```python
def export_config(config, filename):
    """Export configuration to JSON file."""
    try:
        config.saveToFile(filename)
        print(f"Configuration exported to {filename}")
        return True
    except Exception as e:
        print(f"Failed to export configuration: {e}")
        return False

# Usage
export_config(config, "my_configuration.json")
```

### Import Configuration

```python
def import_config(filename, target_config=None):
    """Import configuration from JSON file."""
    if target_config is None:
        target_config = pynlm.createDefaultConfig()
    
    try:
        if target_config.loadFromFile(filename):
            print(f"Configuration imported from {filename}")
            return target_config
        else:
            print(f"Failed to import configuration from {filename}")
            return None
    except Exception as e:
        print(f"Error importing configuration: {e}")
        return None

# Usage
config = import_config("my_configuration.json")
```

---

## Summary

This Configuration Guide provides comprehensive documentation of NLM's configuration system, including:

1. **Core Parameters**: Brain, memory, neuromodulation, and development settings
2. **Configuration Loading**: File-based, command line, and runtime configuration
3. **Best Practices**: Organization, validation, and optimization techniques
4. **Configuration Examples**: Templates for different use cases
5. **Troubleshooting**: Common issues and solutions
6. **Export/Import**: Configuration file management

Use this guide as a reference when configuring NLM for your specific research or application needs. The configuration system provides flexibility to customize NLM's behavior while maintaining a consistent interface across different usage scenarios.