# NLM Configuration Presets

## Overview

This document documents all configuration presets available in NLM, organized by use case and performance characteristics. Each preset is optimized for specific simulation scenarios and can be easily customized.

---

## Quick Start with Presets

```python
import pynlm

# Create specific preset
minimal_brain = pynlm.createBrain(pynlm.createMinimalConfig())
learning_brain = pynlm.createBrain(pynlm.createLearningConfig())
development_brain = pynlm.createBrain(pynlm.createDevelopmentConfig())
performance_brain = pynlm.createBrain(pynlm.createPerformanceConfig())
```

---

## Preset Categories

### 1. Minimal Configurations (Fast, Low Resources)

#### Minimal Configuration
```python
config = pynlm.createMinimalConfig()

# Key characteristics:
# - 500 neurons total (fast simulation)
# - Basic LIF neurons only
# - No plasticity mechanisms enabled
# - Single developmental stage
# - Approximate simulation time: 1-2x real-time

# Primary uses:
# - Testing brain initialization
# - Benchmarking simulation speed
# - Baseline performance measurements
# - Educational demonstrations
```

**Full Configuration:**
```ini
# Core Configuration
brain.neuron_count = 500
brain.synapse_density = 0.05
brain.initial_weight_mean = 0.5
brain.initial_weight_std = 0.1
brain.v_thresh = -50.0
brain.v_rest = -70.0
brain.v_reset = -75.0
brain.tau_m = 20.0
brain.tau_ref = 2.0

# Plasticity Configuration
plasticity.stdp.enable = false
plasticity.hebbian.enable = false
plasticity.structural.enable = false
plasticity.development.enable = false

# Neuromodulation Configuration
neuromod.dopamine.scale = 0.0
neuromod.curiosity.enable = false
neuromod.novelty.enable = false
neuromod.prediction_error.scale = 0.0

# Development Configuration
development.stage_count = 1
development.critical_period_active = false
development.maturation_rate = 0.0

# Memory Configuration
memory.enable_working_memory = false
memory.enable_episodic_memory = false
memory.enable_associative_memory = false
memory.enable_concept_formation = false

# Prediction Configuration
prediction.enable_temporal_prediction = false
prediction.enable_action_consequence = false
prediction.enable_multi_step_prediction = false

# Cognitive Configuration
attention.enable_competitive_selection = false
attention.enable_top_down_bias = false
attention.enable_bottom_up_salience = false

planning.enable_predictive_action_selection = false
self_model.enable_self_awareness = false
social_learning.enable_observation_learning = false
```

#### Fast Simulation Configuration
```python
config = pynlm.createFastConfig()

# Key characteristics:
# - Optimized for speed (event-driven)
# - 1000 neurons with efficient memory
# - STDP only (lighter than Hebbian)
# - Development phases streamlined
# - Approximate simulation time: 0.5-1x real-time

# Performance optimizations:
# - Memory pooling for neurons/synapses
# - Event-driven spike propagation
# - Sparse connectivity representation
# - SIMD vectorization ready
# - Reduced synchronization overhead
```

### 2. Learning Configurations (High Plasticity)

#### Learning Configuration
```python
config = pynlm.createLearningConfig()

# Key characteristics:
# - 2000 neurons for complex learning
# - All plasticity mechanisms enabled
# - Enhanced neuromodulation
# - Full developmental cycle
# - Multiple memory systems
# - Approximate simulation time: 2-5x real-time

# Learning enhancements:
# - Reward-modulated STDP
# - Activity-dependent structural plasticity
# - Critical period mechanisms
# - Novelty-driven exploration
```

