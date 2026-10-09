#!/usr/bin/env python3
"""
Advanced NLM Configuration and Command Line Interface Example

This script demonstrates the new advanced features added to NLM,
including enhanced CLI capabilities, configuration management,
performance monitoring, and pattern analysis tools.

For more information, see the documentation or use the help options.
"""

import sys
import os
import json
import argparse
from typing import Dict, List, Optional

# Import NLM components
import pynlm

def example_basic_cli():
    """Example 1: Basic CLI operations"""
    print("=== Example 1: Basic CLI Operations ===")
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    
    # Set basic parameters
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("simulation.timestep", 0.01)
    
    # Generate a configuration template
    print("\n1. Generating configuration template...")
    if config.generateConfigTemplate("nlm_config_template.json"):
        print("✓ Configuration template saved to 'nlm_config_template.json'")
    else:
        print("✗ Failed to generate configuration template")
    
    # Validate configuration file
    print("\n2. Validating configuration file...")
    if config.validateConfigFile("nlm_config_template.json"):
        print("✓ Configuration file is valid")
    else:
        print("✗ Configuration file has issues")
    
    # Get configuration report
    print("\n3. Configuration validation report...")
    if config.validate():
        print("✓ Configuration is valid")
    else:
        print("✗ Configuration has issues")
    
    recommendations = config.getConfigurationRecommendations()
    if recommendations:
        print("\n4. Optimization recommendations:")
        for i, rec in enumerate(recommendations, 1):
            print(f"   {i}. {rec}")
    
    suggestions = config.getOptimizationSuggestions()
    if suggestions:
        print("\n5. Optimization suggestions:")
        for i, sug in enumerate(suggestions, 1):
            print(f"   {i}. {sug}")
    
    warnings = config.getValidationWarnings()
    if warnings:
        print("\n6. Validation warnings:")
        for i, warn in enumerate(warnings, 1):
            print(f"   {i}. {warn}")
    
    return config

def example_preset_management():
    """Example 2: Preset configuration management"""
    print("\n\n=== Example 2: Preset Configuration Management ===")
    
    config = pynlm.createDefaultConfig()
    
    # Create a performance-optimized preset
    performance_preset = {
        "brain.neuron_count": 1000,
        "brain.synapse_density": 0.05,
        "plasticity.stdp.enable": True,
        "plasticity.stdp.learning_rate": 0.001,
        "neuromod.curiosity.enable": True,
        "neuromod.curiosity.sensitivity": 0.8,
        "memory.working.capacity": 50,
        "memory.working.decay_rate": 0.02
    }
    
    print("\n1. Creating performance-optimized preset...")
    if config.savePreset("performance_optimized", performance_preset):
        print("✓ Performance preset created successfully")
    else:
        print("✗ Failed to create performance preset")
    
    # Create a realism-optimized preset
    realism_preset = {
        "brain.neuron_count": 5000,
        "brain.synapse_density": 0.2,
        "plasticity.stdp.enable": True,
        "plasticity.stdp.learning_rate": 0.002,
        "plasticity.hebbian.enable": True,
        "neuromod.curiosity.enable": True,
        "neuromod.curiosity.sensitivity": 1.0,
        "neuromod.novelty.enable": True,
        "memory.working.capacity": 200,
        "memory.working.decay_rate": 0.01,
        "memory.episodic.capacity": 1000,
        "prediction.enabled": True,
        "prediction.horizon": 10
    }
    
    print("\n2. Creating realism-optimized preset...")
    if config.savePreset("realism_optimized", realism_preset):
        print("✓ Realism preset created successfully")
    else:
        print("✗ Failed to create realism preset")
    
    # Load and compare presets
    print("\n3. Comparing presets...")
    perf_config = pynlm.createDefaultConfig()
    perf_config.set("brain.neuron_count", 1000)
    perf_config.set("brain.synapse_density", 0.05)
    
    realism_config = pynlm.createDefaultConfig()
    realism_config.set("brain.neuron_count", 5000)
    realism_config.set("brain.synapse_density", 0.2)
    
    if perf_config.compareWith(realism_config):
        print("⚠ Presets are similar (only neuron count and synapse density differ)")
    else:
        print("✓ Presets have significant differences")
    
    diff_report = perf_config.generateDiffReport(realism_config)
    print(f"\n4. Difference report:\n{diff_report}")
    
    return config

