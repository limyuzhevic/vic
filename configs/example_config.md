# NLM Configuration File Examples

This file provides examples of configuration files for NLM simulations.
These JSON-based configuration files allow you to specify parameters
for NLM brains and customize simulation behavior.

## Configuration File Format

NLM configuration files are JSON format with the following structure:

```json
{
    "nlm_version": "0.1.0",
    "config": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.15,
        "simulation_timestep": 0.001,
        "random_seed": 42,
        "enable_plasticity": true,
        "enable_neuromodulation": true,
        "enable_development": true,
        "world_width": 64,
        "world_height": 64
    }
}
```

## Configuration Examples

### 1. Basic Neural Simulation

`basic_simulation.cfg`

```json
{
    "nlm_version": "0.1.0",
    "config": {
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.1,
        "simulation_timestep": 0.001,
        "random_seed": 123,
        "enable_plasticity": true,
        "enable_neuromodulation": true,
        "enable_development": false,
        "enable_curiosity": false,
        "world_width": 0,
        "world_height": 0
    }
}
```

**Purpose:** Simple neural simulation for testing basic functionality
**Use case:** Initial development, debugging, basic research
**Neuron count:** 500 neurons for manageable simulation size
**World:** No world (pure neural computation)
**Plasticity:** Enabled for learning demonstration
**Development:** Disabled for stable baseline

### 2. World Interaction Simulation

`world_interaction.cfg`

```json
{
    "nlm_version": "0.1.0",
    "config": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.15,
        "simulation_timestep": 0.001,
        "random_seed": 456,
        "enable_plasticity": true,
        "enable_neuromodulation": true,
        "enable_development": true,
        "enable_curiosity": true,
        "world_width": 64,
        "world_height": 64,
        "vision_width": 16,
        "vision_height": 16
    }
}
```

**Purpose:** Agent learning and behavior in a virtual environment
**Use case:** Behavioral studies, action selection, reinforcement learning
**Neuron count:** 1000 neurons for complex behavior
**World:** 64x64 world with sensory input capabilities
**Plasticity:** Enabled for adaptation to environment
**Development:** Enabled for behavioral maturation
**Curiosity:** Enabled for exploration motivation

### 3. Learning and Development Simulation

`learning_development.cfg`

```json
{
    "nlm_version": "0.1.0",
    "config": {
        "neuron_count": 2000,
        "region_count": 3,
        "connection_probability": 0.2,
        "simulation_timestep": 0.0005,
        "random_seed": 789,
        "enable_plasticity": true,
        "enable_neuromodulation": true,
        "enable_development": true,
        "enable_curiosity": true,
        "world_width": 128,
        "world_height": 128,
        "vision_width": 32,
        "vision_height": 32,
        "working_memory_capacity": 200,
        "episodic_memory_capacity": 1000,
        "prediction_horizon": 3
    }
}
```

**Purpose:** Comprehensive learning with memory and prediction systems
**Use case:** Cognitive development, long-term learning, complex tasks
**Neuron count:** 2000 neurons for rich computational capacity
**World:** Larger 128x128 world for extended exploration
**Plasticity:** Strong (0.2 connection probability)
**Development:** Enabled with developmental stages
**Curiosity:** Enabled for active exploration
**Memory:** Large working and episodic memory capacities
**Prediction:** Multi-step temporal prediction

### 4. High-Performance Research Configuration

`research.cfg`

```json
{
    "nlm_version": "0.1.0",
    "config": {
        "neuron_count": 5000,
        "region_count": 5,
        "connection_probability": 0.25,
        "simulation_timestep": 0.0002,
        "random_seed": 999,
        "enable_plasticity": true,
        "enable_neuromodulation": true,
        "enable_development": true,
        "enable_curiosity": true,
        "world_width": 256,
        "world_height": 256,
        "vision_width": 64,
        "vision_height": 64,
        "working_memory_capacity": 500,
        "episodic_memory_capacity": 5000,
        "prediction_horizon": 5,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "hebbian_learning_rate": 0.02,
        "synaptogenesis_rate": 0.0005,
        "pruning_rate": 0.0001
    }
}
```