**Full Configuration:**
```ini
# Core Configuration (Learning optimized)
brain.neuron_count = 2000
brain.synapse_density = 0.1
brain.initial_weight_mean = 0.5
brain.initial_weight_std = 0.1
brain.v_thresh = -45.0  # Slightly lower threshold for higher activity
brain.v_rest = -70.0
brain.v_reset = -75.0
brain.tau_m = 15.0      # Faster dynamics for learning
brain.tau_ref = 3.0     # Longer refractory for stability

# Advanced Plasticity Configuration
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.01
plasticity.stdp.tau_plus = 20.0
plasticity.stdp.tau_minus = 20.0
plasticity.stdp.reward_modulated = true

plasticity.hebbian.enable = true
plasticity.hebbian.learning_rate = 0.005

plasticity.structural.enable = true
plasticity.structural.formation_rate = 0.02
plasticity.structural.pruning_threshold = 0.05
plasticity.structural.critical_period_boost = true

# Neuromodulation Configuration (Enhanced)
neuromod.dopamine.scale = 1.5
neuromod.dopamine.reward_prediction_error = true

neuromod.curiosity.enable = true
neuromod.curiosity.exploration_strength = 0.8

neuromod.novelty.enable = true
neuromod.novelty.sensitivity = 0.7

neuromod.prediction_error.scale = 1.2
neuromod.prediction_error.decay_rate = 0.05

# Development Configuration (Learning focused)
development.stage_count = 4
development.critical_period.active = true
development.critical_period.start = 0.0
development.critical_period.end = 100.0  # Extended critical period
development.maturation_rate = 0.3
development.plasticity_modulation = true

# Memory Configuration (Learning optimized)
memory.enable_working_memory = true
memory.working_memory.capacity = 50.0
memory.working_memory.stability_decay = 0.1

memory.enable_episodic_memory = true
memory.episodic_memory.encoding_strength = 0.9
memory.episodic_memory.replay_frequency = 0.3

memory.enable_associative_memory = true
memory.associative_memory.association_strength = 0.8

memory.enable_concept_formation = true
memory.concept_formation.prototype_stability = 0.7
memory.concept_formation.category_threshold = 0.6

# Prediction Configuration (Learning required)
prediction.enable_temporal_prediction = true
prediction.temporal_prediction.horizon = 10
prediction.temporal_prediction.accuracy_target = 0.7

prediction.enable_action_consequence = true
prediction.action_consequence.learning_rate = 0.5

prediction.enable_multi_step_prediction = true
prediction.multi_step.planning_depth = 3
prediction.multi_step.error_propagation = true

# Cognitive Configuration (Learning enabled)
attention.enable_competitive_selection = true
attention.competition_strength = 1.2
attention.top_down_influence = 0.6
attention.bottom_up_salience = 0.8

planning.enable_predictive_action_selection = true
planning.prediction_horizon = 5
planning.action_evaluation_weights = { "reward" : 0.5, "novelty" : 0.3, "efficiency" : 0.2 }

self_model.enable_self_awareness = true
self_model.self_model_learning_rate = 0.4
self_model.body_schema_accuracy = 0.8

social_learning.enable_observation_learning = true
social_learning.imitation_threshold = 0.7
social_learning.social_value_weight = 0.6
```

#### Curiosity-Driven Learning Configuration
```python
config = pynlm.createCuriosityConfig()

# Key characteristics:
# - Novelty-driven exploration
# - Curiosity-motivated behavior
# - Adaptive learning rates
# - Frequent exploration phases
# - Approximate simulation time: 3-8x real-time

# Curiosity enhancements:
# - Dynamic novelty thresholds
# - Exploration-exploitation balance
# - Intrinsic motivation systems
# - Environmental exploration strategies
```

### 3. Development Configurations (Developmental Processes)

#### Development Configuration
```python
config = pynlm.createDevelopmentConfig()

# Key characteristics:
# - Full developmental timeline (4 stages)
# - Age-dependent plasticity
# - Critical period mechanisms
# - Synaptogenesis and pruning
# - Extended learning window
# - Approximate simulation time: 3-7x real-time
```

