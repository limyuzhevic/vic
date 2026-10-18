# NLM Configuration Documentation

## Overview

Configuration is central to NLM's operation. The NLM configuration system provides a flexible, hierarchical approach to managing all aspects of neural simulation, from basic brain parameters to advanced learning settings. This document covers all configuration options, best practices, and configuration management strategies.

## Configuration Architecture

### Configuration System Overview

NLM uses a hierarchical configuration system based on JSON-like key-value pairs. Configuration is organized into logical sections:

```
Configuration Hierarchy:
├── Core System Settings
│   ├── neuron_count
│   ├── region_count
│   └── connection_probability
├── Neural Dynamics
│   ├── membrane_parameters
│   ├── plasticity_rules
│   └── spike_generation
├── Learning Systems
│   ├── reward_modulation
│   ├── curiosity
│   └── development
├── Memory Systems
│   ├── working_memory
│   ├── episodic_memory
│   └── associative_memory
├── Prediction Systems
│   ├── temporal_prediction
│   └── action_consequence
├── Cognitive Systems
│   ├── attention
│   ├── planning
│   └── self_model
└── World Interaction
    ├── sensory_processing
    ├── motor_control
    └── environmental_factors
```

### Configuration Sources

Configuration can be loaded from multiple sources, with priority order:

1. **Command Line Arguments** (Highest priority)
2. **JSON Configuration Files** (Medium priority)
3. **Default Configuration** (Lowest priority)

### Configuration Access Methods

#### PyNLM Configuration API

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Load from file
config.loadFromFile("experiment_config.json")

# Load from command line
import sys
config.loadFromArgs(len(sys.argv), sys.argv)

# Access configuration values
neuron_count = config.get("neuron_count")
if config.has("custom_parameter"):
    value = config.get("custom_parameter")

# Get all configuration keys
keys = config.getKeys()

# Save configuration
config.saveToFile("my_config.json")

# Configuration summary
print(config.summary())
```

#### Configuration File Format

JSON configuration files use a hierarchical structure:

```json
{
    "neuron_count": 1000,
    "region_count": 3,
    "connection_probability": 0.1,
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.01,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.005
        }
    },
    "neuromodulation": {
        "dopamine": {
            "scale": 1.0,
            "enable": true
        },
        "curiosity": {
            "enable": true,
            "scale": 0.1
        }
    },
    "development": {
        "enable": true,
        "stages": [
            "initial",
            "critical_period",
            "maturation",
            "adult",
            "aging"
        ],
        "plasticity_rate": 1.0
    }
}
```

### Configuration Templates

#### Standard Configuration

```python
import pynlm

# Create standard configuration for typical experiments
config = pynlm.createStandardConfig()

# Modify specific parameters
config.set("neuron_count", 2000)
config.set("region_count", 4)
config.set("random_seed", 12345)

# Verify configuration
print(config.summary())
```

#### Exploration Configuration

```python
import pynlm

# Create exploration-optimized configuration
config = pynlm.createExplorationConfig()

# Explore different learning parameters
config.set("connection_probability", 0.15)
config.set("synaptogenesis_rate", 0.001)
config.set("pruning_rate", 0.0002)

# Enable exploration systems
config.set("learning.curiosity.enable", true)
config.set("learning.structural.enable", true)
```

#### Custom Configuration Builder

```python
import pynlm
import json

class NLMConfigBuilder:
    """Builder class for creating custom NLM configurations."""
    
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
    
    def add_core_parameters(self, neuron_count, region_count):
        """Add core neural parameters."""
        self.config.set("neuron_count", neuron_count)
        self.config.set("region_count", region_count)
        return self
    
    def add_plasticity_settings(self, stdp_enable=True, hebbian_enable=True,
                                stdp_ltp_weight=0.01, stdp_ltd_weight=0.012):
        """Add plasticity learning rules."""
        self.config.set("plasticity.stdp.enable", stdp_enable)
        self.config.set("plasticity.hebbian.enable", hebbian_enable)
        self.config.set("plasticity.stdp.learning_rate", stdp_ltp_weight)
        self.config.set("plasticity.stdp.tau_plus", stdp_ltd_weight)
        return self
    
    def add_neuromodulation(self, dopamine_scale=1.0, curiosity_enable=True,
                           novelty_enable=True):
        """Add neuromodulation systems."""
        self.config.set("neuromod.dopamine.scale", dopamine_scale)
        self.config.set("neuromod.curiosity.enable", curiosity_enable)
        self.config.set("neuromod.novelty.enable", novelty_enable)
        return self
    
    def add_development(self, enable=True, stages=None):
        """Add development system configuration."""
        self.config.set("development.enable", enable)
        if stages:
            self.config.set("development.stages", stages)
        return self
    
    def add_advanced_features(self, **kwargs):
        """Add advanced features based on keyword arguments."""
        feature_mapping = {
            'working_memory': 'memory.working_memory.enable',
            'episodic_memory': 'memory.episodic.enable',
            'associative_memory': 'memory.associative.enable',
            'temporal_prediction': 'prediction.temporal.enable',
            'action_consequence': 'prediction.action.enable',
            'attention_mechanism': 'cognition.attention.enable',
            'planning_system': 'cognition.planning.enable',
            'self_model': 'cognition.self_model.enable',
            'social_learning': 'cognition.social.enable'
        }
        
        for feature, setting in kwargs.items():
            if feature in feature_mapping:
                self.config.set(feature_mapping[feature], setting)
        
        return self
    
    def build(self):
        """Build and return the configuration."""
        return self.config
    
    def save(self, filename):
        """Save configuration to file."""
        self.config.saveToFile(filename)

# Usage example
builder = NLMConfigBuilder()
config = (builder
          .add_core_parameters(1500, 3)
          .add_plasticity_settings()
          .add_neuromodulation(curiosity_enable=True)
          .add_development(enable=True)
          .add_advanced_features(working_memory=True, episodic_memory=True)
          .build())

