# NLM Configuration Enhancement
# Version 2.0

## Overview
This document outlines the enhanced configuration system for NLM (Neural Learning Machine), including new configuration options, improved loading mechanisms, and better parameter management for advanced users.

## New Configuration Categories

### 1. Advanced Neural Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons in the brain |
| `brain.region_count` | int | 2 | Number of neural regions |
| `brain.population_distribution` | string | "balanced" | Distribution of neuron types: "balanced", "excitatory_heavy", "inhibitory_heavy" |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |

### 2. Enhanced Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity |
| `plasticity.stdp.ltp_weight` | float | 0.02 | Long-term potentiation weight |
| `plasticity.stdp.ltd_weight` | float | 0.015 | Long-term depression weight |
| `plasticity.stdp.tau` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.max_weight` | float | 1.0 | Maximum synaptic weight |
| `plasticity.stdp.min_weight` | float | -1.0 | Minimum synaptic weight |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.001 | Hebbian learning rate |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity (synaptogenesis/pruning) |
| `plasticity.structural.synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `plasticity.structural.pruning_rate` | float | 0.00001 | Rate of synapse elimination |

### 3. Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.enable` | bool | true | Enable dopamine modulation |
| `neuromod.dopamine.baseline` | float | 0.1 | Baseline dopamine level |
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale factor |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.threshold` | float | 0.5 | Curiosity threshold for exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.novelty.sensitivity` | float | 0.1 | Novelty detection sensitivity |
| `neuromod.prediction_error.enable` | bool | true | Enable prediction error computation |

### 4. Memory System Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working.enable` | bool | true | Enable working memory |
| `memory.working.capacity` | int | 100 | Working memory capacity |
| `memory.episodic.enable` | bool | true | Enable episodic memory |
| `memory.episodic.capacity` | int | 50 | Episodic memory capacity |
| `memory.associative.enable` | bool | true | Enable associative memory |
| `memory.associative.association_strength` | float | 0.8 | Association strength decay rate |

### 5. Prediction and Cognition Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `cognition.planner.enable` | bool | true | Enable neural planner |
| `cognition.planner.planning_horizon` | int | 10 | Planning horizon steps |
| `cognition.concept.enable` | bool | true | Enable concept formation |
| `cognition.attention.enable` | bool | true | Enable attentional selection |

### 6. Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.enable` | bool | true | Enable developmental processes |
| `development.initial_stage` | string | "Initial" | Initial developmental stage |
| `development.stage_duration` | int | 1000 | Duration of each developmental stage (steps) |
| `development.plasticity_modifier` | float | 1.0 | Developmental modifier for plasticity rates |

### 7. Environment Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `environment.name` | string | "GridWorld" | Environment type |
| `environment.width` | int | 10 | Environment width |
| `environment.height` | int | 10 | Environment height |
| `environment.vision_width` | int | 8 | Vision sensor width |
| `environment.vision_height` | int | 8 | Vision sensor height |

### 8. Advanced Configuration Options

#### Performance Optimization
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `performance.simd.enable` | bool | true | Enable SIMD optimizations |
| `performance.parallel.enable` | bool | true | Enable parallel processing |
| `performance.checkpointing.enable` | bool | true | Enable automatic checkpointing |
| `performance.checkpoint_interval` | int | 1000 | Checkpoint interval (steps) |

#### Simulation Control
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation.max_steps` | int | 10000 | Maximum simulation steps |
| `simulation.time_limit` | float | 0.0 | Simulation time limit (0 = unlimited) |
| `simulation.random_seed` | int64_t | 42 | Random seed for reproducibility |

#### Debugging and Logging
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `debug.log_level` | string | "INFO" | Logging level: "DEBUG", "INFO", "WARN", "ERROR" |
| `debug.log_to_file` | bool | false | Write logs to file |
| `debug.log_filename` | string | "nlm.log" | Log file name |
| `debug.profiler.enable` | bool | false | Enable performance profiler |
| `debug.memory_tracker.enable` | bool | false | Enable memory tracking |

## Configuration File Format

### Example Config File (nlm_config.cfg):

```ini
# NLM Advanced Configuration
# Generated: 2026-11-01

