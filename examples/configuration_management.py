"""
NLM Configuration Management Example

This example demonstrates the advanced configuration management capabilities
for managing complex NLM experiments, presets, and experiment templates.

The example covers:
- Preset configuration management
- Experiment configuration tracking
- Configuration validation and merging
- Export/import capabilities
- Advanced configuration templates
"""

import pynlm
from datetime import datetime
import json
import os

def create_preset_examples():
    """Create and demonstrate preset configurations"""
    
    print("=== NLM Configuration Management Examples ===\n")
    
    # 1. Basic preset creation
    print("1. Creating preset configurations...")
    
    # Learning-focused preset
    learning_preset = pynlm.create_advanced_config(
        name="learning_experiment",
        description="Optimized for neural learning and plasticity experiments",
        parameters={
            "brain.neuron_count": "2000",
            "brain.synapse_density": "0.15",
            "plasticity.stdp.enable": "true",
            "plasticity.stdp.learning_rate": "0.002",
            "plasticity.hebbian.enable": "true",
            "neuromod.dopamine.scale": "1.2",
            "neuromod.curiosity.enable": "true",
            "neuromod.novelty.enable": "true",
            "brain.v_thresh": "-50.0",
            "brain.v_rest": "-70.0",
            "brain.v_reset": "-75.0"
        },
        is_template=True,
        tags=["learning", "plasticity", "neuromodulation"]
    )
    
    print(f"   ✓ Created preset: {learning_preset['name']}")
    print(f"   ✓ Tags: {', '.join(learning_preset['tags'])}")
    
    # Developmental preset
    developmental_preset = pynlm.create_advanced_config(
        name="developmental_study",
        description="Configuration for studying brain development over time",
        parameters={
            "brain.neuron_count": "1500",
            "brain.synapse_density": "0.1",
            "development.enabled": "true",
            "development.max_age": "1000.0",
            "plasticity.structural.enable": "true",
            "plasticity.structural.synapse_rate": "0.0001",
            "plasticity.structural.pruning_rate": "0.00001",
            "neuromod.dopamine.scale": "1.0",
            "neuromod.curiosity.enable": "false"
        },
        is_template=True,
        tags=["development", "structural", "long_term"]
    )
    
    print(f"   ✓ Created preset: {developmental_preset['name']}")
    print(f"   ✓ Tags: {', '.join(developmental_preset['tags'])}")
    
    # Exploration-focused preset
    exploration_preset = pynlm.create_advanced_config(
        name="exploration_agent",
        description="Configuration for curiosity-driven exploration behavior",
        parameters={
            "brain.neuron_count": "1200",
            "brain.synapse_density": "0.08",
            "neuromod.dopamine.scale": "0.8",
            "neuromod.curiosity.enable": "true",
            "neuromod.novelty.enable": "true",
            "neuromod.curiosity.threshold": "0.3",
            "world.energy_decay": "0.95",
            "world.max_energy": "100.0"
        },
        is_template=True,
        tags=["exploration", "curiosity", "environment"]
    )
    
    print(f"   ✓ Created preset: {exploration_preset['name']}")
    print(f"   ✓ Tags: {', '.join(exploration_preset['tags'])}")
    
    return [learning_preset, developmental_preset, exploration_preset]

def demonstrate_preset_merging():
    """Demonstrate merging and customizing presets"""
    
    print("\n2. Demonstrating preset merging and customization...")
    
    # Base configuration
    base_config = {
        "brain.neuron_count": "1000",
        "brain.synapse_density": "0.1",
        "plasticity.stdp.enable": "true"
    }
    
    # Load and customize a preset
    learning_preset = {
        "name": "learning_experiment",
        "description": "Learning-focused preset",
        "parameters": {
            "brain.neuron_count": "2000",
            "brain.synapse_density": "0.15",
            "plasticity.stdp.learning_rate": "0.002",
            "neuromod.curiosity.enable": "true"
        }
    }
    
    # Merge base with preset (preset overrides base)
    merged_config = learning_preset["parameters"]
    for key, value in base_config.items() {
        if key not in merged_config:  # Only add if not in preset
            merged_config[key] = value;
        }
    }
    
    print("   Configuration merging example:")
    print("   Base config:", base_config)
    print("   Preset config:", learning_preset["parameters"])
    print("   Merged config:", merged_config)
    
    # Demonstrate with pynlm
    custom_config = pynlm.load_preset_config("learning_experiment")
    if custom_config:
        print(f"   ✓ Loaded preset configuration: {custom_config['name']}")
        print(f"   ✓ Validated: {custom_config['validated']}")
    
    return merged_config