builder.save("my_experiment_config.json")
```

## Configuration Parameters

### Core System Parameters

#### Brain Structure

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `neuron_count` | int | 1000 | Total number of neurons | 100 - 1,000,000 |
| `region_count` | int | 1 | Number of brain regions | 1 - 100 |
| `connection_probability` | float | 0.1 | Probability of synapse formation | 0.01 - 1.0 |
| `initial_weight_mean` | float | 0.5 | Mean initial synaptic weight | 0.0 - 1.0 |
| `initial_weight_std` | float | 0.1 | Standard deviation of initial weights | 0.0 - 0.5 |

#### Neural Dynamics

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `v_thresh` | float | -50.0 | Neuron threshold potential (mV) | -80.0 - -20.0 |
| `v_rest` | float | -70.0 | Resting potential (mV) | -80.0 - -60.0 |
| `v_reset` | float | -75.0 | Reset potential after spike (mV) | -80.0 - -70.0 |
| `tau_mem` | float | 20.0 | Membrane time constant (ms) | 10.0 - 50.0 |
| `tau_ref` | float | 2.0 | Refractory period (ms) | 0.5 - 10.0 |

### Plasticity Configuration

#### STDP (Spike-Timing-Dependent Plasticity)

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `plasticity.stdp.enable` | bool | true | Enable STDP learning | true/false |
| `plasticity.stdp.learning_rate` | float | 0.01 | STDP learning rate | 0.001 - 0.1 |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP LTP time constant | 10.0 - 50.0 |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP LTD time constant | 10.0 - 50.0 |
| `plasticity.stdp.max_weight` | float | 1.0 | Maximum synaptic weight | 0.5 - 2.0 |
| `plasticity.stdp.min_weight` | float | 0.0 | Minimum synaptic weight | 0.0 - 0.5 |

#### Hebbian Learning

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning | true/false |
| `plasticity.hebbian.learning_rate` | float | 0.005 | Hebbian learning rate | 0.001 - 0.01 |
| `plasticity.hebbian.threshold` | float | 0.5 | Hebbian activation threshold | 0.1 - 1.0 |

#### Structural Plasticity

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `plasticity.structural.enable` | bool | true | Enable structural plasticity | true/false |
| `plasticity.structural.creation_rate` | float | 0.0001 | Synapse creation rate | 0.00001 - 0.001 |
| `plasticity.structural.pruning_rate` | float | 0.00001 | Synapse removal rate | 0.000001 - 0.0001 |
| `plasticity.structural.max_synapses` | int | 10000 | Maximum synapses per neuron | 1000 - 100000 |

### Neuromodulation Configuration

#### Dopamine System

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale | 0.1 - 5.0 |
| `neuromod.dopamine.enable` | bool | true | Enable dopamine system | true/false |

#### Curiosity System

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration | true/false |
| `neuromod.curiosity.scale` | float | 0.1 | Curiosity modulation scale | 0.01 - 1.0 |
| `neuromod.curiosity.threshold` | float | 0.5 | Curiosity activation threshold | 0.1 - 1.0 |

#### Novelty Detection

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `neuromod.novelty.enable` | bool | true | Enable novelty detection | true/false |
| `neuromod.novelty.sensitivity` | float | 0.3 | Novelty sensitivity | 0.1 - 1.0 |
| `neuromod.novelty.decay_rate` | float | 0.01 | Novelty decay rate | 0.001 - 0.1 |

### Development Configuration

#### Developmental Stages

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.enable` | bool | true | Enable development system |
| `development.stages` | array | ["initial", "critical_period", "maturation", "adult", "aging"] | Developmental stages |
| `development.plasticity_rate` | float | 1.0 | Base plasticity rate | 0.1 - 5.0 |
| `development.critical_period` | float | 100.0 | Critical period duration (steps) | 50 - 500 |
| `development.maturation_rate` | float | 0.1 | Maturation rate | 0.01 - 1.0 |

#### Age-Dependent Plasticity

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `development.age_plasticity` | bool | true | Age-dependent plasticity | true/false |
| `development.youth_boost` | float | 2.0 | Plasticity boost during youth | 1.0 - 5.0 |
| `development.aged_decay` | float | 0.5 | Plasticity decay in old age | 0.1 - 1.0 |

### Memory System Configuration

#### Working Memory

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `memory.working_memory.enable` | bool | true | Enable working memory | true/false |
| `memory.working_memory.capacity` | int | 100 | Working memory capacity | 10 - 1000 |
| `memory.working_memory.decay_rate` | float | 0.01 | Working memory decay rate | 0.001 - 0.1 |
| `memory.working_memory.stability` | float | 0.8 | Working memory stability | 0.5 - 1.0 |

#### Episodic Memory

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `memory.episodic.enable` | bool | true | Enable episodic memory | true/false |
| `memory.episodic.encoding_rate` | float | 0.05 | Episodic encoding rate | 0.01 - 0.1 |
| `memory.episodic.recall_probability` | float | 0.8 | Episodic recall probability | 0.5 - 1.0 |
| `memory.episodic.consolidation_rate` | float | 0.02 | Episodic consolidation rate | 0.001 - 0.05 |

#### Associative Memory

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `memory.associative.enable` | bool | true | Enable associative memory | true/false |
| `memory.associative.learning_rate` | float | 0.03 | Associative learning rate | 0.01 - 0.1 |
| `memory.associative.robustness` | float | 0.7 | Associative memory robustness | 0.5 - 1.0 |
| `memory.associative.generalization` | float | 0.6 | Associative generalization | 0.3 - 1.0 |

### Prediction System Configuration

#### Temporal Prediction

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `prediction.temporal.enable` | bool | true | Enable temporal prediction | true/false |
| `prediction.temporal.horizon` | int | 10 | Prediction horizon (steps) | 1 - 100 |
| `prediction.temporal.accuracy` | float | 0.7 | Temporal prediction accuracy | 0.5 - 1.0 |
| `prediction.temporal.confidence` | bool | true | Enable prediction confidence | true/false |

#### Action Consequence Prediction

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `prediction.action.enable` | bool | true | Enable action consequence prediction | true/false |
| `prediction.action.valuation` | float | 0.5 | Action valuation strength | 0.1 - 1.0 |
| `prediction.action.outcome_accuracy` | float | 0.8 | Action outcome accuracy | 0.5 - 1.0 |

### Cognitive System Configuration

#### Attention Mechanism

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `cognition.attention.enable` | bool | true | Enable attention mechanism | true/false |
| `cognition.attention.focus_strength` | float | 0.6 | Attention focus strength | 0.3 - 1.0 |
| `cognition.attention.burst_rate` | float | 0.2 | Attention burst rate | 0.05 - 0.5 |
| `cognition.attention.gating_strength` | float | 0.8 | Attention gating strength | 0.5 - 1.0 |

#### Planning System

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `cognition.planning.enable` | bool | true | Enable planning system | true/false |
| `cognition.planning.horizon` | int | 5 | Planning horizon (steps) | 1 - 20 |
| `cognition.planning.exploration_bonus` | float | 0.3 | Exploration bonus for planning | 0.1 - 1.0 |
| `cognition.planning.exploitation_weight` | float | 0.7 | Exploitation weight for planning | 0.5 - 1.0 |

#### Self-Model

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `cognition.self_model.enable` | bool | true | Enable self-model system | true/false |
| `cognition.self_model.capacity` | int | 50 | Self-model capacity | 10 - 100 |
| `cognition.self_model.update_rate` | float | 0.1 | Self-model update rate | 0.01 - 0.2 |
| `cognition.self_model.integrity` | float | 0.8 | Self-model integrity | 0.5 - 1.0 |

