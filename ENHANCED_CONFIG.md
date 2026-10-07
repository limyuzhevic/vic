# Enhanced NLM Configuration System

## Overview

This document describes the enhanced configuration system for NLM (Neural Learning Machine). The system provides comprehensive configuration options for all aspects of neural simulation, including brain parameters, plasticity rules, memory systems, neuromodulation, and experimental controls.

## Configuration Categories

### 1. Brain Configuration
Core parameters defining neural network structure and basic dynamics.

### 2. Plasticity Configuration
Parameters controlling learning mechanisms (STDP, Hebbian, structural).

### 3. Neuromodulation Configuration
Parameters for reward, curiosity, novelty, and prediction error systems.

### 4. Memory System Configuration
Parameters for working memory, episodic memory, and associative memory.

### 5. Cognitive Configuration
Parameters for attention, planning, and concept formation.

### 6. Development Configuration
Parameters for developmental stages and aging.

### 7. Experiment Configuration
Parameters for simulation control and experimental design.

### 8. Visualization and Debugging
Parameters for output and monitoring.

## Configuration Structure

The configuration system is organized hierarchically with nested namespaces:

```
brain:
  neuron_count: 1000
  region_count: 1
  connection_probability: 0.1
  neuron_parameters:
    resting_potential: -70.0
    threshold: -55.0
    time_constant: 20.0

plasticity:
  stdp:
    ltp_weight: 0.01
    ltd_weight: 0.012
    time_constant: 20.0
  hebbian:
    learning_rate: 0.001
  structural:
    synaptogenesis_rate: 0.0001
    pruning_rate: 0.00001

memory:
  working_memory:
    capacity: 100
    decay_rate: 0.01
  episodic_memory:
    max_episodes: 1000
    consolidation_interval: 100

neuromodulation:
  dopamine:
    scale: 1.0
    plasticity_factor: 1.0
  curiosity:
    enable: true
    weight_novelty: 0.5
    weight_prediction_error: 0.5
  novelty:
    enable: true
    detection_threshold: 0.01

cognition:
  attention:
    inhibition_strength: 0.5
    excitation_strength: 1.5
    competition_threshold: 0.3
  planner:
    planning_depth: 5
    action_selection_threshold: 0.5

environment:
  world_size: 20
  vision_range: 8
  energy_decay: 0.01
```

## Advanced Configuration Features

### 1. Dynamic Configuration
- **Runtime modification**: Change configuration during simulation
- **Conditional configuration**: Configure based on simulation state
- **Profile-based configuration**: Load different configurations for different experimental conditions

### 2. Validation and Constraints
- **Type validation**: Ensure configuration values are of correct type
- **Range validation**: Check that values are within acceptable ranges
- **Dependency validation**: Ensure required parameters are set
- **Consistency validation**: Check for contradictory settings

### 3. Configuration Groups
- **Standard configurations**: Pre-defined configurations for common use cases
- **Research configurations**: Configurations optimized for specific research questions
- **Performance configurations**: Configurations optimized for speed vs accuracy
- **Custom configurations**: User-defined configurations

### 4. Configuration Templates
- **Configuration templates**: Reusable configuration components
- **Configuration inheritance**: Build configurations by extending templates
- **Configuration merging**: Merge multiple configuration sources

### 5. Advanced Data Types
- **Vector parameters**: Configure lists of parameters (e.g., synaptic weights)
- **Map parameters**: Configure key-value pairs (e.g., region properties)
- **JSON/YAML support**: Load/save complex nested configurations
- **Schema validation**: Validate configuration against JSON Schema

## Configuration API

### Python API Examples

#### Basic Configuration
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Set brain parameters
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 2)
config.set("brain.connection_probability", 0.15)

# Set plasticity parameters
config.set("plasticity.stdp.ltp_weight", 0.02)
config.set("plasticity.stdp.ltd_weight", 0.018)
config.set("plasticity.hebbian.learning_rate", 0.002)

# Set memory parameters
config.set("memory.working_memory.capacity", 500)
config.set("memory.episodic_memory.max_episodes", 5000)

