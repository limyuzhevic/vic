#!/usr/bin/env python3

"""
NLM Experiment Configuration Presets

Configuration presets for common neural learning experiments.
These can be loaded with the NLM CLI to quickly set up experiments.
"""

import json

# Learning Experiment Preset
LEARNING_EXPERIMENT = {
    "name": "Learning Experiment",
    "description": "Basic neural plasticity and learning experiment",
    "simulation": {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.15,
        "max_simulation_steps": 1000
    },
    "plasticity": {
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "stdp_time_constant": 20.0,
        "plasticity_learning_rate": 0.01
    },
    "neuromodulation": {
        "dopamine_baseline": 0.1,
        "dopamine_modulation_strength": 0.5
    },
    "memory": {
        "working_memory_capacity": 100,
        "working_memory_decay_rate": 0.01,
        "episodic_memory_capacity": 1000,
        "consolidation_strength": 0.3
    },
    "development": {
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001,
        "critical_period_start": 100,
        "critical_period_end": 1000
    },
    "environment": {
        "environment_name": "GridWorld",
        "environment_width": 10,
        "environment_height": 10
    },
    "visualization": {
        "visualization_enabled": True,
        "visualization_update_rate": 30.0
    }
}

# Memory Consolidation Experiment Preset
MEMORY_CONSOLIDATION_EXPERIMENT = {
    "name": "Memory Consolidation Experiment",
    "description": "Experiment on memory consolidation during rest periods",
    "simulation": {
        "random_seed": 123,
        "simulation_timestep": 0.001,
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "max_simulation_steps": 5000
    },
    "memory": {
        "working_memory_capacity": 200,
        "working_memory_decay_rate": 0.005,
        "episodic_memory_capacity": 5000,
        "consolidation_strength": 0.8,
        "replay_frequency": 10,  # replay every 10 steps
        "replay_strength": 0.7,
        "interference_strength": 0.3
    },
    "plasticity": {
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "stdp_time_constant": 20.0,
        "homeostatic_plasticity": True,
        "homeostatic_target_rate": 0.05
    },
    "development": {
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00005,
        "experience_dependent_plasticity": True,
        "memory_guided_synaptogenesis": True
    }
}

# Critical Period Experiment Preset
CRITICAL_PERIOD_EXPERIMENT = {
    "name": "Critical Period Experiment",
    "description": "Study of developmental critical periods and experience-dependent plasticity",
    "simulation": {
        "random_seed": 999,
        "simulation_timestep": 0.001,
        "neuron_count": 800,
        "region_count": 1,
        "connection_probability": 0.12,
        "max_simulation_steps": 10000
    },
    "development": {
        "critical_period_start": 100,
        "critical_period_end": 1000,
        "plasticity_magnitude_during_critical_period": 2.0,
        "post_critical_period_plasticity": 0.3,
        "sensory deprivation_impact": True,
        "enriched_environment_boost": 1.5
    },
    "plasticity": {
        "stdp_ltp_weight": 0.015,
        "stdp_ltd_weight": 0.018,
        "stdp_time_constant": 25.0,
        "experience_expectant_plasticity": True
    },
    "memory": {
        "working_memory_capacity": 150,
        "working_memory_decay_rate": 0.02,
        "episodic_memory_capacity": 2000,
        "developmental_memory_consolidation": True
    }
}

# Social Learning Experiment Preset
SOCIAL_LEARNING_EXPERIMENT = {
    "name": "Social Learning Experiment",
    "description": "Study of social learning and observational learning",
    "simulation": {
        "random_seed": 777,
        "simulation_timestep": 0.001,
        "neuron_count": 1200,
        "region_count": 3,
        "connection_probability": 0.08,
        "max_simulation_steps": 8000
    },
    "social": {
        "social_learning_enabled": True,
        "imitation_strength": 0.7,
        "local_belief Propagation": True,
        "social_reward_modulation": True
    },
    "memory": {
        "working_memory_capacity": 300,
        "working_memory_decay_rate": 0.01,
        "episodic_memory_capacity": 3000,
        "social_memory_enhancement": True
    },
    "plasticity": {
        "stdp_ltp_weight": 0.012,
        "stdp_ltd_weight": 0.014,
        "stdp_time_constant": 18.0,
        "social_modulated_plasticity": True
    }
}

# Curiosity-Driven Learning Experiment Preset
CURIOSITY_LEARNING_EXPERIMENT = {
    "name": "Curiosity-Driven Learning Experiment",
    "description": "Study of intrinsic motivation and curiosity-driven learning",
    "simulation": {
        "random_seed": 555,
        "simulation_timestep": 0.001,
        "neuron_count": 600,
        "region_count": 1,
        "connection_probability": 0.14,
        "max_simulation_steps": 6000
    },
    "curiosity": {
        "curiosity_drive_enabled": True,
        "novelty_detection_strength": 0.8,
        "exploration_bonus": 0.5,
        "intrinsic_motivation": True
    },
    "neuromodulation": {
        "dopamine_baseline": 0.2,
        "curiosity_signal_strength": 1.0,
        "reward_prediction_error": True
    },
    "learning": {
        "exploration_exploitation_balance": 0.5,
        "intrinsic_reward_weight": 0.3,
        "extrinsic_reward_weight": 0.7
    }
}

# Complete collection of presets
EXPERIMENT_PRESETS = {
    "learning": LEARNING_EXPERIMENT,
    "memory_consolidation": MEMORY_CONSOLIDATION_EXPERIMENT,
    "critical_period": CRITICAL_PERIOD_EXPERIMENT,
    "social_learning": SOCIAL_LEARNING_EXPERIMENT,
    "curiosity_learning": CURIOSITY_LEARNING_EXPERIMENT
}

if __name__ == "__main__":
    # Write presets to a file for easy loading
    with open("experiment_presets.json", "w") as f:
        json.dump({
            "version": "0.1.0",
            "presets": EXPERIMENT_PRESETS,
            "description": "NLM Experiment Configuration Presets",
            "usage": "Load these presets using the NLM CLI: nlm-cli config experiment_presets.json"
        }, f, indent=2)
    
    print("Created experiment_presets.json with 5 experiment configurations")
    print("\nAvailable presets:")
    for name, preset in EXPERIMENT_PRESETS.items():
        print(f"  - {name}: {preset['name']}")
    
    print("\nUsage: Load with NLM CLI")
    print("  nlm-cli config experiment_presets.json")
