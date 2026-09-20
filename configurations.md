# NLM Configuration Documentation

## Overview

NLM uses a hierarchical configuration system to control all aspects of brain behavior, learning, memory, and world interaction. This document provides comprehensive documentation for all configuration options, their meanings, and usage examples.

---

## Configuration System Architecture

### Hierarchical Key Structure

NLM uses a dot-separated key system organized by functional domains:

```
<category>.<subcategory>.<parameter> = value
```

### Configuration Categories

| Category | Description | Subcategories |
|----------|-------------|---------------|
| `brain.` | Core brain neural network | `brain.neuron_count`, `brain.v_thresh`, etc. |
| `plasticity.` | Learning mechanisms | `plasticity.stdp`, `plasticity.hebbian`, `plasticity.structural` |
| `neuromod.` | Neuromodulation systems | `neuromod.dopamine`, `neuromod.curiosity`, `neuromod.novelty` |
| `memory.` | Memory systems | `memory.working`, `memory.episodic`, `memory.associative` |
| `world.` | World simulation | `world.width`, `world.height`, `world.vision`, etc. |
| `development.` | Developmental processes | `development.critical_period`, `development.maturation_rate` |
| `debug.` | Debugging and logging | `debug.level`, `debug.output`, etc. |

### Configuration Value Types

| Type | Syntax | Examples |
|------|--------|----------|
| Integer | `parameter = 1000` | `brain.neuron_count = 2000` |
| Float | `parameter = 1.5` | `brain.v_thresh = -45.0` |
| Boolean | `parameter = true/false` | `plasticity.stdp.enable = true` |
| String | `parameter = "value"` | `world.name = "test_environment"` |
| Array | `parameter = [1,2,3]` | `development.stages = [0.1, 0.5, 1.0]` |

---

## Brain Configuration Options

### Core Neural Parameters

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `brain.neuron_count` | int | 1000 | [100, 10000] | Total number of neurons in the network |
| `brain.synapse_density` | float | 0.1 | [0.01, 1.0] | Probability of connection between neurons |
| `brain.connection_probability` | float | 0.05 | [0.01, 0.5] | STDP connection probability |

### Neuron Dynamics

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `brain.v_thresh` | float | -50.0 | [-100, -10] | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | [-85, -60] | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | [-90, -60] | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | [5, 100] | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | [1, 10] | Refractory period (ms) |
| `brain.tau_syn` | float | 5.0 | [1, 50] | Synaptic time constant (ms) |

### Initial Conditions

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `brain.initial_weight_mean` | float | 0.5 | [0, 1] | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | [0.01, 0.5] | Standard deviation of initial weights |
| `brain.initial_excitability` | float | 1.0 | [0.1, 5.0] | Initial neural excitability |

### Regional Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.regions.enabled` | bool | true | Enable regional specialization |
| `brain.regions.count` | int | 1 | Number of brain regions |
| `brain.regions.cortex.neuron_count` | int | 0.7 | Default to cortex neurons |

---

## Plasticity Configuration Options