def example_pattern_analysis():
    """Example 3: Pattern analysis tools"""
    print("\n\n=== Example 3: Pattern Analysis Tools ===")
    
    print("\n1. Pattern analysis requires running a simulation first...")
    print("   For a complete example, see the detailed documentation.")
    
    # Create a simple example of pattern analysis concepts
    print("\n2. Pattern analysis example (simulated):")
    
    # Simulate pattern data
    neural_activity = {
        'timestep_0': [0.1, 0.2, 0.3, 0.4, 0.5],
        'timestep_1': [0.15, 0.25, 0.35, 0.45, 0.55],
        'timestep_2': [0.2, 0.3, 0.4, 0.5, 0.6],
    }
    
    print("   Sample neural activity patterns:")
    for timestep, activity in neural_activity.items():
        print(f"   {timestep}: {activity}")
    
    print("\n3. Pattern analysis would typically include:")
    print("   - Frequency domain analysis (FFT)")
    print("   - Correlation and covariance analysis")
    print("   - Pattern classification and clustering")
    print("   - Similarity measurements between patterns")
    print("   - Change detection over time")
    
    return True

def example_performance_monitoring():
    """Example 4: Performance monitoring"""
    print("\n\n=== Example 4: Performance Monitoring ===")
    
    print("\n1. Performance monitoring requires a running NLM simulation...")
    print("   During simulation, metrics like:")
    print("   - CPU and memory usage")
    print("   - Simulation step times")
    print("   - Spike processing rates")
    print("   - Synaptic event counts")
    print("   - Neural firing rates")
    print("   - Plasticity activity")
    
    print("\n2. Example monitoring report format:")
    print("   Performance Metrics Report")
    print("   ===========================")
    print("   - CPU Usage: 45.2%")
    print("   - Memory Usage: 1.2 GB (78%)")
    print("   - Simulation Time: 1000.5s")
    print("   - Step Time: 0.001s avg")
    print("   - Spikes Processed: 125,000")
    print("   - Synaptic Events: 8,750")
    print("   - Firing Rate: 12.5 Hz")
    print("   - Plasticity Rate: 0.85 events/step")
    
    return True

def example_batch_processing():
    """Example 5: Batch processing"""
    print("\n\n=== Example 5: Batch Processing ===")
    
    print("\n1. Batch processing allows running multiple simulations...")
    print("   with different configurations and collecting results.")
    
    # Example configuration sets for batch processing
    config_sets = [
        {
            "name": "minimal",
            "brain.neuron_count": 100,
            "brain.synapse_density": 0.05,
            "plasticity.stdp.enable": True
        },
        {
            "name": "balanced",
            "brain.neuron_count": 1000,
            "brain.synapse_density": 0.1,
            "plasticity.stdp.enable": True,
            "neuromod.curiosity.enable": True
        },
        {
            "name": "complex",
            "brain.neuron_count": 5000,
            "brain.synapse_density": 0.2,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True,
            "memory.working.capacity": 200
        }
    ]
    
    print("\n2. Example configuration sets for batch processing:")
    for i, config_set in enumerate(config_sets, 1):
        print(f"   {i}. {config_set['name']}:")
        print(f"      - Neurons: {config_set['brain.neuron_count']}")
        print(f"      - Synapse density: {config_set['brain.synapse_density']}")
        if 'plasticity.stdp.enable' in config_set:
            print(f"      - STDP: {'enabled' if config_set['plasticity.stdp.enable'] else 'disabled'}")
    
    print("\n3. Batch processing workflow:")
    print("   a) Define configuration sets")
    print("   b) Create output directory")
    print("   c) Run each configuration")
    print("   d) Collect and analyze results")
    print("   e) Generate comparative reports")
    
    return True