### World Configuration

#### Sensory Processing

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `sensory.vision.enabled` | bool | true | Enable vision processing | true/false |
| `sensory.vision.resolution` | int | 64 | Vision resolution | 16 - 256 |
| `sensory.audio.enabled` | bool | true | Enable audio processing | true/false |
| `sensory.internal.enabled` | bool | true | Enable internal signals | true/false |

#### Motor Control

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `motor.action_types` | array | ["MoveForward", "TurnLeft", "Look"] | Available action types | 1 - 20 |
| `motor.execution_speed` | float | 1.0 | Motor execution speed | 0.5 - 2.0 |
| `motor.energy_cost` | float | 0.1 | Energy cost per action | 0.01 - 1.0 |
| `motor.accuracy` | float | 0.9 | Motor control accuracy | 0.5 - 1.0 |

### Configuration Validation

#### Required Parameters

Certain parameters are required for specific configurations:

```python
import pynlm

def validate_configuration(config):
    """Validate configuration for required parameters."""
    errors = []
    
    # Core requirements
    if not config.has("neuron_count") or config.get("neuron_count") < 100:
        errors.append("neuron_count must be >= 100")
    
    if not config.has("region_count") or config.get("region_count") < 1:
        errors.append("region_count must be >= 1")
    
    if not config.has("connection_probability"):
        errors.append("connection_probability is required")
    
    # Learning system requirements
    if config.has("plasticity.stdp.enable") and config.get("plasticity.stdp.enable"):
        if not config.has("plasticity.stdp.learning_rate"):
            errors.append("plasticity.stdp.learning_rate is required when STDP is enabled")
    
    # Development requirements
    if config.has("development.enable") and config.get("development.enable"):
        if not config.has("development.stages"):
            errors.append("development.stages is required when development is enabled")
    
    return errors

# Example usage
config = pynlm.createStandardConfig()
errors = validate_configuration(config)

if errors:
    print("Configuration validation errors:")
    for error in errors:
        print(f"  - {error}")
else:
    print("Configuration is valid!")
```

#### Configuration Schema Validation

```python
import pynlm
import json

class ConfigurationSchema:
    """Schema validation for NLM configurations."""
    
    @staticmethod
    def validate_schema(config_dict):
        """Validate configuration against schema."""
        schema = {
            'type': 'object',
            'required': ['neuron_count', 'region_count'],
            'properties': {
                'neuron_count': {'type': 'integer', 'minimum': 100},
                'region_count': {'type': 'integer', 'minimum': 1},
                'connection_probability': {'type': 'number', 'minimum': 0.01, 'maximum': 1.0},
                'plasticity': {
                    'type': 'object',
                    'properties': {
                        'stdp': {
                            'type': 'object',
                            'properties': {
                                'enable': {'type': 'boolean'},
                                'learning_rate': {'type': 'number', 'minimum': 0.001},
                                'tau_plus': {'type': 'number', 'minimum': 1.0},
                                'tau_minus': {'type': 'number', 'minimum': 1.0}
                            }
                        }
                    }
                }
            }
        }
        
        import jsonschema
        jsonschema.validate(config_dict, schema)
        
    @staticmethod
    def get_example_config(filename):
        """Load example configuration from file."""
        with open(filename, 'r') as f:
            return json.load(f)

# Usage
example_config = ConfigurationSchema.get_example_config("example_config.json")
ConfigurationSchema.validate_schema(example_config)
```

## Configuration Management

### Configuration Templates

#### Built-in Templates

```python
import pynlm

# Built-in configuration templates
TEMPLATES = {
    'minimal': {
        'neuron_count': 100,
        'region_count': 1,
        'connection_probability': 0.05
    },
    'standard': {
        'neuron_count': 1000,
        'region_count': 2,
        'connection_probability': 0.1,
        'plasticity': {
            'stdp': {'enable': True, 'learning_rate': 0.01},
            'hebbian': {'enable': True, 'learning_rate': 0.005}
        }
    },
    'exploration': {
        'neuron_count': 500,
        'region_count': 3,
        'connection_probability': 0.15,
        'plasticity': {
            'stdp': {'enable': True, 'learning_rate': 0.02},
            'structural': {'enable': True, 'creation_rate': 0.001}
        },
        'neuromodulation': {
            'curiosity': {'enable': True, 'scale': 0.2}
        }
    },
    'research': {
        'neuron_count': 2000,
        'region_count': 4,
        'connection_probability': 0.08,
        'plasticity': {
            'stdp': {'enable': True, 'learning_rate': 0.015},
            'hebbian': {'enable': True, 'learning_rate': 0.01},
            'structural': {'enable': True, 'creation_rate': 0.002, 'pruning_rate': 0.0001}
        },
        'development': {
            'enable': True,
            'plasticity_rate': 2.0,
            'stages': ['initial', 'critical_period', 'maturation', 'adult']
        },
        'memory': {
            'working_memory': {'enable': True, 'capacity': 200},
            'episodic_memory': {'enable': True, 'encoding_rate': 0.08}
        }
    }
}

# Use templates
def apply_template(template_name, config):
    """Apply configuration template."""
    if template_name not in TEMPLATES:
        raise ValueError(f"Unknown template: {template_name}")
    
    template = TEMPLATES[template_name]
    
    # Apply template to config
    for key, value in template.items():
        if isinstance(value, dict):
            # Nested configuration
            for nested_key, nested_value in value.items():
                config.set(f"{key}.{nested_key}", nested_value)
        else:
            config.set(key, value)
    
    return config

# Example usage
config = pynlm.createDefaultConfig()
exploration_config = apply_template('exploration', config)
```

#### Custom Template Creation

```python
import pynlm
import json

class ConfigurationTemplateManager:
    """Manager for custom configuration templates."""
    
    def __init__(self, template_file="nlm_templates.json"):
        self.template_file = template_file
        self.templates = self.load_templates()
    
    def load_templates(self):
        """Load templates from file."""
        try:
            with open(self.template_file, 'r') as f:
                return json.load(f)
        except FileNotFoundError:
            return {}
    
    def save_templates(self):
        """Save templates to file."""
        with open(self.template_file, 'w') as f:
            json.dump(self.templates, f, indent=2)
    
    def add_template(self, name, config_dict):
        """Add a new template."""
        self.templates[name] = config_dict
    
    def remove_template(self, name):
        """Remove a template."""
        if name in self.templates:
            del self.templates[name]
    
    def apply_template(self, name, config):
        """Apply template to configuration."""
        if name not in self.templates:
            raise ValueError(f"Template not found: {name}")
        
        template = self.templates[name]
        
        # Apply template
        for key, value in template.items():
            if isinstance(value, dict):
                for nested_key, nested_value in value.items():
                    config.set(f"{key}.{nested_key}", nested_value)
            else:
                config.set(key, value)
        
        return config
    
    def list_templates(self):
        """List available templates."""
        return list(self.templates.keys())

# Usage
template_manager = ConfigurationTemplateManager()

# Add custom template
template_manager.add_template("my_research", {
    "neuron_count": 1500,
    "region_count": 3,
    "connection_probability": 0.12,
    "plasticity": {
        "stdp": {"enable": True, "learning_rate": 0.018},
        "hebbian": {"enable": True, "learning_rate": 0.008}
    },
    "development": {
        "enable": True,
        "plasticity_rate": 1.5
    }
})

# Apply template
config = pynlm.createDefaultConfig()
my_config = template_manager.apply_template("my_research", config)
```

