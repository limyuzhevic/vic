# NLM Configuration Examples

## Overview

This document provides comprehensive JSON configuration examples for all NLM brain systems, experiment configurations, world setups, and performance tuning. It serves as a practical reference for configuring NLM simulations and experiments.

## 1. Brain System Configurations

### 1.1 Default Brain Configuration
```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "learning_rate": 0.01,
        "plasticity_strength": 0.5,
        "development_stage": "adult",
        "simulation_timestep": 0.01,
        "max_spike_history": 100
    }
}
```

### 1.2 High-Resolution Brain Configuration
```json
{
    "brain": {
        "neuron_count": 5000,
        "region_count": 4,
        "connection_probability": 0.08,
        "learning_rate": 0.005,
        "plasticity_strength": 0.7,
        "development_stage": "critical_period",
        "simulation_timestep": 0.005,
        "max_spike_history": 200,
        "neuron_types": {
            "excitatory": 0.8,
            "inhibitory": 0.2,
            "internal": 0.0
        },
        "region_properties": {
            "region_0": {
                "type": "sensory",
                "size": 0.25,
                "plasticity_rate": 0.8
            },
            "region_1": {
                "type": "association",
                "size": 0.3,
                "plasticity_rate": 0.6
            },
            "region_2": {
                "type": "motor",
                "size": 0.25,
                "plasticity_rate": 0.4
            },
            "region_3": {
                "type": "internal",
                "size": 0.2,
                "plasticity_rate": 0.5
            }
        }
    }
}
```

### 1.3 Experimental Brain Configuration
```json
{
    "brain": {
        "neuron_count": 2000,
        "region_count": 3,
        "connection_probability": 0.12,
        "learning_rate": 0.02,
        "plasticity_strength": 0.6,
        "development_stage": "maturation",
        "simulation_timestep": 0.01,
        "max_spike_history": 150,
        "plasticity": {
            "stdp_enabled": true,
            "hebbian_enabled": true,
            "reward_modulated_enabled": true,
            "eligibility_trace_enabled": true,
            "stdp": {
                "learning_rate": 0.01,
                "time_constant": 20.0,
                "max_weight": 1.0,
                "min_weight": 0.0
            },
            "hebbian": {
                "learning_rate": 0.02,
                "global_modulation": true
            }
        },
        "neuromodulation": {
            "dopamine_enabled": true,
            "curiosity_enabled": true,
            "novelty_enabled": true,
            "prediction_error_enabled": true,
            "dopamine": {
                "baseline": 0.1,
                "sensitivity": 0.5,
                "phasic_decay": 0.9
            },
            "curiosity": {
                "weight": 0.3,
                "decay_rate": 0.95,
                "exploration_bonus": 0.5
            }
        }
    }
}
```

## 2. Memory System Configurations

### 2.1 Working Memory Configuration
```json
{
    "memory": {
        "working": {
            "enabled": true,
            "capacity": 100,
            "decay_rate": 0.95,
            "reinforcement_threshold": 0.8,
            "competition_strength": 0.5,
            "stability_time": 50,
            "encoding_efficiency": 0.9,
            "recall_probability": 0.7,
            "binding_strength": 0.6,
            "winner_take_all": true
        }
    }
}
```

### 2.2 Episodic Memory Configuration
```json
{
    "memory": {
        "episodic": {
            "enabled": true,
            "capacity": 200,
            "capture_interval": 10,
            "replay_interval": 100,
            "consolidation_interval": 1000,
            "encoding_strength": 0.8,
            "temporal_decay": 0.98,
            "spatial_decay": 0.99,
            "relevance_weighting": true,
            "replay_frequency": 0.1,
            "consolidation_threshold": 0.6,
            "priority_queue_enabled": true,
            "pattern_completion_enabled": true,
            "noise_resilience": 0.8
        }
    }
}
```

### 2.3 Associative Memory Configuration
```json
{
    "memory": {
        "associative": {
            "enabled": true,
            "similarity_threshold": 0.6,
            "spreading_activation_radius": 0.3,
            "association_strength": 0.7,
            "link_decay_rate": 0.99,
            "reinforcement_boost": 0.5,
            "pattern_vector_size": 128,
            "max_associations_per_pattern": 50,
            "symmetry_enabled": true,
            "bidirectional_links": true,
            "temporal_asymmetry": 0.3,
            "recency_bias": 0.8,
            "frequency_boost": 0.4
        }
    }
}
```