def example_cli_tools():
    """Example 6: CLI tools"""
    print("\n\n=== Example 6: CLI Tools ===")
    
    print("\n1. Advanced CLI tools provide command-line access to NLM...")
    
    # Example command-line arguments
    print("\n2. Example command-line usage:")
    print("   # Create configuration template")
    print("   nlm-cli --generate-template nlm_config.json")
    print()
    print("   # Validate configuration")
    print("   nlm-cli --validate-config nlm_config.json")
    print()
    print("   # Run batch processing")
    print("   nlm-cli --batch-process configs/ output/ --max-steps 1000")
    print()
    print("   # Create checkpoint")
    print("   nlm-cli --create-checkpoint brain.bin world.bin --metadata 'Training session 1'")
    print()
    print("   # Restore from checkpoint")
    print("   nlm-cli --restore-checkpoint session1.bin")
    print()
    print("   # List checkpoints")
    print("   nlm-cli --list-checkpoints")
    
    return True

def example_integration_example():
    """Example 7: Complete integration example"""
    print("\n\n=== Example 7: Complete Integration Example ===")
    
    print("\n1. Complete workflow example:")
    print("   a) Create and configure brain")
    print("   b) Generate configuration template")
    print("   c) Validate configuration")
    print("   d) Optimize based on suggestions")
    print("   e) Create preset configurations")
    print("   f) Run simulation with progress monitoring")
    print("   g) Analyze patterns and performance")
    
    print("\n2. Step-by-step example:")
    
    # Create default configuration
    print("\n   Step 1: Create default configuration")
    config = pynlm.createDefaultConfig()
    print("   ✓ Default configuration created")
    
    # Set up optimization parameters
    print("\n   Step 2: Configure for optimal performance")
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.curiosity.enable", True)
    print("   ✓ Configuration optimized for cognitive capabilities")
    
    # Generate template
    print("\n   Step 3: Generate configuration template")
    config.generateConfigTemplate("production_config.json")
    print("   ✓ Configuration template generated")
    
    # Validate
    print("\n   Step 4: Validate configuration")
    if config.validate():
        print("   ✓ Configuration is valid")
    else:
        print("   ✗ Configuration validation failed")
    
    # Get recommendations
    print("\n   Step 5: Get optimization recommendations")
    recommendations = config.getConfigurationRecommendations()
    if recommendations:
        print(f"   ✓ Found {len(recommendations)} recommendations")
        for rec in recommendations[:2]:  # Show first 2
            print(f"     - {rec}")
    
    # Create preset
    print("\n   Step 6: Create optimized preset")
    optimized_preset = {
        "brain.neuron_count": config.getOr("brain.neuron_count", 1000),
        "brain.synapse_density": config.getOr("brain.synapse_density", 0.1),
        "plasticity.stdp.enable": True,
        "neuromod.curiosity.enable": True
    }
    config.savePreset("optimized_for_cognition", optimized_preset)
    print("   ✓ Optimized preset created")
    
    print("\n3. Summary:")
    print("   The advanced NLM features provide a complete toolkit for:")
    print("   - Configuration management and optimization")
    print("   - Advanced CLI operations")
    print("   - Performance monitoring and analysis")
    print("   - Pattern analysis and classification")
    print("   - Batch processing and automation")
    print("   - Progress tracking and error recovery")
    
    return config

def main():
    """Main function to run all examples"""
    print("Advanced NLM Configuration and CLI Examples")
    print("=" * 50)
    
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Advanced NLM Configuration and CLI Examples")
    parser.add_argument("--example", type=int, choices=1,2,3,4,5,6,7, 
                       help="Run specific example (1-7)")
    parser.add_argument("--output", type=str, default="examples_output",
                       help="Output directory for examples")
    
    args = parser.parse_args()
    
    if args.example:
        # Run specific example
        examples = {
            1: example_basic_cli,
            2: example_preset_management,
            3: example_pattern_analysis,
            4: example_performance_monitoring,
            5: example_batch_processing,
            6: example_cli_tools,
            7: example_integration_example
        }
        examples[args.example]()
    else:
        # Run all examples
        print("\nRunning all examples...")
        example_basic_cli()
        example_preset_management()
        example_pattern_analysis()
        example_performance_monitoring()
        example_batch_processing()
        example_cli_tools()
        example_integration_example()
        
        print("\n" + "=" * 50)
        print("All examples completed successfully!")
        print(f"Output directory: {args.output}")
        print("\nFor more detailed information, see the documentation or")
        print("run individual examples with --example <number>")

if __name__ == "__main__":
    main()
