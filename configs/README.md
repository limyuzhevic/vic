# NLM Configuration Files

This directory contains configuration files for the NLM (Neural Learning Machine) project.

## Configuration File Format

NLM uses a simple key=value format for configuration files. Comments start with `#` or `/`. Values can be strings, numbers, booleans, or arrays.

### Basic Example
```
# NLM Configuration
simulation_timestep = 0.001
random_seed = 42
neuron_count = 1000
connection_probability = 0.1
reward_modulation_enabled = true
```

### Array Examples
```
# Array values
int_array = [1, 2, 3, 4, 5]
double_array = [1.0, 2.5, 3.14, -0.5]
string_array = ["value1", "value2", "value3"]
```

## Available Configuration Files

### 1. default.cfg
The default configuration file used by NLM.

**Purpose**: Baseline configuration for standard NLM operation.
**Use Case**: General testing and development.

**Key Features**:
- Standard simulation timestep (0.001s)
- Moderate neuron count (1000)
- Balanced plasticity parameters
- Full neuromodulation enabled

### 2. minimal.cfg
Minimal configuration for quick testing.

**Purpose**: Lightweight configuration for rapid prototyping.
**Use Case**: Quick verification of basic functionality.

```
# Minimal NLM Configuration
simulation_timestep = 0.001
random_seed = 42
neuron_count = 100
region_count = 1
connection_probability = 0.05
reward_modulation_enabled = false
structural_plasticity_enabled = false
development_enabled = false
curiosity_enabled = false
```

### 3. research.cfg
Configuration optimized for research experiments.

**Purpose**: Configuration suitable for scientific research and long-term studies.
**Use Case**: Publishing reproducible research results.

```
# Research Configuration
simulation_timestep = 0.0005
random_seed = 12345
neuron_count = 5000
region_count = 3
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.018
synaptogenesis_rate = 0.0002
pruning_rate = 0.00005
working_memory_capacity = 500
episodic_memory_max_episodes = 5000
episodic_memory_consolidation_interval = 2000
episodic_memory_replay_interval = 500
checkpoint_dir = ./research_checkpoints
checkpoint_max_save_count = 50000
checkpoint_enabled = true
```

### 4. real_time.cfg
Configuration optimized for real-time applications.

**Purpose**: Configuration suitable for real-time or interactive applications.
**Use Case**: Live demos and interactive simulations.

```
# Real-time Configuration
simulation_timestep = 0.0001
random_seed = 999
neuron_count = 200
region_count = 1
connection_probability = 0.2
reward_modulation_enabled = true
structural_plasticity_enabled = true
development_enabled = true
curiosity_enabled = true
working_memory_capacity = 50
episodic_memory_max_episodes = 200
checkpoint_enabled = false
experiment_duration = 3600.0
```

### 5. learning.cfg
Configuration optimized for learning and training.

**Purpose**: Configuration suitable for intensive learning experiments.
**Use Case**: Studying learning dynamics and neural adaptation.

```
# Learning Configuration
simulation_timestep = 0.0005
random_seed = 777
neuron_count = 3000
region_count = 2
connection_probability = 0.12
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.023
synaptogenesis_rate = 0.00015
pruning_rate = 0.00008
working_memory_capacity = 300
episodic_memory_max_episodes = 3000
episodic_memory_consolidation_interval = 1000
episodic_memory_replay_interval = 300
checkpoint_dir = ./learning_checkpoints
checkpoint_max_save_count = 20000
checkpoint_enabled = true
```

## Configuration Loading Priority

1. **Command Line Arguments** (Highest priority)
   ```bash
   ./nlm --neuron_count=5000 --connection_probability=0.2
   ```

2. **Configuration File** (Medium priority)
   ```bash
   ./nlm --config my_config.cfg
   ```

3. **Default Values** (Lowest priority)

## Configuration Parameter Documentation

### Basic Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation_timestep` | double | 0.001 | Time step for simulation (seconds) |
| `random_seed` | uint64_t | 42 | Seed for random number generator |
| `neuron_count` | size_t | 1000 | Number of neurons in the brain |
| `region_count` | size_t | 1 | Number of neural regions |
| `connection_probability` | float | 0.1 | Probability of connections between neurons |