# ========== CORE NEURAL CONFIGURATION ==========
brain.neuron_count = 2000
brain.region_count = 3
brain.population_distribution = "balanced"
brain.v_thresh = -45.0
brain.v_rest = -70.0
brain.v_reset = -75.0
brain.tau_mem = 25.0
brain.tau_ref = 3.0
brain.initial_weight_mean = 0.5
brain.initial_weight_std = 0.15

# ========== PLASTICITY CONFIGURATION ==========
plasticity.stdp.enable = true
plasticity.stdp.ltp_weight = 0.025
plasticity.stdp.ltd_weight = 0.02
plasticity.stdp.tau = 25.0
plasticity.stdp.max_weight = 1.5
plasticity.stdp.min_weight = -1.5
plasticity.hebbian.enable = true
plasticity.hebbian.learning_rate = 0.002
plasticity.structural.enable = true
plasticity.structural.synaptogenesis_rate = 0.0002
plasticity.structural.pruning_rate = 0.00002

# ========== NEUROMODULATION ==========
neuromod.dopamine.enable = true
neuromod.dopamine.baseline = 0.15
neuromod.dopamine.scale = 1.2
neuromod.curiosity.enable = true
neuromod.curiosity.threshold = 0.6
neuromod.novelty.enable = true
neuromod.novelty.sensitivity = 0.15

# ========== MEMORY SYSTEMS ==========
memory.working.enable = true
memory.working.capacity = 200
memory.episodic.enable = true
memory.episodic.capacity = 100
memory.associative.enable = true

# ========== PREDICTION AND COGNITION ==========
cognition.planner.enable = true
cognition.planner.planning_horizon = 15
cognition.concept.enable = true
cognition.attention.enable = true

# ========== DEVELOPMENT ==========
development.enable = true
development.initial_stage = "CriticalPeriod"
development.stage_duration = 2000
development.plasticity_modifier = 1.5

# ========== ENVIRONMENT ==========
environment.name = "GridWorld"
environment.width = 20
environment.height = 20
environment.vision_width = 12
environment.vision_height = 12

# ========== PERFORMANCE ==========
performance.simd.enable = true
performance.parallel.enable = true
performance.checkpointing.enable = true
performance.checkpoint_interval = 2000

# ========== SIMULATION CONTROL ==========
simulation.max_steps = 50000
simulation.time_limit = 0.0
simulation.random_seed = 12345

# ========== DEBUGGING ==========
debug.log_level = "INFO"
debug.log_to_file = true
debug.log_filename = "nlm_advanced.log"
debug.profiler.enable = true
debug.memory_tracker.enable = true
```

## Python Configuration API

### Creating Advanced Configurations

```python
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()

# Modify advanced parameters
config.set("brain.neuron_count", 5000)
config.set("brain.region_count", 5)
config.set("brain.population_distribution", "excitatory_heavy")
config.set("plasticity.stdp.ltp_weight", 0.03)
config.set("plasticity.stdp.ltd_weight", 0.025)
config.set("neuromod.dopamine.baseline", 0.2)
config.set("memory.working.capacity", 500)
config.set("performance.checkpointing.enable", True)

# Save configuration
config.saveToFile("nlm_advanced.cfg")

# Load configuration
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("nlm_advanced.cfg")
```

### Configuration Builder Class

```python
class NLMConfigBuilder:
    """Builder class for creating complex NLM configurations."""
    
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
    
    def set_neural_config(self, neuron_count=1000, region_count=2,
                         distribution="balanced"):
        """Set neural configuration parameters."""
        self.config.set("brain.neuron_count", neuron_count)
        self.config.set("brain.region_count", region_count)
        self.config.set("brain.population_distribution", distribution)
        return self
    
    def set_plasticity_config(self, stdp_ltp=0.02, stdp_ltd=0.015,
                              structural=True):
        """Set plasticity configuration parameters."""
        self.config.set("plasticity.stdp.ltp_weight", stdp_ltp)
        self.config.set("plasticity.stdp.ltd_weight", stdp_ltd)
        self.config.set("plasticity.structural.enable", structural)
        return self
    
    def set_neuromod_config(self, dopamine_baseline=0.1, curiosity=True):
        """Set neuromodulation configuration parameters."""
        self.config.set("neuromod.dopamine.baseline", dopamine_baseline)
        self.config.set("neuromod.curiosity.enable", curiosity)
        return self
    
    def set_memory_config(self, working_capacity=100, episodic_capacity=50):
        """Set memory system configuration parameters."""
        self.config.set("memory.working.capacity", working_capacity)
        self.config.set("memory.episodic.capacity", episodic_capacity)
        return self
    
    def set_performance_config(self, checkpoint_interval=1000,
                              enable_simd=True):
        """Set performance configuration parameters."""
        self.config.set("performance.checkpoint_interval", checkpoint_interval)
        self.config.set("performance.simd.enable", enable_simd)
        return self
    
    def build(self):
        """Build and return the configuration."""
        return self.config
    
    def save(self, filepath):
        """Save the configuration to file."""
        self.config.saveToFile(filepath)
        return self

