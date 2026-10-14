"""
NLM (Neural Learning Machine) - Configuration Examples

This example demonstrates various configuration patterns and best practices
for setting up neural networks in NLM.

Author: NLM Research Team
Version: 6.0.0
"""

import pynlm

def basic_configuration():
    """Example 1: Basic configuration for simple neural networks"""
    print("Example 1: Basic Configuration")
    print("-" * 30)
    
    # Create basic configuration
    config = pynlm.createDefaultConfig()
    
    # Modify with specific parameters
    config.set("brain.neuron_count", 500)
    config.set("brain.region_count", 2)
    config.set("brain.connection_probability", 0.1)
    config.set("brain.initial_weight_mean", 0.5)
    config.set("brain.initial_weight_std", 0.1)
    
    print(f"Created basic config with:")
    print(f"  - {config.get('brain.neuron_count')} neurons")
    print(f"  - {config.get('brain.region_count')} regions")
    print(f"  - Connection probability: {config.get('brain.connection_probability')}")
    
    return config

def advanced_configuration():
    """Example 2: Advanced configuration for research experiments"""
    print("\nExample 2: Advanced Configuration")
    print("-" * 35)
    
    # Use factory method
    config = pynlm.create_advanced_config()
    
    # Modify specific parameters
    config.set("brain.neuron_count", 3000)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("neuromod.dopamine.scale", 0.8)
    
    print(f"Created advanced config with:")
    print(f"  - {config.get('brain.neuron_count')} neurons")
    print(f"  - STDP learning rate: {config.get('plasticity.stdp.learning_rate')}")
    print(f"  - Dopamine scale: {config.get('neuromod.dopamine.scale')}")
    print(f"  - Synaptogenesis enabled: {config.get('plasticity.structural.enable')}")
    
    return config

def development_configuration():
    """Example 3: Development-focused configuration"""
    print("\nExample 3: Development Configuration")
    print("-" * 35)
    
    # Use development factory with custom parameters
    config = pynlm.create_development_config(
        plasticityRate=1.5,  # Enhanced plasticity
        noveltyRate=1.2     # Increased novelty detection
    )
    
    config.set("brain.neuron_count", 2500)
    config.set("development.initial_plasticity", 1.8)
    
    print(f"Created development config with:")
    print(f"  - {config.get('brain.neuron_count')} neurons")
    print(f"  - Initial plasticity: {config.get('development.initial_plasticity')}")
    print(f"  - Critical period end: {config.get('development.critical_period_end')}")
    
    return config

def custom_configuration():
    """Example 4: Custom configuration with specific use case"""
    print("\nExample 4: Custom Configuration")
    print("-" * 30)
    
    # Start with advanced config
    config = pynlm.create_advanced_config()
    
    # Customize for specific use case: memory-intensive task
    config.set("brain.neuron_count", 4000)
    config.set("memory.working_capacity", 2000)
    config.set("memory.episodic_max_episodes", 10000)
    
    # Optimize for learning stability
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    print(f"Created custom memory-optimized config:")
    print(f"  - {config.get('brain.neuron_count')} neurons")
    print(f"  - Working memory capacity: {config.get('memory.working_capacity')}")
    print(f"  - Episodic memory capacity: {config.get('memory.episodic_max_episodes')}")
    print(f"  - STDP learning rate: {config.get('plasticity.stdp.learning_rate')}")
    
    return config

def configuration_validation():
    """Example 5: Configuration validation and checking"""
    print("\nExample 5: Configuration Validation")
    print("-" * 35)
    
    config = pynlm.create_advanced_config()
    
    # Check if configuration has specific keys
    required_keys = [
        "brain.neuron_count",
        "brain.region_count",
        "brain.connection_probability",
        "plasticity.stdp.enable",
        "neuromod.curiosity.enable"
    ]
    
    print(f"Checking configuration completeness...")
    for key in required_keys:
        has_key = config.has(key)
        print(f"  {key}: {'✓' if has_key else '✗'}")
    
    # Get configuration summary
    print(f"\nConfiguration summary:")
    summary = config.summary()
    print(f"  Total entries: {summary.count('entries')}")  # Count "entries"
    
    # Get all keys
    keys = config.getKeys()
    print(f"  Configuration keys: {len(keys)}")
    for key in keys[:5]:  # Show first 5 keys
        print(f"    - {key}")
    
    return config

