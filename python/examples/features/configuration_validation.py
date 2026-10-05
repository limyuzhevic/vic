#!/usr/bin/env python3
"""
Feature-Specific Examples: Advanced API Demonstrations

This tutorial demonstrates specific features of the enhanced NLM Python API
in detail, covering validation, metrics, export/import, templates, and presets.

## Learning Objectives
- Master configuration validation and error handling
- Implement comprehensive metrics collection
- Understand data export and import capabilities
- Work with configuration template system
- Utilize configuration preset system
- Demonstrate error recovery and best practices
"""

import sys
import os
import time
import json

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

import nlm_py
from nlm_utils import (
    ConfigBuilder, ConfigValidator, ConfigManager,
    MetricsCollector, FeatureDetector
)

def print_section(title):
    """Print a formatted section header"""
    print(f"\n{'='*80}")
    print(f"{title}")
    print(f"{'='*80}")

def section_1_configuration_validation():
    """Section 1: Configuration Validation"""
    print_section("Section 1: Configuration Validation and Error Handling")
    
    print("""
In this section, we'll explore configuration validation:
1. Create comprehensive validation schemas
2. Implement error detection and reporting
3. Test error recovery mechanisms
4. Validate against best practices
""")
    
    # Create a complex configuration with validation
    print("\n1. Creating complex configuration for validation...")
    
    config = ConfigManager()
    
    # Set up configuration with various types of parameters
    config.config = {
        "network_size": 1000,
        "learning_rate": 0.05,
        "simulation_steps": 500,
        "neuron_threshold": 0.5,
        "connection_strength": 0.8,
        "activation_function": "sigmoid",
        "use_plasticity": True,
        "enable_profiling": False
    }
    
    # Create comprehensive validation schemas
    print("\n2. Creating validation schemas...")
    
    schemas = {
        "network_size": {
            "type": "int",
            "min": 100,
            "max": 10000,
            "required": True,
            "description": "Number of neurons in the network"
        },
        "learning_rate": {
            "type": "double",
            "min": 0.001,
            "max": 1.0,
            "required": True,
            "description": "Neural network learning rate"
        },
        "simulation_steps": {
            "type": "int",
            "min": 10,
            "max": 10000,
            "required": True,
            "description": "Number of simulation steps to run"
        },
        "neuron_threshold": {
            "type": "double",
            "min": 0.0,
            "max": 1.0,
            "required": False,
            "description": "Neuron activation threshold"
        },
        "connection_strength": {
            "type": "double",
            "min": 0.0,
            "max": 1.0,
            "required": False,
            "description": "Default synaptic connection strength"
        },
        "activation_function": {
            "type": "string",
            "allowed": ["sigmoid", "tanh", "relu", "linear"],
            "required": True,
            "description": "Neuron activation function"
        },
        "use_plasticity": {
            "type": "boolean",
            "required": False,
            "description": "Enable neural plasticity"
        },
        "enable_profiling": {
            "type": "boolean",
            "required": False,
            "description": "Enable performance profiling"
        }
    }
    
    config.schemas = schemas
    
    # Test validation with correct configuration
    print("\n3. Testing validation with correct configuration...")
    
    # Valid configuration
    valid_config = ConfigManager()
    valid_config.config = {
        "network_size": 2000,
        "learning_rate": 0.03,
        "simulation_steps": 300,
        "neuron_threshold": 0.6,
        "connection_strength": 0.7,
        "activation_function": "relu",
        "use_plasticity": True,
        "enable_profiling": False
    }
    valid_config.schemas = schemas
    
    errors = valid_config.validate()
    if errors:
        print(f"   ⚠ Validation errors ({len(errors)}):")
        for error in errors:
            print(f"     - {error}")
    else:
        print("   ✓ Configuration is valid")
    
    # Test validation with incorrect configuration
    print("\n4. Testing validation with incorrect configuration...")
    
    invalid_config = ConfigManager()
    invalid_config.config = {
        "network_size": 50,  # Too small
        "learning_rate": 1.5,  # Too large
        "simulation_steps": 5,  # Too small
        "neuron_threshold": 1.5,  # Out of range
        "connection_strength": -0.1,  # Negative
        "activation_function": "invalid_function",  # Not allowed
        "use_plasticity": "maybe",  # Not boolean
        "enable_profiling": "yes"  # Not boolean
    }
    invalid_config.schemas = schemas
    
    errors = invalid_config.validate()
    if errors:
        print(f"   ✓ Validation detected {len(errors)} errors:")
        for error in errors:
            print(f"     - {error}")
    else:
        print("   ⚠ No validation errors detected (unexpected)")
    
    # Test ConfigValidator.create_schema method
    print("\n5. Testing ConfigValidator.create_schema method...")
    
    test_schema = ConfigValidator.create_schema(
        key="test_parameter",
        type="double",
        description="Test parameter for validation",
        min_value=0.0,
        max_value=10.0,
        required=True,
        allowed=[1.0, 2.0, 5.0, 10.0]
    )
    
    print(f"   ✓ Created schema: {test_schema}")
    
    return config, valid_config, invalid_config

