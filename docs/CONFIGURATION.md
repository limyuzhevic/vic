# NLM Configuration Schema Documentation
# Complete reference for all configuration parameters in NLM

## Overview
This document provides a comprehensive reference for all configuration parameters available in the NLM (Neural Learning Machine) brain simulation framework. NLM uses a flexible, hierarchical configuration system that allows fine-tuned control over brain behavior, learning processes, and simulation parameters.

## Configuration File Format

NLM configuration files are simple key-value text files with the following format:

```
# Comments start with #
key = value
type_key.subkey = value
```

Configuration values can be:
- Integers (e.g., `neuron_count = 1000`)
- Floating-point numbers (e.g., `plasticity_rate = 0.01`)
- Booleans (e.g., `enabled = true`)
- Strings (e.g., `environment_name = "GridWorld"`)

## Configuration Categories

Configuration parameters are organized into the following categories:

1. **Brain Configuration** - Core brain parameters
2. **Plasticity Configuration** - Learning and synaptic modification rules
3. **Development Configuration** - Brain development and maturation
4. **Neuromodulation Configuration** - Reward, curiosity, and other signals
5. **Memory Configuration** - Working, episodic, and other memory systems
6. **Prediction Configuration** - Prediction and error signals
7. **Cognition Configuration** - Planning, attention, and concept formation
8. **Sensory/Motor Configuration** - Input and output processing
9. **Environment Configuration** - World and simulation environment
10. **Simulation Configuration** - General simulation settings
11. **Logging Configuration** - Debug and output management

## Brain Configuration

### Core Brain Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | integer | 1000 | Total number of neurons in the brain |
| `brain.region_count` | integer | 2 | Number of brain regions |
| `brain.synapse_density` | float | 0.1 | Probability of synapse formation between neurons |
| `brain.connection_probability` | float | 0.05 | Base probability for random connections |
| `brain.initial_weight_mean` | float | 0.5 | Mean value for initial synaptic weights |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation for initial synaptic weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |
| `brain.simulation_timestep` | float | 0.001 | Simulation timestep (s) |
| `brain.max_simulation_steps` | integer | 10000 | Maximum number of simulation steps |

### Neuron Types and Populations

Neuron types are organized into different populations with specific characteristics:

```
brain.neuron_types = ["Excitatory", "Inhibitory", "Sensory", "Motor", "Modulatory", "Internal"]
brain.population_distribution = {
    "cortical": 70,
    "sensory_cortex": 15,
    "motor_cortex": 10,
    "thalamus": 5
}
```

## Plasticity Configuration