### 2.4 Concept Formation Configuration
```json
{
    "memory": {
        "concept_formation": {
            "enabled": true,
            "clustering_algorithm": "hierarchical",
            "prototype_stability": 0.9,
            "learning_rate": 0.05,
            "concept_threshold": 0.7,
            "generalization_strength": 0.6,
            "forgetting_rate": 0.95,
            "concept_merge_threshold": 0.8,
            "divergence_detection": true,
            "category_constraints": {
                "min_concepts_per_category": 3,
                "max_concepts_per_category": 20
            },
            "property_weights": {
                "similarity": 0.4,
                "temporal": 0.2,
                "spatial": 0.2,
                "reward": 0.2
            }
        }
    }
}
```

## 3. Neuromodulation Configurations

### 3.1 Dopamine Configuration
```json
{
    "neuromodulation": {
        "dopamine": {
            "enabled": true,
            "baseline_level": 0.1,
            "phasic_sensitivity": 0.5,
            "tonic_modulation": true,
            "reward_prediction_error": {
                "computation_method": "td_error",
                "discount_factor": 0.95,
                "learning_rate": 0.01,
                "error_clipping": true
            },
            "behavioral_vigor": {
                "action_selection_strength": 0.6,
                "exploration_exploitation_balance": 0.5,
                "effort_allocation": "adaptive"
            },
            "plastic_modulation": {
                "stdp_modulation": true,
                "hebbian_modulation": true,
                "eligibility_trace_scaling": 0.8,
                "prediction_error_scaling": 0.9
            }
        }
    }
}
```

### 3.2 Curiosity Configuration
```json
{
    "neuromodulation": {
        "curiosity": {
            "enabled": true,
            "novelty_weight": 0.7,
            "prediction_error_weight": 0.3,
            "exploration_reward": 0.5,
            "decay_rate": 0.95,
            "threshold": 0.3,
            "reset_on_discovery": true,
            "memory_integration": true,
            "action_selection_bias": {
                "unexplored_activities": 0.8,
                "novel_actions": 0.6,
                "high_prediction_error": 0.4
            },
            "developmental_scaling": {
                "critical_period_multiplier": 1.5,
                "maturation_decay": 0.9
            }
        }
    }
}
```

### 3.3 Novelty Detection Configuration
```json
{
    "neuromodulation": {
        "novelty": {
            "enabled": true,
            "detection_method": "pattern_matching",
            "similarity_threshold": 0.7,
            "comparison_window": 100,
            "adaptation_rate": 0.9,
            "memory_importance": 0.8,
            "decay_curve": "exponential",
            "max_novelty_score": 1.0,
            "noise_filtering": true,
            "smoothing_window": 10,
            "cross_modal_integration": true
        }
    }
}
```

## 4. Prediction System Configuration

### 4.1 Prediction System Configuration
```json
{
    "prediction": {
        "enabled": true,
        "prediction_horizon": 3,
        "temporal_resolution": 0.01,
        "prediction_accuracy": 0.8,
        "confidence_threshold": 0.6,
        "error_computation": {
            "method": "mean_squared_error",
            "penalty_weights": {
                "underprediction": 0.5,
                "overprediction": 0.5,
                "late_prediction": 0.3
            }
        },
        "memory_integration": {
            "working_memory_influences": true,
            "episodic_memory_influences": true,
            "association_memory_influences": true
        },
        "action_consequence_prediction": {
            "depth": 2,
            "importance_weighting": true,
            "action_value_estimation": true
        },
        "self_model_prediction": {
            "enabled": true,
            "self_other_distinction": true,
            "body_ownership": true
        }
    }
}
```

## 5. Cognition System Configuration

### 5.1 Attention System Configuration
```json
{
    "cognition": {
        "attention": {
            "enabled": true,
            "selection_mechanism": "lateral_inhibition",
            "competition_strength": 0.7,
            "bias_integration": true,
            "top_down_weight": 0.4,
            "bottom_up_weight": 0.6,
            "gating_mechanism": "soft_selection",
            "memory_binding": true,
            "adaptive_focus": true,
            "selective_filtering": true,
            "attentional_load": "limited"
        }
    }
}
```