# Example usage
builder = (NLMConfigBuilder()
    .set_neural_config(neuron_count=3000, region_count=4, distribution="balanced")
    .set_plasticity_config(stdp_ltp=0.025, stdp_ltd=0.02)
    .set_neuromod_config(dopamine_baseline=0.15)
    .set_memory_config(working_capacity=300, episodic_capacity=150)
    .set_performance_config(checkpoint_interval=2000)
)

brain_config = builder.build()
brain_config.save("my_brain_config.cfg")
```

## Configuration Validation

### Validate Configuration

```python
def validate_config(config):
    """Validate configuration parameters."""
    errors = []
    
    # Validate neural configuration
    neuron_count = config.getOr("brain.neuron_count", 1000)
    if neuron_count < 100 or neuron_count > 100000:
        errors.append("brain.neuron_count must be between 100 and 100000")
    
    region_count = config.getOr("brain.region_count", 2)
    if region_count < 1 or region_count > 20:
        errors.append("brain.region_count must be between 1 and 20")
    
    # Validate plasticity configuration
    if config.getOr("plasticity.stdp.enable", True):
        ltp_weight = config.getOr("plasticity.stdp.ltp_weight", 0.02)
        ltd_weight = config.getOr("plasticity.stdp.ltd_weight", 0.015)
        if ltp_weight <= 0 or ltd_weight <= 0:
            errors.append("STDP weights must be positive")
    
    # Validate memory configuration
    working_capacity = config.getOr("memory.working.capacity", 100)
    episodic_capacity = config.getOr("memory.episodic.capacity", 50)
    if working_capacity < 10 or episodic_capacity < 5:
        errors.append("Memory capacities too low for effective operation")
    
    # Validate development configuration
    if config.getOr("development.enable", True):
        stage_duration = config.getOr("development.stage_duration", 1000)
        if stage_duration < 100 or stage_duration > 10000:
            errors.append("development.stage_duration must be between 100 and 10000")
    
    return errors

# Usage
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("plasticity.stdp.ltp_weight", 0.025)

errors = validate_config(config)
if errors:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")
else:
    print("Configuration is valid!")
```

## Configuration Templates

### Templates for Common Use Cases

```python
# Template 1: Fast Training Configuration
fast_training_config = {
    "brain.neuron_count": 1000,
    "brain.region_count": 2,
    "plasticity.stdp.ltp_weight": 0.01,
    "plasticity.stdp.ltd_weight": 0.005,
    "performance.simd.enable": True,
    "performance.parallel.enable": True,
    "debug.log_level": "WARN"
}

# Template 2: High Precision Configuration
precision_config = {
    "brain.neuron_count": 5000,
    "brain.region_count": 5,
    "brain.initial_weight_std": 0.05,
    "plasticity.stdp.ltp_weight": 0.015,
    "plasticity.stdp.ltd_weight": 0.01,
    "plasticity.structural.enable": True,
    "memory.working.capacity": 500,
    "memory.episodic.capacity": 200,
    "debug.log_level": "DEBUG"
}

# Template 3: Curiosity-Driven Exploration
exploration_config = {
    "brain.neuron_count": 2000,
    "neuromod.curiosity.enable": True,
    "neuromod.curiosity.threshold": 0.7,
    "neuromod.novelty.enable": True,
    "neuromod.novelty.sensitivity": 0.2,
    "development.enable": True,
    "development.initial_stage": "CriticalPeriod"
}