### Plasticity Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `stdp_ltp_weight` | float | 0.01 | STDP Long-Term Potentiation weight |
| `stdp_ltd_weight` | float | 0.012 | STDP Long-Term Depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant (milliseconds) |
| `synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `pruning_rate` | float | 0.00001 | Rate of synapse pruning |

### Memory System Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `working_memory_capacity` | size_t | 100 | Capacity of working memory |
| `episodic_memory_max_episodes` | size_t | 1000 | Maximum number of episodes in episodic memory |
| `episodic_memory_consolidation_interval` | size_t | 1000 | Interval for memory consolidation (steps) |
| `episodic_memory_replay_interval` | size_t | 100 | Interval for memory replay (steps) |

### Cognitive Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `planner_planning_depth` | size_t | 5 | Planning depth for neural planner |
| `attention_inhibition_strength` | float | 0.5 | Strength of inhibitory connections in attention |
| `attention_excitation_strength` | float | 1.5 | Strength of excitatory connections in attention |

### Neuromodulation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `novelty_decay_rate` | float | 0.99 | Decay rate for novelty signals |
| `reward_modulation_enabled` | bool | true | Enable/disable reward modulation |
| `structural_plasticity_enabled` | bool | true | Enable/disable structural plasticity |
| `development_enabled` | bool | true | Enable/disable development system |
| `curiosity_enabled` | bool | true | Enable/disable curiosity-based exploration |

### Development Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development_initial_age` | double | 0.0 | Initial age of the agent |
| `development_critical_period_start` | double | 60.0 | Start of critical period (simulation steps) |
| `development_critical_period_end` | double | 300.0 | End of critical period (simulation steps) |

### Checkpoint Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `checkpoint_dir` | string | "./checkpoints" | Directory for checkpoint files |
| `checkpoint_max_save_count` | size_t | 10000 | Maximum number of checkpoints to save |
| `checkpoint_max_file_size` | size_t | 5242880 | Maximum file size (5MB) for checkpoints |
| `checkpoint_enabled` | bool | true | Enable/disable checkpointing |

### Agent Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `agent_reward_threshold` | float | 0.1 | Threshold for reward-based actions |
| `agent_novelty_threshold` | float | 0.3 | Threshold for novelty-based exploration |
| `agent_curiosity_threshold` | float | 0.5 | Threshold for curiosity-based exploration |

### Experiment Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `experiment_duration` | double | 60.0 | Duration of experiment (simulation seconds) |
| `experiment_seed` | uint64_t | 42 | Seed for experiment randomization |

## Configuration File Examples

### Simple Configuration
```
# Simple NLM Configuration
simulation_timestep = 0.001
random_seed = 42
neuron_count = 100
connection_probability = 0.05
```

### Complex Configuration
```
# Complex NLM Configuration
simulation_timestep = 0.0005
random_seed = 12345
neuron_count = 5000
region_count = 3
connection_probability = 0.15

# Plasticity
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.023
stdp_tau = 15.0
synaptogenesis_rate = 0.0002
pruning_rate = 0.00008

# Memory
working_memory_capacity = 500
episodic_memory_max_episodes = 5000
episodic_memory_consolidation_interval = 2000
episodic_memory_replay_interval = 500

# Neuromodulation
novelty_decay_rate = 0.95
reward_modulation_enabled = true
structural_plasticity_enabled = true
development_enabled = true
curiosity_enabled = true

# Checkpoints
checkpoint_dir = ./research_checkpoints
checkpoint_max_save_count = 50000
checkpoint_enabled = true
```

## Usage Instructions

### Loading Configuration

#### Command Line
```bash
# Use custom configuration file
./nlm --config my_config.cfg

# Override specific values
./nlm --neuron_count=2000 --connection_probability=0.2

# Generate sample configuration
./nlm --generate_config > sample_config.cfg
```

#### Programmatic
```cpp
#include "Config.hpp"

auto config = std::make_shared<Config>();
config->loadFromFile("configs/experiment.cfg");
config->set("neuron_count", 5000);
```

### Creating New Configuration Files

1. **Copy an existing example** and modify as needed
2. **Use the config file syntax** shown above
3. **Validate** the configuration before use
4. **Test** with a small simulation first

### Configuration Validation

```bash
# Validate configuration syntax
./nlm --validate_config configs/default.cfg

# Check configuration summary
./nlm --config_summary configs/default.cfg
```

## Configuration Tips

1. **Use meaningful names** for configuration parameters
2. **Add comments** to explain parameter purposes
3. **Document your experiments** with configuration files
4. **Use version control** for configuration files
5. **Test different configurations** to understand parameter effects

## Configuration Troubleshooting

### Common Issues

- **File not found**: Ensure the configuration file exists in the specified path
- **Invalid syntax**: Check for proper key=value format
- **Type mismatch**: Values must match expected types
- **Missing required parameters**: Ensure all required parameters are set

### Validation Commands

```bash
# Generate sample config
./nlm --generate_config > sample_config.cfg

# Validate config
./nlm --validate_config sample_config.cfg

# Save current config
./nlm --save_config current.cfg
```

## Configuration Best Practices

1. **Keep configuration simple** and readable
2. **Use descriptive parameter names**
3. **Document configuration parameters**
4. **Test configuration before running experiments**
5. **Use version control** for configuration files
6. **Backup important configurations**

## Configuration File Index

- `default.cfg`: Standard configuration for general use
- `minimal.cfg`: Lightweight configuration for quick testing
- `research.cfg`: Configuration optimized for research experiments
- `real_time.cfg`: Configuration optimized for real-time applications
- `learning.cfg`: Configuration optimized for learning experiments

## Conclusion

Effective configuration management is crucial for reproducible and reliable NLM experiments. Use the provided configuration files as templates and adapt them to your specific experimental requirements.