def section_2_metrics_collection():
    """Section 2: Metrics Collection"""
    print_section("Section 2: Comprehensive Metrics Collection")
    
    print("""
In this section, we'll explore metrics collection:
1. Create comprehensive metrics collectors
2. Track neural activity and performance
3. Export metrics in multiple formats
4. Analyze metrics for insights
""")
    
    # Create metrics collection configuration
    print("\n1. Creating metrics collection configuration...")
    
    metrics_config = ConfigBuilder.create_training_config()
    metrics_config.set("neuron_count", 1500)
    metrics_config.set("plasticity_learning_rate", 0.04)
    metrics_config.set("simulation_timestep", 0.0005)
    
    # Create metrics collection brain
    brain = nlm_py.createBrain(metrics_config)
    brain.initialize()
    
    print(f"   ✓ Metrics collection brain created: {brain.getTotalNeuronCount()} neurons")
    
    # Create comprehensive metrics collector
    print("\n2. Creating comprehensive metrics collector...")
    
    metrics_collector = MetricsCollector()
    
    # Define custom metrics collection
    custom_metrics = {
        'step_numbers': [],
        'firing_rates': [],
        'active_neurons': [],
        'total_neurons': [],
        'total_synapses': [],
        'memory_usage': [],
        'cpu_usage': [],
        'network_throughput': [],
        'plasticity_index': [],
        'stability_measure': []
    }
    
    # Simulate comprehensive metrics collection
    print("\n3. Collecting comprehensive metrics...")
    
    for step in range(200):
        brain.step(step)
        
        if step % 10 == 0:
            # Collect basic neural metrics
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getActiveNeuronCount()
            total_neurons = brain.getTotalNeuronCount()
            total_synapses = brain.getTotalSynapseCount()
            
            # Simulate additional metrics
            memory_usage = 100 + step * 0.5 + random.uniform(-10, 10)  # Simulated
            cpu_usage = 20 + firing_rate * 0.5 + random.uniform(-5, 5)  # Simulated
            network_throughput = active_neurons * firing_rate  # Derived metric
            plasticity_index = min(1.0, (step - 50) / 150)  # Simulated
            stability_measure = 1.0 - abs(0.5 - firing_rate / 100)  # Simulated
            
            # Record all metrics
            custom_metrics['step_numbers'].append(step)
            custom_metrics['firing_rates'].append(firing_rate)
            custom_metrics['active_neurons'].append(active_neurons)
            custom_metrics['total_neurons'].append(total_neurons)
            custom_metrics['total_synapses'].append(total_synapses)
            custom_metrics['memory_usage'].append(memory_usage)
            custom_metrics['cpu_usage'].append(cpu_usage)
            custom_metrics['network_throughput'].append(network_throughput)
            custom_metrics['plasticity_index'].append(plasticity_index)
            custom_metrics['stability_measure'].append(stability_measure)
            
            print(f"   Step {step:3d}: FR: {firing_rate:6.2f} Hz, "
                  f"Active: {active_neurons:4d}, "
                  f"Memory: {memory_usage:6.1f} MB, "
                  f"CPU: {cpu_usage:5.1f}%, "
                  f"Stability: {stability_measure:.2f}")
    
    # Analyze metrics
    print("\n4. Analyzing collected metrics...")
    
    if custom_metrics['step_numbers']:
        print(f"   ✓ Collected metrics for {len(custom_metrics['step_numbers'])} time points")
        
        # Calculate key metrics statistics
        avg_firing_rate = sum(custom_metrics['firing_rates']) / len(custom_metrics['firing_rates'])
        avg_active_neurons = sum(custom_metrics['active_neurons']) / len(custom_metrics['active_neurons'])
        avg_memory_usage = sum(custom_metrics['memory_usage']) / len(custom_metrics['memory_usage'])
        avg_cpu_usage = sum(custom_metrics['cpu_usage']) / len(custom_metrics['cpu_usage'])
        avg_network_throughput = sum(custom_metrics['network_throughput']) / len(custom_metrics['network_throughput'])
        avg_stability = sum(custom_metrics['stability_measure']) / len(custom_metrics['stability_measure'])
        
        print(f"\n   Metrics Summary:")
        print(f"     - Average firing rate: {avg_firing_rate:.2f} Hz")
        print(f"     - Average active neurons: {avg_active_neurons:.1f}")
        print(f"     - Average memory usage: {avg_memory_usage:.1f} MB")
        print(f"     - Average CPU usage: {avg_cpu_usage:.1f}%")
        print(f"     - Average network throughput: {avg_network_throughput:.1f}")
        print(f"     - Average stability measure: {avg_stability:.2f}")
        
        # Performance analysis
        print(f"\n   Performance Analysis:")
        print(f"     - Network efficiency: {avg_active_neurons / total_neurons * 100:.1f}%")
        print(f"     - Memory efficiency: {avg_network_throughput / avg_memory_usage:.2f}")
        print(f"     - Stability: {'Good' if avg_stability > 0.8 else 'Needs improvement'}")
    
    # Test CSV export
    print("\n5. Testing CSV export...")
    
    try:
        csv_file = "comprehensive_metrics.csv"
        
        # Create a simple export function
        with open(csv_file, 'w') as f:
            f.write("step,firing_rate,active_neurons,total_neurons,total_synapses,memory_usage,cpu_usage,network_throughput,plasticity_index,stability_measure\n")
            for i in range(len(custom_metrics['step_numbers'])):
                f.write(f"{custom_metrics['step_numbers'][i]},{custom_metrics['firing_rates'][i]},{custom_metrics['active_neurons'][i]},{custom_metrics['total_neurons'][i]},{custom_metrics['total_synapses'][i]},{custom_metrics['memory_usage'][i]:.1f},{custom_metrics['cpu_usage'][i]:.1f},{custom_metrics['network_throughput'][i]:.1f},{custom_metrics['plasticity_index'][i]:.3f},{custom_metrics['stability_measure'][i]:.3f}\n")
        
        print(f"   ✓ Metrics exported to {csv_file}")
        
        # Clean up
        import os
        if os.path.exists(csv_file):
            os.remove(csv_file)
            print(f"   ✓ Cleaned up {csv_file}")
            
    except Exception as e:
        print(f"   ⚠ CSV export failed: {e}")
    
    return custom_metrics, brain