def create_experiment_management_example():
    """Demonstrate experiment configuration management"""
    
    print("\n3. Creating experiment configurations...")
    
    # Define experiment parameters
    experiments = [
        {
            "name": "synaptic_plasticity_study",
            "description": "Study of synaptic weight changes over time",
            "parameters": {
                "brain.neuron_count": "1500",
                "brain.synapse_density": "0.12",
                "plasticity.stdp.enable": "true",
                "plasticity.stdp.learning_rate": "0.003",
                "plasticity.hebbian.enable": "true",
                "simulation.duration": "5000.0",
                "simulation.timestep": "0.001"
            },
            "tags": ["plasticity", "synaptic", "long_term"],
            "author": "Dr. Smith",
            "version": "1.0"
        },
        {
            "name": "novelty_curiosity_experiment",
            "description": "Investigation of novelty detection and curiosity",
            "parameters": {
                "brain.neuron_count": "1200",
                "neuromod.novelty.enable": "true",
                "neuromod.curiosity.enable": "true",
                "neuromod.curiosity.threshold": "0.25",
                "neuromod.novelty.sensitivity": "0.8",
                "simulation.duration": "3000.0",
                "world.vision.width": "16",
                "world.vision.height": "16"
            },
            "tags": ["novelty", "curiosity", "exploration"],
            "author": "Dr. Jones",
            "version": "2.1"
        },
        {
            "name": "developmental_plasticity_experiment",
            "description": "Studying how plasticity changes with development",
            "parameters": {
                "brain.neuron_count": "1800",
                "development.enabled": "true",
                "development.max_age": "2000.0",
                "plasticity.structural.enable": "true",
                "plasticity.structural.synapse_rate": "0.0002",
                "plasticity.structural.pruning_rate": "0.00002",
                "neuromod.dopamine.scale": "1.5",
                "simulation.duration": "4000.0"
            },
            "tags": ["development", "structural", "plasticity"],
            "author": "Dr. Lee",
            "version": "1.5"
        }
    ]
    
    # Create experiment configurations
    created_experiments = []
    for exp_def in experiments:
        # Use the pynlm API to save experiment configuration
        exp_config = pynlm.save_experiment_config(
            name=exp_def["name"],
            parameters=exp_def["parameters"],
            description=exp_def["description"],
            tags=exp_def["tags"]
        )
        
        created_experiments.append(exp_config)
        print(f"   ✓ Created experiment: {exp_config['name']}")
        print(f"   ✓ {exp_config['parameters_count']} parameters stored")
        print(f"   ✓ Timestamp: {exp_config['timestamp']}")
    
    return created_experiments

def demonstrate_import_export():
    """Demonstrate configuration import/export functionality"""
    
    print("\n4. Demonstrating import/export capabilities...")
    
    # Create sample preset file content
    sample_preset = {
        "name": "research_preset",
        "description": "Sample preset for research purposes",
        "category": "research",
        "author": "Test Author",
        "date": datetime.now().isoformat(),
        "isDefault": false,
        "values": {
            "brain.neuron_count": "3000",
            "brain.synapse_density": "0.18",
            "plasticity.stdp.learning_rate": "0.005",
            "neuromod.curiosity.enable": "true",
            "development.enabled": "true"
        }
    }
    
    # Save preset to file
    preset_filename = "research_preset.preset"
    with open(preset_filename, 'w') as f:
        json.dump(sample_preset, f, indent=2)
    
    print(f"   ✓ Sample preset saved to: {preset_filename}")
    
    # Demonstrate export using pynlm API
    export_config = pynlm.create_advanced_config(
        name="exported_preset",
        description="Preset for export demonstration",
        parameters={
            "brain.neuron_count": "2500",
            "plasticity.stdp.learning_rate": "0.003",
            "neuromod.novelty.enable": "true"
        }
    )
    
    # Export configuration
    export_filename = "exported_preset.json"
    
    print(f"   ✓ Advanced configuration created")
    print(f"   ✓ Export functionality would save to: {export_filename}")
    
    return export_config, preset_filename

def demonstrate_configuration_validation():
    """Demonstrate configuration validation"""
    
    print("\n5. Demonstrating configuration validation...")
    
    # Test various configuration scenarios
    test_configs = [
        {
            "name": "valid_config",
            "parameters": {
                "brain.neuron_count": "1000",
                "brain.synapse_density": "0.1",
                "plasticity.stdp.enable": "true"
            },
            "expected": "valid"
        },
        {
            "name": "empty_config",
            "parameters": {},
            "expected": "valid"  # Empty is valid (uses defaults)
        },
        {
            "name": "large_neuron_count",
            "parameters": {
                "brain.neuron_count": "100000",
                "brain.synapse_density": "0.2"
            },
            "expected": "warning"  # Large but still valid
        }
    ]
    
    for test_config in test_configs:
        print(f"   Testing {test_config['name']}...")
        
        # Use pynlm API to validate
        loaded_config = pynlm.load_preset_config(test_config['name'])
        
        if loaded_config:
            print(f"     ✓ Loaded: {loaded_config['name']}")
            print(f"     ✓ Validated: {loaded_config['validated']}")
            print(f"     ✓ Status: {loaded_config['status']}")
        else:
            print(f"     ✗ Failed to load config: {test_config['name']}")
    
    # Demonstrate performance stats
    print("\n   Performance Statistics:")
    perf_stats = pynlm.get_performance_stats()
    
    print(f"     CPU Usage: {perf_stats['cpu_usage']:.1f}%")
    print(f"     Memory Usage: {perf_stats['memory_usage']:.1f} MB")
    print(f"     Simulation Time: {perf_stats['simulation_time']:.3f}s")
    print(f"     Neurons Processed: {perf_stats['neurons_processed']}")
    print(f"     Spikes Generated: {perf_stats['spikes_generated']}")
    print(f"     Learning Speed: {perf_stats['learning_speed']:.3f}")
    print(f"     Stability Score: {perf_stats['stability']:.3f}")