### Spike-Timing Dependent Plasticity (STDP)

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `plasticity.stdp.enable` | bool | true | - | Enable STDP mechanism |
| `plasticity.stdp.learning_rate` | float | 0.001 | [0.0001, 0.01] | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | [10, 50] | Time constant for potentiation (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | [10, 50] | Time constant for depression (ms) |
| `plasticity.stdp.max_weight` | float | 2.0 | [1, 5] | Maximum synaptic weight |
| `plasticity.stdp.min_weight` | float | 0.0 | [0, 1] | Minimum synaptic weight |
| `plasticity.stdp.pre_scale` | float | 1.0 | [0.5, 2.0] | Pre-synaptic scaling factor |
| `plasticity.stdp.post_scale` | float | 1.0 | [0.5, 2.0] | Post-synaptic scaling factor |

### Hebbian Learning

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `plasticity.hebbian.enable` | bool | true | - | Enable Hebbian learning |
| `plasticity.hebbian.rate` | float | 0.001 | [0.0001, 0.01] | Hebbian learning rate |
| `plasticity.hebbian.threshold` | float | 0.5 | [0.1, 1.0] | Activation threshold for Hebbian updates |
| `plasticity.hebbian.window_size` | int | 100 | [10, 1000] | Time window for Hebbian effects (ms) |

### Structural Plasticity

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `plasticity.structural.enable` | bool | true | - | Enable structural plasticity (network growth) |
| `plasticity.structural.growth_rate` | float | 0.01 | [0.001, 0.1] | Rate of structural changes |
| `plasticity.structural.prune_threshold` | float | 0.1 | [0.01, 0.5] | Weight threshold for pruning |
| `plasticity.structural.max_neurons` | int | 5000 | [100, 10000] | Maximum number of neurons |
| `plasticity.structural.exploration_bias` | bool | false | - | Bias growth toward novel connections |
| `plasticity.structural.reinforcement_bias` | bool | false | - | Bias growth toward strong connections |

---

## Neuromodulation Configuration Options

### Dopamine System

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `neuromod.dopamine.enable` | bool | true | - | Enable dopamine modulation |
| `neuromod.dopamine.scale` | float | 1.0 | [0.1, 5.0] | Dopamine modulation scale |
| `neuromod.dopamine.baseline` | float | 0.1 | [0, 1] | Baseline dopamine level |
| `neuromod.dopamine.decay_rate` | float | 0.9 | [0.5, 1.0] | Dopamine decay rate |
| `neuromod.dopamine.sensitivity` | float | 1.0 | [0.1, 5.0] | Sensitivity to prediction errors |

### Curiosity System

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `neuromod.curiosity.enable` | bool | true | - | Enable curiosity-driven exploration |
| `neuromod.curiosity.sensitivity` | float | 1.0 | [0.1, 5.0] | Curiosity sensitivity to novelty |
| `neuromod.curiosity.threshold` | float | 0.5 | [0.1, 1.0] | Novelty threshold for curiosity activation |
| `neuromod.curiosity.decay_rate` | float | 0.95 | [0.5, 1.0] | Curiosity decay rate |
| `neuromod.curiosity.exploration_bonus` | float | 0.1 | [0, 1] | Bonus for novel exploration |

### Novelty Detection

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `neuromod.novelty.enable` | bool | true | - | Enable novelty detection |
| `neuromod.novelty.threshold` | float | 0.5 | [0.1, 1.0] | Novelty detection threshold |
| `neuromod.novelty.smoothing` | float | 0.9 | [0.5, 1.0] | Novelty smoothing factor |
| `neuromod.novelty.history_size` | int | 100 | [10, 1000] | Size of novelty history buffer |

### Acetylcholine, Norepinephrine, Serotonin

| System | Parameters | Defaults |
|--------|------------|----------|
| `neuromod.acetylcholine` | `enable=true`, `scale=0.8`, `target=sensory` | Basic attention modulation |
| `neuromod.norepinephrine` | `enable=true`, `scale=1.2`, `arousal=0.5` | Arousal and vigilance |
| `neuromod.serotonin` | `enable=true`, `scale=0.9`, `mood=0.5` | Mood and motivation |

---

## Memory Configuration Options

### Working Memory

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `memory.working.capacity` | int | 100 | [10, 1000] | Maximum number of active patterns |
| `memory.working.decay_rate` | float | 0.95 | [0.5, 1.0] | Working memory decay rate |
| `memory.working.competition` | bool | true | - | Enable competition between patterns |
| `memory.working.spread` | float | 0.3 | [0.1, 0.8] | Pattern spread in working memory |

### Episodic Memory

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `memory.episodic.enable` | bool | true | - | Enable episodic memory |
| `memory.episodic.capacity` | int | 50 | [1, 200] | Maximum number of episodes |
| `memory.episodic.encoding_strength` | float | 0.8 | [0.1, 1.0] | Strength of episodic encoding |
| `memory.episodic.replay_rate` | float | 0.1 | [0.01, 0.5] | Rate of memory replay (episodes per timestep) |
| `memory.episodic.consolidation_threshold` | float | 0.7 | [0.1, 1.0] | Quality threshold for consolidation |

### Associative Memory

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `memory.associative.enable` | bool | true | - | Enable associative memory |
| `memory.associative.pattern_threshold` | float | 0.7 | [0.1, 1.0] | Pattern similarity threshold |
| `memory.associative.association_strength` | float | 0.6 | [0.1, 1.0] | Strength of associations |
| `memory.associative.spread` | float | 0.4 | [0.1, 0.8] | Pattern spread in associative memory |
| `memory.associative.forgetting_rate` | float | 0.98 | [0.9, 1.0] | Rate of forgetting associations |

---

## World Configuration Options

### World Geometry

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `world.width` | int | 20 | [10, 100] | World map width (cells) |
| `world.height` | int | 20 | [10, 100] | World map height (cells) |
| `world.vision_width` | int | 8 | [4, 20] | Agent's visual range (width) |
| `world.vision_height` | int | 8 | [4, 20] | Agent's visual range (height) |
| `world.cell_size` | float | 1.0 | [0.5, 2.0] | Physical size of each cell |

### Environmental Properties

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `world.max_energy` | float | 100.0 | [10, 1000] | Maximum agent energy |
| `world.energy_decay` | float | 0.01 | [0.001, 0.1] | Energy loss per timestep |
| `world.time_step` | float | 0.1 | [0.01, 1.0] | Simulation timestep (seconds) |
| `world.gravity` | float | 0.0 | [-10, 10] | Gravitational acceleration |

### Sensory Properties

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `world.sensory.noise_level` | float | 0.05 | [0, 0.2] | Sensory input noise level |
| `world.sensory.range` | int | 10 | [5, 50] | Sensory detection range |
| `world.sensory.threshold` | float | 0.1 | [0, 1] | Minimum detectable signal |

### Object Properties

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `world.objects.resource_decay` | float | 0.99 | [0.9, 1.0] | Resource regeneration rate |
| `world.objects.hazard_intensity` | float | 1.0 | [0.1, 5.0] | Hazard damage intensity |
| `world.objects.wall_resistance` | float | 1.0 | [0.5, 2.0] | Wall collision resistance |

---

## Development Configuration Options

### Developmental Stages

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `development.critical_period` | int | 100 | [50, 500] | Duration of critical learning period (steps) |
| `development.maturation_rate` | float | 0.8 | [0.1, 1.0] | Rate of brain maturation |
| `development.plasticity_decay` | float | 0.9 | [0.5, 1.0] | Age-dependent plasticity decay |
| `development.excitability_change` | float | 0.3 | [0, 1] | Change in neural excitability with age |

### Stage-Specific Parameters

| Stage | Parameters |
|-------|------------|
| `development.stage.initial` | `plasticity_multiplier=1.0`, `learning_rate=0.01` |
| `development.stage.critical_period` | `plasticity_multiplier=2.0`, `learning_rate=0.05` |
| `development.stage.maturation` | `plasticity_multiplier=1.5`, `learning_rate=0.02` |
| `development.stage.adult` | `plasticity_multiplier=1.0`, `learning_rate=0.005` |
| `development.stage.aging` | `plasticity_multiplier=0.5`, `learning_rate=0.001` |

---

## Debug Configuration Options

### Logging

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `debug.logging_level` | string | "INFO" | ["DEBUG", "INFO", "WARN", "ERROR"] | Overall logging verbosity |
| `debug.log_file` | string | "" | - | Path to log file (empty = stdout) |
| `debug.max_log_size` | int | 10485760 | [1048576, 104857600] | Maximum log file size (bytes) |

### Performance Monitoring

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `debug.performance.enable` | bool | false | - | Enable performance monitoring |
| `debug.performance.sample_rate` | int | 1000 | [100, 10000] | Performance sampling frequency (steps) |
| `debug.performance.threshold` | float | 1.0 | [0.1, 10.0] | Performance threshold for alerts |

### Validation

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `debug.validation.enable` | bool | true | - | Enable configuration validation |
| `debug.validation.strict` | bool | false | - | Enable strict validation mode |
| `debug.validation.warnings` | bool | true | - | Output validation warnings |

---

## Configuration Templates

### Development-Optimized Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()

# Optimize for developmental learning
config.set("brain.development.enable", True)
config.set("brain.development.critical_period", 200)
config.set("brain.development.maturation_rate", 0.9)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("plasticity.hebbian.rate", 0.003)
config.set("plasticity.structural.enable", True)
config.set("neuromod.curiosity.sensitivity", 2.0)
config.set("neuromod.novelty.threshold", 0.3)
config.set("memory.episodic.enable", True)
config.set("memory.associative.enable", True)

# Create brain with optimized config
brain = pynlm.createBrain(config)
brain.initialize()
```

### Exploration-Optimized Configuration

```python
config = pynlm.createDefaultConfig()

# Optimize for exploration and discovery
config.set("neuromod.curiosity.sensitivity", 3.0)
config.set("neuromod.curiosity.threshold", 0.2)
config.set("neuromod.novelty.threshold", 0.1)
config.set("plasticity.structural.exploration_bias", True)
config.set("world.vision_width", 15)
config.set("world.vision_height", 15)
config.set("brain.initial_excitability", 2.0)

# Create agent for exploration
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=15, visionHeight=15)
world.reset()
agent.initialize(world)

agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
```

### Performance-Optimized Configuration

```python
config = pynlm.createDefaultConfig()

# Optimize for computational efficiency
config.set("brain.neuron_count", 500)  # Reduce network size
config.set("brain.synapse_density", 0.05)  # Reduce connections
config.set("world.width", 15)  # Smaller world
config.set("world.height", 15)  # Smaller world
config.set("debug.performance.enable", True)  # Enable monitoring
config.set("debug.logging_level", "WARN")  # Reduce logging

# Create efficient brain
brain = pynlm.createBrain(config)
brain.initialize()
```

### Validation and Checking

```python
# Validate configuration
config = pynlm.createDefaultConfig()

# Check critical parameters
if config.has("brain.neuron_count"):
    neurons = config.getInt("brain.neuron_count")
    if neurons < 100:
        print("WARNING: Very small brain, may not learn effectively")
    elif neurons > 5000:
        print("WARNING: Very large brain, may be computationally expensive")

# Check for conflicts
if config.getBool("plasticity.structural.enable"):
    if config.getInt("brain.neuron_count") > 3000:
        print("WARNING: Large brain with structural plasticity may be unstable")

# Validate world configuration
if config.has("world.vision_width") and config.has("world.width"):
    vision = config.getInt("world.vision_width")
    world = config.getInt("world.width")
    if vision > world / 2:
        print("WARNING: Vision extends beyond world boundaries")