def configuration_comparison():
    """Example 6: Comparing different configurations"""
    print("\nExample 6: Configuration Comparison")
    print("-" * 40)
    
    configs = [
        ("Basic", pynlm.createDefaultConfig()),
        ("Advanced", pynlm.create_advanced_config()),
        ("Development", pynlm.create_development_config(1.2, 1.1))
    ]
    
    print("Configuration comparison:")
    print("-" * 30)
    print(f"{'Config':<15} {'Neurons':<10} {'Regions':<10} {'E/I Ratio':<12} {'STDP':<6} {'Curiosity':<8}")
    print(f"{'-'*70}")
    
    for name, config in configs:
        neuron_count = config.get("brain.neuron_count", "N/A")
        region_count = config.get("brain.region_count", "N/A")
        ei_ratio = config.get("plasticity.stdp.learning_rate", "N/A")
        stdp_enabled = config.get("plasticity.stdp.enable", "N/A")
        curiosity_enabled = config.get("neuromod.curiosity.enable", "N/A")
        
        print(f"{name:<15} {str(neuron_count):<10} {str(region_count):<10} {str(ei_ratio):<12} {str(stdp_enabled):<6} {str(curiosity_enabled):<8}")
    
    return configs

def file_io_examples():
    """Example 7: File I/O operations"""
    print("\nExample 7: File I/O Operations")
    print("-" * 35)
    
    # Create and save configuration
    config = pynlm.create_advanced_config()
    config.set("brain.neuron_count", 1000)
    
    filename = "advanced_config.json"
    saved = pynlm.save_config_to_file(config, filename)
    
    print(f"Saved configuration to: {filename}")
    print(f"  File size: ~{len(str(config))} bytes")
    
    # Load configuration
    loaded_config = pynlm.load_config_from_file(filename)
    
    print(f"Loaded configuration from: {filename}")
    print(f"  Neuron count: {loaded_config.get('brain.neuron_count')}")
    
    # Compare original and loaded
    original_neuron_count = config.get("brain.neuron_count")
    loaded_neuron_count = loaded_config.get("brain.neuron_count")
    
    print(f"\nConfiguration integrity check:")
    print(f"  Original neuron count: {original_neuron_count}")
    print(f"  Loaded neuron count: {loaded_neuron_count}")
    print(f"  Match: {'✓' if original_neuron_count == loaded_neuron_count else '✗'}")
    
    return config, loaded_config

def main():
    print("NLM Configuration Examples")
    print("=" * 60)
    print("This example demonstrates various configuration patterns")
    print("and best practices for NLM.")
    
    # Run all examples
    examples = [
        ("Basic Configuration", basic_configuration),
        ("Advanced Configuration", advanced_configuration),
        ("Development Configuration", development_configuration),
        ("Custom Configuration", custom_configuration),
        ("Configuration Validation", configuration_validation),
        ("Configuration Comparison", configuration_comparison),
        ("File I/O Operations", file_io_examples)
    ]
    
    results = []
    
    for name, example_func in examples:
        try:
            result = example_func()
            results.append((name, "SUCCESS", result))
        except Exception as e:
            results.append((name, "ERROR", str(e)))
    
    # Summary
    print(f"\nConfiguration Examples Summary")
    print("=" * 50)
    
    for name, status, result in results:
        print(f"{name:<30} {status:<10}")
        if status == "SUCCESS" and isinstance(result, tuple):
            # For file I/O, show more details
            if name == "File I/O Operations":
                original_config, loaded_config = result
                print(f"    Original config: {original_config.get('brain.neuron_count')} neurons")
                print(f"    Loaded config: {loaded_config.get('brain.neuron_count')} neurons")
    
    print(f"\n✅ All configuration examples completed successfully!")
    print("\nKey features demonstrated:")
    print("  • Basic configuration setup")
    print("  • Advanced configuration factory methods")
    print("  • Development-focused configuration")
    print("  • Custom configuration for specific use cases")
    print("  • Configuration validation and checking")
    print("  • Configuration comparison")
    print("  • File I/O operations")
    print("  • Configuration integrity verification")

if __name__ == "__main__":
    main()
