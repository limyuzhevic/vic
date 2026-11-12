# Enhanced Configuration System for NLM

This document describes the enhanced configuration system with validation, templates, and advanced features.

## Configuration Overview

The enhanced configuration system provides:

### Core Configuration Options

#### Basic Network Configuration
| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `neuron_count` | int | 1000 | Number of neurons in the brain | > 0 |
| `region_count` | int | 2 | Number of brain regions | > 0, <= 10 |
| `connection_probability` | float | 0.1 | Connection density | [0.0, 1.0] |
| `random_seed` | int64 | 42 | Random seed for reproducibility | >= 0 |
| `simulation_timestep` | double | 0.001 | Simulation timestep in seconds | > 0, <= 1.0 |

#### Neuron Dynamics Parameters
| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `v_thresh` | float | -50.0 | Neuron threshold potential (mV) | -100 <= x <= -10 |
| `v_rest` | float | -70.0 | Resting potential (mV) | -100 <= x <= -40 |
| `v_reset` | float | -75.0 | Reset potential after spike (mV) | -90 <= x <= -50 |
| `tau_mem` | float | 20.0 | Membrane time constant (ms) | > 0 |
| `tau_ref` | float | 2.0 | Refractory period (ms) | > 0 |
| `refractory_period` | int | 9 | Refractory period in steps | > 0 |

#### Plasticity Configuration
| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `stdp_ltp_weight` | float | 0.01 | STDP LTP weight | > 0 |
| `stdp_ltd_weight` | float | 0.012 | STDP LTD weight | > 0 |
| `stdp_tau` | float | 20.0 | STDP time constant | > 0 |
| `hebbian_learning_rate` | float | 0.01 | Hebbian learning rate | > 0 |
| `plasticity_learning_rate` | float | 0.01 | General plasticity rate | > 0 |

#### Development System Parameters
| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `synaptogenesis_rate` | float | 0.001 | Synapse formation rate | >= 0 |
| `pruning_rate` | float | 0.0001 | Synapse elimination rate | >= 0 |
| `maturation_rate` | float | 1.0 | Neural maturation rate | > 0 |
| `critical_period_length` | int | 100 | Critical period duration | > 0 |

#### Neuromodulation Parameters
| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `dopamine_scale` | float | 1.0 | Dopamine modulation scale | > 0 |
| `curiosity_enable` | bool | true | Enable curiosity-driven exploration | - |
| `novelty_enable` | bool | true | Enable novelty detection | - |
| `prediction_error_scale` | float | 1.0 | Prediction error scaling | > 0 |

#### Memory System Parameters
| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `working_memory_capacity` | int | 100 | Working memory size | > 0 |
| `episodic_memory_capacity` | int | 1000 | Episodic memory size | > 0 |
| `associative_memory_strength` | float | 0.1 | Memory association strength | [0.0, 1.0] |
| `memory_replay_rate` | float | 0.01 | Memory replay frequency | >= 0 |

#### World Configuration
| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `environment_width` | int | 10 | World width | > 0 |
| `environment_height` | int | 10 | World height | > 0 |
| `vision_width` | int | 16 | Vision sensor width | > 0 |
| `vision_height` | int | 16 | Vision sensor height | > 0 |
| `vision_channels` | int | 3 | Vision sensor channels | > 0 |
| `audio_sample_rate` | int | 44100 | Audio sampling rate | > 0 |
| `audio_samples` | int | 1024 | Audio buffer size | > 0 |

#### Cognitive System Parameters
| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `planning_horizon` | int | 5 | Action planning horizon | > 0 |
| `attention_span` | float | 1.0 | Attention duration | > 0 |
| `concept_formation_threshold` | float | 0.5 | Concept formation threshold | [0.0, 1.0] |
| `prediction_accuracy_threshold` | float | 0.8 | Prediction accuracy threshold | [0.0, 1.0] |

#### Logging and Output
| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `log_level` | string | "INFO" | Logging level | one of ["DEBUG", "INFO", "WARNING", "ERROR"] |
| `log_to_file` | bool | false | Enable file logging | - |
| `log_filename` | string | "nlm.log" | Log file name | - |
| `stats_output_interval` | int | 100 | Statistics output interval | > 0 |
| `checkpoint_interval` | int | 0 | Checkpoint saving interval | >= 0 |