**Purpose:** Large-scale neural network research and experimentation
**Use case:** Publishing research, benchmarking, large-scale simulations
**Neuron count:** 5000 neurons for substantial network effects
**World:** Large 256x256 world for extensive exploration space
**Plasticity:** Fine-tuned parameters for research control
**Development:** Full developmental progression
**Curiosity:** Strong exploration drive
**Memory:** Large-scale memory systems
**Prediction:** Extended temporal horizon
**Plasticity weights:** Specific values for reproducible research

### 5. Developmental Psychology Configuration

`developmental.cfg`

```json
{
    "nlm_version": "0.1.0",
    "config": {
        "neuron_count": 800,
        "region_count": 3,
        "connection_probability": 0.18,
        "simulation_timestep": 0.001,
        "random_seed": 111,
        "enable_plasticity": true,
        "enable_neuromodulation": true,
        "enable_development": true,
        "enable_curiosity": true,
        "world_width": 100,
        "world_height": 100,
        "vision_width": 25,
        "vision_height": 25,
        "working_memory_capacity": 150,
        "episodic_memory_capacity": 1200,
        "prediction_horizon": 2,
        "critical_period_enabled": true,
        "critical_period_start": 100,
        "critical_period_end": 1000
    }
}
```

**Purpose:** Modeling developmental psychology and critical periods
**Use case:** Cognitive development studies, educational research
**Neuron count:** 800 neurons for developmentally plausible scale
**World:** Medium-sized world (100x100)
**Development:** Enhanced with critical period modeling
**Critical periods:** Specific developmental windows
**Memory:** Substantial capacity for experience accumulation
**Prediction:** Short-term prediction suitable for developmental stage

### 6. Memory-Focused Configuration

`memory_focused.cfg`

```json
{
    "nlm_version": "0.1.0",
    "config": {
        "neuron_count": 1200,
        "region_count": 4,
        "connection_probability": 0.12,
        "simulation_timestep": 0.001,
        "random_seed": 222,
        "enable_plasticity": true,
        "enable_neuromodulation": true,
        "enable_development": true,
        "enable_curiosity": true,
        "world_width": 80,
        "world_height": 80,
        "vision_width": 20,
        "vision_height": 20,
        "working_memory_capacity": 300,
        "episodic_memory_capacity": 2500,
        "associative_memory_capacity": 1000,
        "prediction_horizon": 4,
        "memory_replay_enabled": true,
        "replay_frequency": 0.1
    }
}
```

**Purpose:** Emphasis on memory systems and consolidation
**Use case:** Memory research, amnesia studies, consolidation experiments
**Neuron count:** 1200 neurons with strong memory emphasis
**World:** Medium world (80x80) for memory-rich experiences
**Memory:** Enhanced all memory systems (working, episodic, associative)
**Replay:** Memory consolidation through replay mechanisms
**Prediction:** Longer temporal horizon for memory integration

## Configuration Parameters Reference

### Core Simulation Parameters

| Parameter | Type | Default | Description | Typical Range |
|-----------|------|---------|-------------|---------------|
| `neuron_count` | int | 100 | Total number of neurons | 100-10000 |
| `region_count` | int | 1 | Number of neural regions | 1-10 |
| `connection_probability` | float | 0.15 | Probability of connections | 0.05-0.5 |
| `simulation_timestep` | float | 0.001 | Simulation time step | 0.0001-0.01 |

### Plasticity Parameters

| Parameter | Type | Default | Description | Typical Range |
|-----------|------|---------|-------------|---------------|
| `stdp_ltp_weight` | float | 0.02 | STDP LTP weight | 0.01-0.05 |
| `stdp_ltd_weight` | float | 0.015 | STDP LTD weight | 0.01-0.03 |
| `stdp_tau` | float | 20.0 | STDP time constant | 10.0-50.0 |
| `hebbian_learning_rate` | float | 0.01 | Hebbian learning rate | 0.001-0.1 |
| `synaptogenesis_rate` | float | 0.0001 | New synapse formation | 0.00001-0.001 |
| `pruning_rate` | float | 0.00001 | Synapse removal rate | 0.000001-0.0001 |

### Memory Parameters

| Parameter | Type | Default | Description | Typical Range |
|-----------|------|---------|-------------|---------------|
| `working_memory_capacity` | int | 100 | Working memory size | 50-1000 |
| `working_memory_decay_rate` | float | 0.01 | Memory decay rate | 0.001-0.1 |
| `episodic_memory_capacity` | int | 1000 | Episodic memory size | 100-10000 |