### Configuration Serialization

#### Export/Import Configuration

```python
import pynlm
import json

class ConfigurationExporter:
    """Export and import configurations."""
    
    @staticmethod
    def export_config(config, filename):
        """Export configuration to file."""
        config_dict = ConfigurationExporter.config_to_dict(config)
        
        with open(filename, 'w') as f:
            json.dump(config_dict, f, indent=2)
    
    @staticmethod
    def import_config(filename, base_config=None):
        """Import configuration from file."""
        if base_config is None:
            base_config = pynlm.createDefaultConfig()
        
        with open(filename, 'r') as f:
            config_dict = json.load(f)
        
        ConfigurationExporter.dict_to_config(base_config, config_dict)
        return base_config
    
    @staticmethod
    def config_to_dict(config):
        """Convert configuration to dictionary."""
        config_dict = {}
        keys = config.getKeys()
        
        for key in keys:
            # Handle nested keys
            if '.' in key:
                parts = key.split('.')
                current = config_dict
                for part in parts[:-1]:
                    if part not in current:
                        current[part] = {}
                    current = current[part]
                current[parts[-1]] = config.get(key)
            else:
                config_dict[key] = config.get(key)
        
        return config_dict
    
    @staticmethod
    def dict_to_config(config, config_dict):
        """Convert dictionary to configuration."""
        for key, value in config_dict.items():
            if isinstance(value, dict):
                # Nested configuration
                for nested_key, nested_value in value.items():
                    config.set(f"{key}.{nested_key}", nested_value)
            else:
                config.set(key, value)

# Usage
exporter = ConfigurationExporter()

# Export configuration
config = pynlm.createStandardConfig()
exporter.export_config(config, "my_config.json")

# Import configuration
imported_config = exporter.import_config("my_config.json")
```

#### Configuration Comparison

```python
import pynlm

class ConfigurationComparator:
    """Compare configurations."""
    
    @staticmethod
    def compare_configs(config1, config2, name1="Config1", name2="Config2"):
        """Compare two configurations."""
        keys1 = set(config1.getKeys())
        keys2 = set(config2.getKeys())
        
        common_keys = keys1.intersection(keys2)
        only_in_1 = keys1.difference(keys2)
        only_in_2 = keys2.difference(keys1)
        
        print(f"Comparison between {name1} and {name2}:")
        print(f"  Common parameters: {len(common_keys)}")
        print(f"  Only in {name1}: {len(only_in_1)}")
        print(f"  Only in {name2}: {len(only_in_2)}")
        
        # Find differences in common keys
        differences = []
        for key in common_keys:
            value1 = config1.get(key)
            value2 = config2.get(key)
            if value1 != value2:
                differences.append((key, value1, value2))
        
        if differences:
            print(f"  Differences:")
            for key, val1, val2 in differences[:10]:  # Limit to 10 differences
                print(f"    {key}:")
                print(f"      {name1}: {val1}")
                print(f"      {name2}: {val2}")
            if len(differences) > 10:
                print(f"    ... and {len(differences) - 10} more differences")
        
        return {
            'common_keys': len(common_keys),
            'only_in_1': len(only_in_1),
            'only_in_2': len(only_in_2),
            'differences': differences
        }

# Example usage
config1 = pynlm.createStandardConfig()
config1.set("neuron_count", 1000)

config2 = pynlm.createStandardConfig()
config2.set("neuron_count", 2000)

comparison = ConfigurationComparator.compare_configs(config1, config2)
```

## Configuration Best Practices

### 1. Start with Simple Configurations

```python
# Start minimal and add complexity
config = pynlm.createDefaultConfig()

# Basic setup
config.set("neuron_count", 500)

# Add learning if needed
if learning_needed:
    config.set("plasticity.stdp.enable", True)

# Add development for long experiments
if long_experiment:
    config.set("development.enable", True)
```

### 2. Validate Configuration Before Use

```python
def validate_and_log_config(config):
    """Validate configuration and log issues."""
    issues = []
    
    # Check for unreasonable values
    if config.get("neuron_count", 0) > 100000:
        issues.append("neuron_count very large, may impact performance")
    
    if config.get("plasticity.stdp.learning_rate", 0) > 0.1:
        issues.append("STDP learning rate very high, may cause instability")
    
    if config.has("development.enable") and config.get("development.enable"):
        if not config.has("development.stages"):
            issues.append("development.enable true but development.stages not set")
    
    # Log issues
    if issues:
        print("Configuration issues:")
        for issue in issues:
            print(f"  - {issue}")
    
    return len(issues) == 0
```

### 3. Use Descriptive Configuration Names

```python
# Configuration naming conventions
config_names = {
    'baseline': 'Baseline configuration for comparison',
    'exploration': 'Optimized for exploratory learning',
    'development': 'Focused on development and maturation',
    'social': 'Designed for social interaction studies',
    'cognition': 'Emphasizes cognitive capabilities',
    'memory': 'Optimized for memory research'
}

# Create named configurations
named_configs = {}
for name, description in config_names.items():
    config = pynlm.createDefaultConfig()
    config.set("description", description)
    # Apply template based on name
    if name == 'exploration':
        config.set("plasticity.stdp.enable", True)
        config.set("neuromodulation.curiosity.enable", True)
    elif name == 'development':
        config.set("development.enable", True)
        config.set("development.plasticity_rate", 1.5)
    
    named_configs[name] = config
```

### 4. Document Configuration Choices

