# NLM Configuration Documentation

## Overview

The Neural Learning Machine (NLM) uses a flexible configuration system to control all aspects of brain simulation, from basic parameters like neuron count to advanced features like neuromodulation and development. Configuration is loaded from files or command-line arguments and stored in memory for use throughout simulation.

Configuration values can be specified in two ways:

1. **Configuration files** (`.cfg` format): Key=value pairs with optional comments
2. **Command-line arguments**: `--key=value` or `-k value` format

## Configuration File Format

### Basic Syntax
```
# Comment
key = value

# String values must be quoted
quoted_string = "example value"

# Boolean values (case-insensitive)
enabled = true
disabled = false

# Numbers (int, float, scientific notation)
neuron_count = 1000
learning_rate = 0.01
connection_prob = 0.15

# Comments start with # or /
# This is a comment
/ Another comment style
```

### Configuration Categories

Configuration options are grouped by functionality:

- `brain.*` - Core brain parameters
- `plasticity.*` - Learning and synaptic changes
- `neuromod.*` - Neuromodulatory systems
- `development.*` - Developmental processes
- `environment.*` - Simulation environment
- `logging.*` - Log output and debugging
- `performance.*` - Performance optimizations

## Brain Configuration

### Core Brain Parameters

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|------------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons in the brain | 100-10000 |
| `brain.region_count` | int | 2 | Number of neural regions | 1-10 |
| `brain.synapse_density` | float | 0.1 | Probability of connection between neurons | 0.01-0.5 |
| `brain.connection_probability` | float | 0.05 | Alternative to synapse_density | 0.01-0.5 |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight | 0.1-1.0 |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights | 0.01-0.3 |

### Neuron Parameters

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|------------------|
| `brain.v_thresh` | float | -50.0 | Firing threshold (mV) | -60.0 to -40.0 |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) | -65.0 to -75.0 |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) | -80.0 to -70.0 |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) | 10.0-50.0 |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) | 1.0-5.0 |

## Plasticity Configuration

### STDP (Spike-Timing-Dependent Plasticity)

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|------------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP | true/false |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate | 0.0001-0.01 |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP LTP time constant (ms) | 10.0-50.0 |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP LTD time constant (ms) | 10.0-50.0 |
| `plasticity.stdp.ltp_threshold` | float | 0.0 | Minimum weight change for LTP | -0.1 to 0.1 |
| `plasticity.stdp.ltd_threshold` | float | 0.0 | Minimum weight change for LTD | -0.1 to 0.1 |

### Hebbian Learning

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|------------------|
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning | true/false |
| `plasticity.hebbian.learning_rate` | float | 0.01 | Hebbian learning rate | 0.001-0.1 |
| `plasticity.hebbian.threshold` | float | 0.5 | Activity threshold for Hebbian | 0.0-1.0 |