### Neuromodulation Parameters

| Parameter | Type | Default | Description | Typical Range |
|-----------|------|---------|-------------|---------------|
| `dopamine_baseline` | float | 0.1 | Baseline dopamine level | 0.01-1.0 |
| `curiosity_baseline` | float | 0.1 | Baseline curiosity level | 0.01-1.0 |
| `novelty_sensitivity` | float | 0.5 | Novelty detection sensitivity | 0.1-2.0 |

### Development Parameters

| Parameter | Type | Default | Description | Typical Range |
|-----------|------|---------|-------------|---------------|
| `critical_period_enabled` | bool | false | Enable critical periods | true/false |
| `critical_period_start` | int | N/A | Critical period start | 0-10000 |
| `critical_period_end` | int | N/A | Critical period end | 100-50000 |

### World Parameters

| Parameter | Type | Default | Description | Typical Range |
|-----------|------|---------|-------------|---------------|
| `world_width` | int | 0 | World width (0 = no world) | 0-1000 |
| `world_height` | int | 0 | World height (0 = no world) | 0-1000 |
| `vision_width` | int | 16 | Vision input width | 8-128 |
| `vision_height` | int | 16 | Vision input height | 8-128 |

## Configuration Best Practices

### 1. Reproducibility
- Always set `random_seed` for reproducible results
- Document configuration files with experiment descriptions
- Use version control for configuration files

### 2. Resource Management
- Match `neuron_count` to available computational resources
- Use smaller timesteps for more accurate simulations
- Balance plasticity rates with simulation stability

### 3. Experimental Design
- Use different configurations for different experimental questions
- Keep baseline configurations simple for validation
- Add specific parameters for novel experimental manipulations

### 4. Configuration Validation
- Validate configuration parameters before simulation
- Check that required parameters are present
- Ensure parameter values are within acceptable ranges

## Tools for Configuration Management

### Configuration Loading
```python
import pynlm
import json

# Load configuration from file
with open("config.json", "r") as f:
    config_data = json.load(f)

config = pynlm.createDefaultConfig()

# Apply configuration
if "nlm_version" in config_data:
    print(f"NLM version: {config_data['nlm_version']}")

if "config" in config_data:
    for key, value in config_data["config"].items():
        config.set(key, value, pynlm.ConfigSource.File)
```

### Configuration Saving
```python
# Save current configuration to file
config.saveToFile("experiment_config.json")

# Get configuration summary
print(config.summary())
```

### Configuration Comparison
```python
# Compare two configurations
def compare_configs(config1, config2):
    keys1 = set(config1.getKeys())
    keys2 = set(config2.getKeys())
    
    differences = {}
    for key in keys1.union(keys2):
        val1 = config1.getOr(key, None)
        val2 = config2.getOr(key, None)
        if val1 != val2:
            differences[key] = {"config1": val1, "config2": val2}
    
    return differences
```

## Configuration File Naming Convention

Use descriptive names for configuration files:

- `basic_*.cfg` - Basic functionality demonstrations
- `advanced_*.cfg` - Complex demonstrations with full features
- `research_*.cfg` - Research-grade configurations
- `learning_*.cfg` - Learning-focused configurations
- `development_*.cfg` - Development-stage focused configurations
- `memory_*.cfg` - Memory system focused configurations
- `world_*.cfg` - World interaction focused configurations

## File Structure

```
/configs/
├── basic_simulation.cfg
├── world_interaction.cfg
├── learning_development.cfg
├── research.cfg
├── developmental.cfg
├── memory_focused.cfg
├── *.cfg          # Additional configuration files
```

This directory should be included in the repository to provide example
configurations for users and researchers.

## Related Files

- `nlm_demo.py` - Demo system that uses configuration files
- `quick_demo.py` - Quick start demonstration
- `examples/nlm_python_examples.py` - Comprehensive Python examples
- `docs/PYTHON_API.md` - Python API documentation

Configuration files are essential for:
1. Reproducing experiments
2. Sharing research methods
3. Teaching NLM usage
4. Standardizing simulation parameters
5. Enabling configuration-based testing

Use these examples as starting points and customize them for your specific
research questions and experimental requirements.