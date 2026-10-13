# NLM - Configuration Reference

**Date:** 2026-10-13
**Audience:** All users
**Level:** Configuration reference

---

## Table of Contents

1. [Configuration Overview](#configuration-overview)
2. [Brain Configuration](#brain-configuration)
3. [Plasticity Configuration](#plasticity-configuration)
4. [Neuromodulation Configuration](#neuromodulation-configuration)
5. [Development Configuration](#development-configuration)
6. [Simulation Configuration](#simulation-configuration)
7. [Advanced API Configuration](#advanced-api-configuration)
8. [File Configuration](#file-configuration)
9. [Configuration Validation](#configuration-validation)
10. [Configuration Examples](#configuration-examples)

---

## Configuration Overview

The NLM system uses a hierarchical configuration system with **nested sections** for organizing parameters. Configuration can be set programmatically, loaded from files, or created with sensible defaults.

### Configuration Structure

```
Configuration Hierarchy:
├── brain                    # Brain system parameters
│   ├── neuron_count         # Number of neurons
│   ├── synapse_density      # Connectivity density
│   └── plasticity           # Plasticity parameters
├── neuromodulation           # Neuromodulator parameters
│   ├── dopamine            # Dopamine system
│   ├── curiosity           # Curiosity system
│   └── novelty              # Novelty detection
├── development              # Development system
│   ├── enabled             # Development enabled
│   ├── critical_period     # Critical period settings
│   └── maturation          # Maturation parameters
├── simulation               # Simulation parameters
│   ├── steps               # Number of simulation steps
│   ├── dt                  # Time step
│   └── world                # World parameters
└── advanced_api             # Advanced API configuration
    ├── profiling           # Profiling settings
    ├── analysis            # Analysis parameters
    └── export              # Export settings
```

### Configuration Access Methods

```python
# Method 1: Direct access
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

# Method 2: Nested access
brain_neuron_count = config.get("brain.neuron_count", 1000)

# Method 3: Has check
if config.has("brain.neuron_count"):
    value = config.get("brain.neuron_count")

# Method 4: File operations
config.loadFromFile("config.json")
config.saveToFile("config.json")

# Method 5: Factory functions
advanced_config = pynlm.createAdvancedConfig()
```

## Brain Configuration

### Core Brain Parameters

| Key | Type | Default | Description | Units |
|----|------|---------|-------------|-------|
| `brain.neuron_count` | int | 1000 | Total number of neurons | count |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density | dimensionless |
| `brain.connection_probability` | float | 0.05 | Probability of connection | probability |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight | dimensionless |
| `brain.initial_weight_std` | float | 0.1 | Std dev of initial weights | dimensionless |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential | mV |
| `brain.v_rest` | float | -70.0 | Resting potential | mV |
| `brain.v_reset` | float | -75.0 | Reset potential after spike | mV |
| `brain.tau_mem` | float | 20.0 | Membrane time constant | ms |
| `brain.tau_ref` | float | 2.0 | Refractory period | ms |

### Brain Region Configuration

| Key | Type | Default | Description | Units |
|----|------|---------|-------------|-------|
| `brain.region.cortex.excitatory.neuron_count` | int | 800 | Excitatory neurons in cortex | count |
| `brain.region.cortex.inhibitory.neuron_count` | int | 200 | Inhibitory neurons in cortex | count |
| `brain.region.hippocampus.neuron_count` | int | 150 | Neurons in hippocampus | count |
| `brain.region.amygdala.neuron_count` | int | 50 | Neurons in amygdala | count |

### Neural Dynamics Parameters

| Key | Type | Default | Description | Units |
|----|------|---------|-------------|-------|
| `brain.dynamics.spike_threshold` | float | -50.0 | Spike threshold | mV |
| `brain.dynamics.reset_potential` | float | -75.0 | Reset potential | mV |
| `brain.dynamics.refractory_period` | float | 2.0 | Refractory period | ms |
| `brain.dynamics.tau_m` | float | 20.0 | Membrane time constant | ms |
| `brain.dynamics.tau_m_rest` | float | 100.0 | Resting membrane time constant | ms |
| `brain.dynamics.v_leak` | float | -65.0 | Leak potential | mV |
| `brain.dynamics.c_m` | float | 1.0 | Membrane capacitance | μF/cm² |

## Plasticity Configuration

### STDP (Spike-Timing-Dependent Plasticity)

| Key | Type | Default | Description | Units |
|----|------|---------|-------------|-------|
| `plasticity.stdp.enable` | bool | true | Enable STDP | boolean |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate | dimensionless |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (potentiation) | ms |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (depression) | ms |
| `plasticity.stdp.max_weight` | float | 1.0 | Maximum synaptic weight | dimensionless |
| `plasticity.stdp.min_weight` | float | 0.0 | Minimum synaptic weight | dimensionless |
| `plasticity.stdp.hebbian_weight_scale` | float | 0.1 | Hebbian weight scaling | dimensionless |

### Hebbian Plasticity

| Key | Type | Default | Description | Units |
|----|------|---------|-------------|-------|
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning | boolean |
| `plasticity.hebbian.learning_rate` | float | 0.01 | Hebbian learning rate | dimensionless |
| `plasticity.hebbian.correlation_window` | float | 50.0 | Correlation window size | ms |
| `plasticity.hebbian.association_strength` | float | 0.5 | Association strength | dimensionless |

### Structural Plasticity

| Key | Type | Default | Description | Units |
|----|------|---------|-------------|-------|
| `plasticity.structural.enable` | bool | true | Enable structural plasticity | boolean |
| `plasticity.structural.synaptogenesis_rate` | float | 0.01 | Synaptogenesis rate | synapses/ms |
| `plasticity.structural.pruning_rate` | float | 0.005 | Pruning rate | synapses/ms |
| `plasticity.structural.max_synapses_per_neuron` | int | 100 | Maximum synapses per neuron | count |
| `plasticity.structural.min_synapses_per_neuron` | int | 1 | Minimum synapses per neuron | count |

## Neuromodulation Configuration

### Dopamine System

| Key | Type | Default | Description | Units |
|----|------|---------|-------------|-------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale | dimensionless |
| `neuromod.dopamine.learning_rate` | float | 0.001 | Dopamine learning rate | dimensionless |
| `neuromod.dopamine.noise_level` | float | 0.1 | Dopamine noise level | dimensionless |
| `neuromod.dopamine.receiptive_field` | float | 10.0 | Dopamine receptive field size | ms |

### Curiosity System

| Key | Type | Default | Description | Units |
|----|------|---------|-------------|-------|
| `neuromod.curiosity.enable` | bool | true | Enable curiosity system | boolean |
| `neuromod.curiosity.sensitivity` | float | 0.5 | Curiosity sensitivity | dimensionless |
| `neuromod.curiosity.exploration_drive` | float | 0.8 | Exploration drive | dimensionless |
| `neuromod.curiosity.novelty_threshold` | float | 0.5 | Novelty threshold | dimensionless |
| `neuromod.curiosity.exploration_cost` | float | 0.1 | Exploration cost | dimensionless |

### Novelty Detection

| Key | Type | Default | Description | Units |
|----|------|---------|-------------|-------|
| `neuromod.novelty.enable` | bool | true | Enable novelty detection | boolean |
| `neuromod.novelty.sensitivity` | float | 0.5 | Novelty sensitivity | dimensionless |
| `neuromod.novelty.adaptation_rate` | float | 0.01 | Novelty adaptation rate | 1/ms |
| `neuromod.novelty.memory_decay` | float | 0.01 | Novelty memory decay | 1/ms |

### Prediction Error

| Key | Type | Default | Description | Units |
|----|------|---------|-------------|-------|
| `neuromod.prediction_error.enable` | bool | true | Enable prediction error | boolean |
| `neuromod.prediction_error.learning_rate` | float | 0.01 | Prediction error learning rate | dimensionless |
| `neuromod.prediction_error.confidence_threshold` | float | 0.8 | Confidence threshold | dimensionless |
| `neuromod.prediction_error.error_variance` | float | 0.1 | Error variance | dimensionless |

## Development Configuration

### General Development

| Key | Type | Default | Description | Units |
|----|------|---------|-------------|-------|
| `development.enabled` | bool | true | Enable development system | boolean |
| `development.age_unit` | float | 1.0 | Age unit scaling | dimensionless |
| `development.development_rate` | float | 0.1 | Overall development rate | 1/ms |

### Critical Period

| Key | Type | Default | Description | Units |
|----|------|---------|-------------|-------|
| `development.critical_period.enabled` | bool | true | Enable critical period | boolean |
| `development.critical_period.start_age` | float | 10.0 | Critical period start age | ms |
| `development.critical_period.end_age` | float | 100.0 | Critical period end age | ms |
| `development.critical_period.plasticity_boost` | float | 2.0 | Plasticity boost during critical period | dimensionless |
| `development.critical_period.sensitivity` | float | 0.8 | Critical period sensitivity | dimensionless |

### Maturation

| Key | Type | Default | Description | Units |
|----|------|---------|-------------|-------|
| `development.maturation.synapse_pruning_rate` | float | 0.01 | Synapse pruning rate | synapses/ms |
| `development.maturation.excitatory_inhibitory_balance` | float | 4.0 | E/I balance target | dimensionless |
| `development.maturation.neural_excitability` | float | 1.0 | Neural excitability | dimensionless |
| `development.maturation.plasticity_decay` | float | 0.001 | Plasticity decay rate | 1/ms |

## Simulation Configuration

### Core Simulation Parameters

| Key | Type | Default | Description | Units |
|----|------|---------|-------------|-------|
| `simulation.steps` | int | 1000 | Number of simulation steps | steps |
| `simulation.dt` | float | 0.1 | Simulation time step | ms |
| `simulation.max_time` | float | 100.0 | Maximum simulation time | ms |
| `simulation.random_seed` | int | 42 | Random seed for reproducibility | dimensionless |
| `simulation.enable_logging` | bool | false | Enable logging | boolean |
| `simulation.log_frequency` | int | 100 | Log frequency | steps |

### World Configuration

| Key | Type | Default | Description | Units |
|----|------|---------|-------------|-------|
| `world.width` | int | 20 | World width | units |
| `world.height` | int | 20 | World height | units |
| `world.vision_width` | int | 8 | Vision width | units |
| `world.vision_height` | int | 8 | Vision height | units |
| `world.time_step` | float | 0.1 | World time step | ms |
| `world.max_energy` | float | 100.0 | Maximum energy | energy_units |
| `world.energy_decay_rate` | float | 0.01 | Energy decay rate | 1/ms |

## Advanced API Configuration

### Brain Profiling

| Key | Type | Default | Description | Units |
|----|------|---------|-------------|-------|
| `advanced_api.brain_profiler.enable` | bool | true | Enable brain profiling | boolean |
| `advanced_api.brain_profiler.sampling_rate` | int | 10 | Profiling sampling rate | samples/ms |
| `advanced_api.brain_profiler.duration_threshold` | float | 1.0 | Duration threshold | ms |
| `advanced_api.brain_profiler.export_format` | string | "json" | Export format | string |

### Network Analysis

| Key | Type | Default | Description | Units |
|----|------|---------|-------------|-------|
| `advanced_api.network_analyzer.enable` | bool | true | Enable network analysis | boolean |
| `advanced_api.network_analyzer.centrality_calculation` | string | "eigenvector" | Centrality calculation method | string |
| `advanced_api.network_analyzer.community_detection` | string | "louvain" | Community detection algorithm | string |
| `advanced_api.network_analyzer.temporal_resolution` | int | 100 | Temporal resolution | ms |

### Memory Analysis

| Key | Type | Default | Description | Units |
|----|------|---------|-------------|-------|
| `advanced_api.memory_analyzer.enable` | bool | true | Enable memory analysis | boolean |
| `advanced_api.memory_analyzer.working_memory_window` | int | 50 | Working memory time window | ms |
| `advanced_api.memory_analyzer.episodic_retention_threshold` | float | 0.5 | Episodic retention threshold | dimensionless |
| `advanced_api.memory_analyzer.consolidation_rate` | float | 0.1 | Memory consolidation rate | 1/ms |

## File Configuration

### JSON Configuration Format

```json
{
  "brain": {
    "neuron_count": 1000,
    "synapse_density": 0.1,
    "plasticity": {
      "stdp": {
        "enable": true,
        "learning_rate": 0.001
      },
      "hebbian": {
        "enable": true,
        "learning_rate": 0.01
      }
    }
  },
  "neuromodulation": {
    "dopamine": {
      "scale": 1.0,
      "learning_rate": 0.001
    },
    "curiosity": {
      "enable": true,
      "sensitivity": 0.5
    }
  },
  "development": {
    "enabled": true,
    "critical_period": {
      "enabled": true,
      "plasticity_boost": 2.0
    }
  },
  "simulation": {
    "steps": 1000,
    "dt": 0.1,
    "world": {
      "width": 20,
      "height": 20,
      "vision_width": 8,
      "vision_height": 8
    }
  }
}
```

### YAML Configuration Format

```yaml
brain:
  neuron_count: 1000
  synapse_density: 0.1
  plasticity:
    stdp:
      enable: true
      learning_rate: 0.001
    hebbian:
      enable: true
      learning_rate: 0.01

neuromodulation:
  dopamine:
    scale: 1.0
    learning_rate: 0.001
  curiosity:
    enable: true
    sensitivity: 0.5

development:
  enabled: true
  critical_period:
    enabled: true
    plasticity_boost: 2.0

simulation:
  steps: 1000
  dt: 0.1
  world:
    width: 20
    height: 20
    vision_width: 8
    vision_height: 8
```

## Configuration Validation

### Configuration Schema

```python
# Valid configuration schema example
VALID_CONFIG_SCHEMA = {
    'brain': {
        'neuron_count': {'type': int, 'min': 1, 'max': 1000000},
        'synapse_density': {'type': float, 'min': 0.0, 'max': 1.0},
        'connection_probability': {'type': float, 'min': 0.0, 'max': 1.0},
        'initial_weight_mean': {'type': float, 'min': 0.0, 'max': 1.0},
        'initial_weight_std': {'type': float, 'min': 0.0, 'max': 1.0},
        'v_thresh': {'type': float, 'min': -100.0, 'max': 0.0},
        'v_rest': {'type': float, 'min': -100.0, 'max': 0.0},
        'v_reset': {'type': float, 'min': -100.0, 'max': 0.0},
        'tau_mem': {'type': float, 'min': 1.0, 'max': 1000.0},
        'tau_ref': {'type': float, 'min': 0.0, 'max': 100.0}
    },
    'plasticity': {
        'stdp': {
            'enable': {'type': bool},
            'learning_rate': {'type': float, 'min': 0.0, 'max': 1.0},
            'tau_plus': {'type': float, 'min': 1.0, 'max': 1000.0},
            'tau_minus': {'type': float, 'min': 1.0, 'max': 1000.0}
        },
        'hebbian': {
            'enable': {'type': bool},
            'learning_rate': {'type': float, 'min': 0.0, 'max': 1.0}
        },
        'structural': {
            'enable': {'type': bool},
            'synapogenesis_rate': {'type': float, 'min': 0.0, 'max': 0.1},
            'pruning_rate': {'type': float, 'min': 0.0, 'max': 0.1}
        }
    }
    # ... more sections
}
```

### Validation Rules

```python
def validate_configuration(config):
    """Validate configuration against schema"""
    
    errors = []
    warnings = []
    
    # Check required sections
    required_sections = ['brain', 'plasticity', 'neuromodulation', 'development']
    for section in required_sections:
        if section not in config:
            errors.append(f"Missing required section: {section}")
    
    # Validate brain configuration
    if 'brain' in config:
        brain = config['brain']
        
        # Check neuron count
        if 'neuron_count' in brain:
            if not isinstance(brain['neuron_count'], int):
                errors.append("brain.neuron_count must be an integer")
            elif brain['neuron_count'] < 1 or brain['neuron_count'] > 1000000:
                warnings.append("brain.neuron_count outside recommended range (1-1,000,000)")
        
        # Check for consistency
        if 'neuron_count' in brain and 'synapse_density' in brain:
            expected_synapses = brain['neuron_count'] * brain['synapse_density']
            if expected_synapses > 10000000:  # Warn for very large networks
                warnings.append(f"Large network ({expected_synapses:.0f} synapses) may impact performance")
    
    return {
        'valid': len(errors) == 0,
        'errors': errors,
        'warnings': warnings
    }
```

## Configuration Examples

### Example 1: Micro-scale Simulation

```python
import pynlm

# Create micro-scale configuration for rapid prototyping
config = pynlm.createDefaultConfig()

# Small network for testing
config.set("brain.neuron_count", 100)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("plasticity.hebbian.enable", true)

# Short simulation for quick testing
config.set("simulation.steps", 100)
config.set("simulation.dt", 0.5)

# Enable development for growth studies
config.set("development.enabled", true)
config.set("development.critical_period.enabled", true)

print("Micro-scale configuration created")
```

### Example 2: Large-scale Performance Test

```python
import pynlm

# Create large-scale configuration for performance testing
config = pynlm.createDefaultConfig()

# Large network for performance testing
config.set("brain.neuron_count", 10000)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", true)
config.set("plasticity.structural.enable", true)

# Extended simulation for stability testing
config.set("simulation.steps", 5000)
config.set("simulation.dt", 0.01)
config.set("simulation.max_time", 1000.0)

# Enable all neuromodulation systems
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", true)
config.set("neuromod.novelty.enable", true)
config.set("neuromod.prediction_error.enable", true)

# Enable full development system
config.set("development.enabled", true)
config.set("development.critical_period.enabled", true)
config.set("development.critical_period.start_age", 10.0)
config.set("development.critical_period.end_age", 100.0)

# Set world for multi-agent simulation
config.set("world.width", 100)
config.set("world.height", 100)
config.set("world.vision_width", 16)
config.set("world.vision_height", 16)

print("Large-scale performance configuration created")
```

### Example 3: Research Configuration

```python
import pynlm

# Create research configuration for neuroscience studies
config = pynlm.createDefaultConfig()

# Precise parameters for research
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.08)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("plasticity.hebbian.learning_rate", 0.02)

# Advanced neuromodulation for research
config.set("neuromod.dopamine.scale", 0.8)
config.set("neuromod.dopamine.noise_level", 0.05)
config.set("neuromod.curiosity.sensitivity", 0.7)
config.set("neuromod.curiosity.exploration_drive", 0.6)
config.set("neuromod.novelty.sensitivity", 0.6)

# Detailed development for research
config.set("development.enabled", true)
config.set("development.critical_period.enabled", true)
config.set("development.critical_period.plasticity_boost", 3.0)
config.set("development.maturation.synapse_pruning_rate", 0.02)

# Extended recording for analysis
config.set("simulation.steps", 10000)
config.set("simulation.dt", 0.05)
config.set("simulation.enable_logging", true)
config.set("simulation.log_frequency", 100)

# Advanced analysis configuration
config.set("advanced_api.brain_profiler.enable", true)
config.set("advanced_api.brain_profiler.sampling_rate", 5)
config.set("advanced_api.network_analyzer.enable", true)
config.set("advanced_api.memory_analyzer.enable", true)

print("Research configuration created")
```

## Configuration Summary

### Configuration Types

| Type | Description | Use Case |
|------|-------------|----------|
| **Default Configuration** | Sensible defaults for most use cases | Quick testing, prototyping |
| **Custom Configuration** | User-defined parameters | Specific research needs |
| **Performance Configuration** | Optimized for performance | Large-scale simulations |
| **Research Configuration** | Detailed parameters for studies | Neuroscience research |

### Configuration Management

```python
# Configuration management utilities

class ConfigurationManager:
    def __init__(self):
        self.configurations = {}
        self.default_config = None
    
    def save_configuration(self, name, config):
        """Save configuration to registry"""
        self.configurations[name] = config.copy()
    
    def load_configuration(self, name):
        """Load configuration from registry"""
        return self.configurations.get(name)
    
    def create_configuration(self, template="default"):
        """Create new configuration from template"""
        if template == "default":
            return pynlm.createDefaultConfig()
        elif template == "micro":
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 100)
            config.set("simulation.steps", 100)
            return config
        elif template == "large":
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 10000)
            config.set("simulation.steps", 5000)
            return config
        else:
            raise ValueError(f"Unknown template: {template}")
    
    def compare_configurations(self, config1, config2):
        """Compare two configurations"""
        differences = []
        
        # Find keys in config1 not in config2
        for key in config1.keys():
            if key not in config2:
                differences.append(f"Key missing in config2: {key}")
            elif config1[key] != config2[key]:
                differences.append(f"Difference in {key}: {config1[key]} vs {config2[key]}")
        
        # Find keys in config2 not in config1
        for key in config2.keys():
            if key not in config1:
                differences.append(f"Key missing in config1: {key}")
        
        return differences
```

### Configuration Best Practices

1. **Start Simple**: Use default configuration for initial testing
2. **Document Changes**: Keep track of configuration modifications
3. **Validate**: Always validate configuration before use
4. **Backup**: Save important configurations to files
5. **Version Control**: Track configuration changes in version control
6. **Test**: Test configurations with different scales
7. **Profile**: Profile performance with different configurations

---

**Last Updated:** 2026-10-13
**Status:** Production Ready
**License:** Apache 2.0

This configuration reference provides **comprehensive documentation** for NLM configuration options, validation rules, and best practices for **effective configuration management** in NLM simulations.

---

**Key Features:**
- ✅ **Complete configuration reference** with all parameters
- ✅ **Validation rules** for configuration correctness
- ✅ **File format examples** (JSON and YAML)
- ✅ **Research configurations** for different use cases
- ✅ **Configuration management** utilities
- ✅ **Best practices** for effective configuration

**Next Steps:**
- Use `createDefaultConfig()` for quick starts
- Load configurations from files for complex setups
- Validate configurations before running simulations
- Profile performance with different configurations
- Document configuration changes for reproducibility

For more information, see `NLM_IMPROVEMENTS_COMPLETE.md` for configuration implementation details or `USAGE_GUIDE.md` for configuration usage examples.