### Structural Plasticity

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|------------------|
| `plasticity.structural.enable` | bool | true | Enable structural plasticity | true/false |
| `plasticity.structural.synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation | 0.00001-0.001 |
| `plasticity.structural.pruning_rate` | float | 0.00001 | Rate of synapse elimination | 0.000001-0.0001 |
| `plasticity.structural.max_synapses_per_neuron` | int | 100 | Maximum outgoing synapses | 10-500 |

## Neuromodulation Configuration

### Dopamine System

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|------------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation strength | 0.1-5.0 |
| `neuromod.dopamine.baseline` | float | 0.1 | Baseline dopamine level | 0.0-1.0 |
| `neuromod.dopamine.noise` | float | 0.05 | Dopamine noise level | 0.0-0.2 |

### Curiosity System

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|------------------|
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration | true/false |
| `neuromod.curiosity.exploration_rate` | float | 0.1 | Baseline exploration rate | 0.01-0.5 |
| `neuromod.curiosity.surprise_threshold` | float | 0.5 | Surprise detection threshold | 0.1-1.0 |

### Novelty Detection

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|------------------|
| `neuromod.novelty.enable` | bool | true | Enable novelty detection | true/false |
| `neuromod.novelty.sensitivity` | float | 0.5 | Novelty detection sensitivity | 0.1-1.0 |
| `neuromod.novelty.memory_length` | int | 100 | How long to remember novel events | 10-500 |

### Prediction Error

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|------------------|
| `neuromod.prediction_error.enable` | bool | true | Enable prediction error signals | true/false |
| `neuromod.prediction_error.learning_rate` | float | 0.01 | Prediction error learning rate | 0.001-0.1 |

## Development Configuration

### Developmental Stages

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|------------------|
| `development.stage` | enum | "adult" | Current developmental stage | "initial", "critical", "maturation", "adult", "aging" |
| `development.plasticity_decay_rate` | float | 0.01 | How quickly plasticity decreases with age | 0.001-0.05 |
| `development.synapse_stability` | float | 0.95 | Synapse stability factor | 0.8-1.0 |
| `development.excitability_modulation` | float | 1.0 | Age-related excitability changes | 0.5-2.0 |

### Synaptogenesis

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|------------------|
| `development.synaptogenesis.rate` | float | 0.001 | Rate of new synapse formation | 0.0001-0.01 |
| `development.synaptogenesis.neurogenesis_rate` | float | 0.0001 | Rate of new neuron creation | 0.00001-0.001 |

### Pruning

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|------------------|
| `development.pruning.rate` | float | 0.0001 | Rate of synapse elimination | 0.00001-0.001 |
| `development.pruning.activity_threshold` | float | 0.1 | Activity threshold for pruning | 0.01-0.5 |

## Environment Configuration

### World Simulation

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|------------------|
| `environment.name` | string | "GridWorld" | Name of environment simulation | |
| `environment.width` | int | 10 | World width (cells/units) | 5-50 |
| `environment.height` | int | 10 | World height (cells/units) | 5-50 |
| `environment.vision_width` | int | 8 | Agent visual field width | 4-16 |
| `environment.vision_height` | int | 8 | Agent visual field height | 4-16 |
| `environment.max_energy` | float | 100.0 | Maximum energy for agents | 10.0-500.0 |

### Resource Management

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|------------------|
| `environment.resource_generation_rate` | float | 0.1 | How quickly resources regenerate | 0.01-1.0 |
| `environment.resource_decay_rate` | float | 0.01 | How quickly resources decay | 0.001-0.1 |
| `environment.hazard_frequency` | float | 0.05 | How often hazards appear | 0.01-0.2 |

## Logging Configuration

### Log Levels

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `logging.level` | enum | "INFO" | Minimum log level to output |
| `logging.log_to_file` | bool | false | Whether to log to file |
| `logging.log_filename` | string | "nlm.log" | Log file name |
| `logging.max_file_size` | int | 10485760 | Maximum log file size (bytes) |
| `logging.max_files` | int | 5 | Maximum number of log files to keep |

### Performance Logging

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `logging.performance.enabled` | bool | true | Enable performance measurements |
| `logging.performance.frequency` | int | 100 | How often to measure performance (steps) |
| `logging.performance.samples` | int | 10 | Number of samples to average |

## Performance Configuration

### Memory Management

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|------------------|
| `performance.memory_pool_size` | int | 1000 | Size of memory pools | 500-5000 |
| `performance.event_queue_size` | int | 1000 | Event queue capacity | 500-5000 |
| `performance.vectorization_enabled` | bool | true | Enable SIMD vectorization | true/false |

### Parallel Processing

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|------------------|
| `performance.parallel.neural_regions` | bool | true | Process neural regions in parallel | true/false |
| `performance.parallel.synapse_updates` | bool | true | Process synapse updates in parallel | true/false |
| `performance.parallel.spike_propagation` | bool | true | Process spike propagation in parallel | true/false |

## Advanced Configuration Options

### Experimental Settings

```
# Advanced learning parameters
plasticity.stdp.use_weight_dependence = true
plasticity.stdp.weight_dependence_factor = 0.1
plasticity.hebbian.use_correlation = true
plasticity.hebbian.correlation_threshold = 0.3

