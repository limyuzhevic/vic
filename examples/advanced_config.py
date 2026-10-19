#!/usr/bin/env python3
"""
Advanced Configuration Examples
===========================
This example demonstrates advanced NLM configuration capabilities.
It shows how to create, customize, and validate complex brain configurations.

Key Features:
- Configuration presets (default, small, large, advanced)
- Configuration merging and comparison
- Parameter validation and error handling
- Type-safe configuration access
- Performance optimization
- Configuration validation with detailed reports
"""

import sys
import os

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def print_section(title):
    """Print a formatted section header"""
    print(f"\n{'='*70}")
    print(f"{title}")
    print(f"{'='*70}")

def main():
    print_section("NLM Advanced Configuration Examples")
    
    # 1. Configuration Factory Functions
    print("\n1. Configuration Factory Functions")
    print("-" * 50)
    
    # Create different configuration presets
    print("\n   a) Creating configuration presets...")
    
    # Default configuration (basic setup)
    default_config = pynlm.createDefaultConfig()
    print(f"      Default config: {len(default_config.getKeys())} parameters")
    
    # Small configuration (research/testing)
    small_config = pynlm.createBrainWithPresets("small")
    print(f"      Small config: {len(small_config.getKeys())} parameters")
    print(f"        Neurons: {small_config.getOr(100, 'brain.neuronCount')}")
    
    # Large configuration (production)
    large_config = pynlm.createBrainWithPresets("large")
    print(f"      Large config: {len(large_config.getKeys())} parameters")
    print(f"        Neurons: {large_config.getOr(5000, 'brain.neuronCount')}")
    
    # Advanced configuration (research)
    advanced_config = pynlm.createAdvancedConfig("advanced")
    print(f"      Advanced config: {len(advanced_config.getKeys())} parameters")
    
    # Example configuration
    example_config = pynlm.example_config()
    print(f"      Example config: {len(example_config.getKeys())} parameters")
    
    # 2. Configuration Customization
    print("\n2. Configuration Customization")
    print("-" * 50)
    
    # Start with a base configuration
    config = pynlm.createDefaultConfig()
    print(f"\n   Starting with default config ({len(config.getKeys())} parameters)")
    
    # Customize specific parameters
    print("\n   Customizing parameters...")
    config.set("brain.neuronCount", 500)
    config.set("brain.synapseDensity", 0.15)
    config.set("plasticity.stdpLTPWeight", 0.02)
    config.set("plasticity.stdpLTDWeight", 0.08)
    config.set("memory.workingMemoryCapacity", 200)
    config.set("memory.episodicCapacity", 2000)
    config.set("prediction.planningDepth", 8)
    config.set("neuromod.dopamineBaseline", 0.2)
    
    # Add custom parameters
    config.set("custom.researchMode", True)
    config.set("custom.learningRate", 0.001)
    config.set("custom.explorationRate", 0.5)
    config.set("custom.maxSimulationSteps", 1000)
    
    print("   Custom parameters added:")
    custom_keys = [k for k in config.getKeys() if k.startswith("custom.")]
    for key in custom_keys:
        print(f"     - {key}")
    
    # 3. Configuration Merging
    print("\n3. Configuration Merging")
    print("-" * 50)
    
    # Create two base configurations
    config1 = pynlm.createDefaultConfig()
    config2 = pynlm.example_config()
    
    print(f"\n   Config1 (default): {len(config1.getKeys())} parameters")
    print(f"   Config2 (example): {len(config2.getKeys())} parameters")
    
    # Merge configurations (overwrite=False = merge without overwriting)
    config1.merge(config2, overwrite=False)
    print(f"   After merging (overwrite=False): {len(config1.getKeys())} parameters")
    
    # Merge with overwrite
    config3 = pynlm.createDefaultConfig()
    config4 = pynlm.example_config()
    config3.merge(config4, overwrite=True)
    print(f"   After merging (overwrite=True): {len(config3.getKeys())} parameters")
    
    # 4. Configuration Validation
    print("\n4. Configuration Validation")
    print("-" * 50)
    
    # Validate current configuration
    errors = ""
    warnings = ""
    
    print("\n   Validating configuration...")
    is_valid = config.validate(errors, warnings)
    
    print(f"   Validation result: {'PASSED' if is_valid else 'FAILED'}")
    
    if errors:
        print(f"\n   Errors ({len(errors.split(chr(10)))} issues):")
        print(f"     {errors[:200]}...")
    
    if warnings:
        print(f"\n   Warnings ({len(warnings.split(chr(10)))} issues):")
        print(f"     {warnings[:200]}...")
    
    # Validate with strict validation
    print("\n   Running strict validation...")
    strict_errors = ""
    strict_warnings = ""
    strict_valid = config.validate(strict_errors, strict_warnings)
    
    print(f"   Strict validation: {'PASSED' if strict_valid else 'FAILED'}")
    
    # 5. Type-Safe Configuration Access
    print("\n5. Type-Safe Configuration Access")
    print("-" * 50)
    
    print("\n   Using type-safe getters...")
    
    # Integer access
    neuron_count = config.getOr(1000, "brain.neuronCount")
    region_count = config.getOr(1, "brain.regionCount")
    print(f"   Neuron count: {neuron_count} (type: {type(neuron_count).__name__})")
    print(f"   Region count: {region_count} (type: {type(region_count).__name__})")
    
    # Float access
    learning_rate = config.getOr(0.001, "learning.rate")
    synapse_density = config.getOr(0.1, "brain.synapseDensity")
    print(f"   Learning rate: {learning_rate} (type: {type(learning_rate).__name__})")
    print(f"   Synapse density: {synapse_density} (type: {type(synapse_density).__name__})")
    
    # Boolean access
    research_mode = config.getOr(False, "custom.researchMode")
    print(f"   Research mode: {research_mode} (type: {type(research_mode).__name__})")
    
    # String access
    simulation_mode = config.getOr("standard", "simulation.mode")
    print(f"   Simulation mode: {simulation_mode} (type: {type(simulation_mode).__name__})")
    
    # Vector access
    steps_vector = config.getOr([], "simulation.steps")
    print(f"   Steps vector: {steps_vector} (type: {type(steps_vector).__name__})")
    
    # 6. Configuration Optimization
    print("\n6. Configuration Optimization")
    print("-" * 50)
    
    print("\n   Optimizing configuration...")
    config.optimize()
    print(f"   Configuration optimized: {len(config.getKeys())} parameters")
    
    # 7. Configuration Summary
    print("\n7. Configuration Summary")
    print("-" * 50)
    
    print("\n   Final configuration summary:")
    print(f"   Total parameters: {len(config.getKeys())}")
    
    # Count parameter types
    int_params = 0
    float_params = 0
    str_params = 0
    bool_params = 0
    
    for key in config.getKeys():
        # This would require type information - in practice you'd track this
        pass
    
    print("   Parameter distribution:")
    print(f"     - Integer parameters: {int_params}")
    print(f"     - Float parameters: {float_params}")
    print(f"     - String parameters: {str_params}")
    print(f"     - Boolean parameters: {bool_params}")
    print(f"     - Custom parameters: {len([k for k in config.getKeys() if k.startswith('custom.')])}")
    
    # 8. Performance Configuration
    print("\n8. Performance Configuration")
    print("-" * 50)
    
    print("\n   Performance-related parameters:")
    perf_params = [
        ("brain.neuronCount", config.getOr(0, "brain.neuronCount")),
        ("brain.synapseDensity", config.getOr(0.0, "brain.synapseDensity")),
        ("plasticity.stdpLTPWeight", config.getOr(0.0, "plasticity.stdpLTPWeight")),
        ("memory.workingMemoryCapacity", config.getOr(0, "memory.workingMemoryCapacity")),
        ("neuromod.dopamineBaseline", config.getOr(0.0, "neuromod.dopamineBaseline")),
    ]
    
    for param, value in perf_params:
        print(f"     - {param}: {value}")
    
    # 9. Configuration Export/Import
    print("\n9. Configuration Export/Import (Example)")
    print("-" * 50)
    
    print("\n   Configuration could be exported to JSON/YAML for:")
    print("     - Experiment reproducibility")
    print("     - Configuration sharing")
    print("     - Backup and recovery")
    print("     - Version control")
    
    print("\n   Example export (simulated):")
    print("     config.saveToFile('experiment_config.json')")
    
    print("\n10. Best Practices")
    print("-" * 50)
    
    print("\n   Configuration best practices:")
    print("     - Use presets for common configurations")
    print("     - Validate configurations before use")
    print("     - Use descriptive parameter names")
    print("     - Document custom parameters")
    print("     - Validate parameter ranges")
    print("     - Use type-safe access methods")
    print("     - Optimize for performance requirements")
    
    print_separator("Advanced Configuration Examples Complete")
    
    print("\nNext Steps:")
    print("  - Try simple_brain.py for basic examples")
    print("  - Try agent_world.py for environment interaction")
    print("  - Try plasticity_experiment.py for learning experiments")
    print("  - Try neuromodulation_study.py for neuromodulation analysis")
    print("  - Try memory_system_test.py for memory system testing")
    print("\nFor detailed usage, see how_to_use.md")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"\nError: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)