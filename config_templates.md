# NLM Configuration Templates

This file contains configuration templates and examples for different use cases and scenarios.

## Quick Start Templates

### Template 1: Minimal Research Configuration
```json
{
  "name": "Minimal Research",
  "description": "Small brain for quick research experiments",
  "neuron_count": 50,
  "region_count": 1,
  "connection_probability": 0.1,
  "simulation_timestep": 0.001,
  "random_seed": 42,
  "stdp_ltp_weight": 0.01,
  "stdp_ltd_weight": 0.012,
  "synaptogenesis_rate": 0.00001,
  "pruning_rate": 0.000001
}
```

### Template 2: Standard Research Configuration
```json
{
  "name": "Standard Research",
  "description": "Balanced configuration for research experiments",
  "neuron_count": 500,
  "region_count": 2,
  "connection_probability": 0.15,
  "simulation_timestep": 0.001,
  "random_seed": 123,
  "stdp_ltp_weight": 0.02,
  "stdp_ltd_weight": 0.015,
  "stdp_tau": 20.0,
  "synaptogenesis_rate": 0.0001,
  "pruning_rate": 0.00001,
  "enable_checkpointing": true,
  "enable_developmental_plasticity": true
}
```

### Template 3: Large-Scale Simulation Configuration
```json
{
  "name": "Large-Scale Simulation",
  "description": "Large brain for complex simulation scenarios",
  "neuron_count": 2000,
  "region_count": 4,
  "connection_probability": 0.08,
  "simulation_timestep": 0.0005,
  "random_seed": 456,
  "stdp_ltp_weight": 0.015,
  "stdp_ltd_weight": 0.013,
  "stdp_tau": 25.0,
  "synaptogenesis_rate": 0.0005,
  "pruning_rate": 0.00005,
  "enable_checkpointing": true,
  "enable_replay": true,
  "checkpoint_path": "/tmp/nlm_large_simulation.bin"
}
```

## Advanced Configuration Templates

### Template 4: Neuromodulation-Focused Configuration
```json
{
  "name": "Neuromodulation Research",
  "description": "Configuration optimized for studying neuromodulation effects",
  "neuron_count": 1000,
  "region_count": 3,
  "connection_probability": 0.12,
  "simulation_timestep": 0.001,
  "random_seed": 789,
  "stdp_ltp_weight": 0.025,
  "stdp_ltd_weight": 0.018,
  "stdp_tau": 30.0,
  "synaptogenesis_rate": 0.0002,
  "pruning_rate": 0.00002,
  "enable_reward_modulation": true,
  "enable_curiosity": true,
  "enable_novelty_detection": true,
  "enable_prediction_error": true,
  "enable_development": true,
  "development_start_step": 1000,
  "development_end_step": 5000
}
```

### Template 5: Memory Integration Configuration
```json
{
  "name": "Memory Integration Research",
  "description": "Configuration focused on memory system research",
  "neuron_count": 1500,
  "region_count": 2,
  "connection_probability": 0.2,
  "simulation_timestep": 0.001,
  "random_seed": 999,
  "stdp_ltp_weight": 0.018,
  "stdp_ltd_weight": 0.016,
  "stdp_tau": 15.0,
  "synaptogenesis_rate": 0.0003,
  "pruning_rate": 0.00003,
  "enable_working_memory": true,
  "enable_episodic_memory": true,
  "enable_associative_memory": true,
  "enable_structural_plasticity": true,
  "replay_interval": 100,
  "replay_duration": 50,
  "replay_strength": 0.5
}
```

### Template 6: Development and Plasticity Configuration
```json
{
  "name": "Development Research",
  "description": "Configuration optimized for studying developmental processes",
  "neuron_count": 800,
  "region_count": 2,
  "connection_probability": 0.18,
  "simulation_timestep": 0.001,
  "random_seed": 333,
  "stdp_ltp_weight": 0.022,
  "stdp_ltd_weight": 0.019,
  "stdp_tau": 18.0,
  "synaptogenesis_rate": 0.001,
  "pruning_rate": 0.0001,
  "enable_development": true,
  "developmental_stage_initial": "Initial",
  "developmental_stage_critical_period": "CriticalPeriod",
  "developmental_stage_maturation": "Maturation",
  "developmental_stage_adult": "Adult",
  "developmental_stage_aging": "Aging",
  "plasticity_modulation_by_age": true,
  "structural_plasticity_by_stage": true,
  "excitability_changes_with_development": true
}
```

## Environment-Specific Templates