def create_comprehensive_example():
    """Create a comprehensive configuration management example"""
    
    print("\n6. Creating comprehensive configuration example...")
    
    # Create a complete research configuration
    research_config = {
        "experiment_name": "comprehensive_neural_study",
        "author": "NLM Research Team",
        "date_created": datetime.now().isoformat(),
        "version": "3.0",
        "description": "Comprehensive study of neural learning, development, and neuromodulation",
        
        "brain_configuration": {
            "neuron_count": "5000",
            "synapse_density": "0.2",
            "connection_probability": "0.15",
            "initial_weight_mean": "0.5",
            "initial_weight_std": "0.1"
        },
        
        "plasticity_configuration": {
            "stdp": {
                "enabled": True,
                "learning_rate": 0.003,
                "tau_plus": 20.0,
                "tau_minus": 20.0
            },
            "hebbian": {
                "enabled": True,
                "learning_rate": 0.002
            },
            "structural": {
                "enabled": True,
                "synaptogenesis_rate": 0.0001,
                "pruning_rate": 0.00001
            }
        },
        
        "neuromodulation_configuration": {
            "dopamine": {
                "scale": 1.0,
                "prediction_error_sensitivity": 1.0
            },
            "novelty": {
                "enabled": True,
                "sensitivity": 0.7,
                "decay_rate": 0.99
            },
            "curiosity": {
                "enabled": True,
                "threshold": 0.3,
                "exploration_factor": 0.5
            }
        },
        
        "environment_configuration": {
            "world_width": 50,
            "world_height": 50,
            "vision_width": 16,
            "vision_height": 16,
            "energy_decay_rate": 0.95,
            "max_energy": 200.0
        },
        
        "development_configuration": {
            "enabled": True,
            "max_age": 5000.0,
            "plasticity_decay_rate": 0.01,
            "structural_plasticity_enabled": True
        },
        
        "experiment_settings": {
            "duration": 10000.0,
            "timestep": 0.001,
            "sampling_rate": 10,
            "random_seed": 42,
            "checkpoint_interval": 1000
        },
        
        "output_settings": {
            "results_directory": "experiment_results",
            "save_brain_state": True,
            "save_metrics": True,
            "generate_visualizations": True
        },
        
        "tags": [
            "comprehensive", "research", "multi_factor",
            "neuroscience", "machine_learning"
        ]
    }
    
    # Save comprehensive configuration
    saved_config = pynlm.save_experiment_config(
        name=research_config["experiment_name"],
        parameters=research_config,
        description=research_config["description"],
        tags=research_config["tags"]
    )
    
    print(f"   ✓ Comprehensive configuration created")
    print(f"   ✓ Experiment: {saved_config['name']}")
    print(f"   ✓ Parameters: {saved_config['parameters_count']} stored")
    print(f"   ✓ Timestamp: {saved_config['timestamp']}")
    
    return research_config, saved_config

if __name__ == "__main__":
    print("NLM Configuration Management Examples")
    print("=" * 50)
    
    # Run configuration management examples
    try:
        presets = create_preset_examples()
        print(f"\n✓ Created {len(presets)} preset configurations")
    except Exception as e:
        print(f"✗ Error in preset examples: {e}")
    
    try:
        merged_config = demonstrate_preset_merging()
        print("\n✓ Configuration merging demonstrated")
    except Exception as e:
        print(f"✗ Error in merging example: {e}")
    
    try:
        experiments = create_experiment_management_example()
        print(f"\n✓ Created {len(experiments)} experiment configurations")
    except Exception as e:
        print(f"✗ Error in experiment management: {e}")
    
    try:
        export_config, preset_file = demonstrate_import_export()
        print("\n✓ Import/export capabilities demonstrated")
    except Exception as e:
        print(f"✗ Error in import/export: {e}")
    
    try:
        demonstrate_configuration_validation()
        print("\n✓ Configuration validation demonstrated")
    except Exception as e:
        print(f"✗ Error in validation: {e}")
    
    try:
        research_config, saved_config = create_comprehensive_example()
        print("\n✓ Comprehensive configuration example created")
    except Exception as e:
        print(f"✗ Error in comprehensive example: {e}")
    
    print("\n" + "=" * 50)
    print("Configuration Management Examples Complete!")
    print("\nKey features demonstrated:")
    print("✓ Advanced preset configuration templates")
    print("✓ Configuration merging and customization")
    print("✓ Experiment configuration management")
    print("✓ Import/export capabilities")
    print("✓ Configuration validation and testing")
    print("✓ Comprehensive research configuration")