### Spike-Timing-Dependent Plasticity (STDP)

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | boolean | true | Enable STDP mechanism |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant for potentiation (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant for depression (ms) |
| `plasticity.stdp.max_weight_change` | float | 0.1 | Maximum weight change per event |
| `plasticity.stdp.window_size` | float | 50.0 | STDP temporal window size (ms) |

### Hebbian Plasticity

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.hebbian.enable` | boolean | true | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.005 | Hebbian learning rate |
| `plasticity.hebbian.threshold` | float | 0.1 | Co-activity threshold for LTP |
| `plasticity.hebbian.max_weight` | float | 1.0 | Maximum synaptic weight |
| `plasticity.hebbian.min_weight` | float | 0.0 | Minimum synaptic weight |

### Structural Plasticity

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.structural.enable` | boolean | true | Enable structural plasticity (synaptogenesis/pruning) |
| `plasticity.structural.synaptogenesis_rate` | float | 0.001 | Rate of new synapse formation |
| `plasticity.structural.pruning_rate` | float | 0.0001 | Rate of synapse elimination |
| `plasticity.structural.stability_threshold` | float | 0.05 | Weight stability threshold for pruning |
| `plasticity.structural.max_synapses_per_neuron` | integer | 100 | Maximum synapses per neuron |

## Development Configuration

### Developmental System

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.system.enabled` | boolean | true | Enable developmental system |
| `development.critical_period.active` | boolean | true | Enable critical period plasticity |
| `development.critical_period.end_age` | float | 30.0 | Age when critical period ends (simulation steps) |
| `development.plastic_window` | float | 100.0 | Developmental window for plasticity (steps) |
| `development.maturation_rate` | float | 0.01 | Rate of brain maturation |
| `development.pruning_intensity` | float | 0.5 | Intensity of developmental pruning |
| `development.synaptogenesis_burst` | boolean | true | Enable developmental synaptogenesis bursts |

### Age-Related Plasticity

```
development.age_dependency = {
    "initial": {"plasticity_multiplier": 1.0, "learning_rate": 0.1},
    "critical": {"plasticity_multiplier": 2.0, "learning_rate": 0.5},
    "adult": {"plasticity_multiplier": 1.0, "learning_rate": 0.01},
    "aging": {"plasticity_multiplier": 0.5, "learning_rate": 0.001}
}
```

## Neuromodulation Configuration

### Dopamine System

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.dopamine.baseline` | float | 0.1 | Baseline dopamine level |
| `neuromod.dopamine.response_gain` | float | 1.0 | Dopamine response gain |
| `neuromod.dopamine.decay_rate` | float | 0.01 | Dopamine decay rate |

### Curiosity System

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.curiosity.enable` | boolean | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.sensitivity` | float | 0.5 | Curiosity sensitivity to novelty |
| `neuromod.curiosity.exploration_bonus` | float | 0.1 | Bonus for novel experiences |
| `neuromod.curiosity.decay_rate` | float | 0.05 | Curiosity decay rate |

### Novelty Detection

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.novelty.enable` | boolean | true | Enable novelty detection |
| `neuromod.novelty.threshold` | float | 0.7 | Novelty detection threshold |
| `neuromod.novelty.smoothing` | float | 0.9 | Novelty signal smoothing factor |

### Prediction Error

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.prediction_error.enable` | boolean | true | Enable prediction error signaling |
| `neuromod.prediction_error.weight` | float | 1.0 | Prediction error signal weight |
| `neuromod.prediction_error.decay` | float | 0.01 | Prediction error decay rate |

## Memory Configuration

### Working Memory

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working_memory.capacity` | integer | 100 | Capacity of working memory in items |
| `memory.working_memory.duration` | float | 5.0 | Duration of memory items (seconds) |
| `memory.working_memory.decay_rate` | float | 0.1 | Decay rate of working memory items |
| `memory.working_memory.competition` | boolean | true | Enable competition between items |
| `memory.working_memory.encoding_strength` | float | 0.8 | Strength of encoding process |

### Episodic Memory

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.episodic_memory.capacity` | integer | 1000 | Maximum number of episodes |
| `memory.episodic_memory.consolidation_rate` | float | 0.01 | Rate of memory consolidation |
| `memory.episodic_memory.reliability` | float | 0.9 | Reliability of episodic memory |
| `memory.episodic_memory.elaboration` | boolean | true | Enable memory elaboration during recall |
| `memory.episodic_memory.association_strength` | float | 0.7 | Strength of associative links |

### Semantic Memory

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.semantic_memory.learning_rate` | float | 0.001 | Rate of semantic knowledge acquisition |
| `memory.semantic_memory.association_threshold` | float | 0.3 | Threshold for forming associations |
| `memory.semantic_memory.spread` | float | 0.5 | Spread of semantic activation |

## Prediction Configuration

### Prediction System

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `prediction.system.enable` | boolean | true | Enable prediction system |
| `prediction.system.horizon` | integer | 10 | Prediction time horizon (steps) |
| `prediction.system.uncertainty_weight` | float | 0.5 | Weight for prediction uncertainty |
| `prediction.system.error_gain` | float | 1.0 | Prediction error gain |

### Neural Prediction

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `prediction.neural.learning_rate` | float | 0.001 | Neural prediction learning rate |
| `prediction.neural.temporal_decay` | float | 0.9 | Temporal decay for predictions |
| `prediction.neural.attention_weight` | float | 0.5 | Attention weight for predictions |

## Cognition Configuration

### Neural Planning

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `cognition.planning.horizon` | integer | 20 | Planning horizon (steps) |
| `cognition.planning.exploration_weight` | float | 0.3 | Weight for exploration in planning |
| `cognition.planning.exploitation_weight` | float | 0.7 | Weight for exploitation in planning |
| `cognition.planning.confidence_threshold` | float | 0.5 | Confidence threshold for action selection |

### Concept Formation

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `cognition.concept.formation_rate` | float | 0.001 | Rate of new concept formation |
| `cognition.concept.stability` | float | 0.8 | Concept stability threshold |
| `cognition.concept.association_threshold` | float | 0.2 | Threshold for concept associations |
| `cognition.concept.spread` | float | 0.6 | Concept activation spread |

### Attentional Selection

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `cognition.attention.focus_strength` | float | 0.7 | Strength of attentional focus |
| `cognition.attention.saliency_weight` | float | 0.5 | Weight for saliency in attention |
| `cognition.attention.selection_threshold` | float | 0.3 | Threshold for item selection |
| `cognition.attention.decay_rate` | float | 0.1 | Decay rate of attention |

## Sensory/Motor Configuration

### Vision

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `sensory.vision.width` | integer | 8 | Vision field width |
| `sensory.vision.height` | integer | 8 | Vision field height |
| `sensory.vision.sensitivity` | float | 1.0 | Vision sensitivity |
| `sensory.vision.noise_level` | float | 0.01 | Visual noise level |
| `sensory.vision.color_coding` | boolean | true | Enable color coding |

### Audio

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `sensory.audio.channels` | integer | 1 | Number of audio channels |
| `sensory.audio.sample_rate` | integer | 44100 | Audio sample rate |
| `sensory.audio.sensitivity` | float | 1.0 | Audio sensitivity |

### Motor Control

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `motor.action_set_size` | integer | 6 | Number of possible actions |
| `motor.action_threshold` | float | 0.5 | Action selection threshold |
| `motor.movement_smoothness` | float | 0.8 | Smoothness of movements |
| `motor.energy_cost` | float | 1.0 | Energy cost per action |

## Environment Configuration

### World Environment

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `environment.name` | string | "GridWorld" | Environment name |
| `environment.width` | integer | 10 | World width |
| `environment.height` | integer | 10 | World height |
| `environment.cell_size` | float | 1.0 | Size of each world cell |
| `environment.obstacle_density` | float | 0.2 | Density of obstacles |
| `environment.reward_density` | float | 0.1 | Density of reward items |

### Simulation Environment

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation.time_limit` | float | 0.0 | Maximum simulation time (0 = unlimited) |
| `simulation.random_seed` | integer | 42 | Random seed for reproducibility |
| `simulation.checkpoint_interval` | integer | 1000 | Checkpoint interval (steps) |
| `simulation.replay_enabled` | boolean | false | Enable replay capabilities |
| `simulation.max_history` | integer | 1000 | Maximum simulation history size |

## Logging Configuration

### Log Levels

| Level | Numeric Value | Description |
|-------|---------------|-------------|
| `OFF` | 0 | No logging |
| `ERROR` | 1 | Error messages only |
| `WARNING` | 2 | Warnings and errors |
| `INFO` | 3 | Information messages (default) |
| `DEBUG` | 4 | Debug messages |
| `TRACE` | 5 | Trace messages (detailed) |

### Log Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `log.level` | string | "INFO" | Global logging level |
| `log.to_file` | boolean | false | Enable file logging |
| `log.filename` | string | "nlm.log" | Log file name |
| `log.max_size` | integer | 10485760 | Maximum log file size (bytes) |
| `log.max_files` | integer | 5 | Maximum number of log files |
| `log.format` | string | "%timestamp %level %message" | Log message format |
| `log.flush_interval` | float | 1.0 | Log flush interval (seconds) |

## Advanced Configuration Patterns

### Configuration Inheritance

Configurations can inherit from base configurations:

```
base_config = {
    "brain.neuron_count": 1000,
    "plasticity.stdp.enable": true,
    "log.level": "INFO"
}

learning_config = {
    "extends": "base_config",
    "brain.neuron_count": 2000,
    "neuromod.curiosity.enable": true,
    "plasticity.structural.enable": true
}
```

### Environment-Specific Configurations

```
# Development configuration
development_config = {
    "log.level": "DEBUG",
    "simulation.max_simulation_steps": 100000,
    "plasticity.stdp.learning_rate": 0.01,
    "development.system.enabled": true
}

# Production configuration
production_config = {
    "log.level": "INFO",
    "simulation.max_simulation_steps": 1000000,
    "plasticity.stdp.learning_rate": 0.001,
    "development.system.enabled": false,
    "log.to_file": true
}

# Research configuration
research_config = {
    "log.level": "TRACE",
    "simulation.max_simulation_steps": 5000000,
    "plasticity.stdp.learning_rate": 0.1,
    "plasticity.hebbian.enable": true,
    "neuromod.curiosity.enable": true,
    "neuromod.novelty.enable": true,
    "memory.episodic_memory.capacity": 10000
}
```

### Dynamic Configuration

Configurations can be modified at runtime:

```cpp
// Modify configuration during simulation
config->set("brain.synapse_density", 0.2);
config->set("neuromod.curiosity.sensitivity", 0.8);

// Get current configuration values
int neuron_count = config->getOr<int64_t>("brain.neuron_count", 1000);
double learning_rate = config->getOr<double>("plasticity.stdp.learning_rate", 0.001);

// Configuration validation
if (config->validate()) {
    // Configuration is valid, proceed with simulation
} else {
    // Handle invalid configuration
}
```

## Configuration Validation

NLM validates configurations for consistency and correctness:

### Required Parameters
- `brain.neuron_count` must be positive integer
- `brain.simulation_timestep` must be positive float
- `plasticity.stdp.learning_rate` must be non-negative
- `neuromod.curiosity.sensitivity` must be between 0 and 1

### Compatibility Checks
- Plasticity and development settings must be compatible
- Memory capacities must not exceed available resources
- Neuron and region counts must be within supported ranges

### Warnings
Configuration may issue warnings for:
- Unused configuration parameters
- Potentially conflicting parameter combinations
- Performance implications of certain settings

## Configuration Examples

### Minimal Configuration

```
# Minimal working configuration
brain.neuron_count = 100
brain.region_count = 1
simulation.timestep = 0.001
log.level = "INFO"
```

### High-Performance Configuration

```
# High-performance configuration for research
brain.neuron_count = 10000
brain.synapse_density = 0.2
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.01
plasticity.hebbian.enable = true
plasticity.structural.enable = true
neuromod.curiosity.enable = true
neuromod.novelty.enable = true
neuromod.prediction_error.enable = true
development.system.enabled = true
memory.working_memory.capacity = 500
memory.episodic_memory.capacity = 5000
simulation.max_simulation_steps = 100000
log.level = "INFO"
log.to_file = true
```

### Learning-Focused Configuration

```
# Configuration optimized for learning and memory
brain.neuron_count = 2000
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.005
plasticity.stdp.tau_plus = 25.0
plasticity.stdp.tau_minus = 25.0
plasticity.hebbian.enable = true
plasticity.hebbian.learning_rate = 0.001
neuromod.curiosity.enable = true
neuromod.curiosity.sensitivity = 0.7
neuromod.novelty.enable = true
neuromod.novelty.threshold = 0.8
memory.working_memory.capacity = 200
memory.episodic_memory.capacity = 2000
memory.episodic_memory.consolidation_rate = 0.02
cognition.concept.formation_rate = 0.001
cognition.attention.focus_strength = 0.8
```

## Configuration File Templates

### Template Files

NLM provides template configuration files for different use cases:

1. `configs/default.cfg` - Basic configuration
2. `configs/learning.cfg` - Learning-focused configuration
3. `configs/development.cfg` - Development-oriented configuration
4. `configs/experiment.cfg` - Research experiment configuration
5. `configs/debug.cfg` - Debug configuration with verbose logging

### Template Variables

Configuration files can include variable substitutions:

```
# Example with variables
brain.neuron_count = ${BASE_NEURONS:-1000}
plasticity.stdp.learning_rate = ${LEARNING_RATE:-0.001}
simulation.max_simulation_steps = ${MAX_STEPS:-10000}
```

Variables can be set via command line or environment variables.

## Configuration Management Tools

### Command Line Tools

NLM provides command line tools for configuration management:

```bash
# List available configuration files
nlm_cli --list-configs

# Show configuration help
nlm_cli --help

# Show version information
nlm_cli --version

# Run with custom configuration
nlm_cli --config my_config.cfg --steps 5000 --neurons 2000

# Run with modified parameters
nlm_cli --config default.cfg --param brain.synapse_density=0.15 --param plasticity.stdp.learning_rate=0.01
```

### Python Configuration API

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.01)