### Template Configurations

#### Research Template
```ini
# NLM Configuration for Research Experiments

[random]
seed = 12345
reproducible = true

[network]
neuron_count = 2000
region_count = 3
connection_probability = 0.05

[neuron]
threshold = -50.0
resting_potential = -70.0
refractory_period = 9

[plasticity]
stype = "stdp_and_hebbian"
learning_rate = 0.001
structural_plasticity = true

[neuromodulation]
dopamine = true
curiosity = true
novelty = true
prediction_error = true

[memory]
working_memory = true
episodic_memory = true
associative_memory = true

[development]
enabled = true
stage_dependent = true

[world]
environment = "simple_grid"
size_x = 20
size_y = 20
reward_function = "sparse"

[experiment]
duration_steps = 5000
record_interval = 10
output_format = "json"
```

#### Performance Template
```ini
# NLM Configuration for Performance Testing

[performance]
mode = "benchmark"
test_types = ["small_network", "medium_network", "large_network"]

[small_network]
neurons = 100
steps = 1000

[medium_network]
neurons = 500
steps = 500

[large_network]
neurons = 1000
steps = 200

[measurement]
track_cpu = true
track_memory = true
track_spike_rate = true
collect_timing = true

[output]
results_file = "benchmark_results.json"
comparison_baseline = "baseline_config"
```

#### Development Template
```ini
# NLM Configuration for Development and Debugging

[debugging]
enabled = true
verbose_logging = true
breakpoint_on_error = true
profile_simulation = true

[validation]
strict_mode = true
validate_on_start = true
check_for_conflicts = true

[optimization]
auto_optimize = true
target_performance = "high"
memory_efficiency = true

[monitoring]
real_time_stats = true
performance_counters = true
resource_tracking = true
```

### Configuration Validation

#### Validation Rules

1. **Network Constraints**
   - Total neurons must be <= 100,000
   - Connection probability must be between 0.0 and 1.0
   - Neuron count per region: region_count <= neuron_count

2. **Physical Constraints**
   - Refractory period: 1 <= refractory_period <= 100
   - Membrane potentials: -100 <= v <= -10
   - Time constants: 0 < tau <= 1000

3. **Learning Constraints**
   - Learning rates: 0 < learning_rate <= 1.0
   - Plasticity weights: 0 < weight <= 1.0
   - Development rates: 0 <= rate <= 1.0

4. **Memory Constraints**
   - Memory capacities: > 0
   - Association strengths: 0 <= strength <= 1.0
   - Replay rates: 0 <= rate <= 1.0

5. **Environmental Constraints**
   - World dimensions: 1 <= width,height <= 100
   - Sensor dimensions: 1 <= width,height <= 50
   - Time horizons: > 0

#### Validation Errors

| Error Code | Description | Severity | Example |
|------------|-------------|----------|---------|
| NLM_C001 | Invalid neuron count | ERROR | neuron_count = -5 |
| NLM_C002 | Connection probability out of range | ERROR | connection_probability = 1.5 |
| NLM_C003 | Inconsistent configuration | WARNING | neuron_count < region_count |
| NLM_C004 | Deprecated parameter | WARNING | deprecated_setting = true |
| NLM_C005 | Performance issue detected | INFO | large_network_without_optimization |
| NLM_C006 | Memory threshold exceeded | WARNING | memory_capacity > available |
| NLM_C007 | Learning rate too high | ERROR | learning_rate = 10.0 |
| NLM_C008 | Invalid time step | ERROR | simulation_timestep = 0.0 |

#### Validation Example

```python
import nlm_config

# Load and validate configuration
config = nlm_config.load_config("my_config.json")

# Validate configuration
validation_result = config.validate()

if validation_result.is_valid:
    print("Configuration is valid")
    # Run simulation
    brain = nlm.createBrain(config)
    brain.initialize()
    brain.step(1000)
else:
    print("Configuration validation errors:")
    for error in validation_result.errors:
        print(f"  {error.code}: {error.message}")
    for warning in validation_result.warnings:
        print(f"  WARNING: {warning.message}")
```