# Neuromodulation interaction
neuromod.dopamine.affects_plasticity = true
neuromod.curiosity.drives_exploration = true
neuromod.novelty.triggers_attention = true

# Development interactions
development.plasticity_affects_learning = true
development.structural_plasticity_enabled = true

# Environment complexity
environment.resource_complexity = 0.5
environment.hazard_intensity = 0.2
environment.dynamic_elements = true

# Performance tuning
performance.memory_pool_size = 2000
performance.event_queue_size = 2000
performance.thread_count = 4
```

## Configuration Examples

### Minimal Configuration
```
# Basic brain setup
brain.neuron_count = 500
brain.region_count = 1
brain.connection_probability = 0.1

# Enable basic learning
plasticity.stdp.enable = true
plasticity.hebbian.enable = true

# Basic environment
environment.width = 20
environment.height = 20
```

### Complex Learning Environment
```
# Large, complex brain
brain.neuron_count = 5000
brain.region_count = 4
brain.synapse_density = 0.2

# Advanced plasticity
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.stdp.tau_plus = 30.0
plasticity.stdp.tau_minus = 25.0

plasticity.hebbian.enable = true
plasticity.hebbian.learning_rate = 0.05

plasticity.structural.enable = true
plasticity.structural.synaptogenesis_rate = 0.001
plasticity.structural.pruning_rate = 0.0005

# Full neuromodulation
neuromod.dopamine.scale = 2.0
neuromod.curiosity.enable = true
neuromod.curiosity.exploration_rate = 0.2
neuromod.novelty.enable = true

# Development
development.stage = "critical"
development.plasticity_decay_rate = 0.02

# Complex environment
environment.name = "ComplexWorld"
environment.width = 50
environment.height = 50
environment.vision_width = 12
environment.vision_height = 12

# Performance tuning
performance.memory_pool_size = 5000
performance.event_queue_size = 5000
performance.parallel.neural_regions = true
performance.parallel.synapse_updates = true
performance.parallel.spike_propagation = true

# Comprehensive logging
logging.level = "DEBUG"
logging.log_to_file = true
logging.log_filename = "nlm_detailed.log"
```

## Configuration Commands

### Loading Configuration

```bash
# Load configuration from file
cd /path/to/nlm
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
./nlm --config=configs/experiment1.cfg

# Override specific parameters
./nlm --config=configs/default.cfg \
      --brain.neuron_count=2000 \
      --plasticity.stdp.learning_rate=0.005

# Use command-line only configuration
./nlm --brain.neuron_count=100 \
      --environment.width=10 \
      --environment.height=10
```

### Saving Configuration

```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("neuromod.curiosity.enable", True)

# Save to file
config.saveToFile("my_experiment_config.cfg")
print("Configuration saved to my_experiment_config.cfg")
```

## Configuration Validation

The configuration system includes validation to prevent invalid parameter combinations:

### Common Validation Errors

1. **Incompatible Plasticity Settings**
   - STDP and Hebbian both disabled
   - Learning rate too high (causes instability)

2. **Unrealistic Environment Settings**
   - Vision field larger than world
   - Invalid world dimensions

3. **Performance Issues**
   - Memory pool too small for configured size
   - Too many parallel regions for available cores

### Configuration Validation Tools

```python
import pynlm

# Create configuration and validate
config = pynlm.createDefaultConfig()

# Validate configuration
if config.isValid():
    print("Configuration is valid")
else:
    print("Configuration errors:")
    for error in config.getErrors():
        print(f"  - {error}")