### 5.2 Planning System Configuration
```json
{
    "cognition": {
        "planning": {
            "enabled": true,
            "planning_depth": 3,
            "action_evaluation_method": "value_based",
            "reward_prediction": true,
            "sequence_construction": "hierarchical",
            "goal_integration": true,
            "constraint_handling": "soft_constraints",
            "uncertainty_handling": "probabilistic",
            "dynamic_replanning": true,
            "planning_time_budget": 100,
            "lookahead_accuracy": 0.8,
            "action_quality_estimation": "experience_based"
        }
    }
}
```

## 6. World Configuration

### 6.1 Simple World Configuration
```json
{
    "world": {
        "type": "simple",
        "width": 200,
        "height": 200,
        "vision_width": 20,
        "vision_height": 20,
        "timestep": 0.01,
        "max_objects": 50,
        "object_update_frequency": 10,
        "reward_decay": 0.99,
        "terminal_conditions": {
            "max_steps": 1000,
            "target_reward": 100.0,
            "time_limit": 60.0
        },
        "physics": {
            "gravity": 9.8,
            "friction": 0.1,
            "elasticity": 0.5
        },
        "sensory": {
            "vision_enabled": true,
            "vision_range": 20,
            "vision_quality": "high",
            "internal_sensing_enabled": true,
            "proprioception_enabled": true
        },
        "actions": {
            "movement": true,
            "rotation": true,
            "manipulation": true,
            "communication": true
        }
    }
}
```

### 6.2 Complex World Configuration
```json
{
    "world": {
        "type": "complex",
        "width": 500,
        "height": 500,
        "time_steps": 1000,
        "simulation_speed": 1.0,
        "agent_count": 10,
        "object_types": ["static", "dynamic", "moving", "reactive"],
        "environment": {
            "terrain_type": "grid",
            "weather_effects": true,
            "lighting_effects": true,
            "particle_effects": true,
            "sound_effects": true
        },
        "social_dynamics": {
            "interaction_rules": true,
            "hierarchy": true,
            "cooperation": true,
            "competition": true
        },
        "resource_management": {
            "resource_types": ["energy", "information", "matter"],
            "scarcity_factors": true,
            "resource_decay": true,
            "resource_transfer": true
        }
    }
}
```

## 7. Experiment Configuration

### 7.1 Phase 3 Experiment Configuration
```json
{
    "experiment": {
        "name": "Phase3_WorldInteraction",
        "description": "Basic world interaction demo",
        "type": "phase3_demo",
        "parameters": {
            "episodes": 5,
            "steps_per_episode": 200,
            "random_seed": 42,
            "brain_config": {
                "neuron_count": 500,
                "region_count": 1,
                "learning_rate": 0.01
            },
            "world_config": {
                "width": 100,
                "height": 100,
                "vision_width": 10,
                "vision_height": 10
            },
            "agent_config": {
                "curiosity_enabled": true,
                "reward_modulation_enabled": true,
                "action_selection": "exploration_exploitation"
            }
        },
        "metrics": {
            "primary": ["reward", "learning_progress", "behavioral_change"],
            "secondary": ["firing_rate", "memory_usage", "response_time"],
            "thresholds": {
                "reward_convergence": 0.8,
                "learning_improvement": 0.5,
                "behavioral_stability": 0.9
            }
        }
    }
}
```

