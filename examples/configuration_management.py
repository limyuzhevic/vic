import pynlm
import json
import time

def main():
    """Configuration management example demonstrating various configuration approaches."""
    print("=== Configuration Management Example ===")
    
    # Example 1: Basic configuration using helper functions
    print("\n1. Creating configuration with helper functions:")
    config1 = pynlm.createDefaultConfig()
    config1.set("random_seed", 42, pynlm.ConfigSource.Default)
    config1.set("simulation_timestep", 0.001, pynlm.ConfigSource.Default)
    config1.set("neuron_count", 200, pynlm.ConfigSource.Default)
    
    print(f"   Config summary: {config1.summary()}")
    
    # Example 2: Configuration using value-based constructor
    print("\n2. Creating configuration with create_config_with_values:")
    values = {
        'random_seed': 123,
        'simulation_timestep': 0.002,
        'neuron_count': 500,
        'region_count': 3,
        'connection_probability': 0.2,
        'max_energy': 150.0,
        'enable_plasticity': True
    }
    
    config2 = pynlm.create_config_with_values(values)
    
    print(f"   Keys in config: {config2.getKeys()}")
    print(f"   Neuron count: {config2.get_int('neuron_count', 0)}")
    print(f"   Simulation timestep: {config2.get_float('simulation_timestep', 0.001)}")
    print(f"   Max energy: {config2.get_float('max_energy', 0.0)}")
    print(f"   Placity enabled: {config2.get_bool('enable_plasticity', False)}")
    
    # Example 3: Configuration validation
    print("\n3. Validating configurations:")
    
    # Valid configuration
    warnings1 = pynlm.validate_configuration(config2)
    print(f"   Config 2 warnings: {len(warnings1)}")
    for warning in warnings1:
        print(f"     - {warning}")
    
    # Incomplete configuration
    config3 = pynlm.createDefaultConfig()
    config3.set("random_seed", 999, pynlm.ConfigSource.Default)
    # Missing other required keys
    
    warnings2 = pynlm.validate_configuration(config3)
    print(f"   Config 3 warnings: {len(warnings2)}")
    for warning in warnings2:
        print(f"     - {warning}")
    
    # Example 4: Template-based configuration
    print("\n4. Testing configuration templates:")
    
    templates = ["simple", "dense", "sparse"]
    for template in templates:
        config4 = pynlm.create_brain_from_template(template)
        brain = config4  # create_brain_from_template returns a brain, not config
        print(f"   {template:6s} template: {brain.getTotalNeuronCount():4d} neurons, "
              f"{brain.getTotalSynapseCount():4d} synapses")
    
    # Example 5: Configuration summaries
    print("\n5. Configuration summaries:")
    
    summary1 = pynlm.generate_config_summary(config1)
    print("   Config 1 summary:")
    for line in summary1.split('\n'):
        if line.strip():
            print(f"     {line}")
    
    summary2 = pynlm.generate_config_summary(config2)
    print("   Config 2 summary:")
    for line in summary2.split('\n'):
        if line.strip():
            print(f"     {line}")
    
    # Example 6: Configuration utilities
    print("\n6. Configuration utility functions:")
    
    # Merge dictionaries
    dict1 = {'a': 1, 'b': 2}
    dict2 = {'b': 3, 'c': 4}
    dict1.update(dict2)  # Simple merge
    print(f"   Dictionary merge: {dict1}")
    
    # Safe division
    safe_result = pynlm.safe_divide(10, 2)
    print(f"   Safe division (10/2): {safe_result}")
    
    safe_zero = pynlm.safe_divide(10, 0, default_value=999)
    print(f"   Safe division (10/0 with default): {safe_zero}")
    
    # Progress tracking simulation
    print("\n7. Progress tracking simulation:")
    total_steps = 10
    progress_tracker = pynlm.progress_tracker("Configuration Processing", total_steps)
    
    for step in range(total_steps):
        progress_tracker(step)
        time.sleep(0.1)  # Simulate work
    
    print("\n=== Configuration Example Complete ===")

if __name__ == "__main__":
    main()