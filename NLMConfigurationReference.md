# NLM — Configuration Reference Documentation

This document provides comprehensive configuration reference for the NLM codebase, covering all configuration options, presets, templates, and validation.

---

## Configuration Overview

The NLM configuration system provides comprehensive configuration management with multiple sources, validation, and advanced features.

### Configuration Sources and Precedence

Configuration sources are applied in the following order (highest to lowest priority):

1. **Environment Variables** (NLM_ prefix by default)
2. **Command Line Arguments**
3. **Configuration File**
4. **Programmatic Configuration**
5. **Default Values**

### Configuration File Formats

#### .cfg Format (Key-Value)

```
# Configuration comments
brain.neuron_count = 1000
brain.synapse_density = 0.1
plasticity.stdp.enable = true
plasticity.hebbian.learning_rate = 0.001
```

#### JSON Format

```json
{
  "brain": {
    "neuron_count": 1000,
    "synapse_density": 0.1,
    "plasticity": {
      "stdp": {
        "enable": true,
        "learning_rate": 0.001
      }
    }
  }
}
```

---

## Brain Configuration

### Core Brain Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Brain Regions

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.region_count` | int | 1 | Number of brain regions |
| `brain.region_name.<id>` | string | - | Region name (0-indexed) |

### Neural Populations

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.population_count` | int | 1 | Number of neural populations |
| `brain.population_type.<id>` | string | - | Population type |

---

## Plasticity Configuration

### STDP Plasticity

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.a_plus` | float | 0.01 | STDP potentiation amplitude |
| `plasticity.stdp.a_minus` | float | 0.01 | STDP depression amplitude |

### Hebbian Plasticity

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.001 | Hebbian learning rate |
| `plasticity.hebbian.threshold` | float | 0.5 | Hebbian learning threshold |

### Structural Plasticity

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.growth_rate` | float | 0.001 | Neural growth rate |
| `plasticity.structural.pruning_rate` | float | 0.0005 | Neural pruning rate |

---

## Neuromodulation Configuration

### Dopamine Modulation

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.dopamine.initial_level` | float | 0.5 | Initial dopamine level |

### Curiosity Modulation

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.threshold` | float | 0.5 | Curiosity threshold |
| `neuromod.curiosity.decay_rate` | float | 0.01 | Curiosity decay rate |

### Novelty Detection

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.novelty.sensitivity` | float | 1.0 | Novelty detection sensitivity |
| `neuromod.novelty.history_size` | int | 100 | Novelty history size |

---

## Memory System Configuration

### Working Memory

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working.capacity` | int | 1000 | Working memory capacity |
| `memory.working.compete` | bool | true | Enable competition |
| `memory.working.stability` | float | 0.95 | Pattern stability |

### Episodic Memory

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.episodic.capacity` | int | 1000 | Episodic memory capacity |
| `memory.episodic.interest_threshold` | float | 0.8 | Interest threshold for storage |
| `memory.episodic.replay_rate` | float | 0.1 | Memory replay rate |

### Associative Memory

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.associative.capacity` | int | 1000 | Associative memory capacity |
| `memory.associative.hebbian_strength` | float | 0.1 | Hebbian strength |
| `memory.associative.pattern_similarity` | float | 0.8 | Pattern similarity threshold |

---

## Prediction System Configuration

### Forward Prediction

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `prediction.forward.enable` | bool | true | Enable forward prediction |
| `prediction.forward.horizon` | int | 10 | Prediction horizon |
| `prediction.forward.noise` | float | 0.1 | Prediction noise level |

### Inverse Prediction

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `prediction.inverse.enable` | bool | true | Enable inverse prediction |
| `prediction.inverse.accuracy` | float | 0.9 | Prediction accuracy |
| `prediction.inverse.confidence` | float | 0.5 | Prediction confidence |

---

## Cognition System Configuration

### Neural Planner

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `cognition.planner.enable` | bool | true | Enable neural planner |
| `cognition.planner.horizon` | int | 5 | Planning horizon |
| `cognition.planner.exploration` | float | 0.1 | Exploration rate |

### Concept Formation

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `cognition.concept.enable` | bool | true | Enable concept formation |
| `cognition.concept.similarity` | float | 0.8 | Concept similarity threshold |
| `cognition.concept.cohesion` | float | 0.9 | Concept cohesion |

### Attention

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `cognition.attention.enable` | bool | true | Enable attention system |
| `cognition.attention.competition` | float | 0.9 | Attention competition |
| `cognition.attention.gating` | float | 0.8 | Attention gating strength |

---

## Development System Configuration

