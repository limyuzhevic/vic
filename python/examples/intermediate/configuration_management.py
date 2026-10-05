#!/usr/bin/env python3
"""
Intermediate Tutorial: Advanced Configuration and Features

This tutorial demonstrates intermediate-level features of the enhanced NLM Python API,
building on the basics covered in the getting started tutorial.

## Learning Objectives
- Master advanced configuration management
- Work with neural plasticity and learning
- Use configuration templates and presets
- Implement advanced metrics collection
- Export and import configurations
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
    print(f"\n{'='*70}")
    print(f"{title}")
    print(f"{'='*70}")

def section_1_advanced_configuration():
    """Section 1: Advanced Configuration Management"""
    print_section("Section 1: Advanced Configuration Management")
    
    print("""
In this section, we'll explore advanced configuration features:
1. Creating custom configurations with schemas
2. Using configuration templates
3. Applying configuration presets
4. Validating configurations
5. Optimizing configurations
""")
    
    # Create a custom configuration with validation
    print("\n1. Creating custom configuration with schemas...")
    
    # Create a new configuration manager
    config = ConfigBuilder.create_default_config()
    
    # Add schemas for validation
    print("\n2. Adding validation schemas...")
    
    # Neuron count schema
    neuron_schema = ConfigValidator.create_schema(
        key="neuron_count",
        type="int",
        description="Number of neurons in the network",
        min_value=100,
        max_value=10000,
        required=True
    )
    config.add_schema(neuron_schema)
    
    # Learning rate schema
    learning_schema = ConfigValidator.create_schema(
        key="plasticity_learning_rate",
        type="double",
        description="Neural plasticity learning rate",
        min_value=0.001,
        max_value=1.0,
        required=False
    )
    config.add_schema(learning_schema)
    
    # Simulation timestep schema
    timestep_schema = ConfigValidator.create_schema(
        key="simulation_timestep",
        type="double",
        description="Simulation time step",
        min_value=0.0001,
        max_value=0.1,
        required=False
    )
    config.add_schema(timestep_schema)
    
    # Set custom configuration values
    print("\n3. Setting custom configuration values...")
    config.set("neuron_count", 2500)
    config.set("plasticity_learning_rate", 0.05)
    config.set("simulation_timestep", 0.001)
    config.set("log_level", "DEBUG")
    
    # Validate the configuration
    print("\n4. Validating configuration...")
    errors = config.validate()
    if errors:
        print(f"   ⚠ Validation errors ({len(errors)}):")
        for error in errors:
            print(f"     - {error}")
    else:
        print("   ✓ Configuration is valid")
    
    # Display configuration
    print("\n5. Final configuration:")
    for key, value in config.config.items():
        if key in config.schemas:
            schema = config.schemas[key]
            required = " (required)" if schema.get("required", False) else " (optional)"
            print(f"   - {key}: {value} {required}")
    
    return config

def section_2_plasticity_learning():
    """Section 2: Neural Plasticity and Learning"""
    print_section("Section 2: Neural Plasticity and Learning")
    
    print("""