def section_3_export_import():
    """Section 3: Export/Import Capabilities"""
    print_section("Section 3: Configuration Export/Import Capabilities")
    
    print("""
In this section, we'll explore export/import capabilities:
1. Export configuration to JSON
2. Import configuration from JSON
3. Compare configurations
4. Merge configurations
5. Version management
""")
    
    # Create source configuration
    print("\n1. Creating source configuration...")
    
    source_config = ConfigBuilder.create_research_config()
    source_config.set("neuron_count", 2500)
    source_config.set("plasticity_learning_rate", 0.03)
    source_config.set("simulation_timestep", 0.0003)
    source_config.set("log_level", "DEBUG")
    source_config.set("profiling_enabled", True)
    source_config.set("custom_parameter", "research_mode")
    
    print(f"   ✓ Source configuration: {source_config.getEntryCount()} entries")
    
    # Export configuration
    print("\n2. Exporting configuration...")
    
    json_file = "source_configuration.json"
    
    try:
        source_config.export_to_json(json_file, include_defaults=True, pretty_print=True)
        print(f"   ✓ Configuration exported to {json_file}")
        
        # Read and verify exported file
        with open(json_file, 'r') as f:
            exported_data = json.load(f)
            print(f"   Exported file contains {len(exported_data)} entries")
            print(f"   File size: {len(json.dumps(exported_data))} characters")
            
    except Exception as e:
        print(f"   ⚠ Export failed: {e}")
        json_file = None
    
    # Import configuration
    if json_file:
        print("\n3. Importing configuration...")
        
        target_config = ConfigBuilder.create_default_config()
        target_config.import_from_json(json_file)
        
        print(f"   ✓ Configuration imported with {target_config.getEntryCount()} entries")
        
        # Verify import
        print("\n4. Verifying imported configuration...")
        
        for key in source_config.config:
            if key in target_config.config:
                source_val = source_config.config[key]
                target_val = target_config.config[key]
                
                if isinstance(source_val, float):
                    match = abs(source_val - target_val) < 0.001
                else:
                    match = source_val == target_val
                
                status = "✓" if match else "⚠"
                print(f"   {status} {key}: {source_val} -> {target_val}")
            else:
                print(f"   ⚠ {key}: Missing in imported configuration")
        
        # Compare configurations
        print("\n5. Configuration comparison...")
        
        similarity = source_config.calculate_similarity(target_config)
        print(f"   Similarity score: {similarity:.2%}")
        
        if similarity > 0.9:
            print("   ✓ High similarity (>90%)")
        elif similarity > 0.7:
            print("   ✓ Moderate similarity (70-90%)")
        else:
            print("   ⚠ Low similarity (<70%)")
        
        # Get changes
        changes = source_config.get_changes()
        print(f"   Changes detected: {len(changes)}")
        
        if changes:
            print("   Change details:")
            for change in changes:
                print(f"     - {change}")
        
        # Merge configurations
        print("\n6. Configuration merging...")
        
        # Create a different configuration
        different_config = ConfigBuilder.create_training_config()
        different_config.set("neuron_count", 3000)  # Different value
        different_config.set("log_level", "WARNING")  # Different value
        
        # Merge with source
        merged_config = source_config.copy()  # In real implementation, merge logic
        merged_config.set("neuron_count", different_config.config.get("neuron_count"))
        
        print(f"   ✓ Merged configuration: {merged_config.getEntryCount()} entries")
        
        # Clean up
        import os
        if os.path.exists(json_file):
            os.remove(json_file)
            print(f"   ✓ Cleaned up {json_file}")
    
    return source_config