```

---

## Configuration File Format

### JSON Configuration

Configuration can be saved and loaded as JSON files:

```json
{
    "brain": {
        "neuron_count": 2000,
        "v_thresh": -50.0,
        "development": {
            "enable": true,
            "critical_period": 200
        }
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.002
        },
        "hebbian": {
            "enable": true,
            "rate": 0.001
        }
    },
    "world": {
        "width": 20,
        "height": 20,
        "vision_width": 8,
        "vision_height": 8
    }
}
```

### Loading Configuration

```python
import json

# Create new config
config = pynlm.createDefaultConfig()

# Load from JSON file
config.loadFromFile("my_config.json")

# Or load from JSON string
json_string = '{"brain.neuron_count": 1500, "plasticity.stdp.enable": true}'
config.loadFromJsonString(json_string)

# Create brain with loaded configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

### Saving Configuration

```python
# Save to JSON file
config.saveToFile("brain_config.json")

# Get JSON string
json_string = config.toJsonString()
print(json_string)
```

---

## Advanced Configuration Techniques

### Parameter Inheritance

Subsystems can inherit parameters from parent categories:

```python
config = pynlm.createDefaultConfig()

# Set base learning parameters
config.set("plasticity.base_learning_rate", 0.001)
config.set("plasticity.base_plasticity", 1.0)

# Apply to specific mechanisms
config.set("plasticity.stdp.learning_rate", "${plasticity.base_learning_rate}")
config.set("plasticity.hebbian.rate", "${plasticity.base_learning_rate}")
config.set("plasticity.structural.growth_rate", "${plasticity.base_plasticity}")
```

### Conditional Configuration

Use conditional logic for different agent types:

```python
class AgentType:
    EXPLORER = "explorer"
    LEARNER = "learner"
    BALANCED = "balanced"

def create_config(agent_type: str):
    config = pynlm.createDefaultConfig()
    
    if agent_type == AgentType.EXPLORER:
        # Explorer configuration
        config.set("brain.neuron_count", 1500)
        config.set("neuromod.curiosity.sensitivity", 3.0)
        config.set("neuromod.curiosity.threshold", 0.2)
        config.set("plasticity.structural.exploration_bias", True)
        
    elif agent_type == AgentType.LEARNER:
        # Fast learner configuration
        config.set("brain.neuron_count", 3000)
        config.set("plasticity.stdp.learning_rate", 0.01)
        config.set("plasticity.hebbian.rate", 0.008)
        config.set("neuromod.dopamine.scale", 2.0)
        
    elif agent_type == AgentType.BALANCED:
        # Balanced configuration
        config.set("brain.neuron_count", 2000)
        config.set("plasticity.stdp.learning_rate", 0.005)
        config.set("neuromod.curiosity.sensitivity", 1.5)
        config.set("neuromod.novelty.threshold", 0.4)
    
    return config
```