### 7.2 Phase 4 Experiment Configuration
```json
{
    "experiment": {
        "name": "Phase4_CognitiveIntegration",
        "description": "Phase 4 cognitive experiments with temporal prediction, working memory, episodic recall, concept formation, attention, planning, self-model, social learning, continual learning, and generalization",
        "type": "phase4",
        "parameters": {
            "num_trials": 100,
            "random_seed": 123,
            "brain_config": {
                "neuron_count": 2000,
                "region_count": 3,
                "plasticity": {
                    "stdp_enabled": true,
                    "hebbian_enabled": true,
                    "reward_modulated_enabled": true
                },
                "memory": {
                    "working_enabled": true,
                    "episodic_enabled": true,
                    "associative_enabled": true,
                    "concept_formation_enabled": true
                },
                "cognition": {
                    "attention_enabled": true,
                    "planning_enabled": true,
                    "self_model_enabled": true,
                    "social_learning_enabled": true
                }
            },
            "world_config": {
                "width": 200,
                "height": 200,
                "timestep": 0.01
            }
        },
        "experiments": [
            {
                "name": "TemporalPredictionLearning",
                "description": "Tests if NLM can learn that state A predicts state B",
                "type": "prediction_learning",
                "expected_improvement": 0.7,
                "success_threshold": 0.6
            },
            {
                "name": "WorkingMemory",
                "description": "Tests if information can be maintained after stimulus disappears",
                "type": "working_memory_test",
                "decay_threshold": 0.5,
                "capacity_limit": 50
            },
            {
                "name": "EpisodicRecall",
                "description": "Tests if past experiences influence future behavior",
                "type": "episodic_memory_test",
                "recall_accuracy_threshold": 0.6,
                "memory_retention_threshold": 0.8
            },
            {
                "name": "ConceptFormation",
                "description": "Tests if recurring patterns form stable representations without predefined labels",
                "type": "concept_formation_test",
                "stability_threshold": 0.7,
                "generalization_threshold": 0.6
            },
            {
                "name": "AttentionSelection",
                "description": "Tests if competing signals produce selective processing",
                "type": "attention_test",
                "selectivity_threshold": 0.7,
                "consistency_threshold": 0.8
            },
            {
                "name": "MultiStepPlanning",
                "description": "Tests if NLM can plan multi-step sequences to achieve goals",
                "type": "planning_test",
                "planning_accuracy_threshold": 0.6,
                "multi_step_success_threshold": 0.7
            }
        ]
    }
}
```

### 7.3 Phase 6 Integration Experiment Configuration
```json
{
    "experiment": {
        "name": "Phase6_Integration",
        "description": "Comprehensive integration test verifying all brain systems are connected and functional",
        "type": "phase6_integration",
        "parameters": {
            "duration": 2000,
            "random_seed": 42,
            "checkpoint_interval": 500,
            "brain_config": {
                "neuron_count": 3000,
                "region_count": 4,
                "development_enabled": true,
                "development_stage": "maturation",
                "plasticity": {
                    "stdp_enabled": true,
                    "hebbian_enabled": true,
                    "reward_modulated_enabled": true,
                    "eligibility_trace_enabled": true,
                    "structural_enabled": true
                },
                "memory": {
                    "working_enabled": true,
                    "episodic_enabled": true,
                    "associative_enabled": true,
                    "concept_formation_enabled": true
                },
                "neuromodulation": {
                    "dopamine_enabled": true,
                    "curiosity_enabled": true,
                    "novelty_enabled": true,
                    "prediction_error_enabled": true
                },
                "cognition": {
                    "attention_enabled": true,
                    "planning_enabled": true,
                    "self_model_enabled": true,
                    "social_learning_enabled": true
                },
                "prediction": {
                    "enabled": true,
                    "horizon": 5
                }
            },
            "world_config": {
                "width": 300,
                "height": 300,
                "timestep": 0.01,
                "agent_count": 5,
                "complexity": "high"
            }
        },
        "integration_checks": [
            {
                "name": "MemorySystemIntegration",
                "description": "Verify memory systems are connected to neural processing",
                "checkpoints": [
                    "working_memory_functional",
                    "episodic_memory_encoding",
                    "associative_memory_formation",
                    "concept_formation_active"
                ]
            },
            {
                "name": "NeuromodulationIntegration",
                "description": "Verify neuromodulation affects plasticity and dynamics",
                "checkpoints": [
                    "dopamine_affects_excitability",
                    "curiosity_drives_exploration",
                    "novelty_detection_working",
                    "prediction_error_signaling"
                ]
            },
            {
                "name": "PredictionIntegration",
                "description": "Verify prediction system is integrated with brain loop",
                "checkpoints": [
                    "prediction_system_active",
                    "prediction_error_computation",
                    "confidence_tracking",
                    "integration_with_plasticity"
                ]
            },
            {
                "name": "DevelopmentIntegration",
                "description": "Verify developmental stages affect plasticity rates",
                "checkpoints": [
                    "developmental_stages_proper",
                    "plasticity_modulation_by_age",
                    "structural_plasticity_active"
                ]
            }
        ],
        "success_criteria": {
            "memory_integration_score": 0.8,
            "neuromodulation_integration_score": 0.8,
            "prediction_integration_score": 0.7,
            "development_integration_score": 0.7,
            "overall_integration_score": 0.75,
            "stability_duration": 1000,
            "learning_rate": 0.5
        }
    }
}
```

## 8. Performance Tuning Configurations