# Set neuromodulation parameters
config.set("neuromodulation.curiosity.enable", True)
config.set("neuromodulation.curiosity.weight_novelty", 0.7)
config.set("neuromodulation.curiosity.weight_prediction_error", 0.3)

# Set cognitive parameters
config.set("cognition.attention.inhibition_strength", 0.7)
config.set("cognition.planner.planning_depth", 10)

# Save configuration
config.saveToFile("advanced_config.json")
```

#### Advanced Configuration
```python
# Create experimental configuration
config = pynlm.createDefaultConfig()

# Set up for developmental experiment
config.set("brain.development.enabled", True)
config.set("brain.development.initial_plasticity", 1.0)
config.set("brain.development.critical_period_start", 100)
config.set("brain.development.critical_period_end", 500)
config.set("brain.development.adult_plasticity", 0.1)

# Set up for social learning experiment
config.set("brain.social_learning.enabled", True)
config.set("brain.social_learning.observer_neurons", 100)
config.set("brain.social_learning.imitation_strength", 0.8)

# Set up for reward-based learning
config.set("neuromodulation.reward.enable", True)
config.set("neuromodulation.reward.scale", 2.0)
config.set("neuromodulation.reward.learning_rate", 0.05)

# Set up for attention and memory
config.set("cognition.attention.top_down_bias", True)
config.set("memory.episodic.consolidation.enabled", True)
config.set("memory.episodic.replay.interval", 50)

# Set up for visualization
config.set("visualization.mode", "web")
config.set("visualization.update_rate", 60.0)
config.set("visualization.export_format", "png")