### Developmental Stages

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.stage.duration.<stage>` | int | - | Duration of each stage |
| `development.stage.plasticity.<stage>` | float | - | Plasticity rate for each stage |

### Synaptogenesis

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.synaptogenesis.rate` | float | 0.01 | Synaptogenesis rate |
| `development.synaptogenesis.target_density` | float | 0.1 | Target synapse density |

### Pruning

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.pruning.rate` | float | 0.005 | Pruning rate |
| `development.pruning.threshold` | float | 0.1 | Pruning threshold |

### Maturation

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.maturation.rate` | float | 0.01 | Maturation rate |
| `development.maturation.threshold` | float | 0.9 | Maturation threshold |

---

## Neuromodulation Configuration

### System Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.enable.all` | bool | true | Enable all neuromodulators |
| `neuromod.update_rate` | float | 0.1 | Neuromodulator update rate |
| `neuromod.initial_levels.<modulator>` | float | - | Initial modulator levels |

### Individual Neuromodulators

#### Dopamine

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.enable` | bool | true | Enable dopamine |
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.dopamine.initial_level` | float | 0.5 | Initial dopamine level |

#### Serotonin

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.serotonin.enable` | bool | true | Enable serotonin |
| `neuromod.serotonin.scale` | float | 1.0 | Serotonin modulation scale |

#### Acetylcholine

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.acetylcholine.enable` | bool | true | Enable acetylcholine |
| `neuromod.acetylcholine.scale` | float | 1.0 | Acetylcholine modulation scale |

---

## World Configuration

### World Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `world.width` | int | 20 | World width |
| `world.height` | int | 20 | World height |
| `world.vision_width` | int | 8 | Vision width |
| `world.vision_height` | int | 8 | Vision height |
| `world.max_energy` | float | 100.0 | Maximum energy |
| `world.agent_start_x` | float | 10.0 | Agent starting X position |
| `world.agent_start_y` | float | 10.0 | Agent starting Y position |

### World Objects

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `world.objects.<type>.<id>.x` | float | - | Object X position |
| `world.objects.<type>.<id>.y` | float | - | Object Y position |
| `world.objects.<type>.<id>.value` | float | - | Object value |
| `world.objects.<type>.<id>.type` | string | - | Object type |

---

## Built-in Presets

### Basic Preset

```json
{
  "brain": {
    "neuron_count": 1000,
    "synapse_density": 0.1
  },
  "plasticity": {
    "stdp": { "enable": true, "learning_rate": 0.001 },
    "hebbian": { "enable": true, "learning_rate": 0.001 },
    "structural": { "enable": false }
  },
  "neuromod": {
    "curiosity": { "enable": false },
    "novelty": { "enable": false }
  }
}
```

### Advanced Preset

```json
{
  "brain": {
    "neuron_count": 5000,
    "synapse_density": 0.05
  },
  "plasticity": {
    "stdp": { "enable": true, "learning_rate": 0.001 },
    "hebbian": { "enable": true, "learning_rate": 0.001 },
    "structural": { "enable": true, "growth_rate": 0.001 }
  },
  "neuromod": {
    "curiosity": { "enable": true, "threshold": 0.5 },
    "novelty": { "enable": true, "sensitivity": 1.0 }
  }
}
```

### Debug Preset

```json
{
  "brain": {
    "neuron_count": 100,
    "synapse_density": 0.2
  },
  "logging": {
    "level": "DEBUG",
    "file": "debug.log",
    "console": true
  },
  "validation": {
    "enabled": true,
    "strict": true
  }
}
```

### Production Preset

```json
{
  "brain": {
    "neuron_count": 3000,
    "synapse_density": 0.08
  },
  "performance": {
    "optimization": "aggressive",
    "memory_pools": true,
    "event_queues": true,
    "simd": true,
    "parallel": true
  },
  "stability": {
    "validation": "strict",
    "error_handling": "comprehensive",
    "checkpointing": true
  }
}
```

---

## Configuration Templates

### Default Template

```json
{
  "brain": {
    "neuron_count": {{BRAIN_NEURON_COUNT}},
    "synapse_density": {{BRAIN_SYNAPSE_DENSITY}},
    "region_count": {{BRAIN_REGION_COUNT}}
  },
  "plasticity": {
    "stdp": { "enable": {{PLASTICITY_STDP_ENABLE}}, "learning_rate": {{PLASTICITY_STDP_LEARNING_RATE}} },
    "hebbian": { "enable": {{PLASTICITY_HEBBIAN_ENABLE}}, "learning_rate": {{PLASTICITY_HEBBIAN_LEARNING_RATE}} },
    "structural": { "enable": {{PLASTICITY_STRUCTURAL_ENABLE}} }
  }
}
```