```python
import pynlm

class DocumentedConfiguration:
    """Configuration with documentation."""
    
    def __init__(self, name, description):
        self.name = name
        self.description = description
        self.config = pynlm.createDefaultConfig()
        self.config.set("name", name)
        self.config.set("description", description)
    
    def add_parameter(self, key, value, description):
        """Add parameter with documentation."""
        self.config.set(key, value)
        
        # Store documentation
        if not hasattr(self, 'parameter_docs'):
            self.parameter_docs = {}
        self.parameter_docs[key] = description
        
        return self
    
    def add_system_config(self, neurons, regions):
        """Add system configuration."""
        self.add_parameter("neuron_count", neurons, "Number of neurons in the brain")
        self.add_parameter("region_count", regions, "Number of brain regions")
        return self
    
    def add_learning_config(self, stdp_rate=0.01, hebbian_rate=0.005):
        """Add learning configuration."""
        self.add_parameter("plasticity.stdp.learning_rate", stdp_rate, "STDP learning rate")
        self.add_parameter("plasticity.hebbian.learning_rate", hebbian_rate, "Hebbian learning rate")
        self.add_parameter("plasticity.stdp.enable", True, "Enable spike-timing-dependent plasticity")
        self.add_parameter("plasticity.hebbian.enable", True, "Enable Hebbian learning")
        return self
    
    def get_documentation(self):
        """Get configuration documentation."""
        doc = f"Configuration: {self.name}\n"
        doc += f"Description: {self.description}\n"
        doc += "\nParameters:\n"
        
        for key, value in self.config.getKeys():
            if key in getattr(self, 'parameter_docs', {}):
                doc += f"  {key}: {value} - {self.parameter_docs[key]}\n"
        
        return doc
    
    def save_with_documentation(self, filename):
        """Save configuration with documentation."""
        # Save configuration
        self.config.saveToFile(filename.replace('.json', '_config.json'))
        
        # Save documentation
        with open(filename.replace('.json', '_doc.md'), 'w') as f:
            f.write(self.get_documentation())

# Usage
config = DocumentedConfiguration(
    "Exploration Study",
    "Configuration optimized for studying curiosity-driven exploration"
).add_system_config(800, 3)
.add_learning_config(stdp_rate=0.015, hebbian_rate=0.008)

config.save_with_documentation("exploration_study.json")
```

### 5. Profile Configuration Performance

```python
import pynlm
import time

class ConfigurationProfiler:
    """Profile configuration performance."""
    
    def __init__(self):
        self.results = {}
    
    def profile_configuration(self, config, steps=1000, name=""):
        """Profile configuration performance."""
        print(f"Profiling configuration {name}...")
        
        # Time configuration creation
        start_time = time.time()
        brain = pynlm.createBrain(config)
        brain.initialize()
        creation_time = time.time() - start_time
        
        # Time simulation
        start_time = time.time()
        for step in range(steps):
            brain.step(step)
        simulation_time = time.time() - start_time
        
        # Collect metrics
        metrics = {
            'creation_time': creation_time,
            'simulation_time': simulation_time,
            'steps_per_second': steps / simulation_time if simulation_time > 0 else 0,
            'neuron_count': brain.getTotalNeuronCount(),
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount()
        }
        
        self.results[name] = metrics
        
        print(f"  Creation time: {creation_time:.3f}s")
        print(f"  Simulation time: {simulation_time:.3f}s")
        print(f"  Steps per second: {metrics['steps_per_second']:.0f}")
        print(f"  Final firing rate: {metrics['final_firing_rate']:.2f}")
        
        return metrics
    
    def compare_configurations(self, configs, steps=1000):
        """Compare multiple configurations."""
        print(f"\nComparing {len(configs)} configurations...")
        
        for name, config in configs.items():
            self.profile_configuration(config, steps, name)
        
        # Find best configuration
        if self.results:
            best_name = max(self.results.items(), key=lambda x: x[1]['steps_per_second'])[0]
            best_result = self.results[best_name]
            
            print(f"\nBest performing configuration: {best_name}")
            print(f"  Steps per second: {best_result['steps_per_second']:.0f}")
            print(f"  Neurons per second: {best_result['neuron_count'] * best_result['steps_per_second']:.0f}")
            
            return best_name, best_result
    
    def save_results(self, filename):
        """Save profiling results."""
        import json
        
        with open(filename, 'w') as f:
            json.dump(self.results, f, indent=2, default=str)
        
        print(f"\nProfiling results saved to {filename}")

# Usage
profiler = ConfigurationProfiler()

configs = {
    'minimal': pynlm.createDefaultConfig(),
    'standard': pynlm.createStandardConfig(),
    'exploration': pynlm.createExplorationConfig()
}

# Set different neuron counts for comparison
for name, config in configs.items():
    if name == 'minimal':
        config.set("neuron_count", 500)
    elif name == 'standard':
        config.set("neuron_count", 1000)
    elif name == 'exploration':
        config.set("neuron_count", 800)

best_config, best_result = profiler.compare_configurations(configs, steps=500)
profiler.save_results("configuration_profiling.json")
```

### 6. Reproducible Experiments

```python
import pynlm
import hashlib
import json

class ReproducibleExperiment:
    """Ensure reproducible experiments."""
    
    def __init__(self, description):
        self.description = description
        self.config = pynlm.createDefaultConfig()
        self.results = {}
        self.random_seed = 42
        
        # Set deterministic seed
        self.config.set("random_seed", self.random_seed)
    
    def set_configuration(self, **kwargs):
        """Set configuration parameters."""
        for key, value in kwargs.items():
            self.config.set(key, value)
        return self
    
    def run_experiment(self, steps, experiment_name):
        """Run experiment and collect results."""
        print(f"Running {experiment_name}...")
        
        # Create brain
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Run simulation
        for step in range(steps):
            brain.step(step)
        
        # Collect results
        self.results[experiment_name] = {
            'config': dict(self.config.getKeys()),  # Convert to dict for JSON serialization
            'total_spikes': brain.getTotalSpikeCount(),
            'final_firing_rate': brain.getAverageFiringRate(),
            'steps_per_second': steps / (len([0 for _ in range(steps)]) or 1),
            'neuron_count': brain.getTotalNeuronCount()
        }
        
        return self.results[experiment_name]
    
    def get_experiment_signature(self):
        """Get hash signature for reproducibility."""
        config_dict = {}
        keys = self.config.getKeys()
        
        for key in keys:
            if '.' in key:
                parts = key.split('.')
                current = config_dict
                for part in parts[:-1]:
                    if part not in current:
                        current[part] = {}
                    current = current[part]
                current[parts[-1]] = self.config.get(key)
            else:
                config_dict[key] = self.config.get(key)
        
        # Create hash
        signature = hashlib.md5(json.dumps(config_dict, sort_keys=True).encode()).hexdigest()
        return signature
    
    def save_results(self, filename):
        """Save experiment results."""
        data = {
            'description': self.description,
            'random_seed': self.random_seed,
            'config_signature': self.get_experiment_signature(),
            'results': self.results
        }
        
        with open(filename, 'w') as f:
            json.dump(data, f, indent=2)
        
        print(f"Experiment results saved to {filename}")
    
    def load_results(self, filename):
        """Load experiment results."""
        with open(filename, 'r') as f:
            data = json.load(f)
        
        self.description = data['description']
        self.random_seed = data['random_seed']
        self.results = data['results']
        
        # Reconstruct configuration from results
        config_dict = {}
        if 'results' in data:
            for exp_name, exp_data in data['results'].items():
                if 'config' in exp_data:
                    config_dict.update(exp_data['config'])
        
        # Apply to current config
        for key, value in config_dict.items():
            self.config.set(key, value)
        
        return self

# Usage
experiment = ReproducibleExperiment(
    "Study of neural development and learning"
)

# Run multiple experiments
experiment.set_configuration(
    neuron_count=1000,
    region_count=3,
    plasticity_stdp_enable=True,
    development_enable=True
)

experiment.run_experiment(1000, "baseline")
experiment.run_experiment(1000, "with_development")

# Save results
experiment.save_results("reproducible_experiment.json")

# Load results for comparison
loaded_experiment = ReproducibleExperiment("Loaded experiment")
loaded_experiment.load_results("reproducible_experiment.json")
```