### 8.1 Performance Optimization Configuration
```json
{
    "performance": {
        "optimization_level": "aggressive",
        "memory_management": {
            "enable_memory_pools": true,
            "pool_sizes": {
                "neuron_states": 10000,
                "synapse_weights": 50000,
                "spike_events": 20000
            },
            "enable_compact_storage": true,
            "compression_enabled": true,
            "garbage_collection_interval": 1000
        },
        "computation": {
            "enable_vectorization": true,
            "simd_width": "AVX2",
            "cache_blocking_enabled": true,
            "parallel_processing_enabled": true,
            "num_threads": 4,
            "work_splitting": "dynamic"
        },
        "storage": {
            "checkpoint_frequency": 100,
            "checkpoint_compression": true,
            "incremental_backup": true,
            "backup_location": "/tmp/nlm_backups"
        },
        "simulation": {
            "time_stepping": "adaptive",
            "event_driven_enabled": true,
            "lazy_evaluation": true,
            "precision_mode": "double"
        },
        "debugging": {
            "profile_simulation": true,
            "trace_memory_allocations": true,
            "log_performance_metrics": true,
            "enable_cpu_profiling": true,
            "enable_memory_profiling": true
        }
    }
}
```

### 8.2 Resource Management Configuration
```json
{
    "resource_management": {
        "cpu": {
            "affinity_mask": "0-3",
            "enable_scheduling": true,
            "priority": "balanced",
            "power_management": "conservative"
        },
        "memory": {
            "limit_mb": 2048,
            "enable_swapping": false,
            "enable_page_cache": true,
            "memory_alignment": 64,
            "allocate_persistently": true
        },
        "io": {
            "filesystem": "ssd",
            "enable_read_ahead": true,
            "write_buffering": true,
            "max_file_handles": 100
        },
        "network": {
            "enable_remote_monitoring": false,
            "monitoring_port": 8080,
            "enable_remote_backup": false
        }
    }
}
```

## 9. Configuration Templates

### 9.1 Development Environment Configuration
```json
{
    "name": "Development Environment",
    "description": "Configuration for NLM development and testing",
    "brain": {
        "neuron_count": 500,
        "region_count": 2,
        "learning_rate": 0.01
    },
    "memory": {
        "working_enabled": true,
        "episodic_enabled": true,
        "associative_enabled": true,
        "concept_formation_enabled": true
    },
    "neuromodulation": {
        "dopamine_enabled": true,
        "curiosity_enabled": true,
        "novelty_enabled": true
    },
    "world": {
        "width": 100,
        "height": 100,
        "vision_width": 10,
        "vision_height": 10
    },
    "experiment": {
        "type": "development",
        "debug_mode": true,
        "logging_level": "debug"
    }
}
```

### 9.2 Production Configuration
```json
{
    "name": "Production Environment",
    "description": "Configuration for NLM production deployment",
    "brain": {
        "neuron_count": 5000,
        "region_count": 4,
        "learning_rate": 0.005,
        "plasticity_strength": 0.6
    },
    "memory": {
        "working_enabled": true,
        "episodic_enabled": true,
        "associative_enabled": true,
        "concept_formation_enabled": true
    },
    "neuromodulation": {
        "dopamine_enabled": true,
        "curiosity_enabled": true,
        "novelty_enabled": true,
        "prediction_error_enabled": true
    },
    "world": {
        "width": 500,
        "height": 500,
        "timestep": 0.005,
        "agent_count": 10
    },
    "experiment": {
        "type": "production",
        "debug_mode": false,
        "logging_level": "info"
    },
    "performance": {
        "optimization_level": "maximum",
        "memory_management": {
            "enable_memory_pools": true,
            "compression_enabled": true
        },
        "computation": {
            "enable_vectorization": true,
            "parallel_processing_enabled": true,
            "num_threads": 8
        }
    }
}
```