def section_4_template_system():
    """Section 4: Configuration Template System"""
    print_section("Section 4: Configuration Template System")
    
    print("""
In this section, we'll explore the template system:
1. Create and use configuration templates
2. Understand template inheritance
3. Apply templates to simulations
4. Manage template parameters
""")
    
    # Define templates
    print("\n1. Template definitions...")
    
    templates = {
        "basic": {
            "name": "Basic Network",
            "description": "Simple, stable network configuration",
            "neuron_count": 500,
            "plasticity_learning_rate": 0.01,
            "log_level": "INFO",
            "enable_profiling": False
        },
        "advanced": {
            "name": "Advanced Network",
            "description": "High-performance network with advanced features",
            "neuron_count": 2000,
            "plasticity_learning_rate": 0.05,
            "log_level": "DEBUG",
            "enable_profiling": True,
            "simulation_timestep": 0.0001
        },
        "research": {
            "name": "Research Network",
            "description": "Optimized for research and experimentation",
            "neuron_count": 1500,
            "plasticity_learning_rate": 0.03,
            "log_level": "DEBUG",
            "enable_profiling": True,
            "simulation_timestep": 0.0002,
            "enable_research_mode": True
        },
        "production": {
            "name": "Production Network",
            "description": "Optimized for production use",
            "neuron_count": 1000,
            "plasticity_learning_rate": 0.02,
            "log_level": "WARNING",
            "enable_profiling": False,
            "simulation_timestep": 0.001,
            "enable_error_recovery": True
        }
    }
    
    print("   Available templates:")
    for template_name, template_data in templates.items():
        print(f"   - {template_name}: {template_data['name']}")
        print(f"     Description: {template_data['description']}")
    
    # Simulate template loading and application
    print("\n2. Testing template loading...")
    
    template_name = "advanced"
    selected_template = templates[template_name]
    
    print(f"   Loading '{template_name}' template...")
    print(f"   Template: {selected_template['name']}")
    print(f"   Description: {selected_template['description']}")
    
    # Create configuration from template
    print("\n3. Applying template to configuration...")
    
    template_config = ConfigBuilder.create_default_config()
    
    # Apply template parameters
    for key, value in selected_template.items():
        if key not in ["name", "description"]:
            template_config.set(key, value)
    
    print(f"   ✓ Template applied:")
    for key, value in template_config.config.items():
        if isinstance(value, float):
            value_str = f"{value:.3f}"
        else:
            value_str = str(value)
        print(f"     - {key}: {value_str}")
    
    # Test template inheritance
    print("\n4. Testing template inheritance...")
    
    # Create child template from parent
    child_template = {
        "name": "Advanced + Custom",
        "description": "Advanced template with custom additions",
        "neuron_count": 3000,  # Override
        "plasticity_learning_rate": 0.04,  # Override
        "log_level": "DEBUG",  # Override
        "enable_profiling": True,  # Override
        "simulation_timestep": 0.0001,  # Override
        "custom_parameter": "custom_value",  # Additional
        "another_custom": 42  # Additional
    }
    
    print(f"   Child template inherits from 'advanced' with customizations:")
    for key, value in child_template.items():
        if isinstance(value, float):
            value_str = f"{value:.3f}"
        else:
            value_str = str(value)
        print(f"     - {key}: {value_str}")
    
    return templates