## Configuration Troubleshooting

### Common Issues and Solutions

#### 1. Brain Initialization Fails

**Problem:** Brain fails to initialize after configuration.

**Symptoms:**
- Configuration values out of range
- Missing required parameters
- Incompatible parameter combinations

**Solutions:**

```python
import pynlm

def debug_brain_initialization(config):
    """Debug brain initialization issues."""
    print("Debugging brain initialization...")
    
    # Check configuration
    print("Configuration check:")
    print(f"  neuron_count: {config.get('neuron_count', 'NOT SET')}")
    print(f"  region_count: {config.get('region_count', 'NOT SET')}")
    print(f"  connection_probability: {config.get('connection_probability', 'NOT SET')}")
    
    # Check parameter ranges
    if config.get("neuron_count", 0) < 100:
        print("  WARNING: neuron_count very low, may not be useful")
    
    if config.get("region_count", 0) < 1:
        print("  ERROR: region_count must be at least 1")
    
    if config.get("connection_probability", 0) <= 0 or config.get("connection_probability", 0) > 1:
        print("  ERROR: connection_probability must be between 0 and 1")
    
    # Try to create brain with error handling
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("  SUCCESS: Brain initialized successfully")
        return brain
    except Exception as e:
        print(f"  ERROR: Brain initialization failed: {e}")
        return None

# Usage
config = pynlm.createDefaultConfig()
config.set("neuron_count", 50)  # Too low
config.set("region_count", 0)  # Invalid

brain = debug_brain_initialization(config)
```

#### 2. Learning Systems Not Working

**Problem:** Learning systems (STDP, Hebbian, etc.) not activating.

**Symptoms:**
- Low or zero learning rates
- No synaptic changes during simulation
- Learning appears to freeze early

**Solutions:**

```python
import pynlm

def debug_learning_systems(config):
    """Debug learning system issues."""
    print("Debugging learning systems...")
    
    # Check STDP settings
    if config.has("plasticity.stdp.enable"):
        stdp_enabled = config.get("plasticity.stdp.enable")
        print(f"  STDP enabled: {stdp_enabled}")
        
        if stdp_enabled:
            if not config.has("plasticity.stdp.learning_rate"):
                print("  WARNING: STDP enabled but learning_rate not set")
            else:
                learning_rate = config.get("plasticity.stdp.learning_rate")
                print(f"  STDP learning rate: {learning_rate}")
                
                if learning_rate <= 0:
                    print("  ERROR: STDP learning rate must be positive")
    
    # Check Hebbian settings
    if config.has("plasticity.hebbian.enable"):
        hebbian_enabled = config.get("plasticity.hebbian.enable")
        print(f"  Hebbian enabled: {hebbian_enabled}")
        
        if hebbian_enabled:
            if not config.has("plasticity.hebbian.learning_rate"):
                print("  WARNING: Hebbian enabled but learning_rate not set")
    
    # Check structural plasticity
    if config.has("plasticity.structural.enable"):
        structural_enabled = config.get("plasticity.structural.enable")
        print(f"  Structural plasticity enabled: {structural_enabled}")
        
        if structural_enabled:
            if not config.has("plasticity.structural.creation_rate"):
                print("  WARNING: Structural plasticity enabled but creation_rate not set")
    
    # Test learning in simulation
    print("\nTesting learning in simulation...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Check initial state
    initial_spikes = brain.getTotalSpikeCount()
    print(f"  Initial spike count: {initial_spikes}")
    
    # Run a few steps
    for step in range(10):
        brain.step(step)
    
    # Check for changes
    final_spikes = brain.getTotalSpikeCount()
    print(f"  Spike count after 10 steps: {final_spikes}")
    
    if final_spikes == initial_spikes:
        print("  WARNING: No spikes generated, learning systems may not be active")
    
    return brain

# Usage
config = pynlm.createDefaultConfig()
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.0)  # Invalid: too low

brain = debug_learning_systems(config)
```

#### 3. Performance Issues

**Problem:** Poor performance, slow simulation.

**Symptoms:**
- Very slow step execution
- High memory usage
- System becomes unresponsive

**Solutions:**

```python
import pynlm
import time

def debug_performance(config):
    """Debug performance issues."""
    print("Debugging performance...")
    
    # Check configuration for performance impact
    neuron_count = config.get("neuron_count", 1000)
    region_count = config.get("region_count", 1)
    
    print(f"  neuron_count: {neuron_count}")
    print(f"  region_count: {region_count}")
    
    # Estimate memory usage
    estimated_memory_mb = (neuron_count * 64 + region_count * 1024) / 1024 / 1024
    print(f"  Estimated memory usage: {estimated_memory_mb:.1f} MB")
    
    # Check for performance bottlenecks
    if neuron_count > 100000:
        print("  WARNING: Large brain size, may impact performance")
    
    if region_count > 10:
        print("  WARNING: Many regions, may impact simulation speed")
    
    # Profile simulation performance
    print("\nProfiling simulation performance...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Time simulation
    start_time = time.time()
    steps_to_run = min(100, neuron_count // 1000)  # Adjust for large brains
    
    for step in range(steps_to_run):
        brain.step(step)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    steps_per_second = steps_to_run / elapsed if elapsed > 0 else 0
    print(f"  Time for {steps_to_run} steps: {elapsed:.2f}s")
    print(f"  Performance: {steps_per_second:.0f} steps/second")
    print(f"  Neurons per second: {neuron_count * steps_per_second:.0f}")
    
    # Check for performance warnings
    if steps_per_second < 100:
        print("  WARNING: Slow performance, consider:")
        print("    - Reducing neuron count")
        print("    - Disabling some learning systems")
        print("    - Using simpler configuration")
    
    return brain, steps_per_second

# Usage
config = pynlm.createExplorationConfig()
config.set("neuron_count", 50000)  # Very large

brain, perf = debug_performance(config)
```