### 9.3 Research Configuration
```json
{
    "name": "Research Environment",
    "description": "Configuration for NLM research and experimentation",
    "brain": {
        "neuron_count": 3000,
        "region_count": 3,
        "learning_rate": 0.02,
        "development_enabled": true,
        "development_stage": "maturation"
    },
    "memory": {
        "working_enabled": true,
        "episodic_enabled": true,
        "associative_enabled": true,
        "concept_formation_enabled": true
    },
    "neuromodulation": {
        "dopamine_enabled": true,
        "curiosity_enabled": true,
        "novelty_enabled": true,
        "prediction_error_enabled": true
    },
    "world": {
        "width": 300,
        "height": 300,
        "timestep": 0.01,
        "agent_count": 5
    },
    "experiment": {
        "type": "research",
        "debug_mode": true,
        "logging_level": "trace",
        "enable_data_collection": true,
        "enable_profiling": true
    },
    "research_specific": {
        "enable_ablation_studies": true,
        "enable_parameter_sweeps": true,
        "enable_systematic_variations": true,
        "record_detailed_metrics": true,
        "save_intermediate_results": true
    }
}
```

## 10. Configuration Schema Validation

### 10.1 JSON Schema for Configuration
```json
{
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "NLM Configuration Schema",
    "description": "JSON Schema for NLM configuration files",
    "type": "object",
    "properties": {
        "brain": {
            "type": "object",
            "properties": {
                "neuron_count": {
                    "type": "integer",
                    "minimum": 100,
                    "maximum": 10000,
                    "description": "Number of neurons in the brain"
                },
                "region_count": {
                    "type": "integer",
                    "minimum": 1,
                    "maximum": 10,
                    "description": "Number of brain regions"
                },
                "connection_probability": {
                    "type": "number",
                    "minimum": 0.0,
                    "maximum": 1.0,
                    "description": "Probability of synaptic connections"
                },
                "learning_rate": {
                    "type": "number",
                    "minimum": 0.0,
                    "maximum": 1.0,
                    "description": "Learning rate for plasticity"
                }
            }
        },
        "memory": {
            "type": "object",
            "properties": {
                "working": {
                    "type": "object",
                    "properties": {
                        "capacity": {
                            "type": "integer",
                            "minimum": 1,
                            "maximum": 1000
                        },
                        "decay_rate": {
                            "type": "number",
                            "minimum": 0.0,
                            "maximum": 1.0
                        }
                    }
                },
                "episodic": {
                    "type": "object",
                    "properties": {
                        "capacity": {
                            "type": "integer",
                            "minimum": 1,
                            "maximum": 1000
                        },
                        "capture_interval": {
                            "type": "integer",
                            "minimum": 1
                        }
                    }
                }
            }
        }
    },
    "required": ["brain"],
    "additionalProperties": false
}
```

## 11. Configuration Validation

### 11.1 Configuration Validation Rules
```json
{
    "validation_rules": {
        "brain": {
            "neuron_count_range": {"min": 100, "max": 10000},
            "region_count_range": {"min": 1, "max": 10},
            "connection_probability_range": {"min": 0.0, "max": 1.0},
            "learning_rate_range": {"min": 0.0, "max": 1.0}
        },
        "memory": {
            "working_capacity_range": {"min": 1, "max": 1000},
            "episodic_capacity_range": {"min": 1, "max": 1000},
            "decay_rate_range": {"min": 0.0, "max": 1.0}
        },
        "world": {
            "world_size_range": {"min": 50, "max": 1000},
            "timestep_range": {"min": 0.001, "max": 0.1}
        }
    },
    "error_messages": {
        "invalid_neuron_count": "Neuron count must be between 100 and 10000",
        "invalid_region_count": "Region count must be between 1 and 10",
        "invalid_connection_probability": "Connection probability must be between 0.0 and 1.0",
        "memory_over_capacity": "Memory capacity exceeded available system resources"
    }
}
```

## 12. Configuration Backup and Recovery

### 12.1 Configuration Management
```json
{
    "configuration_management": {
        "backup": {
            "enabled": true,
            "interval": 3600,
            "location": "/backups/nlm/",
            "compression": true,
            "retention_days": 30
        },
        "version_control": {
            "enabled": true,
            "repository": "/repos/nlm-configs",
            "auto_commit": true,
            "commit_message_template": "Save NLM configuration {timestamp}"
        },
        "recovery": {
            "enabled": true,
            "checkpoint_frequency": 100,
            "max_history": 1000,
            "restore_prompt": true
        },
        "validation": {
            "enabled": true,
            "auto_validate": true,
            "fail_fast": true,
            "detailed_errors": true
        }
    }
}
```

## 13. Configuration Documentation

This document provides comprehensive JSON configuration examples for NLM (熙然) brain systems. All configuration examples follow the same structure:

### Configuration Structure

1. **brain** - Brain system configuration
2. **memory** - Memory system configurations
3. **neuromodulation** - Neuromodulation system configurations
4. **prediction** - Prediction system configuration
5. **cognition** - Cognitive system configurations
6. **world** - World environment configuration
7. **experiment** - Experiment configuration
8. **performance** - Performance optimization configuration
9. **resource_management** - Resource management configuration

### Configuration Validation

- All numeric values must be within specified ranges
- All boolean values must be valid
- Required fields are marked with `*`
- Configuration files must be valid JSON
- Validation can be performed using the provided JSON schema

### Configuration Updates

This configuration reference is regularly updated to reflect:
- New brain system capabilities
- Updated performance optimizations
- Additional experiment types
- Enhanced validation rules

For the latest configuration examples and updates, visit the official NLM documentation.

## Index

This document provides comprehensive JSON configuration examples for NLM (熙然) brain systems. It covers:

### Configuration Categories

1. **Brain System Configurations** - Default, high-resolution, and experimental brain configurations
2. **Memory System Configurations** - Working memory, episodic memory, associative memory, and concept formation
3. **Neuromodulation Configurations** - Dopamine, curiosity, novelty, and prediction error systems
4. **Prediction System Configuration** - Temporal prediction and action consequence prediction
5. **Cognition System Configurations** - Attention, planning, and self-model systems
6. **World Configurations** - Simple world and complex world settings
7. **Experiment Configurations** - Phase 3, Phase 4, and Phase 6 integration experiments
8. **Performance Tuning Configurations** - Optimization settings and resource management
9. **Configuration Templates** - Development, production, and research configurations
10. **Configuration Schema Validation** - JSON schema and validation rules
11. **Configuration Backup and Recovery** - Configuration management and versioning

### Configuration Files

This documentation includes configuration examples in:
- **brain.cfg** - Default brain configuration
- **high_res_brain.cfg** - High-resolution brain configuration
- **experimental_brain.cfg** - Experimental brain configuration
- **working_memory.cfg** - Working memory configuration
- **episodic_memory.cfg** - Episodic memory configuration
- **associative_memory.cfg** - Associative memory configuration
- **concept_formation.cfg** - Concept formation configuration
- **dopamine.cfg** - Dopamine neuromodulation configuration
- **curiosity.cfg** - Curiosity neuromodulation configuration
- **novelty.cfg** - Novelty detection configuration
- **prediction.cfg** - Prediction system configuration
- **attention.cfg** - Attention system configuration
- **planning.cfg** - Planning system configuration
- **simple_world.cfg** - Simple world configuration
- **complex_world.cfg** - Complex world configuration
- **phase3_experiment.cfg** - Phase 3 experiment configuration
- **phase4_experiment.cfg** - Phase 4 experiment configuration
- **phase6_experiment.cfg** - Phase 6 integration experiment configuration
- **performance_optimization.cfg** - Performance optimization configuration
- **resource_management.cfg** - Resource management configuration
- **development_config.cfg** - Development environment configuration
- **production_config.cfg** - Production environment configuration
- **research_config.cfg** - Research environment configuration

### Configuration Validation

This document includes:
- JSON schema for configuration validation
- Validation rules and error messages
- Configuration backup and recovery procedures
- Configuration management best practices

### Related Documents

This documentation complements the following NLM documents:

- **README.md** - Overview of NLM project and basic usage
- **docs/ARCHITECTURE.md** - Core NLM architecture documentation
- **docs/PHASE6_ARCHITECTURE.md** - Phase 6 final integration architecture
- **python/EnhancedBindingsDocumentation.md** - Enhanced Python API documentation
- **docs/DEVELOPER_INTEGRATION.md** - Developer integration guide (see separate document)
- **docs/ADVANCED_TUTORIALS.md** - Advanced NLM tutorials (see separate document)
- **docs/PHASE6_EXAMPLES.md** - Phase 6 integration examples (see separate document)
- **docs/CLI_GUIDE.md** - Command-line interface guide (see separate document)

## Version Information

- **Version**: 1.0
- **Phase**: Phase 6 Integration
- **Date**: October 29, 2026
- **Status**: Complete
- **Author**: NLM Development Team

This configuration reference document is part of the NLM project's comprehensive documentation suite, providing developers with practical examples for configuring NLM simulations and experiments.