### Dynamic Configuration Adjustment

Adjust configuration during runtime:

```python
class AdaptiveConfig:
    def __init__(self, config):
        self.config = config
        self.performance_history = []
    
    def adjust_for_performance(self, current_performance: float):
        """Adjust configuration based on current performance."""
        self.performance_history.append(current_performance)
        
        if len(self.performance_history) > 100:
            avg_performance = sum(self.performance_history[-100:]) / 100
            
            if avg_performance < 0.5:
                # Performance too low - increase learning
                self.config.set("plasticity.stdp.learning_rate", 
                               self.config.getFloat("plasticity.stdp.learning_rate") * 1.5)
                self.config.set("neuromod.curiosity.sensitivity", 
                               self.config.getFloat("neuromod.curiosity.sensitivity") * 1.2)
                
            elif avg_performance > 0.95:
                # Performance too high - reduce exploration
                self.config.set("neuromod.curiosity.sensitivity", 
                               self.config.getFloat("neuromod.curiosity.sensitivity") * 0.8)
                self.config.set("plasticity.structural.growth_rate", 
                               self.config.getFloat("plasticity.structural.growth_rate") * 0.9)
    
    def get_config(self):
        return self.config
```

---

## Configuration Validation Best Practices

### Pre-Run Validation

```python
def validate_config_for_training(config, world_size: tuple):
    """Validate configuration before running training."""
    errors = []
    warnings = []
    
    # Check brain configuration
    if config.has("brain.neuron_count"):
        neurons = config.getInt("brain.neuron_count")
        if neurons < 50:
            errors.append("Brain too small (<50 neurons) for effective learning")
        elif neurons > 10000:
            warnings.append("Large brain may be computationally expensive")
    
    # Check memory configuration
    if config.getBool("memory.episodic.enable"):
        if config.getInt("memory.episodic.capacity") < 5:
            warnings.append("Episodic memory capacity very low (<5 episodes)")
    
    # Check world configuration
    world_width = config.getInt("world.width")
    world_height = config.getInt("world.height")
    vision_width = config.getInt("world.vision_width")
    vision_height = config.getInt("world.vision_height")
    
    if vision_width > world_width / 2:
        warnings.append("Vision width exceeds half the world width")
    
    if vision_height > world_height / 2:
        warnings.append("Vision height exceeds half the world height")
    
    # Check for inconsistent settings
    if config.getBool("plasticity.structural.enable"):
        if config.getInt("brain.neuron_count") > 5000:
            warnings.append("Large brain with structural plasticity may be unstable")
    
    return errors, warnings

# Usage example
config = pynlm.createDefaultConfig()
# ... configure brain, plasticity, memory, world ...

errors, warnings = validate_config_for_training(config, (20, 20))

if errors:
    print("CONFIGURATION ERRORS:")
    for error in errors:
        print(f"  - {error}")
    raise ValueError("Invalid configuration")

if warnings:
    print("CONFIGURATION WARNINGS:")
    for warning in warnings:
        print(f"  - {warning}")
```

### Runtime Configuration Monitoring

```python
class ConfigurationMonitor:
    def __init__(self, config, check_interval: int = 1000):
        self.config = config
        self.check_interval = check_interval
        self.step_count = 0
        self.warnings = []
    
    def check_stagnation(self, current_performance: float):
        """Check for learning stagnation."""
        self.step_count += 1
        
        if self.step_count % self.check_interval == 0:
            # Get learning-related parameters
            stdp_rate = self.config.getFloat("plasticity.stdp.learning_rate")
            curiosity = self.config.getFloat("neuromod.curiosity.sensitivity")
            
            # Check if parameters need adjustment
            if current_performance < 0.3 and stdp_rate < 0.001:
                self.warnings.append(
                    f"Step {self.step_count}: Low performance with low learning rate"
                )
            
            if curiosity < 0.5 and current_performance > 0.8:
                self.warnings.append(
                    f"Step {self.step_count}: High performance but low curiosity"
                )
    
    def get_warnings(self):
        return self.warnings.copy()
    
    def clear_warnings(self):
        self.warnings.clear()
```

