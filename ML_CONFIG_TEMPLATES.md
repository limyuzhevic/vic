# Machine Learning Configuration Templates for NLM

This directory contains configuration templates optimized for machine learning workflows with the NLM neural network simulation framework.

## Overview

These templates are designed to help researchers and practitioners integrate NLM with their machine learning pipelines, providing standardized configurations for:

- Training neural network simulations
- Experimental parameter sweeps
- Reproducible research
- Batch processing workflows
- Model comparison studies

## Templates Available

### 1. `training_config.json`
Configuration for training neural network simulations with learning loops.

```json
{
  "name": "Training Configuration",
  "description": "Optimized for neural network training and learning experiments",
  "simulation": {
    "timestep": 0.001,
    "duration": 10.0,
    "neuron_count": 500,
    "region_count": 2,
    "connection_probability": 0.15
  },
  "plasticity": {
    "hebbian": true,
    "stdp": true,
    "reward_modulated": false,
    "structural": true,
    "learning_rate": 0.001,
    "plasticity_threshold": 0.01
  },
  "neuromodulation": {
    "dopamine_enabled": true,
    "curiosity_enabled": true,
    "novelty_detection": true,
    "prediction_error": true
  },
  "development": {
    "developmental_stages": 5,
    "plasticity_decay": 0.95,
    "critical_period_multiplier": 2.0
  },
  "recording": {
    "record_spikes": true,
    "record_weights": true,
    "record_behavior": true,
    "output_frequency": "every_step"
  },
  "output": {
    "format": "json",
    "compress": true,
    "include_visualizations": true,
    "analysis_level": "detailed"
  }
}
```

### 2. `experiment_sweep.json`
Configuration for parameter sweep experiments.

```json
{
  "name": "Parameter Sweep Configuration",
  "description": "Configuration for systematic parameter exploration",
  "sweep_parameters": {
    "neuron_count": [100, 200, 500, 1000],
    "connection_probability": [0.05, 0.1, 0.15, 0.2],
    "plasticity_learning_rate": [0.0001, 0.001, 0.01],
    "stdp_tau": [10.0, 20.0, 50.0],
    "developmental_stages": [2, 3, 4, 5]
  },
  "simulation": {
    "timestep": 0.001,
    "duration": 5.0,
    "repetitions": 3
  },
  "analysis": {
    "metrics": ["firing_rate", "weight_change", "learning_efficiency", "network_connectivity"],
    "output_format": "csv",
    "generate_plots": true,
    "statistical_tests": ["anova", "t_test", "correlation"]
  },
  "output": {
    "directory": "experiment_results",
    "prefix": "sweep_",
    "compress": false
  }
}
```

### 3. `comparative_study.json`
Configuration for comparing different network architectures.

```json
{
  "name": "Comparative Study Configuration",
  "description": "Configuration for comparing different neural network architectures",
  "architectures": {
    "standard": {
      "neuron_count": 500,
      "region_count": 2,
      "connection_probability": 0.15,
      "plasticity_rules": ["hebbian", "stdp"]
    },
    "scale_free": {
      "neuron_count": 500,
      "region_count": 2,
      "connection_probability": 0.05,
      "plasticity_rules": ["stdp"],
      "target_degree_distribution": "power_law"
    },
    "small_world": {
      "neuron_count": 500,
      "region_count": 3,
      "connection_probability": 0.1,
      "plasticity_rules": ["hebbian"],
      "small_world_parameter": 3.0
    },
    "modular": {
      "neuron_count": 500,
      "region_count": 4,
      "connection_probability": 0.2,
      "inter_region_connections": 0.3,
      "intra_region_connections": 0.8
    }
  },
  "simulation": {
    "timestep": 0.001,
    "duration": 10.0,
    "repetitions": 5
  },
  "comparison": {
    "metrics": ["learning_speed", "final_performance", "stability", "robustness"],
    "statistical_analysis": true,
    "effect_size_calculation": true,
    "confidence_intervals": true
  },
  "output": {
    "format": "multi_file",
    "separate_directories": true,
    "include_visualizations": true,
    "generate_summary_report": true
  }
}
```

### 4. `reinforcement_learning.json`
Configuration for reinforcement learning scenarios.

```json
{
  "name": "Reinforcement Learning Configuration",
  "description": "Configuration for reinforcement learning with neuromodulation",
  "rl": {
    "reward_function": "prediction_error",
    "action_space_size": 10,
    "exploration_strategy": "epsilon_greedy",
    "epsilon_initial": 1.0,
    "epsilon_decay": 0.995,
    "epsilon_min": 0.01
  },
  "neuromodulation": {
    "dopamine": {
      "enabled": true,
      "sensitivity": 1.0,
      "adaptation_rate": 0.01
    },
    "curiosity": {
      "enabled": true,
      "novelty_threshold": 0.5,
      "exploration_bonus": 0.1
    },
    "prediction_error": {
      "enabled": true,
      "learning_signal": "td_error",
      "weight": 1.0
    }
  },
  "environment": {
    "reward_range": [-1.0, 1.0],
    "reset_on_episode_end": true,
    "max_steps_per_episode": 1000
  },
  "training": {
    "episodes": 1000,
    "steps_per_episode": 100,
    "batch_size": 32,
    "target_update_frequency": 10
  },
  "output": {
    "track_rewards": true,
    "track_exploration": true,
    "save_models": true,
    "generate_learning_curves": true
  }
}
```