# Template 4: Memory-Intensive Configuration
memory_config = {
    "brain.neuron_count": 3000,
    "memory.working.enable": True,
    "memory.working.capacity": 1000,
    "memory.episodic.enable": True,
    "memory.episodic.capacity": 500,
    "memory.associative.enable": True,
    "performance.checkpointing.enable": True,
    "performance.checkpoint_interval": 500
}
```

## Configuration Migration

### Migrate from Old to New Configuration

```python
def migrate_config(old_config):
    """Migrate configuration from old format to new format."""
    new_config = pynlm.createDefaultConfig()
    
    # Migrate core settings
    if old_config.has("neuron_count"):
        new_config.set("brain.neuron_count", old_config.getOr("neuron_count", 1000))
    
    if old_config.has("region_count"):
        new_config.set("brain.region_count", old_config.getOr("region_count", 2))
    
    # Migrate plasticity settings
    if old_config.has("stdp_enabled"):
        new_config.set("plasticity.stdp.enable", old_config.getOr("stdp_enabled", True))
    
    if old_config.has("learning_rate"):
        new_config.set("plasticity.hebbian.learning_rate", old_config.getOr("learning_rate", 0.001))
    
    # Migrate memory settings
    if old_config.has("working_memory_size"):
        new_config.set("memory.working.capacity", old_config.getOr("working_memory_size", 100))
    
    # Keep all other settings as-is
    for key in old_config.getKeys():
        if not key.startswith("neuron_count") and not key.startswith("region_count") and not key.startswith("stdp_enabled") and not key.startswith("learning_rate") and not key.startswith("working_memory_size"):
            new_config.set(key, old_config.getOr(key, None))
    
    return new_config

# Usage
old_config = pynlm.createDefaultConfig()
old_config.set("neuron_count", 2000)
old_config.set("stdp_enabled", True)

new_config = migrate_config(old_config)
new_config.saveToFile("migrated_config.cfg")
```

## Configuration Management

### Configuration Manager Class

```python
class NLMConfigurationManager:
    """Manager for NLM configuration files and templates."""
    
    def __init__(self, config_dir="configs"):
        self.config_dir = config_dir
        self.templates = {}
        self.load_templates()
    
    def load_templates(self):
        """Load predefined configuration templates."""
        self.templates = {
            "fast_training": self._load_template("fast_training"),
            "precision": self._load_template("precision"),
            "exploration": self._load_template("exploration"),
            "memory": self._load_template("memory")
        }
    
    def get_template(self, name):
        """Get a configuration template by name."""
        return self.templates.get(name)
    
    def save_template(self, name, config):
        """Save a configuration as a template."""
        self.templates[name] = config
        # Save to file for persistence
        filepath = f"{self.config_dir}/template_{name}.cfg"
        config.saveToFile(filepath)
    
    def list_templates(self):
        """List available templates."""
        return list(self.templates.keys())
    
    def apply_template(self, template_name, base_config=None):
        """Apply a template to a base configuration."""
        if base_config is None:
            base_config = pynlm.createDefaultConfig()
        
        template = self.get_template(template_name)
        if template:
            for key in template.getKeys():
                value = template.get(key)
                if value is not None:
                    base_config.set(key, value)
        
        return base_config
    
    def export_config(self, config, filename):
        """Export configuration to file."""
        filepath = f"{self.config_dir}/{filename}"
        config.saveToFile(filepath)
        return filepath
    
    def import_config(self, filename):
        """Import configuration from file."""
        filepath = f"{self.config_dir}/{filename}"
        config = pynlm.createDefaultConfig()
        config.loadFromFile(filepath)
        return config

# Example usage
manager = NLMConfigurationManager()

# Create a new configuration using a template
base_config = pynlm.createDefaultConfig()
advanced_config = manager.apply_template("precision", base_config)
advanced_config.saveToFile("my_advanced_config.cfg")

# Save current config as a template
manager.save_template("my_custom", advanced_config)

