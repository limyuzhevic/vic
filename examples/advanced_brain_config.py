#!/usr/bin/env python3
"""
Advanced Brain Configuration Examples
This script demonstrates the new BrainConfigBuilder and BrainFactory features.
"""

import pynlm
import json
import os

def example_default_brain():
    """Create a default brain using the factory."""
    print("=== Example 1: Default Brain ===")
    brain = pynlm.createDefaultBrain("default")
    brain.initialize()
    
    print(f"Created default brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Regions: {brain.getRegionCount()}")
    print(f"Initial firing rate: {brain.getAverageFiringRate():.2f}")
    return brain

def example_minimal_brain():
    """Create a minimal brain for performance testing."""
    print("\n=== Example 2: Minimal Brain ===")
    brain = pynlm.createMinimalBrain()
    brain.initialize()
    
    print(f"Created minimal brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Regions: {brain.getRegionCount()}")
    return brain

def example_experimental_brain():
    """Create an experimental brain with enhanced features."""
    print("\n=== Example 3: Experimental Brain ===")
    brain = pynlm.createExperimentalBrain()
    brain.initialize()
    
    print(f"Created experimental brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Regions: {brain.getRegionCount()}")
    return brain

def example_research_brain():
    """Create a research-grade brain with maximum features."""
    print("\n=== Example 4: Research Brain ===")
    brain = pynlm.createResearchBrain()
    brain.initialize()
    
    print(f"Created research brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Regions: {brain.getRegionCount()}")
    return brain

def example_config_builder():
    """Demonstrate the BrainConfigBuilder for custom configurations."""
    print("\n=== Example 5: Custom Configuration Builder ===")
    
    # Create a custom configuration
    config = pynlm.BrainConfigBuilder() \
        .set_neuron_count(2000) \
        .set_region_count(8) \
        .set_connection_probability(0.15f) \
        .set_timestep(0.002) \
        .set_random_seed(12345) \
        .enable_neuromodulation(True) \
        .enable_development(True) \
        .enable_curiosity(True) \
        .enable_structural_plasticity(True) \
        .enable_memory(True) \
        .enable_prediction(True) \
        .enable_cognition(True) \
        .build()
    
    print(f"Created custom config: {config.summary()}")
    
    # Create brain with custom config
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"Created custom brain with {brain.getTotalNeuronCount()} neurons")
    return brain, config

def example_brain_from_file():
    """Create a brain from a configuration file."""
    print("\n=== Example 6: Brain from Configuration File ===")
    
    # Create a sample configuration file
    config_data = {
        "neuron_count": 500,
        "region_count": 2,
        "connection_probability": 0.05,
        "simulation_timestep": 0.001,
        "random_seed": 42,
        "neuromodulation_enabled": True,
        "development_enabled": True,
        "curiosity_enabled": False,
        "structural_plasticity_enabled": True,
        "memory_enabled": True,
        "prediction_enabled": False,
        "cognition_enabled": True
    }
    
    with open("sample_config.json", "w") as f:
        json.dump(config_data, f, indent=2)
    
    print("Created sample_config.json")
    
    # Create brain from file
    brain = pynlm.createBrainFromFile("sample_config.json")
    brain.initialize()
    
    print(f"Created brain from file: {brain.getTotalNeuronCount()} neurons")
    
    # Clean up
    os.remove("sample_config.json")
    return brain

def example_brain_with_callback():
    """Create a brain with custom initialization callback."""
    print("\n=== Example 7: Brain with Custom Initialization ===")
    
    def custom_init(brain):
        """Custom initialization callback."""
        print(f"  Initializing brain with {brain.getTotalNeuronCount()} neurons")
        # Enable specific features
        # (Note: In the actual implementation, you'd call enable methods here)
        print("  Custom initialization complete")
    
    brain = pynlm.createBrainWithCallback(custom_init)
    brain.initialize()
    
    print(f"Created brain with callback: {brain.getTotalNeuronCount()} neurons")
    return brain

def example_validation():
    """Demonstrate configuration validation."""
    print("\n=== Example 8: Configuration Validation ===")
    
    # Create a default brain
    brain = pynlm.createDefaultBrain("default")
    config = brain.getConfig()
    
    # Validate the configuration
    try:
        pynlm.validateConfig(config)
        print("✓ Configuration validation passed")
    except Exception as e:
        print(f"✗ Configuration validation failed: {e}")
    
    return brain

def example_brain_integration():
    """Example showing integration of all features."""
    print("\n=== Example 9: Complete Integration ===")
    
    # Create a complex brain using the builder
    config = pynlm.BrainConfigBuilder() \
        .set_neuron_count(3000) \
        .set_region_count(6) \
        .set_connection_probability(0.12f) \
        .set_timestep(0.001) \
        .set_random_seed(99999) \
        .enable_neuromodulation(True) \
        .enable_development(True) \
        .enable_curiosity(True) \
        .enable_structural_plasticity(True) \
        .enable_memory(True) \
        .enable_prediction(True) \
        .enable_cognition(True) \
        .build()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run a simulation
    print("Running simulation...")
    for step in range(100):
        brain.step(step)
        
        if step % 20 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"avg rate: {brain.getAverageFiringRate():.2f}")
    
    print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
    return brain

def main():
    """Run all examples."""
    print("NLM Advanced Configuration Examples")
    print("=" * 50)
    
    try:
        example_default_brain()
        example_minimal_brain()
        example_experimental_brain()
        example_research_brain()
        example_config_builder()
        example_brain_from_file()
        example_brain_with_callback()
        example_validation()
        example_brain_integration()
        
        print("\n" + "=" * 50)
        print("All examples completed successfully!")
        print("\nKey improvements:")
        print("1. Factory methods for common brain configurations")
        print("2. Configuration builder for custom setups")
        print("3. Better error handling and validation")
        print("4. Improved Python API with advanced features")
        
    except Exception as e:
        print(f"\nError running examples: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()