### Configuration CLI Commands

#### Basic Configuration Commands
```bash
# List all configurations
nlm-config list

# Save current configuration
nlm-config save my_research_config

# Load configuration
nlm-config load my_research_config

# Validate configuration
nlm-config validate my_config.json

# Get configuration value
nlm-config get brain.neuron_count

# Set configuration value
nlm-config set brain.neuron_count 2000
```

#### Advanced Configuration Commands
```bash
# Export configuration
nlm-config export my_config.json --format json

# Import configuration
nlm-config import my_config.json

# Compare configurations
nlm-config compare baseline_config my_config.json

# Generate configuration report
nlm-config report my_config.json --output report.html

# Check for configuration conflicts
nlm-config check my_config.json --conflicts

# Validate against schema
nlm-config validate my_config.json --schema advanced_schema.json
```

#### Configuration Templates
```bash
# Create configuration from template
nlm-config from-template research --output my_research_config.json

# List available templates
nlm-config list-templates

# Get template information
nlm-config template-info research
```

### Configuration File Format

#### JSON Format
```json
{
    "nlm_version": "0.1.0",
    "config_version": "2.0",
    "description": "Research configuration for neural learning",
    "tags": ["research", "neurons-2000"],
    
    "random": {
        "seed": 12345,
        "reproducible": true
    },
    
    "network": {
        "neuron_count": 2000,
        "region_count": 3,
        "connection_probability": 0.05
    },
    
    "neuron": {
        "threshold": -50.0,
        "resting_potential": -70.0,
        "refractory_period": 9
    },
    
    "performance": {
        "target_fps": 30,
        "memory_limit_mb": 2048,
        "auto_optimize": true
    }
}
```

#### INI Format
```ini
# NLM Configuration
# Version: 2.0
# Description: Research configuration

[metadata]
version = "2.0"
description = "Research configuration for neural learning"
tags = research,neurons-2000

[random]
seed = 12345
reproducible = true

[network]
neuron_count = 2000
region_count = 3
connection_probability = 0.05

[neuron]
threshold = -50.0
resting_potential = -70.0
refractory_period = 9

[performance]
target_fps = 30
memory_limit_mb = 2048
auto_optimize = true
```

#### YAML Format
```yaml
# NLM Configuration
version: "2.0"
description: "Research configuration for neural learning"
tags:
  - research
  - neurons-2000

random:
  seed: 12345
  reproducible: true

network:
  neuron_count: 2000
  region_count: 3
  connection_probability: 0.05

neuron:
  threshold: -50.0
  resting_potential: -70.0
  refractory_period: 9

performance:
  target_fps: 30
  memory_limit_mb: 2048
  auto_optimize: true
```

### Configuration Migration

#### Migrate from Old Format
```python
import nlm_config

# Migrate old configuration format to new format
old_config = nlm_config.load_old_format("old_config.nml")
new_config = nlm_config.migrate_config(old_config, "2.0")

# Save migrated configuration
new_config.save("migrated_config.json")
```

#### Migration Guide

| Old Parameter | New Parameter | Notes |
|---------------|---------------|-------|
| `plasticity_learning_rate` | `plasticity.learning_rate` | Nested structure |
| `stdp_time_constant` | `stdp.tau` | More descriptive name |
| `development_synaptogenesis` | `development.synaptogenesis_rate` | Descriptive naming |
| `neuromodulation_dopamine_scale` | `neuromodulation.dopamine_scale` | Consistent naming |
| `working_memory_size` | `memory.working_memory_capacity` | More descriptive |

### Configuration Examples

