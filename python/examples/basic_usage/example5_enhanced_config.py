# Enhanced Config Example
"""
Example demonstrating the EnhancedConfig interface with type-safe operations.
"""

import pynlm_enhanced as pynlm

def main():
    print("=== Enhanced Config Example ===")
    
    # Create config with enhanced interface
    config = pynlm.EnhancedConfig()
    
    # Type-safe setters
    config.set_value("brain.neuron_count", 1000)
    config.set_value("brain.synapse_density", 0.1)
    config.set_value("brain.v_thresh", -50.0)
    config.set_value("plasticity.stdp.enable", True)
    
    # Type-safe getters with defaults
    neuron_count = config.get_int("brain.neuron_count", 500)
    synapse_density = config.get_float("brain.synapse_density", 0.05)
    threshold = config.get_float("brain.v_thresh", -60.0)
    stdp_enabled = config.get_bool("plasticity.stdp.enable", False)
    
    print(f"Neuron count: {neuron_count}")
    print(f"Synapse density: {synapse_density}")
    print(f"Threshold: {threshold}")
    print(f"STDP enabled: {stdp_enabled}")
    
    # Required getters with error handling
    try:
        required_value = config.get_string_required("nonexistent.key")
        print(f"Unexpected: Found {required_value}")
    except pynlm.pynlm.Exception as e:
        print(f"Expected error: {e}")
    
    # Configuration validation
    config.validate("brain.neuron_count", "int")
    config.validate("brain.synapse_density", "float")
    print("Validation passed")
    
    # Merge and copy operations
    other_config = pynlm.EnhancedConfig()
    other_config.set_value("brain.learning_rate", 0.001)
    
    config.merge(other_config)
    print(f"After merge, keys: {config.get_keys()}")
    
    # Copy operation
    config_copy = config.copy()
    print(f"Copy successful: {config_copy.get_int('brain.neuron_count')}")

if __name__ == "__main__":
    main()
