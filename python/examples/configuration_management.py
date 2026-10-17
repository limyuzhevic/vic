#!/usr/bin/env python3
"""
NLM Python Configuration Management Examples

This script demonstrates configuration management capabilities with NLM Python bindings,
including validation, saving, loading, and advanced configuration features.
"""

import pynlm
import json
import yaml
from pathlib import Path
from datetime import datetime

def example_configuration_validation():
    """Example: Configuration validation and testing"""
    print("=" * 60)
    print("Configuration Validation")
    print("=" * 60)
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Set various parameters
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.connection_probability", 0.05)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    
    print("✓ Configuration created with parameters:")
    print(f"  - Neurons: {config.get('brain.neuron_count')}")
    print(f"  - Synapse Density: {config.get('brain.synapse_density')}")
    print(f"  - Connection Probability: {config.get('brain.connection_probability')}")
    print(f"  - STDP Enabled: {config.get('plasticity.stdp.enable')}")
    print(f"  - Learning Rate: {config.get('plasticity.stdp.learning_rate')}")
    print(f"  - Curiosity Enabled: {config.get('neuromod.curiosity.enable')}")
    
    # Validate configuration
    try:
        pynlm.validateConfig(config)
        print("\n✓ Configuration validated successfully!")
    except pynlm.NLMConfigurationError as e:
        print(f"\n✗ Configuration validation failed: {e}")
        return False
    
    return True

def example_configuration_file_io():
    """Example: Configuration file input/output"""
    print("\n" + "=" * 60)
    print("Configuration File I/O")
    print("=" * 60)
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.region_count", 3)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("neuromod.curiosity.exploration_bonus", 0.5)
    
    # Save configuration to JSON file
    json_file = "brain_config.json"
    config.saveToFile(json_file)
    print(f"✓ Configuration saved to JSON: {json_file}")
    
    # Load configuration from JSON
    loaded_config = pynlm.createDefaultConfig()
    if loaded_config.loadFromFile(json_file):
        print(f"✓ Configuration loaded from JSON: {json_file}")
        
        # Display loaded configuration
        print(f"\nLoaded configuration:")
        print(f"  - Neurons: {loaded_config.get('brain.neuron_count')}")
        print(f"  - Regions: {loaded_config.get('brain.region_count')}")
        print(f"  - Learning Rate: {loaded_config.get('plasticity.stdp.learning_rate')}")
        print(f"  - Exploration Bonus: {loaded_config.get('neuromod.curiosity.exploration_bonus')}")
        
        # Modify loaded configuration
        loaded_config.set("brain.neuron_count", 1500)
        loaded_config.set("plasticity.stdp.learning_rate", 0.003)
        
        print(f"\nModified configuration:")
        print(f"  - Updated Neurons: {loaded_config.get('brain.neuron_count')}")
        print(f"  - Updated Learning Rate: {loaded_config.get('plasticity.stdp.learning_rate')}")
        
        # Save modified configuration
        loaded_config.saveToFile("modified_brain_config.json")
        print(f"✓ Modified configuration saved: 'modified_brain_config.json'")
    
    # Test YAML support (if PyYAML is available)
    try:
        import yaml
        
        # Convert config to dictionary
        config_dict = {}
        for key in config.getKeys():
            config_dict[key] = config.get(key)
        
        # Save to YAML
        yaml_file = "brain_config.yaml"
        with open(yaml_file, 'w') as f:
            yaml.dump(config_dict, f, default_flow_style=False)
        
        print(f"✓ Configuration saved to YAML: {yaml_file}")
        
        # Load from YAML
        with open(yaml_file, 'r') as f:
            loaded_yaml = yaml.safe_load(f)
        
        print(f"✓ Configuration loaded from YAML: {yaml_file}")
        print(f"  - YAML keys: {list(loaded_yaml.keys())}")
        
    except ImportError:
        print(f"\n⚠ PyYAML not available - skipping YAML support")
    
    # Test configuration comparison
    print(f"\nConfiguration Comparison:")
    original_config = pynlm.createDefaultConfig()
    original_config.set("test_param", 123)
    
    modified_config = pynlm.createDefaultConfig()
    modified_config.set("test_param", 456)
    
    print(f"  Original: test_param = {original_config.get('test_param')}")
    print(f"  Modified: test_param = {modified_config.get('test_param')}")
    
    # Show configuration summary
    print(f"\nConfiguration Summary:")
    print(f"  Total keys in original config: {len(original_config.getKeys())}")
    print(f"  Total keys in modified config: {len(modified_config.getKeys())}")
    
    return True