#### Complete Research Configuration
```python
research_config = {
    "metadata": {
        "version": "2.0",
        "description": "Complete research configuration",
        "created": "2026-01-01T00:00:00Z",
        "tags": ["research", "full_simulation"]
    },
    
    "random": {
        "seed": 12345,
        "reproducible": True,
        "algorithm": "mt19937"
    },
    
    "network": {
        "neuron_count": 5000,
        "region_count": 5,
        "connection_probability": 0.08,
        "inter_region_connections": 0.1
    },
    
    "neuron": {
        "threshold": -45.0,
        "resting_potential": -65.0,
        "reset_potential": -70.0,
        "refractory_period": 10,
        "leak_conductance": 15.0,
        "tau_mem": 25.0
    },
    
    "plasticity": {
        "stdp": {
            "learning_rate": 0.01,
            "tau_plus": 20.0,
            "tau_minus": 20.0,
            "ltp_weight": 0.02,
            "ltd_weight": 0.018
        },
        "hebbian": {
            "learning_rate": 0.005,
            "decay_rate": 0.999
        },
        "structural": {
            "synaptogenesis_rate": 0.0005,
            "pruning_rate": 0.0002,
            "max_synapses_per_neuron": 100
        }
    },
    
    "neuromodulation": {
        "dopamine": {
            "scale": 1.2,
            "baseline": 0.1,
            "learning_rate": 0.005
        },
        "curiosity": {
            "enabled": True,
            "scale": 1.0,
            "exploration_bonus": 0.5
        },
        "novelty": {
            "enabled": True,
            "detection_threshold": 0.3
        }
    },
    
    "memory": {
        "working": {
            "capacity": 200,
            "decay_rate": 0.95
        },
        "episodic": {
            "capacity": 5000,
            "replay_rate": 0.02,
            " consolidation_strength": 0.8
        },
        "associative": {
            "strength": 0.2,
            "association_decay": 0.99
        }
    },
    
    "development": {
        "enabled": True,
        "stage_dependent": True,
        "critical_period": {
            "length": 100,
            "plasticity_boost": 2.0
        },
        "maturation": {
            "rate": 0.001,
            "threshold": 1000
        }
    },
    
    "world": {
        "environment": "grid_world",
        "dimensions": {"width": 30, "height": 30},
        "vision": {
            "width": 24,
            "height": 24,
            "channels": 3
        },
        "sensors": {
            "touch_sensors": 64,
            "audio_sensors": 8,
            "internal_sensors": 32
        }
    },
    
    "cognition": {
        "planning": {
            "horizon": 5,
            "lookahead": True
        },
        "attention": {
            "span": 1.0,
            "competitive": True
        },
        "concept_formation": {
            "threshold": 0.6,
            "similarity_threshold": 0.7
        }
    },
    
    "performance": {
        "target_fps": 30,
        "memory_limit_mb": 4096,
        "auto_optimize": True,
        "simd_optimization": True,
        "parallel_processing": True
    },
    
    "logging": {
        "level": "INFO",
        "to_file": True,
        "filename": "nlm_research.log",
        "max_size_mb": 1000,
        "keep_backups": 5
    },
    
    "output": {
        "format": "json",
        "stats_interval": 100,
        "checkpoint_interval": 1000,
        "export_brain": True,
        "export_config": True
    }
}
```

#### Minimal Configuration
```python
minimal_config = {
    "metadata": {
        "version": "1.0",
        "description": "Minimal configuration for testing"
    },
    
    "network": {
        "neuron_count": 100,
        "region_count": 1,
        "connection_probability": 0.1
    }
}
```

### Configuration Best Practices

#### Naming Conventions
- Use kebab-case for top-level sections (e.g., "random-settings")
- Use snake_case for parameters within sections (e.g., "learning_rate")
- Prefix private/experimental parameters with underscore

#### Validation Strategies
1. **Pre-validation**: Check configuration before loading
2. **Runtime validation**: Validate during simulation
3. **Consistency checking**: Ensure interdependent parameters are valid
4. **Performance checking**: Detect potential performance issues

#### Configuration Security
- Restrict access to sensitive configuration parameters
- Log configuration changes for audit trails
- Validate configuration files before execution
- Use encrypted configuration for sensitive experiments

This enhanced configuration system provides comprehensive control over NLM simulations with:

- **Rich parameter validation** with detailed error messages
- **Multiple file format support** (JSON, INI, YAML)
- **Template system** for common configurations
- **Advanced CLI tools** for configuration management
- **Migration path** from older versions
- **Best practice guidelines** for configuration
- **Comprehensive examples** for all use cases