# Save configuration
config.saveToFile("my_config.cfg")

# Load configuration
config2 = pynlm.createBrainConfig()
config2.loadFromFile("my_config.cfg")

# Compare configurations
config3 = pynlm.createBrainConfig()
if config.equals(config3):
    print("Configurations are identical")
else {
    print("Configurations differ")
}
```

## Configuration Best Practices

### Performance Optimization
1. Use minimal configuration for production environments
2. Disable unnecessary features to improve performance
3. Adjust memory settings based on available resources
4. Use checkpointing for long-running simulations
5. Monitor simulation performance and adjust accordingly

### Reproducibility
1. Always specify random seeds for reproducible results
2. Document configuration settings used in experiments
3. Use configuration files to share experimental setups
4. Version control configuration files

### Experimentation
1. Use separate configuration files for different experimental conditions
2. Parameterize configuration values for systematic experiments
3. Use configuration inheritance to avoid duplication
4. Document configuration choices and their rationale

### Debugging
1. Use debug configurations with verbose logging
2. Enable detailed logging for troubleshooting
3. Use checkpointing to recover from failures
4. Monitor system resources during long simulations

## Configuration Validation Scripts

NLM provides scripts for validating and testing configurations:

```bash
# Validate configuration
nlm_validate_config configs/my_config.cfg

# Test configuration with short simulation
nlm_test_config configs/my_config.cfg --steps 100

# Profile configuration performance
nlm_profile_config configs/my_config.cfg

# Compare configurations
nlm_compare_configs config1.cfg config2.cfg
```

## Conclusion

The NLM configuration system provides flexible, hierarchical control over brain simulation parameters. By understanding the configuration schema and following best practices, users can:

1. Create optimized configurations for different use cases
2. Systematically explore parameter spaces
3. Reproduce experimental results
4. Debug and troubleshoot simulation issues
5. Scale simulations from small demonstrations to large research experiments

The configuration system is designed to be both powerful and accessible, with sensible defaults for beginners while providing advanced options for expert users and researchers.

---

For more information on specific parameters or advanced usage, refer to the individual component documentation or the NLM Architecture Guide.