---

## Configuration File Examples

### Minimal Configuration

```json
{
    "brain": {
        "neuron_count": 1000,
        "v_thresh": -50.0
    },
    "world": {
        "width": 20,
        "height": 20,
        "vision_width": 8,
        "vision_height": 8
    }
}
```

### Development Configuration

```json
{
    "brain": {
        "neuron_count": 2000,
        "development": {
            "enable": true,
            "critical_period": 200,
            "maturation_rate": 0.9
        }
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.005
        },
        "hebbian": {
            "enable": true,
            "rate": 0.003
        },
        "structural": {
            "enable": true,
            "growth_rate": 0.02
        }
    },
    "memory": {
        "episodic": {
            "enable": true,
            "capacity": 100
        },
        "associative": {
            "enable": true,
            "pattern_threshold": 0.8
        }
    },
    "world": {
        "width": 30,
        "height": 30,
        "vision_width": 12,
        "vision_height": 12,
        "max_energy": 200.0
    }
}
```

### Production Configuration

```json
{
    "brain": {
        "neuron_count": 1500,
        "v_thresh": -55.0,
        "tau_mem": 15.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.002,
            "tau_plus": 25.0,
            "tau_minus": 25.0
        },
        "hebbian": {
            "enable": true,
            "rate": 0.001
        }
    },
    "world": {
        "width": 25,
        "height": 25,
        "vision_width": 10,
        "vision_height": 10,
        "time_step": 0.05,
        "energy_decay": 0.005
    },
    "debug": {
        "logging_level": "WARN",
        "performance": {
            "enable": true,
            "sample_rate": 5000
        }
    }
}
```

---

## Configuration Migration Guide

### From Version 5.x to 6.x

Changes in Phase 6 configuration structure:

```json
// OLD version 5.x structure
{
    "brain": {
        "neurons": 1000
    },
    "learning": {
        "stdp": {"enabled": true}
    }
}

// NEW version 6.x structure
{
    "brain": {
        "neuron_count": 1000,
        "plasticity": {
            "stdp": {"enable": true}
        }
    }
}
```

### Migration Script

```python
import json

def migrate_config_v5_to_v6(v5_config):
    """Migrate configuration from version 5.x to 6.x."""
    v6_config = {}
    
    # Migrate brain section
    if "brain" in v5_config:
        v6_config["brain"] = {}
        if "neurons" in v5_config["brain"]:
            v6_config["brain"]["neuron_count"] = v5_config["brain"]["neurons"]
    
    # Migrate learning section
    if "learning" in v5_config:
        v6_config["plasticity"] = {}
        if "stdp" in v5_config["learning"]:
            v6_config["plasticity"]["stdp"] = {
                "enable": v5_config["learning"]["stdp"].get("enabled", True),
                "learning_rate": v5_config["learning"]["stdp"].get("rate", 0.001)
            }
        if "hebbian" in v5_config["learning"]:
            v6_config["plasticity"]["hebbian"] = {
                "enable": v5_config["learning"]["hebbian"].get("enabled", True),
                "rate": v5_config["learning"]["hebbian"].get("rate", 0.001)
            }
    
    # Copy other sections with naming changes
    if "memory" in v5_config:
        v6_config["memory"] = v5_config["memory"]
    
    if "world" in v5_config:
        v6_config["world"] = v5_config["world"]
    
    return v6_config

# Usage
with open("config_v5.json", "r") as f:
    v5_config = json.load(f)

v6_config = migrate_config_v5_to_v6(v5_config)

with open("config_v6.json", "w") as f:
    json.dump(v6_config, f, indent=2)
```

---

## Conclusion

Configuration is the foundation of NLM brain development. Understanding configuration options and their interactions is essential for:

1. **Creating effective learning agents** with desired behaviors
2. **Optimizing performance** for specific tasks and hardware
3. **Debugging and troubleshooting** brain training issues
4. **Reproducing experiments** with consistent parameters
5. **Comparing different agent types** and their characteristics

The hierarchical configuration system provides flexibility while maintaining organization. Use the templates and examples as starting points, and always validate your configuration before running simulations.

---

*For the most current configuration options and examples, check the NLM documentation website and GitHub repository.*