**Developmental Timeline:**
```ini
# Development Configuration

development.stage_count = 4
development.stages = {
    "INITIAL" : {
        "duration" : 0.0,
        "plasticity_multiplier" : 2.0,
        "plasticity_type" : "high",
        "synaptogenesis_rate" : 0.05,
        "pruning_rate" : 0.01,
        "maturation_rate" : 0.0
    },
    
    "CRITICAL_PERIOD" : {
        "duration" : 100.0,
        "plasticity_multiplier" : 1.5,
        "plasticity_type" : "activity_dependent",
        "synaptogenesis_rate" : 0.03,
        "pruning_rate" : 0.02,
        "maturation_rate" : 0.1
    },
    
    "MATURATION" : {
        "duration" : 200.0,
        "plasticity_multiplier" : 0.8,
        "plasticity_type" : "refining",
        "synaptogenesis_rate" : 0.01,
        "pruning_rate" : 0.04,
        "maturation_rate" : 0.5
    },
    
    "ADULT" : {
        "duration" : 300.0,
        "plasticity_multiplier" : 0.5,
        "plasticity_type" : "maintenance",
        "synaptogenesis_rate" : 0.001,
        "pruning_rate" : 0.03,
        "maturation_rate" : 0.8
    },
    
    "AGING" : {
        "duration" : 100.0,
        "plasticity_multiplier" : 0.3,
        "plasticity_type" : "declining",
        "synaptogenesis_rate" : 0.0,
        "pruning_rate" : 0.01,
        "maturation_rate" : 0.9
    }
}

development.adaptation_mechanisms = [
    "homeostatic_plasticity",
    "activity_dependent_pruning",
    "resource_allocation"
]

development.learning_windows = {
    "critical_period" : true,
    "sensitive_periods" : true,
    "continuous_learning" : true
}
```

#### Lifelong Learning Configuration
```python
config = pynlm.createLifelongLearningConfig()

# Key characteristics:
# - Extended development for lifelong learning
# - Continual adaptation to new experiences
# - Memory consolidation mechanisms
# - Skills maintenance and refinement
# - Approximate simulation time: 5-15x real-time
```

### 4. Performance Configurations (High Performance)

#### Performance Configuration
```python
config = pynlm.createPerformanceConfig()

# Key characteristics:
# - Large-scale simulations
# - Optimized for computational resources
# - Advanced memory management
# - Parallel processing ready
# - Approximate simulation time: 0.3-0.8x real-time
```

**Performance Optimizations:**
```ini
# Core Configuration (Performance optimized)
brain.neuron_count = 10000
brain.synapse_density = 0.05
brain.initial_weight_mean = 0.5
brain.initial_weight_std = 0.1

# Memory Configuration (Optimized)
memory.pools.enable_neuron_pool = true
memory.pools.neuron_pool_size = 10000
memory.pools.enable_synapse_pool = true
memory.pools.synapse_pool_size = 500000
memory.pools.pool_allocation_strategy = "best_fit"

# Event System (Optimized)
events.queue_type = "lock_free_ring_buffer"
events.bucket_count = 256
events.time_bucket_size = 10.0

# Connectivity (Optimized)
connectivity.type = "adjacency_list"
connectivity.undirected_connections = true
connectivity.compression_enabled = true

# Processing (Parallel ready)
processing.thread_pool_size = 8
processing.enable_vectorization = true
processing.simd_instruction_set = "avx512"

# Plasticity (Streamlined)
plasticity.update_frequency = "event_driven"
plasticity.batch_updates = true
plasticity.update_threads = 4
```

### 5. Research Configurations (Scientific Control)

#### Research Configuration
```python
config = pynlm.createResearchConfig()

# Key characteristics:
# - Scientific control and reproducibility
# - Precise parameter measurement
# - Hypothesis testing ready
# - A/B comparison support
# - Statistical analysis integration
# - Approximate simulation time: 1-3x real-time
```

**Research Features:**
```ini
# Scientific Control Configuration
research.control.enabled = true
research.control.random_seed = 42
research.control.reproducibility_level = "high"

research.hypothesis.testing.hypothesis_count = 10
research.hypothesis.control_groups = 3
research.hypothesis.experimental_groups = 3

research.measurement.temporal_resolution = 0.1
research.measurement.spatial_resolution = 0.5
research.measurement.sampling_frequency = 100.0

research.analysis.statistical_tests = [
    "t_test", "anova", "correlation", "regression"
]
research.analysis.confidence_level = 0.95

# Recording Configuration
recording.enabled = true
recording.data_format = "hdf5"
recording.compression.enabled = true
recording.compression.algorithm = "lz4"

recording.sampling_rate = 1000.0
recording.adaptive_sampling.enabled = true
recording.adaptive_sampling.threshold = 0.01
```

