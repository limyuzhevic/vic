#!/usr/bin/env python3
"""
NLM Configuration - Sample JSON Configuration Files

This file contains sample configuration files in JSON format that demonstrate
the new configuration capabilities in the NLM (熙然) project.

These configurations can be used as starting points for creating custom
NLM brain simulations with specific requirements.
"""

import json
import os
from pathlib import Path

def create_sample_configs():
    """Create sample configuration files in JSON format"""
    
    # Ensure configs directory exists
    configs_dir = Path("configs")
    configs_dir.mkdir(exist_ok=True)
    
    # 1. Basic Configuration
    basic_config = {
        "simulation": {
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "neuron_count": 500,
            "region_count": 1,
            "connection_probability": 0.15
        },
        "neural_dynamics": {
            "lif_resting_potential": -70.0,
            "lif_threshold": -55.0,
            "lif_reset_potential": -70.0,
            "lif_leak_conductance": 10.0,
            "lif_refractory_period": 9
        },
        "plasticity": {
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.015,
            "stdp_tau": 20.0,
            "hebbian_learning_rate": 0.01
        },
        "environment": {
            "width": 16,
            "height": 16,
            "vision_width": 16,
            "vision_height": 16,
            "max_energy": 100,
            "energy_decay_rate": 0.01
        },
        "agent": {
            "enable_reward_modulation": True,
            "enable_structural_plasticity": True,
            "enable_development": True,
            "enable_curiosity": True
        }
    }
    
    # Save basic configuration
    with open(configs_dir / "basic_config.json", "w") as f:
        json.dump(basic_config, f, indent=2)
    
    # 2. High-Performance Configuration
    high_perf_config = {
        "simulation": {
            "random_seed": 123,
            "simulation_timestep": 0.0005,
            "neuron_count": 2000,
            "region_count": 4,
            "connection_probability": 0.08
        },
        "neural_dynamics": {
            "lif_resting_potential": -70.0,
            "lif_threshold": -55.0,
            "lif_reset_potential": -70.0,
            "lif_leak_conductance": 15.0,
            "lif_refractory_period": 7
        },
        "plasticity": {
            "stdp_ltp_weight": 0.025,
            "stdp_ltd_weight": 0.02,
            "stdp_tau": 30.0,
            "hebbian_learning_rate": 0.015,
            "synaptogenesis_rate": 0.00001,
            "pruning_rate": 0.000001
        },
        "memory": {
            "working_memory_capacity": 5000,
            "episodic_memory_capacity": 3000,
            "associative_memory_capacity": 4000
        },
        "neuromodulation": {
            "rpe_learning_rate": 0.02,
            "curiosity_decay_rate": 0.0005,
            "curiosity_threshold": 0.3,
            "novelty_threshold": 0.1
        },
        "development": {
            "developmental_stage_duration": 2000,
            "plasticity_decay_rate": 0.002,
            "excitability_growth_rate": 0.015
        }
    }
    
    # Save high-performance configuration
    with open(configs_dir / "high_performance_config.json", "w") as f:
        json.dump(high_perf_config, f, indent=2)
    
    # 3. Research Configuration
    research_config = {
        "simulation": {
            "random_seed": 999,
            "simulation_timestep": 0.001,
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.12
        },
        "neural_dynamics": {
            "lif_resting_potential": -75.0,
            "lif_threshold": -50.0,
            "lif_reset_potential": -75.0,
            "lif_leak_conductance": 12.0,
            "lif_refractory_period": 10
        },
        "plasticity": {
            "stdp_ltp_weight": 0.015,
            "stdp_ltd_weight": 0.018,
            "stdp_tau": 25.0,
            "hebbian_learning_rate": 0.005,
            "synaptogenesis_rate": 0.00003,
            "pruning_rate": 0.000008
        },
        "memory": {
            "working_memory_capacity": 2000,
            "episodic_memory_capacity": 1500,
            "associative_memory_capacity": 2500
        },
        "neuromodulation": {
            "rpe_learning_rate": 0.01,
            "curiosity_decay_rate": 0.001,
            "curiosity_threshold": 0.5,
            "novelty_threshold": 0.05
        },
        "development": {
            "developmental_stage_duration": 3000,
            "plasticity_decay_rate": 0.001,
            "excitability_growth_rate": 0.01
        },
        "control": {
            "max_steps": 50000,
            "checkpoint_interval": 5000,
            "replay_interval": 500,
            "consolidation_interval": 10000
        }
    }
    
    # Save research configuration
    with open(configs_dir / "research_config.json", "w") as f:
        json.dump(research_config, f, indent=2)
    
    # 4. Simple Configuration (for backward compatibility)
    simple_config = {
        "simulation": {
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "neuron_count": 500
        },
        "plasticity": {
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.015
        }
    }
    
    # Save simple configuration
    with open(configs_dir / "simple_config.json", "w") as f:
        json.dump(simple_config, f, indent=2)
    
    # Create a README file explaining the configuration files
    readme_content = """# NLM Configuration Files

This directory contains sample configuration files for the NLM (熙然) brain simulation project.

## Configuration Formats

NLM supports multiple configuration file formats:

1. **JSON Configuration** (recommended)
   - Modern, structured format
   - Easy to validate and programmatically access
   - Supports complex nested structures
   - Example: `basic_config.json`, `research_config.json`

2. **Simple Key=Value Format** (legacy)
   - Traditional format used in Phase 1
   - Backward compatible with existing systems
   - Example: `configs/default.cfg`

## Configuration Files

### simple_config.json
A minimal JSON configuration for basic NLM simulations.
Use this as a starting point for simple experiments.

### basic_config.json
A complete configuration with standard settings for most NLM simulations.
Includes neural dynamics, plasticity, memory, and neuromodulation settings.

### high_performance_config.json
Optimized for large-scale simulations with enhanced memory and neuromodulation.
Use this configuration for research-level simulations with 2000+ neurons.

### research_config.json
Advanced configuration for detailed research simulations.
Includes long simulation runs, comprehensive checkpointing, and replay systems.

## Usage

### Using JSON Configuration Files

```bash
# Set a configuration file
./nlm --config configs/basic_config.json

# Override settings from command line
./nlm --config configs/basic_config.json --neuron_count=1000 --stdp_ltp_weight=0.03
```

### Configuration Format Examples

#### Basic Settings
```json
{
  "simulation": {
    "random_seed": 42,
    "neuron_count": 500,
    "connection_probability": 0.15
  }
}
```

#### Neural Dynamics
```json
{
  "neural_dynamics": {
    "lif_resting_potential": -70.0,
    "lif_threshold": -55.0,
    "lif_reset_potential": -70.0
  }
}
```

#### Plasticity
```json
{
  "plasticity": {
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0
  }
}
```

## Configuration Parameters

See the NLM documentation for detailed parameter descriptions:
- [Config reference documentation]
- [Simulation parameter guide]
- [Plasticity configuration guide]

## Notes

- All numerical values are floats (unless noted otherwise)
- Boolean values use `true` or `false`
- Array values use standard JSON array syntax
- Missing values will use defaults from `createDefaultConfig()`
- Configuration files are validated for basic syntax errors

## Customization

You can easily customize these configurations:

1. **Modify Values**: Change any numeric parameter to suit your research needs
2. **Add Sections**: Include new sections for experimental parameters
3. **Override via CLI**: Use command-line arguments to override file settings
4. **Generate Programmatically**: Create configurations using NLM's Python API

## Examples

Generate a custom configuration:

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("connection_probability", 0.2)
config.set("stdp_ltp_weight", 0.03)

# Save to file
config.saveToFile("my_config.json")
```

Load and use a custom configuration:

```python
import pynlm

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
```

## Version Information

Configuration file format version: 1.0
NLM version: Phase 6.0

For more information, see the NLM documentation or the NLM GitHub repository.
"""
    
    # Save README
    with open(configs_dir / "README.md", "w") as f:
        f.write(readme_content)
    
    print("Sample configuration files created in configs/ directory:")
    for file in configs_dir.glob("*.json"):
        print(f"  - {file.name}")
    
    print(f"\nTotal files created: {len(list(configs_dir.glob('*.json')))}")

if __name__ == "__main__":
    create_sample_configs()