### 5. `continual_learning.json`
Configuration for continual learning scenarios.

```json
{
  "name": "Continual Learning Configuration",
  "description": "Configuration for continual learning with interference management",
  "tasks": {
    "num_tasks": 10,
    "task_duration": 50,
    "task_types": ["pattern_recognition", "sequence_learning", "spatial_navigation"],
    "task_difficulty": "increasing"
  },
  "catastrophic_interference": {
    "enabled": true,
    "mitigation_strategy": "elastic_weight_consolidation",
    "lambda": 1.0,
    "gamma": 0.9
  },
  "plasticity": {
    "learning_rate": 0.001,
    "plasticity_decay": 0.998,
    "plasticity_threshold": 0.01,
    "structural_plasticity": true
  },
  "memory": {
    "working_memory_size": 20,
    "episodic_memory_capacity": 100,
    "consolidation_frequency": "per_task"
  },
  "output": {
    "track_forgetting": true,
    "track_interference": true,
    "save_task_performance": true,
    "generate_forgetting_curves": true
  }
}
```

### 6. `development_template.json`
Configuration for studying neural development.

```json
{
  "name": "Neural Development Configuration",
  "description": "Configuration for simulating neural development over time",
  "developmental_stages": {
    "stage_count": 10,
    "stage_durations": [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
    "plasticity_rates": [0.1, 0.2, 0.3, 0.4, 0.5, 0.4, 0.3, 0.2, 0.1, 0.05]
  },
  "synaptogenesis": {
    "enabled": true,
    "rate": 0.01,
    "initial_synaptogenesis_phase": "high",
    "pruning_phase": "late_adolescence"
  },
  "maturation": {
    "excitatory_inhibitory_balance": "dynamic",
    "myelination_rate": 0.8,
    "connection_stabilization": true
  },
  "output": {
    "track_development_trajectory": true,
    "record_stage_characteristics": true,
    "generate_development_plots": true,
    "save_intermediate_states": true
  }
}
```

### 7. `performance_benchmark.json`
Configuration for performance benchmarking.

```json
{
  "name": "Performance Benchmark Configuration",
  "description": "Configuration for performance benchmarking and optimization",
  "benchmarks": {
    "neural_initialization": {
      "neuron_count": [100, 500, 1000, 5000],
      "measurements": ["initialization_time", "memory_usage", "cpu_cycles"]
    },
    "simulation_steps": {
      "neuron_count": [500, 1000],
      "steps": [100, 1000, 10000],
      "measurements": ["time_per_step", "spike_rate", "memory_growth"]
    },
    "plasticity_updates": {
      "synapse_count": [1000, 5000, 10000],
      "update_frequency": ["every_step", "every_10_steps", "every_100_steps"],
      "measurements": ["update_time", "learning_effectiveness", "stability"]
    }
  },
  "hardware_profiling": {
    "measure_memory_bandwidth": true,
    "measure_cpu_usage": true,
    "measure_energy_consumption": true
  },
  "output": {
    "format": "benchmark_results.csv",
    "compress": false,
    "generate_plots": true,
    "statistical_analysis": true,
    "performance_summary": true
  }
}
```

## Usage Examples

### Using Training Configuration

```bash
# Run with training configuration
nlm run --config configs/training_config.json

# Custom training configuration
nlm run --config configs/training_config.json --steps 5000 --neuron_count 1000
```

### Running Parameter Sweeps

```bash
# Run parameter sweep
nlm run --config configs/experiment_sweep.json

# Create experiment directory
mkdir -p experiment_results
```

### Running Comparative Studies

```bash
# Run comparative study
nlm run --config configs/comparative_study.json

# Analyze results
nlm analyze --output comparison_results.json
```

## File Format Notes

- All JSON files use double quotes for strings
- Numbers are represented as JSON numbers (no quotes)
- Boolean values are lowercase `true`/`false`
- Arrays are comma-separated and enclosed in square brackets
- Objects are key-value pairs enclosed in curly braces

## Best Practices

1. **Version Control**: Keep configuration files under version control for reproducibility
2. **Comments**: Use JSON comments if your parser supports them
3. **Modular Design**: Break large configurations into smaller, focused templates
4. **Validation**: Validate configuration files before running simulations
5. **Documentation**: Document the purpose and expected behavior of each configuration

## Integration with NLM

These templates are designed to work seamlessly with the NLM framework:

- Compatible with the new JSON configuration system
- Support the enhanced CLI interface
- Integrate with debugging and profiling tools
- Work with visualization capabilities
- Optimized for machine learning workflows

## Future Enhancements

We plan to add:

1. **Template Inheritance**: Base templates that can be extended
2. **Validation Schema**: JSON schema validation for configurations
3. **Automated Parameter Optimization**: Integration with optimization libraries
4. **Cloud Computing Support**: Configuration for distributed computing
5. **Experiment Tracking**: Integration with experiment tracking systems

These templates provide a solid foundation for machine learning workflows with NLM, making it easier to:

- Run systematic experiments
- Compare different architectures
- Reproduce research results
- Integrate with existing machine learning pipelines
- Analyze and visualize neural network behavior

The templates are designed to be both flexible and structured, allowing researchers and practitioners to easily customize them for their specific needs while maintaining good experimental practices.