In this section, we'll explore neural plasticity and learning:
1. Create a brain with plasticity enabled
2. Run a learning simulation
3. Collect learning metrics
4. Analyze learning progress
""")
    
    # Create a configuration optimized for learning
    print("\n1. Creating learning-optimized configuration...")
    config = ConfigBuilder.create_training_config()
    
    # Create brain with this configuration
    print("\n2. Creating brain with plasticity...")
    brain = nlm_py.createBrain(config)
    brain.initialize()
    
    print(f"   ✓ Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"   ✓ Brain has {brain.getTotalSynapseCount()} synapses")
    
    # Create metrics collector for learning
    print("\n3. Setting up learning metrics collection...")
    metrics_collector = MetricsCollector()
    
    # Run learning simulation
    print("\n4. Running learning simulation...")
    print("   Simulating neural plasticity and learning...")
    
    learning_rates = []
    firing_rates = []
    
    for step in range(200):
        brain.step(step)
        
        if step % 20 == 0:
            # Collect metrics
            metrics_collector.collect_metrics(brain, step)
            
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getActiveNeuronCount()
            
            firing_rates.append(firing_rate)
            learning_rates.append(config.config.get("plasticity_learning_rate", 0.05))
            
            print(f"   Step {step:3d}: Firing rate: {firing_rate:6.2f} Hz, "
                  f"Active neurons: {active_neurons:4d}")
    
    # Analyze learning progress
    print("\n5. Analyzing learning progress...")
    if firing_rates:
        avg_initial_firing = sum(firing_rates[:5]) / 5  # First 5 steps
        avg_final_firing = sum(firing_rates[-5:]) / 5   # Last 5 steps
        
        print(f"   Initial average firing rate: {avg_initial_firing:.2f} Hz")
        print(f"   Final average firing rate: {avg_final_firing:.2f} Hz")
        print(f"   Change: {avg_final_firing - avg_initial_firing:+.2f} Hz")
        
        if avg_final_firing > avg_initial_firing:
            print("   ✓ Learning detected: Network activity increased")
        else:
            print("   ⚠ No significant learning detected")
    
    return metrics_collector, brain

def section_3_template_system():
    """Section 3: Configuration Template System"""
    print_section("Section 3: Configuration Template System")
    
    print("""
In this section, we'll explore the configuration template system:
1. Load and use configuration templates
2. Create custom templates
3. Understand template inheritance
4. Apply templates to simulations
""")
    
    # Start with a base configuration
    print("\n1. Creating base configuration...")
    base_config = ConfigBuilder.create_default_config()
    
    # Try to load a template
    print("\n2. Testing template system...")
    
    # Note: In a real implementation, we would have actual templates
    # For this demo, we'll simulate template loading
    templates = {
        "basic": {
            "description": "Basic network configuration",
            "neuron_count": 500,
            "plasticity_learning_rate": 0.01
        },
        "advanced": {
            "description": "Advanced network with high plasticity",
            "neuron_count": 5000,
            "plasticity_learning_rate": 0.05,
            "log_level": "DEBUG"
        },
        "research": {
            "description": "Research-grade configuration",
            "neuron_count": 2000,
            "plasticity_learning_rate": 0.03,
            "profiling_enabled": True
        }
    }
    
    print("\n3. Available templates:")
    for template_name, template_data in templates.items():
        print(f"   - {template_name}: {template_data['description']}")
    
    # Simulate template loading and application
    print("\n4. Applying 'advanced' template...")
    applied_config = ConfigBuilder.create_default_config()
    for key, value in templates["advanced"].items():
        if key != "description":
            applied_config.set(key, value)
    
    print(f"   ✓ Applied advanced template:")
    for key, value in applied_config.config.items():
        if key != "random_seed":  # Skip this as it's added by ConfigBuilder
            print(f"     - {key}: {value}")
    
    return applied_config

def section_4_preset_system():
    """Section 4: Configuration Preset System"""
    print_section("Section 4: Configuration Preset System")
    
    print("""