# List all templates
print(f"Available templates: {manager.list_templates()}")
```

## Configuration Statistics and Analysis

### Analyze Configuration

```python
def analyze_config(config, config_name=""):
    """Analyze configuration parameters and provide statistics."""
    analysis = {
        "name": config_name,
        "total_parameters": 0,
        "categories": {},
        "parameter_types": {},
        "memory_estimates": {}
    }
    
    keys = config.getKeys()
    analysis["total_parameters"] = len(keys)
    
    # Categorize parameters
    categories = {
        "neural": ["brain."],
        "plasticity": ["plasticity."],
        "neuromodulation": ["neuromod."],
        "memory": ["memory."],
        "cognition": ["cognition."],
        "development": ["development."],
        "environment": ["environment."],
        "performance": ["performance."],
        "simulation": ["simulation."],
        "debugging": ["debug."]
    }
    
    for key in keys:
        category_found = False
        for category, prefixes in categories.items():
            for prefix in prefixes:
                if key.startswith(prefix):
                    analysis["categories"][category] = analysis["categories"].get(category, 0) + 1
                    category_found = True
                    break
            if category_found:
                break
        
        if not category_found:
            analysis["categories"]["other"] = analysis["categories"].get("other", 0) + 1
        
        # Count parameter types
        value = config.get(key)
        if value is not None:
            type_name = type(value).__name__
            analysis["parameter_types"][type_name] = analysis["parameter_types"].get(type_name, 0) + 1
    
    # Estimate memory usage
    estimated_memory = {
        "config_overhead": len(keys) * 100,  # Rough estimate
        "string_storage": sum(len(key) for key in keys) * 2,
        "value_storage": analysis["total_parameters"] * 50
    }
    
    analysis["memory_estimates"] = estimated_memory
    analysis["memory_estimates"]["total_mb"] = (estimated_memory["config_overhead"] + 
                                                   estimated_memory["string_storage"] + 
                                                   estimated_memory["value_storage"]) / (1024 * 1024)
    
    return analysis

# Usage
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.set("plasticity.stdp.ltp_weight", 0.025)
config.set("neuromod.dopamine.baseline", 0.15)

analysis = analyze_config(config, "my_brain_config")
print(f"Configuration Analysis: {analysis['name']}")
print(f"Total parameters: {analysis['total_parameters']}")
print(f"Categories: {analysis['categories']}")
print(f"Parameter types: {analysis['parameter_types']}")
print(f"Estimated memory usage: {analysis['memory_estimates']['total_mb']:.2f} MB")
```

## Best Practices

### Configuration Best Practices

1. **Use Descriptive Names**: Use clear, descriptive parameter names
2. **Consistent Naming**: Follow consistent naming conventions (snake_case for parameters)
3. **Document Configuration**: Document all configuration parameters
4. **Validate Parameters**: Always validate configuration parameters before use
5. **Use Templates**: Use templates for common configurations
6. **Backup Configurations**: Keep backups of important configurations
7. **Version Control**: Use version control for configuration files
8. **Environment Variables**: Consider using environment variables for configuration
9. **Configuration Validation**: Implement configuration validation
10. **Performance Testing**: Test configurations for performance

### Python Configuration Best Practices

1. **Use Builder Pattern**: Use builder pattern for complex configurations
2. **Type Safety**: Use type-safe configuration access
3. **Validation**: Validate configuration before use
4. **Error Handling**: Handle configuration errors gracefully
5. **Documentation**: Document configuration usage
6. **Testing**: Test configuration functionality
7. **Logging**: Log configuration changes
8. **Security**: Secure sensitive configuration parameters
9. **Persistence**: Persist configuration settings
10. **Migration**: Support configuration migration

## Example Usage Script

```python
#!/usr/bin/env python3
"""
Example script demonstrating advanced NLM configuration usage.
"""

import pynlm
import json
import sys

def create_production_config():
    """Create a production-ready NLM configuration."""
    config = pynlm.createDefaultConfig()
    
    # Core neural configuration
    config.set("brain.neuron_count", 5000)
    config.set("brain.region_count", 4)
    config.set("brain.population_distribution", "balanced")
    config.set("brain.v_thresh", -45.0)
    config.set("brain.v_rest", -70.0)
    
    # Advanced plasticity
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.ltp_weight", 0.025)
    config.set("plasticity.stdp.ltd_weight", 0.02)
    config.set("plasticity.stdp.tau", 25.0)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.002)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.0002)
    
    # Neuromodulation
    config.set("neuromod.dopamine.enable", True)
    config.set("neuromod.dopamine.baseline", 0.15)
    config.set("neuromod.dopamine.scale", 1.2)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.threshold", 0.6)
    config.set("neuromod.novelty.enable", True)
    
    # Memory systems
    config.set("memory.working.enable", True)
    config.set("memory.working.capacity", 500)
    config.set("memory.episodic.enable", True)
    config.set("memory.episodic.capacity", 200)
    config.set("memory.associative.enable", True)
    
    # Performance
    config.set("performance.simd.enable", True)
    config.set("performance.parallel.enable", True)
    config.set("performance.checkpointing.enable", True)
    config.set("performance.checkpoint_interval", 2000)
    
    # Development
    config.set("development.enable", True)
    config.set("development.initial_stage", "CriticalPeriod")
    config.set("development.stage_duration", 2000)
    config.set("development.plasticity_modifier", 1.5)
    
    # Environment
    config.set("environment.name", "GridWorld")
    config.set("environment.width", 20)
    config.set("environment.height", 20)
    config.set("environment.vision_width", 12)
    config.set("environment.vision_height", 12)
    
    # Simulation control
    config.set("simulation.max_steps", 50000)
    config.set("simulation.random_seed", 12345)
    
    # Debugging
    config.set("debug.log_level", "INFO")
    config.set("debug.log_to_file", True)
    config.set("debug.log_filename", "nlm_production.log")
    
    return config