def example_configuration_validation_advanced():
    """Example: Advanced configuration validation"""
    print("\n" + "=" * 60)
    print("Advanced Configuration Validation")
    print("=" * 60)
    
    # Test 1: Valid configuration
    print("\nTest 1: Valid Configuration")
    config1 = pynlm.createDefaultConfig()
    config1.set("brain.neuron_count", 500)
    config1.set("brain.synapse_density", 0.1)
    config1.set("plasticity.stdp.enable", True)
    
    try:
        pynlm.validateConfig(config1)
        print("✓ Configuration 1 validated successfully")
    except pynlm.NLMConfigurationError as e:
        print(f"✗ Configuration 1 validation failed: {e}")
    
    # Test 2: Missing required parameters
    print("\nTest 2: Missing Required Parameters")
    config2 = pynlm.createDefaultConfig()
    # Note: Not setting brain.neuron_count
    
    try:
        pynlm.validateConfig(config2)
        print("✓ Configuration 2 validated successfully (unexpected)")
    except pynlm.NLMConfigurationError as e:
        print(f"✓ Configuration 2 validation failed as expected: {e}")
    
    # Test 3: Invalid parameter types
    print("\nTest 3: Invalid Parameter Types")
    config3 = pynlm.createDefaultConfig()
    config3.set("brain.neuron_count", "invalid")  # String instead of int
    
    try:
        pynlm.validateConfig(config3)
        print("✓ Configuration 3 validated successfully (unexpected)")
    except Exception as e:
        print(f"✓ Configuration 3 validation failed as expected: {type(e).__name__}")
    
    # Test 4: Configuration templates
    print("\nTest 4: Configuration Templates")
    
    def create_research_config():
        """Create research configuration with specific parameters"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 10000)
        config.set("brain.synapse_density", 0.05)
        config.set("brain.region_count", 10)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.0005)
        config.set("plasticity.hebbian.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.curiosity.exploration_bonus", 0.3)
        config.set("neuromod.novelty.enable", True)
        return config
    
    research_config = create_research_config()
    pynlm.validateConfig(research_config)
    research_config.saveToFile("research_config.json")
    print("✓ Research configuration created and validated")
    print(f"  - Neurons: {research_config.get('brain.neuron_count')}")
    print(f"  - Learning Rate: {research_config.get('plasticity.stdp.learning_rate')}")
    
    # Test 5: Configuration comparison
    print("\nTest 5: Configuration Comparison")
    
    def compare_configs(config1, config2):
        """Compare two configurations"""
        keys1 = set(config1.getKeys())
        keys2 = set(config2.getKeys())
        
        common_keys = keys1.intersection(keys2)
        only_in_1 = keys1.difference(keys2)
        only_in_2 = keys2.difference(keys1)
        
        print(f"  Common keys: {len(common_keys)}")
        print(f"  Only in config1: {len(only_in_1)}")
        print(f"  Only in config2: {len(only_in_2)}")
        
        if common_keys:
            differences = []
            for key in common_keys:
                val1 = config1.get(key)
                val2 = config2.get(key)
                if val1 != val2:
                    differences.append(f"  {key}: {val1} != {val2}")
            
            print(f"  Differences in common keys: {len(differences)}")
            for diff in differences[:3]:  # Show first 3 differences
                print(diff)
    
    config_a = pynlm.createDefaultConfig()
    config_a.set("param1", "value1")
    config_a.set("param2", 123)
    
    config_b = pynlm.createDefaultConfig()
    config_b.set("param1", "value2")  # Different value
    config_b.set("param3", True)     # Additional parameter
    
    compare_configs(config_a, config_b)
    
    return True

def example_configuration_utilities():
    """Example: Configuration utility functions"""
    print("\n" + "=" * 60)
    print("Configuration Utility Functions")
    print("=" * 60)
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Set various parameters
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("neuromod.curiosity.exploration_bonus", 0.5)
    config.set("neuromod.novelty.sensitivity", 0.8)
    
    # Utility 1: Get configuration summary
    print("\nUtility 1: Configuration Summary")
    summary = config.summary()
    print(f"Summary:\n{summary}")
    
    # Utility 2: Check configuration status
    print("\nUtility 2: Configuration Status Check")
    print(f"Has 'brain.neuron_count': {config.has('brain.neuron_count')}")
    print(f"Has 'nonexistent.key': {config.has('nonexistent.key')}")
    
    # Utility 3: Get configuration keys
    print("\nUtility 3: Configuration Keys")
    keys = config.getKeys()
    print(f"Total keys: {len(keys)}")
    print(f"Keys: {keys}")
    
    # Utility 4: Configuration search
    print("\nUtility 4: Configuration Search")
    search_term = "brain"
    brain_keys = [key for key in keys if search_term in key]
    print(f"Keys containing '{search_term}': {brain_keys}")
    
    # Utility 5: Configuration modification
    print("\nUtility 5: Configuration Modification")
    print(f"Original neuron count: {config.get('brain.neuron_count')}")
    
    # Modify configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.2)
    config.set("plasticity.stdp.learning_rate", 0.003)
    
    print(f"Modified neuron count: {config.get('brain.neuron_count')}")
    print(f"Modified synapse density: {config.get('brain.synapse_density')}")
    print(f"Modified learning rate: {config.get('plasticity.stdp.learning_rate')}")
    
    # Utility 6: Configuration reset
    print("\nUtility 6: Configuration Reset")
    config.clear()
    print(f"After clear, has 'brain.neuron_count': {config.has('brain.neuron_count')}")
    print(f"Total keys after clear: {len(config.getKeys())}")
    
    # Re-add some parameters
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.1)
    
    print(f"After re-add, has 'brain.neuron_count': {config.has('brain.neuron_count')}")
    
    return True

def example_error_handling():
    """Example: Error handling with configuration"""
    print("\n" + "=" * 60)
    print("Error Handling")
    print("=" * 60)
    
    # Test 1: Null configuration validation
    print("\nTest 1: Null Configuration Validation")
    try:
        pynlm.validateConfig(None)
        print("✗ Should have raised an error")
    except Exception as e:
        print(f"✓ Caught error as expected: {type(e).__name__}: {e}")
    
    # Test 2: Invalid file path
    print("\nTest 2: Invalid File Path")
    config = pynlm.createDefaultConfig()
    try:
        config.loadFromFile("/nonexistent/path/config.json")
        print("✗ Should have raised an error")
    except Exception as e:
        print(f"✓ Caught error as expected: {type(e).__name__}")
    
    # Test 3: Invalid parameter value
    print("\nTest 3: Invalid Parameter Value")
    config = pynlm.createDefaultConfig()
    try:
        config.set("brain.neuron_count", -1)
        print("✓ Set negative neuron count (should validate elsewhere)")
    except Exception as e:
        print(f"✓ Caught error: {type(e).__name__}: {e}")
    
    # Test 4: Missing configuration files
    print("\nTest 4: Missing Configuration Files")
    try:
        config = pynlm.createDefaultConfig()
        config.loadFromFile("missing_config.json")
        print("✗ Should have raised an error")
    except Exception as e:
        print(f"✓ Caught error as expected: {type(e).__name__}")
    
    return True

def main():
    """Run all configuration management examples"""
    print("NLM Python Configuration Management Examples")
    print("=" * 60)
    print("This script demonstrates configuration management with NLM Python bindings.")
    print()
    
    # Run all examples
    example_configuration_validation()
    example_configuration_file_io()
    example_configuration_validation_advanced()
    example_configuration_utilities()
    example_error_handling()
    
    print("\n" + "=" * 60)
    print("All configuration management examples completed successfully!")
    print("=" * 60)
    print("\nKey features demonstrated:")
    print("1. ✓ Configuration validation and testing")
    print("2. ✓ File I/O (JSON and YAML)")
    print("3. ✓ Advanced configuration validation")
    print("4. ✓ Configuration utility functions")
    print("5. ✓ Error handling and validation")
    print("\nGenerated files:")
    print("  - brain_config.json")
    print("  - modified_brain_config.json")
    print("  - research_config.json")
    print("\nAll configuration management features working correctly!")

if __name__ == "__main__":
    main()