---

## Preset Management API

### Accessing Presets

```python
import pynlm

# Get preset configuration
minimal = pynlm.getConfigPreset("minimal")
learning = pynlm.getConfigPreset("learning")
development = pynlm.getConfigPreset("development")
performance = pynlm.getConfigPreset("performance")
research = pynlm.getConfigPreset("research")

# Get preset names
preset_names = pynlm.listConfigPresets()
```

### Preset Information

```python
import pynlm

# Get preset metadata
minimal_info = pynlm.getPresetInfo("minimal")
print(f"Preset: {minimal_info.name}")
print(f"Neurons: {minimal_info.neuron_count}")
print(f"Simulation time factor: {minimal_info.simulation_time_factor}")
print(f"Key features: {', '.join(minimal_info.features)}")
print(f"Use cases: {', '.join(minimal_info.use_cases)}")

# Compare presets
comparison = pynlm.comparePresets(["minimal", "learning", "performance"])
for preset_name, metrics in comparison.items():
    print(f"\n{preset_name}:")
    for metric, value in metrics.items():
        print(f"  {metric}: {value}")
```

### Custom Presets

```python
import pynlm

# Create custom preset
custom_config = pynlm.createCustomConfig()
custom_config.set("brain.neuron_count", 1500)
custom_config.set("plasticity.stdp.enable", True)
custom_config.set("plasticity.hebbian.enable", True)
custom_config.set("neuromod.curiosity.enable", True)

# Save custom preset
pynlm.saveConfigPreset("my_custom_brain", custom_config, 
                      description="Custom learning brain with curiosity")

# Load custom preset
loaded_preset = pynlm.loadConfigPreset("my_custom_brain")
```

---

## Preset Recommendations

### When to Use Each Preset

| Preset | Best For | Simulation Time | Resource Usage |
|--------|----------|-----------------|----------------|
| **Minimal** | Quick testing, educational demos | 1-2x real-time | Low |
| **Fast** | Performance benchmarking, large-scale studies | 0.5-1x real-time | Medium |
| **Learning** | Standard learning research, skill acquisition | 2-5x real-time | High |
| **Curiosity** | Exploration behavior studies, intrinsic motivation | 3-8x real-time | Very High |
| **Development** | Developmental psychology, maturation studies | 3-7x real-time | High |
| **Lifelong Learning** | Continual learning, transfer learning | 5-15x real-time | Very High |
| **Performance** | Large brain simulations, high-performance computing | 0.3-0.8x real-time | Very High |
| **Research** | Scientific experiments, hypothesis testing | 1-3x real-time | High |

### Preset Comparison Matrix

| Feature | Minimal | Fast | Learning | Curiosity | Development | Performance | Research |
|---------|---------|------|----------|-----------|-------------|-------------|----------|
| **Neurons** | 500 | 1000 | 2000 | 2000 | 2000 | 10000+ | 1000 |
| **Plasticity** | None | STDP only | All | All + Adaptive | All + Critical | All | All |
| **Memory** | None | Working | Full | Full | Full | Full | Full |
| **Neuromodulation** | None | Basic | Full | Curiosity | Full | Full | Full |
| **Development** | 1 stage | 2 stages | 4 stages | 4 stages | 4 stages | 4 stages | 4 stages |
| **Prediction** | None | None | Full | Full | Full | Full | Full |
| **Performance** | Slow | Fast | Medium | Slow | Medium | Fast | Medium |
| **Use Case** | Demo | Benchmark | Standard | Exploration | Maturation | Scale | Science |

### Preset Customization Tips

1. **For Educational Use**: Start with `minimal` preset and gradually enable subsystems
2. **For Research**: Use `research` preset for reproducibility and statistical analysis
3. **For Large-Scale Studies**: Use `performance` preset for efficiency
4. **For Novel Behavior**: Use `curiosity` preset for exploration-driven learning
5. **For Developmental Studies**: Use `development` or `lifelong_learning` presets
6. **For Standard Learning**: Use `learning` preset for balanced capabilities