# Get configuration summary
print(config.summary())
```

## Configuration Troubleshooting

### Common Issues and Solutions

#### Problem: Simulation too slow
**Symptoms**: Low FPS, long computation times
**Solutions**:
1. Increase `performance.parallel.neural_regions` (if multi-core)
2. Increase `performance.memory_pool_size` 
3. Reduce `brain.neuron_count` or `brain.synapse_density`
4. Disable `neuromod.novelty.enable` if novelty detection is expensive

#### Problem: Learning not occurring
**Symptoms**: No change in synaptic weights, no behavioral change
**Solutions**:
1. Check `plasticity.stdp.enable = true`
2. Verify `plasticity.hebbian.enable = true`
3. Ensure reward signals are being generated
4. Check if agents are receiving sensory input

#### Problem: Memory usage too high
**Symptoms**: System swap, crashes with large configurations
**Solutions**:
1. Decrease `brain.neuron_count`
2. Reduce `performance.memory_pool_size`
3. Disable `performance.vectorization_enabled`
4. Reduce `environment.width` and `environment.height`

#### Problem: Configuration file not loading
**Symptoms**: "Using default configuration" message
**Solutions**:
1. Verify file path: `--config=/path/to/config.cfg`
2. Check file permissions
3. Ensure file is readable
4. Validate file format (key=value syntax)

### Debugging Configuration Issues

```bash
# Run with debug logging
./nlm --config=configs/debug_config.cfg --log_level=DEBUG

# Save configuration for inspection
./nlm --config=configs/large_config.cfg --save_config=loaded_config.cfg

# Test configuration with simple validation
./nlm --test_config --brain.neuron_count=10 --environment.width=5
```

## Configuration Best Practices

### 1. Start Simple
```
# For initial development/debugging
brain.neuron_count = 100
brain.region_count = 1
environment.width = 10
environment.height = 10
```

### 2. Incrementally Increase Complexity
```
# Add learning gradually
brain.neuron_count = 1000
plasticity.stdp.enable = true
plasticity.hebbian.enable = true
environment.width = 20
environment.height = 20
```

### 3. Use Descriptive Configuration Names
```
# Experiment naming convention
experiment_name = "phase1_learning_basic"
config_filename = "config_phase1_learning_basic.cfg"
```

### 4. Document Your Configuration
```
# Add comments explaining your choices
# Using 500 neurons for quick testing
brain.neuron_count = 500

# Testing STDP with moderate learning rate
plasticity.stdp.learning_rate = 0.001

# Environment optimized for agent navigation
environment.width = 15
environment.height = 15
environment.vision_width = 7
environment.vision_height = 7
```

### 5. Save and Share Configurations
```python
# Create experimental configuration
experiment_config = {
    "brain.neuron_count": 2000,
    "plasticity.stdp.learning_rate": 0.005,
    "neuromod.curiosity.enable": True,
    "environment.width": 30,
    "environment.height": 30,
}

# Save for reproduction
config = pynlm.createDefaultConfig()
for key, value in experiment_config.items():
    config.set(key, value)

config.saveToFile("experiment_reproduction.cfg")
print("Saved experiment configuration to experiment_reproduction.cfg")
```

## Advanced Configuration Features

### Configuration Templates

```bash
# Create configuration templates for common experiments
templates/
├── basic_learning.cfg
├── complex_behavior.cfg
├── developmental.cfg
└── social_learning.cfg
```

### Configuration Inheritance

Configuration can inherit from base configurations:

```
# base_config.cfg
brain.neuron_count = 1000
environment.width = 20

# experiment_config.cfg (inherits base_config.cfg)
# brain.neuron_count = 2000  # Override base
plasticity.stdp.enable = true
```

### Dynamic Configuration

Parameters can be changed during simulation:

```python
# During simulation
config.set("environment.width", 25)  # Change environment mid-simulation
config.set("neuromod.curiosity.exploration_rate", 0.3)  # Adjust exploration
```

## Conclusion

The NLM configuration system provides comprehensive control over brain simulation parameters while maintaining flexibility for experimentation and research. By following these best practices and understanding the configuration options, you can create effective experiments and simulations tailored to your research needs.

Key takeaways:

1. **Start simple and scale up**: Begin with minimal configurations and add complexity as needed
2. **Document your choices**: Use comments to explain configuration decisions
3. **Save configurations**: Document your experiments for reproducibility
4. **Validate configurations**: Check for common issues before running simulations
5. **Use environment-specific settings**: Tailor configuration to your experimental goals

The configuration system is designed to support both rapid prototyping and sophisticated research experiments, providing the flexibility needed for advancing computational neuroscience research.

---
*This documentation is continuously updated based on user feedback and new configuration options.*