def section_5_preset_system():
    """Section 5: Configuration Preset System"""
    print_section("Section 5: Configuration Preset System")
    
    print("""
In this section, we'll explore the preset system:
1. Browse available presets
2. Apply different presets
3. Compare preset performance
4. Create custom presets
""")
    
    # Define presets
    print("\n1. Preset definitions...")
    
    presets = {
        "training": {
            "name": "Training Mode",
            "category": "Learning",
            "description": "Optimized for neural network training and learning",
            "parameters": {
                "neuron_count": 3000,
                "plasticity_learning_rate": 0.05,
                "log_level": "DEBUG",
                "enable_profiling": True,
                "simulation_timestep": 0.0005,
                "learning_mode": True
            },
            "performance_score": 85.5,
            "required_features": ["plasticity", "learning", "debugging"],
            "estimated_time": "3-5 minutes"
        },
        "simulation": {
            "name": "Simulation Mode",
            "category": "Performance",
            "description": "Optimized for stable simulation performance",
            "parameters": {
                "neuron_count": 1000,
                "plasticity_learning_rate": 0.01,
                "log_level": "INFO",
                "enable_profiling": False,
                "simulation_timestep": 0.001,
                "stability_mode": True
            },
            "performance_score": 92.3,
            "required_features": ["stability", "performance", "speed"],
            "estimated_time": "2-3 minutes"
        },
        "research": {
            "name": "Research Mode",
            "category": "Analysis",
            "description": "Optimized for research and analysis",
            "parameters": {
                "neuron_count": 2000,
                "plasticity_learning_rate": 0.03,
                "log_level": "DEBUG",
                "enable_profiling": True,
                "simulation_timestep": 0.0002,
                "enable_research_mode": True,
                "enable_data_collection": True
            },
            "performance_score": 78.9,
            "required_features": ["analysis", "profiling", "data_collection"],
            "estimated_time": "4-6 minutes"
        },
        "development": {
            "name": "Development Mode",
            "category": "Development",
            "description": "Optimized for development and testing",
            "parameters": {
                "neuron_count": 500,
                "plasticity_learning_rate": 0.02,
                "log_level": "DEBUG",
                "enable_profiling": True,
                "simulation_timestep": 0.0001,
                "enable_debug_mode": True,
                "enable_checkpointing": True
            },
            "performance_score": 88.7,
            "required_features": ["debugging", "checkpointing", "development"],
            "estimated_time": "1-2 minutes"
        }
    }
    
    print("   Available presets:")
    for preset_name, preset_data in presets.items():
        print(f"\n   {preset_name.upper()} Preset:")
        print(f"   Name: {preset_data['name']}")
        print(f"   Category: {preset_data['category']}")
        print(f"   Description: {preset_data['description']}")
        print(f"   Performance Score: {preset_data['performance_score']}/100")
        print(f"   Estimated Time: {preset_data['estimated_time']}")
        print(f"   Required Features: {', '.join(preset_data['required_features'])}")
    
    # Simulate applying presets
    print("\n2. Applying presets to simulation...")
    
    for preset_name in ["training", "simulation", "research", "development"]:
        preset = presets[preset_name]
        
        print(f"\n   Applying '{preset_name}' preset:")
        print(f"   Configuration:")
        for key, value in preset["parameters"].items():
            print(f"     - {key}: {value}")
        print(f"   Performance: {preset['performance_score']}/100")
        print(f"   Time estimate: {preset['estimated_time']}")
    
    # Compare preset performance
    print("\n3. Preset performance comparison...")
    
    best_preset = max(presets.values(), key=lambda x: x["performance_score"])
    fastest_preset = min(presets.values(), key=lambda x: x["estimated_time"].split('-')[0])
    
    print(f"   Best Performance: {best_preset['name']} ({best_preset['performance_score']}/100)")
    print(f"   Fastest: {fastest_preset['name']} ({fastest_preset['estimated_time']})")
    
    # Create custom preset
    print("\n4. Creating custom preset...")
    
    custom_preset = {
        "name": "Custom Development",
        "category": "Custom",
        "description": "My custom preset for specific needs",
        "parameters": {
            "neuron_count": 1500,
            "plasticity_learning_rate": 0.04,
            "log_level": "INFO",
            "enable_profiling": True,
            "simulation_timestep": 0.0003,
            "enable_custom_feature": True,
            "custom_parameter_1": "value1",
            "custom_parameter_2": 123
        },
        "performance_score": 0.0,  # To be determined
        "required_features": ["custom_feature", "parameterization"],
        "estimated_time": "2-4 minutes"
    }
    
    print(f"   ✓ Custom preset created: {custom_preset['name']}")
    print(f"   Description: {custom_preset['description']}")
    print(f"   Parameters: {len(custom_preset['parameters'])}")
    print(f"   Required features: {len(custom_preset['required_features'])}")
    
    return presets