---

## Preset Migration Guide

### Between Versions

```python
# Migrate old preset to new preset
old_config = pynlm.loadConfigPreset("legacy_brain")
migrated_config = pynlm.migratePreset(old_config, from_version="0.4", to_version="0.5")
pynlm.saveConfigPreset("migrated_brain", migrated_config)

# Check for deprecated settings
deprecated_settings = pynlm.checkDeprecatedSettings(old_config)
for setting in deprecated_settings:
    print(f"Deprecated: {setting}")
    print(f"Replacement: {pynlm.getReplacementSetting(setting)}")
```

### Performance Tuning Based on Preset

```python
import pynlm

# Start with a preset
config = pynlm.createLearningConfig()

# Profile the simulation
profile = pynlm.profileSimulation(config)
print(f"Bottleneck: {profile.bottleneck_component}")
print(f"Memory usage: {profile.memory_usage} MB")
print(f"CPU time: {profile.cpu_time} seconds")

# Adjust based on profile
if profile.bottleneck == "memory":
    config.set("memory.pools.enable_neuron_pool", True)
elif profile.bottleneck == "plasticity":
    config.set("plasticity.update_frequency", "event_driven")
elif profile.bottleneck == "prediction":
    config.set("prediction.enable_multi_step_prediction", False)
```

## Preset Validation

### Configuration Validation

```python
import pynlm

# Validate preset configuration
validation_result = pynlm.validatePresetConfig("learning")

if not validation_result.is_valid:
    print("Preset validation failed:")
    for error in validation_result.errors:
        print(f"  - {error}")
    
    print("\nSuggested fixes:")
    for fix in validation_result.suggestions:
        print(f"  - {fix}")
else:
    print("Preset configuration is valid!")
    print(f"Recommended neuron count: {validation_result.recommended_neuron_count}")
    print(f"Estimated simulation time: {validation_result.estimated_simulation_time}x real-time")
```

### Preset Testing

```python
import pynlm

# Test preset produces expected behavior
test_results = pynlm.testPreset("learning")

print(f"Preset: {test_results.preset_name}")
print(f"Tests passed: {test_results.passed_tests}/{test_results.total_tests}")

for test_name, result in test_results.test_results.items():
    status = "✓" if result.passed else "✗"
    print(f"{status} {test_name}: {result.description}")
    if not result.passed:
        print(f"  Issue: {result.error_message}")

# Get preset recommendations
recommendations = pynlm.getPresetRecommendations()
for preset_name, recommendation in recommendations.items():
    print(f"\n{preset_name}: {recommendation.reason}")
```

---

## Future Preset Developments

### Planned Presets (Phase 5+)

1. **Social Learning Preset**: Enhanced social interaction and cultural transmission
2. **Meta-Learning Preset**: Learning how to learn, adaptive learning rates
3. **Transfer Learning Preset**: Knowledge transfer across tasks and domains
4. **Hierarchical Learning Preset**: Multi-level skill acquisition
5. **Mirror Neuron Preset**: Imitation and observation learning enhancement

### Preset Evolution Path

NLM configuration presets will evolve to support:

- **Modular Presets**: Combine base presets with specific feature modules
- **Parameter Sweeps**: Automated preset generation for parameter exploration
- **Machine Learning Optimized**: Presets designed for reinforcement learning
- **Resource-Constrained Presets**: TinyML and embedded device variants
- **Domain-Specific Presets**: Neuroscience, psychology, robotics specializations

---

## Preset Summary

NLM configuration presets provide optimized starting points for different research and application scenarios. Whether you need a quick demo, high-performance simulation, or scientific control, there's a preset designed for your needs.

Choose your preset based on:
- Required simulation capabilities
- Available computational resources
- Research questions being investigated
- Desired level of realism
- Performance requirements

The presets guide you from simple demonstrations to complex, large-scale simulations while maintaining scientific rigor and reproducibility.