In this section, we'll explore the configuration preset system:
1. Browse available presets
2. Apply different presets
3. Compare preset performance
4. Create custom presets
""")
    
    # Create a base configuration
    print("\n1. Creating base configuration...")
    config = ConfigBuilder.create_default_config()
    
    # Define presets
    presets = {
        "training": {
            "name": "Training Mode",
            "category": "Learning",
            "description": "Optimized for neural network training and learning",
            "parameters": {
                "neuron_count": 3000,
                "plasticity_learning_rate": 0.05,
                "log_level": "DEBUG"
            },
            "performance_score": 85.5,
            "required_features": ["plasticity", "learning"]
        },
        "simulation": {
            "name": "Simulation Mode",
            "category": "Performance",
            "description": "Optimized for stable simulation performance",
            "parameters": {
                "neuron_count": 1000,
                "plasticity_learning_rate": 0.01,
                "log_level": "INFO"
            },
            "performance_score": 92.3,
            "required_features": ["stability", "performance"]
        },
        "research": {
            "name": "Research Mode",
            "category": "Analysis",
            "description": "Optimized for research and analysis",
            "parameters": {
                "neuron_count": 2000,
                "plasticity_learning_rate": 0.03,
                "log_level": "DEBUG",
                "profiling_enabled": True
            },
            "performance_score": 78.9,
            "required_features": ["analysis", "profiling"]
        }
    }
    
    print("\n2. Available presets:")
    for preset_name, preset_data in presets.items():
        print(f"\n   {preset_name.upper()} Preset:")
        print(f"   Name: {preset_data['name']}")
        print(f"   Category: {preset_data['category']}")
        print(f"   Description: {preset_data['description']}")
        print(f"   Performance Score: {preset_data['performance_score']}/100")
    
    # Simulate applying presets
    print("\n3. Applying presets to simulation...")
    
    for preset_name in ["training", "simulation", "research"]:
        preset = presets[preset_name]
        print(f"\n   Applying '{preset_name}' preset:")
        for key, value in preset["parameters"].items():
            print(f"     {key}: {value}")
        print(f"     Performance: {preset['performance_score']}/100")
    
    return presets

def section_5_configuration_export_import():
    """Section 5: Configuration Export/Import"""
    print_section("Section 5: Configuration Export/Import")
    
    print("""
In this section, we'll explore configuration export and import:
1. Export configuration to JSON
2. Import configuration from JSON
3. Compare configurations
4. Merge configurations
""")
    
    # Create base configuration
    print("\n1. Creating base configuration...")
    config1 = ConfigBuilder.create_default_config()
    config1.set("neuron_count", 1500)
    config1.set("plasticity_learning_rate", 0.04)
    config1.set("simulation_timestep", 0.0005)
    config1.set("log_level", "INFO")
    
    print(f"   Base config: {config1.getEntryCount()} entries")
    
    # Export configuration
    print("\n2. Exporting configuration...")
    json_file = "example_config.json"
    
    try:
        config1.export_to_json(json_file, include_defaults=True, pretty_print=True)
        print(f"   ✓ Configuration exported to {json_file}")
        
        # Read and display the exported file
        with open(json_file, 'r') as f:
            exported_data = json.load(f)
            print(f"   File contains {len(exported_data)} entries")
    except Exception as e:
        print(f"   ⚠ Export failed: {e}")
        json_file = None
    
    # Import configuration
    if json_file:
        print("\n3. Importing configuration...")
        config2 = ConfigBuilder.create_default_config()
        config2.import_from_json(json_file)
        print(f"   ✓ Configuration imported with {config2.getEntryCount()} entries")
        
        # Compare configurations
        print("\n4. Comparing configurations...")
        similarity = config1.calculate_similarity(config2)
        print(f"   Similarity: {similarity:.2%}")
        
        changes = config1.get_changes()
        print(f"   Changes found: {len(changes)}")
    
    # Clean up
    if json_file and os.path.exists(json_file):
        os.remove(json_file)
        print(f"   ✓ Cleaned up {json_file}")
    
    return config1

def section_6_performance_optimization():
    """Section 6: Performance Optimization"""
    print_section("Section 6: Performance Optimization")
    
    print("""