### Debug Template

```json
{
  "brain": {
    "neuron_count": {{BRAIN_NEURON_COUNT}},
    "synapse_density": {{BRAIN_SYNAPSE_DENSITY}}
  },
  "logging": {
    "level": "{{LOGGING_LEVEL}}",
    "file": "{{LOGGING_FILE}}",
    "console": {{LOGGING_CONSOLE}}
  },
  "validation": {
    "enabled": {{VALIDATION_ENABLED}},
    "strict": {{VALIDATION_STRICT}}
  }
}
```

### Simple Template

```json
{
  "brain": {
    "neuron_count": {{BRAIN_NEURON_COUNT}},
    "synapse_density": {{BRAIN_SYNAPSE_DENSITY}}
  }
}
```

---

## Configuration Validation

### Validation Rules

1. **Type Validation**: All values must match their declared types
2. **Range Validation**: Values must be within specified ranges
3. **Pattern Validation**: String values must match patterns
4. **Choice Validation**: Values must be in allowed choices
5. **Dependency Validation**: Required dependencies must be satisfied

### Validation Error Types

- `MissingRequired` - Required configuration missing
- `InvalidType` - Invalid configuration type
- `InvalidRange` - Value out of range
- `InvalidPattern` - Value doesn't match pattern
- `InvalidChoice` - Value not in allowed choices
- `DependencyFailed` - Required dependency missing

### Validation Commands

```bash
# Validate current configuration
./nlm --validate-config

# Validate specific configuration file
./nlm --validate-config --config=production.cfg

# Validate with schema file
./nlm --validate-config --schema=custom_schema.json
```

---

## Configuration Export/Import

### Export Commands

```bash
# Export to JSON format
./nlm --export-config --format=json --file=exported_config.json

# Export to .cfg format
./nlm --export-config --format=cfg --file=exported_config.cfg

# Export with information
./nlm --export-config --info-only
```

### Import Commands

```bash
# Import from JSON format
./nlm --config=exported_config.json

# Import from .cfg format
./nlm --config=exported_config.cfg

# Import with preset
./nlm --preset=imported_preset

# Import with template
./nlm --template=imported_template
```

---

## Configuration Comparison

### Compare Commands

```bash
# Compare two configuration files
./nlm --compare-configs \
  --file=config1.json \
  --file2=config2.json
```

### Comparison Results

- **Added Keys**: Keys present in second configuration but not in first
- **Removed Keys**: Keys present in first configuration but not in second
- **Modified Keys**: Keys with different values
- **Unchanged Keys**: Keys with same values

---

## Environment Variables

### Environment Variable Prefix

Set prefix for environment variables (default: NLM_):

```bash
./nlm --env-prefix=NLM
```

### Common Environment Variables

| Variable | Type | Default | Description |
|----------|------|---------|-------------|
| `NLM_brain.neuron_count` | int | 1000 | Total number of neurons |
| `NLM_brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `NLM_plasticity.stdp.enable` | bool | true | Enable STDP |
| `NLM_neuromod.curiosity.enable` | bool | true | Enable curiosity |
| `NLM_neuromod.novelty.enable` | bool | true | Enable novelty |

### Environment Variable Precedence

```bash
# Set environment variables
export NLM_brain_neuron_count=1000
export NLM_brain_synapse_density=0.1
export NLM_log_level=DEBUG

# Override via CLI
./nlm --config=basic.cfg --brain_neuron_count=500
# Result: brain.neuron_count=500 (CLI overrides env)
```

---

## Configuration Schema

### Schema Format

Configuration schemas are JSON files that define validation rules:

```json
{
  "key_name": {
    "type": "string",
    "required": false,
    "default": "default_value",
    "min": 0.0,
    "max": 100.0,
    "pattern": "^[a-z]+$",
    "choices": ["option1", "option2"],
    "description": "Description of the key"
  }
}
```

### Schema Validation

- **Type validation**: Validates configuration value types
- **Range validation**: Validates value ranges
- **Pattern validation**: Validates string patterns
- **Choice validation**: Validates allowed choices
- **Dependency validation**: Validates dependencies

---

## References

1. **NLMUserGuide.md** - User guide and tutorials
2. **NLMApiReference.md** - API documentation
3. **NLMExamples.md** - Additional examples
4. **NLMMigrationGuide.md** - Migration guide
5. **NLMBestPractices.md** - Best practices
6. **NLMTroubleshooting.md** - Troubleshooting guide

---

*Last Updated: Phase 6 Completion*
*Version: 1.0.0*