#### 4. Reproduction Issues

**Problem:** Experiments not reproducible.

**Symptoms:**
- Different results with same configuration
- Random seed not working
- Non-deterministic behavior

**Solutions:**

```python
import pynlm
import hashlib
import json

def ensure_reproducibility(config, experiment_name, steps):
    """Ensure experiment reproducibility."""
    print(f"Ensuring reproducibility for {experiment_name}...")
    
    # Check random seed
    if not config.has("random_seed"):
        print("  WARNING: random_seed not set, results may not be reproducible")
        config.set("random_seed", 42)
    
    random_seed = config.get("random_seed")
    print(f"  Random seed: {random_seed}")
    
    # Create deterministic hash
    config_dict = {}
    keys = config.getKeys()
    
    for key in keys:
        if '.' in key:
            parts = key.split('.')
            current = config_dict
            for part in parts[:-1]:
                if part not in current:
                    current[part] = {}
                current = current[part]
            current[parts[-1]] = config.get(key)
        else:
            config_dict[key] = config.get(key)
    
    # Create signature
    signature = hashlib.md5(json.dumps(config_dict, sort_keys=True).encode()).hexdigest()
    print(f"  Configuration signature: {signature}")
    
    # Run experiment
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Collect data
    results = {
        'signature': signature,
        'random_seed': random_seed,
        'neuron_count': brain.getTotalNeuronCount(),
        'steps': steps,
        'total_spikes': brain.getTotalSpikeCount(),
        'final_firing_rate': brain.getAverageFiringRate()
    }
    
    # Run simulation
    start_time = time.time()
    for step in range(steps):
        brain.step(step)
    
    end_time = time.time()
    
    results['simulation_time'] = end_time - start_time
    results['steps_per_second'] = steps / (end_time - start_time)
    
    print(f"  Steps per second: {results['steps_per_second']:.0f}")
    print(f"  Total spikes: {results['total_spikes']}")
    print(f"  Final firing rate: {results['final_firing_rate']:.2f}")
    
    return results

# Usage
config = pynlm.createDefaultConfig()
# Test without seed
print("Test 1: Without random seed")
results1 = ensure_reproducibility(config, "test1", 100)

# Test with seed
print("\nTest 2: With random seed")
config.set("random_seed", 123)
results2 = ensure_reproducibility(config, "test2", 100)

# Compare signatures
if results1['signature'] == results2['signature']:
    print("\n✓ Signatures match - results are reproducible")
else:
    print("\n✗ Signatures differ - results may not be reproducible")
```

## Configuration Files and Examples

### Example Configuration Files

#### basic_config.json

```json
{
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.1,
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.01
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.005
        }
    }
}
```

#### exploration_config.json

```json
{
    "neuron_count": 800,
    "region_count": 3,
    "connection_probability": 0.15,
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.02
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.01
        },
        "structural": {
            "enable": true,
            "creation_rate": 0.001
        }
    },
    "neuromodulation": {
        "curiosity": {
            "enable": true,
            "scale": 0.2
        }
    },
    "development": {
        "enable": true,
        "plasticity_rate": 1.5
    }
}
```

#### research_config.json

```json
{
    "neuron_count": 2000,
    "region_count": 4,
    "connection_probability": 0.08,
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.015
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.008
        },
        "structural": {
            "enable": true,
            "creation_rate": 0.002,
            "pruning_rate": 0.0001
        }
    },
    "development": {
        "enable": true,
        "plasticity_rate": 2.0,
        "stages": ["initial", "critical_period", "maturation", "adult"],
        "critical_period": 100.0
    },
    "memory": {
        "working_memory": {
            "enable": true,
            "capacity": 200
        },
        "episodic_memory": {
            "enable": true,
            "encoding_rate": 0.08
        }
    },
    "prediction": {
        "temporal": {
            "enable": true,
            "horizon": 10
        }
    }
}
```

### Configuration File Utilities

#### Configuration Validator

```python
import pynlm
import json
import sys

class ConfigValidator:
    """Validate configuration files."""
    
    @staticmethod
    def validate_json_file(filename):
        """Validate JSON configuration file."""
        try:
            with open(filename, 'r') as f:
                config_data = json.load(f)
            
            print(f"✓ Valid JSON format in {filename}")
            return config_data
            
        except json.JSONDecodeError as e:
            print(f"✗ Invalid JSON in {filename}: {e}")
            return None
        except FileNotFoundError:
            print(f"✗ File not found: {filename}")
            return None
    
    @staticmethod
    def validate_pynlm_config(config_data):
        """Validate configuration against NLM requirements."""
        if not isinstance(config_data, dict):
            print("✗ Configuration must be a JSON object")
            return False
        
        # Check required keys
        required_keys = ['neuron_count', 'region_count', 'connection_probability']
        missing_keys = [key for key in required_keys if key not in config_data]
        
        if missing_keys:
            print(f"✗ Missing required keys: {missing_keys}")
            return False
        
        # Check value ranges
        errors = []
        
        if not isinstance(config_data['neuron_count'], int) or config_data['neuron_count'] < 100:
            errors.append("neuron_count must be integer >= 100")
        
        if not isinstance(config_data['region_count'], int) or config_data['region_count'] < 1:
            errors.append("region_count must be integer >= 1")
        
        if not isinstance(config_data['connection_probability'], (int, float)) or \
           config_data['connection_probability'] <= 0 or config_data['connection_probability'] > 1:
            errors.append("connection_probability must be between 0 and 1")
        
        if errors:
            print("✗ Configuration validation errors:")
            for error in errors:
                print(f"  - {error}")
            return False
        
        print("✓ Configuration validation passed")
        return True
    
    @staticmethod
    def apply_config_to_brain(config_data):
        """Apply configuration data to create brain."""
        try:
            # Create configuration
            config = pynlm.createDefaultConfig()
            
            # Apply configuration data
            for key, value in config_data.items():
                if '.' in key:
                    # Handle nested keys
                    parts = key.split('.')
                    current = config
                    for part in parts[:-1]:
                        if part not in current:
                            print(f"✗ Invalid nested key: {key}")
                            return None
                        current = current.get(part, {})
                    current.set(parts[-1], value)
                else:
                    config.set(key, value)
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            print("✓ Brain created successfully from configuration")
            return brain
            
        except Exception as e:
            print(f"✗ Error creating brain from configuration: {e}")
            return None
    
    @staticmethod
    def validate_and_create(filename):
        """Complete validation and brain creation."""
        print(f"\nValidating and creating brain from {filename}...")
        
        # Load configuration
        config_data = ConfigValidator.validate_json_file(filename)
        if not config_data:
            return None
        
        # Validate configuration
        if not ConfigValidator.validate_pynlm_config(config_data):
            return None
        
        # Create brain
        brain = ConfigValidator.apply_config_to_brain(config_data)
        if not brain:
            return None
        
        return brain

# Usage
validator = ConfigValidator()

# Validate and create from file
brain = validator.validate_and_create("exploration_config.json")

if brain:
    print(f"Successfully created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Run quick test
    for step in range(10):
        brain.step(step)
    
    print(f"Test completed: {brain.getTotalSpikeCount()} spikes generated")
```