In this section, we'll explore performance optimization techniques:
1. Create optimized configurations
2. Run performance benchmarks
3. Analyze performance metrics
4. Implement optimization strategies
""")
    
    # Create multiple configurations for comparison
    print("\n1. Creating optimized configurations...")
    
    # Default configuration
    default_config = ConfigBuilder.create_default_config()
    default_config.set("neuron_count", 1000)
    default_config.set("plasticity_learning_rate", 0.01)
    
    # Optimized configuration
    optimized_config = ConfigBuilder.create_training_config()
    optimized_config.set("neuron_count", 2000)
    optimized_config.set("plasticity_learning_rate", 0.03)
    optimized_config.set("log_level", "WARNING")  # Reduce logging overhead
    
    # Research configuration
    research_config = ConfigBuilder.create_research_config()
    research_config.set("neuron_count", 1500)
    research_config.set("plasticity_learning_rate", 0.02)
    
    print("\n2. Configuration comparison:")
    print(f"   Default: {default_config.getEntryCount()} entries, {default_config.config.get('neuron_count')} neurons")
    print(f"   Optimized: {optimized_config.getEntryCount()} entries, {optimized_config.config.get('neuron_count')} neurons")
    print(f"   Research: {research_config.getEntryCount()} entries, {research_config.config.get('neuron_count')} neurons")
    
    # Create brains and run simulations
    print("\n3. Performance benchmarking...")
    
    configs = [default_config, optimized_config, research_config]
    results = []
    
    for i, config in enumerate(configs, 1):
        brain = nlm_py.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        for step in range(100):
            brain.step(step)
        end_time = time.time()
        
        elapsed_time = end_time - start_time
        fps = 100 / elapsed_time
        
        results.append({
            'config': f'Config {i}',
            'neurons': config.config.get('neuron_count'),
            'time': elapsed_time,
            'fps': fps
        })
        
        print(f"   Config {i}: {elapsed_time:.3f}s, {fps:.1f} FPS, {config.config.get('neuron_count')} neurons")
    
    # Analyze results
    print("\n4. Performance analysis:")
    
    fastest = min(results, key=lambda x: x['time'])
    print(f"   Fastest configuration: {fastest['config']} ({fastest['time']:.3f}s)")
    
    if results[1]['fps'] > results[0]['fps'] and results[1]['fps'] > results[2]['fps']:
        print("   ✓ Optimized configuration shows best performance!")
    else:
        print("   ⚠ Performance varies by configuration type")
    
    return results

def main():
    """Main function to run the complete intermediate tutorial"""
    print("\n" + "="*70)
    print("Enhanced NLM Python API - Intermediate Tutorial")
    print("="*70)
    print("\nThis tutorial demonstrates intermediate-level features including")
    print("advanced configuration management, neural plasticity, and optimization.")
    print("\nPress Enter to begin...")
    input()
    
    try:
        # Section 1: Advanced Configuration
        config = section_1_advanced_configuration()
        
        # Section 2: Plasticity Learning
        metrics_collector, brain = section_2_plasticity_learning()
        
        # Section 3: Template System
        template_config = section_3_template_system()
        
        # Section 4: Preset System
        presets = section_4_preset_system()
        
        # Section 5: Export/Import
        export_config = section_5_configuration_export_import()
        
        # Section 6: Performance Optimization
        performance_results = section_6_performance_optimization()
        
        # Summary
        print_section("Tutorial Summary")
        print("""
You have successfully completed the Intermediate Tutorial!

Key accomplishments:
✓ Mastered advanced configuration management with schemas
✓ Explored neural plasticity and learning mechanisms
✓ Understood configuration template and preset systems
✓ Practiced configuration export/import operations
✓ Analyzed and optimized performance

Intermediate Skills Developed:
• Configuration validation and error handling
• Custom configuration creation
• Performance benchmarking
• Metrics collection and analysis
• Feature detection and compatibility checking

Next Steps:
- Explore Advanced Tutorials for complex scenarios
- Study Real-World Examples for practical applications
- Review Performance Examples for optimization techniques
- Use Reference Examples for best practices

The enhanced NLM Python API provides powerful tools for neural network simulation
and analysis. Continue exploring to master all its capabilities!
""")
        
    except Exception as e:
        print(f"\n❌ Error occurred: {e}")
        import traceback
        traceback.print_exc()
        print("\nPlease check your installation and try again.")

if __name__ == "__main__":
    main()