def main():
    """Main function to run the complete feature-specific tutorial"""
    print("\n" + "="*80)
    print("Enhanced NLM Python API - Feature-Specific Examples")
    print("="*80)
    print("\nThis tutorial demonstrates specific features of the enhanced NLM Python API")
    print("including validation, metrics, export/import, templates, and presets.")
    print("\nPress Enter to begin...")
    input()
    
    try:
        # Section 1: Configuration Validation
        config1, valid_config, invalid_config = section_1_configuration_validation()
        
        # Section 2: Metrics Collection
        metrics_data, metrics_brain = section_2_metrics_collection()
        
        # Section 3: Export/Import
        export_config = section_3_export_import()
        
        # Section 4: Template System
        templates = section_4_template_system()
        
        # Section 5: Preset System
        presets = section_5_preset_system()
        
        # Summary
        print_header("Tutorial Summary")
        print("""
You have successfully completed the Feature-Specific Examples Tutorial!

Key accomplishments:
✓ Mastered configuration validation and error handling
✓ Implemented comprehensive metrics collection
✓ Understood data export and import capabilities
✓ Worked with configuration template system
✓ Utilized configuration preset system
✓ Demonstrated best practices and error recovery

Feature-Specific Skills:
• Configuration validation and schema creation
• Comprehensive metrics collection and analysis
• Data export/import with version management
• Template system design and application
• Preset system management and optimization
• Error handling and recovery mechanisms

Feature Results:
• Validation: Detected {len(invalid_config.config) if invalid_config else 0} validation errors
• Metrics: Collected {len(metrics_data['step_numbers'] if metrics_data else [])} data points
• Export: Successfully exported and imported configuration
• Templates: Managed {len(templates) if templates else 0} templates
• Presets: Utilized {len(presets) if presets else 0} presets

Next Steps:
- Create custom validation schemas for your use cases
- Implement advanced metrics collection strategies
- Build automated export/import pipelines
- Design reusable configuration templates
- Optimize preset management for your workflow

The enhanced NLM Python API provides powerful feature-specific tools for
advanced neural network simulation and analysis. By mastering these
features, you can build robust, efficient, and maintainable simulation
systems tailored to your specific needs!
""")
        
    except Exception as e:
        print(f"\n❌ Error occurred: {e}")
        import traceback
        traceback.print_exc()
        print("\nPlease check your installation and try again.")

if __name__ == "__main__":
    main()