#### Configuration Migration Tool

```python
import pynlm
import json

class ConfigurationMigrator:
    """Migrate configurations between versions."""
    
    def __init__(self):
        self.version_mappings = {
            'v1_to_v2': self._migrate_v1_to_v2,
            'v2_to_v3': self._migrate_v2_to_v3,
            'legacy_to_modern': self._migrate_legacy_to_modern
        }
    
    def _migrate_v1_to_v2(self, config_dict):
        """Migrate version 1 to version 2 configuration."""
        migrated = config_dict.copy()
        
        # Add new version 2 parameters
        migrated['version'] = 'v2'
        migrated['plasticity'] = {
            'stdp': {
                'enable': migrated.get('stdp_enable', True),
                'learning_rate': migrated.get('stdp_learning_rate', 0.01)
            },
            'hebbian': {
                'enable': migrated.get('hebbian_enable', True),
                'learning_rate': migrated.get('hebbian_learning_rate', 0.005)
            }
        }
        
        # Remove old version 1 parameters
        old_keys = ['stdp_enable', 'hebbian_enable', 'stdp_learning_rate', 'hebbian_learning_rate']
        for key in old_keys:
            migrated.pop(key, None)
        
        return migrated
    
    def _migrate_v2_to_v3(self, config_dict):
        """Migrate version 2 to version 3 configuration."""
        migrated = self._migrate_v1_to_v2(config_dict)
        
        # Add new version 3 parameters
        migrated['version'] = 'v3'
        migrated['neuromodulation'] = {
            'curiosity': {
                'enable': migrated.get('curiosity_enable', True),
                'scale': migrated.get('curiosity_scale', 0.1)
            },
            'novelty': {
                'enable': migrated.get('novelty_enable', True),
                'sensitivity': migrated.get('novelty_sensitivity', 0.3)
            }
        }
        
        # Remove old version 2 parameters
        old_keys = ['curiosity_enable', 'novelty_enable', 'curiosity_scale', 'novelty_sensitivity']
        for key in old_keys:
            migrated.pop(key, None)
        
        return migrated
    
    def _migrate_legacy_to_modern(self, config_dict):
        """Migrate legacy configuration to modern format."""
        migrated = config_dict.copy()
        
        # Add modern structure
        migrated['version'] = 'modern'
        migrated['core'] = {
            'neurons': migrated.get('neuron_count', 1000),
            'regions': migrated.get('region_count', 1)
        }
        migrated['learning'] = {
            'plasticity': {
                'stdp': {'enable': True},
                'hebbian': {'enable': True}
            }
        }
        migrated['development'] = {
            'enable': True,
            'stages': ['initial', 'critical_period', 'maturation', 'adult']
        }
        
        # Keep original parameters
        migrated['connection_probability'] = migrated.get('connection_probability', 0.1)
        
        return migrated
    
    def migrate_config(self, source_filename, target_version='v3'):
        """Migrate configuration file to target version."""
        try:
            # Load source configuration
            with open(source_filename, 'r') as f:
                source_config = json.load(f)
            
            # Determine migration function
            migration_key = f'legacy_to_{target_version}'
            if migration_key in self.version_mappings:
                migrate_func = self.version_mappings[migration_key]
            elif f'v2_to_{target_version}' in self.version_mappings:
                migrate_func = self.version_mappings[f'v2_to_{target_version}']
            else:
                print(f"✗ No migration function found for {target_version}")
                return False
            
            # Migrate configuration
            migrated_config = migrate_func(source_config)
            
            # Save migrated configuration
            target_filename = source_filename.replace('.json', f'_{target_version}.json')
            
            with open(target_filename, 'w') as f:
                json.dump(migrated_config, f, indent=2)
            
            print(f"✓ Migrated {source_filename} to {target_filename}")
            return True
            
        except FileNotFoundError:
            print(f"✗ Source file not found: {source_filename}")
            return False
        except json.JSONDecodeError as e:
            print(f"✗ Invalid JSON in source file: {e}")
            return False
        except Exception as e:
            print(f"✗ Migration error: {e}")
            return False
    
    def batch_migrate(self, source_files, target_version='v3'):
        """Migrate multiple configuration files."""
        print(f"\nBatch migrating {len(source_files)} configuration files to {target_version}...")
        
        success_count = 0
        for source_file in source_files:
            if self.migrate_config(source_file, target_version):
                success_count += 1
        
        print(f"\nMigration complete: {success_count}/{len(source_files)} files migrated successfully")
        return success_count == len(source_files)

# Usage
migrator = ConfigurationMigrator()

# Migrate single file
migrator.migrate_config("legacy_config.json", "v3")

# Migrate multiple files
migrator.batch_migrate(["config1.json", "config2.json", "config3.json"], "v3")
```

## Conclusion

Configuration management is a critical aspect of NLM research and development. The configuration system provides:

### Key Benefits:

1. **Flexibility**: Wide range of configuration options for diverse research needs
2. **Reproducibility**: Deterministic experiments through proper configuration
3. **Performance Control**: Configuration affects simulation performance and accuracy
4. **Extensibility**: Easy to add new configuration options and features
5. **Validation**: Comprehensive validation ensures correct configurations

### Best Practices:

1. **Start Simple**: Begin with minimal configurations and add complexity gradually
2. **Document**: Document configuration choices for reproducibility
3. **Validate**: Validate configurations before use
4. **Profile**: Profile performance with different configurations
5. **Reproduce**: Ensure experiments are reproducible

### Future Directions:

1. **Configuration AI**: Automated configuration generation
2. **Dynamic Configuration**: Runtime configuration changes
3. **Configuration Learning**: Machine learning for optimal configurations
4. **Distributed Configuration**: Configuration management across distributed systems

This comprehensive configuration documentation provides the foundation for effective NLM research, development, and experimentation. The configuration system enables users to:

- Create tailored neural simulations for specific research questions
- Optimize performance for different computational resources
- Ensure reproducibility of experiments
- Explore the full capabilities of the NLM framework

By following the guidelines and best practices outlined in this document, users can effectively leverage NLM configurations to advance their neural simulation research.