# Run simulation with advanced configuration
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(10000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: Firing rate = {brain.getAverageFiringRate():.3f}")
```

#### Configuration Validation
```python
def validate_configuration(config):
    """Validate configuration for consistency and correctness."""
    
    # Check brain parameters
    neuron_count = config.getOr("brain.neuron_count", 1000)
    if neuron_count < 10 or neuron_count > 100000:
        raise ValueError(f"Invalid neuron count: {neuron_count}")
    
    # Check plasticity parameters
    stdp_ltp = config.getOr("plasticity.stdp.ltp_weight", 0.01)
    stdp_ltd = config.getOr("plasticity.stdp.ltd_weight", 0.012)
    if stdp_ltp <= 0 or stdp_ltd <= 0:
        raise ValueError("STDP weights must be positive")
    
    # Check memory parameters
    wm_capacity = config.getOr("memory.working_memory.capacity", 100)
    em_episodes = config.getOr("memory.episodic_memory.max_episodes", 1000)
    if wm_capacity < em_episodes / 10:
        raise ValueError("Working memory capacity too small for episodic memory")
    
    # Check development parameters
    if config.has("brain.development.enabled"):
        if config.getOr("brain.development.critical_period_start", 0) > \
           config.getOr("brain.development.critical_period_end", 1000):
            raise ValueError("Critical period start must be before end")
    
    return True

# Validate configuration
validate_configuration(config)
```

#### Configuration Comparison
```python
def compare_configurations(config1, config2):
    """Compare two configurations and identify differences."""
    
    keys1 = set(config1.getKeys())
    keys2 = set(config2.getKeys())
    
    all_keys = keys1.union(keys2)
    differences = []
    
    for key in all_keys:
        val1 = config1.getOr(key, None)
        val2 = config2.getOr(key, None)
        
        if val1 != val2:
            differences.append((key, val1, val2))
    
    return differences

# Compare configurations
diffs = compare_configurations(config1, config2)
if diffs:
    print("Configuration differences:")
    for key, val1, val2 in diffs:
        print(f"  {key}: {val1} -> {val2}")
```

#### Configuration Templates
```python
# Define configuration template
NEURAL_NETWORK_TEMPLATE = {
    "brain.neuron_count": 1000,
    "brain.region_count": 1,
    "brain.connection_probability": 0.1,
    "plasticity.stdp.ltp_weight": 0.01,
    "plasticity.stdp.ltd_weight": 0.012,
    "memory.working_memory.capacity": 100,
    "memory.episodic_memory.max_episodes": 1000,
}

# Create configuration from template
config = pynlm.createDefaultConfig()
for key, value in NEURAL_NETWORK_TEMPLATE.items():
    config.set(key, value)

# Override specific parameters for experiment
config.set("brain.neuron_count", 2000)
config.set("neuromodulation.curiosity.enable", True)
```

#### Dynamic Configuration
```python
def run_experiment_with_dynamic_config(config_template, experiment_params):
    """Run experiment with dynamically modified configuration."""
    
    config = pynlm.createDefaultConfig()
    
    # Load template
    for key, value in config_template.items():
        config.set(key, value)
    
    # Modify based on experiment parameters
    config.set("brain.neuron_count", experiment_params.get("neuron_count", 1000))
    config.set("plasticity.stdp.ltp_weight", experiment_params.get("stdp_ltp", 0.01))
    config.set("neuromodulation.curiosity.weight_novelty", experiment_params.get("curiosity_weight", 0.5))
    
    # Create brain with dynamic configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run experiment
    results = []
    for step in range(experiment_params.get("steps", 1000)):
        brain.step(step)
        
        # Log results
        results.append({
            "step": step,
            "firing_rate": brain.getAverageFiringRate(),
            "total_spikes": brain.getTotalSpikeCount(),
            "active_neurons": brain.getActiveNeuronCount()
        })
    
    return results

# Run experiment with different parameters
results1 = run_experiment_with_dynamic_config(
    config_template=NEURAL_NETWORK_TEMPLATE,
    experiment_params={"neuron_count": 1500, "stdp_ltp": 0.02, "steps": 500}
)

results2 = run_experiment_with_dynamic_config(
    config_template=NEURAL_NETWORK_TEMPLATE,
    experiment_params={"neuron_count": 2000, "stdp_ltp": 0.015, "steps": 1000}
)
```

## Configuration Categories and Parameters

### Brain Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.region_count` | int | 1 | Number of neural regions |
| `brain.connection_probability` | float | 0.1 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP potentiation time constant |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP depression time constant |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.001 | Hebbian learning rate |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.synaptogenesis_rate` | float | 0.0001 | Synaptogenesis rate |
| `plasticity.structural.pruning_rate` | float | 0.00001 | Pruning rate |

### Neuromodulation Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.prediction.error_threshold` | float | 0.01 | Prediction error threshold |

### Memory Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working_memory.capacity` | int | 100 | Working memory capacity |
| `memory.working_memory.decay_rate` | float | 0.01 | Working memory decay rate |
| `memory.episodic_memory.max_episodes` | int | 1000 | Maximum number of episodic memories |
| `memory.episodic_memory.consolidation_interval` | int | 100 | Memory consolidation interval |
| `memory.associative_similarity_threshold` | float | 0.7 | Associative memory similarity threshold |

### Cognitive Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `cognition.attention.inhibition_strength` | float | 0.5 | Attention inhibition strength |
| `cognition.attention.excitation_strength` | float | 1.5 | Attention excitation strength |
| `cognition.attention.competition_threshold` | float | 0.3 | Attention competition threshold |
| `cognition.planner.planning_depth` | int | 5 | Planning depth |
| `cognition.planner.action_selection_threshold` | float | 0.5 | Action selection threshold |
| `cognition.concept.formation_threshold` | float | 0.8 | Concept formation threshold |

### Development Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.enabled` | bool | true | Enable development system |
| `development.initial_plasticity` | float | 1.0 | Initial plasticity factor |
| `development.critical_period_start` | int | 100 | Critical period start (step) |
| `development.critical_period_end` | int | 1000 | Critical period end (step) |
| `development.adult_plasticity` | float | 0.2 | Adult plasticity factor |
| `development.aging_rate` | float | 0.01 | Aging rate |

### Environment Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `environment.world_width` | int | 20 | World width |
| `environment.world_height` | int | 20 | World height |
| `environment.vision_width` | int | 8 | Vision width |
| `environment.vision_height` | int | 8 | Vision height |
| `environment.energy_decay` | float | 0.01 | Energy decay rate |
| `environment.max_energy` | float | 100.0 | Maximum energy |

### Visualization Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `visualization.mode` | string | "console" | Visualization mode (console, web, file) |
| `visualization.update_rate` | float | 30.0 | Update rate (Hz) |
| `visualization.export_format` | string | "png" | Export format (png, json, csv) |
| `visualization.show_network_topology` | bool | true | Show network topology |
| `visualization.show_activity_heatmap` | bool | true | Show activity heatmap |
| `visualization.show_spike_raster` | bool | true | Show spike raster plot |
| `visualization.show_weight_matrix` | bool | true | Show weight matrix |

### Experimental Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `experiment.max_steps` | int | 1000 | Maximum simulation steps |
| `experiment.save_interval` | int | 100 | Save interval |
| `experiment.checkpoint_interval` | int | 1000 | Checkpoint interval |
| `experiment.reward_function` | string | "standard" | Reward function type |
| `experiment.termination_condition` | string | "max_steps" | Termination condition |

## Configuration Files

### Default Configuration
- `configs/default.cfg` - Basic configuration
- `configs/experiment.cfg` - Experimental configuration
- `configs/research.cfg` - Research configuration

### Configuration Templates
- `templates/standard.json` - Standard configuration template
- `templates/research.json` - Research configuration template
- `templates/performance.json` - Performance configuration template

## Configuration Validation

### Validation Rules
1. **Type validation**: Ensure parameter types are correct
2. **Range validation**: Ensure parameter values are within acceptable ranges
3. **Dependency validation**: Ensure dependent parameters are consistent
4. **Compatibility validation**: Ensure configuration is compatible with simulation

### Validation Examples
```python
def validate_config(config):
    # Validate brain parameters
    assert config.getOr("brain.neuron_count", 1000) > 0
    assert config.getOr("brain.region_count", 1) > 0
    assert config.getOr("brain.connection_probability", 0.1) >= 0.0
    assert config.getOr("brain.connection_probability", 0.1) <= 1.0
    
    # Validate plasticity parameters
    assert config.getOr("plasticity.stdp.ltp_weight", 0.01) > 0.0
    assert config.getOr("plasticity.stdp.ltd_weight", 0.012) > 0.0
    assert config.getOr("plasticity.hebbian.learning_rate", 0.001) > 0.0
    
    # Validate memory parameters
    assert config.getOr("memory.working_memory.capacity", 100) > 0
    assert config.getOr("memory.episodic_memory.max_episodes", 1000) > 0
    
    return True
```

## Configuration Serialization

### Saving Configuration
```python
# Save configuration to file
config.saveToFile("experiment_config.cfg")
config.saveToFile("experiment_config.json")

# Save configuration to string
config_str = config.summary()
```

### Loading Configuration
```python
# Load configuration from file
config.loadFromFile("experiment_config.cfg")
config.loadFromFile("experiment_config.json")

# Merge configurations
config1.loadFromFile("base_config.cfg")
config2.loadFromFile("experiment_config.cfg")
# config2 merges with config1, command line overrides file
```

## Configuration Usage Tips

### 1. Use Descriptive Keys
```python
# Good - descriptive keys
config.set("brain.neural_populations.excitatory.count", 800)
config.set("brain.neural_populations.inhibitory.count", 200)

# Bad - generic keys
config.set("param1", 800)
config.set("param2", 200)
```

### 2. Use Consistent Naming
```python
# Use consistent naming convention
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.ltp_weight", 0.01)
config.set("memory.working_memory.capacity", 100)
```

### 3. Group Related Parameters
```python
# Group brain parameters
brain_config = {
    "brain.neuron_count": 1000,
    "brain.region_count": 1,
    "brain.connection_probability": 0.1,
}

# Group plasticity parameters
plasticity_config = {
    "plasticity.stdp.ltp_weight": 0.01,
    "plasticity.stdp.ltd_weight": 0.012,
    "plasticity.hebbian.learning_rate": 0.001,
}
```

### 4. Use Configuration Templates
```python
# Define configuration template
STANDARD_CONFIG = {
    "brain.neuron_count": 1000,
    "brain.region_count": 1,
    "brain.connection_probability": 0.1,
    "plasticity.stdp.ltp_weight": 0.01,
    "plasticity.stdp.ltd_weight": 0.012,
    "memory.working_memory.capacity": 100,
    "memory.episodic_memory.max_episodes": 1000,
}

# Apply template
for key, value in STANDARD_CONFIG.items():
    config.set(key, value)
```

### 5. Validate Configuration
```python
# Validate configuration before use
if validate_config(config):
    brain = pynlm.createBrain(config)
    brain.initialize()
    # Run simulation
else:
    print("Configuration validation failed!")
```

## Configuration Best Practices

### 1. Document Configuration
```python
"""Configuration for neural network simulation."""

CONFIG_DESCRIPTIONS = {
    "brain.neuron_count": "Total number of neurons in the network",
    "brain.region_count": "Number of neural regions",
    "brain.connection_probability": "Probability of connection between neurons",
    "plasticity.stdp.ltp_weight": "STDP potentiation weight",
    "plasticity.stdp.ltd_weight": "STDP depression weight",
    "memory.working_memory.capacity": "Working memory capacity",
}
```

### 2. Use Meaningful Variable Names
```python
# Use meaningful variable names
neuron_count = config.getOr("brain.neuron_count", 1000)
connection_prob = config.getOr("brain.connection_probability", 0.1)
learning_rate = config.getOr("plasticity.stdp.learning_rate", 0.001)
```

### 3. Use Constants for Magic Numbers
```python
# Use constants for magic numbers
DEFAULT_NEURON_COUNT = 1000
DEFAULT_CONNECTION_PROBABILITY = 0.1
DEFAULT_LEARNING_RATE = 0.001

config.set("brain.neuron_count", DEFAULT_NEURON_COUNT)
config.set("brain.connection_probability", DEFAULT_CONNECTION_PROBABILITY)
config.set("plasticity.stdp.learning_rate", DEFAULT_LEARNING_RATE)
```

### 4. Use Configuration Validation
```python
# Use configuration validation
config_validator = ConfigurationValidator()
if not config_validator.validate(config):
    print("Configuration validation failed!")
    for error in config_validator.get_errors():
        print(f"Error: {error}")
```

## Configuration Migration

### Migration Examples
```python
# Migrate from old configuration format
def migrate_old_config(old_config):
    """Migrate old configuration to new format."""
    
    new_config = pynlm.createDefaultConfig()
    
    # Migrate brain parameters
    if old_config.has("neuron_count"):
        new_config.set("brain.neuron_count", old_config.getOr("neuron_count", 1000))
    
    if old_config.has("region_count"):
        new_config.set("brain.region_count", old_config.getOr("region_count", 1))
    
    # Migrate plasticity parameters
    if old_config.has("stdp_ltp"):
        new_config.set("plasticity.stdp.ltp_weight", old_config.getOr("stdp_ltp", 0.01))
    
    if old_config.has("stdp_ltd"):
        new_config.set("plasticity.stdp.ltd_weight", old_config.getOr("stdp_ltd", 0.012))
    
    return new_config
```

## Future Enhancements

### 1. Advanced Validation
- Schema validation using JSON Schema
- Machine learning-based validation
- Automated configuration optimization

### 2. Configuration AI
- AI-powered configuration recommendations
- Automatic configuration optimization
- Automated configuration validation

### 3. Configuration Sharing
- Configuration sharing between researchers
- Configuration versioning
- Configuration comparison tools

### 4. Configuration Documentation
- Automatic configuration documentation generation
- Configuration examples
- Configuration tutorials

## Conclusion

The enhanced configuration system provides comprehensive configuration options for all aspects of NLM simulation. It supports:

1. **Hierarchical configuration**: Nested configuration keys
2. **Validation and constraints**: Type, range, and dependency validation
3. **Configuration groups**: Pre-defined configuration groups
4. **Configuration templates**: Reusable configuration templates
5. **Dynamic configuration**: Runtime configuration modification
6. **Advanced data types**: Vector and map parameters
7. **Serialization**: Load/save configuration
8. **Best practices**: Documentation, validation, and migration

This configuration system enables researchers and developers to:
- Configure complex neural simulations with ease
- Validate configurations for correctness
- Share configurations between experiments
- Automate configuration generation
- Optimize configurations for specific research questions

The enhanced configuration system significantly improves the usability and flexibility of NLM, making it more accessible to researchers and developers while maintaining backward compatibility.