### Template 7: Resource-Constrained Environment
```json
{
  "name": "Resource-Constrained",
  "description": "Configuration for limited computational resources",
  "neuron_count": 100,
  "region_count": 1,
  "connection_probability": 0.05,
  "simulation_timestep": 0.002,
  "random_seed": 555,
  "stdp_ltp_weight": 0.01,
  "stdp_ltd_weight": 0.009,
  "stdp_tau": 10.0,
  "synaptogenesis_rate": 0.00001,
  "pruning_rate": 0.000001,
  "enable_checkpointing": true,
  "checkpoint_interval": 1000,
  "enable_replay": false,
  "max_memory_usage_mb": 512,
  "performance_target_ms_per_step": 1.0
}
```

### Template 8: High-Performance Environment
```json
{
  "name": "High-Performance",
  "description": "Configuration for high-performance computing environments",
  "neuron_count": 5000,
  "region_count": 8,
  "connection_probability": 0.06,
  "simulation_timestep": 0.0001,
  "random_seed": 777,
  "stdp_ltp_weight": 0.02,
  "stdp_ltd_weight": 0.017,
  "stdp_tau": 20.0,
  "synaptogenesis_rate": 0.0008,
  "pruning_rate": 0.00008,
  "enable_checkpointing": true,
  "enable_replay": true,
  "checkpoint_interval": 500,
  "parallel_processing": true,
  "use_memory_pools": true,
  "simd_optimization": true,
  "max_memory_usage_mb": 2048,
  "performance_target_ms_per_step": 0.1
}
```

## Quick Reference

### Most Common Templates

| Template Name | Neurons | Regions | Use Case |
|---------------|---------|---------|----------|
| `minimal` | 50 | 1 | Quick testing, education |
| `standard` | 500 | 2 | Standard research |
| `large` | 2000 | 4 | Complex simulations |
| `neuromodulation` | 1000 | 3 | Neuromodulation research |
| `memory` | 1500 | 2 | Memory system research |
| `development` | 800 | 2 | Developmental research |
| `resource-constrained` | 100 | 1 | Limited computing resources |
| `high-performance` | 5000 | 8 | High-performance computing |

### Template Usage Examples

```python
# Import the template manager
from config_templates import template_manager

# List available templates
templates = template_manager.list_templates()

# Get a specific template
minimal_config = template_manager.get_template("minimal")

# Create a brain using a template
brain = template_manager.create_brain_from_template("standard")

# Save a custom template
custom_config = {
    "name": "My Custom Research",
    "description": "My specific research configuration",
    "neuron_count": 300,
    "region_count": 2,
    "connection_probability": 0.12,
    "simulation_timestep": 0.001
}
template_manager.save_template("my_custom", custom_config)
```

## Configuration Parameter Guide

### Core Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | integer | 100 | Number of neurons in the brain |
| `region_count` | integer | 1 | Number of neural regions |
| `connection_probability` | float | 0.15 | Probability of synapse formation |
| `simulation_timestep` | float | 0.001 | Time step for simulation (seconds) |
| `random_seed` | integer | 42 | Random seed for reproducibility |

### Plasticity Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `stdp_ltp_weight` | float | 0.02 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.015 | STDP long-term depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant |
| `synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `pruning_rate` | float | 0.00001 | Rate of synapse elimination |

### Advanced Features

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `enable_checkpointing` | boolean | false | Enable checkpoint/save functionality |
| `enable_developmental_plasticity` | boolean | false | Enable age-dependent plasticity |
| `enable_replay` | boolean | false | Enable memory replay functionality |
| `enable_reward_modulation` | boolean | false | Enable reward-based neuromodulation |
| `enable_curiosity` | boolean | false | Enable curiosity-driven exploration |
| `enable_novelty_detection` | boolean | false | Enable novelty detection |
| `enable_prediction_error` | boolean | false | Enable prediction error signaling |

## Performance Optimization Tips

1. **For Resource-Constrained Systems**: Use `resource-constrained` template
2. **For Research**: Start with `minimal` or `standard` templates
3. **For Large Networks**: Use `high-performance` template
4. **For Neuromodulation Research**: Use `neuromodulation` template
5. **For Development Studies**: Use `development` template

## File Format Notes

- All templates are JSON format for easy reading and editing
- Templates include descriptive names and use case information
- Parameters are well-documented with typical values
- Templates can be easily customized by modifying values
- All templates include essential parameters for basic operation

## Getting Started

1. Copy the template that matches your use case
2. Modify parameters as needed
3. Load the template into your NLM project
4. Run your simulation
5. Save results for comparison

This collection of templates provides a comprehensive starting point for NLM research, making it easy to set up simulations for different purposes and computational constraints.