def save_config_with_metadata(config, filename):
    """Save configuration with metadata."""
    # Create metadata
    metadata = {
        "description": "Production NLM Configuration",
        "version": "2.0",
        "created": "2026-11-01",
        "author": "NLM Team",
        "config_hash": hash(str(config.getKeys())),
        "parameters": dict(config.getKeys())
    }
    
    # Save configuration
    config.saveToFile(filename)
    
    # Save metadata separately
    metadata_file = filename.replace('.cfg', '_metadata.json')
    with open(metadata_file, 'w') as f:
        json.dump(metadata, f, indent=2)
    
    return metadata_file

def validate_and_test_config(config):
    """Validate and test configuration."""
    print("Validating configuration...")
    
    # Validate configuration
    keys = config.getKeys()
    print(f"Configuration has {len(keys)} parameters")
    
    # Check for required parameters
    required_params = ["brain.neuron_count", "plasticity.stdp.enable"]
    missing_params = [param for param in required_params if not config.has(param)]
    
    if missing_params:
        print(f"Warning: Missing required parameters: {missing_params}")
    else:
        print("All required parameters present")
    
    # Test parameter ranges
    neuron_count = config.getOr("brain.neuron_count", 1000)
    if neuron_count < 100 or neuron_count > 100000:
        print(f"Warning: neuron_count ({neuron_count}) is outside recommended range [100, 100000]")
    
    stdp_ltp = config.getOr("plasticity.stdp.ltp_weight", 0.02)
    if stdp_ltp <= 0:
        print("Warning: STDP LTP weight must be positive")
    
    print("Configuration validation complete")

def main():
    """Main function."""
    print("Creating production NLM configuration...")
    
    # Create configuration
    config = create_production_config()
    
    # Validate configuration
    validate_and_test_config(config)
    
    # Save configuration
    config_file = "nlm_production.cfg"
    metadata_file = save_config_with_metadata(config, config_file)
    
    print(f"Configuration saved to: {config_file}")
    print(f"Metadata saved to: {metadata_file}")
    
    # Create a copy with different settings
    config2 = pynlm.createDefaultConfig()
    config2.loadFromFile(config_file)
    config2.set("brain.neuron_count", 2000)
    config2.set("simulation.max_steps", 10000)
    config2.saveToFile("nlm_test.cfg")
    
    print("Test configuration created: nlm_test.cfg")
    
    # Export configuration summary
    print("\nConfiguration Summary:")
    print(f"  - Total parameters: {len(config.getKeys())}")
    print(f"  - Categories: {list(set(key.split('.')[0] for key in config.getKeys()))}")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
```

## Summary

This comprehensive configuration enhancement provides:

1. **Advanced Configuration Categories**: Neural, plasticity, neuromodulation, memory, cognition, development, environment, performance, simulation, and debugging.

2. **Enhanced Python API**: Builder pattern, validation, analysis, and management.

3. **Configuration Templates**: Predefined templates for common use cases.

4. **Configuration Validation**: Comprehensive validation with detailed error messages.

5. **Configuration Analysis**: Detailed analysis of configuration parameters.

6. **Migration Support**: Easy migration from old configuration formats.

7. **Best Practices**: Comprehensive best practices for configuration management.

8. **Example Scripts**: Practical example scripts demonstrating advanced usage.

This enhanced configuration system provides users with powerful tools for creating, managing, and optimizing NLM configurations for a wide range of use cases.
